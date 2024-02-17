#include "paraphrase.h"
#include "file.h"
#include "typedValue.h"
#include "stack.h"

PP_API File::File():ungetStack(NULL)  { /* empty */ }
PP_API File::~File() { Close(); }

PP_API bool File::Open(const char *inFileName,const char *inMode) {
	Stack *us=new Stack();
	ungetStack=us;
#ifdef _MSVC_LANG
	errno_t result=fopen_s(&fp,inFileName,inMode);
	return result==0;
#else
	fp=fopen(inFileName,inMode);
	return fp!=NULL;
#endif
}

PP_API bool File::Close() {
	if(ungetStack!=NULL) {
		Stack *us=(Stack*)ungetStack;
		delete us;
		ungetStack=NULL;
	}
	if(fp!=NULL) {
		bool ret = fclose(fp)==0;
		fp=NULL;
		return ret;
	} else {
		return true;
	}
}

bool File::IsValid() const { return ungetStack!=NULL; }

