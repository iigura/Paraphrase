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

inline TypedValue& ReadSecond(Stack& inStack) {
	return inStack[inStack.size()-2];
}

PP_API void ShowStack(Stack& inStack,const char *inStackName);

