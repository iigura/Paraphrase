#include <boost/format.hpp>

#include "typedValue.h"
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

PP_API TypedValue GetFormattedString(const char *inFmt,TypedValue& inTV) {
	try {
		boost::format fmt(inFmt);
		switch(inTV.dataType) {
			case DataType::Int:		fmt=fmt%inTV.intValue;		break;
			case DataType::Long:	fmt=fmt%inTV.longValue;		break;
			case DataType::Float:	fmt=fmt%inTV.floatValue;	break;
			case DataType::Double:	fmt=fmt%inTV.doubleValue;	break;
			default:
				const int kNoIndent=0;
				fmt=fmt%inTV.GetValueString(kNoIndent).c_str();
		}
		std::string s=fmt.str();
		return TypedValue(s);
	} catch(...) {
		return TypedValue();
	}
}

PP_API void ExitOnSystemError(const char *inFuncName) {
	fprintf(stderr,"SYSTEM ERROR (%s).\n",inFuncName);
	exit(-1);
}

