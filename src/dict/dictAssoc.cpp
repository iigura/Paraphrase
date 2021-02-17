#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

void InitDict_Assoc() {
	Install(new Word("new-assoc",WORD_FUNC {
		inContext.DS.emplace_back(new KeyValue());
		NEXT;
	}));
	// A X --- A B
	Install(new Word("has-key?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvKey=Pop(inContext.DS);
		TypedValue& tvKV=ReadTOS(inContext.DS);
		if(tvKV.dataType!=DataType::kTypeKV) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_KV,tvKV);
		}
		inContext.DS.emplace_back(tvKV.kvPtr->count(tvKey)>=1);
		NEXT;
	}));
	// A X --- A X B
	Install(new Word("@has-key?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue& tvKey=ReadTOS(inContext.DS);
		TypedValue& tvKV=ReadSecond(inContext.DS);
		if(tvKV.dataType!=DataType::kTypeKV) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_KV,tvKV);
		}
		inContext.DS.emplace_back(tvKV.kvPtr->count(tvKey)>=1);
		NEXT;
	}));
}

