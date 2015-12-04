RM = rm
MV = mv
CC = cc $(Q64)
EC = esql $(Q64)
AR = ar -rv
LIBSRC = $(HOME)/src/pubf/base
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_ec \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) $(Q64) -qmaxmem=-1
APPINCLUDE= -I$(HOME)/src/incl_c -I$(HOME)/src/incl_ec -I$(HOME)/src/incl_pub
PROCINCLUDE= include=$(HOME)/src/incl_c include=$(HOME)/src/incl_ec include=$(HOME)/src/incl_pub
PROC	= proc


OBJ =	pubf_rpt_readpfm.o  strtools.o  
#sqltool.o

all:librpt.so

librpt.so: $(OBJ)
	$(CC) -G  -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib
	rm -f sqltool.c

librpt.a: $(OBJ)
	$(AR) $(X64) $@ $(OBJ)
	mv $@ $(HOME)/lib


#*******************************************/
.SUFFIXES: .pc .c .o
.pc.o:
	$(PROC) $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
	$(CC) -c $@ $(CCFLAGS) $*.c
	@rm -f $*.c
	@rm -f $*.lis
.c.o:
	$(CC)  -c $(CCFLAGS)  $(APPINCLUDE)  $*.c

clear:
	rm -f *.o 
