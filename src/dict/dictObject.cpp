#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

void InitDict_Object() {
	Install(new Word("type",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.GetTypeStr());
		NEXT;
	}));

	Install(new Word("valid?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.dataType!=kTypeInvalid);
		NEXT;
	}));

	Install(new Word("exec",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeWord) {
			return inContext.Error_InvalidType(E_TOS_WP,tos);
		}
		if(inContext.Exec(tos)==false) {
			return false;
		}
		NEXT;
	}));
}

