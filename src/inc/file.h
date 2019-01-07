#pragma once

struct File {
	FILE *fp=NULL;

	File() { /* empty */ }
	~File() { Close(); }

	bool Open(const char *inFileName,const char *inMode) {
#ifdef _MSVC_LANG
		errno_t result=fopen_s(&fp,inFileName,inMode);
		return result==0;
#else
		fp=fopen(inFileName,inMode);
		return fp!=NULL;
#endif
	}

	bool Close() {
		if(fp!=NULL) {
			bool ret = fclose(fp)==0;
			fp=NULL;
			return ret;
		} else {
			return true;
		}
	}
};

