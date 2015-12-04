RM = rm
MV = mv
CC = cc $(Q64)
AR = ar -rv
LIBSRC = $(HOME)/src/pubf/sttl
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_ec \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) -qmaxmem=-1


OBJ =	pubf_sttl_brno.o pubf_sttl_bankAcc.o pubf_sttl_sgCash.o\
	pubf_sttl_draft.o pubf_sttl_cashier.o pubf_sttl_fax.o\
	pubf_sttl_trustPay.o

all:libsttl.so

libsttl.so: $(OBJ)
	$(CC) -G  -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib
libsttl.a: $(OBJ)
	$(AR) $(X64) $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -c $<

clear:
	rm -f *.o
