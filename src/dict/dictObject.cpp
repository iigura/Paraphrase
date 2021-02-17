#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

void InitDict_Object() {
	Install(new Word("@type",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.GetTypeStr());
		NEXT;
	}));

	Install(new Word("type",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(tos.GetTypeStr());
		NEXT;
	}));

	Install(new Word("invalid",WORD_FUNC {
		inContext.DS.emplace_back();
		NEXT;
	}));

	Install(new Word("valid?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(tos.dataType!=DataType::kTypeInvalid);
		NEXT;
	}));

	Install(new Word("@valid?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.dataType!=DataType::kTypeInvalid);
		NEXT;
	}));

	Install(new Word("invalid?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==DataType::kTypeInvalid);
		NEXT;
	}));

	Install(new Word("@invalid?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==DataType::kTypeInvalid);
		NEXT;
	}));

	Install(new Word("exec",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.HasWordPtr()) {
			if(inContext.Exec(tos)==false) { return false; }
		} else if(tos.dataType==DataType::kTypeStdCode) {
			if(tos.stdCodePtr(inContext)==false) { return false; }
		} else {
			return inContext.Error(InvalidTypeErrorID::E_TOS_WP,tos);
		}
		NEXT;
	}));

	Install(new Word("@r>exec",WORD_FUNC {
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=DataType::kTypeWord) {
			return inContext.Error(InvalidTypeErrorID::E_RS_TOS_WP,rsTos);
		}
		if(inContext.Exec(rsTos)==false) { return false; }
		NEXT;
	}));
}

