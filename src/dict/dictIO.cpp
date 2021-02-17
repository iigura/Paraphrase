#include <fstream>
#include <iostream>
#include <limits>
#include <cstdio>

#include <boost/format.hpp>
#include <unicode/unistr.h>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <dlfcn.h>
#endif

enum CharCodeConv {
	kCCC_THRU,
	kUTF8_to_SJIS,
	kSJIS_to_UTF8,
};

static CharCodeConv gCCC=kCCC_THRU;

static bool gUseMockStdin=false;
static std::deque<char> gMockStdin;

static bool gUseMockStdout=false;
static std::deque<char> gMockStdout;

static void *getCFuncPointer(Context& inContext,
							 std::string *inLibPath,const char *inFuncName);
static void printValue(TypedValue& inTV);
static void printStr(std::string& inStr);

static void addMockStdin(const char *inStr);
static bool getlineFromMockStdin(std::string *outLine);

static void toStdout(char inChar);

void InitDict_IO() {
	Install(new Word("thru",WORD_FUNC {
		gCCC=kCCC_THRU;
		NEXT;
	}));

	Install(new Word("utf8>sjis",WORD_FUNC {
		gCCC=kUTF8_to_SJIS;
		NEXT;
	}));

	Install(new Word("sjis>utf8",WORD_FUNC {
		gCCC=kSJIS_to_UTF8;
		NEXT;
	}));

	Install(new Word("ccc-str",WORD_FUNC {
		std::string s;
		switch(gCCC) {
			case kCCC_THRU:		s="TRHU"; 		break;
			case kUTF8_to_SJIS: s="utf8>sjis"; 	break;
			case kSJIS_to_UTF8: s="sjis>utf8";	break;
		}
		inContext.DS.emplace_back(s);
		NEXT;
	}));

	// S ---
	Install(new Word(">mock-stdin",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tv);
		}

		addMockStdin(tv.stringPtr->c_str());
		NEXT;
	}));

	// ---
	Install(new Word("use-mock-stdout",WORD_FUNC {
		gUseMockStdout=true;
		NEXT;
	}));

	// ---
	Install(new Word("use-mock-stdin",WORD_FUNC {
		gUseMockStdin=true;
		NEXT;
	}));

	// ---
	Install(new Word("use-stdout",WORD_FUNC {
		gUseMockStdout=false;
		NEXT;
	}));

	// ---
	Install(new Word("use-stdin",WORD_FUNC {
		gUseMockStdin=false;
		NEXT;
	}));

	// --- S
	Install(new Word("get-line-from-mock-stdout",WORD_FUNC {
		std::string s="";
		size_t n=gMockStdout.size();
		for(int i=0; i<n; i++) {
			char c=gMockStdout.front();
			if(c=='\r' && i<n-1 && gMockStdout[1]=='\n') {
				gMockStdout.pop_front();
				gMockStdout.pop_front();
				break;
			}
			if(c=='\n') {
				gMockStdout.pop_front();
				break;
			}
			s+=c;
			gMockStdout.pop_front();
		}
		inContext.DS.emplace_back(s);
		NEXT;
	}));

	Install(new Word(".",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		toStdout(' ');
		printValue(tv);
		NEXT;
	}));

	Install(new Word(".cr",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		toStdout(' ');
		printValue(tv);
		toStdout('\n');
		NEXT;
	}));

	Install(new Word("@.",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tv=ReadTOS(inContext.DS);
		toStdout(' ');
		printValue(tv);
		NEXT;
	}));

	Install(new Word("write",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		printValue(tv);
		NEXT;
	}));

	Install(new Word("putf",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}
		try {
			boost::format fmt(tos.stringPtr->c_str());
			TypedValue second=Pop(inContext.DS);
			switch(second.dataType) {
				case DataType::kTypeInt:		fmt=fmt%second.intValue;	break;
				case DataType::kTypeLong:		fmt=fmt%second.longValue;	break;
				case DataType::kTypeFloat:		fmt=fmt%second.floatValue;	break;
				case DataType::kTypeDouble:		fmt=fmt%second.doubleValue;	break;
				case DataType::kTypeBigFloat:
				case DataType::kTypeBigInt:
				default:
					const int kNoIndent=0;
					fmt=fmt%second.GetValueString(kNoIndent).c_str();
			}
			std::string s=fmt.str();
			printStr(s);
		} catch(...) {
			return inContext.Error(NoParamErrorID::E_FORMAT_DATA_MISMATCH);
		}
		NEXT;
	}));

	Install(new Word("cr",WORD_FUNC {
		toStdout('\n');
		NEXT;
	}));

	Install(new Word("flush-stdout",WORD_FUNC {
		fflush(stdout);
		NEXT;
	}));
	
	Install(new Word("open",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tv);
		}
		File *file=new File();
		if(file->Open(tv.stringPtr->c_str(),"a+")==false) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_OPEN_FILE,*tv.stringPtr);
		}
		inContext.DS.emplace_back(file);
		NEXT;
	}));

	Install(new Word("new-file",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tv);
		}

		File *file=new File();
		if(file->Open(tv.stringPtr->c_str(),"w")==false) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_CREATE_FILE,*tv.stringPtr);
		}
		inContext.DS.emplace_back(file);
		NEXT;
	}));

	Install(new Word("close",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=DataType::kTypeFile) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_FILE,tv);
		}

		File *file=tv.filePtr.get();
		if(file->fp==NULL) {
			return inContext.Error(NoParamErrorID::E_FILE_IS_ALREADY_CLOSED);
		}
		if(file->Close()==false) {
			return inContext.Error(NoParamErrorID::E_CAN_NOT_CLOSE_FILE);
		}
		NEXT;
	}));

	Install(new Word("to-read",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::kTypeFile) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_FILE,tos);
		}

		File *file=tos.filePtr.get();
		if(file->fp==NULL) {
			return inContext.Error(NoParamErrorID::E_FILE_IS_ALREADY_CLOSED);
		}

		fseek(file->fp,0L,SEEK_SET);
		NEXT;
	}));

	// file X --- file
	Install(new Word(">file",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tos=Pop(inContext.DS);
		TypedValue& tvFile=ReadTOS(inContext.DS);
		if(tvFile.dataType!=DataType::kTypeFile) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_FILE,tvFile);
		}	
		File *file=tvFile.filePtr.get();
		fprintf(file->fp,"%s",tos.GetValueString(0).c_str());
		NEXT;
	}));

	Install(new Word("fgets",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::kTypeFile) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_FILE,tos);
		}

		File *file=tos.filePtr.get();
		if(file->fp==NULL) {
			return inContext.Error(NoParamErrorID::E_FILE_IS_ALREADY_CLOSED);
		}

		FILE *fp=file->fp;
		if( feof(fp) ) {
			inContext.DS.emplace_back();
		} else {
			std::string buf;
			buf.reserve(1024);
			while(!feof(fp)) {
				int c=fgetc(fp);
				if(c<0) {
					if(buf.size()==0) {
						inContext.DS.emplace_back();
						goto next;
					}
				}
				if(c=='\r') {
					int d=fgetc(fp);
					if(d=='\n') {
						break;
					} else {
						ungetc(d,fp);
						break;
					}
				} else if(c=='\n') {
					break;
				}
				buf+=c;
			}
			std::string s(buf);
			inContext.DS.emplace_back(s);
		}
