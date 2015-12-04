
SHELL	=	sh

#Q64     =   -q64
CC		=	cc $(Q64) -g
EC		=	esql $(Q64) 
LD		=	ld

RM		=	@rm -f
MV		=	@mv
CP		=	@cp
ECHO	=	@echo


# paths and flags
 
BIN	= $(HOME)/bin
LIBDIR  = $(HOME)/lib

INC	= -I$(HOME)/src/incl_c -I$(HOME)/src/incl_ec -I$(HOME)/src/incl_pub
CLIB	= -L$(HOME)/lib -L.
CFLAGS	= $(INC) $(CLIB)
#LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lc -lm
LIBAPL  = -lbaltcp -leod -lcomm -lbase -lcom -lpubdb -lpubdb_m -lass -lc -lm -lrept
APPINCLUDE= -I$(HOME)/src/incl_c -I$(HOME)/src/incl_ec -I$(HOME)/src/incl_pub
PROCINCLUDE= include=$(HOME)/src/incl_c include=$(HOME)/src/incl_ec include=$(HOME)/src/incl_pub
PROC	= proc
EFLAGS = $(CFLAGS)



OBJS =  gD0252.so gD0289.so gD129.so gD405.so gDX09.so lhf.so rpt071.so rpt922.so rpt971.so rpt980.so testy.so \
	XD.so gD027.so gD0290.so gD130.so gD406.so gDYEDQ.so ln001.so rpt076.so rpt923.so rpt972.so rpt980z.so \
	bbxxxx.so gD0280.so gD0292.so gD131.so gD502.so gDbfz15.so ln_pln.so rpt077.so rpt924.so rpt974.so rpt99D.so \
	bhdforcups.so gD0281.so gD0293.so gD132.so gD855.so gDbfz20.so loaddzy.so rpt078.so rpt925.so rpt974.so rptcupsfee.so upd406.so \
	bhdupd712.so gD0282.so gD0294.so gD204.so gD888.so gDbfz5.so looppkg.so rpt888.so rpt926.so rpt976.so rptfxq.so upd420.so \
	bhdupd713.so gD0283.so gD0296.so gD206.so gD902.so gT001.so note_cheq.so rpt917.so rpt927.so rpt977.so sp4561.so \
	check_dc.so gD0284.so gD043.so gD903.so gT002.so rpt002.so rpt918.so rpt942.so rpt978.so sp8125.so \
	dclogck.so gD0285.so gD076.so gD401.so gD998.so gT003.so rpt033.so rpt919.so rpt961.so rpt979.so spE605.so \
	dclogck2.so gD0286.so gD123.so gD402.so gDKHWH.so gT004.so rpt052.so rpt962.so rpt97a.so spJ406.so \
	ftpgetcups.so gD0287.so gD127.so gD403.so gDSQSX.so gT005.so rpt054.so rpt920.so rpt963.so rpt97b.so spJ416.so \
	gD0251.so gD0288.so gD128.so gD404.so gDTJXF.so jywpc.so rpt056.so rpt921.so rpt964.so rpt97c.so test.so

#gD295.so



TARGET	= \
		$(BIN)/lib.so

all: Mstart $(TARGET) Mcomplete
 
Mstart:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) ..."
	$(ECHO)

Mcomplete:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) completed."
	$(ECHO)
 
$(TARGET):$(OBJS)

# implicit
 
.SUFFIXES: .pc .c .so
.pc.so:
	$(PROC) $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
	$(CC) -G -o $@ $(CFLAGS) $*.c $(LIBS) 
	@rm -f $*.c
	@rm -f $*.lis
	mv *.so $(LIBDIR)
.c.so:
	$(CC) -w $(CFLAGS)  $(APPINCLUDE) -c $*.c
	$(CC) -G -o $@ $(CFLAGS) $*.c $(LIBS)
	mv *.so $(LIBDIR)

# default
 
#.DEFAULT:
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies

