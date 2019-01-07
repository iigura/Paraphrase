// extension of the literate programming.
#ifdef _MSVC_LANG
	#pragma comment(lib,"libPP.lib")
#endif

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static std::string gVocabulary("lp");

extern "C" void InitDict() {
	Dict["<!DOCTYPE"]=Dict["/*"];
	Dict["<!doctype"]=Dict["/*"];
	Dict["</html>"]=Dict["*/"];

	Dict["<code"] =Dict["*/"];
	Dict["<code>"]=Dict["*/"];

	Dict["</code>"]=Dict["/*"];
}

