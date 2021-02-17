#include <vector>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"

PP_API void ShowStack(Stack& inStack,const char *inStackName);

static int getStandardLoopChunkTop(Context& inContext);

/*---
  The Paraphrase standard iterative blocks are:
  	for+ ... next	<- counting up
	for- ... next	<- counting down
	while cond do ... repeat  <- same as while(cond) { ... } in C.
	loop ... when cond repeat <- same as do { ... } while(cond) in C.

  The iterative processing block of "Paraphrase" has the following functions:.

  	continue:	Jump to the condition check for the next iteration.
				If the repetition is a simple infinite loop,
				it is semantically the same as the word "redo" in programming.

	redo:		Jump to the iteration block top without the condition check.

	leave:		Exit to the iteration block.
---*/

void InitDict_Control() {
	Install(new Word("quit",WORD_FUNC {
		printf("bye.\n");
		exit(0);
	}));

	Install(new Word("exit",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tos);
		}
		exit(tos.intValue);
	}));

	Install(new Word("_branch-if-false",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=inContext.DS.back();
		if(tos.dataType!=DataType::kTypeBool) {
			inContext.DS.pop_back();
			return inContext.Error(InvalidTypeErrorID::E_TOS_BOOL,tos);
		}
		if( tos.boolValue ) {
			inContext.ip+=2;	// Next
		} else {
			inContext.ip=(const Word**)(*(inContext.ip+1));
		}
		inContext.DS.pop_back();
		return true;
	}));

	Install(new Word("_branch-if-true",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeBool) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_BOOL,tos);
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

	// SS:
	//     +------------------+
	//     | kSyntax_IF       |
	//     +------------------+
	//     | 3 (==chunk size) |
	//     +------------------+
	//     | alpha            | <--- empty-slot-addr (*1)
	//     +------------------+
	//
	//     *1  The address value is written to this empty slot
	//         by processing the word 'else' or 'then'.
	Install(new Word("if",WordLevel::Immediate,WORD_FUNC {
		if(inContext.BeginControlBlock()==false) {
			return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		}
		inContext.Compile(std::string("_branch-if-false"));
		TypedValue tvAlpha=inContext.MarkAndCreateEmptySlot();
		
		// build syntax-if chunk on SS.
		inContext.SS.emplace_back(tvAlpha);
		inContext.SS.emplace_back(3);
		inContext.SS.emplace_back(ControlBlockType::kSyntax_IF);
		NEXT;
	}));

	Install(new Word("else",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);
		}
		if(inContext.SS.size()<3) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=DataType::kTypeCB
		   || tvSyntax.intValue!=(int)ControlBlockType::kSyntax_IF) {
			return inContext.Error(NoParamErrorID::E_SYNTAX_MISMATCH_IF);
		}
		int p=(int)inContext.SS.size()-1;
		if(p-2<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
		int slotForTheWordOfThen=p-2;
		TypedValue& tvAlpha=inContext.SS.at(slotForTheWordOfThen);
		if(tvAlpha.dataType!=DataType::kTypeAddress) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		int alpha=tvAlpha.intValue;
		if(alpha<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
		// write an address of head of else-block
		inContext.Compile(alpha,
						  TypedValue(DataType::kTypeAddress,
									 inContext.GetNextThreadAddressOnCompile()+2));
		inContext.Compile(std::string("_absolute-jump"));
		TypedValue tvAlpha2=inContext.MarkAndCreateEmptySlot();
		inContext.SS.at(slotForTheWordOfThen)=tvAlpha2;
		NEXT;
	}));

	Install(new Word("then",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);
		}
		if(inContext.SS.size()<3) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=DataType::kTypeCB
		   || tvSyntax.intValue!=(int)ControlBlockType::kSyntax_IF) {
			return inContext.Error(NoParamErrorID::E_SYNTAX_MISMATCH_IF);
		}
		int p=(int)inContext.SS.size()-1;
		if(p-2<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
		int slotForTheWordOfThen=p-2;
		TypedValue& tvAlpha=inContext.SS.at(slotForTheWordOfThen);
		if(tvAlpha.dataType!=DataType::kTypeAddress) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		int alpha=tvAlpha.intValue;
		if(alpha<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
		// write an address of head of else-block
		inContext.Compile(alpha,
			   			  TypedValue(DataType::kTypeAddress,
									 inContext.GetNextThreadAddressOnCompile()));
		if(inContext.RemoveTopChunk()==false) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		if(inContext.EndControlBlock()==false) {
			return false;
		}
		NEXT;
	}));

	// equivalent: @r> +second/r> <=
	Install(new Word("_repeat?+",WORD_FUNC {
		const size_t n=inContext.RS.size();
		if(n<2) { return inContext.Error(NoParamErrorID::E_RS_BROKEN); } 

		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=DataType::kTypeInt
		  && rsTos.dataType!=DataType::kTypeLong
		  && rsTos.dataType!=DataType::kTypeBigInt) {
			return inContext.Error(InvalidTypeErrorID::E_RS_TOS_INT_OR_LONG_OR_BIGINT,rsTos);
		}

		TypedValue& rsSecond=inContext.RS[n-2];
		if(rsSecond.dataType!=DataType::kTypeInt
		  && rsSecond.dataType!=DataType::kTypeLong
		  && rsSecond.dataType!=DataType::kTypeBigInt) {
			return inContext.Error(InvalidTypeErrorID::E_RS_SECOND_INT_OR_LONG_OR_BIGINT,rsSecond);
		}

		bool result;
		GetIntegerCmpOpResult_AndConvert(result,rsTos,<,rsSecond);
		inContext.DS.emplace_back(result);

		NEXT;
	}));

	// equivalent: @r> +second/r> >=
	Install(new Word("_repeat?-",WORD_FUNC {
		const size_t n=inContext.RS.size();
		if(n<2) { return inContext.Error(NoParamErrorID::E_RS_BROKEN); } 

		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=DataType::kTypeInt
		  && rsTos.dataType!=DataType::kTypeLong
		  && rsTos.dataType!=DataType::kTypeBigInt) {
			return inContext.Error(InvalidTypeErrorID::E_RS_TOS_INT_OR_LONG_OR_BIGINT,rsTos);
		}

		TypedValue& rsSecond=inContext.RS[n-2];
		if(rsSecond.dataType!=DataType::kTypeInt
		  && rsSecond.dataType!=DataType::kTypeLong
		  && rsSecond.dataType!=DataType::kTypeBigInt) {
			return inContext.Error(InvalidTypeErrorID::E_RS_SECOND_INT_OR_LONG_OR_BIGINT,rsSecond);
		}

		bool result;
		GetIntegerCmpOpResult_AndConvert(result,rsTos,>,rsSecond);
		inContext.DS.emplace_back(result);

		NEXT;
	}));
	
	// equivalent: r> 1+ >r
	Install(new Word("_inc-loop-counter",WORD_FUNC {
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}

		TypedValue& tvCounter=ReadTOS(inContext.RS);
		switch(tvCounter.dataType) {
			case DataType::kTypeInt:	tvCounter.intValue++;		break;
			case DataType::kTypeLong:	tvCounter.longValue++;		break;
			case DataType::kTypeBigInt:	(*tvCounter.bigIntPtr)++;	break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_RS_TOS_INT_OR_LONG_OR_BIGINT,tvCounter);
		}
		NEXT;
	}));

	// equivalent: r> 1- >r
	Install(new Word("_dec-loop-counter",WORD_FUNC {
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}

		TypedValue& tvCounter=ReadTOS(inContext.RS);
		switch(tvCounter.dataType) {
			case DataType::kTypeInt:	tvCounter.intValue--;		break;
			case DataType::kTypeLong:	tvCounter.longValue--;		break;
			case DataType::kTypeBigInt:	(*tvCounter.bigIntPtr)--;	break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_RS_TOS_INT_OR_LONG_OR_BIGINT,tvCounter);
		}
		NEXT;
	}));

	// equivalent: drop-rs drop-rs
	Install(new Word("_loop-epilogue",WORD_FUNC {
		if(inContext.RS.size()<2) {
			return inContext.Error(NoParamErrorID::E_RS_BROKEN);
		}
		inContext.RS.pop_back();
		inContext.RS.pop_back();
		NEXT;
	}));

	// SS:
	//    +------------------+
	//    | kSyntax_FOR_PLUS |
	//    +------------------+
	//    | 6 (==chunk size) |
	//    +------------------+
	//    | alpha1           | <--- the address of loop top for 'next' 
	//    +------------------+      
	//    | alpha2           | <--- the address of repeat-check branch
	//    +------------------+      for 'leave'
	//    | alpha3           | <--- the empty slot address for loop-exit.
	//    +------------------+
	//    | alpha4           | <--- the address for 'redo'
	//    +------------------+      loop again without count up and repeat-check.
	//    | alpha5           | <--- the address for 'continue' (same as alpha1).
	//    +------------------+
	//
	//	'for+ ... next' is equivalent to
	//		>r 1- >r while _inc-loop-counter _repeat?+ do ... repeat drop-rs drop-rs
	//		^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^        ^^^^^^
	//		       ==  for+                                     next
	Install(new Word("for+",WordLevel::Immediate,WORD_FUNC {
		if(inContext.BeginControlBlock()==false) {
			return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		}
		inContext.Compile(std::string(">r"));	// target value
		inContext.Compile(std::string("1-"));
		inContext.Compile(std::string(">r"));	// current value
		TypedValue tvAlpha1=inContext.MarkHere();	// come back here.
		inContext.Compile(std::string("_inc-loop-counter"));
		inContext.Compile(std::string("_repeat?+"));
		TypedValue tvAlpha2=inContext.MarkHere();	// for the word 'leave'.
		inContext.Compile(std::string("_branch-if-false"));
		TypedValue tvAlpha3=inContext.MarkAndCreateEmptySlot();
		TypedValue tvAlpha4=inContext.MarkHere();	// for 'redo'.

		const int chunkSize=7;
		inContext.SS.emplace_back(tvAlpha1);	// alpha5
		inContext.SS.emplace_back(tvAlpha4);
		inContext.SS.emplace_back(tvAlpha3);
		inContext.SS.emplace_back(tvAlpha2);
		inContext.SS.emplace_back(tvAlpha1);
		inContext.SS.emplace_back(chunkSize);
		inContext.SS.emplace_back(ControlBlockType::kSyntax_FOR_PLUS);

		NEXT;
	}));

	// SS: almost same as the word 'for+'.
	Install(new Word("for-",WordLevel::Immediate,WORD_FUNC {
		if(inContext.BeginControlBlock()==false) {
			return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		}
		inContext.Compile(std::string(">r"));	// target value
		inContext.Compile(std::string("1-"));
		inContext.Compile(std::string(">r"));	// current value
		TypedValue tvAlpha1=inContext.MarkHere();	// come back here.
		inContext.Compile(std::string("_dec-loop-counter"));
		inContext.Compile(std::string("_repeat?-"));
		TypedValue tvAlpha2=inContext.MarkHere();
		inContext.Compile(std::string("_branch-if-false"));
		TypedValue tvAlpha3=inContext.MarkAndCreateEmptySlot();
		TypedValue tvAlpha4=inContext.MarkHere();

		const int chunkSize=7;
		inContext.SS.emplace_back(tvAlpha1);	// alpha5
		inContext.SS.emplace_back(tvAlpha4);
		inContext.SS.emplace_back(tvAlpha3);
		inContext.SS.emplace_back(tvAlpha2);
		inContext.SS.emplace_back(tvAlpha1);
		inContext.SS.emplace_back(chunkSize);
		inContext.SS.emplace_back(ControlBlockType::kSyntax_FOR_MINUS);

		NEXT;
	}));

	Install(new Word("continue",WordLevel::Immediate,WORD_FUNC {
		int chunkTop=getStandardLoopChunkTop(inContext);
		if(chunkTop<0) { return false; }
		int alpha5=GetAddress(inContext.SS,chunkTop-6);
		if(alpha5<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(TypedValue(DataType::kTypeAddress,alpha5));
		NEXT;
	}));
	
	Install(new Word("redo",WordLevel::Immediate,WORD_FUNC {
		int chunkTop=getStandardLoopChunkTop(inContext);
		if(chunkTop<0) { return false; }
		int alpha4=GetAddress(inContext.SS,chunkTop-5);
		if(alpha4<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(TypedValue(DataType::kTypeAddress,alpha4));
		NEXT;
	}));

	// equivalent to "false continue".
	Install(new Word("leave",WordLevel::Immediate,WORD_FUNC {
		int chunkTop=getStandardLoopChunkTop(inContext);
		if(chunkTop<0) { return false; }
		int alpha2=GetAddress(inContext.SS,chunkTop-3);
		if(alpha2<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }

		inContext.Compile(std::string("_lit"));
		inContext.Compile(TypedValue(false));
		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(TypedValue(DataType::kTypeAddress,alpha2));
		NEXT;
	}));

	Install(new Word("i",Dict["@r>"]->code));

	// equivalent to DS.push(RS[RS.size-3])
	Install(new Word("j",WORD_FUNC {
		if(inContext.RS.size()<3) {
			return inContext.Error(NoParamErrorID::E_RS_AT_LEAST_3);
		}
		inContext.DS.emplace_back(inContext.RS[inContext.RS.size()-3]);
		NEXT;
	}));

	// loopCounter+=TOS
	// usage: n step
	// equivalent: r> + 1- >R
	Install(new Word("step",WordLevel::Immediate,WORD_FUNC {
		const int chunkSize=6;
		if(inContext.SS.size()<chunkSize) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=DataType::kTypeCB
		  || (tvSyntax.intValue!=(int)ControlBlockType::kSyntax_FOR_PLUS
			  && tvSyntax.intValue!=(int)ControlBlockType::kSyntax_FOR_MINUS)) {
			return inContext.Error(NoParamErrorID::E_SYNTAX_MISMATCH_FOR);
		}
		if(tvSyntax.intValue==(int)ControlBlockType::kSyntax_FOR_PLUS) {
			inContext.Compile(std::string("std:_step+"));
		} else {
			inContext.Compile(std::string("std:_step-"));
		}
		NEXT;
	}));

	// step for count up.
	Install(new Word("_step+",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}
		TypedValue& tvCounter=ReadTOS(inContext.RS);
		// tvCounter=tvCounter+tos-1
		AssignOpToSecond(inContext,tvCounter,+,tos);
		switch(tvCounter.dataType) {
			case DataType::kTypeInt:		tvCounter.intValue--;		break;
			case DataType::kTypeLong:		tvCounter.longValue--;		break;
			case DataType::kTypeBigInt:		(*tvCounter.bigIntPtr)--;	break;
			case DataType::kTypeFloat:		tvCounter.floatValue--;		break;
			case DataType::kTypeDouble:		tvCounter.doubleValue--;	break;
			case DataType::kTypeBigFloat:	(*tvCounter.bigFloatPtr)--;	break;
			default:
				inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				exit(-1);
		}
		NEXT;
	}));

	// equivalent to "2 _step+"
	Install(new Word("_step++",WORD_FUNC {
		TypedValue& tvCounter=ReadTOS(inContext.RS);
		switch(tvCounter.dataType) {
			case DataType::kTypeInt:		tvCounter.intValue++;		break;
			case DataType::kTypeLong:		tvCounter.longValue++;		break;
			case DataType::kTypeBigInt:		(*tvCounter.bigIntPtr)++;	break;
			case DataType::kTypeFloat:		tvCounter.floatValue++;		break;
			case DataType::kTypeDouble:		tvCounter.doubleValue++;	break;
			case DataType::kTypeBigFloat:	(*tvCounter.bigFloatPtr)++;	break;
			default:
				inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				exit(-1);
		}
		NEXT;
	}));

	// step for count down.
	Install(new Word("_step-",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}
		TypedValue& tvCounter=ReadTOS(inContext.RS);
		// tvCounter=tvCounter+tos+1
		AssignOpToSecond(inContext,tvCounter,+,tos);
		switch(tvCounter.dataType) {
			case DataType::kTypeInt:		tvCounter.intValue++;		break;
			case DataType::kTypeLong:		tvCounter.longValue++;		break;
			case DataType::kTypeBigInt:		(*tvCounter.bigIntPtr)++;	break;
			case DataType::kTypeFloat:		tvCounter.floatValue++;		break;
			case DataType::kTypeDouble:		tvCounter.doubleValue++;	break;
			case DataType::kTypeBigFloat:	(*tvCounter.bigFloatPtr)++;	break;
			default:
				inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				exit(-1);
		}
		NEXT;
	}));

	// while - repeat
	// ex:
	//		input : 10 true while dup . 1 - dup 0 > repeat
	//		output: 10 9 8 7 6 5 4 3 2 1 ok.
	//
	// SS:
	//    +------------------+
	//    | kSyntax_WHILE    |
	//    +------------------+
	//    | 7 (==chunk size) |
	//    +------------------+
	//    | alpha1           | <--- the address of loop top with repeat-check 
	//    +------------------+      for 'repeat'
	//    | alpha2           | <--- the address of repeat-check branch
	//    +------------------+      for 'leave' 
	//    | alpha3           | <--- the empty slot address for loop-exit.
	//    +------------------+
	//    | alpha4           | <--- the address for 'redo'
	//    +------------------+      loop again without repeat-check
	//    | alpha5           | <--- the address for 'continue' (same as alpha1)
	//    +------------------+
	//
	// ex:
	// 	while <-- alpha1
	// 	... condition check code ... <-- alpha2 and alpha5
	// 	do <-- alpha4
	// 		... loop body ...
	// 	repeat
	// 	 <-- jump here by slot at alpha3
	Install(new Word("while",WordLevel::Immediate,WORD_FUNC {
		if(inContext.BeginControlBlock()==false) {
			return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		}
		TypedValue tvAlpha1=inContext.MarkHere();	// come back here.
		TypedValue tvAlpha2,tvAlpha3,tvAlpha4;		// determined by the word 'do'.

		const int chunkSize=7;
		inContext.SS.emplace_back(tvAlpha1);	// alpha5 equal to alpha1.
		inContext.SS.emplace_back(tvAlpha4);	// TBD
		inContext.SS.emplace_back(tvAlpha3);	// TBD
		inContext.SS.emplace_back(tvAlpha1);	// alpha2
		inContext.SS.emplace_back(tvAlpha1);	
		inContext.SS.emplace_back(chunkSize);
		inContext.SS.emplace_back(ControlBlockType::kSyntax_WHILE);

		NEXT;
	}));

	Install(new Word("do",WordLevel::Immediate,WORD_FUNC {
		int chunkTop=getStandardLoopChunkTop(inContext);
		if(chunkTop<0) { return false; }

		TypedValue tvAlpha2=inContext.MarkHere();
		inContext.Compile(std::string("_branch-if-false"));
		TypedValue tvAlpha3=inContext.MarkAndCreateEmptySlot();	// exit address slot		
		TypedValue tvAlpha4=inContext.MarkHere();

		inContext.SS[chunkTop-3]=tvAlpha2;
		inContext.SS[chunkTop-4]=tvAlpha3;
		inContext.SS[chunkTop-5]=tvAlpha4;

		NEXT;
	}));

	// loop ... when cond repeat
	// 	or
	// loop ... repeat <- infinite loop
	//
	// execute a loop-block at least once.
	// ex1:
	//		input : 10 loop dup . when 1 - dup 0 > repeat
	//		output: 10 9 8 7 6 5 4 3 2 1 ok.
	//
	// ex2:
	//		input : -10 loop "LOOP-IN" . dup . when 1 - dup 0 > repeat
	//		output: LOOP-IN -10 ok.
	//
	// 'loop' generates
	// 			jump to alpha4
	// alpha1:	true	(for 'repeat')
	// alpha2:  branch-if-false beta (for 'leave'). 'beta' is in the slot which address is alpha3.
	// 			jump to alpha4	(for the case of 'repeat'; come from alpha1).
	// alpha5:	jump to alpha4	(for 'continue'). this OP may be change by 'when'.
	// alpha4:	(first code of the iteration)
	//				:
	// beta:
	// 
	//
	// if the iterate block has 'when'.
	// 					jump to alpha4
	// 					true <- never used
	// (set alpha1's value is alpha2 by the word 'when')
	// alpha1(==alpha2):	branch-if-false beta (for 'repeat' and 'leave') <- this is generated by the word 'when'.  'beta' is in the slot which address is alpha3.
	//						jump to alpha4
	// alpha5:				jump to the position of 'when'.	(for 'continue'.) <- this is generated by the word 'when'. 
	// alpha4:				(first code of the iteration)
	//							:
	// beta:
	//
	//
	// continue:
	// 		jump to the position of 'cond'.
	// 		if the iteration is infinite, jump to iteration top (same as 'redo').
	// redo:
	// 		jump to the iteration top.
	// leave:
	// 		jump to the next position of the word 'repeat'.
	//
	//
	// SS: same as the word 'while'.
	//
	// SS:
	//    +------------------+
	//    | kSyntax_LOOP     |
	//    +------------------+
	//    | 7 (==chunk size) |
	//    +------------------+
	//    | alpha1           | <--- the address for 'repeat'.
	//    +------------------+      
	//    | alpha2           | <--- the address of repeat-check branch for 'leave'.
	//    +------------------+      
	//    | alpha3           | <--- the empty slot address for loop-exit.
	//    +------------------+		note: address[alpha3]=beta
	//    | alpha4           | <--- the address for 'redo'
	//    +------------------+      
	//    | alpha5           | <--- the address for 'continue'
	//    +------------------+
	//
	// cheked: repeat - OK, continue - OK, leave - OK, redo - OK.
	Install(new Word("loop",WordLevel::Immediate,WORD_FUNC {
		if(inContext.BeginControlBlock()==false) {
			return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		}
		// 9 == {jump,alpha4,
		// 		 true,
		// 		 branch-if-true, alpha4,
		// 		 jump,beta,
		// 		 jump,alpha4}
		TypedValue tvAlpha4=inContext.MarkHere(9);

		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(tvAlpha4);
		
		TypedValue tvAlpha1=inContext.MarkHere();
		inContext.Compile(std::string("true"));

		TypedValue tvAlpha2=inContext.MarkHere();
		inContext.Compile(std::string("_branch-if-false"));
		TypedValue tvAlpha3=inContext.MarkAndCreateEmptySlot();	// exit address slot		

		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(tvAlpha4);

		TypedValue tvAlpha5=inContext.MarkHere();

		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(tvAlpha4);

		const int chunkSize=7;
		inContext.SS.emplace_back(tvAlpha5);
		inContext.SS.emplace_back(tvAlpha4);
		inContext.SS.emplace_back(tvAlpha3);
		inContext.SS.emplace_back(tvAlpha2);
		inContext.SS.emplace_back(tvAlpha1);
		inContext.SS.emplace_back(chunkSize);
		inContext.SS.emplace_back(ControlBlockType::kSyntax_LOOP);

		NEXT;
	}));
	
	// jump to alpha1
	//   and
	// determine the address to alpha3.
	Install(new Word("repeat",WordLevel::Immediate,WORD_FUNC {
		int chunkTop=getStandardLoopChunkTop(inContext);
		if(chunkTop<0) { return false; }

		int alpha1=GetAddress(inContext.SS,chunkTop-2);
		if(alpha1<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
		int alpha3=GetAddress(inContext.SS,chunkTop-4);
		if(alpha3<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }

		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(TypedValue(DataType::kTypeAddress,alpha1));

		TypedValue tvAddressToExit(DataType::kTypeAddress,
								   inContext.GetNextThreadAddressOnCompile());
		inContext.Compile(alpha3,tvAddressToExit);
		if(inContext.RemoveTopChunk()==false) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		if(inContext.EndControlBlock()==false) { return false; }
		NEXT;
	}));

	Install(new Word("next",WordLevel::Immediate,WORD_FUNC {
		int chunkTop=getStandardLoopChunkTop(inContext);
		if(chunkTop<0) { return false; }

		int alpha1=GetAddress(inContext.SS,chunkTop-2);
		if(alpha1<0) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		int alpha3=GetAddress(inContext.SS,chunkTop-4);
		if(alpha3<0) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}

		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(TypedValue(DataType::kTypeAddress,alpha1));

		TypedValue tvAddressToExit(DataType::kTypeAddress,
								   inContext.GetNextThreadAddressOnCompile());
		inContext.Compile(alpha3,tvAddressToExit);
		inContext.Compile(std::string("2drop-rs"));
		if(inContext.RemoveTopChunk()==false) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		if(inContext.EndControlBlock()==false) { return false; }

		NEXT;
	}));

	Install(new Word("when",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);
		}
		const int chunkSize=7;
		if(inContext.SS.size()<chunkSize) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=DataType::kTypeCB) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		if(tvSyntax.intValue!=(int)ControlBlockType::kSyntax_LOOP) {
			return inContext.Error(InvalidTypeErrorID::E_SS_TOS_SHOULD_BE_LOOP,tvSyntax);
		}
		int chunkTop=(int)inContext.SS.size()-1;
		TypedValue& tvChunkSize=inContext.SS.at(chunkTop-1);
		if(tvChunkSize.dataType!=DataType::kTypeInt) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		if(tvChunkSize.intValue!=chunkSize) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}

		int alpha1=GetAddress(inContext.SS,chunkTop-2);
		if(alpha1<0) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}

		int alpha2=GetAddress(inContext.SS,chunkTop-3);
		if(alpha2<0) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}

		// now alpha1=alpha2
		inContext.SS[chunkTop-2]=TypedValue(DataType::kTypeAddress,alpha2);

		// update address at alpha5
		int alpha5=GetAddress(inContext.SS,chunkTop-6);
		if(alpha5<0) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		TypedValue whenAddress=inContext.MarkHere();
		inContext.Compile(alpha5+1,whenAddress,true);
		NEXT;
	}));
	
	// for optimize
	Install(new Word("_branchIfNotZero",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}

		TypedValue& tos=inContext.DS.back();
		if((tos.dataType==DataType::kTypeInt && tos.intValue!=0)
	      || (tos.dataType==DataType::kTypeLong && tos.longValue!=0) 
		  || (tos.dataType==DataType::kTypeFloat && tos.floatValue!=0)
		  || (tos.dataType==DataType::kTypeDouble && tos.doubleValue!=0)
		  || (tos.dataType==DataType::kTypeBigInt && *tos.bigIntPtr!=0)
		  || (tos.dataType==DataType::kTypeBigFloat && *tos.bigFloatPtr!=0)) {
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
		if(n<2) { return inContext.Error(NoParamErrorID::E_RS_BROKEN); } 

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

	// address                 code-thread
	//                        | ..................... |
	//                        +-----------------------+ <-- invoke a word 'switch'
	// ADDR+0                 | JUMP                  | <-- jump to switch-block body.
	//                        +-----------------------+
	// ADDR+1              +--+ (ADDR+1)+3            |
	//                     |  +-----------------------+
	// ADDR+2 (==alpha1)   |  | JUMP                  | <-- 'break' jump to here for
	//                     |  +-----------------------+     exit to the 'switch' block.
	// ADDR+3 (==alpha2)   |  | (end-to-switch-block) | <-- empty slot
	//                     |  +-----------------------+
	// ADDR+4              +->| ..................... |
	//                        +-----------------------+
	//                                   :
	//                        +-----------------------+ <-- invoke a word '->'
	// ADDR+n-1               | _branck-if-false      |
	//                        +-----------------------+
	// ADDR+n (==alpha3)      | (next-case-block)     | <-- empty slot
	//                        +-----------------------+
	//                                   :
	//
	// note: alpha2 is updated by each '->' or '->>'.
	//
	// SS:
	//     +-----------------+
	//     | kSyntax_SWITCH  |
	//     +-----------------+
	//     | 5 (==chunkSize) |
	//     +-----------------+
	//     | alpha1          | <-- address for the word 'break'.
	//     +-----------------+
	//     | alpha2          | <-- address for empty slot to write the address of
	//     +-----------------+     the next of the end of the switch-block.
	//     | alpha3          | <-- address for empty slot to write the address of
	//     +-----------------+     the next of the end of the case-block.
	//
	Install(new Word("switch",WordLevel::Immediate,WORD_FUNC {
		if(inContext.BeginControlBlock()==false) {
			return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		}
		inContext.Compile(std::string("_absolute-jump"));
		int currentAddress=inContext.GetNextThreadAddressOnCompile();
		inContext.Compile(TypedValue(DataType::kTypeAddress,currentAddress+3));
		TypedValue tvAlpha1=inContext.MarkHere();
		inContext.Compile(std::string("_absolute-jump"));
		TypedValue tvAlpha2=inContext.MarkAndCreateEmptySlot();

		inContext.SS.emplace_back(TypedValue()); // initial value of alpha3 is invalid.
		inContext.SS.emplace_back(tvAlpha2);
		inContext.SS.emplace_back(tvAlpha1);
		const int chunkSize=5;
		inContext.SS.emplace_back(chunkSize);
		inContext.SS.emplace_back(ControlBlockType::kSyntax_SWITCH);

		NEXT;
	}));

	Install(new Word("case",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);;
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=DataType::kTypeCB
		   || tvSyntax.intValue!=(int)ControlBlockType::kSyntax_SWITCH) {
			return inContext.Error(NoParamErrorID::E_SYNTAX_MISMATCH_SWITCH);
		}
		TypedValue& tvChunkSize=ReadSecond(inContext.SS);
		const int chunkSize=5;
		if(tvChunkSize.dataType!=DataType::kTypeInt
		   || tvChunkSize.intValue!=chunkSize) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		int chunkTop=(int)inContext.SS.size()-1;
		if(chunkTop-4<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
		TypedValue& tvAlpha3=inContext.SS.at(chunkTop-4);
		if(tvAlpha3.dataType==DataType::kTypeAddress) {
			TypedValue tvJumpHere=inContext.MarkHere();
			int alpha3=tvAlpha3.intValue;
			if(alpha3<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
			inContext.Compile(alpha3,tvJumpHere);
		}
		inContext.Compile(std::string("dup"));
		NEXT;
	}));

	Install(new Word("default",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);;
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=DataType::kTypeCB
		   || tvSyntax.intValue!=(int)ControlBlockType::kSyntax_SWITCH) {
			return inContext.Error(NoParamErrorID::E_SYNTAX_MISMATCH_SWITCH);
		}
		TypedValue& tvChunkSize=ReadSecond(inContext.SS);
		const int chunkSize=5;
		if(tvChunkSize.dataType!=DataType::kTypeInt
		   || tvChunkSize.intValue!=chunkSize) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		int chunkTop=(int)inContext.SS.size()-1;
		if(chunkTop-4<0) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		TypedValue& tvAlpha3=inContext.SS.at(chunkTop-4);
		if(tvAlpha3.dataType==DataType::kTypeAddress) {
			TypedValue tvJumpHere=inContext.MarkHere();
			int alpha3=tvAlpha3.intValue;
			if(alpha3<0) {
				return inContext.Error(NoParamErrorID::E_SS_BROKEN);
			}
			inContext.Compile(alpha3,tvJumpHere);
		}
		inContext.SS.at(chunkTop-4)=TypedValue();	// invalid value
		NEXT;
	}));

	Install(new Word("->",WordLevel::Immediate,WORD_FUNC {
		if(inContext.Exec(std::string("->>"))==false) { return false; }
		inContext.Compile(std::string("drop"));
		NEXT;
	}));

	Install(new Word("->>",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);;
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=DataType::kTypeCB
		   || tvSyntax.intValue!=(int)ControlBlockType::kSyntax_SWITCH) {
			return inContext.Error(NoParamErrorID::E_SYNTAX_MISMATCH_SWITCH);
		}
		TypedValue& tvChunkSize=ReadSecond(inContext.SS);
		const int chunkSize=5;
		if(tvChunkSize.dataType!=DataType::kTypeInt
		   || tvChunkSize.intValue!=chunkSize) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		int chunkTop=(int)inContext.SS.size()-1;
		if(chunkTop-4<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }

		inContext.Compile(std::string("_branch-if-false"));
		TypedValue tvNewAlpha3=inContext.MarkAndCreateEmptySlot();
		inContext.SS.at(chunkTop-4)=tvNewAlpha3;
		NEXT;
	}));

	Install(new Word("break",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);
		}
		int chunkTop=inContext.GetChunkIndex((int)ControlBlockType::kOPEN_SWITCH_GROUP);
		TypedValue& tvAlpha1=inContext.SS.at(chunkTop-2);
		inContext.Compile(std::string("_absolute-jump"));
		inContext.Compile(tvAlpha1);
		NEXT;
	}));

	Install(new Word("dispatch",WordLevel::Immediate,WORD_FUNC {
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.SS);
		if(tvSyntax.dataType!=DataType::kTypeCB
		   || tvSyntax.intValue!=(int)ControlBlockType::kSyntax_SWITCH) {
			return inContext.Error(NoParamErrorID::E_SYNTAX_MISMATCH_SWITCH);
		}
		TypedValue& tvChunkSize=ReadSecond(inContext.SS);
		const int chunkSize=5;
		if(tvChunkSize.dataType!=DataType::kTypeInt
		   || tvChunkSize.intValue!=chunkSize) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		int chunkTop=(int)inContext.SS.size()-1;
		if(chunkTop-4<0) { inContext.Error(NoParamErrorID::E_SS_BROKEN); }
		TypedValue& tvAlpha2=inContext.SS.at(chunkTop-3);
		if(tvAlpha2.dataType!=DataType::kTypeAddress || tvAlpha2.intValue<0) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		TypedValue& tvAlpha3=inContext.SS.at(chunkTop-4);
		TypedValue tvAddressOfEndOfSwitchBlock=inContext.MarkHere();
		if(tvAlpha3.dataType==DataType::kTypeAddress) {
			int alpha3=tvAlpha3.intValue;
			if(alpha3<0) { return inContext.Error(NoParamErrorID::E_SS_BROKEN); }
			inContext.Compile(alpha3,tvAddressOfEndOfSwitchBlock);
		}
		inContext.Compile(tvAlpha2.intValue,tvAddressOfEndOfSwitchBlock);

		if(inContext.RemoveTopChunk()==false) {
			return inContext.Error(NoParamErrorID::E_SS_BROKEN);
		}
		if(inContext.EndControlBlock()==false) { return false; }
		NEXT;
	}));
}

