#pragma once

#include <string>

PP_API void InitOuterInterpreter();
PP_API bool OuterInterpreter(Context& inContext,std::string& inLine);
 
