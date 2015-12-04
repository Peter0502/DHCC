RM = rm
MV = mv
CC = cc $(Q64)
AR = ar -rv
LIBSRC = $(HOME)/src/pubf/base
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_ec \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) -qmaxmem=-1

OBJ =	\
	pubf_ln_modify.o  pubf_ln_check.o  pubf_ln_intst.o  pubf_ln_PayPlan.o \
	pubf_ln_AcCheck.o pubf_ln_OrderPay.o pubf_ln_trust.o pubf_ln_eodtrust.o \
	pubf_ln_rol.o


all:libln.so

libln.so: $(OBJ)
	$(CC) -G -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib
libln.a: $(OBJ)
	$(AR) $(X64) $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
