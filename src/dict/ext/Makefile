INC=../../inc
OBJ=.
LIB=../../obj
PPDIR=../../..
EXT_BIN_DIR=$(PPDIR)/ext

INCLUDE_PATHS=-I $(INC)
LIB_PATHS=-L $(PPDIR)
LIB_NAME=libPP.so

extDicts=$(EXT_BIN_DIR)/extLP.so \
		 $(EXT_BIN_DIR)/extFactor.so \
		 $(EXT_BIN_DIR)/extAOP.so \
		 $(EXT_BIN_DIR)/extStdCode.so \
#		 $(EXT_BIN_DIR)/extVec3.so 

.PHONY: all clean

all: $(extDicts)

DICT_DEPS=$(INC)/externals.h \
		  $(INC)/typedValue.h $(INC)/paraphrase.h \
		  $(INC)/stack.h \
		  $(INC)/word.h $(INC)/array.h \
		  $(INC)/context.h $(INC)/outer.h \
		  $(INC)/chan.h \
		  $(INC)/inner.h

$(EXT_BIN_DIR)/%.so: %.cpp $(DICT_DEPS) $(PPDIR)/$(LIB_NAME)
	@if [ ! -d $(EXT_BIN_DIR) ]; then \
		echo "mkdir -p $(EXT_BIN_DIR)"; mkdir -p $(EXT_BIN_DIR); \
	fi
	$(CC) -shared $(CPPFLAGS) $(INCLUDE_PATHS) \
		$(LIB_PATHS) -lPP -o $@  $*.cpp

clean:
	rm -f $(extDicts)

