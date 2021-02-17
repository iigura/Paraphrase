#ifdef _MSVC_LANG
	#pragma comment(lib,"libPP.lib")
#endif

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

#include "inner.h"

// This is the StdCode used in the AOP sample program.
// See the reference for the word set-code.
extern "C" PP_API bool docolWrapperExample(Context& inContext) {
	printf("IN:docolWrapperExample\n");
	const Word *word=(*inContext.ip);
	printf("word name: %s\n",word->longName.c_str());

		Docol(inContext);
		InnerInterpreter(inContext);

	printf("OUT:docolWrapperExample\n");
	return true;
}

