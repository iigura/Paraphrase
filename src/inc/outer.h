#pragma once

#include <string>

#ifdef USE_READLINE
	#include <readline/readline.h>
	#include <readline/history.h>
#endif

#include "typedValue.h"

// result code of outer interpreter 
enum class OIResult {
	OI_NO_ERROR,		// no error
	OI_E_WORD,			// error(s) occured in some word.
	OI_E_INVALID_TOKEN,	// token is invalid.
	OI_E_NO_SUCH_WORD,	// no such a word
	OI_E_SYSTEM_ERROR,	// unknown bug is occured.
};

PP_API void InitOuterInterpreter();
PP_API OIResult OuterInterpreter(Context& inContext,std::string& inLine);
PP_API TypedValue GetTypedValue(std::string inToken);
PP_API std::string ReadFromStdin(bool *outIsEOF,const char *inPrompt);
 
