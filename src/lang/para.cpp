// "Paraphrase" : a script language for parallel processing ;
//	by Koji Iigura.

#define VERSION "0.94.0"

#ifdef _MSVC_LANG
	#pragma comment(lib,"libPP.lib")
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <limits>

#include <thread>

#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>

#ifdef _MSVC_LANG
	#include <tchar.h>
#endif

#ifdef USE_READLINE
	#include <readline/readline.h>
	#include <readline/history.h>
#else
	// invalidation
	#define stifle_history(x)
	#define add_history(x)
	#define write_history(x)
	#define read_history(x)
#endif

#include "paraphrase.h"
#include "externals.h"
#include "word.h"
#include "stack.h"
#include "context.h"
#include "outer.h"
#include "optimizer.h"

#ifdef _WIN32
	#include <Windows.h>
#endif

const char *kVersion=VERSION;

TypedValue G_InvalidTV = TypedValue();

BigInt G_BI_DBL_MAX(DBL_MAX);
BigInt G_BI_MINUS_DBL_MAX(-DBL_MAX);

const int kMaxHistory=255;

static std::deque<TypedValue> *gTvListForArgs=new std::deque<TypedValue>();
static const char *gInputFilePath=NULL;
static std::string (*gReadLineFunc)();
static bool gIsEOF=false;

static std::ifstream gFileStream;

static std::vector<std::string> gArgsToExec;
static bool gSimpleOneLiner=false;

static bool gDisplayTime=false;
static bool gUsePrompt=true;
static bool gUseOkDisplay=true;
static std::string gToEvalStr="";
static bool gEvalAndExit=false;

static bool initReadLineFunc();
static void initDict();
static bool parseOption(int argc,char *argv[]);
static std::string readFromFile();
static std::string readFromStdin();
static const char *getPrompt();

static void printUsage();
static void printVersion();

PP_API void InitErrorMessage();
void InitDict_Word();
void InitDict_LangSys();
void InitDict_Object();
void InitDict_IO();
void InitDict_Math();
void InitDict_Stack();
void InitDict_Control();
void InitDict_String();
void InitDict_Array();
void InitDict_List();
void InitDict_Assoc();
void InitDict_Parallel();
void InitDict_LocalVar();
void InitDict_Optimize();
void InitDict_AOP();
void InitDict_Debug();

void RunningOnInteractive();

