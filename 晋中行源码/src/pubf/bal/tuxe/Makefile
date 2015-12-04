
# ident "@(#)bal:main/bal.mak"

# inputs

BAL		=	.
#FLG		=	-g -KPIC -DDEBUG -DBAL_DEBUG
FLG		=	-DDEBUG -DBAL_DEBUG
 
# commands
 
SHELL	=	/bin/sh

CC		=	cc $(Q64)	
EC		=	esql
BS		=	buildserver
BC		=	buildclient -v
#LD		=	ld
LD		=	cc -G
AR		=	ar $(X64) -rv

RM		=	@rm -f
MV		=	@mv
CP		=	@cp
ECHO	=	@echo

# TUXEDO specific

RSM		=	Oracle_XA

# paths and flags
 
BIN		=	$(HOME)/bin
LIB		=	$(HOME)/lib
INC		=	$(HOME)/src/incl

DEF		=
 
CFLAG	=	$(FLG) $(DEF) $(Q64)
EFLAG	=	$(FLG) $(DEF)
#LFLAG	=	-shared
LFLAG	=
 
#APC_INC	=	$(HOME)/src/comm/tuxe/a_incl
#BAL_INC	=	$(HOME)/src/pubf/bal/tuxe/incl
APC_INC	=	./a_incl
PUB_INC	=	$(HOME)/src/incl_pub
BAL_INC	=	./incl
 
TX_INC	=	$(TUXDIR)/include
TX_LIB	=	$(TUXDIR)/lib

DB_LIB	=	$(INFORMIXDIR)/lib
EC_INC	=	$(INFORMIXDIR)/incl/esql
EC_LIB	=	$(INFORMIXDIR)/lib/esql
 
C_INCP	=	-I$(EC_INC) -I$(BAL_INC) -I$(TX_INC) -I$(INC) -I$(PUB_INC) -I.
C_LIBP	=	-L$(DB_LIB) -L$(EC_LIB) -L$(LIB) -L$TX_LIB -L$(HOME)/lib -L.
#C_INCP	=	-I$(TX_INC) -I$(INC) -I.
#C_LIBP	=	-L$(LIB) -L.
 
CFLAGS	=	$(CFLAG) $(C_INCP) $(C_LIBP) -D__GL_LOG__
EFLAGS	=	$(EFLAG) $(C_INCP) $(C_LIBP)
LFLAGS	=	$(LFLAG) $(C_LIBP)
APPINCLUDE	=-I$(EC_INC) -I$(BAL_INC) -I$(TX_INC) -I$(INC) -I. 
PROCINCLUDE	=include=$(EC_INC) include=$(BAL_INC) include=$(TX_INC) include=$(INC) include=. 
PROC	=proc
 
# libraries and targets

SQLLIBS = 	-lixsql -lixasf -lixgen -lixos -lixgls -lsocket 	\
			-lnsl -lmutex -lelf -lm -lc -lresolv \
			$(INFORMIXDIR)/lib/esql/checkapi.o -lixglx -lrbscomm

#UCBLIBS = /usr/ucblib/libucb.a
UCBLIBS = 
DESLIBS = $(HOME)/lib/libdes.a
#GENLIBS = /usr/lib/libgen.a
GENLIBS = 

LIBS =  $(UCBLIBS) $(DESLIBS) $(GENLIBS)

OBJS	= \
		balcipher.o		\
		apctl.o 		\
		appkey.o		\
		balkeycnv.o		\
		balmac.o		\
		balmain.o 		\
		baltx.o 		\
		callback.o		\
		chitty_cipher.o \
		cnvtin.o 		\
		cnvtout.o 		\
		cryperr.o		\
		dbsvc.o 		\
		encrypt.o		\
		nt_decrypt.o	\
		getclkey.o		\
		funcs.o 		\
		initinfo.o 		\
		pollcall.o		\
		process.o 		\
		srvdone.o 		\
		srvinit.o		\
		reqwait.o		\
		reqnowait.o		\
	        mobdo.o
BALFTPOBJS = balftp.o
RPTFTPOBJS = rptftp.o

TARGET	= \
		libbal3y.so \
		balftp \
		rptftp
all: Mstart $(TARGET) Mcomplete
 
Mstart:
	$(ECHO)
	$(ECHO) "MAKE bal:main/binary ..."
	$(ECHO)

Mcomplete:
	$(ECHO)
	$(ECHO) "MAKE bal:main/binary completed."
	$(ECHO)
 
libbal3y.so: $(OBJS)
	$(LD) $(Q64) -o $@ $(LFLAGS) $(OBJS) $(DESLIBS)
	$(CP) -f $@ $(HOME)/lib

libbal3y.a: $(OBJS)
	$(AR) $@ $(OBJS) #$(DESLIBS)
	$(CP) -f $@ $(HOME)/lib


#$(BIN)/balcrypt: $(CRYPTOBJS)
#	CFLAGS="$(CFLAGS)" \
#	TUXDIR="$(TUXDIR)" \
#	$(BS) -o $@ -r $(RSM) -s :balcrypt_main -f "$(CRYPTOBJS) $(LIBS) $(UCBLIBS)"
#	$(CP) -f $@ $(HOME)/bin

balftp: $(BALFTPOBJS)
	CFLAGS="$(CFLAGS)" \
	TUXDIR="$(TUXDIR)" \
	$(BS) -o $@ -r $(RSM) -s BALFTP -f "-brtl $(BALFTPOBJS) $(LIBS) $(UCBLIBS)"
	$(CP) -f $@ $(HOME)/bin

rptftp: $(RPTFTPOBJS)
	CFLAGS="$(CFLAGS)" \
	TUXDIR="$(TUXDIR)" \
	$(BS) -o $@ -r $(RSM) -s RPTFTP -f "-brtl $(RPTFTPOBJS) $(LIBS) $(UCBLIBS)"
	$(CP) -f $@ $(HOME)/bin

# implicit
 
#.SUFFIXES:
.SUFFIXES: .o .c .pc
 
.pc.o: 
	$(PROC) $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
	$(CC) -O -g -s $(CFLAGS) -c $*.c
	@rm -f $*.c
	@rm -f $*.lis
 
.c.o:
	$(CC) $(CFLAGS) -c $*.c
 
#.c:
#	$(CC) -o $@ $(CFLAGS) $*.c $(LIBS)
 
# default
 
#.DEFAULT:
#	$(CC) -o $@ $(CFLAGS) $(@F).c $(LIBS)
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS) 
 
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

