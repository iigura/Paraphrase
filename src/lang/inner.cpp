#include "paraphrase.h"
#include "inner.h"
#include "typedValue.h"
#include "stack.h"
#include "context.h"
#include "word.h"

PP_API bool InnerInterpreter(Context& inContext) {
	do {
		if((**(WordFunc **)inContext.ip)(inContext)==false) { return false; }
	} while(*inContext.ip!=NULL);
	return true;
}
