#include <thread>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"
#include "util.h"

static bool gRunningOnInteractive=false;

static bool doLVOP(Context& inContext,LVOP inLVOP);

void RunningOnInteractive() {
	gRunningOnInteractive=true;
}

void InitDict_LangSys() {
	G_NumOfCores = std::thread::hardware_concurrency();

	Install(new Word("_lvop",WORD_FUNC {
		TypedValue *tv=(TypedValue *)(inContext.ip+1);
		if(tv->dataType!=DataType::kTypeLVOP) {
			return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		}
		LVOP lvop=static_cast<LVOP>(tv->intValue);
		if(doLVOP(inContext,lvop)==false) {
			tv->Dump();
			return inContext.Error(NoParamErrorID::E_ILLEGAL_LVOP);
		}
		inContext.ip+=TvSize;
		NEXT;
	}));

	Install(new Word("_nLVOP",WORD_FUNC {
		TypedValue *tvBuf=(TypedValue *)(inContext.ip+1);
		TypedValue& tvN=tvBuf[0];
		if(tvN.dataType!=DataType::kTypeInt) {
			return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		}
		const int n=tvN.intValue;
		for(int i=0; i<n; i++) {
			TypedValue& tvLVOP=tvBuf[1+i];
			if(tvLVOP.dataType!=DataType::kTypeLVOP) {
				return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
			}
			LVOP lvop=static_cast<LVOP>(tvLVOP.intValue);
			if(doLVOP(inContext,lvop)==false) {
				tvLVOP.Dump();
				return inContext.Error(NoParamErrorID::E_ILLEGAL_LVOP);
			}
		}
		inContext.ip+=TvSize*(n+1);
		NEXT;
	}));

	Install(new Word("args",WORD_FUNC {
		inContext.DS.emplace_back(G_ARGS);
		NEXT;
	}));

	Install(new Word("windows?",WORD_FUNC {
#ifdef _MSC_VER
		inContext.DS.emplace_back(true);
#else
		inContext.DS.emplace_back(false);
#endif
		NEXT;
	}));

	Install(new Word("mac?",WORD_FUNC {
#ifdef FOR_MACOS
		inContext.DS.emplace_back(true);
#else
		inContext.DS.emplace_back(false);
#endif
		NEXT;
	}));

	Install(new Word("linux?",WORD_FUNC {
#ifdef FOR_LINUX
		inContext.DS.emplace_back(true);
#else
		inContext.DS.emplace_back(false);
#endif
		NEXT;
	}));

	Install(new Word("interactive?",WORD_FUNC {
		inContext.DS.emplace_back(gRunningOnInteractive);
		NEXT;
	}));

	Install(new Word("show-trace",WORD_FUNC {
		if(inContext.IS.size()<1) {
			printf("trace info is empty.\n");
		} else {
			ShowTrace(inContext);
		}
		NEXT;
	}));

	Install(new Word("tron",WORD_FUNC {
		SetTraceOn();
		NEXT;
	}));

	Install(new Word("troff",WORD_FUNC {
		SetTraceOff();
		NEXT;
	}));

	Install(new Word("debug",WORD_FUNC {
		SetOptimizeLevel(0);
		NEXT;
	}));

	Install(new Word("/*",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInCppStyleComment() ) { NEXT; }
		inContext.EnterLevel2(ControlBlockType::kOPEN_C_STYLE_COMMENT);
		NEXT;
	}));

	Install(new Word("*/",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInCppStyleComment() ) { NEXT; }
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_C_STYLE_COMMENT_MISMATCH);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.RS);
		if(tvSyntax.dataType!=DataType::kTypeMiscInt
		  || tvSyntax.intValue!=(int)ControlBlockType::kOPEN_C_STYLE_COMMENT) {
			return inContext.Error(NoParamErrorID::E_C_STYLE_COMMENT_MISMATCH);
		}
		Pop(inContext.RS);

		if(inContext.LeaveLevel2()==false) { return false; }
		NEXT;
	}));

	Install(new Word("//",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInCStyleComment() )	  { NEXT; }
		if( inContext.IsInCppStyleComment() ) { NEXT; }
		inContext.PushThreshold();
		inContext.PushNewWord();
		inContext.RS.emplace_back(DataType::kTypeMiscInt,
								  ControlBlockType::kOPEN_CPP_STYLE_ONE_LINE_COMMENT);
		inContext.newWord=new Word(WordType::Normal);
		inContext.ExecutionThreshold=Level::kSymbol;
		NEXT;
	}));

	Install(new Word(EOL_WORD,WordLevel::Level2,WORD_FUNC {
		if(inContext.RS.size()<1) { NEXT; }
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType==DataType::kTypeMiscInt) {
			switch((ControlBlockType)rsTos.intValue) {
				case ControlBlockType::kOPEN_CPP_STYLE_ONE_LINE_COMMENT: {
						delete inContext.newWord->tmpParam;
						delete inContext.newWord;
						if(inContext.DS.size()<2) {
							return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
						}
						TypedValue tvNW=Pop(inContext.DS);
						if(tvNW.dataType!=DataType::kTypeNewWord) {
							return inContext.Error(InvalidTypeErrorID::E_TOS_NEW_WORD,tvNW);
						}
			 			inContext.newWord=(Word *)tvNW.wordPtr;

						Pop(inContext.RS);	// already checked by ReadTOS(inContext.RS).

						TypedValue tvThreshold=Pop(inContext.DS);
						if(tvThreshold.dataType!=DataType::kTypeThreshold) {
							return inContext.Error(InvalidTypeErrorID::E_SECOND_THRESHOLD,tvThreshold);
						}
						inContext.ExecutionThreshold=(Level)tvThreshold.intValue;
					}
					break;
				case ControlBlockType::kOPEN_HERE_DOCUMENT_RAW: {
						bool isSuccess;
						inContext.hereDocStr+=EvalEscapeSeq(inContext.line,&isSuccess);
						if( inContext.appendNewlineForHereDocStr ) {
#ifdef FOR_MACOS
							inContext.hereDocStr+="\n";
#elif FOR_LINUX
							inContext.hereDocStr+="\n";
#elif _MSC_VER
							inContext.hereDocStr+="\r\n";
#else
	#error BUILD CONFIGURATION ERROR target device symbol should be FOR_MACOS, FOR_LINUX, _MSC_VER.
#endif
						} else {
							inContext.appendNewlineForHereDocStr=true;
						}
					}
					break;
				case ControlBlockType::kOPEN_HERE_DOCUMENT_DEDENT: {
						std::string s=SkipWhiteSpace(inContext.line);
						bool isSuccess;
						inContext.hereDocStr+=EvalEscapeSeq(s,&isSuccess);
						if( inContext.appendNewlineForHereDocStr ) {
#ifdef FOR_MACOS
							inContext.hereDocStr+="\n";
#elif FOR_LINUX
							inContext.hereDocStr+="\n";
#elif _MSC_VER
							inContext.hereDocStr+="\r\n";
#else
	#error BUILD CONFIGURATION ERROR target device symbol should be FOR_MACOS, FOR_LINUX, _MSC_VER.
#endif
						} else {
							inContext.appendNewlineForHereDocStr=true;
						}
					}
					break;
				default:
					;	// do nothing
			}
		}
		NEXT;
	}));
}

