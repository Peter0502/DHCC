#******************************************************************************#
#* 说明::本工程文件适应于如下配置:                                            *#
#*       操作系统  : AIX5                                                     *#
#*       数据库系统: ORACLE9i                                                 *#
#*       编译器    : gcc                                                      *#
#*----------------------------------------------------------------------------*#
#* 编者：Li JunXu    编写日期:20030112   修改者:           修改日期:          *#
#******************************************************************************#

#############################################
##             PUBLIC PART                 ##
#############################################
####################################
PROCINCLUDE=include=$(ORACLE_HOME)/precomp/public include=$(TUXDIR)/include include=$(HOME)/src/incl_pub include=$(HOME)/src/incl_c
APPINCLUDE=-I$(WORKDIR)/a_incl -I/usr/include -I/usr/local/include -I$(ORACLE_HOME)/precomp/public -I$(TUXDIR)/include -I$(HOME)/src/incl_pub -I$(HOME)/src/incl_c
SYS_INCLUDE='sys_include=(/usr/include)'
ORALIBDIR= -L$(ORACLE_HOME)/lib
APPLIBDIR= -L$(WORKDIR)/lib
# -L/usr/lib -L/usr/local/lib/gcc-lib/powerpc-ibm-aix5.1.0.0/3.2.1
BINDIR= $(WORKDIR)/bin
LIBDIR= $(WORKDIR)/lib
USERID=scott/tiger
PROC=proc
#ORALIB= -lclntsh -lpthread -lc -lm `cat $(ORACLE_HOME)/$(ORACLE_LIBDIR)/sysliblist`
ORALIB= -lclntsh -lpthread -lc -lm `cat $(ORACLE_HOME)/lib/sysliblist`

#############################################
##             USER DEFINE PART            ##
#############################################
TUSERLIB = -lcis -lhv -llv \
	-lbaltcp -lacct -lcomm -lln -lbase -lpubdb -lcom -lde \
	-lpubdb_m -lcif -lc -lm -lagent -lcard -lod  -lass -lrpt -leod -lrept -lbase
TUSERLIB3 = -lhv -llv \
	-lbaltcp -lacct -lcomm -lln -lbase -lpubdb -lcom -lde \
	-lpubdb_m -lcif -lc -lm -lagent -lcard -lod  -lass -lrpt -leod -lrept -lbase

CC=cc $(Q64)
CFLAGS = $(Q64) -DAIX -D__DATA_FROM_CIF__ -brtl
LIB_OBJS = sndrcv.o cif_comm.o cifpack.o errlog.o DesNew.o


EXE_OBJS=cif_comm.o cifserver.o

LIB_TARGET=$(LIBDIR)/libcis.so
EXE_TARGET=$(BINDIR)/cifserver

#############################################
##             PUBLIC PART                 ##
#############################################
all:
	make $(LIB_TARGET)
	make $(EXE_TARGET)

$(EXE_TARGET): $(EXE_OBJS)
	$(CC)  $(CFLAGS) $(ORALIBDIR) $(APPLIBDIR) $(APPINCLUDE) \
		$(EXE_OBJS) -o $@ $(ORALIB) $(TUSERLIB) 
	@echo "$@ is OK!"

$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(Q64) -G -o $@  $(LIB_OBJS) 

	@echo "$@ is OK !"

.SUFFIXES: .pc .c .o
#.pc.o:
#	$(PROC) $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
#	$(CC) $(CFLAGS)  $(APPINCLUDE) -c $*.c
#	@rm -f $*.c
#	@rm -f $*.lis
.c.o:
	$(CC) $(CFLAGS)  $(APPINCLUDE) -c $*.c
fall:
	@touch *c
	@make
clean:
