#ifdef _MSVC_LANG
	#pragma comment(lib,"libPP.lib")
#endif

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

#include "inner.h"

// Sample code for limited simplified FFI
// See the references for Word 'exec' and get-std-code.
//
// ex:
// 	> ./ext/extStdCode.so" stdCodeHello get-std-code exec
//	 hello by StdCode.
//	 ok.
//
extern "C" PP_API bool stdCodeHello(Context& /* inContext */) {
	printf("hello by StdCode.\n");
	return true;
}
