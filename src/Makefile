.ONESHELL:

# export CC=clang++
export CC=g++
# --- for debug build
# export CPPFLAGS=-std=c++1z -Og -fno-rtti

# export PROFILER_OPT=-lprofiler -Wl,-no_pie

PROG_NAME=para

PPDIR=..
BIN=bin
OBJ=obj

LANG_OBJS=$(OBJ)/para.o 

LIB_OBJS= $(OBJ)/outer.o $(OBJ)/inner.o \
		  $(OBJ)/context.o $(OBJ)/typedValue.o $(OBJ)/chan.o $(OBJ)/threadMan.o \
		  $(OBJ)/dictMan.o $(OBJ)/errorMessage.o \
		  $(OBJ)/optimizer.o $(OBJ)/stack.o \
		  $(OBJ)/langSys.o $(OBJ)/util.o

DICT_OBJS=$(OBJ)/dictWord.o $(OBJ)/dictLangSys.o \
		  $(OBJ)/dictObject.o $(OBJ)/dictStack.o \
		  $(OBJ)/dictControl.o $(OBJ)/dictIO.o $(OBJ)/dictMath.o $(OBJ)/dictType.o \
		  $(OBJ)/dictString.o $(OBJ)/dictArray.o $(OBJ)/dictList.o $(OBJ)/dictAssoc.o \
		  $(OBJ)/dictParallel.o $(OBJ)/dictLocalVar.o \
		  $(OBJ)/dictOptimize.o \
		  $(OBJ)/dictAOP.o $(OBJ)/dictDebug.o

objs=$(LANG_OBJS) $(DICT_OBJS)

SAMPLES_DIR=../samples

.PHONY: macos linux
.PHONY: all lang dict clean

.PHONY: macos-debug linux-debug

no_target:
	@echo "please specify the target operating system as argument of the make command,"
	@echo "for example: make macos."
	@echo ---
	@echo "for MacOS:   make macos  or  make macos-debug"
	@echo "for Linux:   make linux  or  make linux-debug"
	@echo "for Windows: please open windows/paraphrase.sln by VisualStudio."
	@echo "for test:    make test   or  make test-light or make test-failure"

homebrew-prefix:
ifndef HOMEBREW_PREFIX
	@echo
	@echo "!!!!! PLEASE SET HOMEBREW_PREFIX !!!"
	@echo please set \"export HOMEBREW_PREFIX='$$'\(brew --prefix\)\" in your environment.
	@echo
endif

macos: export CPPFLAGS=-std=c++1z -fomit-frame-pointer -fno-stack-check -O3 \
		-D NDEBUG -D USE_NOEXCEPT -D USE_READLINE \
		-D USE_G_DOCOL_ADVICE \
		-D FOR_MACOS \
		-I$(HOMEBREW_PREFIX)/include \
		-I$(HOMEBREW_PREFIX)/opt/icu4c/include \
		-I$(HOMEBREW_PREFIX)/opt/libffi/include
macos: export LIB_NAME=libPP.so
macos: export SO_OPT  =-install_name @executable_path/./$(LIB_NAME)
macos: ICU4C_OPT=$(shell icu-config --cppflags --ldflags --ldflags-icuio)
macos: MDEP_LOPT=-L$(HOMEBREW_PREFIX)/lib
macos: homebrew-prefix all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (^^)v   |"
	@echo "+---------------------------------------+"

macos-debug: export CPPFLAGS=-std=c++1z -g -O0 \
		-D DEBUG -D USE_NOEXCEPT -D USE_READLINE \
		-D USE_G_DOCOL_ADVICE \
		-D FOR_MACOS \
		-I$(HOMEBREW_PREFIX)/include \
		-I$(HOMEBREW_PREFIX)/opt/icu4c/include \
		-I$(HOMEBREW_PREFIX)/opt/libffi/include
macos-debug: export LIB_NAME=libPP.a
macos-debug: export SO_OPT  =-install_name @executable_path/./$(LIB_NAME)
macos-debug: ICU4C_OPT=$(shell icu-config --cppflags --ldflags --ldflags-icuio)
macos-debug: MDEP_LOPT=-L$(HOMEBREW_PREFIX)/lib
macos-debug: homebrew-prefix all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (@@)v   |"
	@echo "+---------------------------------------+"