#define LVMathOp(inContext,OP) do { \
	LocalVarSlot& localVarSlot=inContext.Env.back(); \
	int src1=static_cast<int>((inLVOP & LVOP::src1Mask)>>8); \
	TypedValue& tv1=localVarSlot[src1]; \
	int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4); \
	TypedValue& tv2=localVarSlot[src2]; \
	int dest=static_cast<int>((inLVOP & LVOP::destMask)); \
	MathOp(localVarSlot[dest],tv1,OP,tv2); \
} while(0)

#define LVMathOpPush(inContext,OP) do { \
	LocalVarSlot& localVarSlot=inContext.Env.back(); \
	int src1=static_cast<int>((inLVOP & LVOP::src1Mask)>>8); \
	TypedValue& tv1=localVarSlot[src1]; \
	int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4); \
	TypedValue& tv2=localVarSlot[src2]; \
	TypedValue result; \
	MathOp(result,tv1,OP,tv2); \
	LVOP dest=inLVOP & LVOP::destMask; \
	if(dest==LVOP::dDS) { \
		inContext.DS.emplace_back(result); \
	} else if(dest==LVOP::dRS) { \
		inContext.RS.emplace_back(result); \
	} else { \
		return false; \
	} \
} while(0)

#define LVMathPopOp(inContext,OP) do { \
	LocalVarSlot& localVarSlot=inContext.Env.back(); \
	int stack=static_cast<int>((inLVOP & LVOP::src1Mask)>>8); \
	TypedValue tv1=Pop( ((LVOP)stack)==LVOP::sDS ? inContext.DS : inContext.RS); \
	int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4); \
	TypedValue& tv2=localVarSlot[src2]; \
	int dest=static_cast<int>((inLVOP & LVOP::destMask)); \
	MathOp(localVarSlot[dest],tv1,OP,tv2); \
} while(0)

