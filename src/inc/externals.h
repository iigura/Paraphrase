#pragma once

#include <string>
#include <unordered_map>

#include "typedValue.h"
#include "paraphrase.h"
#include "errorID.h"

PP_API extern Context *GlobalContext;
PP_API extern std::unordered_map<std::string,const Word*> Dict;

PP_API extern unsigned int G_NumOfCores;
PP_API extern TypedValue G_ARGS;

#ifdef USE_G_DOCOL_ADVICE // for MSVC dll
	PP_API extern WordFunc G_DocolAdvice;
#endif

PP_API extern void SetTraceOn();
PP_API extern void SetTraceOff();
PP_API extern bool IsTraceOn();
PP_API extern void ShowTrace(const Context& inContext);

PP_API extern void SetScriptFileDir(const std::string& inScriptFileDir);
PP_API extern const char *GetScriptFileDir();