macos-std_thread: export CPPFLAGS=-std=c++1z -O3 \
		-D NDEBUG -D USE_NOEXCEPT -D USE_READLINE \
		-D USE_G_DOCOL_ADVICE \
		-D FOR_MACOS \
		-I$(HOMEBREW_PREFIX)/include \
		-I$(HOMEBREW_PREFIX)/opt/icu4c/include \
		-I$(HOMEBREW_PREFIX)/opt/libffi/include
macos-std_thread: export LIB_NAME=libPP.so
macos-std_thread: export SO_OPT  =-install_name @executable_path/./$(LIB_NAME)
macos-std_thread: ICU4C_OPT=$(shell icu-config --cppflags --ldflags --ldflags-icuio)
macos-std_thread: MDEP_LOPT=-L$(HOMEBREW_PREFIX)/lib
macos-std_thread: homebrew-prefix all $(PPDIR)/$(PROG_NAME)
	@echo "+-----------------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (std::thread)   |"
	@echo "+-----------------------------------------------+"

macos-pthread: export CPPFLAGS=-std=c++1z -fomit-frame-pointer -fno-stack-check -O3 \
		-D NDEBUG -D USE_NOEXCEPT -D USE_READLINE \
		-D USE_PTHREAD \
		-D USE_G_DOCOL_ADVICE \
		-D FOR_MACOS \
		-I$(HOMEBREW_PREFIX)/include \
		-I$(HOMEBREW_PREFIX)/opt/icu4c/include \
		-I$(HOMEBREW_PREFIX)/opt/libffi/include
macos-pthread: export LIB_NAME=libPP.so
macos-pthread: export SO_OPT  =-install_name @executable_path/./$(LIB_NAME)
macos-pthread: ICU4C_OPT=$(shell icu-config --cppflags --ldflags --ldflags-icuio)
macos-pthread: MDEP_LOPT=-L$(HOMEBREW_PREFIX)/lib
macos-pthread: homebrew-prefix all $(PPDIR)/$(PROG_NAME)
	@echo "+-----------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (pthread) |"
	@echo "+-----------------------------------------+"
	@echo "!!! NOTE: Currently on the M1 version of macOS,   !!!"
	@echo "!!!       the pthread version crashes at runtime. !!!"

linux: export CPPFLAGS=-std=c++1z -O3 -fPIC -Wno-psabi \
	-D NDEBUG -D USE_READLINE \
	-D USE_G_DOCOL_ADVICE \
	-D FOR_LINUX
linux: export LIB_NAME=libPP.so
linux: export RUNPATH_OPT=-Wl,-rpath,'$$ORIGIN'
linux: MDEP_LOPT=-ldl
linux: ICU4C_OPT=$(shell pkg-config --libs --cflags icu-uc icu-io)
linux: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (^^)v   |"
	@echo "+---------------------------------------+"

linux-debug: export CPPFLAGS=-std=c++1z -g -O0 -fPIC -Wno-psabi \
	-D DEBUG -D USE_READLINE \
	-D USE_G_DOCOL_ADVICE \
	-D FOR_LINUX
linux-debug: export LIB_NAME=libPP.so
linux-debug: export RUNPATH_OPT=-Wl,-rpath,'$$ORIGIN'
linux-debug: MDEP_LOPT=-ldl
linux-debug: ICU4C_OPT=$(shell pkg-config --libs --cflags icu-uc icu-io)
linux-debug: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (@@)v   |"
	@echo "+---------------------------------------+"

linux-pthread: export CPPFLAGS=-std=c++1z -O3 -fPIC -Wno-psabi \
	-D NDEBUG -D USE_PTHREAD -D USE_READLINE \
	-D USE_G_DOCOL_ADVICE \
	-D FOR_LINUX
linux-pthread: export LIB_NAME=libPP.so
linux-pthread: export RUNPATH_OPT=-Wl,-rpath,'$$ORIGIN'
linux-pthread: MDEP_LOPT=-ldl
linux-pthread: ICU4C_OPT=$(shell pkg-config --libs --cflags icu-uc icu-io)
linux-pthread: all $(PPDIR)/$(PROG_NAME)
	@echo "+-------------------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (^^)v (pthread)   |"
	@echo "+-------------------------------------------------+"

linux-std_thread: export CPPFLAGS=-std=c++1z -O3 -fPIC -Wno-psabi \
	-D NDEBUG -D USE_READLINE \
	-D USE_G_DOCOL_ADVICE \
	-D FOR_LINUX
