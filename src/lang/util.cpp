#include "util.h"

PP_API int GetIndexForSkipWhiteSpace(std::string& inString,int inStartPos) {
	int i=inStartPos;
	const char *s=inString.c_str();
	for(;;) {
		char c=s[i];
		if(c==' ' || c=='\t' || c=='\n' || c=='\r') {
			i++;
			continue;
		}
		break;
	}
	return s[i]=='\0' ? -1 : i;
}

PP_API std::string SkipWhiteSpace(std::string& inString) {
	int t=GetIndexForSkipWhiteSpace(inString,0);
	std::string ret = t<0 ? "" : inString.substr(t);
	return ret;
}

