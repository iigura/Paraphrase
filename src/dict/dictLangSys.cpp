#include <thread>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"
#include "optimizer.h"
#include "util.h"

static bool gRunningOnInteractive=false;

static bool argBeginBody(Context& inContext);
static bool argEndBody(Context& inContext);

static bool doLVOP(Context& inContext,LVOP inLVOP);

void RunningOnInteractive() {
	gRunningOnInteractive=true;
}

void InitDict_LangSys() {
	G_NumOfCores = std::thread::hardware_concurrency();

	Install(new Word("_lvop",WORD_FUNC {
		TypedValue *tv=(TypedValue *)(inContext.ip+1);
		if(tv->dataType!=DataType::LVOP) {
			return inContext.Error(NoParamErrorID::SystemError);
		}
		LVOP lvop=static_cast<LVOP>(tv->intValue);
		if(doLVOP(inContext,lvop)==false) {
			tv->Dump();
			return inContext.Error(NoParamErrorID::IllegalLVOP);
		}
		inContext.ip+=TvSize;
		NEXT;
	}));

	Install(new Word("_nLVOP",WORD_FUNC {
		TypedValue *tvBuf=(TypedValue *)(inContext.ip+1);
		TypedValue& tvN=tvBuf[0];
		if(tvN.dataType!=DataType::Int) {
			return inContext.Error(NoParamErrorID::SystemError);
		}
		const int n=tvN.intValue;
		for(int i=0; i<n; i++) {
			TypedValue& tvLVOP=tvBuf[1+i];
			if(tvLVOP.dataType!=DataType::LVOP) {
				return inContext.Error(NoParamErrorID::SystemError);
			}
			LVOP lvop=static_cast<LVOP>(tvLVOP.intValue);
			if(doLVOP(inContext,lvop)==false) {
				tvLVOP.Dump();
				return inContext.Error(NoParamErrorID::IllegalLVOP);
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

	Install(new Word("tron?",WORD_FUNC {
		inContext.DS.emplace_back(IsTraceOn());
		NEXT;
	}));

	Install(new Word("debug",WORD_FUNC {
		SetOptimizeLevel(0);
		NEXT;
	}));

	Install(new Word("/*",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInCppStyleComment() ) { NEXT; }
		inContext.EnterLevel2(ControlBlockType::OpenCStyleComment);
		NEXT;
	}));

	Install(new Word("*/",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInCppStyleComment() ) { NEXT; }
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::CStyleCommentMismatch);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.RS);
		if(tvSyntax.dataType!=DataType::MiscInt
		  || tvSyntax.intValue!=(int)ControlBlockType::OpenCStyleComment) {
			return inContext.Error(NoParamErrorID::CStyleCommentMismatch);
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
		inContext.RS.emplace_back(DataType::MiscInt,
								  ControlBlockType::OpenCppStyleOneLineComment);
		inContext.newWord=new Word(WordType::Normal);
		inContext.ExecutionThreshold=Level::Symbol;
		NEXT;
	}));

	Install(new Word(EOL_WORD,WordLevel::Level2,WORD_FUNC {
		if(inContext.RS.size()<1) { NEXT; }
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType==DataType::MiscInt) {
			switch((ControlBlockType)rsTos.intValue) {
				case ControlBlockType::OpenCppStyleOneLineComment: {
						delete inContext.newWord->tmpParam;
						delete inContext.newWord;
						if(inContext.DS.size()<2) {
							return inContext.Error(NoParamErrorID::DsAtLeast2);
						}
						TypedValue tvNW=Pop(inContext.DS);
						if(tvNW.dataType!=DataType::NewWord) {
							return inContext.Error(InvalidTypeErrorID::TosNewWord,tvNW);
						}
			 			inContext.newWord=(Word *)tvNW.wordPtr;

						Pop(inContext.RS);	// already checked by ReadTOS(inContext.RS).

						TypedValue tvThreshold=Pop(inContext.DS);
						if(tvThreshold.dataType!=DataType::Threshold) {
							return inContext.Error(InvalidTypeErrorID::SecondThreshold,tvThreshold);
						}
						inContext.ExecutionThreshold=(Level)tvThreshold.intValue;
					}
					break;
				case ControlBlockType::OpenHereDocumentRaw: {
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
				case ControlBlockType::OpenHereDocumentDedent: {
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

	Install(new Word("_arg-begin",WORD_FUNC {
		if(argBeginBody(inContext)==false) { return false; }
		NEXT;
	}));
	Install(new Word("_arg-end",WORD_FUNC {
		if(argEndBody(inContext)==false) { return false; }
		NEXT;
	}));


	// usage: `func :( 1 2 );
	// 	this program equivalent to  1 2 func.
	// the word :( equivalent to  >word >r
	Install(new Word(":(",WordLevel::Immediate,WORD_FUNC {
		if( inContext.IsInterpretMode() ) {
			if(argBeginBody(inContext)==false) { return false; }			
		} else {
			TypedValue tv=inContext.GetLiteralFromThreadedCode();
			if(tv.dataType==DataType::Invalid) {
				inContext.newWord->CompileWord("_arg-begin");
			} else if(tv.dataType==DataType::Symbol || tv.dataType==DataType::String) {	
				auto iter=Dict.find(*tv.stringPtr);
				if(iter==Dict.end()) {
					return inContext.Error(ErrorIdWithString
										   ::CanNotFindTheWord,*tv.stringPtr);
				}
				inContext.RS.emplace_back(iter->second);
				inContext.SS.emplace_back(ControlBlockType::SyntaxArgBegin);
			} else {
				return inContext.Error(InvalidTypeErrorID
									   ::CompiledLiteralShouldBeSymbolOrString,tv);
			}
		}
		NEXT;
	}));

	// equivalent to  r> exec
	Install(new Word(");",WordLevel::Immediate,WORD_FUNC {
		if( inContext.IsInterpretMode() ) {
			if(argEndBody(inContext)==false) { return false; }			
		} else {
			if(inContext.SS.size()<1) {
				goto compileArgEnd;
			} else {
				TypedValue& ssTos=ReadTOS(inContext.SS);
				if(ssTos.dataType!=DataType::CB) { goto compileArgEnd; }
				if(ssTos.intValue!=(int)ControlBlockType::SyntaxArgBegin) {
					goto compileArgEnd;
				}
				inContext.SS.pop_back();
				TypedValue tvWord=Pop(inContext.RS);
				if(tvWord.HasWordPtr(NULL)==false) {
					return inContext.Error(NoParamErrorID::RsBroken);
				}
				inContext.newWord->CompileValue(tvWord);	
				goto finish;
			}
compileArgEnd:
			inContext.newWord->CompileWord("_arg-end");
		}
finish:
		NEXT;
	}));
}

static bool argBeginBody(Context& inContext) {
	if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
	TypedValue tos=Pop(inContext.DS);
	if( tos.HasWordPtr(NULL) ) {
		inContext.RS.emplace_back(tos);
	} else if(tos.dataType==DataType::Symbol || tos.dataType==DataType::String) {
		auto iter=Dict.find(*tos.stringPtr);
		if(iter==Dict.end()) {
			return inContext.Error(ErrorIdWithString
								   ::CanNotFindTheWord,*tos.stringPtr);
		}
		inContext.RS.emplace_back(iter->second);
	} else { return inContext.Error(InvalidTypeErrorID::TosSymbolOrWord,tos); }
	return true;
}
static bool argEndBody(Context& inContext) {
	if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::RsIsEmpty); }
	TypedValue rsTos=Pop(inContext.RS);
	if(rsTos.HasWordPtr(NULL)==false) {
		return inContext.Error(NoParamErrorID::RsBroken);
	}	
	if(inContext.Exec(rsTos)==false) { return false; }
	return true;
}

#define LVMathOp(inContext,OP) do { \
	LocalVarSlot& localVarSlot=inContext.Env.back(); \
	int src1=static_cast<int>((inLVOP & LVOP::src1Mask)>>8); \
	TypedValue& tv1=localVarSlot[src1].first; \
	int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4); \
	TypedValue& tv2=localVarSlot[src2].first; \
	int dest=static_cast<int>((inLVOP & LVOP::destMask)); \
	MathOp(localVarSlot[dest].first,tv1,OP,tv2); \
} while(0)

#define LVMathOpPush(inContext,OP) do { \
	LocalVarSlot& localVarSlot=inContext.Env.back(); \
	int src1=static_cast<int>((inLVOP & LVOP::src1Mask)>>8); \
	TypedValue& tv1=localVarSlot[src1].first; \
	int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4); \
	TypedValue& tv2=localVarSlot[src2].first; \
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
	TypedValue& tv2=localVarSlot[src2].first; \
	int dest=static_cast<int>((inLVOP & LVOP::destMask)); \
	MathOp(localVarSlot[dest].first,tv1,OP,tv2); \
} while(0)

