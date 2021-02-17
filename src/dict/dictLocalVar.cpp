#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"
#include "util.h"

static TypedValue getLiteralFromThreadedCode(Context& inContext,
											 bool inIsRemoveFromThread=true);
static bool declareLocalVar(Context& inContext,const TypedValue& inTV);
static bool compileLet(Context& inContext,const TypedValue& inTV);
static bool compileIncOrDec(Context& inContext,const char *inIncOrDecWordBodyName);

void InitDict_LocalVar() {
	// symbol ---
	// register var name symbol to symbol-table.
	// ex1: ... {{ `varName local-decl }} ...
	// ex2: ... {{ ( var1 var2 ... varN ) local-decl }} ...
	Install(new Word("local-decl",WordLevel::Immediate,WORD_FUNC {
		if(inContext.IsInterpretMode()) {
			
		} else {
			TypedValue tv=getLiteralFromThreadedCode(inContext);
			if(tv.dataType==DataType::kTypeInvalid) { return false; }
			if(declareLocalVar(inContext,tv)==false) { return false; }
		}
		NEXT;
	}));	

	// X slotPos ---
	Install(new Word("_setValue",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tos);
		}
		TypedValue second=Pop(inContext.DS);
		if(inContext.SetCurrentLocalVar(tos.intValue,second)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,tos.intValue);
		}
		NEXT;
	}));

	// assign a value to a local variable
	// ex: 0 `count let
	Install(new Word("let",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=getLiteralFromThreadedCode(inContext);
		if(tv.dataType==DataType::kTypeInvalid) { return false; }
		if(compileLet(inContext,tv)==false) { return false; }
		NEXT;
	}));

	// equivalent to "dup local-decl let".
	Install(new Word("local",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=getLiteralFromThreadedCode(inContext);
		if(tv.dataType==DataType::kTypeInvalid) { return false; }
		if(declareLocalVar(inContext,tv)==false) { return false; }
		if(compileLet(inContext,tv)==false) { return false; }
		NEXT;
	}));

	// slotPos --- X
	Install(new Word("_getValue",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tos);
		}
		if(inContext.GetLocalVarValue(tos.intValue,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,tos.intValue);
		}
		NEXT;
	}));

	// symbol --- X
	Install(new Word("_getValueByDynamic",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeSymbol) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_SYMBOL,tos);
		}
		if(inContext.GetLocalVarValueByDynamic(*tos.stringPtr,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_LOCAL_VAR,*tos.stringPtr);
		}
		NEXT;
	}));

	Install(new Word(">param",WordLevel::Immediate,WORD_FUNC {
		TypedValue tv=getLiteralFromThreadedCode(inContext,false);
		if(tv.dataType!=DataType::kTypeList) { return false; }
		// register local-vars
		int n=(int)tv.listPtr->size();
		bool hasOptional=false;
		for(int i=0; i<n; i++) {
			TypedValue tvVarName=tv.listPtr->at(i);
			if(tvVarName.dataType!=DataType::kTypeSymbol) { return false; }
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
								   TypedValue(s,DataType::kTypeSymbol))==false) {
					return false;
				}
			}
		}
		inContext.Compile(std::string("_>param"));
		NEXT;
	}));

	Install(new Word("_>param",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tvVarInfo=Pop(inContext.DS);
		if(tvVarInfo.dataType!=DataType::kTypeList) {
			return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeList) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_LIST,tos);
		}
		// bind a value into variable
		const Word *word=*inContext.IS.back();
		const LocalVarDict& localVarDict=word->localVarDict;
		int n=(int)tvVarInfo.listPtr->size();
		int valueSourceListSize=(int)tos.listPtr->size();
		bool hasOptional=false;
		for(int i=0; i<n; i++) {
			TypedValue tvVarName=tvVarInfo.listPtr->at(i);
			if(tvVarName.dataType!=DataType::kTypeSymbol) {
				return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
			}
			std::string& varName=*tvVarName.stringPtr;
			if(varName[0]!='(') {
				if(localVarDict.count(varName)<1) {
					return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				}
				int slotPos=localVarDict.at(varName);
				if(slotPos<0) {
					return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				}
				if(varName=="...") {
					if(i!=n-1) { // ... should be placed last.
						return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
					}
					List *restList=new List();
					if(i<valueSourceListSize) {
						for(int j=i; j<valueSourceListSize; j++) {
							restList->emplace_back(tos.listPtr->at(j));
						}
					}
					TypedValue tvRestList(restList);
					if(inContext.SetCurrentLocalVar(slotPos,tvRestList)==false) {
						return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotPos);
					}
				} else if( hasOptional ) {
					return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				} else {
					if(i>=valueSourceListSize) {
						return inContext.Error(ErrorIdWithString::E_NOT_ENOUGH_PARAM_VALUES,varName);
					} else {
						if(inContext.SetCurrentLocalVar(slotPos,
														tos.listPtr->at(i))==false) {
							return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotPos);
						}
					}
				}
			} else {
				hasOptional=true;
				if(varName.size()<3) {
					return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				}
				std::string s=varName.substr(1,varName.size()-2);
				int slotPos=localVarDict.at(s);
				if(slotPos<0) {
					return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				}
				if(i>=valueSourceListSize) {
					TypedValue tvInvalid;
					if(inContext.SetCurrentLocalVar(slotPos,tvInvalid)==false) {
						return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotPos);
					}
				} else {
					if(inContext.SetCurrentLocalVar(slotPos,
													tos.listPtr->at(i))==false) {
						return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotPos);
					}
				}
			}
		}
		NEXT;
	}));

	Install(new Word("...",WordLevel::Immediate,WORD_FUNC {
		if(inContext.newWord==NULL) {
			inContext.Error(NoParamErrorID::E_SHOULD_BE_EXECUTED_IN_DEFINITION);
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
		TypedValue tv=getLiteralFromThreadedCode(inContext);
		if(tv.dataType==DataType::kTypeInvalid) { return false; }
		if(tv.dataType==DataType::kTypeSymbol) {
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
				if(listElem.dataType!=DataType::kTypeSymbol) {
					return inContext.Error(NoParamErrorID::E_TOS_SYMBOL_LIST);
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
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tos);
		}
		assert(inContext.Env.size()>1);
		const int slotPos=tos.intValue;
		TypedValue& tvLocalVar=(inContext.Env.back())[slotPos];
		switch(tvLocalVar.dataType) {
			case DataType::kTypeInt:
				tvLocalVar.intValue++;
				break;
			case DataType::kTypeLong:
				tvLocalVar.longValue++;
				break;
			case DataType::kTypeFloat:
				tvLocalVar.floatValue++;
				break;
			case DataType::kTypeDouble:
				tvLocalVar.doubleValue++;
				break;
			case DataType::kTypeBigInt:
				(*tvLocalVar.bigIntPtr)++;
				break;
			case DataType::kTypeBigFloat:
				(*tvLocalVar.bigFloatPtr)++;
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_LV_NUMBER,tvLocalVar);
		}
		NEXT;
	}));

	Install(new Word("_decLocalVar",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tos);
		}
		assert(inContext.Env.size()>1);
		const int slotPos=tos.intValue;
		TypedValue& tvLocalVar=(inContext.Env.back())[slotPos];
		switch(tvLocalVar.dataType) {
			case DataType::kTypeInt:
				tvLocalVar.intValue--;
				break;
			case DataType::kTypeLong:
				tvLocalVar.longValue--;
				break;
			case DataType::kTypeFloat:
				tvLocalVar.floatValue--;
				break;
			case DataType::kTypeDouble:
				tvLocalVar.doubleValue--;
				break;
			case DataType::kTypeBigInt:
				(*tvLocalVar.bigIntPtr)--;
				break;
			case DataType::kTypeBigFloat:
				(*tvLocalVar.bigFloatPtr)--;
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_LV_NUMBER,tvLocalVar);
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