linux-std_thread: export RUNPATH_OPT=-Wl,-rpath,'$$ORIGIN'
linux-std_thread: ICU4C_OPT=$(shell pkg-config --libs --cflags icu-uc icu-io)
linux-std_thread: MDEP_LOPT=-ldl
linux-std_thread: all $(PPDIR)/$(PROG_NAME)
	@echo "+-----------------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (std::thread)   |"
	@echo "+-----------------------------------------------+"

all: lang dict $(PPDIR)/$(PROG_NAME)


$(PPDIR)/$(PROG_NAME): $(objs)
	$(CC) $(LANG_OBJS) $(DICT_OBJS) \
	-lreadline -lpthread $(MDEP_LOPT) \
	-lboost_program_options -lboost_system -lboost_timer \
	-L$(PPDIR) -lPP $(PROFILER_OPT) \
	$(ICU4C_OPT) \
	-o $(PPDIR)/$(PROG_NAME) $(RUNPATH_OPT) 
lang:
	make -C lang

dict:
	make -C dict

clean:
	make -C lang clean
	make -C dict clean
	rm -f $(PPDIR)/$(PROG_NAME)
	rm -f $(PPDIR)/libPP.so

test:
	@echo 99.p8e
	../para $(SAMPLES_DIR)/99.p8e test 
	@echo check-scriptFile-args.p8e
	../para -E "1 2.3 'str' ( a ( b c ) )" $(SAMPLES_DIR)/check-scriptFile-args.p8e test 
	@echo attr.p8e
	../para $(SAMPLES_DIR)/attr.p8e test
	@echo countPrimeMT-short.p8e
	../para $(SAMPLES_DIR)/countPrimeMT-short.p8e test
	@echo dumpMPmt.p8e
	../para $(SAMPLES_DIR)/dumpMPmt.p8e test
	@echo dynamic-scoping.p8e
	../para $(SAMPLES_DIR)/dynamic-scoping.p8e test
	@echo fact-tr.p8e
	../para $(SAMPLES_DIR)/fact-tr.p8e test
	@echo fastInvSqrt.p8e
	../para $(SAMPLES_DIR)/fastInvSqrt.p8e test
	@echo FizzBuzz.p8e
	../para $(SAMPLES_DIR)/FizzBuzz.p8e test
	@echo join.p8e
	../para $(SAMPLES_DIR)/join.p8e test
	@echo Mersenne.p8e
	../para $(SAMPLES_DIR)/Mersenne.p8e test
	@echo PE10mt.p8e
	../para $(SAMPLES_DIR)/PE10mt.p8e test
	@echo PE21mt.p8e
	../para $(SAMPLES_DIR)/PE21mt.p8e test
	@echo PE25.p8e
	../para $(SAMPLES_DIR)/PE25.p8e test
	@echo PE56.p8e
	../para $(SAMPLES_DIR)/PE56.p8e test
	@echo PolishNotation.p8e
	../para $(SAMPLES_DIR)/PolishNotation.p8e test
	@echo zdk.p8e
	../para $(SAMPLES_DIR)/zdk.p8e test
	@echo "===== The all test passed. ====="

test-light:
	../para $(SAMPLES_DIR)/99.p8e test
	../para -E "1 2.3 'str' ( a ( b c ) )" $(SAMPLES_DIR)/check-scriptFile-args.p8e test
	../para $(SAMPLES_DIR)/countPrimeMT-short.p8e test-light
	../para $(SAMPLES_DIR)/dumpMPmt.p8e test
	../para $(SAMPLES_DIR)/fact-tr.p8e test
	../para $(SAMPLES_DIR)/FizzBuzz.p8e test
	../para $(SAMPLES_DIR)/Mersenne.p8e test
	../para $(SAMPLES_DIR)/PE10mt.p8e test
	../para $(SAMPLES_DIR)/PE21mt.p8e test
	../para $(SAMPLES_DIR)/PE25.p8e test
	../para $(SAMPLES_DIR)/PE56.p8e test
	../para $(SAMPLES_DIR)/PolishNotation.p8e test
	../para $(SAMPLES_DIR)/zdk.p8e test
	@echo "===== The all test passed. ====="

test-failure:
	../para $(SAMPLES_DIR)/pass.p8e
	../para $(SAMPLES_DIR)/failure.p8e
	../para $(SAMPLES_DIR)/pass.p8e
	@echo "===== The all test passed. ====="

