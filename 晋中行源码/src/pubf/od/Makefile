RM = rm
MV = mv
CC = cc $(Q64)
AR = ar -rv
LIBSRC = $(HOME)/src/pubf/od
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_ec \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) -qmaxmem=-1

OBJ =	\
	pubf_od_modify.o pubf_od_check.o pubf_od_proc.o


all:libod.so

libod.so: $(OBJ)
	$(CC) -G  -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib
libod.a: $(OBJ)
	$(AR) $(X64) $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -c $<

clear:
	rm -f *.o
