#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "util.h"

static bool regexError(Context& inContext,std::regex_error& inErr);
static bool isStartWith(std::string& inTarget,std::string& inPrefix);
static bool isEndWith(std::string& inTarget,std::string& inPostfix);

void InitDict_String() {
	Install(new Word("raw<<<",WordLevel::Level2,WORD_FUNC {
		if(inContext.EnterHereDocument(ControlBlockType::kOPEN_HERE_DOCUMENT_RAW)==false) { NEXT; }

		int s=(int)inContext.line.find("raw<<<");
		s=GetIndexForSkipWhiteSpace(inContext.line,s+6);	// 6 is length of "raw<<<".
		inContext.hereDocStr="";
		if(s<0) {
			inContext.appendNewlineForHereDocStr=false;
			inContext.line="";
		} else {
			inContext.line=inContext.line.substr(s);
			inContext.appendNewlineForHereDocStr=true;
		}
		NEXT;
	}));

	Install(new Word(">>>raw",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInCStyleComment() )	  { NEXT; }
		if( inContext.IsInCppStyleComment() ) { NEXT; }
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_HERE_DOCUMENT_MISMATCH);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.RS);
		if(tvSyntax.dataType!=DataType::kTypeMiscInt
		  || tvSyntax.intValue!=(int)ControlBlockType::kOPEN_HERE_DOCUMENT_RAW) {
			return inContext.Error(NoParamErrorID::E_HERE_DOCUMENT_MISMATCH);
		}
		Pop(inContext.RS);

		std::string::size_type n=inContext.line.find(">>>raw");
		std::string s = n!=std::string::npos ? inContext.line.substr(0,n)
											 : inContext.line;
		inContext.hereDocStr+=s;
		if(inContext.LeaveHereDocument()==false) { return false; }

		NEXT;
	}));

	Install(new Word("<<<",WordLevel::Level2,WORD_FUNC {
		if(inContext.EnterHereDocument(ControlBlockType::kOPEN_HERE_DOCUMENT_DEDENT)==false) { NEXT; }

		int s=(int)inContext.line.find("<<<");
		s=GetIndexForSkipWhiteSpace(inContext.line,s+3);	// 3 is length of "<<<".
		inContext.hereDocStr="";
		if(s<0) {
			inContext.appendNewlineForHereDocStr=false;
			inContext.line="";
		} else {
			inContext.line=inContext.line.substr(s);
			inContext.appendNewlineForHereDocStr=true;
		}
		NEXT;
	}));

	Install(new Word(">>>",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInCStyleComment() )	  { NEXT; }
		if( inContext.IsInCppStyleComment() ) { NEXT; }
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_HERE_DOCUMENT_MISMATCH);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.RS);
		if(tvSyntax.dataType!=DataType::kTypeMiscInt
		  || tvSyntax.intValue!=(int)ControlBlockType::kOPEN_HERE_DOCUMENT_DEDENT) {
			return inContext.Error(NoParamErrorID::E_HERE_DOCUMENT_MISMATCH);
		}
		Pop(inContext.RS);

		std::string::size_type n=inContext.line.find(">>>");
		std::string s = n!=std::string::npos ? inContext.line.substr(0,n)
											 : inContext.line;
		s=SkipWhiteSpace(s);
		bool isSuccess;
		inContext.hereDocStr+=EvalEscapeSeq(s,&isSuccess);
		if(inContext.LeaveHereDocument()==false) { return false; }

		NEXT;
	}));

	// S I --- S
	Install(new Word("at",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvIndex=Pop(inContext.DS);
		if(tvIndex.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tvIndex);
		}
		int pos=tvIndex.intValue;
		TypedValue &tvString=ReadTOS(inContext.DS);
		if(tvString.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,tvString);
		}
		int length=(int)tvString.stringPtr->length();
		if(pos<0 || length<=pos) {
			return inContext.Error(ErrorIdWith2int::E_STRING_INDEX_OUT_OF_RANGE,length,pos);
		}
		std::string c=tvString.stringPtr->substr(pos,1);
		inContext.DS.emplace_back(c);
		NEXT;
	}));

	// S S --- S B
	Install(new Word("@start-with?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvPrefix=Pop(inContext.DS);
		if(tvPrefix.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tvPrefix);
		}
		TypedValue& tvString=ReadTOS(inContext.DS);
		if(tvString.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,tvString);
		}
		std::string& target=*tvString.stringPtr;
		std::string& prefix=*tvPrefix.stringPtr;
		bool result=isStartWith(target,prefix);
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	// S S --- B
	Install(new Word("start-with?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvPrefix=Pop(inContext.DS);
		if(tvPrefix.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tvPrefix);
		}
		TypedValue tvString=Pop(inContext.DS);
		if(tvString.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,tvString);
		}
		std::string& target=*tvString.stringPtr;
		std::string& prefix=*tvPrefix.stringPtr;
		bool result=isStartWith(target,prefix);
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	// S S --- S B
	Install(new Word("@end-with?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvPrefix=Pop(inContext.DS);
		if(tvPrefix.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tvPrefix);
		}
		TypedValue& tvString=ReadTOS(inContext.DS);
		if(tvString.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,tvString);
		}
		std::string& target=*tvString.stringPtr;
		std::string& postfix=*tvPrefix.stringPtr;
		bool result=isEndWith(target,postfix);
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	// S S --- S B
	Install(new Word("end-with?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvPrefix=Pop(inContext.DS);
		if(tvPrefix.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tvPrefix);
		}
		TypedValue tvString=Pop(inContext.DS);
		if(tvString.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,tvString);
		}
		std::string& target=*tvString.stringPtr;
		std::string& postfix=*tvPrefix.stringPtr;
		bool result=isEndWith(target,postfix);
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	Install(new Word("eval",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tvScriptSource=Pop(inContext.DS);
		if(tvScriptSource.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tvScriptSource);
		}
		size_t depthOfIS=inContext.IS.size();
		OIResult result=OuterInterpreter(inContext,*tvScriptSource.stringPtr);
		if(result!=OIResult::OI_NO_ERROR) {
			auto itr=inContext.IS.begin();
			std::advance(itr,depthOfIS);
			inContext.IS.erase(itr,inContext.IS.end());
		}
		NEXT;
	}));

	Install(new Word(">array",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		std::vector<std::string> strVec;
		std::string *str=tos.stringPtr.get();
		size_t n=str->length();
		const char *p=str->c_str();
		
		int start,end=-1;
		do {
			start=end+1;
			char c;
			// skip white space
			for(; c=p[start],c==' ' || c=='\t' || c=='\n' || c=='\r'; start++) {
				// empty
			}
			if(c=='\0') { break; }
			// search substring
			for(end=start+1;
				c=p[end],c!=' ' && c!='\t' && c!='\n' && c!='\r' && c!='\0'; end++) {
				// empty
			}
			strVec.emplace_back(str->substr(start,(size_t)end-(size_t)start));
			if(c=='\0') { break; }
		} while(end<n);		

		int numOfElements=(int)strVec.size();
		TypedValue *dataBody=new TypedValue[numOfElements];
		for(size_t i=0; i<numOfElements; i++) {
			dataBody[i]=strVec[i];
		}
		Array<TypedValue> *arrayPtr=new Array<TypedValue>(numOfElements,dataBody,true);
		inContext.DS.emplace_back(arrayPtr);

		NEXT;
	}));

	Install(new Word("to-hex-str",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tos);
		}

		std::stringstream ss;
		ss << std::hex << tos.intValue;
		std::string hexStr=ss.str();
		transform(hexStr.begin(),hexStr.end(),hexStr.begin(),toupper);
		inContext.DS.emplace_back(hexStr);
		NEXT;
	}));

	// s --- s t/f
	Install(new Word("empty-str?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		inContext.DS.emplace_back(tos.stringPtr->length()==0);
		NEXT;
	}));

	// s --- s t/f
	Install(new Word("not-empty-str?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		inContext.DS.emplace_back(tos.stringPtr->length()>0);
		NEXT;
	}));

	// s --- s
	Install(new Word(">upper",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		std::transform(tos.stringPtr->cbegin(),tos.stringPtr->cend(),
					   tos.stringPtr->begin(),toupper);
		NEXT;
	}));
	
	// s --- s
	Install(new Word(">lower",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		std::transform(tos.stringPtr->cbegin(),tos.stringPtr->cend(),
					   tos.stringPtr->begin(),tolower);
		NEXT;
	}));

	// X --- s
	Install(new Word(">str",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(tos.GetValueString());
		NEXT;
	}));

	// S --- symbol
	Install(new Word(">symbol",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		std::string s=tos.GetValueString();
		char c=s[0];
		if(('A'<=c && c<='Z') || ('a'<=c && c<='z') || c=='_') {
			inContext.DS.emplace_back(s,DataType::kTypeSymbol);
		} else {
			return inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_SYMBOL);
		}
		NEXT;
	}));

	// S S --- array
	// or
	// S S --- invalid
	Install(new Word("search",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,second);
		}
		try {
			std::regex regex(*tos.stringPtr);
  			std::smatch match;
			if( std::regex_search(*second.stringPtr,match,regex) ) {
				size_t n=match.size();
				if(n==0) {
					inContext.DS.emplace_back();
				} else {
					TypedValue *dataBody=new TypedValue[n+1];
					for(size_t i=0; i<n; i++) {
						dataBody[i]=TypedValue(match[i].str());
					}
					dataBody[n]=TypedValue(match.suffix());
					Array<TypedValue> *arrayPtr
						=new Array<TypedValue>((int)n+1,dataBody,true);
					inContext.DS.emplace_back(arrayPtr);
				}
			} else {
				inContext.DS.emplace_back();
			}
		} catch (std::regex_error& inErr) {
			return regexError(inContext,inErr);
		}
		NEXT;
	}));

	// S S --- S array
	// or
	// S S --- S invalid
	Install(new Word("@search",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_STRING,second);
		}
		try {
			std::regex regex(*tos.stringPtr);
  			std::smatch match;
			if( std::regex_search(*second.stringPtr,match,regex) ) {
				size_t n=match.size();
				if(n==0) {
					inContext.DS.emplace_back();
				} else {
					TypedValue *dataBody=new TypedValue[n+1];
					for(size_t i=0; i<n; i++) {
						dataBody[i]=TypedValue(match[i].str());
					}
					dataBody[n]=TypedValue(match.suffix());
					Array<TypedValue> *arrayPtr
						=new Array<TypedValue>((int)n+1,dataBody,true);
					inContext.DS.emplace_back(arrayPtr);
				}
			} else {
				inContext.DS.emplace_back();
			}
		} catch (std::regex_error& inErr) {
			return regexError(inContext,inErr);
		}
		NEXT;
	}));

	// search++

	// S --- S
	Install(new Word("add-enter-char",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue& tv=ReadTOS(inContext.DS);
		if(tv.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tv);
		}

		#if defined(_MSC_VER)
			tv.stringPtr->append("\n");			
		#elif defined(FOR_MACOS)
			tv.stringPtr->append("\n");			
		#elif defined(FOR_LINUX)
			tv.stringPtr->append("\n");			
		#else
			#error The symbol _MSC_VER or FOR_MACOS or FOR_LINUX must be defined.
		#endif
		NEXT;
	}));
}

