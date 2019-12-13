#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static bool colon(const char *inName,Context& inContext,bool inDefineShortend);
static bool compileLambda(Context& inContext,TypedValue& inLambda);
static bool constant(const char *inName,Context& inContext,bool inOverwriteCheck);
static bool isInCStyleComment(Context& inContext);
static bool isInCppStyleComment(Context& inContext);

void InitDict_Word() {
	Install(new Word("_lit",WORD_FUNC {
		TypedValue *tv=(TypedValue *)(inContext.ip+1);
		inContext.DS.emplace_back(*tv);
		inContext.ip+=TvSize;
		NEXT;
	}));

	Install(new Word("_semis",WORD_FUNC {
		if(inContext.IS.size()<1) { return inContext.Error(E_IS_BROKEN); }
		inContext.ip=inContext.IS.back();
		inContext.IS.pop_back();
		if((*inContext.ip)->numOfLocalVar>0) { inContext.Env.pop_back(); }
		NEXT;
	}));
	
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
			if(tv.dataType!=kTypeMiscInt) {
				inContext.Error(E_SS_MISCINT,tv);
				exit(-1);
			}
			switch(tv.intValue) {
				case kSyntax_FOR_PLUS:
				case kSyntax_FOR_MINUS:
					inContext.Error(E_SYNTAX_OPEN_FOR);		goto onError;
				case kSyntax_WHILE:
				case kSyntax_WHILE_COND: 
					inContext.Error(E_SYNTAX_OPEN_WHILE);	goto onError;
				case kSyntax_DO:
					inContext.Error(E_SYNTAX_OPEN_DO);		goto onError;
				case kSyntax_IF:
					inContext.Error(E_SYNTAX_OPEN_IF);		goto onError;
				case kSyntax_SWITCH:
					inContext.Error(E_SYNTAX_OPEN_SWITCH);	goto onError;
				default:
					inContext.Error(E_SS_MISCINT_INVALID);
					exit(-1);
			}
		}
		if(inContext.newWord==NULL) {
			return inContext.Error(E_SHOULD_BE_EXECUTED_IN_DEFINITION);
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
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);

		std::string name;
		const Word *srcWord;
		if(tos.dataType==kTypeString
		   && (second.dataType==kTypeNewWord
			   || second.dataType==kTypeWord
			   || second.dataType==kTypeDirectWord)) {
			name=*tos.stringPtr;
			srcWord=second.wordPtr;
		} else if((tos.dataType==kTypeNewWord
				   || tos.dataType==kTypeWord
				   || tos.dataType==kTypeDirectWord) && second.dataType==kTypeString) {
			name=*second.stringPtr;
			srcWord=tos.wordPtr;
		} else {
			return inContext.Error(E_INVALID_DATA_TYPE_TOS_SECOND,tos,second);
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

	// string ---
	Install(new Word("forget",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeString) { return inContext.Error(E_TOS_STRING,tos); }
		auto iter=Dict.find(*tos.stringPtr);
		if(iter==Dict.end()) { 
			return inContext.Error(E_CAN_NOT_FIND_THE_WORD,*tos.stringPtr);
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
			return inContext.Error(E_NO_LAST_DEFINED_WORD);
		}
		inContext.lastDefinedWord->level=WordLevel::Immediate;
		NEXT;
	}));

	Install(new Word("vocabulary",WORD_FUNC {
		inContext.DS.emplace_back(GetCurrentVocName());
		NEXT;
	}));

	Install(new Word("set-vocabulary",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeString) {
			return inContext.Error(E_TOS_STRING,tos);
		}
		SetCurrentVocName(*tos.stringPtr.get());
		NEXT;
	}));

	// usage: "constName" value const
	// string value --- 
	Install(new Word("const",WORD_FUNC {
		const char *name="const";
		if(constant(name,inContext,true)==false) {
			return false;
		}
		NEXT;	
	}));

	// usage: "constName" value const!
	// string value --- 
	Install(new Word("const!",WORD_FUNC {
		const char *name="const!";
		if(constant(name,inContext,false)==false) {
			return false;
		}
		NEXT;	
	}));

	// usage: "varName" var
	// string ---
	Install(new Word("var",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeString) { return inContext.Error(E_TOS_STRING,tos); }

		auto iter=Dict.find(*tos.stringPtr);
		if(iter!=Dict.end()) {
			return inContext.Error(E_ALREADY_DEFINED,*tos.stringPtr);
		}

		Word *newWord=new Word(tos.stringPtr.get());

		const int paramSize=2+TvSize*2+MutexSize;
		newWord->param=new const Word*[paramSize];
		
		const Word *lit=GetWordPtr(std::string("_lit"));
		if(lit==NULL) {
			return inContext.Error(E_CAN_NOT_FIND_THE_WORD,std::string("_lit"));
		}
		const Word *semis=GetWordPtr(std::string("_semis"));
		if(semis==NULL) {
			return inContext.Error(E_CAN_NOT_FIND_THE_WORD,std::string("_semis"));
		}

		const int storageIndex=1+TvSize+1;
		const int mutexIndex=1+TvSize+1+TvSize;
		newWord->param[0]=lit;
		new((TypedValue*)(newWord->param+1)) TypedValue(kTypeParamDest,
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
			return inContext.Error(E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeParamDest) {
			return inContext.Error(E_TOS_PARAMDEST,tos);
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
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeParamDest) {
			return inContext.Error(E_TOS_PARAMDEST,tos);
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
		TypedValue tvWord(kTypeWord,newWordBackup);
		if(inContext.EndNoNameWordBlock()==false) { return false; }
		switch(inContext.ExecutionThreshold) {
			case kInterpretLevel:
				inContext.DS.emplace_back(tvWord);
				break;
			case kCompileLevel:
				assert(inContext.newWord!=NULL);
				inContext.Compile(std::string("_lit"));
				if(compileLambda(inContext,tvWord)==false) { return false; }
				break;
			case kSymbolLevel:
				assert(inContext.newWord!=NULL);
				if(compileLambda(inContext,tvWord)==false) { return false; }
				break;
			default:
				inContext.Error(E_SYSTEM_ERROR);
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
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		inContext.Compile(std::string("_semis"));
		Word *newWordBackup=inContext.newWord;
		inContext.FinishNewWord();
		TypedValue tvWord(kTypeWord,newWordBackup);
		if(inContext.EndNoNameWordBlock()==false) { return false; }
		if(inContext.Exec(tvWord)==false) { return false; }
		TypedValue tv=Pop(inContext.RS);
		if(tv.dataType!=kTypeInt) {
			return inContext.Error(E_RS_BROKEN_TOS_SHOULD_BE_INT);
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
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		inContext.Compile(std::string("_semis"));
		Word *newWordBackup=inContext.newWord;
		inContext.FinishNewWord();
		TypedValue tvWord(kTypeWord,newWordBackup);
		if(inContext.EndNoNameWordBlock()==false) { return false; }
		if(inContext.Exec(tvWord)==false) { return false; }
		TypedValue tv=Pop(inContext.RS);
		if(tv.dataType!=kTypeInt) {
			return inContext.Error(E_RS_BROKEN_TOS_SHOULD_BE_INT);
		}
		if(inContext.DS.size()>tv.intValue) {
			switch(inContext.ExecutionThreshold) {
				case kInterpretLevel:
					// do nothing
					break;
				case kCompileLevel:
					assert(inContext.newWord!=NULL);
					for(int i=tv.intValue; i<inContext.DS.size(); i++) {
						inContext.Compile(std::string("_lit"));
						inContext.Compile(inContext.DS[i]);
					}
					inContext.DS.erase(inContext.DS.begin()+tv.intValue,
									   inContext.DS.end());
					break;
				case kSymbolLevel:
					assert(inContext.newWord!=NULL);
					for(int i=tv.intValue; i<inContext.DS.size(); i++) {
						inContext.Compile(inContext.DS[i]);
					}
					inContext.DS.erase(inContext.DS.begin()+tv.intValue,
									   inContext.DS.end());
					break;
			default:
				inContext.Error(E_SYSTEM_ERROR);
				exit(-1);
			}
		}
		NEXT;
	}));

	Install(new Word("/*",WordLevel::Level2,WORD_FUNC {
		if( isInCppStyleComment(inContext) ) { NEXT; }
		inContext.PushThreshold();
		inContext.PushNewWord();
		inContext.RS.emplace_back(kTypeMiscInt,kOPEN_C_STYLE_COMMENT);
		inContext.newWord=new Word(WordType::Normal);
		inContext.ExecutionThreshold=kSymbolLevel;
		NEXT;
	}));

	Install(new Word("*/",WordLevel::Level2,WORD_FUNC {
		if( isInCppStyleComment(inContext) ) { NEXT; }
		if(inContext.RS.size()<1) {
			return inContext.Error(E_C_STYLE_COMMENT_MISMATCH);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.RS);
		if(tvSyntax.dataType==kTypeMiscInt
		  && tvSyntax.intValue==kOPEN_CPP_STYLE_ONE_LINE_COMMENT) {
			// do nothing in CPP style comment.
			NEXT;
		}
		if(tvSyntax.dataType!=kTypeMiscInt
		  || tvSyntax.intValue!=kOPEN_C_STYLE_COMMENT) {
			return inContext.Error(E_C_STYLE_COMMENT_MISMATCH);
		}
		Pop(inContext.RS);

		delete inContext.newWord->tmpParam;
		delete inContext.newWord;

		if(inContext.DS.size()<2) {
			return inContext.Error(E_DS_AT_LEAST_2);
		}
		TypedValue tvNW=Pop(inContext.DS);
		if(tvNW.dataType!=kTypeNewWord) { return inContext.Error(E_TOS_NEW_WORD,tvNW); }
 		inContext.newWord=(Word *)tvNW.wordPtr;

		TypedValue tvThreshold=Pop(inContext.DS);
		if(tvThreshold.dataType!=kTypeThreshold) {
			return inContext.Error(E_SECOND_THRESHOLD,tvThreshold);
		}
		inContext.ExecutionThreshold=tvThreshold.intValue;
		NEXT;
	}));

	Install(new Word("//",WordLevel::Level2,WORD_FUNC {
		if( isInCStyleComment(inContext) ) 	 { NEXT; }
		if( isInCppStyleComment(inContext) ) { NEXT; }
		inContext.PushThreshold();
		inContext.PushNewWord();
		inContext.RS.emplace_back(kTypeMiscInt,kOPEN_CPP_STYLE_ONE_LINE_COMMENT);
		inContext.newWord=new Word(WordType::Normal);
		inContext.ExecutionThreshold=kSymbolLevel;
		NEXT;
	}));

	Install(new Word(EOL_WORD,WordLevel::Level2,WORD_FUNC {
		if(inContext.RS.size()<1) { NEXT; }
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType==kTypeMiscInt
		  && rsTos.intValue==kOPEN_CPP_STYLE_ONE_LINE_COMMENT) {
			delete inContext.newWord->tmpParam;
			delete inContext.newWord;

			if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
			TypedValue tvNW=Pop(inContext.DS);
			if(tvNW.dataType!=kTypeNewWord) {
				return inContext.Error(E_TOS_NEW_WORD,tvNW);
			}
	 		inContext.newWord=(Word *)tvNW.wordPtr;

			Pop(inContext.RS);	// already checked by ReadTOS(inContext.RS).

			TypedValue tvThreshold=Pop(inContext.DS);
			if(tvThreshold.dataType!=kTypeThreshold) {
				return inContext.Error(E_SECOND_THRESHOLD,tvThreshold);
			}
			inContext.ExecutionThreshold=tvThreshold.intValue;
		}
		NEXT;
	}));

	// (s -- wordPtr)
	Install(new Word(">word",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeString) { return inContext.Error(E_TOS_STRING,tos); }
		auto iter=Dict.find(*tos.stringPtr);
		if(iter==Dict.end()) {
			return inContext.Error(E_CAN_NOT_FIND_THE_WORD,*tos.stringPtr);
		}
		const Word *word=iter->second;
		inContext.DS.emplace_back(kTypeWord,word);
		NEXT;
	}));

	Install(new Word(">lit",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		inContext.Compile(std::string("_lit"));
		inContext.Compile(tos);
		NEXT;
	}));

	Install(new Word(">here",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		if( inContext.IsInterpretMode() ) {
			return inContext.Error(E_SHOULD_BE_COMPILE_MODE);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.Compile(tos);
		NEXT;
	}));

	// original-wordName aliaseName alias
	// ex: "dup" "case" alias
	Install(new Word("alias",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue tvSrcWordName=Pop(inContext.DS);
		if(tvSrcWordName.dataType!=kTypeString) {
			return inContext.Error(E_SECOND_STRING,tvSrcWordName);
		}
		auto iter=Dict.find(*tvSrcWordName.stringPtr);
		if(iter==Dict.end()) {
			return inContext.Error(E_CAN_NOT_FIND_THE_WORD,*tvSrcWordName.stringPtr);
		}
		TypedValue tvNewWordName=Pop(inContext.DS);
		if(tvNewWordName.dataType!=kTypeString) {
			return inContext.Error(E_TOS_STRING,tvNewWordName);
		}
		const Word *srcWord=iter->second;
		const char *newWordName=strdup(tvNewWordName.stringPtr.get()->c_str());
		Word *newWord=new Word(newWordName,srcWord->level,srcWord->code);
		Install(newWord);
		newWord->param=srcWord->param;
		newWord->tmpParam=srcWord->tmpParam;
		NEXT;
	}));

	// wordPtr --- 
	Install(new Word("dump",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeWord && tos.dataType!=kTypeString) {
			return inContext.Error(E_TOS_WP,tos);
		}
		const Word *word=NULL;
		if(tos.dataType==kTypeWord) {
			word=tos.wordPtr;
		} else {
			auto iter=Dict.find(*tos.stringPtr);
			if(iter==Dict.end()) {
				return inContext.Error(E_CAN_NOT_FIND_THE_WORD,*tos.stringPtr);
			}
			word=iter->second;
		}
		if(word->isForgetable==false) {
			printf("the word '%s' is internal.\n",word->longName.c_str());
		} else {
			word->Dump();
		}
		NEXT;
	}));
}

