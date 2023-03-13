#include "externals.h"
#include "stack.h"
#include "context.h"

PP_API TypedValue G_ARGS;
PP_API unsigned int G_NumOfCores;
PP_API WordFunc G_DocolAdvice=Docol;

static std::string gScriptFileDir="";

PP_API void SetScriptFileDir(const std::string& inScriptFileDir) {
	gScriptFileDir=inScriptFileDir;
}
PP_API const char *GetScriptFileDir() {
	return gScriptFileDir.length()>0 ? gScriptFileDir.c_str() : NULL;
}