static TypedValue getLiteralFromThreadedCode(Context& inContext,
											 bool inIsRemoveFromThread) {
	if(inContext.newWord==NULL) {
		inContext.Error(NoParamErrorID::E_SHOULD_BE_EXECUTED_IN_DEFINITION);
		return TypedValue();
	}
	if(inContext.newWord->tmpParam==NULL) {
		inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		exit(-1);
	}
	CodeThread *thread=inContext.newWord->tmpParam;
	int n=(int)thread->size();
	if(n<1) {
		inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		exit(-1);
	}
	if((thread->at(n-2).dataType!=DataType::kTypeWord
		&& thread->at(n-2).dataType!=DataType::kTypeDirectWord)
	  || thread->at(n-2).wordPtr->longName!="std:_lit"
	  || (thread->at(n-1).dataType!=DataType::kTypeSymbol
		  && thread->at(n-1).dataType!=DataType::kTypeList)) {
		inContext.Error(NoParamErrorID::E_INVALID_USE);
		return TypedValue();
	}
	TypedValue tv=thread->at(n-1);
	if( inIsRemoveFromThread ) {
		thread->pop_back();
		thread->pop_back();
	}
	return tv;
}

static bool declareLocalVar(Context& inContext,const TypedValue& inTV) {
	if(inTV.dataType==DataType::kTypeSymbol) {
		if(inContext.newWord->RegisterLocalVar(*inTV.stringPtr)==false) {
			return inContext.Error(ErrorIdWithString::E_ALREADY_REGISTERED_LOCAL_VAR,*inTV.stringPtr);
		}
	} else { // the case of tv.dataType==kTypeList
		const size_t n=inTV.listPtr->size();
		for(size_t i=0; i<n; i++) {
			TypedValue& listElem=inTV.listPtr->at(i);
			if(listElem.dataType!=DataType::kTypeSymbol) {
				return inContext.Error(NoParamErrorID::E_TOS_SYMBOL_LIST);
			}
			if(inContext.newWord->RegisterLocalVar(*listElem.stringPtr)==false) {
				return inContext.Error(ErrorIdWithString::E_ALREADY_REGISTERED_LOCAL_VAR,
									   *listElem.stringPtr);
			}
		}
	}
	return true;
}