next:
		NEXT;
	}));

	Install(new Word("flush-file",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::kTypeFile) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_FILE,tos);
		}

		File *file=tos.filePtr.get();
		if(file->fp==NULL) {
			return inContext.Error(NoParamErrorID::E_FILE_IS_ALREADY_CLOSED);
		}

		if(fflush(file->fp)!=0) {
			return inContext.Error(NoParamErrorID::E_CAN_NOT_FLUSH_FILE);
		}
		NEXT;
	}));

	Install(new Word("get-line",WORD_FUNC {
		std::string line;
		if( gUseMockStdin ) {
			// from mock-stdin
			bool isValid=getlineFromMockStdin(&line);
			if( isValid ) {
				inContext.DS.emplace_back(line);
			} else {
				inContext.DS.emplace_back(DataType::kTypeEOF);
			}
		} else {
			// from stdin
			if( std::getline(std::cin,line) ) {
				inContext.DS.emplace_back(line);
			} else {
				inContext.DS.emplace_back(DataType::kTypeEOF);
				std::cin.clear();
				clearerr(stdin);
			}
		}
		NEXT;
	}));

	Install(new Word("load",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tvFileName=Pop(inContext.DS);
		if(tvFileName.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tvFileName);
		}

		std::ifstream inputStream;
		inputStream.open(tvFileName.stringPtr->c_str());
		if( inputStream.fail() ) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_READ_FILE,*tvFileName.stringPtr);
		}
		std::string line;
		std::getline(inputStream,line);
		// the first line
		if(line.length()>1 && line[0]=='#') {
			// emtpy <- skip shebang.
		} else {
			if(OuterInterpreter(inContext,line)!=OIResult::OI_NO_ERROR) {
				inContext.IS.clear();
				return false;
			}
		}
		int count=2;	// line count is 1-origin.
		while(std::getline(inputStream,line).eof()==false) {
			if(OuterInterpreter(inContext,line)!=OIResult::OI_NO_ERROR) {
				fprintf(stderr,"Error at %d, line=%s\n",count,line.c_str());
				inContext.IS.clear();
				return false;
			}
			count++;
		}
		NEXT;
	}));

	Install(new Word("import",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeString) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_STRING,tos);
		}

		typedef void(*FuncPtr)(void);
		FuncPtr func=(FuncPtr)getCFuncPointer(inContext,tos.stringPtr.get(),"InitDict");
		if(func==NULL) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_ENTRY_POINT,*tos.stringPtr);
		}
		func();
		NEXT;
	}));

	// S(=libFilePath) S(=func-name) --- C(=stdCode)
	Install(new Word("get-std-code",WORD_FUNC {
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
		WordFunc func=(WordFunc)getCFuncPointer(inContext,
												second.stringPtr.get(),
												tos.stringPtr->c_str());
		if(func==NULL) {
			return inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_ENTRY_POINT,*tos.stringPtr);
		}
		inContext.DS.emplace_back(func);
		NEXT;
	}));

	Install(new Word("eof?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==DataType::kTypeEOF);
		NEXT;
	}));

	Install(new Word("@eof?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==DataType::kTypeEOF);
		NEXT;
	}));

	Install(new Word("inspect",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tv=ReadTOS(inContext.DS);
		tv.Dump();
		NEXT;
	}));

	Install(new Word("__size/tv__",WORD_FUNC {
		const char *name="__size/tv__";
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tv=Pop(inContext.DS);
		printf("sizeof(TypedValue)=%d\n",(int)sizeof(tv));
		printf("sizeof(dataType)=%d\n",(int)sizeof(tv.dataType));
		NEXT;
	}));
}

