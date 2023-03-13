#pragma once

#include <string>

#ifdef USE_NOEXCEPT
	#define NOEXCEPT noexcept
	#ifdef _WIN32
		#pragma warning(disable : 26495)
	#endif
#else
	#define NOEXCEPT
#endif

#ifdef _WIN32
	#ifdef LIBPP_EXPORTS
		#define PP_API __declspec(dllexport)
	#else
		#define PP_API __declspec(dllimport)
	#endif
	#define strdup _strdup
#else
	#define PP_API
#endif

#define EOL_WORD "__EOL__"

struct Context;
struct Word;

PP_API bool Install(Word *ioWord,const bool inUseShortendForm=true);
PP_API void Uninstall(const Word *inTargetWord);
PP_API void Alias(const char *inNewName,const char *inSrcName);

PP_API const std::string& GetCurrentVocName();
PP_API void SetCurrentVocName(const std::string& inStr);

PP_API std::string EvalEscapeSeq(std::string& inString,bool *outIsSuccess);

