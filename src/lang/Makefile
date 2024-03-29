INC=../inc
OBJ=../obj
PPDIR=../..

INCLUDE_PATHS=-I $(INC)

LANG_OBJS=$(OBJ)/para.o

LIB_OBJS=$(OBJ)/outer.o $(OBJ)/inner.o \
		 $(OBJ)/word.o $(OBJ)/context.o $(OBJ)/typedValue.o $(OBJ)/chan.o \
		 $(OBJ)/threadMan.o \
		 $(OBJ)/dictMan.o $(OBJ)/errorMessage.o \
		 $(OBJ)/optimizer.o $(OBJ)/stack.o \
		 $(OBJ)/langSys.o $(OBJ)/util.o $(OBJ)/file.o

objs=$(LANG_OBJS) $(LIB_OBJS)

COMMON_HEADER=$(INC)/paraphrase.h \
			  $(INC)/word.h $(INC)/externals.h $(INC)/typedValue.h $(INC)/chan.h \
			  $(INC)/threadMan.h \
			  $(INC)/errorID.h $(INC)/array.h $(INC)/file.h $(INC)/outer.h

.PHONY: all clean

all: $(objs) $(PPDIR)/$(LIB_NAME)

$(OBJ)/%.o: %.cpp
	@if [ ! -d $(OBJ) ]; then \
		echo "mkdir -p $(OBJ)"; mkdir -p $(OBJ); \
	fi
	$(CC) $(CPPFLAGS) $(INCLUDE_PATHS) -o $@ -c $*.cpp

$(PPDIR)/libPP.so: $(LIB_OBJS)
	$(CC) -shared $(CPPFLAGS) $(LIB_OBJS) -lreadline $(SO_OPT) -o $(PPDIR)/$(LIB_NAME) 

$(PPDIR)/libPP.a: $(LIB_OBJS)
	ar rcs $(PPDIR)/libPP.a $(LIB_OBJS)

$(OBJ)/para.o: 			$(COMMON_HEADER) \
						$(INC)/stack.h $(INC)/context.h $(INC)/chan.h \
						$(INC)/lvop.h $(INC)/optimizer.h

$(OBJ)/outer.o: 		$(COMMON_HEADER) \
						$(INC)/stack.h $(INC)/context.h $(INC)/chan.h

$(OBJ)/inner.o: 		$(COMMON_HEADER) \
						$(INC)/stack.h $(INC)/context.h $(INC)/chan.h \
						$(INC)/inner.h

$(OBJ)/word.o:		$(COMMON_HEADER)

$(OBJ)/context.o:		$(COMMON_HEADER) \
						$(INC)/stack.h $(INC)/context.h $(INC)/chan.h \
						$(INC)/lvop.h $(INC)/optimizer.h \
						$(INC)/inner.h

$(OBJ)/typedValue.o:	$(COMMON_HEADER)

$(OBJ)/chan.o:			$(COMMON_HEADER)

$(OBJ)/threadMan.o:		$(COMMON_HEADER)

$(OBJ)/dictMan.o:		$(COMMON_HEADER)

$(OBJ)/stack.o:			$(COMMON_HEADER)

$(OBJ)/optimizer.o:		$(COMMON_HEADER) \
						$(INC)/lvop.h $(INC)/optimizer.h 

$(OBJ)/langSys.o:		$(COMMON_HEADER)

$(OBJ)/util.o:			$(COMMON_HEADER)

clean:
	rm -f $(objs)
	rm -f $(PPDIR)/libPP.so
	rm -f $(OBJ)/libPP.a

