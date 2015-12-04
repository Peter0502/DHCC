
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
LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lass -lc -lm -lrept 
APPINCLUDE= -I$(HOME)/src/incl_c -I$(HOME)/src/incl_ec -I$(HOME)/src/incl_pub
PROCINCLUDE= include=$(HOME)/src/incl_c include=$(HOME)/src/incl_ec include=$(HOME)/src/incl_pub
PROC	= proc
EFLAGS = $(CFLAGS)
COMM=pubf_shjd_com.o
OBJS=   sp3642.so  sp3623.so  sp3635.so  sp3644.so sp3661.so  sp3667.so  sp3672.so  sp3614.so \
    sp3624.so  sp3668.so  sp3673.so  sp3631.so \
    sp3646.so  sp3663.so  sp3671.so  sp3632.so \
    sp3664.so  sp3621.so  sp3651.so  sp3665.so \
    sp3612.so  sp3643.so  sp3652.so  sp3666.so 

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
	$(CC) -G -o $@ $(CFLAGS) $*.c $(LIBS) $(COMM) 
	@rm -f $*.c
	@rm -f $*.lis
	mv *.so $(LIBDIR)
.c.so:
	$(CC) -w $(CFLAGS)  $(APPINCLUDE) -c $*.c
	$(CC) -G -o $@ $(CFLAGS) $*.c $(LIBS) $(COMM)
	mv *.so $(LIBDIR)

# default
 
#.DEFAULT:
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies

