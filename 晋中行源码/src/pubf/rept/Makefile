RM = rm
MV = mv
CC = cc $(Q64)
AR = ar -rv
LIBSRC = $(HOME)/src/pubf/rept
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) -qmaxmem=-1


OBJ =	pubf_rept_dealfunc.o pubf_rept_com.o pubf_rept_cal.o pubf_rept_tab.o

all:librept.so clear

librept.so: $(OBJ)
	$(CC) -G  -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

librept.a: $(OBJ)
	$(AR) $(X64) $@ $(OBJ)
	mv $@ $(HOME)/lib


#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
