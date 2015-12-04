
# ident "@(#)apsvr:sa/si.mak"

# Thunder Bird

# inputs

APP		=	$(HOME)
FLG		=	-DDEBUG -DBAL_DEBUG
 
# commands
 
SHELL	=	sh

#CC		=	xlc $(Q64) 
CC		=	cc $(Q64) 
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

RSM		=	Oracle_XA
BSTUB	=	:_balmain

# paths and flags
 
BIN		= 	$(APP)/bin
LIB		=	$(APP)/lib
INC		=	-I$(APP)/src/incl_c -I$(APP)/src/incl_pub

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
 
C_INCP	=	-I$(EC_INC) -I$(TX_INC) -I$(AP_INC) $(INC) -I.
C_LIBP	=	-L$(EC_LIB) -L$(TX_LIB) -L$(CM_LIB) -L$(LIB) -L.
 
CFLAGS	=	$(CFLAG) $(C_INCP) $(C_LIBP)
EFLAGS	=	$(CFLAG) $(C_INCP) $(C_LIBP)
LFLAGS	=	$(LFLAG)
 
# libraries and targets

#LIBUCB	=	/usr/ucblib/libucb.a
LIBUCB	=
LIBNET	=	$(DB_LIB)/netlib.a

LIBAPL  = -lbaltcp -lacct -lcomm -lln -lbase -lpubdb -lcom -lde \
	-lpubdb_m -lcif -lc -lm -lagent -lcard -lod  -lass -lrpt -leod -lrept\
    -lbase -lcis -llv -lhv -lenvinfo -liconv
SQLLIBS	=	-lsql $(LIBNET) -los -lgen -lsocket -lnsl
#RBSLIBS	=	-ldes -lbhcomm
#DEPS    =   $(AP_LIB)/libbaltcp.a 

#BALLIBS	=	$(RBSLIBS) -ldbsvr
BALLIBS	=	$(LIBBAL) $(LIBAPL)

LIBS	=	$(LIBBAL) $(LIBUCB) $(BALLIBS)
#######################      ORACLE         #################
ORALIBDIR= -L$(ORACLE_HOME)/lib
APPLIBDIR= -L$(HOME)/lib 
APPINCLUDE	=	$(INC) $(C_INCP)
ORALIB= -lclntsh `cat $(ORACLE_HOME)/lib/ldflags` -lpthread -lc `cat $(ORACLE_HOME)/lib/sysliblist`
#############################################################

OBJS	=	tcpdmn.o u_servmain.o
OBJS2	=	sndrcv.o bal_getfile.o	
OBJS3	=	eodtst.o
OBJS4	=	hv_main.o
OBJS5	=	lv_main.o
OBJS6   =       mobdata.o mobmqlib.o mobsend_real.o sndrcv.o bal_getfile.o md5c.o
OBJS7   =       mobdata.o mobmqlib.o mobsend_bat.o sndrcv.o bal_getfile.o  md5c.o

TARGET	= $(BIN)/tcpappsvr \
	  $(BIN)/bal_getfile \
	  $(BIN)/eodtst \
	  $(BIN)/hv_main \
	  $(BIN)/lv_main \
          $(BIN)/mobsend_real\
          $(BIN)/mobsend_bat
all: Mstart $(TARGET) Mcomplete 
Mstart: 
	$(ECHO) 
	$(ECHO) "MAKE apsvr:$(TARGET) ..." 
	$(ECHO) 
Mcomplete:
	$(ECHO)
	$(ECHO) "MAKE apsvr:$(TARGET) completed."
	$(ECHO)
 
$(BIN)/tcpappsvr: $(OBJS)
	$(CC) -brtl $(Q64) $(ORALIBDIR) $(ORALIB) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) -o  $@   $(OBJS)  $(LIBS) $(ORALIB)

	@echo "$@ is OK!"

$(BIN)/bal_getfile: $(OBJS2)
#	$(CC) -brtl  $(Q64) -o  $@   $(OBJS2) 
	$(CC) -brtl  $(Q64)  $(ORALIBDIR) $(ORALIB) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) -o  $@   $(OBJS2)  $(LIBS) $(ORALIB)
	@echo "$@ is OK!"
# implicit

$(BIN)/eodtst: $(OBJS3)
#	$(CC) -brtl  $(Q64) -o  $@   $(OBJS3) 
	$(CC) -brtl  $(Q64)  $(ORALIBDIR) $(ORALIB) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) -o  $@   $(OBJS3)  $(LIBS) $(ORALIB)
	@echo "$@ is OK!"
	
$(BIN)/hv_main: $(OBJS4)
#	$(CC) -brtl  $(Q64) -o  $@   $(OBJS4) 
	$(CC) -brtl  $(Q64)  $(ORALIBDIR) $(ORALIB) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) -o  $@   $(OBJS4)  $(LIBS) $(ORALIB)
	@echo "$@ is OK!"
	
$(BIN)/lv_main: $(OBJS5)
#	$(CC) -brtl  $(Q64) -o  $@   $(OBJS5) 
	$(CC) -brtl  $(Q64)  $(ORALIBDIR) $(ORALIB) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) -o  $@   $(OBJS5)  $(LIBS) $(ORALIB)
	@echo "$@ is OK!"
#¶ÌÐÅ³ÌÐò

$(BIN)/mobsend_real: $(OBJS6) 
	$(CC) -brtl  $(Q64)  $(ORALIBDIR) $(ORALIB) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) -o  $@   $(OBJS6)  $(LIBS) $(ORALIB)
	@echo "$@ is OK!"

$(BIN)/mobsend_bat: $(OBJS7)
	$(CC) -brtl  $(Q64)  $(ORALIBDIR) $(ORALIB) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) -o  $@   $(OBJS7)  $(LIBS) $(ORALIB)
	@echo "$@ is OK!"

.SUFFIXES: .o .c .ec
 
.ec.o:
	$(EC) $(EFLAGS) -c $*.ec
#	$(CC) -o $@ $(CFLAGS) -c $(*F).c
	$(RM) $(*F).c
 
.c.o:
	$(CC) -o $@ $(CFLAGS) -O -g -s -c $*.c
 
.c:
	$(CC) -o $@ $(CFLAGS) $*.c $(LIBS)
 
# default
 
#.DEFAULT:
#	$(CC) -o $@ $(CFLAGS) $(@F).c $(LIBS)
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies


