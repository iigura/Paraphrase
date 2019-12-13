#pragma once

#include <string>

#include "typedValue.h"

PP_API void InitOuterInterpreter();
PP_API bool OuterInterpreter(Context& inContext,std::string& inLine);
PP_API TypedValue GetTypedValue(std::string inToken);
 
