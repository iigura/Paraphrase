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
		  $(OBJ)/optimizer.o

DICT_OBJS=$(OBJ)/dictWord.o $(OBJ)/dictObject.o $(OBJ)/dictStack.o \
		  $(OBJ)/dictControl.o $(OBJ)/dictIO.o $(OBJ)/dictMath.o \
		  $(OBJ)/dictString.o $(OBJ)/dictArray.o $(OBJ)/dictList.o \
		  $(OBJ)/dictParallel.o \
		  $(OBJ)/dictOptimize.o

objs=$(LANG_OBJS) $(DICT_OBJS)

.PHONY: macos linux
.PHONY: all lang dict clean

.PHONY: macos-debug linux-debug

no_target:
	@echo "please specify the target operating system as argument of the make command,"
	@echo "for example: make macos."
	@echo ---
	@echo "for MacOS:   make macos"
	@echo "for Linux:   make linux"
	@echo "for Windows: please open windows/paraphrase.sln by VisualStudio."

macos: export CPPFLAGS=-std=c++1z -O3 -fno-rtti \
		-D NDEBUG -D USE_NOEXCEPT -D USE_PTHREAD -D USE_READLINE
macos: export LIB_NAME=libPP.so
macos: export SO_OPT  =-install_name @executable_path/./$(LIB_NAME)
macos: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (^^)v   |"
	@echo "+---------------------------------------+"

macos-debug: export CPPFLAGS=-std=c++1z -g -O0 -fno-rtti \
		-D DEBUG -D USE_NOEXCEPT -D USE_PTHREAD -D USE_READLINE
macos-debug: export LIB_NAME=libPP.a
macos-debug:  all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (@@)v   |"
	@echo "+---------------------------------------+"

macos-std_thread: export CPPFLAGS=-std=c++1z -O3 -fno-rtti \
		-D NDEBUG -D USE_NOEXCEPT -D USE_READLINE
macos-std_thread: export LIB_NAME=libPP.so
macos-std_thread: export SO_OPT  =-install_name @executable_path/./$(LIB_NAME)
macos-std_thread: all $(PPDIR)/$(PROG_NAME)
	@echo "+-----------------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (std::thread)   |"
	@echo "+-----------------------------------------------+"

linux: export CPPFLAGS=-std=c++1z -O3 -fPIC -fno-rtti \
		-D NDEBUG -D USE_PTHREAD -D USE_READLINE
linux: export LIB_NAME=libPP.so
linux: export RUNPATH_OPT=-Wl,-rpath,'$$ORIGIN'
linux: MDEP_LOPT=-ldl
linux: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (^^)v   |"
	@echo "+---------------------------------------+"

linux-debug: export CPPFLAGS=-std=c++1z -g -O0 -fPIC -fno-rtti \
		-D DEBUG -D USE_PTHREAD -D USE_READLINE
linux-debug: export RUNPATH_OPT=-Wl,-rpath,'$$ORIGIN'
linux-debug: MDEP_LOPT=-ldl
linux-debug: all $(PPDIR)/$(PROG_NAME)
	@echo "+---------------------------------------+"
	@echo "|   Paraphrase : BUILD SUCCESS! (@@)v   |"
	@echo "+---------------------------------------+"

linux-std_thread: export CPPFLAGS=-std=c++1z -O3 -fPIC -fno-rtti \
		-D NDEBUG -D USE_READLINE
linux-std_thread: export RUNPATH_OPT=-Wl,-rpath,'$$ORIGIN'
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
	-L$(PPDIR) -lPP $(PROFILER_OPT) -o $(PPDIR)/$(PROG_NAME) $(RUNPATH_OPT)

lang:
	make -C lang

dict:
	make -C dict

clean:
	make -C lang clean
	make -C dict clean
	rm -f $(PPDIR)/$(PROG_NAME)
	rm -f $(PPDIR)/libPP.so