// outDest=srcTV OP
//	ex: outDest=srcTV +1
//		SetLocalVarWithSingleOP(localVarSlot[dest],tv,+1);
#define SetLocalVarWithSingleOP(outDest,srcTV,OP) do { \
	switch(srcTV.dataType) { \
		case DataType::kTypeInt: \
			outDest=TypedValue(srcTV.intValue OP); \
			break; \
		case DataType::kTypeLong: \
			outDest=TypedValue(srcTV.longValue OP); \
			break; \
		case DataType::kTypeBigInt:	\
			outDest=TypedValue(*(srcTV.bigIntPtr) OP); \
			break; \
		case DataType::kTypeFloat: \
			outDest=TypedValue(srcTV.floatValue OP); \
			break; \
		case DataType::kTypeDouble: \
			outDest=TypedValue(srcTV.doubleValue OP); \
			break; \
		case DataType::kTypeBigFloat: \
			outDest=TypedValue(*(srcTV.bigFloatPtr) OP); \
			break; \
		default: fprintf(stderr,"SYSTEM ERROR\n"); exit(-1); \
	} \
} while(0)

// Push(srcTV OP)
// 	ex: DS.Push(srcTV +1)
// 		OpThenPush(inContext.DS,srvTV,+1)
#define OpThenPush(inStack,srcTV,OP) do { \
	switch(srcTV.dataType) { \
		case DataType::kTypeInt: \
			inStack.emplace_back(srcTV.intValue OP); \
			break; \
		case DataType::kTypeLong: \
			inStack.emplace_back(srcTV.longValue OP); \
			break; \
		case DataType::kTypeBigInt: \
			inStack.emplace_back(*(srcTV.bigIntPtr) OP); \
			break; \
		case DataType::kTypeFloat: \
			inStack.emplace_back(srcTV.floatValue OP); \
			break; \
		case DataType::kTypeDouble: \
			inStack.emplace_back(srcTV.doubleValue OP); \
			break; \
		case DataType::kTypeBigFloat: \
			inStack.emplace_back(*(srcTV.bigFloatPtr) OP); \
			break; \
		default: fprintf(stderr,"SYSTEM ERROR\n"); exit(-1); \
	} \
} while(0)

