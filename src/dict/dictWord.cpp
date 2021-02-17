#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static bool colon(const char *inName,Context& inContext,bool inDefineShortend);
static bool compileLambda(Context& inContext,TypedValue& inLambda);
static bool constant(Context& inContext,
					 const std::string *inName,const TypedValue& inValue,
					 bool inOverwriteCheck);

void InitDict_Word() {
	Install(new Word("_lit",WORD_FUNC {
		TypedValue *tv=(TypedValue *)(inContext.ip+1);
		inContext.DS.emplace_back(*tv);
		inContext.ip+=TvSize;
		NEXT;
	}));

	Install(new Word("_litInt",WORD_FUNC {
		TypedValue *tv=(TypedValue *)(inContext.ip+1);
		inContext.DS.emplace_back(tv->intValue);
		inContext.ip+=TvSize;
		NEXT;
	}));

	Install(new Word("_litLong",WORD_FUNC {
		TypedValue *tv=(TypedValue *)(inContext.ip+1);
		inContext.DS.emplace_back(tv->longValue);
		inContext.ip+=TvSize;
		NEXT;
	}));

	Install(new Word("_litFloat",WORD_FUNC {
		TypedValue *tv=(TypedValue *)(inContext.ip+1);
		inContext.DS.emplace_back(tv->floatValue);
		inContext.ip+=TvSize;
		NEXT;
	}));

	Install(new Word("_litDouble",WORD_FUNC {
		TypedValue *tv=(TypedValue *)(inContext.ip+1);
		inContext.DS.emplace_back(tv->doubleValue);
		inContext.ip+=TvSize;
		NEXT;
	}));

	Install(new Word("_semis",WORD_FUNC {
		if(inContext.IS.size()<1) {
			return inContext.Error(NoParamErrorID::E_IS_BROKEN);
		}
		inContext.ip=inContext.IS.back();
		inContext.IS.pop_back();
		if((*inContext.ip)->numOfLocalVar>0) { inContext.Env.pop_back(); }
		NEXT;
	}));

	Install(new Word("return",Dict["_semis"]->code));
	
	Install(new Word(":",WORD_FUNC {
		if(colon(":",inContext,true)==false) { return false; }
		NEXT;
	}));

	Install(new Word("::",WORD_FUNC {
		if(colon("::",inContext,false)==false) { return false; }
		NEXT;
	}));

	Install(new Word(";",WordLevel::Immediate,WORD_FUNC {
		if(inContext.SS.size()>0) {
			TypedValue& tv=ReadTOS(inContext.SS);
			if(tv.dataType!=DataType::kTypeMiscInt) {
				return inContext.Error(InvalidTypeErrorID::E_SS_MISCINT,tv);
			}
			switch((ControlBlockType)tv.intValue) {
				case ControlBlockType::kSyntax_FOR_PLUS:
				case ControlBlockType::kSyntax_FOR_MINUS:
					inContext.Error(NoParamErrorID::E_SYNTAX_OPEN_FOR);
					goto onError;
				case ControlBlockType::kSyntax_WHILE:
				case ControlBlockType::kSyntax_IF:
					inContext.Error(NoParamErrorID::E_SYNTAX_OPEN_IF);
					goto onError;
				case ControlBlockType::kSyntax_SWITCH:
					inContext.Error(NoParamErrorID::E_SYNTAX_OPEN_SWITCH);
					goto onError;
				default:
					inContext.Error(NoParamErrorID::E_SS_MISCINT_INVALID);
					exit(-1);
			}
		}
		if(inContext.newWord==NULL) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_EXECUTED_IN_DEFINITION);
		}
		inContext.newWord->numOfLocalVar=(int)inContext.newWord->localVarDict.size();
		inContext.Compile(std::string("_semis"));
		inContext.FinishNewWord();
		inContext.SetInterpretMode();
		NEXT;
onError:
		inContext.SS.clear();
		return false;
	}));

	// string lambda --
	// or
	// lambda string --
	Install(new Word("def",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);

		std::string name;
		const Word *srcWord;
		if(tos.dataType==DataType::kTypeString
		   && (second.dataType==DataType::kTypeNewWord
			   || second.dataType==DataType::kTypeWord
			   || second.dataType==DataType::kTypeDirectWord)) {
			name=*tos.stringPtr;
			srcWord=second.wordPtr;
		} else if((tos.dataType==DataType::kTypeNewWord
				   || tos.dataType==DataType::kTypeWord
				   || tos.dataType==DataType::kTypeDirectWord)
				 && second.dataType==DataType::kTypeString) {
			name=*second.stringPtr;
			srcWord=tos.wordPtr;
		} else {
			return inContext.Error(InvalidTypeTosSecondErrorID::E_INVALID_DATA_TYPE_TOS_SECOND,tos,second);
		}
		Word *newWord=new Word(name.c_str(),srcWord->code);
		newWord->level=srcWord->level;
		newWord->isForgetable=srcWord->isForgetable;
		newWord->type=srcWord->type;
		newWord->numOfLocalVar=srcWord->numOfLocalVar;
		newWord->param=srcWord->param;
		newWord->tmpParam=srcWord->tmpParam;
		newWord->localVarDict=srcWord->localVarDict;
		newWord->LVOpHint=srcWord->LVOpHint;
		Install(newWord);
		NEXT;
	}));

	// string lambda --
	// or
	// lambda string --
	Install(new Word("update",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);

		std::string name;
		const Word *srcWord;
		if(tos.dataType==DataType::kTypeString
		   && (second.dataType==DataType::kTypeNewWord
			   || second.dataType==DataType::kTypeWord
			   || second.dataType==DataType::kTypeDirectWord)) {
			name=*tos.stringPtr;
			srcWord=second.wordPtr;
		} else if((tos.dataType==DataType::kTypeNewWord
				   || tos.dataType==DataType::kTypeWord
				   || tos.dataType==DataType::kTypeDirectWord)
				 && second.dataType==DataType::kTypeString) {
			name=*second.stringPtr;
			srcWord=tos.wordPtr;
		} else {
			return inContext.Error(InvalidTypeTosSecondErrorID::E_INVALID_DATA_TYPE_TOS_SECOND,tos,second);
		}

		auto iter=Dict.find(name);
		if(iter==Dict.end()) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_WORD,name);
		}

		Word *destWord=(Word *)iter->second;
		destWord->code=srcWord->code;
		destWord->numOfLocalVar=srcWord->numOfLocalVar;
		destWord->param=srcWord->param;
		destWord->tmpParam=srcWord->tmpParam;
		destWord->localVarDict=srcWord->localVarDict;
		destWord->LVOpHint=srcWord->LVOpHint;

		NEXT;
	}));
	

	// string ---
	Install(new Word("forget",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		auto iter=Dict.find(*tos.stringPtr);
		if(iter==Dict.end()) { 
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_WORD,*tos.stringPtr);
		}
		const Word *word=iter->second;
		Dict.erase(iter);
		auto iterByShort=Dict.find(word->shortName);
		auto iterByLong=Dict.find(word->longName);
		if(iterByShort==Dict.end() && iterByLong==Dict.end()) {
			delete word;
		}
		NEXT;
	}));

	Install(new Word("immediate",WORD_FUNC {
		if(inContext.lastDefinedWord==NULL) {
			return inContext.Error(NoParamErrorID::E_NO_LAST_DEFINED_WORD);
		}
		inContext.lastDefinedWord->level=WordLevel::Immediate;
		NEXT;
	}));

	Install(new Word("vocabulary",WORD_FUNC {
		inContext.DS.emplace_back(GetCurrentVocName());
		NEXT;
	}));

	Install(new Word("set-vocabulary",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		SetCurrentVocName(*tos.stringPtr.get());
		NEXT;
	}));

	// S(="wordName") --- S(="vocabularyName")
	// W --- S(="vocabularyName")
	Install(new Word("get-voc",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		const Word *word=NULL;
		if(tos.dataType==DataType::kTypeString) {
			auto iter=Dict.find(*tos.stringPtr);
			if(iter==Dict.end()) {
				return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_WORD,*tos.stringPtr);
			}
			word=iter->second;
		} else if(tos.dataType==DataType::kTypeWord) {
			word=tos.wordPtr;
		} else {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING_OR_WORD,tos);
		}
		inContext.DS.emplace_back(word->vocabulary);
		NEXT;
	}));
	
	// usage: "constName" value const
	// string value --- 
	Install(new Word("const",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvValue=Pop(inContext.DS);
		TypedValue tvName=Pop(inContext.DS);
		if(tvName.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,tvName);
		}
		const std::string *name=tvName.stringPtr.get();
		if(constant(inContext,name,tvValue,true)==false) { return false; }
		NEXT;	
	}));

	// usage: "constName" value const!
	// string value --- 
	Install(new Word("const!",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvValue=Pop(inContext.DS);
		TypedValue tvName=Pop(inContext.DS);
		if(tvName.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,tvName);
		}
		const std::string *name=tvName.stringPtr.get();
		if(constant(inContext,name,tvValue,false)==false) {
			return false;
		}
		NEXT;	
	}));

	// usage: enum-list enum
	// ex1: ( ( apple 100  )
	//        ( banana 101 )
	//        ( cherry 102 ) ) enum
	// equivalen to
	// ex2: ( ( apple 100 )
	//        banana
	//        cherry ) enum
	Install(new Word("enum",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::kTypeList) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_LIST,tvList);
		}
		const List *list=tvList.listPtr.get();
		const size_t n=list->size();	
		if(n==0) {
			return inContext.Error(NoParamErrorID::E_INVALID_ENUM_LIST);
		}
		bool autoValueMode=true;
		TypedValue tvAutoValue(0);
		for(size_t i=0; i<n; i++) {
			const TypedValue& tvElem=list->at(i);
			if(tvElem.dataType!=DataType::kTypeList) {
				if(autoValueMode==false || tvElem.dataType!=DataType::kTypeSymbol) {
					return inContext.Error(NoParamErrorID::E_INVALID_ENUM_LIST);
				}
			}
			const std::string *name=NULL;
			TypedValue tvValue;
			if(tvElem.dataType==DataType::kTypeList) {
				const List *tuple=tvElem.listPtr.get();
				if(tuple->size()!=2) {
					return inContext.Error(NoParamErrorID::E_INVALID_ENUM_LIST);
				}
				const TypedValue& tvName=tuple->at(0);
				if(tvName.dataType!=DataType::kTypeSymbol) {
					return inContext.Error(NoParamErrorID::E_INVALID_ENUM_LIST);
				}
				name=tvName.stringPtr.get();
				tvValue=tuple->at(1);
				if(tvValue.dataType==DataType::kTypeInt
				  || tvValue.dataType==DataType::kTypeBigInt) {
					tvAutoValue=tvValue;
				} else {
					tvAutoValue=TypedValue();
				}
			} else {
				assert(tvElem.dataType==DataType::kTypeSymbol);
				name=tvElem.stringPtr.get();
				tvValue=tvAutoValue;
			}
			if(constant(inContext,name,tvValue,true)==false) {
				return false;
			}
			if(tvAutoValue.dataType==DataType::kTypeInt) {
				tvAutoValue.intValue++;
			} else if(tvAutoValue.dataType==DataType::kTypeBigInt) {
				(*tvAutoValue.bigIntPtr)++;
			}
		}
		NEXT;
	}));

	// usage: "varName" var
	// string ---
	Install(new Word("var",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}

		auto iter=Dict.find(*tos.stringPtr);
		if(iter!=Dict.end()) {
			return inContext.Error(ErrorIdWithString::E_ALREADY_DEFINED,*tos.stringPtr);
		}

		Word *newWord=new Word(tos.stringPtr.get());

		const int paramSize=2+TvSize*2+MutexSize;
		newWord->param=new const Word*[paramSize];
		
		const Word *lit=GetWordPtr(std::string("_lit"));
		if(lit==NULL) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_WORD,std::string("_lit"));
		}
		const Word *semis=GetWordPtr(std::string("_semis"));
		if(semis==NULL) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_WORD,std::string("_semis"));
		}

		const int storageIndex=1+TvSize+1;
		const int mutexIndex=1+TvSize+1+TvSize;
		newWord->param[0]=lit;
		new((TypedValue*)(newWord->param+1)) TypedValue(DataType::kTypeParamDest,
														newWord->param+storageIndex);
		newWord->param[1+TvSize]=semis;
		new((TypedValue*)(newWord->param+storageIndex)) TypedValue();	// <- storage
		new ((Mutex *)(newWord->param+mutexIndex)) Mutex();
		initMutex(*((Mutex *)(newWord->param+mutexIndex))); 

		Dict[newWord->shortName]=newWord;
		Dict[newWord->longName]=newWord;

		// define utility words.
		Word *newWordBackup=inContext.newWord;
			// define >$varName (ex: >$x)
			std::string toVarName((">$"+*tos.stringPtr.get()).c_str());
			Word *toVar=new Word(&toVarName);
			inContext.newWord=toVar;
			inContext.Compile(std::string(*tos.stringPtr.get()));
			inContext.Compile(std::string("store"));
			inContext.Compile(std::string("_semis"));
			inContext.FinishNewWord();
			Dict[toVar->shortName]=toVar;
			Dict[toVar->longName ]=toVar;

			// dfine $varName> (ex: $x>)
			std::string fromVarName((("$"+*tos.stringPtr.get())+">").c_str());
			Word *fromVar=new Word(&fromVarName);
			inContext.newWord=fromVar;
			inContext.Compile(std::string(*tos.stringPtr.get()));
			inContext.Compile(std::string("fetch"));
			inContext.Compile(std::string("_semis"));
			inContext.FinishNewWord();
			Dict[fromVar->shortName]=fromVar;
			Dict[fromVar->longName ]=fromVar;
		inContext.newWord=newWordBackup;
		NEXT;
	}));

	// usage: value var store
	// ex:
	//	"x" var
	//	100 x store
	//
	// value address ---
	// note: same as Forth's '!'
	Install(new Word("store",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeParamDest) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_PARAMDEST,tos);
		}
		
		Mutex *mutex=(Mutex *)(((Word**)tos.ipValue)+TvSize);
		Lock(*mutex);
			TypedValue second=Pop(inContext.DS);
			new((TypedValue*)tos.ipValue) TypedValue(second);
		Unlock(*mutex);	
		NEXT;
	}));

	// address --- value
	// same as Forth's '@'.
	Install(new Word("fetch",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeParamDest) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_PARAMDEST,tos);
		}

		Mutex *mutex=(Mutex *)(((Word**)tos.ipValue)+TvSize);
		Lock(*mutex);
			inContext.DS.emplace_back(*(TypedValue*)tos.ipValue);
		Unlock(*mutex);
		NEXT;
	}));

	// left_curly
	Install(new Word("{",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		inContext.BeginNoNameWordBlock();
		NEXT;
	}));

	Install(new Word("}",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		inContext.newWord->numOfLocalVar=(int)inContext.newWord->localVarDict.size();
		inContext.Compile(std::string("_semis"));
		Word *newWordBackup=inContext.newWord;
		inContext.FinishNewWord();
		TypedValue tvWord(DataType::kTypeWord,newWordBackup);
		if(inContext.EndNoNameWordBlock()==false) { return false; }
		switch(inContext.ExecutionThreshold) {
			case Level::kInterpret:
				inContext.DS.emplace_back(tvWord);
				break;
			case Level::kCompile:
				assert(inContext.newWord!=NULL);
				inContext.Compile(std::string("_lit"));
				if(compileLambda(inContext,tvWord)==false) { return false; }
				break;
			case Level::kSymbol:
				assert(inContext.newWord!=NULL);
				if(compileLambda(inContext,tvWord)==false) { return false; }
				break;
			default:
				inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				exit(-1);
		}
		NEXT;
	}));

	Install(new Word("{{",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		inContext.RS.emplace_back((int)inContext.DS.size());
		inContext.BeginNoNameWordBlock();
		NEXT;
	}));

	Install(new Word("}}",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);
		}
		inContext.Compile(std::string("_semis"));
		Word *newWordBackup=inContext.newWord;
		inContext.FinishNewWord();
		TypedValue tvWord(DataType::kTypeWord,newWordBackup);
		if(inContext.EndNoNameWordBlock()==false) { return false; }
		if(inContext.Exec(tvWord)==false) { return false; }
		TypedValue tv=Pop(inContext.RS);
		if(tv.dataType!=DataType::kTypeInt) {
			return inContext.Error(NoParamErrorID::E_RS_BROKEN_TOS_SHOULD_BE_INT);
		}
		NEXT;
	}));

