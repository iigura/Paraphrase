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
static int findCharForRawString(std::string& inLine,size_t inStartPos);

static std::string getToken(std::string& inLine,int *inScanPos,bool *outIsSuccess=NULL);

static bool doInterpretLevel(Context& inContext,TypedValue& inTV);
static bool doCompileLevel(Context& inContext,TypedValue& inTV);
static bool doSymbolLevel(Context& inContext,TypedValue& inTV,std::string& inToken);

static bool execute(Context& inContext,const TypedValue& inTypedValue);

static std::string evalEscapeSeqForRawString(std::string inString,bool *outIsSuccess);

PP_API void InitOuterInterpreter() {
	gTvEOL=TypedValue(Dict[EOL_WORD]);
}

PP_API OIResult OuterInterpreter(Context& inContext,std::string& inLine) {
	inContext.line=inLine;
	int scanPos=0;
	bool isSuccess=true;
	for(std::string tokVal=getToken(inLine,&scanPos,&isSuccess);
	  tokVal!=""; tokVal=getToken(inLine,&scanPos)) {
		TypedValue tv=GetTypedValue(tokVal);
		if(tv.dataType==DataType::Invalid && inContext.IsSymbolMode()==false) {
			inContext.Error(ErrorIdWithString::InvalidToken,tokVal);
			return OIResult::InvalidToken;
		}
		const Level theta=inContext.ExecutionThreshold;
		switch(theta) {
			case Level::Interpret:
				if(tv.dataType==DataType::Invalid) {
					inContext.ip=NULL;
					inContext.Error(ErrorIdWithString::CanNotFindTheWord,tokVal);
					return OIResult::NoSuchWord;
				} else {
					if(doInterpretLevel(inContext,tv)==false) {
						return OIResult::WordExecutingError;
					}
				}
				break;
			case Level::Compile:
				if(tv.dataType==DataType::Invalid) {
					inContext.Error(ErrorIdWithString::CanNotFindTheWord,tokVal);
					return OIResult::NoSuchWord;
				} else {
					if(doCompileLevel(inContext,tv)==false) {
						return OIResult::WordExecutingError;
					}
				}
				break;
			case Level::Symbol:
				if(tv.dataType==DataType::Invalid) {
					tv=TypedValue(tokVal,DataType::Symbol);
				}
				if(doSymbolLevel(inContext,tv,tokVal)==false) {
					return OIResult::WordExecutingError;
				}
				break;
			default:
				fprintf(stderr,"SYSTEM ERROR at OuterInterpreter.\n");
				return OIResult::SystemError;	
		}
	}
	if( isSuccess ) {
		execute(inContext,gTvEOL);
		return OIResult::NoError;
	} else {
		return OIResult::OpenQuoteError;
	}
}
static bool doInterpretLevel(Context& inContext,TypedValue& inTV) {
	if(inTV.dataType==DataType::DirectWord) {
		if(inContext.Exec(inTV)==false) { return false; }
	} else {
		inContext.DS.emplace_back(inTV);
	}
	return true;
}
static bool doCompileLevel(Context& inContext,TypedValue& inTV) {
	if(inTV.dataType==DataType::DirectWord
	  && (int)inTV.wordPtr->level>=(int)inContext.ExecutionThreshold) {
		if(inContext.Exec(inTV)==false) { return false; }
	} else {
		if(inTV.dataType!=DataType::DirectWord) {
			inContext.newWord->CompileWord("_lit");
		}
		inContext.newWord->CompileValue(inTV);
	}
	return true;
}
static bool doSymbolLevel(Context& inContext,TypedValue& inTV,std::string& inToken) {
	if(inTV.dataType==DataType::DirectWord) {
		if((int)inTV.wordPtr->level>=(int)inContext.ExecutionThreshold) {
			if(inContext.Exec(inTV)==false) { return false; }
		} else {
			inContext.newWord->CompileValue(TypedValue(inToken,DataType::Symbol));
		}
	} else {
		inContext.newWord->CompileValue(inTV);
	}
	return true;
}

static bool execute(Context& inContext,const TypedValue& inTypedValue) {
	if(inTypedValue.dataType==DataType::DirectWord) {
		return inContext.Exec(inTypedValue);
	} else {
		inContext.DS.push_back(inTypedValue);
		return true;
	}
}

