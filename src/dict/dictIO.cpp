#include <fstream>
#include <iostream>

#include <boost/format.hpp>

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

void InitDict_IO() {
	Install(new Word(".",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		printf(" ");
		tv.PrintValue();
		NEXT;
	}));

	Install(new Word("@.",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tv=ReadTOS(inContext.DS);
		printf(" ");
		tv.PrintValue();
		NEXT;
	}));

	Install(new Word("write",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		tv.PrintValue();
		NEXT;
	}));

	Install(new Word("putf",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tos);
		}
		try {
			boost::format fmt(tos.stringPtr->c_str());
			TypedValue second=Pop(inContext.DS);
			switch(second.dataType) {
				case kTypeInt:		fmt=fmt%second.intValue;	break;
				case kTypeLong:		fmt=fmt%second.longValue;	break;
				case kTypeFloat:	fmt=fmt%second.floatValue;	break;
				case kTypeDouble:	fmt=fmt%second.doubleValue;	break;
				case kTypeBigFloat:
				case kTypeBigInt:
				default:
					const int kNoIndent=0;
					fmt=fmt%second.GetValueString(kNoIndent).c_str();
			}
			std::cout << fmt;
		} catch(...) {
			return inContext.Error(E_FORMAT_DATA_MISMATCH);
		}
		NEXT;
	}));

	Install(new Word("cr",WORD_FUNC {
		printf("\n");
		NEXT;
	}));

	Install(new Word("flush-stdout",WORD_FUNC {
		fflush(stdout);
		NEXT;
	}));
	
	Install(new Word("open",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tv);
		}
		File *file=new File();
		if(file->Open(tv.stringPtr->c_str(),"a+")==false) {
			return inContext.Error(E_CAN_NOT_OPEN_FILE,*tv.stringPtr);
		}
		inContext.DS.emplace_back(file);
		NEXT;
	}));

	Install(new Word("new-file",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tv);
		}

		File *file=new File();
		if(file->Open(tv.stringPtr->c_str(),"w")==false) {
			return inContext.Error(E_CAN_NOT_CREATE_FILE,*tv.stringPtr);
		}
		inContext.DS.emplace_back(file);
		NEXT;
	}));

	Install(new Word("close",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tv=Pop(inContext.DS);
		if(tv.dataType!=kTypeFile) {
			return inContext.Error_InvalidType(E_TOS_FILE,tv);
		}

		File *file=tv.filePtr.get();
		if(file->fp==NULL) { return inContext.Error(E_FILE_IS_ALREADY_CLOSED); }
		if(file->Close()==false) { return inContext.Error(E_CAN_NOT_CLOSE_FILE); }
		NEXT;
	}));

	Install(new Word("to-read",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeFile) {
			return inContext.Error_InvalidType(E_TOS_FILE,tos);
		}

		File *file=tos.filePtr.get();
		if(file->fp==NULL) { return inContext.Error(E_FILE_IS_ALREADY_CLOSED); }

		fseek(file->fp,0L,SEEK_SET);
		NEXT;
	}));

	// file X --- file
	Install(new Word(">file",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tos=Pop(inContext.DS);
		TypedValue& tvFile=ReadTOS(inContext.DS);
		if(tvFile.dataType!=kTypeFile) {
			return inContext.Error_InvalidType(E_SECOND_FILE,tvFile);
		}	
		File *file=tvFile.filePtr.get();
		fprintf(file->fp,"%s",tos.GetValueString(0).c_str());
		NEXT;
	}));

	Install(new Word("fgets",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeFile) {
			return inContext.Error_InvalidType(E_TOS_FILE,tos);
		}

		File *file=tos.filePtr.get();
		if(file->fp==NULL) { return inContext.Error(E_FILE_IS_ALREADY_CLOSED); }

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
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeFile) {
			return inContext.Error_InvalidType(E_TOS_FILE,tos);
		}

		File *file=tos.filePtr.get();
		if(file->fp==NULL) { return inContext.Error(E_FILE_IS_ALREADY_CLOSED); }

		if(fflush(file->fp)!=0) { return inContext.Error(E_CAN_NOT_FLUSH_FILE); }
		NEXT;
	}));

	Install(new Word("get-line",WORD_FUNC {
		std::string line;
		if( std::getline(std::cin,line) ) {
			inContext.DS.emplace_back(line);
		} else {
			inContext.DS.emplace_back(kTypeEOF);
		}
		NEXT;
	}));

	Install(new Word("load",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tvFileName=Pop(inContext.DS);
		if(tvFileName.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tvFileName);
		}

		std::ifstream inputStream;
		inputStream.open(tvFileName.stringPtr->c_str());
		if( inputStream.fail() ) {
			return inContext.Error(E_CAN_NOT_READ_FILE,*tvFileName.stringPtr);
		}
		std::string line;
		std::getline(inputStream,line);
		if(line.length()>1 && line[0]=='#') {
			// emtpy <- skip shebang.
		} else {
			if(OuterInterpreter(inContext,line)==false) {
				return false;
			}
		}
		int count=0;
		while(std::getline(inputStream,line).eof()==false) {
			if(OuterInterpreter(inContext,line)==false) {
				fprintf(stderr,"Error at %d, line=%s\n",count,line.c_str());
				return false;
			}
			count++;
		}
		NEXT;
	}));

	Install(new Word("import",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tos);
		}

		typedef void(*FuncPtr)(void);

#ifdef _WIN32
		std::wstring dllFileName(tos.stringPtr->begin(), tos.stringPtr->end());
		HINSTANCE hDLL=LoadLibrary(dllFileName.c_str());
		if(hDLL==NULL) { return inContext.Error(E_CAN_NOT_OPEN_FILE,*tos.stringPtr); }
		FuncPtr func=(FuncPtr)GetProcAddress(hDLL,"InitDict");
		if(func==NULL) {
			inContext.Error(E_CAN_NOT_FIND_THE_ENTRY_POINT,*tos.stringPtr);
			if(FreeLibrary(hDLL)==0) {
				inContext.Error(E_CAN_NOT_CLOSE_THE_FILE,*tos.stringPtr);
			}
			return false;
		}
		func();
		// DO NOT CALL FreeLibrary(hDLL);
#else
		void *fh=dlopen(tos.stringPtr->c_str(),RTLD_NOW | RTLD_NODELETE);
		if(fh==NULL) { return inContext.Error(E_CAN_NOT_OPEN_FILE,*tos.stringPtr); }
		FuncPtr func=(FuncPtr)dlsym(fh,"InitDict");
		if(func==NULL) {
			inContext.Error(E_CAN_NOT_FIND_THE_ENTRY_POINT,*tos.stringPtr);
			if(dlclose(fh)!=0) {
				inContext.Error(E_CAN_NOT_CLOSE_THE_FILE,*tos.stringPtr);
			}
			return false;
		}
		func();
		if(dlclose(fh)!=0) {
			return inContext.Error(E_CAN_NOT_CLOSE_THE_FILE,*tos.stringPtr);
		}
#endif
		NEXT;
	}));

	Install(new Word("inspect",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tv=ReadTOS(inContext.DS);
		tv.Dump();
		NEXT;
	}));

	Install(new Word("__size/tv__",WORD_FUNC {
		const char *name="__size/tv__";
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tv=Pop(inContext.DS);
		printf("sizeof(TypedValue)=%d\n",(int)sizeof(tv));
		printf("sizeof(dataType)=%d\n",(int)sizeof(tv.dataType));
		NEXT;
	}));
}
