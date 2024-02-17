#include <fstream>
#include <iostream>
#include <limits>
#include <cstdio>
#include <filesystem>

#include <boost/format.hpp>
#include <unicode/unistr.h>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "util.h"

#ifdef _WIN32
	#define NOMINMAX
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

static Mutex gMutexForConsoleOutput;

static bool fgetsMain(Context& inContext,File *inFile);

static void *getCFuncPointer(Context& inContext,
							 std::string *inLibPath,const char *inFuncName);
static void printValue(const TypedValue& inTV,bool inTypePostfix=false);
static void printStr(const std::string& inStr);

static void addMockStdin(const char *inStr);
static bool getlineFromMockStdin(std::string *outLine);

static void toStdout(char inChar);

void LockForConsoleOutput() { Lock(gMutexForConsoleOutput); }
void UnlockForConsoleOutput() { Unlock(gMutexForConsoleOutput); }

void InitDict_IO() {
	initMutex(gMutexForConsoleOutput);

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
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tv);
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
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tv=Pop(inContext.DS);
		toStdout(' ');
		printValue(tv,true);
		NEXT;
	}));

	Install(new Word(".cr",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tv=Pop(inContext.DS);
		toStdout(' ');
		printValue(tv,true);
		toStdout('\n');
		NEXT;
	}));

	Install(new Word("@.",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tv=ReadTOS(inContext.DS);
		toStdout(' ');
		printValue(tv,true);
		NEXT;
	}));

	Install(new Word("write",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tv=Pop(inContext.DS);
		printValue(tv,false);
		NEXT;
	}));

	Install(new Word("putf",WORD_FUNC {
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
		printStr(*tvFormatted.stringPtr);
		NEXT;
	}));

	Install(new Word("putc",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.IsInteger()==false) {
			return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos);
		}
		int n=0;
		if(tos.dataType==DataType::Int) {
			n=tos.intValue;
		} else if(tos.dataType==DataType::Long) {
			if(tos.longValue<INT_MIN || INT_MAX<tos.longValue) {
				return inContext.Error(NoParamErrorID::CanNotConvertToLongDueToOverflow);
			}
			n=(int)tos.longValue;
		} else if(tos.dataType==DataType::BigInt) {
			if(*tos.bigIntPtr<INT_MIN || INT_MAX<*tos.bigIntPtr) {
				return inContext.Error(NoParamErrorID::CanNotConvertToLongDueToOverflow);
			}
			n=(int)*tos.bigIntPtr;
		}
		putc(n,stdout);
		NEXT;
	}));

	// { wordPtr | S | KV } --- 
	Install(new Word("dump",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType==DataType::Word
		   || tos.dataType==DataType::String || tos.dataType==DataType::Symbol) {
			const Word *word=NULL;
			if(tos.dataType==DataType::Word) {
				word=tos.wordPtr;
			} else {
				auto iter=Dict.find(*tos.stringPtr);
				if(iter==Dict.end()) {
					return inContext.Error(ErrorIdWithString
										   ::CanNotFindTheWord,*tos.stringPtr);
				}
				word=iter->second;
			}
			if(word->param==NULL) {
				printf("the word '%s' is internal.\n",word->longName.c_str());
			} else {
				word->Dump();
			}
		} else if(tos.dataType==DataType::KV) {
			printStr("assoc(\n");
			for(auto const &pair : *tos.kvPtr) {
				printStr("\tkey=");
				printValue(pair.first,true);
				printStr(", value=");
				printValue(pair.second,true);
				toStdout('\n');
			}
			printStr(")\n");
		} else {
			return inContext.Error(InvalidTypeErrorID::TosWpOrStringOrSymbolOrKV,tos);
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

	// S --- B
	Install(new Word("fexist?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(
					InvalidTypeErrorID::TosString,tos);
		}
		bool result=std::filesystem::exists(tos.stringPtr->c_str());
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	Install(new Word("open",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=DataType::String) {
			return inContext.Error(
				InvalidTypeErrorID::TosString,tv);
		}
		File *file=new File();
		if(file->Open(tv.stringPtr->c_str(),"a+")==false) {
			return inContext.Error(ErrorIdWithString::CanNotOpenFile,*tv.stringPtr);
		}
		inContext.DS.emplace_back(file);
		NEXT;
	}));

	Install(new Word("new-file",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tv);
		}

		File *file=new File();
		if(file->Open(tv.stringPtr->c_str(),"w")==false) {
			return inContext.Error(ErrorIdWithString::CanNotCreateFile,*tv.stringPtr);
		}
		inContext.DS.emplace_back(file);
		NEXT;
	}));

	Install(new Word("close",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=DataType::File) {
			return inContext.Error(InvalidTypeErrorID::TosFile,tv);
		}

		File *file=tv.filePtr.get();
		if(file->fp==NULL) {
			return inContext.Error(NoParamErrorID::FileIsAlreadyClosed);
		}
		if(file->Close()==false) {
			return inContext.Error(NoParamErrorID::CanNotCloseFile);
		}
		NEXT;
	}));

	Install(new Word("to-read",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::File) {
			return inContext.Error(InvalidTypeErrorID::TosFile,tos);
		}

		File *file=tos.filePtr.get();
		if(file->fp==NULL) {
			return inContext.Error(NoParamErrorID::FileIsAlreadyClosed);
		}

		fseek(file->fp,0L,SEEK_SET);
		NEXT;
	}));

	// file X --- file
	Install(new Word(">file",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue& tvFile=ReadTOS(inContext.DS);
		if(tvFile.dataType!=DataType::File) {
			return inContext.Error(InvalidTypeErrorID::SecondFile,tvFile);
		}	
		File *file=tvFile.filePtr.get();
		fprintf(file->fp,"%s",tos.GetValueString(0).c_str());
		NEXT;
	}));

	// X file ---
	Install(new Word("fwrite",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvFile=Pop(inContext.DS);
		if(tvFile.dataType!=DataType::File) {
			return inContext.Error(InvalidTypeErrorID::TosFile,tvFile);
		}
		TypedValue tvValue=Pop(inContext.DS);
		File *file=tvFile.filePtr.get();
		fprintf(file->fp,"%s",tvValue.GetValueString(0).c_str());
		NEXT;
	}));

	Install(new Word("@fgets",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::File) {
			return inContext.Error(InvalidTypeErrorID::TosFile,tos);
		}
		File *file=tos.filePtr.get();
		if(fgetsMain(inContext,file)==false) { return false; }
		NEXT;
	}));

	Install(new Word("fgets",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::File) {
			return inContext.Error(InvalidTypeErrorID::TosFile,tos);
		}
		File *file=tos.filePtr.get();
		if(fgetsMain(inContext,file)==false) { return false; }
		NEXT;
	}));

	// file string ---
	//   or
	// string file ---
	Install(new Word("unfgets",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue sec=Pop(inContext.DS);
		TypedValue tvFile,tvString;
		if(tos.dataType==DataType::File && sec.dataType==DataType::String) {
			tvFile=tos; tvString=sec;
		} else if(tos.dataType==DataType::String && sec.dataType==DataType::File) {
			tvFile=sec; tvString=tos;
		} else {
			return inContext.Error(InvalidTypeTosSecondErrorID::BothDataInvalid,
								   tos,sec);
		}
		File *file=tvFile.filePtr.get();
		Stack *us=(Stack*)(file->ungetStack);
		us->emplace_back(tvString);
		inContext.DS.emplace_back(tvFile);
		NEXT;		
	}));

	Install(new Word("flush-file",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::File) {
			return inContext.Error(InvalidTypeErrorID::TosFile,tos);
		}

		File *file=tos.filePtr.get();
		if(file->fp==NULL) {
			return inContext.Error(NoParamErrorID::FileIsAlreadyClosed);
		}

		if(fflush(file->fp)!=0) {
			return inContext.Error(NoParamErrorID::CanNotFlushFile);
		}
		NEXT;
	}));

	// S ---
	Install(new Word("mkdir",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(
					InvalidTypeErrorID::TosString,tos);
		}
		std::string dirPath=*tos.stringPtr;
		std::error_code ec;
		if( !std::filesystem::create_directory(dirPath,ec) ) {
			if( ec ) {
				return inContext.Error(
						ErrorIdWithString::CanNotCreateDir,
						ec.message());
			} else {
				return inContext.Error(
						ErrorIdWithString::DirAlreadyExists,
						dirPath);
			}
		}
		NEXT;
	}));

	// s1(=src) s2(=dest) ---
	Install(new Word("fcopy",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvDest=Pop(inContext.DS);
		if(tvDest.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tvDest);
		}
		TypedValue tvSrc=Pop(inContext.DS);
		if(tvSrc.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::SecondString,tvSrc);
		}
		if(std::filesystem::exists(*tvSrc.stringPtr)==false) {
			return inContext.Error(ErrorIdWithString::NoSuchFileExist,
								   *tvSrc.stringPtr);
		}
		try {
			std::filesystem::copy(tvSrc.stringPtr->c_str(),tvDest.stringPtr->c_str(),
								  std::filesystem::copy_options::overwrite_existing);
		} catch(...) {
			inContext.Error(NoParamErrorID::SystemError);
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
				inContext.DS.emplace_back(DataType::EoF);
			}
		} else {
			// from stdin
			if( std::getline(std::cin,line) ) {
				inContext.DS.emplace_back(line);
			} else {
				inContext.DS.emplace_back(DataType::EoF);
				std::cin.clear();
				clearerr(stdin);
			}
		}
		NEXT;
	}));

	Install(new Word("load",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tvFileName=Pop(inContext.DS);
		if(tvFileName.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tvFileName);
		}

		std::ifstream inputStream;
		inputStream.open(tvFileName.stringPtr->c_str());
		if( inputStream.fail() ) {
			const char *scriptFileDir=NULL;
			std::filesystem::path targetFilePath=tvFileName.stringPtr->c_str();
			if( targetFilePath.is_absolute() ) { goto openFileError; }

			scriptFileDir=GetScriptFileDir();
			if(scriptFileDir!=NULL) {
				std::filesystem::path path=scriptFileDir;
				path.append(tvFileName.stringPtr->c_str());
				std::string pathString=path.string();
				inputStream.open(pathString.c_str());
				if( inputStream.fail() ) { goto openFileError; }
			} else {
openFileError:
				return inContext.Error(ErrorIdWithString::CanNotReadFile,*tvFileName.stringPtr);
			}
		}
		std::string line;
		std::getline(inputStream,line);
		// the first line
		if(line.length()>1 && line[0]=='#') {
			// emtpy <- skip shebang.
		} else {
			if(OuterInterpreter(inContext,line,1)!=OIResult::NoError) {
				inContext.IS.clear();
				return false;
			}
		}
		int count=2;	// line count is 1-origin.
		while(std::getline(inputStream,line).eof()==false) {
			if(OuterInterpreter(inContext,line,count)!=OIResult::NoError) {
				fprintf(stderr,"load error at %d, line=%s\n",count,line.c_str());
				inContext.IS.clear();
				return false;
			}
			count++;
		}
		NEXT;
	}));

	Install(new Word("import",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}

		typedef void(*FuncPtr)(void);
		FuncPtr func=(FuncPtr)getCFuncPointer(inContext,tos.stringPtr.get(),"InitDict");
		if(func==NULL) {
			return inContext.Error(ErrorIdWithString::CanNotFindTheEntryPoint,*tos.stringPtr);
		}
		func();
		NEXT;
	}));

	// S(=libFilePath) S(=func-name) --- C(=stdCode)
	Install(new Word("get-std-code",WORD_FUNC {
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
		WordFunc func=(WordFunc)getCFuncPointer(inContext,
												second.stringPtr.get(),
												tos.stringPtr->c_str());
		if(func==NULL) {
			return inContext.Error(ErrorIdWithString::CanNotFindTheEntryPoint,
								   *tos.stringPtr);
		}
		inContext.DS.emplace_back(func);
		NEXT;
	}));

	Install(new Word("eof?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==DataType::EoF);
		NEXT;
	}));

	Install(new Word("@eof?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==DataType::EoF);
		NEXT;
	}));

	Install(new Word("inspect",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tv=ReadTOS(inContext.DS);
		tv.Dump();
		NEXT;
	}));

	Install(new Word("__size/tv__",WORD_FUNC {
		const char *name="__size/tv__";
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tv=Pop(inContext.DS);
		printf("sizeof(TypedValue)=%d\n",(int)sizeof(tv));
		printf("sizeof(dataType)=%d\n",(int)sizeof(tv.dataType));
		NEXT;
	}));
}

