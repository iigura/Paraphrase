#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "inner.h"

static void clearDebugCommand(Context& ioContext);
static void setDebugCommand(Context& ioContext,DebugCommandType inDebugCommand);
static DebugCommandType getDebugCommand(Context& inContext);
static bool isOnDebugger(Context& inContext);
static const Word *step(Context& inContext,bool inIsStepIn);
static void dump(const Word *inWord,Context& inContext);
static bool sendDebugCommand(Context& inContext,DebugCommandType inDebugCommand);

const char *const kDebugPrompt="debug> ";
const TypedValue kDebugChannelKey("_DEBUG_COMMAND");

static std::string gLastInput="";

static bool docolForDebug(Context& inContext) NOEXCEPT {
	inContext.IS.emplace_back(inContext.ip);
	const Word *word=(*inContext.ip);
	if(word->numOfLocalVar>0) {
		inContext.Env.push_back(LocalVarSlot(word->numOfLocalVar,TypedValue()));
	}

	printf("[--- break at the word '%s'. ---]\n",word->longName.c_str());
	inContext.ip=word->param;
	// word->Dump(inContext);
	for(;;) {
		bool isEOF=false;
		dump(word,inContext);
		std::string line=ReadFromStdin(&isEOF,kDebugPrompt);
		if( isEOF ) { goto exitDebugger; }
		if(line=="" && gLastInput!="") {
			printf("INPUT=%s\n",gLastInput.c_str());
			line=gLastInput;
		}
		gLastInput=line;

		clearDebugCommand(inContext);
		OIResult result=OuterInterpreter(inContext,line);
		DebugCommandType debugCommand=getDebugCommand(inContext);
		if(debugCommand==DebugCommandType::kDCTypeSystemError) {
			fprintf(stderr,"SYSTEM ERROR: illegal debug command.\n");
			return false;
		}
		if(result==OIResult::OI_NO_ERROR
		   && debugCommand!=DebugCommandType::kDCTypeInvalid) {
			switch(debugCommand) {
				case DebugCommandType::kDCTypeContinue: goto execRest;
				case DebugCommandType::kDCTypeStepOver: 
					word=step(inContext,false);
					if(word==NULL) { return true; }	// exit dubugger
					continue;
				case DebugCommandType::kDCTypeStepIn:
					word=step(inContext,true);
					if(word==NULL) { return true; }	// exit dubugger
					continue;
				case DebugCommandType::kDCTypeQuit:	  goto exitDebugger;
				default:
					fprintf(stderr,"SYSTEM ERROR\n");
					exit(-1);
			}
		} else {
			switch(result) {
				case OIResult::OI_NO_ERROR: puts(" ok."); break;
				case OIResult::OI_E_WORD:
				case OIResult::OI_E_INVALID_TOKEN:
				case OIResult::OI_E_NO_SUCH_WORD:
						continue;
				case OIResult::OI_E_SYSTEM_ERROR:	return false;
			}
		}
	}
execRest:
	return true;

exitDebugger:
	if(inContext.attr!=NULL) {
		auto itr=inContext.attr->find(kDebugChannelKey);
		if(itr!=inContext.attr->end()) { inContext.attr->erase(kDebugChannelKey); }
		if(inContext.attr->size()==0) {
			delete inContext.attr;
			inContext.attr=NULL;
		}
	}
	puts("\n[--- exit debugger ---]");
	inContext.IS.clear();
	return false;
}

static void clearDebugCommand(Context& ioContext) {
	if(ioContext.attr==NULL) { ioContext.attr=new Attr(); }

	auto itr=ioContext.attr->find(kDebugChannelKey);
	if(itr==ioContext.attr->end()) {
		ioContext.attr->insert({kDebugChannelKey,
								TypedValue(DebugCommandType::kDCTypeInvalid)});
	} else {
		itr->second=TypedValue(DebugCommandType::kDCTypeInvalid);
	}
}

static void setDebugCommand(Context& ioContext,DebugCommandType inDebugCommand) {
	if(ioContext.attr==NULL) { ioContext.attr=new Attr(); }

	auto itr=ioContext.attr->find(kDebugChannelKey);
	if(itr==ioContext.attr->end()) {
		ioContext.attr->insert({kDebugChannelKey,TypedValue(inDebugCommand)});
	} else {
		itr->second=TypedValue(inDebugCommand);
	}
}

static DebugCommandType getDebugCommand(Context& inContext) {
	if(inContext.attr==NULL) { return DebugCommandType::kDCTypeSystemError; }
	auto itr=inContext.attr->find(kDebugChannelKey);
	if(itr==inContext.attr->end()) { return DebugCommandType::kDCTypeSystemError; }
	if(itr->second.dataType!=DataType::kTypeInt) {
		return DebugCommandType::kDCTypeSystemError;
	}
	switch((DebugCommandType)itr->second.intValue) {
		case DebugCommandType::kDCTypeInvalid:
		case DebugCommandType::kDCTypeContinue:
		case DebugCommandType::kDCTypeStepOver:
		case DebugCommandType::kDCTypeStepIn:
		case DebugCommandType::kDCTypeQuit:
			return (DebugCommandType)itr->second.intValue;
		default:
			return DebugCommandType::kDCTypeSystemError;
	}
}

