#include <vector>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"

void ShowStack(Stack& inStack,const char *inStackName);

void InitDict_Control() {
	Install(new Word("_branch-if-false",WORD_FUNC {
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

	Install(new Word("_branch-if-true",WORD_FUNC {
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

	Install(new Word("_absolute-jump",WORD_FUNC {
		inContext.ip=(const Word**)(*(inContext.ip+1));
		return true;
	}));

	Install(new Word("if",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginControlBlock(kSyntax_IF);
		inContext.Compile(std::string("_branch-if-false"));
		inContext.MarkAndCreateEmptySlot();
		NEXT;
	}));

	Install(new Word("else",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		if(inContext.SS.size()<1) { return inContext.Error(E_SS_BROKEN); }
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=kTypeMiscInt || tvSyntax.intValue!=kSyntax_IF) {
			return inContext.Error(E_SYNTAX_MISMATCH_IF);
		}

		int p=inContext.GetEmptySlotPosition();
		if(p<0) { return false; }
		inContext.Compile(p,TypedValue(kTypeAddress,
									   inContext.GetNextThreadAddressOnCompile()+2));

		inContext.Compile(std::string("_absolute-jump"));
		inContext.MarkAndCreateEmptySlot();
		NEXT;
	}));

	Install(new Word("then",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		if(inContext.SS.size()<1) { return inContext.Error(E_SS_BROKEN); }
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=kTypeMiscInt || tvSyntax.intValue!=kSyntax_IF) {
			return inContext.Error(E_SYNTAX_MISMATCH_IF);
		}
		int p=inContext.GetEmptySlotPosition();
		if(p<0) { return inContext.Error(E_RS_BROKEN); }
		inContext.Compile(p,TypedValue(kTypeAddress,
									   inContext.GetNextThreadAddressOnCompile()));

		if(inContext.EndControlBlock()==false) {
			return false;
		}
		NEXT;
	}));

	// equivalent: @r> +second/r> <=
	Install(new Word("_repeat?+",WORD_FUNC {
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

		bool result;
		GetIntegerCmpOpResult_AndConvert(result,rsTos,<=,rsSecond);
		inContext.DS.emplace_back(result);

		NEXT;
	}));

	// equivalent: @r> +second/r> >=
	Install(new Word("_repeat?-",WORD_FUNC {
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

		bool result;
		GetIntegerCmpOpResult_AndConvert(result,rsTos,>=,rsSecond);
		inContext.DS.emplace_back(result);

		NEXT;
	}));
	
	// equivalent: r> 1+ >r
	Install(new Word("_inc-loop-counter",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }

		TypedValue& tvCounter=ReadTOS(inContext.RS);
		switch(tvCounter.dataType) {
			case kTypeInt:		tvCounter.intValue++;		break;
			case kTypeLong:		tvCounter.longValue++;		break;
			case kTypeBigInt:	(*tvCounter.bigIntPtr)++;	break;
			default:
				return inContext.Error_InvalidType(E_RS_TOS_INT_OR_LONG_OR_BIGINT,
												   tvCounter);
		}
		NEXT;
	}));

	// equivalent: r> 1- >r
	Install(new Word("_dec-loop-counter",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }

		TypedValue& tvCounter=ReadTOS(inContext.RS);
		switch(tvCounter.dataType) {
			case kTypeInt:		tvCounter.intValue--;		break;
			case kTypeLong:		tvCounter.longValue--;		break;
			case kTypeBigInt:	(*tvCounter.bigIntPtr)--;	break;
			default:
				return inContext.Error_InvalidType(E_RS_TOS_INT_OR_LONG_OR_BIGINT,
												   tvCounter);
		}
		NEXT;
	}));

	// equivalent: drop-rs drop-rs
	Install(new Word("_loop-epilogue",WORD_FUNC {
		if(inContext.RS.size()<2) { return inContext.Error(E_RS_BROKEN); }
		inContext.RS.pop_back();
		inContext.RS.pop_back();
		NEXT;
	}));

	Install(new Word("for+",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginControlBlock(kSyntax_FOR_PLUS);
		inContext.Compile(std::string(">r"));	// target value
		inContext.Compile(std::string(">r"));	// current value
		inContext.MarkHere();	// come back here.
		inContext.Compile(std::string("_repeat?+"));
		inContext.Compile(std::string("_branch-if-false"));
		inContext.MarkAndCreateEmptySlot();	// jump to _loop-epilogue
		inContext.EnterLeavableLoop();
		NEXT;
	}));

	Install(new Word("for-",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginControlBlock(kSyntax_FOR_MINUS);
		inContext.Compile(std::string(">r"));	// target value
		inContext.Compile(std::string(">r"));	// current value
		inContext.MarkHere();	// come back here.
		inContext.Compile(std::string("_repeat?-"));
		inContext.Compile(std::string("_branch-if-false"));
		inContext.MarkAndCreateEmptySlot();	// jump to _loop-epilogue
		inContext.EnterLeavableLoop();
		NEXT;
	}));

	Install(new Word("next",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=kTypeMiscInt
		  || (tvSyntax.intValue!=kSyntax_FOR_PLUS
			  && tvSyntax.intValue!=kSyntax_FOR_MINUS)) {
			return inContext.Error(E_SYNTAX_MISMATCH_FOR);
		}

		const int slotPosToExit=inContext.GetMarkPosition();
		if(slotPosToExit<0) { return false; }

		if(tvSyntax.intValue==kSyntax_FOR_PLUS) {
			inContext.Compile(std::string("_inc-loop-counter"));
		} else {
			inContext.Compile(std::string("_dec-loop-counter"));
		}

		const int p=inContext.GetMarkPosition();
		if(p<0) { return false; }
		TypedValue tvAddrLoopTop(kTypeAddress,p);
		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(tvAddrLoopTop);

		inContext.ExitLeavableLoop();

		TypedValue tvAddressToExit(kTypeAddress,
								   inContext.GetNextThreadAddressOnCompile());
		inContext.Compile(slotPosToExit,tvAddressToExit);

		inContext.Compile(std::string("_loop-epilogue"));

		if(inContext.EndControlBlock()==false) { return false; }
		NEXT;
	}));

	Install(new Word("leave",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		inContext.Compile_Leave();
		NEXT;
	}));

	Install(new Word("i",Dict["@r>"]->code));

	// loopCounter+=TOS
	// usage: n step
	// equivalent: r> + 1- >R
	Install(new Word("step",WordLevel::Immediate,WORD_FUNC {
		if(inContext.SS.size()<1) { return inContext.Error(E_SS_BROKEN); }
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=kTypeMiscInt
		  || (tvSyntax.intValue!=kSyntax_FOR_PLUS
			  && tvSyntax.intValue!=kSyntax_FOR_MINUS)) {
			return inContext.Error(E_SYNTAX_MISMATCH_FOR);
		}
		if(tvSyntax.intValue==kSyntax_FOR_PLUS) {
			inContext.Compile(std::string("std:_step+"));
		} else {
			inContext.Compile(std::string("std:_step-"));
		}
		NEXT;
	}));

	// step for count up.
	Install(new Word("_step+",WORD_FUNC {
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

	// equivalent to "2 _step+"
	Install(new Word("_step++",WORD_FUNC {
		TypedValue& tvCounter=ReadTOS(inContext.RS);
		switch(tvCounter.dataType) {
			case kTypeInt:		tvCounter.intValue++;		break;
			case kTypeLong:		tvCounter.longValue++;		break;
			case kTypeBigInt:	(*tvCounter.bigIntPtr)++;	break;
			case kTypeFloat:	tvCounter.floatValue++;		break;
			case kTypeDouble:	tvCounter.doubleValue++;	break;
			case kTypeBigFloat:	(*tvCounter.bigFloatPtr)++;	break;
			default:
				inContext.Error(E_SYSTEM_ERROR);
				exit(-1);
		}
		NEXT;
	}));

	// step for count down.
	Install(new Word("_step-",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& tvCounter=ReadTOS(inContext.RS);
		// tvCounter=tvCounter+tos+1
			AssignOp(tvCounter,+,tos);
		switch(tvCounter.dataType) {
			case kTypeInt:		tvCounter.intValue++;		break;
			case kTypeLong:		tvCounter.longValue++;		break;
			case kTypeBigInt:	(*tvCounter.bigIntPtr)++;	break;
			case kTypeFloat:	tvCounter.floatValue++;		break;
			case kTypeDouble:	tvCounter.doubleValue++;	break;
			case kTypeBigFloat:	(*tvCounter.bigFloatPtr)++;	break;
			default:
				inContext.Error(E_SYSTEM_ERROR);
				exit(-1);
		}
		NEXT;
	}));

	// while - repeat
	// ex:
	//		input : 10 true while dup . 1 - dup 0 > repeat
	//		output: 10 9 8 7 6 5 4 3 2 1 ok.
	Install(new Word("while",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginControlBlock(kSyntax_WHILE);
		inContext.MarkHere();	// come back here.
		inContext.Compile(std::string("_branch-if-false"));
		inContext.MarkAndCreateEmptySlot();	// exit address slot		
		inContext.EnterLeavableLoop();
		NEXT;
	}));

	Install(new Word("repeat",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		if(inContext.RS.size()<3) { return inContext.Error(E_RS_NOT_ENOUGH); }
		if(inContext.SS.size()<1) { return inContext.Error(E_SS_BROKEN); }
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=kTypeMiscInt
		   || (tvSyntax.intValue!=kSyntax_WHILE
			   && tvSyntax.intValue!=kSyntax_WHILE_COND)) {
			return inContext.Error(E_SYNTAX_MISMATCH_WHILE);
		}

		if(tvSyntax.intValue==kSyntax_WHILE_COND) {
			inContext.Compile(std::string("@r>exec"));
		}

		const int addrSlotToExit=inContext.GetEmptySlotPosition();
		if(addrSlotToExit<0) { return false; }

		const int jumpAddrToLoop=inContext.GetMarkPosition();
		if(jumpAddrToLoop<0) { return false; }
			
		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(TypedValue(kTypeAddress,jumpAddrToLoop));

		TypedValue emptySlot=inContext.newWord->tmpParam->at(addrSlotToExit);
		if(emptySlot.dataType!=kTypeEmptySlot) {
			return inContext.Error(E_SYSTEM_ERROR);
		}
		inContext.Compile(addrSlotToExit,
						  TypedValue(kTypeAddress,
									 inContext.GetNextThreadAddressOnCompile()));

		inContext.ExitLeavableLoop();
		if(inContext.EndControlBlock()==false) { return false; }
		NEXT;
	}));

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
	// equivalent to "_repeat?+ branch-if-false"
	Install(new Word("_branchWhenLoopEnded",WORD_FUNC {
		const size_t n=inContext.RS.size();
		if(n<2) { return inContext.Error(E_RS_BROKEN); } 

		TypedValue& rsTos=inContext.RS.back();
		TypedValue& rsSecond=inContext.RS[n-2];

		bool result;
		GetIntegerCmpOpResult_AndConvert(result,rsTos,<=,rsSecond);
		if( result ) {
			inContext.ip+=2;	// Next
		} else {
			inContext.ip=(const Word**)(*(inContext.ip+1));
		}
		return true;
	}));

	Alias("while{","{");

	// equivalent to "} >r @r>exec while"
	Install(new Word("}->",WordLevel::Level2,WORD_FUNC {
		if(inContext.Exec(std::string("}"))==false) { return false; }
		switch(inContext.ExecutionThreshold) {
			case kInterpretLevel:
				inContext.Exec(std::string(">r"));
				inContext.Exec(std::string("@r>exec"));
				inContext.Exec(std::string("while"));
				break;
			case kCompileLevel:
				inContext.Compile(std::string(">r"));
				inContext.Compile(std::string("@r>exec"));
				inContext.Exec(std::string("while"));
				break;
			default: // kSymbolLevel or unknown case
				inContext.Error(E_SYSTEM_ERROR);
				exit(-1);
		}
		TypedValue& ssTos=ReadTOS(inContext.SS);
		assert(ssTos.dataType==kTypeMiscInt && ssTos.intValue==kSyntax_WHILE);
		ssTos.intValue=kSyntax_WHILE_COND;
		NEXT;
	}));

	Install(new Word("do",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginControlBlock(kSyntax_DO);
		inContext.MarkHere();	// come back here.
		inContext.EnterLeavableLoop();
		NEXT;
	}));

	Install(new Word("loop",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_NOT_ENOUGH); }
		if(inContext.SS.size()<1) { return inContext.Error(E_SS_BROKEN); }
		TypedValue tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=kTypeMiscInt || tvSyntax.intValue!=kSyntax_DO) {
			return inContext.Error(E_SYNTAX_MISMATCH_DO);
		}

		const int jumpAddrToLoop=inContext.GetMarkPosition();
		if(jumpAddrToLoop<0) { return false; }
		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(TypedValue(kTypeAddress,jumpAddrToLoop));

		inContext.ExitLeavableLoop();
		if(inContext.EndControlBlock()==false) { return false; }
		NEXT;
	}));

	Install(new Word("switch",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginControlBlock(kSyntax_SWITCH);
		inContext.EnterSwitchBlock();
		NEXT;
	}));

	Install(new Word("->",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);;
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=kTypeMiscInt && tvSyntax.intValue!=kSyntax_SWITCH) {
			return inContext.Error(E_SYNTAX_MISMATCH_SWITCH);
		}

		inContext.Compile(std::string("_branch-if-false"));
		inContext.MarkAndCreateEmptySlot();
		inContext.Compile(std::string("drop"));
		NEXT;
	}));

	Install(new Word("break",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_BROKEN); }
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=kTypeMiscInt && tvSyntax.intValue!=kSyntax_SWITCH) {
			return inContext.Error(E_SYNTAX_MISMATCH_SWITCH);
		}

		int p=inContext.GetEmptySlotPosition();
		if(p<0) { return false; }

		inContext.Compile_Break();
		inContext.Compile(p,TypedValue(kTypeAddress,
									   inContext.GetNextThreadAddressOnCompile()));

		NEXT;
	}));

	Install(new Word("case",Dict["dup"]->code));

	Install(new Word("dispatch",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		inContext.ExitSwitchBlock();
		if(inContext.EndControlBlock()==false) { return false; }
		NEXT;
	}));
}

