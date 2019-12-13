#pragma once

#include <vector>

#include "typedValue.h"

typedef std::vector<TypedValue> CodeThread;

PP_API void InitOptPattern();
void Optimize(CodeThread *ioThread);
void ReplaceTailRecursionToJump(Word *inWord,CodeThread *ioThread);

