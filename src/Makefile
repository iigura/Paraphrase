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
		  $(OBJ)/context.o $(OBJ)/typedValue.o \
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
	@echo "for Intel MacOS:   make macos  or  make macos-debug"
	@echo "for M1 MacOS:      make m1mac  or  make m1mac-debug"
	@echo "for Linux:         make linux  or  make linux-debug"
	@echo "for Windows:       please open windows/paraphrase.sln by VisualStudio."
	@echo "for test:          make test   or  make test-failure"

macos: export CPPFLAGS=-std=c++1z -fomit-frame-pointer -fno-stack-check -O3 \
		-D NDEBUG -D USE_NOEXCEPT -D USE_PTHREAD -D USE_READLINE \
		-D FOR_MACOS \
		-I$(HOMEBREW_PREFIX)/opt/icu4c/include
macos: export LIB_NAME=libPP.so
macos: export SO_OPT  =-install_name @executable_path/./$(LIB_NAME)
macos: ICU4C_OPT=$(shell icu-config --cppflags --ldflags --ldflags-icuio)
macos: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (^^)v   |"
	@echo "+---------------------------------------+"

macos-debug: export CPPFLAGS=-std=c++1z -g -O0 \
		-D DEBUG -D USE_NOEXCEPT -D USE_PTHREAD -D USE_READLINE -D FOR_MACOS \
		-I$(HOMEBREW_PREFIX)/opt/icu4c/include
macos-debug: export LIB_NAME=libPP.a
macos-debug: ICU4C_OPT=$(shell icu-config --cppflags --ldflags --ldflags-icuio)
macos-debug:  all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (@@)v   |"
	@echo "+---------------------------------------+"

macos-std_thread: export CPPFLAGS=-std=c++1z -O3 \
		-D NDEBUG -D USE_NOEXCEPT -D USE_READLINE -D FOR_MACOS
macos-std_thread: export LIB_NAME=libPP.so
macos-std_thread: export SO_OPT  =-install_name @executable_path/./$(LIB_NAME)
macos-std_thread: ICU4C_OPT=$(shell icu-config --cppflags --ldflags --ldflags-icuio)
macos-std_thread: all $(PPDIR)/$(PROG_NAME)
	@echo "+-----------------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (std::thread)   |"
	@echo "+-----------------------------------------------+"

m1mac: export CPPFLAGS=-std=c++1z -fomit-frame-pointer -fno-stack-check -O3 \
		-D NDEBUG -D USE_NOEXCEPT -D USE_PTHREAD -D USE_READLINE \
		-D FOR_MACOS \
		-I$(HOMEBREW_PREFIX)/include \
		-I$(HOMEBREW_PREFIX)/opt/icu4c/include
m1mac: export LIB_NAME=libPP.so
m1mac: export SO_OPT  =-install_name @executable_path/./$(LIB_NAME)
m1mac: ICU4C_OPT=$(shell icu-config --cppflags --ldflags --ldflags-icuio)
m1mac: MDEP_LOPT=-L$(HOMEBREW_PREFIX)/lib
m1mac: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (^^)v   |"
	@echo "+---------------------------------------+"

m1mac-debug: export CPPFLAGS=-std=c++1z -g -O0 \
		-D DEBUG -D USE_NOEXCEPT -D USE_PTHREAD -D USE_READLINE \
		-D FOR_MACOS \
		-I$(HOMEBREW_PREFIX)/include \
		-I$(HOMEBREW_PREFIX)/opt/icu4c/include
m1mac-debug: export LIB_NAME=libPP.a
m1mac-debug: ICU4C_OPT=$(shell icu-config --cppflags --ldflags --ldflags-icuio)
m1mac-debug: MDEP_LOPT=-L$(HOMEBREW_PREFIX)/lib
m1mac-debug: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (@@)v   |"
	@echo "+---------------------------------------+"


linux: export CPPFLAGS=-std=c++1z -O3 -fPIC -Wno-psabi \
		-D NDEBUG -D USE_PTHREAD -D USE_READLINE -D FOR_LINUX
linux: export LIB_NAME=libPP.so
linux: export RUNPATH_OPT=-Wl,-rpath,'$$ORIGIN'
linux: MDEP_LOPT=-ldl
linux: ICU4C_OPT=$(shell pkg-config --libs --cflags icu-uc icu-io)
linux: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (^^)v   |"
	@echo "+---------------------------------------+"

linux-debug: export CPPFLAGS=-std=c++1z -g -O0 -fPIC -Wno-psabi \
		-D DEBUG -D USE_PTHREAD -D USE_READLINE -D FOR_LINUX
linux-debug: export RUNPATH_OPT=-Wl,-rpath,'$$ORIGIN'
linux-debug: ICU4C_OPT=$(shell pkg-config --libs --cflags icu-uc icu-io)
linux-debug: MDEP_LOPT=-ldl
linux-debug: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (@@)v   |"
	@echo "+---------------------------------------+"

linux-std_thread: export CPPFLAGS=-std=c++1z -O3 -fPIC -Wno-psabi \
		-D NDEBUG -D USE_READLINE -D FOR_LINUX
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
	../para $(SAMPLES_DIR)/99.p8e test
	../para -E "1 2.3 'str' ( a ( b c ) )" $(SAMPLES_DIR)/check-scriptFile-args.p8e test
	../para $(SAMPLES_DIR)/countPrimeMT-short.p8e test
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

