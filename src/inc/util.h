#pragma once

#include <string>

#include "paraphrase.h"

PP_API int GetIndexForSkipWhiteSpace(std::string& inString,int inStartPos);
PP_API std::string SkipWhiteSpace(std::string& inString);

