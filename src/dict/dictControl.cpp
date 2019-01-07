#include <vector>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"

void ShowStack(Stack& inStack,const char *inStackName);

void InitDict_Control(bool inOnlyUseCore) {
	Install(new Word("begin-control-block",WORD_FUNC {
		if(inContext.BeginControlBlock()==false) {
			inContext.Error(E_SYSTEM_ERROR);
			exit(-1);
		}
		NEXT;
	}));

	Install(new Word("end-control-block",WORD_FUNC {
		if(inContext.EndControlBlock()==false) { return false; }
		NEXT;
	}));

	Install(new Word("compile",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		switch(tos.dataType) {
			case kTypeString:
				if(inContext.Compile(*tos.stringPtr)==false) {
					return inContext.Error(E_CAN_NOT_FIND_THE_WORD,
										   tos.stringPtr.get()->c_str());
				}
				break;
			case kTypeAddress:
				inContext.Compile(tos);
				break;
			default:
				return inContext.Error_InvalidType(E_TOS_STRING,tos);
		}
		NEXT;
	}));

	Install(new Word("new-slot",WORD_FUNC {
		inContext.MarkAndCreateEmptySlot();
		NEXT;
	}));

	Install(new Word("check-compile-mode",WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		NEXT;
	}));

	Install(new Word("get-next-thread-address",WORD_FUNC {
		inContext.RS.emplace_back(kTypeAddress,
								  inContext.GetNextThreadAddressOnCompile());
		NEXT;
	}));

	Install(new Word("compile-address-into-empty-slot",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue tvAddress=Pop(inContext.RS);
		if(tvAddress.dataType!=kTypeAddress) {
			return inContext.Error_InvalidType(E_RS_TOS_ADDRESS,tvAddress);
		}
		int p=inContext.GetEmptySlotPosition();
		if(p<0) { return inContext.Error(E_RS_BROKEN); }
		inContext.newWord->tmpParam->at(p)=tvAddress;
		NEXT;
	}));

	Install(new Word("mark-address",WORD_FUNC {
		inContext.MarkHere();
		NEXT;
	}));

	Install(new Word("begin-loop-block",WORD_FUNC {
		inContext.EnterLeavableLoop();
		NEXT;
	}));

	Install(new Word("end-loop-block",WORD_FUNC {
		inContext.ExitLeavableLoop();
		NEXT;
	}));

	Install(new Word("begin-switch-block",WORD_FUNC {
		inContext.EnterSwitchBlock();
		NEXT;
	}));

	Install(new Word("end-switch-block",WORD_FUNC {
		inContext.ExitSwitchBlock();
		NEXT;
	}));

	Install(new Word("branch-if-false",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeBool) {
			return inContext.Error_InvalidType(E_TOS_BOOL,tos);
		}
		if( tos.boolValue ) {
			inContext.ip+=2;	// Next
		} else {
			inContext.ip=(const Word**)(*(inContext.ip+1));
		}
		return true;
	}));

	Install(new Word("branch-if-true",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeBool) {
			return inContext.Error_InvalidType(E_TOS_BOOL,tos);
		}

		if( tos.boolValue ) {
			inContext.ip=(const Word**)(*(inContext.ip+1));
		} else {
			inContext.ip+=2;	// Next
		}
		return true;
	}));

	Install(new Word("absolute-jump",WORD_FUNC {
		inContext.ip=(const Word**)(*(inContext.ip+1));
		return true;
	}));

	if(inOnlyUseCore==false) {
		// ex:
		//	1) cond if true-part then
		//	2) cond if true-part else false-part then		
		// "if" : begin-control-block "branch-if-false" compile new-slot ; immediate
		Install(new Word("if",WordLevel::Immediate,WORD_FUNC {
			inContext.BeginControlBlock();
			inContext.Compile(std::string("branch-if-false"));
			inContext.MarkAndCreateEmptySlot();
			NEXT;
		}));

		/*---
			"else" :
				check-compile-mode
				get-next-thread-address r> to-int 2 + to-address >r
				compile-address-into-empty-slot
				"absolute-jump" compile
				new-slot
			; immediate	
		---*/
		Install(new Word("else",WordLevel::Immediate,WORD_FUNC {
			if(inContext.CheckCompileMode()==false) {
				return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
			}
			int p=inContext.GetEmptySlotPosition();
			if(p<0) { return false; }
			inContext.newWord->tmpParam->at(p)
				=TypedValue(kTypeAddress,
							inContext.GetNextThreadAddressOnCompile()+2);
			inContext.Compile(std::string("absolute-jump"));
			inContext.MarkAndCreateEmptySlot();
			NEXT;
		}));

		/*-----
		  	"then" : check-compile-mode
		 			 get-next-thread-address compile-address-into-empty-slot
		 			 end-control-block
				   ; immediate
		-----*/
		Install(new Word("then",WordLevel::Immediate,WORD_FUNC {
			if(inContext.CheckCompileMode()==false) {
				return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
			}
			int p=inContext.GetEmptySlotPosition();
			if(p<0) { return inContext.Error(E_RS_BROKEN); }
			inContext.newWord->tmpParam->at(p)
				=TypedValue(kTypeAddress,
							inContext.GetNextThreadAddressOnCompile());
			if(inContext.EndControlBlock()==false) {
				return false;
			}
			NEXT;
		}));
	}

	// equivalent: @r> +second/r> <
	Install(new Word("repeat?",WORD_FUNC {
		const size_t n=inContext.RS.size();
		if(n<2) { return inContext.Error(E_RS_BROKEN); } 

		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=kTypeInt
		  && rsTos.dataType!=kTypeLong
		  && rsTos.dataType!=kTypeBigInt) {
			return inContext.Error_InvalidType(E_RS_TOS_INT_OR_LONG_OR_BIGINT,rsTos);
		}

		TypedValue& rsSecond=inContext.RS[n-2];
		if(rsSecond.dataType!=kTypeInt
		  && rsSecond.dataType!=kTypeLong
		  && rsSecond.dataType!=kTypeBigInt) {
			return inContext.Error_InvalidType(E_RS_SECOND_INT_OR_LONG_OR_BIGINT,
											   rsSecond);
		}

		if(rsTos.dataType==kTypeInt && rsSecond.dataType==kTypeInt) {
			inContext.DS.emplace_back(rsTos.intValue<=rsSecond.intValue);
		} else if(rsTos.dataType==kTypeLong && rsSecond.dataType==kTypeLong) {
			inContext.DS.emplace_back(rsTos.longValue<=rsSecond.longValue);
		} else if(rsTos.dataType==kTypeBigInt && rsSecond.dataType==kTypeBigInt) {
			inContext.DS.emplace_back(*rsTos.bigIntPtr<=*rsSecond.bigIntPtr);
		} else {
			switch(rsTos.dataType) {
				case kTypeInt:
					if(rsSecond.dataType==kTypeLong) {
						rsTos.dataType=kTypeLong;
						rsTos.longValue=(long)rsTos.intValue;
						inContext.DS.emplace_back(rsTos.longValue<=rsSecond.longValue);
					} else {
						assert(rsSecond.dataType==kTypeBigInt);
						rsTos.dataType=kTypeBigInt;
						rsTos.bigIntPtr=new BigInt(rsTos.intValue);
						inContext.DS.emplace_back(
							*rsTos.bigIntPtr<=*rsSecond.bigIntPtr);
					}
					break;
				case kTypeLong:
					if(rsSecond.dataType==kTypeInt) {
						rsSecond.dataType=kTypeLong;
						rsSecond.longValue=(long)rsSecond.intValue;
						inContext.DS.emplace_back(rsTos.longValue<=rsSecond.longValue);
					} else {
						assert(rsSecond.dataType==kTypeBigInt);
						rsTos.dataType=kTypeBigInt;
						rsTos.bigIntPtr=new BigInt(rsTos.longValue);
						inContext.DS.emplace_back(
							*rsTos.bigIntPtr<=*rsSecond.bigIntPtr);
					}
					break;
				case kTypeBigInt:
					if(rsSecond.dataType==kTypeInt) {
						rsSecond.dataType=kTypeBigInt;
						rsSecond.bigIntPtr=new BigInt(rsSecond.intValue);
						inContext.DS.emplace_back(
							*rsTos.bigIntPtr<=*rsSecond.bigIntPtr);
					} else {
						assert(rsSecond.dataType==kTypeLong);
						rsSecond.dataType=kTypeBigInt;
						rsSecond.bigIntPtr=new BigInt(rsSecond.longValue);
						inContext.DS.emplace_back(
							*rsTos.bigIntPtr<=*rsSecond.bigIntPtr);
					}
					break;
				default:
					return inContext.Error(E_SYSTEM_ERROR);
			}
		}
		NEXT;
	}));

	// equivalent: r> 1+ >r
	Install(new Word("inc-loop-counter",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }

		TypedValue& tvCounter=ReadTOS(inContext.RS);
		if(tvCounter.dataType!=kTypeInt
		  && tvCounter.dataType!=kTypeLong
		  && tvCounter.dataType!=kTypeBigInt) {
			return inContext.Error_InvalidType(E_RS_TOS_INT_OR_LONG_OR_BIGINT,
											   tvCounter);
		}
		switch(tvCounter.dataType) {
			case kTypeInt:		tvCounter.intValue++;		break;
			case kTypeLong:		tvCounter.longValue++;		break;
			case kTypeBigInt:	(*tvCounter.bigIntPtr)++;	break;
			default:
				inContext.Error(E_SYSTEM_ERROR);
		}
		NEXT;
	}));

	// equivalent: drop-rs drop-rs
	Install(new Word("loop-epilogue",WORD_FUNC {
		if(inContext.RS.size()<2) { return inContext.Error(E_RS_BROKEN); }
		inContext.RS.pop_back();
		inContext.RS.pop_back();
		NEXT;
	}));

	if(inOnlyUseCore==false) {
		// start end do - loop
		// ex) 1 10 do i . loop
		// Even if the value of end is less than the start,
		// the 'do-loop' block will be executed at least once.
		/*---
			"do" : begin-control-block
				">r" compile
				">r" compile
				mark-address
				begin-loop-block
			; immediate
		---*/
		Install(new Word("do",WordLevel::Immediate,WORD_FUNC {
			inContext.BeginControlBlock();
			inContext.Compile(std::string(">r"));	// target value
			inContext.Compile(std::string(">r"));	// current value
			inContext.MarkHere();	// come back here.
			
			inContext.EnterLeavableLoop();
	
			NEXT;
		}));

		/*---
			"loop" :
				check-compile-mode
				r> to-address
				"inc-loop-counter" compile
				"repeat?" compile
				"branch-if-true" compile
				compile
				end-loop-block
				"loop-epilogue" compile
				end-control-block
			; immediate
		---*/
		Install(new Word("loop",WordLevel::Immediate,WORD_FUNC {
			if(inContext.CheckCompileMode()==false) {
				return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
			}
			const int p=inContext.GetMarkPosition();
			if(p<0) { return false; }
			TypedValue tvAddrLoopTop(kTypeAddress,p);

			inContext.Compile(std::string("inc-loop-counter"));
			inContext.Compile(std::string("repeat?"));
			inContext.Compile(std::string("branch-if-true"));
			inContext.Compile(tvAddrLoopTop);

			inContext.ExitLeavableLoop();

			inContext.Compile(std::string("loop-epilogue"));

			if(inContext.EndControlBlock()==false) { return false; }
			NEXT;
		}));
	}

	Install(new Word("leave",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		inContext.Compile_Leave();
		NEXT;
	}));

	Install(new Word("i",Dict["@r>"]->code));

	if(inOnlyUseCore==false) {
		// for-next loop
		// start end for - next
		// start end for - stepValue step next
		// If the value of end is less than the value of start,
		// the "for-next" block is never executed.
		/*---
			"for" :
				begin-control-block
				">r" compile
				">r" compile
				mark-address
				"repeat?" compile
				"branch-if-false" compile
				new-slot
				begin-loop-blok
			; immediate
		---*/
		Install(new Word("for",WordLevel::Immediate,WORD_FUNC {
			inContext.BeginControlBlock();
			inContext.Compile(std::string(">r"));	// target value
			inContext.Compile(std::string(">r"));	// current value
			inContext.MarkHere();	// come back here.

			inContext.Compile(std::string("repeat?"));
			inContext.Compile(std::string("branch-if-false"));
			inContext.MarkAndCreateEmptySlot();	// jump to loop-epilogue

			inContext.EnterLeavableLoop();
			NEXT;
		}));

		/*---
			"next" :
				check-compile-mode
				"inc-loop-counter" compile
				"absolute-jump" compile
				r> compile
				get-next-thread-address
				compile-address-into-empty-slot
				end-loop-block
				"loop-epilogue" compile
				end-control-block
			; immediate
		---*/
		Install(new Word("next",WordLevel::Immediate,WORD_FUNC {
			if(inContext.CheckCompileMode()==false) {
				return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
			}

			const int slotPosToEpilogue=inContext.GetEmptySlotPosition();
			if(slotPosToEpilogue<0) { return false; }
			const int addrToLoopTop=inContext.GetMarkPosition();
			if(addrToLoopTop<0) { return false; }
			inContext.Compile(std::string("inc-loop-counter"));
			inContext.Compile(std::string("absolute-jump"));
			TypedValue tvAddrLoopTop(kTypeAddress,addrToLoopTop);
			inContext.Compile(tvAddrLoopTop);
		
			inContext.newWord->tmpParam->at(slotPosToEpilogue)
				=TypedValue(kTypeAddress,
							inContext.GetNextThreadAddressOnCompile());
		
			inContext.ExitLeavableLoop();
			inContext.Compile(std::string("loop-epilogue"));

			if(inContext.EndControlBlock()==false) { return false; }
			NEXT;
		}));
	}
	
	// loopCounter+=TOS
	// usage: n step
	// equivalent: r> + 1- >R
	Install(new Word("step",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& tvCounter=ReadTOS(inContext.RS);
		// tvCounter=tvCounter+tos-1
		AssignOp(tvCounter,+,tos);
		switch(tvCounter.dataType) {
			case kTypeInt:		tvCounter.intValue--;		break;
			case kTypeLong:		tvCounter.longValue--;		break;
			case kTypeBigInt:	(*tvCounter.bigIntPtr)--;	break;
			case kTypeFloat:	tvCounter.floatValue--;		break;
			case kTypeDouble:	tvCounter.doubleValue--;	break;
			case kTypeBigFloat:	(*tvCounter.bigFloatPtr)--;	break;
			default:
				inContext.Error(E_SYSTEM_ERROR);
				exit(-1);
		}
		NEXT;
	}));

	if(inOnlyUseCore==false) {
		// while - repeat
		// ex:
		//		input : 10 true while dup . 1 - dup 0 > repeat
		//		output: 10 9 8 7 6 5 4 3 2 1 ok.
		//
		/*---
			"while" :
				begin-control-block
				mark-address
				"branch-if-false" compile
				new-slot
				begin-loop-block
			; immediate
		---*/
		Install(new Word("while",WordLevel::Immediate,WORD_FUNC {
			inContext.BeginControlBlock();
			inContext.MarkHere();	// come back here.
			inContext.Compile(std::string("branch-if-false"));
			inContext.MarkAndCreateEmptySlot();	// exit address slot		
			inContext.EnterLeavableLoop();
			NEXT;
		}));

		/*---
			"repeat" :
				check-compile-mode
				"absolute-jump" compile
				r> compile
				get-next-thread-address
				compile-address-into-empty-slot
				end-loop-block
				end-control-block
			; immediate
		---*/
		Install(new Word("repeat",WordLevel::Immediate,WORD_FUNC {
			if(inContext.CheckCompileMode()==false) {
				return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
			}
			if(inContext.RS.size()<3) { return inContext.Error(E_RS_NOT_ENOUGH); }

			const int addrSlotToExit=inContext.GetEmptySlotPosition();
			if(addrSlotToExit<0) { return false; }

			const int jumpAddrToLoop=inContext.GetMarkPosition();
			if(jumpAddrToLoop<0) { return false; }
			
			inContext.Compile(std::string("absolute-jump"));
			inContext.Compile(TypedValue(kTypeAddress,jumpAddrToLoop));

			TypedValue emptySlot=inContext.newWord->tmpParam->at(addrSlotToExit);
			if(emptySlot.dataType!=kTypeEmptySlot) {
				return inContext.Error(E_SYSTEM_ERROR);
			}
			inContext.newWord->tmpParam->at(addrSlotToExit)
				=TypedValue(kTypeAddress,
							inContext.GetNextThreadAddressOnCompile());
			inContext.ExitLeavableLoop();
			if(inContext.EndControlBlock()==false) { return false; }
			NEXT;
		}));
	}

	// for optimize
	Install(new Word("_branchIfNotZero",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }

		TypedValue& tos=inContext.DS.back();
		if((tos.dataType==kTypeInt && tos.intValue!=0)
	      || (tos.dataType==kTypeLong && tos.longValue!=0) 
		  || (tos.dataType==kTypeFloat && tos.floatValue!=0)
		  || (tos.dataType==kTypeDouble && tos.doubleValue!=0)
		  || (tos.dataType==kTypeBigInt && *tos.bigIntPtr!=0)
		  || (tos.dataType==kTypeBigFloat && *tos.bigFloatPtr!=0)) {
			// BRANCH!
			inContext.ip=(const Word**)(*(inContext.ip+1));
		} else {
			inContext.ip+=2;	// Next
		}
		inContext.DS.pop_back();
		return true;
	}));

	// for optimize
	// equivalent to "repeat? branch-if-false"
	Install(new Word("_branchWhenLoopEnded",WORD_FUNC {
		const size_t n=inContext.RS.size();
		if(n<2) { return inContext.Error(E_RS_BROKEN); } 

		// equivalent: _r> +second/r> <
		const TypedValue& rsTos=inContext.RS.back();
		if(rsTos.dataType!=kTypeInt) {
			return inContext.Error(E_RS_BROKEN_TOS_SHOULD_BE_INT);
		}

		const int current=rsTos.intValue;
		const TypedValue& rsSecond=inContext.RS[n-2];
		if(rsSecond.dataType!=kTypeInt) {
			return inContext.Error(E_RS_BROKEN_SECOND_SHOULD_BE_INT);
		}

		const int target=rsSecond.intValue;
		if( current<=target ) {
			inContext.ip+=2;	// Next
		} else {
			inContext.ip=(const Word**)(*(inContext.ip+1));
		}
		return true;
	}));

	if(inOnlyUseCore==false) {
		/*---
			"switch" : begin-control-block begin-switch-block ; immediate
		---*/
		Install(new Word("switch",WordLevel::Immediate,WORD_FUNC {
			inContext.BeginControlBlock();
			inContext.EnterSwitchBlock();
			NEXT;
		}));

		/*---
			"->" : check-compile-mode "branch-if-false" compile new-slot ; immediate
		---*/
		Install(new Word("->",WordLevel::Immediate,WORD_FUNC {
			if(inContext.CheckCompileMode()==false) {
				return inContext.Error(E_SHOULD_BE_COMPILE_MODE);;
			}
			inContext.Compile(std::string("branch-if-false"));
			inContext.MarkAndCreateEmptySlot();
			NEXT;
		}));
	}

	Install(new Word("break",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_BROKEN); }

		int p=inContext.GetEmptySlotPosition();
		if(p<0) { return false; }

		inContext.Compile_Break();
		inContext.newWord->tmpParam->at(p)
			=TypedValue(kTypeAddress,
						inContext.GetNextThreadAddressOnCompile());
		NEXT;
	}));

	Install(new Word("case",Dict["dup"]->code));

	if(inOnlyUseCore==false) {
		/*---
			"dispatch" :
				check-compile-mode end-switch-block end-control-block
			; immediate
		---*/
		Install(new Word("dispatch",WordLevel::Immediate,WORD_FUNC {
			if(inContext.CheckCompileMode()==false) {
				return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
			}
			inContext.ExitSwitchBlock();
			if(inContext.EndControlBlock()==false) { return false; }
			NEXT;
		}));
	}
}

