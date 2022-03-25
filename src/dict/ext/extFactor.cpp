#ifdef _MSVC_LANG
	#pragma comment(lib,"libPP.lib")
#endif

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static std::string gVocabulary("factor");

extern "C" PP_API void InitDict() {
	std::string vocBackup=GetCurrentVocName();
	SetCurrentVocName(gVocabulary);

	// usage:
	// 	boolValue true-part false-part factor:if
	// ex:
	//	2 even? { "OK" } { "FALSE" } factor:if .
	Install(new Word("if",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::DsAtLeast3);
		}
		TypedValue elsePart=Pop(inContext.DS);
		if(elsePart.dataType!=DataType::Word) {
			return inContext.Error(InvalidTypeErrorID::TosWp,elsePart);
		}

		TypedValue truePart=Pop(inContext.DS);
		if(truePart.dataType!=DataType::Word) {
			return inContext.Error(InvalidTypeErrorID::SecondWp,truePart);
		}

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=DataType::Bool) {
			return inContext.Error(InvalidTypeErrorID::ThirdBool,tv);
		}

		if( tv.boolValue ) {
			if(inContext.Exec(truePart)==false) {
				return false;
			}
		} else {
			if(inContext.Exec(elsePart)==false) {
				return false;
			}
		}
		NEXT;
	}),false);

	SetCurrentVocName(vocBackup);
}
