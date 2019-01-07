#pragma once

#include <string>
#include <unordered_map>

#include "paraphrase.h"
#include "errorID.h"

PP_API extern Context *GlobalContext;
PP_API extern std::unordered_map<std::string,const Word*> Dict;

extern unsigned int G_NumOfCores;

PP_API bool Docol(Context& inContext) NOEXCEPT;
