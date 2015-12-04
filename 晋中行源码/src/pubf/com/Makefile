RM = rm
MV = mv
CC = cc $(Q64) 

LIBSRC = $(HOME)/src/pubf/com
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_ec \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) -qmaxmem=-1


OBJ =	pubf_com_note.o pubf_com_cheq.o pubf_com_noteUse.o \
			pubf_com_noteUndo.o pubf_com_cash.o pubf_com_noteCXFD.o \
			pubf_com_noteCX.o pubf_com_auth.o pubf_com_errlog.o\
			pubf_com_QS.o pubf_com_chnl_detl.o pubf_com_CheckGZ.o

all:libcom.so

libcom.so: $(OBJ)
	$(CC) -G -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

libcom.a: $(OBJ)
		ar $(X64) -rv $@ $(OBJ)
		mv $@ $(HOME)/lib


.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