static bool isOnDebugger(Context& inContext) {
	if(inContext.attr==NULL) { return false; }
	auto itr=inContext.attr->find(kDebugChannelKey);
	return itr!=inContext.attr->end();
}

void InitDict_Debug() {
	// S ---	ex: "wordName" breakpoint
	//   or
	// W ---	ex: word-ptr breakpoint
	Install(new Word("set-breakpoint",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		Word *targetWord=(Word *)GetWordPtr(inContext,tos);
		if(targetWord==NULL) { return false; }
		targetWord->code=docolForDebug;
		NEXT;
	}));

	// _b is an alias to 'set-breakpoint'.
	Install(new Word("_b",Dict["set-breakpoint"]->code));

	// S ---	ex: "wordName" breakpoint
	//   or
	// W ---	ex: word-ptr breakpoint
	Install(new Word("clear-breakpoint",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		Word *targetWord=(Word *)GetWordPtr(inContext,tos);
		if(targetWord==NULL) { return false; }
		targetWord->code=G_DocolAdvice;
		NEXT;
	}));
	
	// _cb is an alias to 'clear-breakpoint'.
	Install(new Word("_cb",Dict["clear-breakpoint"]->code));

	// ---
	Install(new Word("cont",WORD_FUNC {
		return sendDebugCommand(inContext,DebugCommandType::kDCTypeContinue);
	}));
	
	// _c is an alias to 'cont'.
	Install(new Word("_c",Dict["cont"]->code));

	// ---
	Install(new Word("quit-debugger",WORD_FUNC {
		return sendDebugCommand(inContext,DebugCommandType::kDCTypeQuit);
	}));
	
	// _q is an alias to 'cont'.
	Install(new Word("_q",Dict["quit-debugger"]->code));

	// ---
	Install(new Word("step-over",WORD_FUNC {
		return sendDebugCommand(inContext,DebugCommandType::kDCTypeStepOver);
	}));
	
	// _n is an alias to 'step-over'.
	Install(new Word("_n",Dict["step-over"]->code));

	Install(new Word("step-in",WORD_FUNC {
		return sendDebugCommand(inContext,DebugCommandType::kDCTypeStepIn);
	}));

	// _s is an alias to 'step-in'.
	Install(new Word("_s",Dict["step-in"]->code));
}

static const Word *step(Context& inContext,bool inIsStepIn) {
	const Word *word=*inContext.ip;
	WordFunc **next=(WordFunc **)inContext.ip;
	if(*next==NULL) { goto exitDebugger; }
	if(word->param==NULL) {
		std::string currentWordName;
		const Word *currentWord=NULL;
		if(inContext.IS.size()>0) {
			currentWord = *inContext.IS.back();
			currentWordName=currentWord->longName;
		}
		size_t isSize=inContext.IS.size();
		(**(WordFunc **)inContext.ip)(inContext);
		if(inContext.IS.size()<isSize) {
			printf("[--- leave the word '%s' ---]\n",currentWordName.c_str());
			if(inContext.IS.size()>0) {
				currentWord = *inContext.IS.back();
			} else {
				goto exitDebugger; 
			}
		}
		return currentWord;
	} else {
		if( inIsStepIn ) {
			// the case of step-in
			printf("[--- step in the word '%s' ---]\n",word->longName.c_str());
			Docol(inContext);
			return word;
		} else {
			// step over
			inContext.Exec(word);
			++inContext.ip;
			if(inContext.IS.size()>0) {
				const Word *currentWord = *inContext.IS.back();
				return currentWord;
			} else {
				return NULL;
			}
		}
	}

exitDebugger:
	printf("[--- exit debugger!! ---]\n");
	return NULL;
}

static void dump(const Word *inWord,Context& inContext) {
	if(inWord->param==NULL) { /* word is internal */ return; }
	int currentPos=(int)(inContext.ip-inWord->param);
	auto itr=inWord->addressOffsetToIndexMapper->find(currentPos);
	if(itr==inWord->addressOffsetToIndexMapper->end()) {
		fprintf(stderr,"SYSTEM ERROR: in dictDebug.dump.\n");
		exit(-1);
	}	
	int currentIndex=inWord->addressOffsetToIndexMapper->at(currentPos);
	for(int i=currentIndex-3; i<currentIndex+5; i++) {
		if(i<0) { continue; }
		if(i>=inWord->tmpParam->size()) { break; }
		printf(i==currentIndex ? "--> " : "    ");
		printf("[%4d] ",i);
		inWord->tmpParam->at(i).Dump();		
	}
}

static bool sendDebugCommand(Context& inContext,DebugCommandType inDebugCommand) {
	if(isOnDebugger(inContext)==false) {
		return inContext.Error(NoParamErrorID::E_SHOULD_BE_EXECUTE_ON_DEBUGGER);
	}
	setDebugCommand(inContext,inDebugCommand);
	NEXT;
}

