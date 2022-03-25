#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

void InitDict_Object() {
	Install(new Word("_nop",WORD_FUNC {
		NEXT;
	}));

	Install(new Word("invalid",WORD_FUNC {
		inContext.DS.emplace_back();
		NEXT;
	}));

	Install(new Word("valid?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(tos.dataType!=DataType::Invalid);
		NEXT;
	}));

	Install(new Word("@valid?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.dataType!=DataType::Invalid);
		NEXT;
	}));

	Install(new Word("invalid?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==DataType::Invalid);
		NEXT;
	}));

	Install(new Word("@invalid?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==DataType::Invalid);
		NEXT;
	}));

	Install(new Word("exec",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.HasWordPtr(NULL)) {
			if(inContext.Exec(tos)==false) { return false; }
		} else if(tos.dataType==DataType::StdCode) {
			if(tos.stdCodePtr(inContext)==false) { return false; }
		} else {
			return inContext.Error(InvalidTypeErrorID::TosWp,tos);
		}
		NEXT;
	}));

	Install(new Word("@r>exec",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(NoParamErrorID::RsIsEmpty); }
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=DataType::Word) {
			return inContext.Error(InvalidTypeErrorID::RsTosWp,rsTos);
		}
		if(inContext.Exec(rsTos)==false) { return false; }
		NEXT;
	}));
}

