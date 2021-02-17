#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "inner.h"

static const Word *gWordedDocol=NULL;

// user defined docol should be call 'docol' word.
static bool wordedDocolCaller(Context& inContext) {
	const Word *word=(*inContext.ip);
	Context aopContext(&inContext,Level::kInterpret,NULL,NULL,NULL);
	aopContext.DS.emplace_back(word);
	if(aopContext.Exec(gWordedDocol)==false) { return false; }
	return true;
}

void InitDict_AOP() {
	// W C (=StdCode ptr) ---
	Install(new Word("set-code",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvNewStdCode=Pop(inContext.DS);
		if(tvNewStdCode.dataType!=DataType::kTypeStdCode) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STDCODE,tvNewStdCode);
		}
		WordFunc newStdCode=tvNewStdCode.stdCodePtr;

		TypedValue tvTargetWord=Pop(inContext.DS);
		if(tvTargetWord.dataType!=DataType::kTypeWord) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_WP,tvTargetWord);
		}
		Word *targetWord=(Word *)tvTargetWord.wordPtr;

		targetWord->code=newStdCode;
		NEXT;
	}));

	// W --- 
	// "userDefDocol" :
	// 		"IN: userDefDocol" .cr
	// 			"target word:" . @word-name .cr
	// 			exec
	// 		"OUT: userDefDocol" .cr
	//
	// "userDefDocol" >word set-worded-docol-callee
	// "targetWord" >word worded-docol-caller set-code
	Install(new Word("set-worded-docol-callee",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr()==false) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_WP,tvWord);
		}
		gWordedDocol=tvWord.wordPtr;
		NEXT;
	}));

	Install(new Word("default-docol",WORD_FUNC {
		inContext.DS.emplace_back(Docol);
		NEXT;
	}));

	Install(new Word("worded-docol-caller",WORD_FUNC {
		inContext.DS.emplace_back((WordFunc)wordedDocolCaller);
		NEXT;
	}));

	// --- 
	Install(new Word("docol",WORD_FUNC {
		// inContext is aopContext.
		Context *targetContext=inContext.parent;
		if(targetContext==NULL) {
			fprintf(stderr,"Should be use via worded-docol-caller\n");
			return false;
		}
		Docol(*targetContext);
		InnerInterpreter(*targetContext);
		NEXT;
	}));
}