// outDest=srcTV OP
//	ex: outDest=srcTV +1
//		SetLocalVarWithSingleOP(localVarSlot[dest],tv,+1);
#define SetLocalVarWithSingleOP(outDest,srcTV,OP) do { \
	switch(srcTV.dataType) { \
		case DataType::Int:      outDest=TypedValue(srcTV.intValue OP);       break; \
		case DataType::Long:     outDest=TypedValue(srcTV.longValue OP);      break; \
		case DataType::BigInt:   outDest=TypedValue(*(srcTV.bigIntPtr) OP);   break; \
		case DataType::Float:    outDest=TypedValue(srcTV.floatValue OP);     break; \
		case DataType::Double: 	 outDest=TypedValue(srcTV.doubleValue OP);    break; \
		default: fprintf(stderr,"SYSTEM ERROR\n"); exit(-1); \
	} \
} while(0)

// Push(srcTV OP)
// 	ex: DS.Push(srcTV +1)
// 		OpThenPush(inContext.DS,srvTV,+1)
#define OpThenPush(inStack,srcTV,OP) do { \
	switch(srcTV.dataType) { \
		case DataType::Int:  	inStack.emplace_back(srcTV.intValue OP);  	   break; \
		case DataType::Long: 	inStack.emplace_back(srcTV.longValue OP); 	   break; \
		case DataType::BigInt:  inStack.emplace_back(*(srcTV.bigIntPtr) OP);   break; \
		case DataType::Float: 	inStack.emplace_back(srcTV.floatValue OP);     break; \
		case DataType::Double:  inStack.emplace_back(srcTV.doubleValue OP);    break; \
		default: fprintf(stderr,"SYSTEM ERROR\n"); exit(-1); \
	} \
} while(0)

