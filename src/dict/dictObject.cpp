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

	Install(new Word("@r>exec",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=kTypeWord) {
			return inContext.Error_InvalidType(E_RS_TOS_WP,rsTos);
		}
		if(inContext.Exec(rsTos)==false) { return false; }
		NEXT;
	}));
}

