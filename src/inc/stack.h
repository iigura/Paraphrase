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

inline int GetAddress(Stack& inStack,int inPos) {
	TypedValue& tv=inStack.at(inPos);
	if(tv.dataType!=DataType::kTypeAddress) { return -1; }
	return tv.intValue;
}

PP_API void ShowStack(Stack& inStack,const char *inStackName);

