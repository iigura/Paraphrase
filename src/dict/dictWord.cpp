#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static bool colon(Context& inContext,bool inDefineShortend);
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
		if(Semis(inContext)==false) { return false; }
		NEXT;
	}));

	Install(new Word("return",Dict["_semis"]->code));
	
	Install(new Word(":",WORD_FUNC {
		if(colon(inContext,true)==false) { return false; }
		NEXT;
	}));

	Install(new Word("::",WORD_FUNC {
		if(colon(inContext,false)==false) { return false; }
		NEXT;
	}));

	Install(new Word(";",WordLevel::Immediate,WORD_FUNC {
		if(inContext.SS.size()>0) {
			TypedValue& tv=ReadTOS(inContext.SS);
			if(tv.dataType!=DataType::MiscInt) {
				return inContext.Error(InvalidTypeErrorID::SsMiscInt,tv);
			}
			switch((ControlBlockType)tv.intValue) {
				case ControlBlockType::SyntaxForPlus:
				case ControlBlockType::SyntaxForMinus:
					inContext.Error(NoParamErrorID::SyntaxOpenFor);
					goto onError;
				case ControlBlockType::SyntaxWhile:
				case ControlBlockType::SyntaxIf:
					inContext.Error(NoParamErrorID::SyntaxOpenIf);
					goto onError;
				case ControlBlockType::SyntaxSwitch:
					inContext.Error(NoParamErrorID::SyntaxOpenSwitch);
					goto onError;
				default:
					inContext.Error(NoParamErrorID::SsMiscIntInvalid);
					exit(-1);
			}
		}
		if(inContext.newWord==NULL) {
			return inContext.Error(NoParamErrorID::ShouldBeExecutedInDefinition);
		}
		inContext.newWord->CompileWord("_semis");
		inContext.newWord->BuildParam();
		inContext.FinishWordDef();
		inContext.SetInterpretMode();
		NEXT;
onError:
		inContext.SS.clear();
		return false;
	}));

	// --- W
	Install(new Word("last-defined-word",WORD_FUNC {
		if(inContext.lastDefinedWord==NULL) {
			inContext.DS.emplace_back(TypedValue());
		} else {
			inContext.DS.emplace_back(TypedValue(DataType::Word,
												 inContext.lastDefinedWord));
		}
		NEXT;
	}));
	
	// string lambda --
	// or
	// lambda string --
	Install(new Word("def",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);

		std::string name;
		const Word *srcWord;
		std::shared_ptr<const Word> srcLambda(NULL);
		if((tos.dataType==DataType::String || tos.dataType==DataType::Symbol)
		   && second.HasWordPtr(&srcWord) ) {
			name=*tos.stringPtr;
			if(second.dataType==DataType::Lambda) { srcLambda=second.lambdaPtr; }
		} else if( tos.HasWordPtr(&srcWord)
				  && (second.dataType==DataType::String
				 	  || second.dataType==DataType::Symbol)) {
			name=*second.stringPtr;
			if(tos.dataType==DataType::Lambda) { srcLambda=tos.lambdaPtr; }
		} else {
			return inContext.Error(InvalidTypeTosSecondErrorID::BothDataInvalid,tos,second);
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
		if(srcLambda!=NULL) { newWord->paramSrcWordHolder=srcLambda; }
		Install(newWord);

		NEXT;
	}));

	// string lambda --
	// or
	// lambda string --
	Install(new Word("update",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);

		std::string name;
		const Word *srcWord;
		std::shared_ptr<const Word> srcLambda(NULL);
		if((tos.dataType==DataType::String || tos.dataType==DataType::Symbol)
		   && second.HasWordPtr(&srcWord) ) {
			name=*tos.stringPtr;
			if(second.dataType==DataType::Lambda) { srcLambda=second.lambdaPtr; }
		} else if(tos.HasWordPtr(&srcWord)
				  && (second.dataType==DataType::String
				 	  || second.dataType==DataType::Symbol)) {
			name=*second.stringPtr;
			if(tos.dataType==DataType::Lambda) { srcLambda=tos.lambdaPtr; }
		} else {
			return inContext.Error(InvalidTypeTosSecondErrorID::BothDataInvalid,tos,second);
		}

		auto iter=Dict.find(name);
		if(iter==Dict.end()) {
			return inContext.Error(ErrorIdWithString::CanNotFindTheWord,name);
		}

		Word *destWord=(Word *)iter->second;
		destWord->code=srcWord->code;
		destWord->numOfLocalVar=srcWord->numOfLocalVar;
		destWord->param=srcWord->param;
		destWord->tmpParam=srcWord->tmpParam;
		destWord->localVarDict=srcWord->localVarDict;
		destWord->LVOpHint=srcWord->LVOpHint;
		if(srcLambda!=NULL) { destWord->paramSrcWordHolder=srcLambda; }

		NEXT;
	}));
	
	Install(new Word("this-word-is-removed",WORD_FUNC {
		const Word *self=inContext.GetCurrentWord();
		const char *shortName=self->shortName.c_str();
		const char *longName =self->longName.c_str();
		fprintf(stderr,"this word ('%s' / '%s') is already removed.\n",shortName,longName);
		return false;
	}));

	Install(new Word("self",WORD_FUNC {
		if(inContext.IS.size()<1) {
			// the case of executed at top-level.
			inContext.DS.emplace_back(TypedValue());
		} else {
			const Word **caller=inContext.IS.back();
			const Word *self=*caller;
			inContext.DS.emplace_back(DataType::Word,self);
		}
		NEXT;
	}));

	// string ---
	Install(new Word("forget",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String && tos.dataType!=DataType::Symbol) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		auto iter=Dict.find(*tos.stringPtr);
		if(iter==Dict.end()) { 
			return inContext.Error(ErrorIdWithString::CanNotFindTheWord,*tos.stringPtr);
		}
		Word *word=(Word *)iter->second;
		Dict.erase(iter);
		if( CheckDeleteByForget() ) {
			auto iterByShort=Dict.find(word->shortName);
			auto iterByLong=Dict.find(word->longName);
			if(iterByShort==Dict.end() && iterByLong==Dict.end()) {
				word->code=Dict["this-word-is-removed"]->code;
			}
		}
		NEXT;
	}));
	Install(new Word("set-delete-by-forget",WORD_FUNC {
		SetDeleteByForget();
		NEXT;
	}));
	Install(new Word("clear-delete-by-forget",WORD_FUNC {
		ClearDeleteByForget();
		NEXT;
	}));
	Install(new Word("check-delete-by-forget",WORD_FUNC {
		inContext.DS.emplace_back(CheckDeleteByForget());
		NEXT;
	}));

	Install(new Word("immediate",WORD_FUNC {
		if(inContext.lastDefinedWord==NULL) {
			return inContext.Error(NoParamErrorID::NoLastDefinedWord);
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
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		SetCurrentVocName(*tos.stringPtr.get());
		NEXT;
	}));

	// S(="wordName") --- S(="vocabularyName")
	// W --- S(="vocabularyName")
	Install(new Word("get-voc",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tos=Pop(inContext.DS);
		const Word *word=NULL;
		if(tos.dataType==DataType::String) {
			auto iter=Dict.find(*tos.stringPtr);
			if(iter==Dict.end()) {
				return inContext.Error(ErrorIdWithString::CanNotFindTheWord,*tos.stringPtr);
			}
			word=iter->second;
		} else if(tos.dataType==DataType::Word) {
			word=tos.wordPtr;
		} else {
			return inContext.Error(InvalidTypeErrorID::TosStringOrWord,tos);
		}
		inContext.DS.emplace_back(word->vocabulary);
		NEXT;
	}));
	
	// usage: "constName" value const
	// string value --- 
	Install(new Word("const",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvValue=Pop(inContext.DS);
		TypedValue tvName=Pop(inContext.DS);
		if(tvName.dataType!=DataType::String && tvName.dataType!=DataType::Symbol) {
			return inContext.Error(InvalidTypeErrorID::SecondStringOrSymbol,tvName);
		}
		const std::string *name=tvName.stringPtr.get();
		if(constant(inContext,name,tvValue,true)==false) { return false; }
		NEXT;	
	}));

	// usage: "constName" value const!
	// string value --- 
	Install(new Word("const!",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvValue=Pop(inContext.DS);
		TypedValue tvName=Pop(inContext.DS);
		if(tvName.dataType!=DataType::String && tvName.dataType!=DataType::Symbol) {
			return inContext.Error(InvalidTypeErrorID::SecondStringOrSymbol,tvName);
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
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tvList);
		}
		const List *list=tvList.listPtr.get();
		const size_t n=list->size();	
		if(n==0) {
			return inContext.Error(NoParamErrorID::InvalidEnumList);
		}
		bool autoValueMode=true;
		TypedValue tvAutoValue(0);
		for(size_t i=0; i<n; i++) {
			const TypedValue& tvElem=list->at(i);
			if(tvElem.dataType!=DataType::List) {
				if(autoValueMode==false || tvElem.dataType!=DataType::Symbol) {
					return inContext.Error(NoParamErrorID::InvalidEnumList);
				}
			}
			const std::string *name=NULL;
			TypedValue tvValue;
			if(tvElem.dataType==DataType::List) {
				const List *tuple=tvElem.listPtr.get();
				if(tuple->size()!=2) {
					return inContext.Error(NoParamErrorID::InvalidEnumList);
				}
				const TypedValue& tvName=tuple->at(0);
				if(tvName.dataType!=DataType::Symbol) {
					return inContext.Error(NoParamErrorID::InvalidEnumList);
				}
				name=tvName.stringPtr.get();
				tvValue=tuple->at(1);
				if(tvValue.dataType==DataType::Int
				  || tvValue.dataType==DataType::BigInt) {
					tvAutoValue=tvValue;
				} else {
					tvAutoValue=TypedValue();
				}
			} else {
				assert(tvElem.dataType==DataType::Symbol);
				name=tvElem.stringPtr.get();
				tvValue=tvAutoValue;
			}
			if(constant(inContext,name,tvValue,true)==false) {
				return false;
			}
			if(tvAutoValue.dataType==DataType::Int) {
				tvAutoValue.intValue++;
			} else if(tvAutoValue.dataType==DataType::BigInt) {
				(*tvAutoValue.bigIntPtr)++;
			}
		}
		NEXT;
	}));

	// usage: "varName" var
	// string ---
	Install(new Word("var",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String && tos.dataType!=DataType::Symbol) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}

		auto iter=Dict.find(*tos.stringPtr);
		if(iter!=Dict.end()) {
			return inContext.Error(ErrorIdWithString::AlreadyDefined,*tos.stringPtr);
		}

		Word *newWord=new Word(tos.stringPtr.get());

		const int paramSize=2+TvSize*2+MutexSize;
		newWord->param=new const Word*[paramSize];
		
		const Word *lit=GetWordPtr("_lit");
		if(lit==NULL) {
			return inContext.Error(ErrorIdWithString::CanNotFindTheWord,
								   std::string("_lit"));
		}
		const Word *semis=GetWordPtr("_semis");
		if(semis==NULL) {
			return inContext.Error(ErrorIdWithString::CanNotFindTheWord,
								   std::string("_semis"));
		}

		const int storageIndex=1+TvSize+1;
		const int mutexIndex=1+TvSize+1+TvSize;
		newWord->param[0]=lit;
		new((TypedValue*)(newWord->param+1)) TypedValue(DataType::ParamDest,
														newWord->param+storageIndex);
		newWord->param[1+TvSize]=semis;
		new((TypedValue*)(newWord->param+storageIndex)) TypedValue();	// <- storage
		new ((Mutex *)(newWord->param+mutexIndex)) Mutex();
		initMutex(*((Mutex *)(newWord->param+mutexIndex))); 

		Dict[newWord->shortName]=newWord;
		Dict[newWord->longName ]=newWord;

		// define utility words.
		Word *newWordBackup=inContext.newWord;
			// define >$varName (ex: >$x)
			std::string toVarName((">$"+*tos.stringPtr.get()).c_str());
			Word *toVar=new Word(&toVarName);
			toVar->CompileWord(tos.stringPtr->c_str());
			toVar->CompileWord("store");
			toVar->CompileWord("_semis");
			toVar->BuildParam();
			inContext.newWord=toVar;
			inContext.FinishWordDef();
			Dict[toVar->shortName]=toVar;
			Dict[toVar->longName ]=toVar;

			// define $varName> (ex: $x>)
			std::string fromVarName((("$"+*tos.stringPtr.get())+">").c_str());
			Word *fromVar=new Word(&fromVarName);
			fromVar->CompileWord(tos.stringPtr->c_str());
			fromVar->CompileWord("fetch");
			fromVar->CompileWord("_semis");
			fromVar->BuildParam();
			inContext.newWord=fromVar;
			inContext.FinishWordDef();
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
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::ParamDest) {
			return inContext.Error(InvalidTypeErrorID::TosParamDest,tos);
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
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::ParamDest) {
			return inContext.Error(InvalidTypeErrorID::TosParamDest,tos);
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
		inContext.BeginNoNameWordBlock(ControlBlockType::NewSimpleLambda);
		NEXT;
	}));

	Install(new Word("}",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		inContext.newWord->CompileWord("_semis");
		inContext.newWord->BuildParam();
		inContext.FinishWordDef();
		TypedValue tvWord((std::shared_ptr<const Word>(inContext.lastDefinedWord)));
		if(inContext.EndNoNameWordBlock()==false) { return false; }
		switch(inContext.ExecutionThreshold) {
			case Level::Interpret:
				inContext.DS.emplace_back(tvWord);
				break;
			case Level::Compile:
				assert(inContext.newWord!=NULL);
				inContext.newWord->CompileWord("_lit");
				if(compileLambda(inContext,tvWord)==false) { return false; }
				break;
			case Level::Symbol:
				assert(inContext.newWord!=NULL);
				if(compileLambda(inContext,tvWord)==false) { return false; }
				break;
			default:
				inContext.Error(NoParamErrorID::SystemError);
				exit(-1);
		}
		NEXT;
	}));

	Install(new Word("{{",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		inContext.RS.emplace_back((int)inContext.DS.size());
		inContext.BeginNoNameWordBlock(ControlBlockType::NewSimpleLambda);
		NEXT;
	}));

	Install(new Word("}}",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::ShouldBeCompileMode);
		}
		inContext.newWord->CompileWord("_semis");
		inContext.newWord->BuildParam();
		inContext.FinishWordDef();
		TypedValue tvWord(DataType::Word,inContext.lastDefinedWord);
		if(inContext.EndNoNameWordBlock()==false) { return false; }
		if(inContext.Exec(tvWord)==false) { return false; }
		TypedValue tv=Pop(inContext.RS);
		if(tv.dataType!=DataType::Int) {
			return inContext.Error(NoParamErrorID::RsBrokenTosShouldBeInt);
		}
		NEXT;
	}));

	Install(new Word("}},",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		if(inContext.CheckCompileMode()==false) {
			return inContext.Error(NoParamErrorID::ShouldBeCompileMode);
		}
		inContext.newWord->CompileWord("_semis");
		inContext.newWord->BuildParam();
		inContext.FinishWordDef();
		TypedValue tvWord(DataType::Word,inContext.lastDefinedWord);
		if(inContext.EndNoNameWordBlock()==false) { return false; }
		if(inContext.Exec(tvWord)==false) { return false; }
		TypedValue tv=Pop(inContext.RS);
		if(tv.dataType!=DataType::Int) {
			return inContext.Error(NoParamErrorID::RsBrokenTosShouldBeInt);
		}
		if(inContext.DS.size()>tv.intValue) {
			switch(inContext.ExecutionThreshold) {
				case Level::Interpret:
					// do nothing
					break;
				case Level::Compile:
					assert(inContext.newWord!=NULL);
					for(int i=tv.intValue; i<inContext.DS.size(); i++) {
						inContext.newWord->CompileWord("_lit");
						inContext.newWord->CompileValue(inContext.DS[i]);
					}
					inContext.DS.erase(inContext.DS.begin()+tv.intValue,
									   inContext.DS.end());
					break;
				case Level::Symbol:
					assert(inContext.newWord!=NULL);
					for(int i=tv.intValue; i<inContext.DS.size(); i++) {
						inContext.newWord->CompileValue(inContext.DS[i]);
					}
					inContext.DS.erase(inContext.DS.begin()+tv.intValue,
									   inContext.DS.end());
					break;
			default:
				inContext.Error(NoParamErrorID::SystemError);
				exit(-1);
			}
		}
		NEXT;
	}));

	Install(new Word(">lit",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		inContext.newWord->CompileWord("_lit");
		inContext.newWord->CompileValue(tos);
		NEXT;
	}));

	Install(new Word(">here",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		if( inContext.IsInterpretMode() ) {
			return inContext.Error(NoParamErrorID::ShouldBeCompileMode);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.newWord->CompileValue(tos);
		NEXT;
	}));

	// original-wordName aliaseName alias
	Install(new Word("alias",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvSrcWordName=Pop(inContext.DS);
		if(tvSrcWordName.dataType!=DataType::String
		   && tvSrcWordName.dataType!=DataType::Symbol) {
			return inContext.Error(InvalidTypeErrorID::SecondStringOrSymbol,
								   tvSrcWordName);
		}
		auto iter=Dict.find(*tvSrcWordName.stringPtr);
		if(iter==Dict.end()) {
			return inContext.Error(ErrorIdWithString::CanNotFindTheWord,*tvSrcWordName.stringPtr);
		}
		TypedValue tvNewWordName=Pop(inContext.DS);
		if(tvNewWordName.dataType!=DataType::String
		   && tvNewWordName.dataType!=DataType::Symbol) {
			return inContext.Error(InvalidTypeErrorID::TosSymbolOrString,
								   tvNewWordName);
		}
		const Word *srcWord=iter->second;
		const char *newWordName=strdup(tvNewWordName.stringPtr.get()->c_str());
		Word *newWord=new Word(newWordName,srcWord->level,srcWord->code);
		Install(newWord);
		CopyWordProperty(newWord,srcWord);
		NEXT;
	}));

	// S --- B
	Install(new Word("defined?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String && tos.dataType!=DataType::Symbol) {
			return inContext.Error(InvalidTypeErrorID::TosSymbolOrString,tos);
		}
		inContext.DS.emplace_back(Dict.find(*tos.stringPtr)!=Dict.end());
		NEXT;
	}));

	// W --- W S
	Install(new Word("@word-name",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue& tvWord=ReadTOS(inContext.DS);
		const Word *wordPtr;
		if(tvWord.HasWordPtr(&wordPtr)==false) {
			return inContext.Error(InvalidTypeErrorID::TosWp,tvWord);
		}
		inContext.DS.emplace_back(wordPtr->longName);
		NEXT;
	}));
	
	// W --- S
	Install(new Word("word-name",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		const Word *wordPtr;
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr(&wordPtr)==false) {
			return inContext.Error(InvalidTypeErrorID::TosWp,tvWord);
		}
		inContext.DS.emplace_back(wordPtr->longName);
		NEXT;
	}));

	// --------------------------------
	// 	attribute
	// --------------------------------

	// W Xk Xv ---
	// Xk : key for attribute
	// Xv : value for the key Xk
	// ex: "targetWord" >word "logLevel" 5 set-attr
	Install(new Word("set-attr",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::DsAtLeast3);
		}
		TypedValue tvValue=Pop(inContext.DS);
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue tvWord=Pop(inContext.DS);
		if(SetAttr(inContext,tvWord,tvKey,tvValue)==false) { return false; }
		NEXT;
	}));

	// W Xk Xv --- W
	Install(new Word("@set-attr",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::DsAtLeast3);
		}
		TypedValue tvValue=Pop(inContext.DS);
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue& tvWord=ReadTOS(inContext.DS);
		if(SetAttr(inContext,tvWord,tvKey,tvValue)==false) { return false; }
		NEXT;
	}));

	// W Xk --- Xv
	// ex: "targetWord" >word "logLevel" get-attr
	Install(new Word("get-attr",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue tvWord=Pop(inContext.DS);
		const Word *wordPtr;
		if(tvWord.HasWordPtr(&wordPtr)==false) {
			return inContext.Error(InvalidTypeErrorID::SecondWp,tvWord);
		}
		Word *word=(Word *)wordPtr;
		if(word->attr==NULL) {
			return inContext.Error(InvalidValueErrorID::TosNoSuchAKey,tvKey);
		}
		auto itr=word->attr->find(tvKey);
		if(itr==word->attr->end()) {
			return inContext.Error(InvalidValueErrorID::TosNoSuchAKey,tvKey); 
		} else {
			inContext.DS.emplace_back(itr->second);
		}
		NEXT;	
	}));

	// W Xk --- 
	// ex: "targetWord" >word "logLevel" remove-attr
	Install(new Word("remove-attr",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue tvWord=Pop(inContext.DS);
		const Word *wordPtr;
		if(tvWord.HasWordPtr(&wordPtr)==false) {
			return inContext.Error(InvalidTypeErrorID::SecondWp,tvWord);
		}
		Word *word=(Word *)wordPtr;
		if(word->attr==NULL) {
			return inContext.Error(InvalidValueErrorID::TosNoSuchAKey,tvKey);
		}
		size_t numOfErase=word->attr->erase(tvKey);
		if(numOfErase==0) {
			return inContext.Error(InvalidValueErrorID::TosNoSuchAKey,tvKey); 
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
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue tvWord=Pop(inContext.DS);
		const Word *wordPtr;
		if(tvWord.HasWordPtr(&wordPtr)==false) {
			return inContext.Error(InvalidTypeErrorID::SecondWp,tvWord);
		}
		Word *word=(Word *)wordPtr;
		bool result=HasAttr(word,tvKey);	
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	// W --- t/f
	// ex: "targetWord" >word has-any-attr?
	Install(new Word("has-any-attr?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tvWord=Pop(inContext.DS);
		const Word *wordPtr;
		if(tvWord.HasWordPtr(&wordPtr)==false) {
			return inContext.Error(InvalidTypeErrorID::TosWp,tvWord);
		}
		Word *word=(Word *)wordPtr;
		inContext.DS.emplace_back(word->attr != NULL);
		NEXT;				
	}));

	// W ---
	// ex: "targetWord" >word show-attr
	Install(new Word("show-attr",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tvWord=Pop(inContext.DS);
		const Word *wordPtr;
		if(tvWord.HasWordPtr(&wordPtr)==false) {
			return inContext.Error(InvalidTypeErrorID::TosWp,tvWord);
		}
		Word *word=(Word *)wordPtr;
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

static bool colon(Context& inContext,bool inDefineShortend) {
	if(inContext.DS.size()<1) {
		return inContext.Error(NoParamErrorID::DsIsEmpty);
	}
	TypedValue tos=Pop(inContext.DS);
	if(tos.dataType!=DataType::Symbol && tos.dataType!=DataType::String) {
		return inContext.Error(InvalidTypeErrorID::TosSymbolOrString,tos);
	}
	if(tos.stringPtr->at(0)=='$') {
		return inContext.Error(NoParamErrorID::InvalidWordName);
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
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.newWord->CompileValue(tos);
	} else {
		inContext.newWord->CompileValue(inLambda);
	}
	return true;
}

static bool constant(Context& inContext,
					 const std::string *inName,const TypedValue& inValue,
					 bool inOverwriteCheck) {
	if( inOverwriteCheck ) {
		auto iter=Dict.find(*inName);
		if(iter!=Dict.end()) {
			return inContext.Error(ErrorIdWithString::AlreadyDefined,*inName);
		}
	}

	Word *newWord=new Word(inName);
	newWord->CompileWord("_lit");
	newWord->CompileValue(inValue);
	newWord->CompileWord("_semis");
	newWord->BuildParam();
	Dict[newWord->shortName]=newWord;
	Dict[newWord->longName ]=newWord;
	return true;
}

