#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "inner.h"

static const Word *gWordedDocol=NULL;
static std::shared_ptr<const Word> gSharedPtrForWordedDocol=NULL;

// user defined docol should be call 'docol' word.
static bool wordedDocolCaller(Context& inContext) {
	if(gWordedDocol==NULL) { return inContext.Error(NoParamErrorID::NoWordedDocol); }
	inContext.IS.emplace_back(inContext.ip);
	if(gWordedDocol->numOfLocalVar>0) {
		inContext.Env.push_back(LocalVarSlot(gWordedDocol->numOfLocalVar,TypedValue()));
	}
	inContext.ip=gWordedDocol->param;
	return true;
}

void InitDict_AOP() {
	// W C (=StdCode ptr) ---
	Install(new Word("set-code",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvNewStdCode=Pop(inContext.DS);
		if(tvNewStdCode.dataType!=DataType::StdCode) {
			return inContext.Error(InvalidTypeErrorID::TosStdCode,tvNewStdCode);
		}
		WordFunc newStdCode=tvNewStdCode.stdCodePtr;

		TypedValue tvTargetWord=Pop(inContext.DS);
		if(tvTargetWord.dataType!=DataType::Word) {
			return inContext.Error(InvalidTypeErrorID::SecondWp,tvTargetWord);
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
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tvWord=Pop(inContext.DS);
		if(tvWord.HasWordPtr(NULL)==false) {
			return inContext.Error(InvalidTypeErrorID::TosWp,tvWord);
		}
		if(tvWord.dataType==DataType::Lambda) {
			gWordedDocol=tvWord.lambdaPtr.get();
			gSharedPtrForWordedDocol=tvWord.lambdaPtr;
		} else {
			gWordedDocol=tvWord.wordPtr;
			gSharedPtrForWordedDocol=NULL;
		}
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
	Install(new Word("_docolMain",WORD_FUNC {
		if(inContext.IS.size()<1) { return inContext.Error(NoParamErrorID::IpsBroken); }
		const Word **target=inContext.IS.back();
		const Word *targetWord=*target;
		inContext.IS.emplace_back(inContext.ip);
		if(targetWord->numOfLocalVar>0) {
			inContext.Env.push_back(LocalVarSlot(targetWord->numOfLocalVar,
												 TypedValue()));
		}
		inContext.ip=targetWord->param;
		return true;
	}));

	Install(new Word("_docolCleanUp",WORD_FUNC {
		if(inContext.IS.size()<1) { return inContext.Error(NoParamErrorID::IpsBroken); }
		const Word **target=inContext.IS.back();
		const Word *targetWord=*target;
		if(targetWord->numOfLocalVar>0) { inContext.Env.pop_back(); }
		NEXT;
	}));

	// ローカル変数を考えなければ _docolMain だけで十分なのだが、
	// ローカル変数のクリアのため、_docolCleanUp を呼ぶ必要がある。<-- 本当か？
	// Forth であれば、docol == _docolMain で良い。
	Install(new Word("docol",WordLevel::Immediate,WORD_FUNC {
		inContext.Compile(std::string("_docolMain"));
		// inContext.Compile(std::string("_docolCleanUp"));
		NEXT;
	}));

	Install(new Word("docol-target-word",WORD_FUNC {
		if(inContext.IS.size()<1) { return inContext.Error(NoParamErrorID::IpsBroken); }
		const Word **target=inContext.IS.back();
		const Word *targetWord=*target;
		inContext.DS.emplace_back(targetWord);
		NEXT;
	}));
}