static void addMockStdin(const char *inStr) {
	for(int i=0; inStr[i]!='\0'; i++) {
		char c=inStr[i];
		if(c=='\n') { c='\0'; }
		gMockStdin.emplace_back(c);
	}
	gUseMockStdin=true;
}

static bool getlineFromMockStdin(std::string *outLine) {
	std::string s="";
	size_t n=gMockStdin.size();
	if(n==0) { return false; }
	for(int i=0; i<n; i++) {
		char c=gMockStdin.front();
		gMockStdin.pop_front();
		if(c=='\0') { break; }
		s+=c;
	}
	if(outLine!=NULL) { *outLine=s; }
	return true;
}

static void *getCFuncPointer(Context& inContext,
							 std::string *inLibPath,const char *inFuncName) {
	void *ret=NULL;
#ifdef _WIN32
	std::wstring dllFileName(inLibPath->begin(),inLibPath->end());
	HINSTANCE hDLL=LoadLibrary(dllFileName.c_str());
	if(hDLL==NULL) {
		inContext.Error(ErrorIdWithString::E_CAN_NOT_OPEN_FILE,*inLibPath);
		return NULL;
	}
	ret=GetProcAddress(hDLL,inFuncName);
	if(ret==NULL) {
		inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_ENTRY_POINT,*inLibPath);
		if(FreeLibrary(hDLL)==0) {
			inContext.Error(ErrorIdWithString::E_CAN_NOT_CLOSE_THE_FILE,*inLibPath);
		}
		return NULL;
	}
	// DO NOT CALL FreeLibrary(hDLL);
