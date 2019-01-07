#pragma once

#include <vector>

extern TypedValue G_InvalidTV;

typedef std::vector<TypedValue> Stack;

inline TypedValue Pop(Stack& inStack) {
	TypedValue ret=inStack.back();
	inStack.pop_back();
	return ret;
}

inline TypedValue& ReadTOS(Stack& inStack) {
	return inStack.back();
}

inline TypedValue& ReadSecond(Stack& inStack,const char *inName) {
	const size_t len=inStack.size();
	if(len<2) {
		fprintf(stderr,"ERROR: %s is empty\n",inName);
		return G_InvalidTV;
	} else {
		return inStack[len-2];
	}
}

