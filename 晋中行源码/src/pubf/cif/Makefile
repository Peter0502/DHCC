RM = rm
MV = mv
CC = cc $(Q64)
#CC = cc 
AR = ar -rv
LIBSRC = $(HOME)/src/pubf/cif
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_ec \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE)			


OBJ =	pubf_cif_CreCifNo.o	pubf_cif_CifCheck.o

all:libcif.so libcif.a

libcif.so: $(OBJ)
	$(CC) -G -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib
libcif.a: $(OBJ)
	$(AR) $(X64) $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
