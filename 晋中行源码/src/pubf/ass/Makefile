RM = rm
MV = mv
CC = cc $(Q64)
AR = ar -rv
LIBSRC = $(HOME)/src/pubf/ass
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) -qmaxmem=-1


OBJ =	pubf_ass_cp_stat_upd.o pubf_ass_tx.o pubf_ass_br_def.o 
#pubf_ass_liqu.o

all:libass.so 

libass.so: $(OBJ)
	$(CC) -G  -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib
libass.a: $(OBJ)
	$(AR) $(X64) $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