static int getStandardLoopChunkTop(Context& inContext) {
	const int chunkSize=7;
	if(inContext.CheckCompileMode()==false) {
		inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);
		goto onError;
	}
	if(inContext.SS.size()<chunkSize) {
		inContext.Error(NoParamErrorID::E_SS_BROKEN);
		goto onError;
	}

	{
		int i=(int)inContext.SS.size()-1;
		while(i>=0) {
			TypedValue& tvSyntax=inContext.SS[i];
			if(tvSyntax.dataType!=DataType::kTypeCB) {
				inContext.Error(NoParamErrorID::E_SS_BROKEN);
				goto onError;
			}
			if((tvSyntax.intValue & (int)ControlBlockType::kOPEN_LEAVABLE_LOOP_GROUP)==0) {
				if(i<=0) {
					inContext.Error(NoParamErrorID::E_SS_BROKEN);
					goto onError;
				} else {
					TypedValue& tvChunkSize=inContext.SS[i-1];
					if(tvChunkSize.dataType!=DataType::kTypeInt) {
						inContext.Error(NoParamErrorID::E_SS_BROKEN);
						goto onError;
					}
					i-=tvChunkSize.intValue;
					continue;	
				}
			} else {
				break;	// found standard loop chunkTop.
			}
		}
		if(i<chunkSize-1) {
			inContext.Error(NoParamErrorID::E_SYNTAX_MISMATCH_LEAVABLE_LOOP_GROUP);
			goto onError;
		} else {
			int chunkTop=i;
			TypedValue& tvChunkSize=inContext.SS.at(chunkTop-1);
			if(tvChunkSize.dataType!=DataType::kTypeInt) {
				inContext.Error(NoParamErrorID::E_SS_BROKEN);
				goto onError;
			}
			if(tvChunkSize.intValue!=chunkSize) {
				inContext.Error(NoParamErrorID::E_SS_BROKEN);
				goto onError;
			}
			return chunkTop;
		}
	}

onError:
	return -1;
}

