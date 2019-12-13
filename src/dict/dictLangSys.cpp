#include <thread>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"

static bool gRunningOnInteractive=false;

static bool doLVOP(Context& inContext,LVOP inLVOP);
static TypedValue getLiteralFromThreadedCode(Context& inContext);
static bool declareLocalVar(Context& inContext,const TypedValue& inTV);
static bool compileLet(Context& inContext,const TypedValue& inTV);

void RunningOnInteractive() {
	gRunningOnInteractive=true;
}

void InitDict_LangSys() {
	G_NumOfCores = std::thread::hardware_concurrency();

	// symbol ---
	// register var name symbol to symbol-table.
	// ex1: ... {{ `varName local }}} ...
	// ex2: ... {{ ( var1 var2 ... varN ) local }} ...
	Install(new Word("local",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=getLiteralFromThreadedCode(inContext);
		if(tv.dataType==kTypeInvalid) { return false; }
		if(declareLocalVar(inContext,tv)==false) { return false; }
		NEXT;
	}));	

	// X slotPos ---
	Install(new Word("_setValue",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeInt) { return inContext.Error(E_TOS_INT,tos); }
		TypedValue second=Pop(inContext.DS);
		if(inContext.SetCurrentLocalVar(tos.intValue,second)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,tos.intValue);
		}
		NEXT;
	}));

	// assign a value to a local variable
	// ex: 0 `count let
	Install(new Word("let",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=getLiteralFromThreadedCode(inContext);
		if(tv.dataType==kTypeInvalid) { return false; }
		if(compileLet(inContext,tv)==false) { return false; }
		NEXT;
	}));

	// equivalent to "dup local let".
	Install(new Word("local-with",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=getLiteralFromThreadedCode(inContext);
		if(tv.dataType==kTypeInvalid) { return false; }
		if(declareLocalVar(inContext,tv)==false) { return false; }
		if(compileLet(inContext,tv)==false) { return false; }
		NEXT;
	}));

	// slotPos --- X
	Install(new Word("_getValue",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeInt) { return inContext.Error(E_TOS_INT,tos); }
		if(inContext.GetLocalVarValue(tos.intValue,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,tos.intValue);
		}
		NEXT;
	}));

	// symbol --- X
	Install(new Word("_getValueByDynamic",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeSymbol) { return inContext.Error(E_TOS_SYMBOL,tos); }
		if(inContext.GetLocalVarValueByDynamic(*tos.stringPtr,inContext.DS)==false) {
			return inContext.Error(E_CAN_NOT_FIND_THE_LOCAL_VAR,*tos.stringPtr);
		}
		NEXT;
	}));

	Install(new Word(",",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=getLiteralFromThreadedCode(inContext);
		if(tv.dataType==kTypeInvalid) { return false; }
		if(tv.dataType==kTypeSymbol) {
			int slotPos=inContext.newWord->GetLocalVarSlotPos(*tv.stringPtr);
			if(slotPos>=0) {
				inContext.Compile(std::string("_lit"));
				inContext.Compile(slotPos);
				inContext.Compile(std::string("_getValue"));
			} else {
				inContext.Compile(std::string("_lit"));
				inContext.Compile(tv);
				inContext.Compile(std::string("_getValueByDynamic"));
			}
		} else { // the case of tos.dataType==kTypeList
			const int n=(int)tv.listPtr->size();
			for(int i=n-1; i>=0; i--) {
				TypedValue listElem=tv.listPtr->at(i);
				if(listElem.dataType!=kTypeSymbol) {
					return inContext.Error(E_TOS_SYMBOL_LIST);
				}
				int slotPos=inContext.newWord->GetLocalVarSlotPos(*listElem.stringPtr);
				if(slotPos>=0) {
					inContext.Compile(std::string("_lit"));
					inContext.Compile(slotPos);
					inContext.Compile(std::string("_getValue"));
				} else {
					inContext.Compile(std::string("_lit"));
					inContext.Compile(*listElem.stringPtr);
					inContext.Compile(std::string("_getValueByDynamic"));
				}
			}
		}
		NEXT;
	}));

	Install(new Word("show-local-vars",WORD_FUNC {
		if(inContext.Env.size()<1) {
			printf("no local vars.\n");
		} else {
			LocalVarSlot& localVarSlot=inContext.Env.back();
			const Word *word=*inContext.IS.back();
			const int numOfLocalVar=word->numOfLocalVar;
			printf("num of local vars: %d\n",numOfLocalVar);
			std::vector<std::string> lvInfo(numOfLocalVar);
			for(auto itr=word->localVarDict.begin();
			  itr!=word->localVarDict.end(); itr++) {
				lvInfo[itr->second]=itr->first;
	   		}
			for(int i=0; i<numOfLocalVar; i++) {
				printf("  slot #%02d: local var name: %s ",i,lvInfo[i].c_str());
				localVarSlot[i].Dump();
			}
		}
		NEXT;
	}));

	Install(new Word("_lvop",WORD_FUNC {
		TypedValue *tv=(TypedValue *)(inContext.ip+1);
		if(tv->dataType!=kTypeLVOP) { return inContext.Error(E_SYSTEM_ERROR); }
		LVOP lvop=static_cast<LVOP>(tv->intValue);
		if(doLVOP(inContext,lvop)==false) {
			tv->Dump();
			return inContext.Error(E_ILLEGAL_LVOP);
		}
		inContext.ip+=TvSize;
		NEXT;
	}));

	Install(new Word("_nLVOP",WORD_FUNC {
		TypedValue *tvBuf=(TypedValue *)(inContext.ip+1);
		TypedValue& tvN=tvBuf[0];
		if(tvN.dataType!=kTypeInt) { return inContext.Error(E_SYSTEM_ERROR); }
		const int n=tvN.intValue;
		for(int i=0; i<n; i++) {
			TypedValue& tvLVOP=tvBuf[1+i];
			if(tvLVOP.dataType!=kTypeLVOP) { return inContext.Error(E_SYSTEM_ERROR); }
			LVOP lvop=static_cast<LVOP>(tvLVOP.intValue);
			if(doLVOP(inContext,lvop)==false) {
				tvLVOP.Dump();
				return inContext.Error(E_ILLEGAL_LVOP);
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
		case kTypeInt:		outDest=TypedValue(srcTV.intValue OP); 		break; \
		case kTypeLong: 	outDest=TypedValue(srcTV.longValue OP);		break; \
		case kTypeBigInt:	outDest=TypedValue(*(srcTV.bigIntPtr) OP);	break; \
		case kTypeFloat: 	outDest=TypedValue(srcTV.floatValue OP); 	break; \
		case kTypeDouble: 	outDest=TypedValue(srcTV.doubleValue OP); 	break; \
		case kTypeBigFloat: outDest=TypedValue(*(srcTV.bigFloatPtr) OP);break; \
		default: fprintf(stderr,"SYSTEM ERROR\n"); exit(-1); \
	} \
} while(0)

// Push(srcTV OP)
// 	ex: DS.Push(srcTV +1)
// 		OpThenPush(inContext.DS,srvTV,+1)
#define OpThenPush(inStack,srcTV,OP) do { \
	switch(srcTV.dataType) { \
		case kTypeInt:		inStack.emplace_back(srcTV.intValue OP); 		break; \
		case kTypeLong: 	inStack.emplace_back(srcTV.longValue OP);		break; \
		case kTypeBigInt:	inStack.emplace_back(*(srcTV.bigIntPtr) OP);	break; \
		case kTypeFloat: 	inStack.emplace_back(srcTV.floatValue OP); 		break; \
		case kTypeDouble: 	inStack.emplace_back(srcTV.doubleValue OP); 	break; \
		case kTypeBigFloat: inStack.emplace_back(*(srcTV.bigFloatPtr) OP);	break; \
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
						case kTypeInt:		lv.intValue++;			break;
						case kTypeLong:		lv.longValue++;			break;
						case kTypeBigInt:	(*lv.bigIntPtr)++;		break;
						case kTypeFloat:	lv.floatValue++;		break;
						case kTypeDouble:	lv.doubleValue++;		break;
						case kTypeBigFloat:	(*lv.bigFloatPtr)++;	break;
						default:
							return inContext.Error(E_SYSTEM_ERROR);
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
					case kTypeInt:
						localVarSlot[dest]=TypedValue(tv.intValue*tv.intValue);
						break;
					case kTypeLong:
						localVarSlot[dest]=TypedValue(tv.longValue*tv.longValue);
						break;
					case kTypeBigInt:
						localVarSlot[dest]=TypedValue(*(tv.bigIntPtr)
													  * (*(tv.bigIntPtr)));
						break;
					case kTypeFloat:
						localVarSlot[dest]=TypedValue(tv.floatValue*tv.floatValue);
						break;
					case kTypeDouble:
						localVarSlot[dest]=TypedValue(tv.doubleValue*tv.doubleValue);
						break;
					case kTypeBigFloat:
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
					case kTypeInt:
						inContext.DS.emplace_back(tv.intValue * tv.intValue);
						break;
					case kTypeLong:
						inContext.DS.emplace_back(tv.longValue * tv.longValue);
						break;
					case kTypeBigInt:
						inContext.DS.emplace_back(
							*(tv.bigIntPtr) * (*(tv.bigIntPtr)) );
						break;
					case kTypeFloat:
						inContext.DS.emplace_back(tv.floatValue * tv.floatValue);
						break;
					case kTypeDouble:
						inContext.DS.emplace_back(tv.doubleValue * tv.doubleValue);
						break;
					case kTypeBigFloat:
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

static TypedValue getLiteralFromThreadedCode(Context& inContext) {
	if(inContext.newWord==NULL) {
		inContext.Error(E_SHOULD_BE_EXECUTED_IN_DEFINITION);
		return TypedValue();
	}
	if(inContext.newWord->tmpParam==NULL) {
		inContext.Error(E_SYSTEM_ERROR);
		exit(-1);
	}
	CodeThread *thread=inContext.newWord->tmpParam;
	int n=(int)thread->size();
	if(n<1) {
		inContext.Error(E_SYSTEM_ERROR);
		exit(-1);
	}
	if((thread->at(n-2).dataType!=kTypeWord
		&& thread->at(n-2).dataType!=kTypeDirectWord)
	  || thread->at(n-2).wordPtr->longName!="std:_lit"
	  || (thread->at(n-1).dataType!=kTypeSymbol
		  && thread->at(n-1).dataType!=kTypeList)) {
		inContext.Error(E_INVALID_USE);
		return TypedValue();
	}
	TypedValue tv=thread->at(n-1);
	thread->pop_back();
	thread->pop_back();
	return tv;
}

static bool declareLocalVar(Context& inContext,const TypedValue& inTV) {
	if(inTV.dataType==kTypeSymbol) {
		if(inContext.newWord->RegisterLocalVar(*inTV.stringPtr)==false) {
			return inContext.Error(E_ALREADY_REGISTERED_LOCAL_VAR,*inTV.stringPtr);
		}
	} else { // the case of tv.dataType==kTypeList
		const size_t n=inTV.listPtr->size();
		for(size_t i=0; i<n; i++) {
			TypedValue& listElem=inTV.listPtr->at(i);
			if(listElem.dataType!=kTypeSymbol) {
				return inContext.Error(E_TOS_SYMBOL_LIST);
			}
			if(inContext.newWord->RegisterLocalVar(*listElem.stringPtr)==false) {
				return inContext.Error(E_ALREADY_REGISTERED_LOCAL_VAR,
									   *listElem.stringPtr);
			}
		}
	}
	return true;
}

static bool compileLet(Context& inContext,const TypedValue& inTV) {
	if(inTV.dataType==kTypeSymbol) {
		int slotPos=inContext.newWord->GetLocalVarSlotPos(*inTV.stringPtr);
		if(slotPos<0) {
			return inContext.Error(E_CAN_NOT_FIND_THE_LOCAL_VAR,*inTV.stringPtr);
		}
		inContext.Compile(std::string("_lit"));
		inContext.Compile(slotPos);
		inContext.Compile(std::string("_setValue"));
	} else { // the case of tos.dataType==kTypeList
		const int n=(int)inTV.listPtr->size();
		for(int i=n-1; i>=0; i--) {
			TypedValue listElem=inTV.listPtr->at(i);
			if(listElem.dataType!=kTypeSymbol) {
				return inContext.Error(E_TOS_SYMBOL_LIST);
			}
			int slotPos=inContext.newWord->GetLocalVarSlotPos(*listElem.stringPtr);
			if(slotPos<0) {
				return inContext.Error(E_CAN_NOT_FIND_THE_LOCAL_VAR,
									   *listElem.stringPtr);
			}
			inContext.Compile(std::string("_lit"));
			inContext.Compile(slotPos);
			inContext.Compile(std::string("_setValue"));
		}
	}
	return true;
}