#else
	void *fh=dlopen(inLibPath->c_str(),RTLD_NOW | RTLD_NODELETE);
	if(fh==NULL) {
		inContext.Error(ErrorIdWithString::E_CAN_NOT_OPEN_FILE,*inLibPath);
		return NULL;
	}
	ret=dlsym(fh,inFuncName);
	if(ret==NULL) {
		inContext.Error(ErrorIdWithString::E_CAN_NOT_FIND_THE_ENTRY_POINT,*inLibPath);
		if(dlclose(fh)!=0) {
			inContext.Error(ErrorIdWithString::E_CAN_NOT_CLOSE_THE_FILE,*inLibPath);
		}
		return NULL;
	}
	if(dlclose(fh)!=0) {
		inContext.Error(ErrorIdWithString::E_CAN_NOT_CLOSE_THE_FILE,*inLibPath);
		return NULL;
	}
#endif
	return ret;
}

const char *kSJIS="shift_jis";
const char *kUTF8="utf8";

static void printValue(TypedValue& inTV) {
	const char *encodeFrom=NULL;
	const char *encodeTo=NULL;
	switch(gCCC) {
		case kCCC_THRU:
			if( gUseMockStdout ) {
				std::string valueString=inTV.GetValueString();
				const char *s=valueString.c_str();
				for(int i=0; s[i]!='\0'; i++) {
					gMockStdout.emplace_back(s[i]);
				}
			} else {
				inTV.PrintValue();
			}
			return;
		case kUTF8_to_SJIS: encodeFrom=kUTF8; encodeTo=kSJIS; break;
		case kSJIS_to_UTF8: encodeFrom=kSJIS; encodeTo=kUTF8; break;
	}
	icu::UnicodeString src(inTV.GetValueString().c_str(),encodeFrom);
   	int len=src.extract(0,(int)src.length(),NULL,encodeTo);
	char *tmp=new char[len+1];
		src.extract(0,src.length(),tmp,encodeTo);
		if( gUseMockStdout ) {
			for(int i=0; tmp[i]!='\0'; i++) {
				gMockStdout.emplace_back(tmp[i]);
			}
		} else {
			printf("%s",tmp);
		}
	delete[] tmp;
}

static void printStr(std::string& inStr) {
	const char *encodeFrom=NULL;
	const char *encodeTo=NULL;
	switch(gCCC) {
		case kCCC_THRU:
			if( gUseMockStdout ) {
				const char *s=inStr.c_str();
				for(int i=0; s[i]!='\0'; i++) {
					gMockStdout.emplace_back(s[i]);
				}
			} else {
				printf("%s",inStr.c_str());
			}
			return;
		case kUTF8_to_SJIS: encodeFrom=kUTF8; encodeTo=kSJIS; break;
		case kSJIS_to_UTF8: encodeFrom=kSJIS; encodeTo=kUTF8; break;
	}
	icu::UnicodeString src(inStr.c_str(),encodeFrom);
   	int len=src.extract(0,src.length(),NULL,encodeTo);
	char *tmp=new char[len+1];
		src.extract(0,src.length(),tmp,encodeTo);
		if( gUseMockStdout ) {
			for(int i=0; tmp[i]!='\0'; i++) {
				gMockStdout.emplace_back(tmp[i]);
			}
		} else {
			printf("%s",tmp);
		}
	delete[] tmp;
}

static void toStdout(char inChar) {
	if( gUseMockStdout ) {
		gMockStdout.emplace_back(inChar);
	} else {
		putc(inChar,stdout);
	}
}

