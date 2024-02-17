#pragma once

struct File {
	FILE *fp=NULL;
	void* ungetStack=NULL;	// Stack *ungetStack

	PP_API File();
	PP_API ~File();

	PP_API bool Open(const char *inFileName,const char *inMode);
	PP_API bool Close();
	PP_API bool IsValid() const;
};