/*---
	Install(new Word("{{>",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		inContext.RS.emplace_back((int)inContext.DS.size()-1);
		inContext.BeginNoNameWordBlock();
		NEXT;
	}));
---*/

	Install(new Word("}},",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);
		}
		inContext.Compile(std::string("_semis"));
		Word *newWordBackup=inContext.newWord;
		inContext.FinishNewWord();
		TypedValue tvWord(DataType::kTypeWord,newWordBackup);
		if(inContext.EndNoNameWordBlock()==false) { return false; }
		if(inContext.Exec(tvWord)==false) { return false; }
		TypedValue tv=Pop(inContext.RS);
		if(tv.dataType!=DataType::kTypeInt) {
			return inContext.Error(NoParamErrorID::E_RS_BROKEN_TOS_SHOULD_BE_INT);
		}
		if(inContext.DS.size()>tv.intValue) {
			switch(inContext.ExecutionThreshold) {
				case Level::kInterpret:
					// do nothing
					break;
				case Level::kCompile:
					assert(inContext.newWord!=NULL);
					for(int i=tv.intValue; i<inContext.DS.size(); i++) {
						inContext.Compile(std::string("_lit"));
						inContext.Compile(inContext.DS[i]);
					}
					inContext.DS.erase(inContext.DS.begin()+tv.intValue,
									   inContext.DS.end());
					break;
				case Level::kSymbol:
					assert(inContext.newWord!=NULL);
					for(int i=tv.intValue; i<inContext.DS.size(); i++) {
						inContext.Compile(inContext.DS[i]);
					}
					inContext.DS.erase(inContext.DS.begin()+tv.intValue,
									   inContext.DS.end());
					break;
			default:
				inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
				exit(-1);
			}
		}
		NEXT;
	}));

	// (s -- wordPtr)
	Install(new Word(">word",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}

		const Word *word=GetWordPtr(*tos.stringPtr);
		if(word==NULL) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_WORD,*tos.stringPtr);
		}
		inContext.DS.emplace_back(DataType::kTypeWord,word);
		NEXT;
	}));

	Install(new Word(">lit",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.Compile(std::string("_lit"));
		inContext.Compile(tos);
		NEXT;
	}));

	Install(new Word(">here",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		if( inContext.IsInterpretMode() ) {
			return inContext.Error(NoParamErrorID::E_SHOULD_BE_COMPILE_MODE);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.Compile(tos);
		NEXT;
	}));

	// original-wordName aliaseName alias
	// ex: "dup" "case" alias
	Install(new Word("alias",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvSrcWordName=Pop(inContext.DS);
		if(tvSrcWordName.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,tvSrcWordName);
		}
		auto iter=Dict.find(*tvSrcWordName.stringPtr);
		if(iter==Dict.end()) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_WORD,*tvSrcWordName.stringPtr);
		}
		TypedValue tvNewWordName=Pop(inContext.DS);
		if(tvNewWordName.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tvNewWordName);
		}
		const Word *srcWord=iter->second;
		const char *newWordName=strdup(tvNewWordName.stringPtr.get()->c_str());
		Word *newWord=new Word(newWordName,srcWord->level,srcWord->code);
		Install(newWord);
		CopyWordProperty(newWord,srcWord);
		NEXT;
	}));

	// wordPtr --- 
	// or
	// S ---
	Install(new Word("dump",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeWord && tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_WP_OR_STRING,tos);
		}
		const Word *word=NULL;
		if(tos.dataType==DataType::kTypeWord) {
			word=tos.wordPtr;
		} else {
			auto iter=Dict.find(*tos.stringPtr);
			if(iter==Dict.end()) {
				return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_WORD,*tos.stringPtr);
			}
			word=iter->second;
		}
		//if(word->isForgetable==false) {
		if(word->param==NULL) {
			printf("the word '%s' is internal.\n",word->longName.c_str());
		} else {
			word->Dump();
		}
		NEXT;
	}));

	// S --- B
	Install(new Word("defined?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		inContext.DS.emplace_back(Dict.find(*tos.stringPtr)!=Dict.end());
		NEXT;
	}));

	// W --- W S
	Install(new Word("@word-name",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tvWord=ReadTOS(inContext.DS);
		if(tvWord.HasWordPtr()==false) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_WP,tvWord);
		}
		inContext.DS.emplace_back(tvWord.wordPtr->longName);
		NEXT;
	}));
	
	// W --- S
	Install(new Word("word-name",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr()==false) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_WP,tvWord);
		}
		inContext.DS.emplace_back(tvWord.wordPtr->longName);
		NEXT;
	}));

	// attribute

	// W Xk Xv ---
	// Xk : key for attribute
	// Xv : value for the key Xk
	// ex: "targetWord" >word "logLevel" 5 set-attr
	Install(new Word("set-attr",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_3);
		}
		TypedValue tvValue=Pop(inContext.DS);
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr()==false) {
			return inContext.Error(InvalidTypeErrorID::E_THIRD_WP,tvWord);
		}
		Word *word=(Word *)tvWord.wordPtr;
		if(word->attr==NULL) { word->attr=new Attr(); }
		auto itr=word->attr->find(tvKey);
		if(itr==word->attr->end()) {
			word->attr->insert({tvKey,tvValue});
		} else {
			itr->second=tvValue;
		}
		NEXT;
	}));

	// W Xk --- Xv
	// ex: "targetWord" >word "logLevel" get-attr
	Install(new Word("get-attr",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr()==false) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_WP,tvWord);
		}
		Word *word=(Word *)tvWord.wordPtr;
		if(word->attr==NULL) {
			return inContext.Error(InvalidValueErrorID::E_TOS_NO_SUCH_A_KEY,tvKey);
		}
		auto itr=word->attr->find(tvKey);
		if(itr==word->attr->end()) {
			return inContext.Error(InvalidValueErrorID::E_TOS_NO_SUCH_A_KEY,tvKey); 
		} else {
			inContext.DS.emplace_back(itr->second);
		}
		NEXT;	
	}));

	// W Xk --- 
	// ex: "targetWord" >word "logLevel" remove-attr
	Install(new Word("remove-attr",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr()==false) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_WP,tvWord);
		}
		Word *word=(Word *)tvWord.wordPtr;
		if(word->attr==NULL) {
			return inContext.Error(InvalidValueErrorID::E_TOS_NO_SUCH_A_KEY,tvKey);
		}
		size_t numOfErase=word->attr->erase(tvKey);
		if(numOfErase==0) {
			return inContext.Error(InvalidValueErrorID::E_TOS_NO_SUCH_A_KEY,tvKey); 
		}
		if(word->attr->size()==0) {
			delete word->attr;
			word->attr=NULL;
		}
		NEXT;
	}));
	
	// W Xk --- t/f
	// ex: "targetWord" >word "logLevel" has-attr?
	Install(new Word("has-attr?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr()==false) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_WP,tvWord);
		}
		Word *word=(Word *)tvWord.wordPtr;
		bool result;
		if(word->attr==NULL) {
			result=false;
		} else {
			size_t numOfCounts=word->attr->count(tvKey);
			result = numOfCounts!=0;
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	// W --- t/f
	// ex: "targetWord" >word has-any-attr?
	Install(new Word("has-any-attr?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr()==false) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_WP,tvWord);
		}
		Word *word=(Word *)tvWord.wordPtr;
		inContext.DS.emplace_back(word->attr != NULL);
		NEXT;				
	}));

	// W ---
	// ex: "targetWord" >word show-attr
	Install(new Word("show-attr",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr()==false) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_WP,tvWord);
		}
		Word *word=(Word *)tvWord.wordPtr;
		if(word->attr==NULL) {
			printf("no attribute.\n");
		} else {
			for(auto itr=word->attr->cbegin(); itr!=word->attr->cend(); ++itr) {
				putc('(',stdout);
				(*itr).first.PrintValue();
				putc(',',stdout);
				(*itr).second.PrintValue();
				puts(")");
			}
		}
		NEXT;
	}));
}

