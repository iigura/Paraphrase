#include <stdlib.h>

#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include "externals.h"
#include "outer.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static TypedValue gTvEOL;

static int findChar(std::string& inLine,const char inTargetChar,size_t inStartPos);

static std::string getToken(std::string& inLine,int *inScanPos);
static TypedValue getTypedValue(Context& inContext,std::string inToken);

static bool doInterpretLevel(Context& inContext,TypedValue& inTV);
static bool doCompileLevel(Context& inContext,TypedValue& inTV);
static bool doSymbolLevel(Context& inContext,TypedValue& inTV,std::string& inToken);

static bool execute(Context& inContext,TypedValue inTypedValue);

static std::string evalEscapeSeq(std::string inString);

PP_API void InitOuterInterpreter() {
	gTvEOL=TypedValue(Dict[EOL_WORD]);
}

PP_API bool OuterInterpreter(Context& inContext,std::string& inLine) {
	int scanPos=0;
	for(std::string tokVal=getToken(inLine,&scanPos);
	  tokVal!=""; tokVal=getToken(inLine,&scanPos)) {
		TypedValue tv=getTypedValue(inContext,tokVal);
		if(tokVal!="\"\"" && tokVal!="''" 
		  && tv.dataType==kTypeString && tv.stringPtr.get()->size()==0) {
			// escape seq. error
			continue;
		} else {
			const int theta=inContext.ExecutionThreshold;
			switch(theta) {
				case kInterpretLevel:
					if(tv.dataType==kTypeInvalid) {
						inContext.ip=NULL;
						return inContext.Error(E_CAN_NOT_FIND_THE_WORD,tokVal);
					} else {
						if(doInterpretLevel(inContext,tv)==false) {
							return false;
						}
					}
					break;
				case kCompileLevel:
					if(tv.dataType==kTypeInvalid) {
						return inContext.Error(E_CAN_NOT_FIND_THE_WORD,tokVal);
					} else {
						if(doCompileLevel(inContext,tv)==false) {
							return false;
						}
					}
					break;
				case kSymbolLevel:
					if(tv.dataType==kTypeInvalid) {
						tv=TypedValue(tokVal,kTypeSymbol);
					}
					if(doSymbolLevel(inContext,tv,tokVal)==false) {
						return false;
					}
					break;
				default:
					fprintf(stderr,"SYSTEM ERROR at OuterInterpreter.\n");
					return false;	
			}
		}
	}
	execute(inContext,gTvEOL);
	return true;
}
static bool doInterpretLevel(Context& inContext,TypedValue& inTV) {
	if(inTV.dataType==kTypeDirectWord) {
		if(inContext.Exec(inTV)==false) {
			return false;
		}
	} else {
		inContext.DS.emplace_back(inTV);
	}
	return true;
}
static bool doCompileLevel(Context& inContext,TypedValue& inTV) {
	if(inTV.dataType==kTypeDirectWord
	  && (int)inTV.wordPtr->level>=inContext.ExecutionThreshold) {
		if(inContext.Exec(inTV)==false) { return false; }
	} else {
		if(inTV.dataType!=kTypeDirectWord) {
			inContext.Compile(std::string("_lit"));
		}
		inContext.Compile(inTV);
	}
	return true;
}
static bool doSymbolLevel(Context& inContext,TypedValue& inTV,std::string& inToken) {
	if(inTV.dataType==kTypeDirectWord) {
		if((int)inTV.wordPtr->level>=inContext.ExecutionThreshold) {
			if(inContext.Exec(inTV)==false) { return false; }
		} else {
			inContext.Compile(TypedValue(inToken,kTypeSymbol));
		}
	} else {
		inContext.Compile(inTV);
	}
	return true;
}

static bool execute(Context& inContext,TypedValue inTypedValue) {
	if(inTypedValue.dataType==kTypeDirectWord) {
		return inContext.Exec(inTypedValue);
	} else {
		inContext.DS.push_back(inTypedValue);
		return true;
	}
}

static std::string getToken(std::string& inLine,int *ioScanPos) {
	if(inLine.length()<=*ioScanPos) { return ""; }
	const char *whiteSpaceChar=" \t\n\r";
	size_t start=inLine.find_first_not_of(whiteSpaceChar,*ioScanPos);
	if(start==std::string::npos) { return ""; }
	int end;
	if(inLine[start]=='\"') {
		end=findChar(inLine,'\"',start+1);
		if(end<0) {
			GlobalContext->Error(E_OPEN_DOUBLE_QUOTE);
			return "";
		}
		end=(int)inLine.find_first_of(whiteSpaceChar,end+1);
	} else if(inLine[start]=='\'') {
		end=findChar(inLine,'\'',(size_t)start+1);
		if(end<0) {
			GlobalContext->Error(E_OPEN_SINGLE_QUOTE);
			return "";
		}
		end=(int)inLine.find_first_of(whiteSpaceChar,end+1);
	} else {
		end=(int)inLine.find_first_of(whiteSpaceChar,start+1);
	}
	*ioScanPos = end>=0 ? end : (int)inLine.length();
	return inLine.substr(start,end-start);
}

