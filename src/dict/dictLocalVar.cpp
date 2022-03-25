#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"
#include "util.h"

static bool declareLocalVar(Context& inContext,const TypedValue& inTV);
static bool mapLetBody(Context& inContext,const TypedValue& inTvLocalVarList,
					   const char *inWordNameAtErrorMessage);
static bool compileLet(Context& inContext,const TypedValue& inTV);
static bool compileIncOrDec(Context& inContext,const char *inIncOrDecWordBodyName);
static bool assignOpMain(Context& inContext,const char *inRuntimeWord);
static TypedValue getSymbolForAssignOp(Context& inContext);
static int checkForAssignOpRuntime(Context& inContext);

void InitDict_LocalVar() {
	// symbol ---
	// register var name symbol to symbol-table.
	// ex1: ... {{ `varName local-decl }} ...
	// ex2: ... {{ ( var1 var2 ... varN ) local-decl }} ...
	Install(new Word("local-decl",WordLevel::Immediate,WORD_FUNC {
		if(inContext.IsInterpretMode()) {
			return inContext.Error(NoParamErrorID::LocalVarUseOnlyWordDef);
		}
		TypedValue tv=inContext.GetLiteralFromThreadedCode();
		if( tv.IsInvalid() ) { return false; }
		if(declareLocalVar(inContext,tv)==false) { return false; }
		NEXT;
	}));	

	// X slotPos ---
	Install(new Word("_setValue",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
		}
		TypedValue second=Pop(inContext.DS);
		if(inContext.SetCurrentLocalVar(tos.intValue,second)==false) {
			return inContext.Error(ErrorIdWithInt::InvalidLocalSlotIndex,tos.intValue);
		}
		NEXT;
	}));

	// X slotPos --- X
	Install(new Word("_@setValue",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
		}
		TypedValue& second=ReadTOS(inContext.DS);
		if(inContext.SetCurrentLocalVar(tos.intValue,second)==false) {
			return inContext.Error(ErrorIdWithInt::InvalidLocalSlotIndex,tos.intValue);
		}
		NEXT;
	}));

	// assign a value to a local variable
	// ex: 0 `count let
	// X S ---
	Install(new Word("let",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=inContext.GetLiteralFromThreadedCode();
		if(tv.dataType==DataType::Invalid) { return false; }
		if(compileLet(inContext,tv)==false) { return false; }
		NEXT;
	}));

	Install(new Word("map-let",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=inContext.GetLiteralFromThreadedCode();
		if(tv.dataType==DataType::Invalid) { return false; }
		if(tv.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::CompiledLiteralShouldBeList,tv);
		}
		if(mapLetBody(inContext,tv,"map-let")==false) { return false; }
		NEXT;
	}));

	// equivalent to "dup local-decl let".
	Install(new Word("local",WordLevel::Immediate,WORD_FUNC {
		if(inContext.IsInterpretMode()) {
			return inContext.Error(NoParamErrorID::LocalVarUseOnlyWordDef);
		}
		TypedValue tv=inContext.GetLiteralFromThreadedCode();
		if(tv.dataType==DataType::Invalid) { return false; }
		if(declareLocalVar(inContext,tv)==false) { return false; }
		if(compileLet(inContext,tv)==false) { return false; }
		NEXT;
	}));

	Install(new Word("map-local",WordLevel::Immediate,WORD_FUNC {
		if(inContext.IsInterpretMode()) {
			return inContext.Error(NoParamErrorID::LocalVarUseOnlyWordDef);
		}
		TypedValue tv=inContext.GetLiteralFromThreadedCode();
		if(tv.dataType==DataType::Invalid) { return false; }
		if(tv.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::CompiledLiteralShouldBeList,tv);
		}
		if(declareLocalVar(inContext,tv)==false) { return false; }
		if(mapLetBody(inContext,tv,"map-local")==false) { return false; }
		NEXT;
	}));

	// slotPos --- X
	Install(new Word("_getValue",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
		}
		if(inContext.GetLocalVarValue(tos.intValue,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::InvalidLocalSlotIndex,tos.intValue);
		}
		NEXT;
	}));

	// symbol --- X
	Install(new Word("_getValueByDynamic",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Symbol) {
			return inContext.Error(InvalidTypeErrorID::TosSymbol,tos);
		}
		if(inContext.GetLocalVarValueByDynamic(*tos.stringPtr,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithString::CanNotFindTheLocalVar,*tos.stringPtr);
		}
		NEXT;
	}));

	Install(new Word(">param",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=inContext.GetLiteralFromThreadedCode(false);
		if(tv.dataType!=DataType::List) { return false; }
		// register local-vars
		int n=(int)tv.listPtr->size();
		bool hasOptional=false;
		for(int i=0; i<n; i++) {
			TypedValue tvVarName=tv.listPtr->at(i);
			if(tvVarName.dataType!=DataType::Symbol) { return false; }
			std::string& varName=*tvVarName.stringPtr;
			if(varName[0]!='(') {
				if(varName=="...") {
					if(i!=n-1) { return false; }	// ... should be placed last.
					if(declareLocalVar(inContext,tvVarName)==false) { return false; }
				} else if( hasOptional ) {
					return false;
				} else {
					if(declareLocalVar(inContext,tvVarName)==false) { return false; }
				}
			} else {
				hasOptional=true;
				if(varName.size()<3) { return false; }
				std::string s=varName.substr(1,varName.size()-2);
				if(declareLocalVar(inContext,
								   TypedValue(s,DataType::Symbol))==false) {
					return false;
				}
			}
		}
		inContext.Compile(std::string("_>param"));
		NEXT;
	}));

	Install(new Word("_>param",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tvVarInfo=Pop(inContext.DS);
		if(tvVarInfo.dataType!=DataType::List) {
			return inContext.Error(NoParamErrorID::SystemError);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		// bind a value into variable
		const Word *word=*inContext.IS.back();
		const LocalVarDict& localVarDict=word->localVarDict;
		int n=(int)tvVarInfo.listPtr->size();
		int valueSourceListSize=(int)tos.listPtr->size();
		bool hasOptional=false;
		for(int i=0; i<n; i++) {
			TypedValue tvVarName=tvVarInfo.listPtr->at(i);
			if(tvVarName.dataType!=DataType::Symbol) {
				return inContext.Error(NoParamErrorID::SystemError);
			}
			std::string& varName=*tvVarName.stringPtr;
			if(varName[0]!='(') {
				if(localVarDict.count(varName)<1) {
					return inContext.Error(NoParamErrorID::SystemError);
				}
				int slotPos=localVarDict.at(varName);
				if(slotPos<0) {
					return inContext.Error(NoParamErrorID::SystemError);
				}
				if(varName=="...") {
					if(i!=n-1) { // ... should be placed last.
						return inContext.Error(NoParamErrorID::SystemError);
					}
					List *restList=new List();
					if(i<valueSourceListSize) {
						for(int j=i; j<valueSourceListSize; j++) {
							restList->emplace_back(tos.listPtr->at(j));
						}
					}
					TypedValue tvRestList(restList);
					if(inContext.SetCurrentLocalVar(slotPos,tvRestList)==false) {
						return inContext.Error(ErrorIdWithInt::InvalidLocalSlotIndex,
											   slotPos);
					}
				} else if( hasOptional ) {
					return inContext.Error(NoParamErrorID::SystemError);
				} else {
					if(i>=valueSourceListSize) {
						return inContext.Error(ErrorIdWithString::NotEnoughParamValues,
											   varName);
					} else {
						if(inContext.SetCurrentLocalVar(slotPos,
														tos.listPtr->at(i))==false) {
							return inContext.Error(ErrorIdWithInt::InvalidLocalSlotIndex,slotPos);
						}
					}
				}
			} else {
				hasOptional=true;
				if(varName.size()<3) {
					return inContext.Error(NoParamErrorID::SystemError);
				}
				std::string s=varName.substr(1,varName.size()-2);
				int slotPos=localVarDict.at(s);
				if(slotPos<0) {
					return inContext.Error(NoParamErrorID::SystemError);
				}
				if(i>=valueSourceListSize) {
					TypedValue tvInvalid;
					if(inContext.SetCurrentLocalVar(slotPos,tvInvalid)==false) {
						return inContext.Error(ErrorIdWithInt::InvalidLocalSlotIndex,slotPos);
					}
				} else {
					if(inContext.SetCurrentLocalVar(slotPos,
													tos.listPtr->at(i))==false) {
						return inContext.Error(ErrorIdWithInt::InvalidLocalSlotIndex,slotPos);
					}
				}
			}
		}
		NEXT;
	}));

	Install(new Word("...",WordLevel::Immediate,WORD_FUNC {
		if(inContext.newWord==NULL) {
			inContext.Error(NoParamErrorID::ShouldBeExecutedInDefinition);
			return false;
		}
		int slotPos=inContext.newWord->GetLocalVarSlotPos("...");
		if(slotPos>=0) {
			inContext.Compile(std::string("_lit"));
			inContext.Compile(slotPos);
			inContext.Compile(std::string("_getValue"));
		} else {
			return false;
		}
		NEXT;
	}));

	Install(new Word(",",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=inContext.GetLiteralFromThreadedCode(true,false);
		if(tv.dataType==DataType::Invalid) {
			inContext.Compile(std::string("_getValueByDynamic"));
		} else if(tv.dataType==DataType::Symbol) {
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
		} else { // the case of tos.dataType==DataType::List
			const int n=(int)tv.listPtr->size();
			for(int i=n-1; i>=0; i--) {
				TypedValue listElem=tv.listPtr->at(i);
				if(listElem.dataType!=DataType::Symbol) {
					return inContext.Error(NoParamErrorID::TosSymbolList);
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

	Install(new Word("_incLocalVar",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
		}
		assert(inContext.Env.size()>1);
		const int slotPos=tos.intValue;
		if(slotPos<0) { return inContext.Error(NoParamErrorID::SystemError); }
		TypedValue& tvLocalVar=(inContext.Env.back())[slotPos];
		switch(tvLocalVar.dataType) {
			case DataType::Int: 	 tvLocalVar.intValue++;		  break;
			case DataType::Long: 	 tvLocalVar.longValue++;	  break;
			case DataType::Float: 	 tvLocalVar.floatValue++;	  break;
			case DataType::Double:	 tvLocalVar.doubleValue++;	  break;
			case DataType::BigInt:	 (*tvLocalVar.bigIntPtr)++;	  break;
			case DataType::BigFloat: (*tvLocalVar.bigFloatPtr)++; break;
			default:
				return inContext.Error(InvalidTypeErrorID::LvNumber,tvLocalVar);
		}
		NEXT;
	}));

	Install(new Word("_decLocalVar",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
		}
		assert(inContext.Env.size()>1);
		const int slotPos=tos.intValue;
		TypedValue& tvLocalVar=(inContext.Env.back())[slotPos];
		switch(tvLocalVar.dataType) {
			case DataType::Int: 	 tvLocalVar.intValue--;		  break;
			case DataType::Long: 	 tvLocalVar.longValue--;	  break;
			case DataType::Float: 	 tvLocalVar.floatValue--;	  break;
			case DataType::Double:	 tvLocalVar.doubleValue--;	  break;
			case DataType::BigInt:   (*tvLocalVar.bigIntPtr)--;	  break;
			case DataType::BigFloat: (*tvLocalVar.bigFloatPtr)--; break;
			default:
				return inContext.Error(InvalidTypeErrorID::LvNumber,tvLocalVar);
		}
		NEXT;
	}));

	Install(new Word("++",WordLevel::Immediate,WORD_FUNC {
		if(compileIncOrDec(inContext,"_incLocalVar")==false) { return false; }
		NEXT;
	}));

	Install(new Word("--",WordLevel::Immediate,WORD_FUNC {
		if(compileIncOrDec(inContext,"_decLocalVar")==false) { return false; }
		NEXT;
	}));

	Install(new Word("<-",WordLevel::Immediate,WORD_FUNC {
		if(inContext.newWord==NULL) {
			return inContext.Error(NoParamErrorID::ShouldBeExecutedInDefinition);
		}
		inContext.SS.emplace_back((int)inContext.newWord->tmpParam->size());
		NEXT;
	}));

	// number local-var-slot-no ---
	// ex) 10 loval-var-slot(`t) ---
	// 	t += 10
	Install(new Word("_swap+=",WORD_FUNC {
		int slotPos=checkForAssignOpRuntime(inContext);
		TypedValue& tvLocalVar=(inContext.Env.back())[slotPos];
		TypedValue tvDelta=Pop(inContext.DS);
		AssignOpToSecond(inContext,tvLocalVar,+,tvDelta);
		NEXT;
	}));

	// number local-var-slot-no ---
	// ex) 10 loval-var-slot(`t) ---
	// 	t -= 10
	Install(new Word("_swap-=",WORD_FUNC {
		int slotPos=checkForAssignOpRuntime(inContext);
		TypedValue& tvLocalVar=(inContext.Env.back())[slotPos];
		TypedValue tvDelta=Pop(inContext.DS);
		AssignOpToSecond(inContext,tvLocalVar,-,tvDelta);
		NEXT;
	}));

	Install(new Word("_swap*=",WORD_FUNC {
		int slotPos=checkForAssignOpRuntime(inContext);
		TypedValue& tvLocalVar=(inContext.Env.back())[slotPos];
		TypedValue tvScale=Pop(inContext.DS);
		AssignOpToSecond(inContext,tvLocalVar,*,tvScale);
		NEXT;
	}));

	Install(new Word("_swap/=",WORD_FUNC {
		int slotPos=checkForAssignOpRuntime(inContext);
		TypedValue& tvLocalVar=(inContext.Env.back())[slotPos];
		TypedValue tvDiv=Pop(inContext.DS);
		if( tvDiv.IsZero() ) {
			return inContext.Error(NoParamErrorID::TosShouldBeNonZero);			
		}
		AssignOpToSecond(inContext,tvLocalVar,/,tvDiv);
		NEXT;
	}));

	Install(new Word("_swap%=",WORD_FUNC {
		int slotPos=checkForAssignOpRuntime(inContext);
		TypedValue& tvLocalVar=(inContext.Env.back())[slotPos];
		TypedValue tvDiv=Pop(inContext.DS);
		if( tvDiv.IsZero() ) {
			return inContext.Error(NoParamErrorID::TosShouldBeNonZero);			
		}
		ModAssign(tvLocalVar,tvDiv);
		NEXT;
	}));

	Install(new Word("+=",WordLevel::Immediate,WORD_FUNC {
		if(assignOpMain(inContext,"_swap+=")==false) { return false; }
		NEXT;
	}));

	Install(new Word("-=",WordLevel::Immediate,WORD_FUNC {
		if(assignOpMain(inContext,"_swap-=")==false) { return false; }
		NEXT;
	}));

	Install(new Word("*=",WordLevel::Immediate,WORD_FUNC {
		if(assignOpMain(inContext,"_swap*=")==false) { return false; }
		NEXT;
	}));

	Install(new Word("/=",WordLevel::Immediate,WORD_FUNC {
		if(assignOpMain(inContext,"_swap/=")==false) { return false; }
		NEXT;
	}));

	Install(new Word("%=",WordLevel::Immediate,WORD_FUNC {
		if(assignOpMain(inContext,"_swap%=")==false) { return false; }
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
				printf("  slot #%02d: local var name: '%s' ",i,lvInfo[i].c_str());
				localVarSlot[i].Dump();
			}
		}
		NEXT;
	}));
}

