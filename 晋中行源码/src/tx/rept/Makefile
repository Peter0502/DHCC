
# ident "@(#)apsvr:sa/si.mak"

# Thunder Bird

# inputs

APP		=	$(HOME)
FLG		=	-DDEBUG -DBAL_DEBUG
 
# commands
 
SHELL	=	sh

CC		=	cc $(Q64) -g
EC		=	esql $(Q64)
BS		=	buildserver
BC		=	buildclient
LD		=	ld
AR		=	ar ruc

RM		=	@rm -f
MV		=	@mv
CP		=	@cp
ECHO	=	@echo

# TUXEDO specific

RSM		=	INFORMIX-OnLine
BSTUB	=	:_balmain

# paths and flags
 
BIN		= 	$(APP)/bin
LIB		=	$(APP)/lib
INC		=	$(APP)/src/incl_c -I$(APP)/src/incl_pub

DEF		=	-DNOWHAT=1
 
CFLAG	=	$(FLG) $(DEF)
EFLAG	=	$(FLG) $(DEF)
LFLAG	=	$(FLG) 
 
CM_LIB	=	/usr/ucblib

AP_INC	=	$(APP)/src/pubf/bal/tcp/incl
AP_LIB	=	$(APP)/lib
 
TX_INC	=	$(TUXDIR)/include
TX_LIB	=	$(TUXDIR)/lib

DB_LIB	=	$(INFORMIXDIR)/lib
EC_INC	=	$(INFORMIXDIR)/incl/esql
EC_LIB	=	$(INFORMIXDIR)/lib/esql
 
C_INCP	=	-I$(EC_INC) -I$(TX_INC) -I$(AP_INC) -I$(INC) -I.
C_LIBP	=	-L$(EC_LIB) -L$(TX_LIB) -L$(CM_LIB) -L$(LIB) -L.
 
CFLAGS	=	$(CFLAG) $(C_INCP) $(C_LIBP) 
EFLAGS	=	$(CFLAG) $(C_INCP) $(C_LIBP)
LFLAGS	=	$(LFLAG)
 
# libraries and targets

#LIBUCB	=	/usr/ucblib/libucb.a
LIBUCB	=
LIBNET	=	$(DB_LIB)/netlib.a

LIBAPL  = -lacct -lln -lbase -lpubdb -lass -lcom -lde -lbaltcp -lcomm \
	-lpubdb_m -lcif -lagent -lc -lm -lcard -lod -lrpt -lrept -lpthread -llv -lhv


SQLLIBS	=	-lsql $(LIBNET) -los -lgen -lsocket -lnsl
#RBSLIBS	=	-ldes -lbhcomm
#DEPS    =   $(AP_LIB)/libbaltcp.a 

#BALLIBS	=	$(RBSLIBS) -ldbsvr
BALLIBS	=	$(LIBBAL) $(LIBAPL)

LIBS	=	$(LIBBAL) $(LIBUCB) $(BALLIBS)
APPINCLUDE	=-I$(HOME)/src/incl_c -I$(HOME)/src/incl_pub
PROCINCLUDE	=include=$(HOME)/src/incl_c include=$(HOME)/src/incl_pub
ORALIBDIR= -L$(ORACLE_HOME)/lib
APPLIBDIR	=-L$(HOME)/lib
ORALIB	=-lclntsh
PROC	= proc


OBJS	=	bbxxxx.o looppkg.o

TARGET	= \
		$(BIN)/bbxxxx\
		$(BIN)/looppkg

all: Mstart $(TARGET) Mcomplete cleanup
 
Mstart:
	$(ECHO)
	$(ECHO) "MAKE apsvr:$(TARGET) ..."
	$(ECHO)

Mcomplete:
	$(ECHO)
	$(ECHO) "MAKE apsvr:$(TARGET) completed."
	$(ECHO)
 
$(TARGET): $$(@F).o
	$(CC) -brtl -g $(ORALIBDIR) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) $(@F).o -o $@ $(ORALIB) $(TUSERLIB) $(LIBAPL)

	@echo "$@ is OK!"

# implicit
 
.SUFFIXES: .pc .c .o

.pc.o:
	$(PROC) $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
	$(CC)  $(CFLAGS) -w $(APPINCLUDE) -c $*.c
	@rm -f $*.c
	@rm -f $*.lis

.c.o:
	$(CC)  $(CFLAGS) -w $(APPINCLUDE) -c $*.c
 
# default
 
#.DEFAULT:
#	$(CC) -o $@ $(CFLAGS) $(@F).c $(LIBS)
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies
