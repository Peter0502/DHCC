RM = rm
MV = mv
CC = cc $(Q64)
AR = ar -rv 
LIBSRC = $(HOME)/src/pubf/acct
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE)			


OBJ =	pubf_card_num.o pubf_card_chk.o dhcdes.o pubf_card_cvn.o pubf_write_card_file.o pubf_card_chk_msr.o pub_card_reg.o

all:libcard.so

libcard.so: $(OBJ)
	$(CC) -G  -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

libcard.a: $(OBJ)
	$(AR) $(X64) $@  $(OBJ)
	$(MV) $@ $(HOME)/lib
#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