static bool declareLocalVar(Context& inContext,const TypedValue& inTV) {
	if(inTV.dataType==DataType::Symbol) {
		if(inContext.newWord->RegisterLocalVar(*inTV.stringPtr)==false) {
			return inContext.Error(ErrorIdWithString::AlreadyRegisteredLocalVar,*inTV.stringPtr);
		}
	} else if(inTV.dataType==DataType::List) {
		const size_t n=inTV.listPtr->size();
		for(size_t i=0; i<n; i++) {
			TypedValue& listElem=inTV.listPtr->at(i);
			if(listElem.dataType!=DataType::Symbol) {
				return inContext.Error(NoParamErrorID::TosSymbolList);
			}
			if(inContext.newWord->RegisterLocalVar(*listElem.stringPtr)==false) {
				return inContext.Error(ErrorIdWithString::AlreadyRegisteredLocalVar,
									   *listElem.stringPtr);
			}
		}
	} else {
		return inContext.Error(InvalidTypeErrorID
							   ::CompiledLiteralShouldBeSymbolOrStringOrList,inTV);
	}
	return true;
}

static bool mapLetBody(Context& inContext,const TypedValue& inTvLocalVarList,
					   const char *inWordNameAtErrorMessage) {
	// @size numOfVar != if error then
	int numOfVar=(int)inTvLocalVarList.listPtr->size();
	inContext.Compile(std::string("@size"));
	inContext.Compile(std::string("_lit"));
	inContext.Compile(numOfVar);
	inContext.Compile(std::string("!="));
	inContext.Exec(std::string("if"));
	inContext.Compile(std::string("_lit"));
	inContext.Compile(TypedValue(std::string("ERROR (")
								 +std::string(inWordNameAtErrorMessage)
								 +std::string(") : list size mismatch.")));
	inContext.Compile(std::string(".cr"));
	inContext.Compile(std::string("panic"));
	inContext.Exec(std::string("then"));
	inContext.Compile(std::string("expand"));
	if(compileLet(inContext,inTvLocalVarList)==false) { return false; }
	return true;
}