int main(int argc,char *argv[]) {
#if DEBUG || _DEBUG
	printf("!!! THIS IS DEBUG VERSION !!!\n");
	#ifdef _MSVC_LANG
		TCHAR cdir[255];
		GetCurrentDirectory(255,cdir);
		printf("current=%ls\n",cdir);
		printf("argv[0]=%s\n",argv[0]);
	#endif
#endif

// check data size for TypedValue optimize hack.
static_assert(sizeof(double)>=sizeof(Word*) 
		   && sizeof(double)>=sizeof(Word**)
		   && sizeof(double)>=sizeof(bool)
		   && sizeof(double)>=sizeof(int) 
		   && sizeof(double)>=sizeof(long)
		   && sizeof(double)>=sizeof(float)
		   && sizeof(double)>=sizeof(WordFunc),
			  "SYSTEM ERROR: unexpected compile system. sizeof(double) is not max.");
#if 0
	fprintf(stderr,"sizeof(Word*)  =%lu\n",sizeof(Word*));
	fprintf(stderr,"sizeof(Word**) =%lu\n",sizeof(Word**));
	fprintf(stderr,"sizeof(bool)   =%lu\n",sizeof(bool));
	fprintf(stderr,"sizeof(int)    =%lu\n",sizeof(int));
	fprintf(stderr,"sizeof(long)   =%lu\n",sizeof(long));
	fprintf(stderr,"sizeof(float)  =%lu\n",sizeof(float));
	fprintf(stderr,"sizeof(double) =%lu\n",sizeof(double));
	fprintf(stderr,"sizeof(WordFunc)=%lu\n",sizeof(WordFunc));
	exit(-1);
#endif

	boost::timer::cpu_timer timer; 

	if(parseOption(argc,argv)==false) { return -1; }
	if(initReadLineFunc()==false) { return -1; }

	InitErrorMessage();
	initDict();
	InitOptPattern();
	InitOuterInterpreter();
	SetCurrentVocName("user");
	
	if(gToEvalStr!="") {
		OIResult result=OuterInterpreter(*GlobalContext,gToEvalStr);
		if(result!=OIResult::OI_NO_ERROR) { return -1; }
	}
	if(gInputFilePath==NULL && gEvalAndExit ) { return 0; }

	for(int i=0; i<gArgsToExec.size(); i++) {
		std::string token=gArgsToExec[i];
		TypedValue tv=GetTypedValue(token);
		if(tv.dataType==DataType::kTypeInvalid) {
			tv=TypedValue(token);
			tv.dataType=DataType::kTypeMayBeAWord;
		}
		gTvListForArgs->emplace_back(tv);
	}
	G_ARGS=TypedValue(gTvListForArgs);

	if(gSimpleOneLiner==false) {
		if(gInputFilePath==NULL) { printVersion(); }
		while(gIsEOF==false) {
			std::string line=gReadLineFunc();
			OIResult result=OuterInterpreter(*GlobalContext,line);
			if(gInputFilePath==NULL) {
				if(result!=OIResult::OI_NO_ERROR) {
					if(GlobalContext->IsInterpretMode()==false) {
						GlobalContext->RemoveDefiningWord();
						GlobalContext->SetInterpretMode();
						GlobalContext->IS.clear();
					}
				} else {
					if(gUseOkDisplay && GlobalContext->IsInterpretMode()) {
						puts(" ok.");
					}
				}
			} else if(result!=OIResult::OI_NO_ERROR) {
				break;
			}
		}
	}

	if(gArgsToExec.size()>0) {
		const size_t n=gArgsToExec.size();	
		for(size_t i=0; i<n; i++) {
			std::string s=gArgsToExec[i];	
			OIResult result=OuterInterpreter(*GlobalContext,s);	
			if(result!=OIResult::OI_NO_ERROR) { return -1; }	
		}
	}

	if(gInputFilePath!=NULL) {
		if(!gSimpleOneLiner) { gFileStream.close(); }
	} else {
		write_history(NULL);
	}

	if( gDisplayTime ) {
		std::string result=timer.format(4,"real: %w[s]\n"
                                          "user: %u[s]\n"
										  "sys : %s[s]");
    	std::cout << result << std::endl;
	}

	return 0;
}

static void initDict() {
	Dict.reserve(1024);

	InitDict_Word();
	InitDict_LangSys();
	InitDict_Object();
	InitDict_IO();
	InitDict_Math();
	InitDict_Stack();
	InitDict_Control();
	InitDict_String();
	InitDict_Array();
	InitDict_List();
	InitDict_Assoc();
	InitDict_Parallel();
	InitDict_LocalVar();
	InitDict_Optimize();
	InitDict_AOP();
	InitDict_Debug();
}

