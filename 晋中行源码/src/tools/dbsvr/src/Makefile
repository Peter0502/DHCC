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
PROCINCLUDE=include=$(ORACLE_HOME)/precomp/public include=../incl
APPINCLUDE= -I../incl -I$(ORACLE_HOME)/precomp/public
SYS_INCLUDE='sys_include=(/usr/include)'
ORALIBDIR= -L$(ORACLE_HOME)/lib
APPLIBDIR= -L$(WORKDIR)/lib 
BINDIR= $(DBSDIR)/bin
LIBDIR= $(WORKDIR)/lib
USERID=scott/tiger
PROC=proc
CC=cc $(Q64)
CFLAGS=  $(Q64)  -brtl
ORALIB= -lclntsh -lpthread -lc `cat $(ORACLE_HOME)/lib/sysliblist`

#############################################
##           USER DEFINE PART			   ##
#############################################
TUSERLIB =
TARGET= $(BINDIR)/makech \
		$(BINDIR)/makedb \
		$(BINDIR)/makedb_view \
		$(BINDIR)/makedb_nomy \
		$(BINDIR)/makedb_notab 
TARGET2= $(BINDIR)/mtest 

#############################################
#############################################

#############################################
##           PUBLIC PART				   ##
#############################################
all: $(TARGET)
tool: $(TARGET2)
$(TARGET): $$(@F).o
	$(CC) -g $(ORALIBDIR) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) $(@F).o -o $@ $(ORALIB) $(TUSERLIB)

	@echo "$@ is OK!"
$(TARGET2): $$(@F).o
	$(CC) -g -brtl  $(ORALIBDIR) $(APPLIBDIR) $(CFLAGS) $(APPINCLUDE) $(@F).o -o $@ $(ORALIB) $(TUSERLIB) -lpubdb -lpubdb_m -lbase -lacct

	@echo "$@ is OK!"

.SUFFIXES: .pc .c .o

.pc.o:
	$(PROC) $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
	$(CC)  $(CFLAGS) -w $(APPINCLUDE) -c $*.c
	@rm -f $*.c
	@rm -f $*.lis

.c.o:
	$(CC)  $(CFLAGS) -w $(APPINCLUDE) -c $*.c

fall:
	@touch *c
	@make all
