#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "inner.h"

static const Word *step(Context& inContext,bool inIsStepIn);
static void dump(const Word *inWord,Context& inContext);
static bool sendDebugCommand(Context& inContext,DebugCommand inDebugCommand);

const char *const kDebugPrompt="debug> ";
const TypedValue kDebugChannelKey("_DEBUG_COMMAND");

static std::string gLastInput="";

static bool docolForDebug(Context& inContext) NOEXCEPT {
	const Word *word=(*inContext.ip);
	Docol(inContext);

	bool tronBackup=IsTraceOn();
	SetTraceOn();

	printf("[--- break at the word '%s'. ---]\n",word->longName.c_str());
	for(;;) {
		if( IsTraceOn() ) { ShowTrace(inContext); }
		bool isEOF=false;
		dump(word,inContext);
		std::string line=ReadFromStdin(&isEOF,kDebugPrompt);
		if( isEOF ) { goto exitDebugger; }
		if(line=="" && gLastInput!="") {
			printf("INPUT=%s\n",gLastInput.c_str());
			line=gLastInput;
		}
		gLastInput=line;

		inContext.nowDebugging=true;
		inContext.debugCommand=DebugCommand::None;
		OIResult result=OuterInterpreter(inContext,line);
		inContext.nowDebugging=true;
		const DebugCommand debugCommand=inContext.debugCommand;
		if(debugCommand==DebugCommand::SystemError) {
			fprintf(stderr,"SYSTEM ERROR: illegal debug command.\n");
			return false;
		}
		if(result==OIResult::NoError
		   && debugCommand!=DebugCommand::None) {
			switch(debugCommand) {
				case DebugCommand::Continue:	// exec rest
					inContext.nowDebugging=false;
					return true;
				case DebugCommand::StepOver: 
					word=step(inContext,false);
					if(word==NULL || *inContext.ip==NULL) {
						return true;	// exit dubugger
					}
					continue;
				case DebugCommand::StepIn:
					word=step(inContext,true);
					if(word==NULL || *inContext.ip==NULL) {
						return true;	// exit dubugger
					}
					continue;
				case DebugCommand::Quit:
				  	goto exitDebugger;
				default:
					fprintf(stderr,"SYSTEM ERROR\n");
					exit(-1);
			}
		} else {
			switch(result) {
				case OIResult::NoError: puts(" ok."); break;

				case OIResult::WordExecutingError:
				case OIResult::InvalidToken:
				case OIResult::NoSuchWord:
				case OIResult::OpenQuoteError:
					continue;

				case OIResult::SystemError:	return false;
			}
		}
	}

exitDebugger:
	puts("\n[--- exit debugger ---]");
	inContext.IS.clear();
	inContext.nowDebugging=false;
	if(tronBackup==false) { SetTraceOff(); }
	return false;
}

void InitDict_Debug() {
	// S ---	ex: "wordName" breakpoint
	//   or
	// W ---	ex: word-ptr breakpoint
	Install(new Word("set-breakpoint",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
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
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
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
		return sendDebugCommand(inContext,DebugCommand::Continue);
	}));
	
	// _c is an alias to 'cont'.
	Install(new Word("_c",Dict["cont"]->code));

	// ---
	Install(new Word("quit-debugger",WORD_FUNC {
		return sendDebugCommand(inContext,DebugCommand::Quit);
	}));
	
	// _q is an alias to 'cont'.
	Install(new Word("_q",Dict["quit-debugger"]->code));

	// ---
	Install(new Word("step-over",WORD_FUNC {
		return sendDebugCommand(inContext,DebugCommand::StepOver);
	}));
	
	// _n is an alias to 'step-over'.
	Install(new Word("_n",Dict["step-over"]->code));

	Install(new Word("step-in",WORD_FUNC {
		return sendDebugCommand(inContext,DebugCommand::StepIn);
	}));

	// _s is an alias to 'step-in'.
	Install(new Word("_s",Dict["step-in"]->code));
}

static const Word *step(Context& inContext,bool inIsStepIn) {
	const Word *word=*inContext.ip;
	WordFunc **next=(WordFunc **)inContext.ip;
	if(*next==NULL) { goto exitDebugger; }
	if(word->param==NULL) { // the case of word is inner word (defined by C++).
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
			const Word **ipBackup=inContext.ip;
			inContext.Exec(word);
			inContext.ip=ipBackup+1;
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
		fprintf(stderr,"word =%s\n",inWord->longName.c_str());
		fprintf(stderr,"param=%p\n",inWord->param);
		fprintf(stderr,"ip   =%p\n",inContext.ip);
		fprintf(stderr,"currentPos=%d\n",currentPos);
		inWord->Dump();
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

static bool sendDebugCommand(Context& inContext,DebugCommand inDebugCommand) {
	if(inContext.nowDebugging==false) {
		return inContext.Error(NoParamErrorID::ShouldBeExecuteOnDebugger);
	}
	inContext.debugCommand=inDebugCommand;
	NEXT;	// needs 'inContext'.
}

