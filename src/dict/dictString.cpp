#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

void InitDict_String() {
	Install(new Word("eval",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tvScriptSource=Pop(inContext.DS);
		if(tvScriptSource.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tvScriptSource);
		}
		OuterInterpreter(inContext,*tvScriptSource.stringPtr);
		NEXT;
	}));

	Install(new Word("to-hex-str",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeInt) {
			return inContext.Error_InvalidType(E_TOS_INT,tos);
		}

		std::stringstream ss;
		ss << std::hex << tos.intValue;
		std::string hexStr=ss.str();
		transform(hexStr.begin(),hexStr.end(),hexStr.begin(),toupper);
		inContext.DS.emplace_back(hexStr);
		NEXT;
	}));

	// s --- s t/f
	Install(new Word("empty-str?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tos);
		}
		inContext.DS.emplace_back(tos.stringPtr->length()==0);
		NEXT;
	}));

	// s --- s t/f
	Install(new Word("not-empty-str?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tos);
		}
		inContext.DS.emplace_back(tos.stringPtr->length()>0);
		NEXT;
	}));

	// s --- s
	Install(new Word(">upper",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tos);
		}
		std::transform(tos.stringPtr->cbegin(),tos.stringPtr->cend(),
					   tos.stringPtr->begin(),toupper);
		NEXT;
	}));
	//
	// s --- s
	Install(new Word(">lower",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tos);
		}
		std::transform(tos.stringPtr->cbegin(),tos.stringPtr->cend(),
					   tos.stringPtr->begin(),tolower);
		NEXT;
	}));
}

