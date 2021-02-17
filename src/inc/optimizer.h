#pragma once

#include <vector>

#include "typedValue.h"

typedef std::vector<TypedValue> CodeThread;

PP_API void InitOptPattern();
PP_API void SetOptimizeLevel(int inOptimizeLevel);
PP_API int GetOptimizeLevel();

void Optimize(CodeThread *ioThread);
void ReplaceTailRecursionToJump(Word *inWord,CodeThread *ioThread);

