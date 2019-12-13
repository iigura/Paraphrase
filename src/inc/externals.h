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

PP_API bool Docol(Context& inContext) NOEXCEPT;