static bool colon(const char *inName,Context& inContext,bool inDefineShortend) {
	if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
	TypedValue tos=Pop(inContext.DS);
	if(tos.dataType!=kTypeString) { return inContext.Error(E_TOS_STRING,tos); }
	if(tos.stringPtr->at(0)=='$') { return inContext.Error(E_INVALID_WORD_NAME); }
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
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		inContext.Compile(tos);
	} else {
		inContext.Compile(inLambda);
	}
	return true;
}

static bool constant(const char *inName,Context& inContext,bool inOverwriteCheck) {
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
	TypedValue tos=Pop(inContext.DS);
	TypedValue second=Pop(inContext.DS);
	if(second.dataType!=kTypeString) {
		return inContext.Error(E_SECOND_STRING,second);
	}

	if( inOverwriteCheck ) {
		auto iter=Dict.find(*second.stringPtr);
		if(iter!=Dict.end()) {
			return inContext.Error(E_ALREADY_DEFINED,*second.stringPtr);
		}
	}

	Word *newWord=new Word(second.stringPtr.get());
	Word *newWordBackup=inContext.newWord;
	inContext.newWord=newWord;
		inContext.Compile(std::string("_lit"));
		inContext.Compile(tos);
		inContext.Compile(std::string("_semis"));
		inContext.FinishNewWord();
	inContext.newWord=newWordBackup;
	Dict[newWord->shortName]=newWord;
	Dict[newWord->longName ]=newWord;
	return true;
}

static bool isInCStyleComment(Context& inContext) {
	if(inContext.RS.size()>0) {
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType==kTypeMiscInt && rsTos.intValue==kOPEN_C_STYLE_COMMENT) {
			return true;
		}
	}
	return false;
}

static bool isInCppStyleComment(Context& inContext) {
	if(inContext.RS.size()>0) {
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType==kTypeMiscInt
		  && rsTos.intValue==kOPEN_CPP_STYLE_ONE_LINE_COMMENT) {
			return true;
		}
	}
	return false;
}