static bool parseOption(int argc,char *argv[]) {
	namespace bstPrgOpt=boost::program_options;
	bstPrgOpt::options_description desc("options");
	desc.add_options()
		("help,h",		"print help.")
		("version,v",	"print version info.")
		("quiet,q",		"quiet mode (equivalent to -nk).")
		("thread",bstPrgOpt::value<int>(),"set maximum thread (ex: --thread 8).")
		("eval,e",bstPrgOpt::value<std::string>(),"eval parameter.")
		("eval-and-exit,E",bstPrgOpt::value<std::string>(),
		 				"eval and exit (for one liner).")
		("noprompt,n",	"suppress prompt.")
		("nook,k",		"suppress 'ok'.")
		("time,t",		"display spent time.");
		//("refimp",		"run with reduced dict for reference implementation.");

	bstPrgOpt::variables_map vm; // =bstPrgOpt::variables_map {};
	std::vector<std::string> argVec;
	try {
		auto const parseResult=bstPrgOpt::parse_command_line(argc,argv,desc);
		store(parseResult,vm);
		notify(vm);

		for(auto const& str
		  : bstPrgOpt::collect_unrecognized(parseResult.options,
											bstPrgOpt::include_positional)) {
			argVec.push_back(str);	
		}
	} catch(const bstPrgOpt::error& e) {
		std::cerr << e.what() << std::endl << desc << std::endl;
        return false;
	}
	
	if( vm.count("help")     ) { printUsage();	return false; }
	if( vm.count("version")  ) { printVersion(); return false; }
	if( vm.count("time")     ) { gDisplayTime=true; }
	if( vm.count("noprompt") ) { gUsePrompt=false; }
	if( vm.count("nook")     ) { gUseOkDisplay=false; }
	if( vm.count("quiet")    ) { gUsePrompt=gUseOkDisplay=false; }
	if( vm.count("thread")	 ) {
		int n=vm["thread"].as<int>();
		if(n<0) {
			std::cerr << "--thread argument should be a positive integer (current argument is " << n << ")." << std::endl;
			return false;
		}
		G_NumOfCores=n;
	}
	if( vm.count("eval") ) {
		// this code needs RTTI. do not use -fno-rtti.
		gToEvalStr=vm["eval"].as<std::string>();
	}
	if( vm.count("eval-and-exit") ) {
		gToEvalStr=vm["eval-and-exit"].as<std::string>();
		gEvalAndExit=true;
	}

	const size_t n=argVec.size();
	if(n>0) {
		gInputFilePath=strdup(argVec[0].c_str());
		for(size_t i=1; i<n; i++) {
			std::string s=argVec[i];	
			if(s.length()>1 && s[0]=='"' && s[s.length()-1]!='"') {
				for(++i; i<n; i++) {
					std::string t=argVec[i];
					if(t[0]=='"') {
						std::cerr << "ERROR: illegal quotation at script arg[" << i << "]" << std::endl;
						return false;
					}	
					s+=" "+t;
					if(t.length()>1 && t[0]!='"' && t[t.length()-1]=='"') {
						break;
					}
				}
			}	
			gArgsToExec.push_back(s);	
		}
	} else {
		gInputFilePath=NULL;	
	}
	return true;
}

static bool initReadLineFunc() {
	if(gInputFilePath==NULL) {
		stifle_history(kMaxHistory);
		read_history(NULL);
		gReadLineFunc=readFromStdin;
		RunningOnInteractive();
	} else {
		if(strlen(gInputFilePath)==0) {
			gSimpleOneLiner=true;
		} else {
			gFileStream.open(gInputFilePath);
			if(gFileStream.fail()) {
				return GlobalContext->Error(ErrorIdWithString::E_CAN_NOT_OPEN_FILE,
											std::string(gInputFilePath));
			}
			if(gFileStream.peek()=='#') {
				std::string skipLine;
				gIsEOF=std::getline(gFileStream,skipLine).eof();
			}
			gReadLineFunc=readFromFile;	
		}
	}
	return true;
}

static std::string readFromStdin() {
	bool isEOF;
	std::string ret=ReadFromStdin(&isEOF,gUsePrompt ? getPrompt() : "");
	gIsEOF=isEOF;
	return ret;
}

static const char *getPrompt() {
	return GlobalContext->IsInterpretMode() ? " > " : ">> ";
}

static std::string readFromFile() {
	std::string ret;
	gIsEOF=std::getline(gFileStream,ret).eof();
	return ret;
}

static void printUsage() {
	std::cerr
		<< "Usage: para [-ehknqtv] [--thread]  [program-file] [program-code]" << std::endl
		<< "  -h (--help)      print help." << std::endl
		<< "  -v (--version)   print version." << std::endl
		<< "  --thread N       set maximum thread. N should be a positive integer." << std::endl
		<< "  -e (--eval) S    evaluate the string S before executing the given script file." << std::endl
		<< "  -E (--eval-and-exit) S  evaluate the string S and exit." << std::endl
		<< "  -q (--quiet)     suppress prompt and 'ok'. (equivalent to -nk)" << std::endl
		<< "  -n (--noprompt)  suppress prompt." << std::endl
		<< "  -k (--nook)      suppress 'ok' message (no ok)" << std::endl
		<< "  -t (--time)      display spent time." << std::endl
		// << "  --refimp         run with reduced dict for reference implementation." << std::endl
		<< "Ex) para FizzBuzz.pp 1 20 for+ i FizzBuzz . next cr" << std::endl;
}

static void printVersion() {
	// printf("para: Paraphrase interpreter version %s by Koji Iigura.\n",kVersion);
	std::cout << "Paraphrase " << kVersion << "  Copyright (C) 2018-2021 Koji Iigura" << std::endl;
}