static bool regexError(Context& inContext,std::regex_error& inErr) {
	std::unordered_map<std::regex_constants::error_type,
					   NoParamErrorID> errIdMapper={
		{std::regex_constants::error_collate,	NoParamErrorID::E_REGEX_COLLATE	  },
		{std::regex_constants::error_ctype,		NoParamErrorID::E_REGEX_CTYPE	  },
		{std::regex_constants::error_escape,	NoParamErrorID::E_REGEX_ESCAPE	  },
		{std::regex_constants::error_backref, 	NoParamErrorID::E_REGEX_BACKREF	  },
		{std::regex_constants::error_brack,		NoParamErrorID::E_REGEX_BRACK	  },
		{std::regex_constants::error_paren,		NoParamErrorID::E_REGEX_PAREN	  },
		{std::regex_constants::error_brace,		NoParamErrorID::E_REGEX_BRACE	  },
		{std::regex_constants::error_badbrace, 	NoParamErrorID::E_REGEX_BADBRACE  },
		{std::regex_constants::error_range,		NoParamErrorID::E_REGEX_RANGE	  },
   		{std::regex_constants::error_space,		NoParamErrorID::E_REGEX_SPACE	  },
		{std::regex_constants::error_badrepeat,	NoParamErrorID::E_REGEX_BADREPEAT },
		{std::regex_constants::error_complexity,NoParamErrorID::E_REGEX_COMPLEXITY},
		{std::regex_constants::error_stack,		NoParamErrorID::E_REGEX_STACK	  },
	};
	return inContext.Error( errIdMapper.count(inErr.code())>0
							? errIdMapper[inErr.code()]
							: NoParamErrorID::E_REGEX_UNKNOWN_ERROR );
}

static bool isStartWith(std::string& inTarget,std::string& inPrefix) {
	bool ret;
	if(inTarget.length()<inPrefix.length()) {
		ret=false;
	} else {
		int n=(int)inPrefix.length();
		ret=true;
		for(int i=0; i<n; i++) {
			if(inTarget[i]!=inPrefix[i]) {
				ret=false;
				break;
			}
		}
	}
	return ret;
}

static bool isEndWith(std::string& inTarget,std::string& inPostfix) {
	bool ret;
	if(inTarget.length()<inPostfix.length()) {
		ret=false;
	} else {
		size_t n=inPostfix.length();
		size_t l=inTarget.length();
		ret=true;
		for(int i=0; i<n; i++) {
			if(inTarget[l-1-i]!=inPostfix[n-1-i]) {
				ret=false;
				break;
			}
		}
	}
	return ret;
}