static bool colon(const char *inName,Context& inContext,bool inDefineShortend) {
	if(inContext.DS.size()<1) {
		return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
	}
	TypedValue tos=Pop(inContext.DS);
	if(tos.dataType!=DataType::kTypeString) {
		return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
	}
	if(tos.stringPtr->at(0)=='$') {
		return inContext.Error(NoParamErrorID::E_INVALID_WORD_NAME);
	}
	Word *newWord=new Word(tos.stringPtr.get());
	if(Install(newWord,inDefineShortend)==false) {
		delete newWord;
		inContext.newWord=NULL;
		return false;
	}
	newWord->isForgetable=true;
	inContext.newWord=newWord;
	inContext.SetCompileMode();
	return true;
}

static bool compileLambda(Context& inContext,TypedValue& inLambda) {
	if(inLambda.wordPtr->level==WordLevel::Immediate) {
		// the case of '${'
		if(inContext.Exec(inLambda)==false) { return false; }
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.Compile(tos);
	} else {
		inContext.Compile(inLambda);
	}
	return true;
}

static bool constant(Context& inContext,
					 const std::string *inName,const TypedValue& inValue,
					 bool inOverwriteCheck) {
	if( inOverwriteCheck ) {
		auto iter=Dict.find(*inName);
		if(iter!=Dict.end()) {
			return inContext.Error(ErrorIdWithString::E_ALREADY_DEFINED,*inName);
		}
	}

	Word *newWord=new Word(inName);
	Word *newWordBackup=inContext.newWord;
	inContext.newWord=newWord;
		inContext.Compile(std::string("_lit"));
		inContext.Compile(inValue);
		inContext.Compile(std::string("_semis"));
		inContext.FinishNewWord();
	inContext.newWord=newWordBackup;
	Dict[newWord->shortName]=newWord;
	Dict[newWord->longName ]=newWord;
	return true;
}