static int findChar(std::string& inLine,const char inTargetChar,size_t inStartPos) {
	const size_t n=inLine.size();
	for(size_t i=inStartPos; i<n; i++) {
		const char c=inLine[i];
		if(c=='\\') {
			if(i==n-1) { return -1; }
			i++;
			continue;
		} else if(c==inTargetChar) {
			return (int)i;
		}
	}
	return -1;
}

static TypedValue getTypedValue(Context& inContext,std::string inToken) {
	// word?
	auto iter=Dict.find(inToken);
	if(iter!=Dict.end()) {
		const Word *word=iter->second;
		TypedValue ret(word);
		return ret;
	} 

	// string?
	const size_t len=inToken.length();
	if((inToken[0]=='\"' && inToken[len-1]=='\"')
	|| (inToken[0]=='\'' && inToken[len-1]=='\'')) {
		std::string s=evalEscapeSeq(inToken.substr(1,len-2));
		TypedValue ret(s);
		return ret;
	}
	
	bool hasDot=false;
	for(int i=0; i<len; i++) {
		if(inToken[i]=='.') { hasDot=true; break; }
	}
	if(hasDot==false) {
		// note:
		// 	0x100f は 0x100F と同じだから float にはならない
		// 	0x100d も同様（0x100D)

		bool hexString=len>2
					   && inToken[0]=='0' && (inToken[1]=='x' || inToken[1]=='X');

		// 123LL or 123ll is big-int.
		if(len>2
		  && (inToken[len-2]=='l' || inToken[len-2]=='L')
		  && (inToken[len-1]=='l' || inToken[len-1]=='L')) {
			inToken.erase(len-2);
			goto bigInt;
		}
		if(len>1 && (inToken[len-1]=='l' || inToken[len-1]=='L')) {
			inToken.erase(len-1);
			goto checkLong;
		}
		// int?
		if( hexString ) {
			try {
				int n=std::stoi(inToken,NULL,16);
				TypedValue ret(n);
				return ret;
			} catch(...) {
				goto checkLong;
			}
		} else {
			try {
				int n=boost::lexical_cast<int>(inToken);
				TypedValue ret(n);
				return ret;
			} catch(const boost::bad_lexical_cast& /* inExcpt */) {
				goto checkLong;
			}
		}

checkLong:
		if( hexString ) {
			try {
				long n=std::stol(inToken,NULL,16);
				TypedValue ret(n);
				return ret;
			} catch(...) {
				goto bigInt;
			}
	} else {
			try {
				long n=boost::lexical_cast<long>(inToken);
				TypedValue ret(n);
				return ret;
			} catch(const boost::bad_lexical_cast& /* inExcpt */) {
				goto bigInt;
			}
		}

bigInt:
		try {
			TypedValue ret(BigInt(inToken.c_str()));
			return ret;
		} catch(...) {
			goto invalid;
		}
	}

	if( hasDot ) {
		try {
			size_t sz;
			float f=std::stof(inToken,&sz);
			const size_t len=inToken.length();

			if(sz==len-1 && inToken[sz]=='f') {
				TypedValue ret(f);
				return ret;
			} else if(sz!=len) {
				if(inToken[sz]=='d') {
					goto checkDouble;
				} else {
					goto invalid;
				}
			}

			double d=std::stod(inToken);
			if(((double)f)!=d) {
				goto checkDouble;
			}
			TypedValue ret(f);
			return ret;
		} catch(const std::invalid_argument& /* inException */) {
			goto checkDouble;
		} catch(const std::out_of_range& /* inException */) {
			goto checkDouble;
		}

checkDouble:
		try {
			size_t szd;
			double d=std::stof(inToken,&szd);
			const size_t len=inToken.length();
			if(szd==len-1 && inToken[szd]=='d') {
				TypedValue ret(d);
				return ret;
			} else if(szd!=len) {
				goto invalid;
			}
			TypedValue ret(d);
			return ret;
		} catch(const std::invalid_argument& /* inException */) {
			goto invalid;
		} catch(const std::out_of_range& /* inException */) {
			goto invalid;
		}
	}

invalid:
	TypedValue ret;
	return ret;
}

static std::string evalEscapeSeq(std::string inString) {
	std::string ret;
	const size_t n=inString.size();
	char *buf=(char *)malloc(sizeof(char)*(n+1));
	if(buf==NULL) {
		fprintf(stderr,"SYSTEM ERROR: can not malloc at evalEscapeLiteral().\n");
		exit(-1);
	}
	int destIndex=0;
	for(size_t srcIndex=0; srcIndex<n; srcIndex++,destIndex++) {
		if(inString[srcIndex]=='\\') {
			if(srcIndex==n-1) {
				goto escSeqError;
			}
			switch(inString[srcIndex+1]) {
				case '\"':	buf[destIndex]='\"';	break;
				case '\'':	buf[destIndex]='\'';	break;
				case '\\':	buf[destIndex]='\\';	break;
				case 'r':	buf[destIndex]='\r';	break;
				case 't':	buf[destIndex]='\t';	break;
				case 'n':	buf[destIndex]='\n';	break;
				default:
					goto escSeqError;
			}
			srcIndex++;
		} else {
			buf[destIndex]=inString[srcIndex];
		}
	}
	buf[destIndex]='\0';
	ret=std::string(buf);
	free(buf);
	return ret;

escSeqError:
	fprintf(stderr,"escape sequence error.\n");
	free(buf);
	return std::string();
}


