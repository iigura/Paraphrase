#pragma once

#include <vector>

extern TypedValue G_InvalidTV;

typedef std::vector<TypedValue> Stack;

inline TypedValue Pop(Stack& inStack) {
	TypedValue ret=inStack.back();
	inStack.pop_back();
	return ret;
}

inline TypedValue Pop(Stack *inStack) {
	TypedValue ret=inStack->back();
	inStack->pop_back();
	return ret;
}

inline TypedValue& ReadTOS(Stack& inStack) {
	return inStack.back();
}

inline TypedValue& ReadSecond(Stack& inStack) {
	return inStack[inStack.size()-2];
}

inline void BackInsert(Stack *ioDest,const Stack *inSrc) {
	ioDest->reserve(inSrc->size());
	std::copy(inSrc->begin(),inSrc->end(),std::back_inserter(*ioDest));
}

inline int GetAddress(Stack& inStack,int inPos) {
	TypedValue& tv=inStack.at(inPos);
	if(tv.dataType!=DataType::Address) { return -1; }
	return tv.intValue;
}

PP_API void ShowStack(Stack& inStack,const char *inStackName);