static bool fgetsMain(Context& inContext,File *inFile) {
	if(inFile->fp==NULL) {
		return inContext.Error(NoParamErrorID::FileIsAlreadyClosed);
	}
	Stack *us=(Stack*)(inFile->ungetStack);
	if(us->size()>0) {
		TypedValue tvString=Pop(us);
		inContext.DS.emplace_back(tvString);
		return true;
	}

	FILE *fp=inFile->fp;
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
					return true;
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
	return true;
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
		inContext.Error(ErrorIdWithString::CanNotOpenFile,*inLibPath);
		return NULL;
	}
	ret=GetProcAddress(hDLL,inFuncName);
	if(ret==NULL) {
		inContext.Error(ErrorIdWithString::CanNotFindTheEntryPoint,*inLibPath);
		if(FreeLibrary(hDLL)==0) {
			inContext.Error(ErrorIdWithString::CanNotCloseTheFile,*inLibPath);
		}
		return NULL;
	}
	// DO NOT CALL FreeLibrary(hDLL);
#else
	void *fh=dlopen(inLibPath->c_str(),RTLD_NOW | RTLD_NODELETE);
	if(fh==NULL) {
		inContext.Error(ErrorIdWithString::CanNotOpenFile,*inLibPath);
		return NULL;
	}
	ret=dlsym(fh,inFuncName);
	if(ret==NULL) {
		inContext.Error(ErrorIdWithString::CanNotFindTheEntryPoint,*inLibPath);
		if(dlclose(fh)!=0) {
			inContext.Error(ErrorIdWithString::CanNotCloseTheFile,*inLibPath);
		}
		return NULL;
	}
	if(dlclose(fh)!=0) {
		inContext.Error(ErrorIdWithString::CanNotCloseTheFile,*inLibPath);
		return NULL;
	}
#endif
	return ret;
}

const char *kSJIS="shift_jis";
const char *kUTF8="utf8";

static void printValue(const TypedValue& inTV,bool inTypePostfix) {
	const char *encodeFrom=NULL;
	const char *encodeTo=NULL;
	switch(gCCC) {
		case kCCC_THRU:
			if( gUseMockStdout ) {
				std::string valueString=inTV.GetValueString(-1,true,inTypePostfix);
				const char *s=valueString.c_str();
				for(int i=0; s[i]!='\0'; i++) {
					gMockStdout.emplace_back(s[i]);
				}
			} else {
				inTV.PrintValue(0,inTypePostfix);
			}
			return;
		case kUTF8_to_SJIS: encodeFrom=kUTF8; encodeTo=kSJIS; break;
		case kSJIS_to_UTF8: encodeFrom=kSJIS; encodeTo=kUTF8; break;
	}
	icu::UnicodeString src(inTV.GetValueString(-1,true,inTypePostfix).c_str(),
						   encodeFrom);
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

static void printStr(const std::string& inStr) {
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