static bool doLVOP(Context& inContext,LVOP inLVOP) {
	switch(inLVOP & LVOP::opMask) {
		case LVOP::ADD:	LVMathOp(inContext,+);  break;
		case LVOP::SUB:	LVMathOp(inContext,-);  break;
		case LVOP::MUL:	LVMathOp(inContext,*);  break;
		case LVOP::DIV:	LVMathOp(inContext,/);  break;
		case LVOP::MOD:	{
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src1=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv1=localVarSlot[src1].first;
				int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4);
				TypedValue& tv2=localVarSlot[src2].first;
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				ModOp(localVarSlot[dest].first,tv1,tv2);
			}
			break;
		case LVOP::INC: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				if(src==dest) {
					TypedValue& lv=localVarSlot[src].first;
					switch(lv.dataType) {
						case DataType::Int:			lv.intValue++;			break;
						case DataType::Long:		lv.longValue++;			break;
						case DataType::BigInt:		(*lv.bigIntPtr)++;		break;
						case DataType::Float:		lv.floatValue++;		break;
						case DataType::Double:		lv.doubleValue++;		break;
						default:
							return inContext.Error(NoParamErrorID::SystemError);
					}
				} else {
					TypedValue& tv=localVarSlot[src].first;
					SetLocalVarWithSingleOP(localVarSlot[dest].first,tv,+1);
				}
			}
			break;
		case LVOP::DEC: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src].first;
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				SetLocalVarWithSingleOP(localVarSlot[dest].first,tv,-1);
			}
			break;
		case LVOP::TWC: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src].first;
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				SetLocalVarWithSingleOP(localVarSlot[dest].first,tv,*2);
			}
			break;
		case LVOP::HLF: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src].first;
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				SetLocalVarWithSingleOP(localVarSlot[dest].first,tv,/2);
			}
			break;
		case LVOP::PW2: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src].first;
				int dest=static_cast<int>((inLVOP & LVOP::destMask));
				switch(tv.dataType) {
					case DataType::Int:
						localVarSlot[dest].first=TypedValue(tv.intValue*tv.intValue);
						break;
					case DataType::Long:
						localVarSlot[dest].first=TypedValue(tv.longValue*tv.longValue);
						break;
					case DataType::BigInt:
						localVarSlot[dest].first=TypedValue(*(tv.bigIntPtr)
													  * (*(tv.bigIntPtr)));
						break;
					case DataType::Float:
						localVarSlot[dest].first=TypedValue(tv.floatValue*tv.floatValue);
						break;
					case DataType::Double:
						localVarSlot[dest].first=TypedValue(tv.doubleValue*tv.doubleValue);
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
				TypedValue& tv1=localVarSlot[src1].first;
				int src2=static_cast<int>((inLVOP & LVOP::src2Mask)>>4);
				TypedValue& tv2=localVarSlot[src2].first;
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
				TypedValue& tv=localVarSlot[src].first;
				OpThenPush(inContext.DS,tv,+1);
			}
			break;
		case LVOP::DecPush: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src].first;
				OpThenPush(inContext.DS,tv,-1);
			}
			break;
		case LVOP::TwcPush: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src].first;
				OpThenPush(inContext.DS,tv,*2);
			}
			break;
		case LVOP::HlfPush: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src].first;
				OpThenPush(inContext.DS,tv,/2);
			}
			break;
		case LVOP::Pw2Push: {
				LocalVarSlot& localVarSlot=inContext.Env.back(); 
				int src=static_cast<int>((inLVOP & LVOP::src1Mask)>>8);
				TypedValue& tv=localVarSlot[src].first;
				switch(tv.dataType) {
					case DataType::Int:
						inContext.DS.emplace_back(tv.intValue * tv.intValue);
						break;
					case DataType::Long:
						inContext.DS.emplace_back(tv.longValue * tv.longValue);
						break;
					case DataType::BigInt:
						inContext.DS.emplace_back(
							*(tv.bigIntPtr) * (*(tv.bigIntPtr)) );
						break;
					case DataType::Float:
						inContext.DS.emplace_back(tv.floatValue * tv.floatValue);
						break;
					case DataType::Double:
						inContext.DS.emplace_back(tv.doubleValue * tv.doubleValue);
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
				TypedValue& tv2=localVarSlot[src2].first;
				int dest=static_cast<int>(inLVOP & LVOP::destMask);
				ModOp(localVarSlot[dest].first,tv1,tv2);
			}
			break;
		case LVOP::NOP:	return true;
		default:
			fprintf(stderr,"SYSTEM ERROR\n");
			exit(-1);
	}
	return true;
}