static bool doLVOP(Context& inContext,LVOP inLVOP) {
	switch(inLVOP & LVOP::opMask) {
		case LVOP::NOP:	return true;
		case LVOP::ADD:	LVMathOp(inContext,+);  break;
		case LVOP::SUB:	LVMathOp(inContext,-);  break;
		case LVOP::MUL:	LVMathOp(inContext,*);  break;
		case LVOP::DIV:	LVMathOp(inContext,/);  break;
		case LVOP::MOD:	{
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src1=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv1=localVarSlot[src1];
				int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4);
				TypedValue& tv2=localVarSlot[src2];
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				ModOp(localVarSlot[dest],tv1,tv2);
			}
			break;
		case LVOP::INC: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				if(src==dest) {
					TypedValue& lv=localVarSlot[src];
					switch(lv.dataType) {
						case DataType::kTypeInt:		lv.intValue++;			break;
						case DataType::kTypeLong:		lv.longValue++;			break;
						case DataType::kTypeBigInt:		(*lv.bigIntPtr)++;		break;
						case DataType::kTypeFloat:		lv.floatValue++;		break;
						case DataType::kTypeDouble:		lv.doubleValue++;		break;
						case DataType::kTypeBigFloat:	(*lv.bigFloatPtr)++;	break;
						default:
							return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
					}
				} else {
					TypedValue& tv=localVarSlot[src];
					SetLocalVarWithSingleOP(localVarSlot[dest],tv,+1);
				}
			}
			break;
		case LVOP::DEC: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src];
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				SetLocalVarWithSingleOP(localVarSlot[dest],tv,-1);
			}
			break;
		case LVOP::TWC: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src];
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				SetLocalVarWithSingleOP(localVarSlot[dest],tv,*2);
			}
			break;
		case LVOP::HLF: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src];
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				SetLocalVarWithSingleOP(localVarSlot[dest],tv,/2);
			}
			break;
		case LVOP::PW2: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src];
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				switch(tv.dataType) {
					case DataType::kTypeInt:
						localVarSlot[dest]=TypedValue(tv.intValue*tv.intValue);
						break;
					case DataType::kTypeLong:
						localVarSlot[dest]=TypedValue(tv.longValue*tv.longValue);
						break;
					case DataType::kTypeBigInt:
						localVarSlot[dest]=TypedValue(*(tv.bigIntPtr)
													  * (*(tv.bigIntPtr)));
						break;
					case DataType::kTypeFloat:
						localVarSlot[dest]=TypedValue(tv.floatValue*tv.floatValue);
						break;
					case DataType::kTypeDouble:
						localVarSlot[dest]=TypedValue(tv.doubleValue*tv.doubleValue);
						break;
					case DataType::kTypeBigFloat:
						localVarSlot[dest]=TypedValue(*(tv.bigFloatPtr)
													  * (*(tv.bigFloatPtr)));
						break;
					default:
						fprintf(stderr,"SYSTEM ERROR\n");
						exit(-1);
				}
			}
			break;
		case LVOP::AddPush:	LVMathOpPush(inContext,+);  break;
		case LVOP::SubPush:	LVMathOpPush(inContext,-);  break;
		case LVOP::MulPush:	LVMathOpPush(inContext,*);  break;
		case LVOP::DivPush:	LVMathOpPush(inContext,/);  break;
		case LVOP::ModPush:	{
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src1=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv1=localVarSlot[src1];
				int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4);
				TypedValue& tv2=localVarSlot[src2];
				TypedValue result;
				ModOp(result,tv1,tv2);
				LVOP dest=inLVOP & LVOP::destMask;
				if(dest==LVOP::dDS) {
					inContext.DS.emplace_back(result);
				} else if(dest==LVOP::dRS) {
					inContext.RS.emplace_back(result); 
				} else {
					return false;
				} 
			}
			break;

		case LVOP::IncPush: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src];
				OpThenPush(inContext.DS,tv,+1);
			}
			break;
		case LVOP::DecPush: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src];
				OpThenPush(inContext.DS,tv,-1);
			}
			break;
		case LVOP::TwcPush: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src];
				OpThenPush(inContext.DS,tv,*2);
			}
			break;
		case LVOP::HlfPush: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src];
				OpThenPush(inContext.DS,tv,/2);
			}
			break;
		case LVOP::Pw2Push: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src];
				switch(tv.dataType) {
					case DataType::kTypeInt:
						inContext.DS.emplace_back(tv.intValue * tv.intValue);
						break;
					case DataType::kTypeLong:
						inContext.DS.emplace_back(tv.longValue * tv.longValue);
						break;
					case DataType::kTypeBigInt:
						inContext.DS.emplace_back(
							*(tv.bigIntPtr) * (*(tv.bigIntPtr)) );
						break;
					case DataType::kTypeFloat:
						inContext.DS.emplace_back(tv.floatValue * tv.floatValue);
						break;
					case DataType::kTypeDouble:
						inContext.DS.emplace_back(tv.doubleValue * tv.doubleValue);
						break;
					case DataType::kTypeBigFloat:
						inContext.DS.emplace_back(
							*(tv.bigFloatPtr) * (*(tv.bigFloatPtr)) );
						break;
					default:
						fprintf(stderr,"SYSTEM ERROR\n");
						exit(-1);
				}
			}
			break;

		case LVOP::PopAdd:	LVMathPopOp(inContext,+);	break;
		case LVOP::PopSub:	LVMathPopOp(inContext,-);	break;
		case LVOP::PopMul:	LVMathPopOp(inContext,*);	break;
		case LVOP::PopDiv:	LVMathPopOp(inContext,/);	break;
		case LVOP::PopMod: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				LVOP stack=inLVOP & LVOP::src1Mask;
				TypedValue tv1= stack==LVOP::sDS ? Pop(inContext.DS)
											   	 : Pop(inContext.RS);
				int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4);
				TypedValue& tv2=localVarSlot[src2];
				int dest=static_cast<int>(inLVOP & LVOP::destMask);
				ModOp(localVarSlot[dest],tv1,tv2);
			}
			break;
		default:
			fprintf(stderr,"SYSTEM ERROR\n");
			exit(-1);
	}
	return true;
}

