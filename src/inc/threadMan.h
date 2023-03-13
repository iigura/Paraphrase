#pragma once

#include "paraphrase.h"

PP_API void StartThreadCollector();
PP_API void StopThreadCollector();

PP_API void AddToThreadPool(TypedValue& inTvContext);

PP_API int SetThreadCollectorInterval(int inIntervalSec);
PP_API int GetThreadCollectorInterval();

