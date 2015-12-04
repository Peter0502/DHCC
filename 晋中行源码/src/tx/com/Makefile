
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

OBJS	= 	spN101.so spN102.so spN103.so spN104.so spN105.so \
		spN106.so spN107.so spN108.so spN109.so spN110.so \
		spN111.so spM001.so spM002.so spM003.so spM004.so spM005.so \
        	spN112.so spN301.so spE701.so spN113.so \
		spN302.so spN303.so spE503.so spE501.so spE502.so spE504.so \
		spE505.so spE603.so spE605.so spE702.so spE703.so spM202.so \
		spE705.so spE706.so spW701.so spW702.so spM204.so spE707.so spE708.so spE774.so \
		spE606.so spE607.so spE608.so spE609.so spE610.so spE709.so \
        	spE611.so spN114.so spM205.so spE704.so spN160.so spN117.so spN119.so 




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

