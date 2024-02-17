#pragma once

#include <string>

#ifdef USE_READLINE
	#include <readline/readline.h>
	#include <readline/history.h>
#endif

#include "typedValue.h"

// result code of outer interpreter 
enum class OIResult {
	NoError,			// no error
	WordExecutingError,	// error(s) occured in some word.
	InvalidToken,		// token is invalid.
	NoSuchWord,			// no such a word
	OpenQuoteError,		// single or double quotation mismatch.
	SystemError,		// unknown bug is occured.
};

PP_API void InitOuterInterpreter();
PP_API OIResult OuterInterpreter(Context& inContext,std::string& inLine,int inLineNo);
PP_API OIResult OuterInterpreter(Context& inContext,TypedValue& inTV,
								 const std::string& inTokVal,int inLineNo);
PP_API TypedValue GetTypedValue(std::string inToken);
PP_API std::string ReadFromStdin(bool *outIsEOF,const char *inPrompt);
 
