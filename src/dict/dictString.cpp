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
		if(inContext.EnterHereDocument(ControlBlockType::OpenHereDocumentRaw)==false) { NEXT; }

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
			return inContext.Error(NoParamErrorID::HereDocumentMismatch);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.RS);
		if(tvSyntax.dataType!=DataType::MiscInt
		  || tvSyntax.intValue!=(int)ControlBlockType::OpenHereDocumentRaw) {
			return inContext.Error(NoParamErrorID::HereDocumentMismatch);
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
		if(inContext.EnterHereDocument(ControlBlockType::OpenHereDocumentDedent)
		   ==false) { NEXT; }

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
			return inContext.Error(NoParamErrorID::HereDocumentMismatch);
		}
		TypedValue& tvSyntax=ReadTOS(inContext.RS);
		if(tvSyntax.dataType!=DataType::MiscInt
		  || tvSyntax.intValue!=(int)ControlBlockType::OpenHereDocumentDedent) {
			return inContext.Error(NoParamErrorID::HereDocumentMismatch);
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
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvIndex=Pop(inContext.DS);
		if(tvIndex.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tvIndex);
		}
		int pos=tvIndex.intValue;
		TypedValue &tvString=ReadTOS(inContext.DS);
		if(tvString.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,tvString);
		}
		int length=(int)tvString.stringPtr->length();
		if(pos<0 || length<=pos) {
			return inContext.Error(ErrorIdWith2int::StringIndexOutOfRange,length,pos);
		}
		std::string c=tvString.stringPtr->substr(pos,1);
		inContext.DS.emplace_back(c);
		NEXT;
	}));

	// S S --- S B
	Install(new Word("@start-with?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvPrefix=Pop(inContext.DS);
		if(tvPrefix.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tvPrefix);
		}
		TypedValue& tvString=ReadTOS(inContext.DS);
		if(tvString.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,tvString);
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
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvPrefix=Pop(inContext.DS);
		if(tvPrefix.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tvPrefix);
		}
		TypedValue tvString=Pop(inContext.DS);
		if(tvString.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,tvString);
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
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvPrefix=Pop(inContext.DS);
		if(tvPrefix.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tvPrefix);
		}
		TypedValue& tvString=ReadTOS(inContext.DS);
		if(tvString.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,tvString);
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
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvPrefix=Pop(inContext.DS);
		if(tvPrefix.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tvPrefix);
		}
		TypedValue tvString=Pop(inContext.DS);
		if(tvString.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,tvString);
		}
		std::string& target=*tvString.stringPtr;
		std::string& postfix=*tvPrefix.stringPtr;
		bool result=isEndWith(target,postfix);
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	// S --- S
	Install(new Word("trim",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		inContext.DS.emplace_back(
			std::regex_replace(*tos.stringPtr,std::regex("^\\s+|\\s+$"),"")
		);
		NEXT;
	}));

	// S1 S2 S3 --- S
	// Replace the substring S2 in S1 with the string S3.
	Install(new Word("replace-str",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::DsAtLeast3);
		}
		TypedValue tvStrReplaceTo=Pop(inContext.DS);
		if(tvStrReplaceTo.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tvStrReplaceTo);
		}
		TypedValue tvStrReplaceFrom=Pop(inContext.DS);
		if(tvStrReplaceFrom.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,tvStrReplaceFrom);
		}
		TypedValue tvTargetStr=Pop(inContext.DS);
		if(tvTargetStr.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::ThirdString,tvTargetStr);
		}
		try {
			std::regex regexPattern=std::regex(*tvStrReplaceFrom.stringPtr);
			inContext.DS.emplace_back(
				std::regex_replace(*tvTargetStr.stringPtr,
					regexPattern,
					*tvStrReplaceTo.stringPtr
				)
			);
		} catch (std::regex_error& inErr) {
			return regexError(inContext,inErr);
		}
		NEXT;
	}));

	// S1 S2 --- A
	Install(new Word("split-str-by",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvSplitStr=Pop(inContext.DS);
		if(tvSplitStr.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tvSplitStr);
		}
		if(tvSplitStr.stringPtr->length()!=1) {
			return inContext.Error(ErrorIdWithString::StringShouldBeOneChar,
								   *tvSplitStr.stringPtr);
		}
		TypedValue tvTargetStr=Pop(inContext.DS);
		if(tvTargetStr.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,tvTargetStr);
		}
		std::vector<std::string> tmp;
		std::stringstream stream(*tvTargetStr.stringPtr);
		std::string buf;
		const char separator=tvSplitStr.stringPtr->c_str()[0];
		while( std::getline(stream,buf,separator) ) {
			tmp.push_back(buf);
		}
		int n=(int)tmp.size();
		TypedValue *dataBody=new TypedValue[n];
		Array<TypedValue> *arrayPtr=new Array<TypedValue>(n,dataBody,true);
		for(int i=0; i<n; i++) {
			dataBody[i]=tmp[i];
		}
		inContext.DS.emplace_back(arrayPtr);
		NEXT;
	}));

	Install(new Word("sputf",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		TypedValue second=Pop(inContext.DS);
		TypedValue tvFormatted=GetFormattedString(tos.stringPtr->c_str(),second);
		if(tvFormatted.dataType!=DataType::String) {
			return inContext.Error(NoParamErrorID::FormatDataMismatch);
		}
		inContext.DS.emplace_back(tvFormatted);
		NEXT;
	}));

	Install(new Word("eval",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tvScriptSource=Pop(inContext.DS);
		if(tvScriptSource.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tvScriptSource);
		}
		size_t depthOfIS=inContext.IS.size();
		OIResult result=OuterInterpreter(inContext,*tvScriptSource.stringPtr,
										 inContext.LineNo);
		if(result!=OIResult::NoError) {
			auto itr=inContext.IS.begin();
			std::advance(itr,depthOfIS);
			inContext.IS.erase(itr,inContext.IS.end());
		}
		NEXT;
	}));

	// s --- s t/f
	Install(new Word("@empty-str?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		inContext.DS.emplace_back(tos.stringPtr->length()==0);
		NEXT;
	}));

	// s --- s t/f
	Install(new Word("@not-empty-str?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		inContext.DS.emplace_back(tos.stringPtr->length()>0);
		NEXT;
	}));

	// s --- s
	Install(new Word(">upper",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		std::transform(tos.stringPtr->cbegin(),tos.stringPtr->cend(),
					   tos.stringPtr->begin(),toupper);
		NEXT;
	}));
	
	// s --- s
	Install(new Word(">lower",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		std::transform(tos.stringPtr->cbegin(),tos.stringPtr->cend(),
					   tos.stringPtr->begin(),tolower);
		NEXT;
	}));

	// S S --- array
	// or
	// S S --- invalid
	Install(new Word("search",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,second);
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
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,second);
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
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue& tv=ReadTOS(inContext.DS);
		if(tv.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tv);
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
		{std::regex_constants::error_collate,	NoParamErrorID::RegexCollate   },
		{std::regex_constants::error_ctype,		NoParamErrorID::RegexCType	   },
		{std::regex_constants::error_escape,	NoParamErrorID::RegexEscape	   },
		{std::regex_constants::error_backref, 	NoParamErrorID::RegexBackRef   },
		{std::regex_constants::error_brack,		NoParamErrorID::RegexBrack	   },
		{std::regex_constants::error_paren,		NoParamErrorID::RegexParen	   },
		{std::regex_constants::error_brace,		NoParamErrorID::RegexBrace	   },
		{std::regex_constants::error_badbrace, 	NoParamErrorID::RegexBadBrace  },
		{std::regex_constants::error_range,		NoParamErrorID::RegexRange	   },
   		{std::regex_constants::error_space,		NoParamErrorID::RegexSpace	   },
		{std::regex_constants::error_badrepeat,	NoParamErrorID::RegexBadRepeat },
		{std::regex_constants::error_complexity,NoParamErrorID::RegexComplexity},
		{std::regex_constants::error_stack,		NoParamErrorID::RegexStack     },
	};
	return inContext.Error( errIdMapper.count(inErr.code())>0
							? errIdMapper[inErr.code()]
							: NoParamErrorID::RegexUnknownError );
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