static bool compileLet(Context& inContext,const TypedValue& inTV) {
	if(inTV.dataType==DataType::Symbol) {
		int slotPos=inContext.newWord->GetLocalVarSlotPos(*inTV.stringPtr);
		if(slotPos<0) {
			return inContext.Error(ErrorIdWithString::CanNotFindTheLocalVar,
								   *inTV.stringPtr);
		}
		inContext.Compile(std::string("_lit"));
		inContext.Compile(slotPos);
		inContext.Compile(std::string("_setValue"));
	} else { // the case of tos.dataType==DataType::List
		const int n=(int)inTV.listPtr->size();
		for(int i=n-1; i>=0; i--) {
			TypedValue listElem=inTV.listPtr->at(i);
			if(listElem.dataType!=DataType::Symbol) {
				return inContext.Error(NoParamErrorID::TosSymbolList);
			}
			int slotPos=inContext.newWord->GetLocalVarSlotPos(*listElem.stringPtr);
			if(slotPos<0) {
				return inContext.Error(ErrorIdWithString::CanNotFindTheLocalVar,
									   *listElem.stringPtr);
			}
			inContext.Compile(std::string("_lit"));
			inContext.Compile(slotPos);
			inContext.Compile(std::string("_setValue"));
		}
	}
	return true;
}