static std::string getToken(std::string& inLine,int *ioScanPos,bool *outIsSuccess) {
	if(inLine.length()<=*ioScanPos) { return ""; }
	const char *whiteSpaceChar=" \t\n\r";
	size_t start=inLine.find_first_not_of(whiteSpaceChar,*ioScanPos);
	if(start==std::string::npos) { return ""; }
	int end;
	if(inLine[start]=='\"') {
		end=findChar(inLine,'\"',start+1);
		if(end<0) {
			GlobalContext->Error(NoParamErrorID::OpenDoubleQuote);
			if(outIsSuccess!=NULL) { *outIsSuccess=false; }
			return "";
		}
		end=(int)inLine.find_first_of(whiteSpaceChar,end+1);
	} else if(inLine[start]=='\'') {
		end=findCharForRawString(inLine,(size_t)start+1);
		if(end<0) {
			GlobalContext->Error(NoParamErrorID::OpenSingleQuote);
			if(outIsSuccess!=NULL) { *outIsSuccess=false; }
			return "";
		}
		end=(int)inLine.find_first_of(whiteSpaceChar,end+1);
	} else {
		end=(int)inLine.find_first_of(whiteSpaceChar,start+1);
	}
	*ioScanPos = end>=0 ? end : (int)inLine.length();
	std::string ret=inLine.substr(start,end-start);
	if(outIsSuccess!=NULL) { *outIsSuccess=true; }
	return ret;
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

static int findCharForRawString(std::string& inLine,size_t inStartPos) {
	const size_t n=inLine.size();
	for(size_t i=inStartPos; i<n; i++) {
		const char c=inLine[i];
		if(c=='\\') {
			if(i==n-1) { return -1; }
			if(inLine[i+1]=='\'' || inLine[i+1]=='0') { i++; }
			continue;
		} else if(c=='\'') {
			return (int)i;
		}
	}
	return -1;
}

static bool checkHasDot(const std::string& inToken,const size_t inTokenLength) {
	assert(inToken.length()==inTokenLength);
	for(int i=0; i<inTokenLength; i++) {
		if(inToken[i]=='.') { return true; }
	}
	return false;
}

PP_API TypedValue GetTypedValue(std::string inToken) {
	// word?
	auto iter=Dict.find(inToken);
	if(iter!=Dict.end()) {
		const Word *word=iter->second;
		TypedValue ret(word);
		return ret;
	} 

	// string?
	const size_t len=inToken.length();
	if(inToken[0]=='\"' && inToken[len-1]=='\"') {
		bool isSuccess=false;
		std::string s=inToken.substr(1,len-2);
		s=EvalEscapeSeq(s,&isSuccess);
		return isSuccess ? TypedValue(s) : TypedValue();
	}
	if(inToken[0]=='\'' && inToken[len-1]=='\'') {
		bool isSuccess=false;
		std::string s=evalEscapeSeqForRawString(inToken.substr(1,len-2),&isSuccess);
		return isSuccess ? TypedValue(s) : TypedValue();
	}
		
	// symbol?
	if(inToken[0]=='`') {
		if(inToken.length()<2) { goto invalid; }
		char c=inToken[1];
		if(('A'<=c && c<='Z') || ('a'<=c && c<='z') || c=='_' || c=='@') {
			std::string s=inToken.substr(1,len-1);
			TypedValue ret(s,DataType::Symbol);
			return ret;
		} else {
			goto invalid;
		}
	}

	if(checkHasDot(inToken,len)==false) {
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
	} else { // hasDot==true
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

PP_API std::string EvalEscapeSeq(std::string& inString,bool *outIsSuccess) {
	std::string ret;
	const size_t n=inString.size();
	char *buf=(char *)malloc(sizeof(char)*(n+1));
	if(buf==NULL) {
		fprintf(stderr,"SYSTEM ERROR: can not malloc at EvalEscapeSeq().\n");
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
				case ' ':	buf[destIndex]=' ';		break;
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
	*outIsSuccess=true;
	return ret;

escSeqError:
	fprintf(stderr,"escape sequence error.\n");
	free(buf);
	*outIsSuccess=false;
	return std::string();
}

static std::string evalEscapeSeqForRawString(std::string inString,bool *outIsSuccess) {
	std::string ret;
	const size_t n=inString.size();
	char *buf=(char *)malloc(sizeof(char)*(n+1));
	if(buf==NULL) {
		fprintf(stderr,
				"SYSTEM ERROR: can not malloc at evalEscapeSeqForRawString().\n");
		exit(-1);
	}

	int destIndex=0;
	for(size_t srcIndex=0; srcIndex<n; srcIndex++,destIndex++) {
		if(inString[srcIndex]=='\\') {
			if(srcIndex==n-1) { goto escSeqError; }
			if(inString[srcIndex+1]=='\'') {
				buf[destIndex]='\'';
				srcIndex++;
			} else if(inString[srcIndex+1]=='0') {
				// '\0' == empty char.
				// ex: '\\\0' express two backslash.
				srcIndex++;
				destIndex--;
			} else {
				goto justCopy;
			}
		} else {
justCopy:
			buf[destIndex]=inString[srcIndex];
		}
	}
	buf[destIndex]='\0';
	ret=std::string(buf);

	free(buf);
	*outIsSuccess=true;
	return ret;

escSeqError:
	fprintf(stderr,"escape sequence error.\n");
	free(buf);
	*outIsSuccess=false;
	return std::string();
}

#ifdef USE_READLINE
std::string gLastLine="";
PP_API std::string ReadFromStdin(bool *outIsEOF,const char *inPrompt) {
	std::string ret;
	char *line=readline(inPrompt);
		if(line!=NULL) {
			std::string s=std::string(line);
			if(*line!=0 && s!=gLastLine) {
				add_history(line);
				gLastLine=s;
			}
		} else {
			*outIsEOF=true;
			return "";
		}
		ret=std::string(line);
		*outIsEOF=false;
	free(line);
	return ret;
}
#else
PP_API std::string ReadFromStdin(bool *outIsEOF,const char *inPrompt) {
	printf(inPrompt);
	std::string ret;
	*outIsEOF=std::getline(std::cin,ret).eof();
	return ret;
}
#endif

