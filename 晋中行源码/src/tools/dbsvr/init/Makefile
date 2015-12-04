#
# NAME:       makefile.aix
# AUTHOR:
# VERSION:    1.1
# DATE:       98/03/19
#

#ESQL=$(INFORMIXDIR)/bin/esql -q64
ESQL=$(INFORMIXDIR)/bin/esql 
MV=mv

INCLUDE=-I$(HOME)/src/incl_pub -I$(HOME)/src/incl_ec -I$(HOME)/src/incl_c
APPINCLUDE=$(INCLUDE)
PROCINCLUDE=include=$(HOME)/src/incl_pub include=$(HOME)/src/incl_ec include=$(HOME)/src/incl_c
ORALIBDIR= -L$(ORACLE_HOME)/lib32
APPLIBDIR= -L$(HOME)/lib 
PROC	= proc

ORALIB= -lclntsh `cat $(ORACLE_HOME)/lib32/ldflags` -lpthread -lc `cat $(ORACLE_HOME)/lib32/sysliblist`

CFLAGS	= -O -c -g
#USERLDFLAGS= -lc -lm -L$(HOME)/lib -lbase -lpubdb -lpubdb_m
USERLDFLAGS= -lc -lm -L$(HOME)/lib -lacct -lln -lbase -lpubdb -lass -lcom -lpubdb_m -lcif -lde -lc -lm -lagent -lcard -lrpt

#LDFLAGS= -lc -lm -L./lib   -linit 
LDFLAGS= $(USERLDFLAGS)

CCFLAGS= -L/usr/lpp/encina/lib \
		-L$(INFORMIXDIR)/lib \
		-L$(INFORMIXDIR)/lib/esql \
		-I$(INFORMIXDIR)/incl/esql \
		-I$(HOME)/src/incl \
		-DSELECT  

BINDIR=$(DBSDIR)/bin
TARGET=	$(BINDIR)/init1 \
		$(BINDIR)/init2 \
		$(BINDIR)/init3 \
		$(BINDIR)/init5 \
		$(BINDIR)/initfortran 
#大额支付不初始化
$(TARGET): $$(@F).o
	$(CC) -brtl -g $(ORALIBDIR) $(APPLIBDIR) $(CCFLAGS) $(APPINCLUDE) $(@F).o -o $@ $(ORALIB) $(TUSERLIB) $(LDFLAGS)
	@echo "$@ is OK!"

.SUFFIXES: .pc .c .o

.pc.o:
	$(PROC) $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
	$(CC)  $(CCFLAGS)  $(APPINCLUDE) -c $*.c
	@rm -f $*.c
	@rm -f $*.lis
.c.o:
	$(CC)  $(CCFLAGS)  $(APPINCLUDE) -c $*.c

clear:
#	$(RM) -f *.o *.c