static bool compileIncOrDec(Context& inContext,const char *inIncOrDecWordBodyName) {
	TypedValue tv=inContext.GetLiteralFromThreadedCode();
	if(tv.dataType==DataType::Invalid) { return false; }
	if(tv.dataType==DataType::Symbol) {
		int slotPos=inContext.newWord->GetLocalVarSlotPos(*tv.stringPtr);
		if(slotPos<0) {
			return inContext.Error(ErrorIdWithString::CanNotFindTheLocalVar,*tv.stringPtr);
		}
		inContext.Compile(std::string("_lit"));
		inContext.Compile(slotPos);
		inContext.Compile(std::string(inIncOrDecWordBodyName));
	}
	return true;
}

static bool assignOpMain(Context& inContext,const char *inRuntimeWord) {
	TypedValue tvSymbol=getSymbolForAssignOp(inContext);
	if(tvSymbol.dataType!=DataType::Symbol) { return false; }
	int slotPos=inContext.newWord->GetLocalVarSlotPos(*tvSymbol.stringPtr);
	if(slotPos<0) {
		return inContext.Error(ErrorIdWithString::CanNotFindTheLocalVar,
							   *tvSymbol.stringPtr);
	}
	inContext.Compile(std::string("_lit"));
	inContext.Compile(slotPos);
	inContext.Compile(std::string(inRuntimeWord));
	return true;
}

static TypedValue getSymbolForAssignOp(Context& inContext) {
	if(inContext.SS.size()<1) {
		inContext.Error(NoParamErrorID::SsBroken); return TypedValue();
	}
	TypedValue tvSsTos=Pop(inContext.SS);
	if(tvSsTos.dataType!=DataType::Int) {
		inContext.Error(NoParamErrorID::SsBroken); return TypedValue();
	}
	int targetPos=tvSsTos.intValue;
	if(targetPos<2) { inContext.Error(NoParamErrorID::SsBroken); return TypedValue(); }
	if(inContext.newWord==NULL) {
		inContext.Error(NoParamErrorID::ShouldBeExecutedInDefinition);
		return TypedValue();
	}
	if(inContext.newWord->tmpParam==NULL) {
		inContext.Error(NoParamErrorID::SystemError);
		exit(-1);
	}

	CodeThread *thread=inContext.newWord->tmpParam;
	int n=(int)thread->size();
	if(n<1) {
		inContext.Error(NoParamErrorID::SystemError);
		exit(-1);
	}
	if(n<targetPos) { inContext.Error(NoParamErrorID::SsBroken); return TypedValue(); }
	if((thread->at(targetPos-2).dataType!=DataType::Word
		&& thread->at(targetPos-2).dataType!=DataType::DirectWord)
	  || thread->at(targetPos-2).wordPtr->longName!="std:_lit"
	  || thread->at(targetPos-1).dataType!=DataType::Symbol) {
		inContext.Error(NoParamErrorID::SsBroken); return TypedValue();
	}
	TypedValue ret=thread->at(targetPos-1);
	thread->at(targetPos-2)=TypedValue(GetWordPtr("std:_nop"));
	thread->at(targetPos-1)=TypedValue(GetWordPtr("std:_nop"));
	return ret;
}

static int checkForAssignOpRuntime(Context& inContext) {
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); }
	TypedValue tvLocalVarSlotNo=Pop(inContext.DS);
	if(tvLocalVarSlotNo.dataType!=DataType::Int) {
		inContext.Error(InvalidTypeErrorID::TosInt,tvLocalVarSlotNo);
		return -1;
	}
	assert(inContext.Env.size()>1);
	const int slotPos=tvLocalVarSlotNo.intValue;
	if(slotPos<0) {
		inContext.Error(NoParamErrorID::SystemError);
		return -1;
	}
	return slotPos;
}

