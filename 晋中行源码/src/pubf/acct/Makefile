RM = rm
MV = mv
CC = cc $(Q64)
AR = ar -rv
LIBSRC = $(HOME)/src/pubf/acct
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) -qmaxmem=-1


OBJ =	pubf_acct_trance.o pubf_acct_entry.o pubf_acct_cash.o \
		pubf_acct_opn_mdm.o pubf_acct_opn_ac.o pubf_acct_cls_mdm.o \
		pubf_acct_loss.o  pubf_acct_chg_mdm.o pubf_acct_redo.o \
		pubf_acct_rol_reg.o pubf_acct_rollback.o pubf_acct_chrg.o \
		pubf_acct_rol_special.o pubf_acct_jx.o pubf_acct_trance_check.o pubf_acct_check_tctd.o\
        pubf_acct_zjgl.o

all: libacct.so

libacct.so: $(OBJ)
	$(CC) -G -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