static bool compileLet(Context& inContext,const TypedValue& inTV) {
	if(inTV.dataType==DataType::kTypeSymbol) {
		int slotPos=inContext.newWord->GetLocalVarSlotPos(*inTV.stringPtr);
		if(slotPos<0) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_LOCAL_VAR,*inTV.stringPtr);
		}
		inContext.Compile(std::string("_lit"));
		inContext.Compile(slotPos);
		inContext.Compile(std::string("_setValue"));
	} else { // the case of tos.dataType==kTypeList
		const int n=(int)inTV.listPtr->size();
		for(int i=n-1; i>=0; i--) {
			TypedValue listElem=inTV.listPtr->at(i);
			if(listElem.dataType!=DataType::kTypeSymbol) {
				return inContext.Error(NoParamErrorID::E_TOS_SYMBOL_LIST);
			}
			int slotPos=inContext.newWord->GetLocalVarSlotPos(*listElem.stringPtr);
			if(slotPos<0) {
				return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_LOCAL_VAR,
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
	TypedValue tv=getLiteralFromThreadedCode(inContext);
	if(tv.dataType==DataType::kTypeInvalid) { return false; }
	if(tv.dataType==DataType::kTypeSymbol) {
		int slotPos=inContext.newWord->GetLocalVarSlotPos(*tv.stringPtr);
		if(slotPos<0) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_LOCAL_VAR,*tv.stringPtr);
		}
		inContext.Compile(std::string("_lit"));
		inContext.Compile(slotPos);
		inContext.Compile(std::string(inIncOrDecWordBodyName));
	}
	return true;
}

