// extension of the literate programming.
#ifdef _MSVC_LANG
	#pragma comment(lib,"libPP.lib")
#endif

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static std::string gVocabulary("lp");

extern "C" PP_API void InitDict() {
	std::string vocBackup=GetCurrentVocName();
	SetCurrentVocName(gVocabulary);

	Alias("<!DOCTYPE","/*");
	Alias("<!doctype","/*");
	Alias("</html>","*/");

	Alias("<code","*/");
	Alias("<code>","*/");

	Alias("</code>","/*");

	SetCurrentVocName(vocBackup);
}
