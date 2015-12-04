
# ident "@(#)bal:main/bal.mak"

# inputs

BAL		=	.
#FLG		=	-g -KPIC -DDEBUG -DBAL_DEBUG
FLG		=	-DDEBUG -DBAL_DEBUG
 
# commands
 
SHELL	=	/bin/sh

CC		=	cc $(Q64)
EC		=	esql 
PROC	=	proc
BS		=	buildserver
BC		=	buildclient -v
LD		=	$(CC) -G
#LD		=	makeC++SharedLib -p0 -G -bexpall -bnoentry  -brtl
AR		=	ar -rv

RM		=	@rm -f
MV		=	@mv
CP		=	@cp
ECHO	=	@echo

# TUXEDO specific

RSM		=	INFORMIX-OnLine

# paths and flags
 
BIN		=	$(HOME)/bin
LIB		=	$(HOME)/lib
INC		=	$(HOME)/src/incl_pub 

DEF		=
 
CFLAG	=	$(FLG) $(DEF)
EFLAG	=	$(FLG) $(DEF)
#LFLAG	=	-shared
LFLAG	=
 
#APC_INC	=	$(HOME)/src/comm/tcp/a_incl
#BAL_INC	=	$(HOME)/src/comm/tcp/incl
APC_INC	=	./a_incl
BAL_INC	=	./incl 
 
#TX_INC	=	$(TUXDIR)/include
#TX_LIB	=	$(TUXDIR)/lib

DB_LIB	=	$(INFORMIXDIR)/lib
EC_INC	=	$(INFORMIXDIR)/incl/esql
EC_LIB	=	$(INFORMIXDIR)/lib/esql
 
C_INCP	=	-I$(EC_INC) -I$(BAL_INC)  -I$(INC) -I.
PROCINCLUDE	=	include=$(EC_INC) include=$(BAL_INC)  include=$(INC) include=.
C_LIBP	=	-L$(DB_LIB) -L$(EC_LIB) -L$(LIB)  -L$(HOME)/lib -L.

#C_INCP	=	-I$(TX_INC) -I$(INC) -I. 
#C_LIBP	=	-L$(LIB) -L.
 
CFLAGS	=	$(CFLAG) $(C_INCP) $(C_LIBP)
EFLAGS	=	$(EFLAG) $(C_INCP) $(C_LIBP)
LFLAGS	=	$(LFLAG) $(C_LIBP)
 
# libraries and targets

SQLLIBS =	-lxdes -lixsql -lixasf -lixgen -lixos -lixgls -lsocket 	\
			-lnsl -lmutex -lelf -lm -lc -lresolv 	\
			$(INFORMIXDIR)/lib/esql/checkapi.o -lixglx -lrbscomm

#UCBLIBS = /usr/ucblib/libucb.a
UCBLIBS = 
DESLIBS = 
#GENLIBS = /usr/lib/libgen.a
GENLIBS = 

LIBS =  $(UCBLIBS) $(DESLIBS) $(GENLIBS)

OBJS	= \
		pgmsvr.o		\
		tradsvr.o		\
		balmain.o 		\
		apctl.o 		\
		cnvtin.o 		\
		cnvtout.o 		\
		dbsvc.o 		\
		nt_decrypt.o	\
		funcs.o 		\
		process.o

BALFTPOBJS = balftp.o

TARGET	= \
		libbaltcp.so  

all: Mstart $(TARGET) Mcomplete
 
Mstart:
	$(ECHO)
	$(ECHO) "MAKE bal:main/binary ..."
	$(ECHO)

Mcomplete:
	$(ECHO)
	$(ECHO) "MAKE bal:main/binary completed."
	$(ECHO)
 
libbaltcp.so: $(OBJS)
	$(LD) -o $@ $(LFLAGS) $(OBJS) $(DESLIBS)
#	$(AR) $(X64) $@ $(CFLAGS) $(OBJS) $(DESLIBS)
	$(CP) -f $@ $(HOME)/lib

libbaltcp.a: $(OBJS)
	$(AR) $(X64) $@ $(OBJS)
	$(CP) -f $@ $(HOME)/lib

#$(BIN)/balcrypt: $(CRYPTOBJS)
#	CFLAGS="$(CFLAGS)" \
#	TUXDIR="$(TUXDIR)" \
#	$(BS) -o $@ -r $(RSM) -s :balcrypt_main -f "$(CRYPTOBJS) $(LIBS) $(UCBLIBS)"
#	$(CP) -f $@ $(HOME)/bin
# implicit
 
#.SUFFIXES:
.SUFFIXES: .o .c .pc
 
.pc.o: 
	$(PROC) $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
	$(CC)  $(CFLAGS) -c $*.c
	@rm -f $*.c
	@rm -f $*.lis
 
.o:
	$(CC)  $(CFLAGS) -c $*.c
 
#.c:
#	$(CC) -o $@ $(CFLAGS) $*.c $(LIBS)
 
# default
 
#.DEFAULT:
#	$(CC) -o $@ $(CFLAGS) $(@F).c $(LIBS)
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS) olist.o
 
# dependencies

#balmain.o	: $(HDRFS)
#init.o		: $(HDRFS)
#get.o		: $(HDRFS)
#process.o	: $(HDRFS)
#put.o		: $(HDRFS)
#srvinit.o	: $(HDRFS)
#srvdone.o	: $(HDRFS)
#cnvtin.o	: $(HDRFS)
#cnvtout.o	: $(HDRFS)
#macom.o		: $(HDRFS)
#cipher.o	: $(HDRFS)
#funcs.o		: $(HDRFS)
#balerr.o	: $(HDRFS)
#balsub.o	: $(HDRFS)
#apctl.o		: $(HDRFS)
#dbsvc.o		: $(HDRFS)

