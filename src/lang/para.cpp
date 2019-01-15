// "Paraphrase" : a script language for parallel processing ;
//	by Koji Iigura.

#define VERSION "0.91.0"

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

TypedValue G_InvalidTV=TypedValue();
unsigned int G_NumOfCores=std::thread::hardware_concurrency();

const int kMaxHistory=255;

static const char *gInputFile=NULL;
static std::string (*gReadLineFunc)();
static bool gIsEOF=false;

static std::ifstream gFileStream;

static std::vector<std::string> gArgsForScriptFile;

static bool gDisplayTime=false;
static bool gRunWithReducedDict=false;
static bool gUsePrompt=true;
static bool gUseOkDisplay=true;

static bool initReadLineFunc();
static void initDict();
static bool parseOption(int argc,char *argv[]);
static std::string readFromFile();
static std::string readFromStdin();
static const char *getPrompt();

static void printUsage();
static void printVersion();

void InitDict_Word();
void InitDict_Object();
void InitDict_IO();
void InitDict_Math();
void InitDict_Stack();
void InitDict_Control(bool inOnlyUseCore);
void InitDict_String();
void InitDict_Array();
void InitDict_List();
void InitDict_Parallel();
void InitDict_Optimize();

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

	boost::timer::cpu_timer timer; 

	if(parseOption(argc,argv)==false) { return -1; }
	if(initReadLineFunc()==false) { return -1; }

	initDict();
	InitOptPattern();

	InitOuterInterpreter();

	if(gArgsForScriptFile.size()>0) {
		const size_t n=gArgsForScriptFile.size();	
		for(size_t i=0; i<n; i++) {
			std::string s=gArgsForScriptFile[i];	
			if(s[0]!='"' && s[0]!='\'') {
				s="'"+s+"'";
			}
			bool result=OuterInterpreter(*GlobalContext,s);	
			if(result==false) {
				return -1;
			}	
		}
	}

	SetCurrentVocName("user");
	while(gIsEOF==false) {
		std::string line=gReadLineFunc();
		if(line=="") { continue; }
		bool result=OuterInterpreter(*GlobalContext,line);
		if(gInputFile==NULL) {
			if(result==false) {
				if(GlobalContext->IsInterpretMode()==false) {
					GlobalContext->RemoveDefiningWord();
					GlobalContext->SetInterpretMode();
				}
			} else {
				if( gUseOkDisplay ) { puts(" ok."); }
			}
		} else if(result==false) {
			break;
		}
	}

	if(gInputFile!=NULL) {
		gFileStream.close();
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
	InitDict_Object();
	InitDict_IO();
	InitDict_Math();
	InitDict_Stack();
	InitDict_Control(gRunWithReducedDict);
	InitDict_String();
	InitDict_Array();
	InitDict_List();
	InitDict_Parallel();
	InitDict_Optimize();
}

static bool parseOption(int argc,char *argv[]) {
	namespace bstPrgOpt=boost::program_options;
	bstPrgOpt::options_description desc("options");
	desc.add_options()
		("help,h",		"pint help.")
		("version,v",	"print version info.")
		("quiet,q",		"quiet mode (equivalent to -nk).")
		("noprompt,n",	"suppress prompt.")
		("nook,k",		"suppress 'ok'.")
		("time,t",		"display spent time.")
		("refimp",		"run with reduced dict for reference implementation.");

	bstPrgOpt::variables_map vm=bstPrgOpt::variables_map {};
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
		std::cerr << e.what() << '\n';
		std::cerr << desc << '\n';
        return false;
	}
	
	if( vm.count("help")     ) { printUsage();	return false; }
	if( vm.count("version")  ) { printVersion(); return false; }
	if( vm.count("time")     ) { gDisplayTime=true; }
	if( vm.count("refimp")   ) { gRunWithReducedDict=true;    }
	if( vm.count("noprompt") ) { gUsePrompt=false; }
	if( vm.count("nook")     ) { gUseOkDisplay=false; }
	if( vm.count("quiet")    ) { gUsePrompt=gUseOkDisplay=false; }

	const size_t n=argVec.size();
	if(n>0) {
		gInputFile=strdup(argVec[0].c_str());
		for(size_t i=1; i<n; i++) {
			std::string s=argVec[i];	
			if(s.length()>1 && s[0]=='"' && s[s.length()-1]!='"') {
				for(++i; i<n; i++) {
					std::string t=argVec[i];
					if(t[0]=='"') {
						fprintf(stderr,
								"ERROR: illegal quotation at script arg[%zu]\n",i);
						return false;
					}	
					s+=" "+t;
					if(t.length()>1 && t[0]!='"' && t[t.length()-1]=='"') {
						break;
					}
				}
			}	
			gArgsForScriptFile.push_back(s);	
		}
	} else {
		gInputFile=NULL;	
	}
	return true;
}

static bool initReadLineFunc() {
	if(gInputFile==NULL) {
		stifle_history(kMaxHistory);
		read_history(NULL);
		gReadLineFunc=readFromStdin;
	} else {
		gFileStream.open(gInputFile);
		if(gFileStream.fail()) {
			return GlobalContext->Error(E_CAN_NOT_OPEN_FILE,std::string(gInputFile));
		}
		if(gFileStream.peek()=='#') {
			std::string skipLine;
			gIsEOF=std::getline(gFileStream,skipLine).eof();
		}
		gReadLineFunc=readFromFile;	
	}
	return true;
}

#ifdef USE_READLINE
static std::string readFromStdin() {
	char *line=readline(gUsePrompt ? getPrompt() : "");
		if(line!=NULL) {
			if(strlen(line)>0) {
				add_history(line);
			}
		} else {
			gIsEOF=true;
		}
		std::string ret=std::string(line==NULL ? "" : line);
	free(line);
	return ret;
}
#else
static std::string readFromStdin() {
	if( gUsePrompt ) { printf(getPrompt()); }
	std::string ret;
	gIsEOF=std::getline(std::cin,ret).eof();
	return ret;
}
#endif

static const char *getPrompt() {
	return GlobalContext->IsInterpretMode() ? "> " : ">>";
}

static std::string readFromFile() {
	std::string ret;
	gIsEOF=std::getline(gFileStream,ret).eof();
	return ret;
}

static void printUsage() {
	fprintf(stderr,
			"Usage: para [-nhv] [--refimp] [program-file] [value1 ... valueN]\n");
	fprintf(stderr,"  -h (--help)      print help.\n");
	fprintf(stderr,"  -v (--version)   print version.\n");
	fprintf(stderr,
			"  -q (--quiet)     suppress prompt and 'ok'. (equivalent to -nk)\n");
	fprintf(stderr,"  -n (--noprompt)  suppress prompt.\n");
	fprintf(stderr,"  -k (--nook))     suppress 'ok' message (no ok)\n");
	fprintf(stderr,"  -t (--time))     display spent time.\n");
	fprintf(stderr,"  --refimp         run with reduced dict "
									   "for reference implementation.\n");
}

static void printVersion() {
	printf("para: Paraphrase interpreter version %s by Koji Iigura.\n",kVersion);
}

