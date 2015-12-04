RM = rm
MV = mv
CC = cc $(Q64)
#CC = cc 

LIBSRC = $(HOME)/src/pubf/base
LIBDIR = $(HOME)/lib 

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) -lC -qmaxmem=-1


OBJ =	pubf_base_cur_no.o  pubf_base_date.o pubf_base_br_no.o \
        pubf_base_des.o  pubf_base_number.o \
		pubf_base_prot.o pubf_base_TlsrNo.o \
	pubf_base_str.o pubf_base_time.o pubf_base_file.o \
	pubf_base_trace.o pubf_base_charge.o\
	pubf_base_sysinit.o pubf_base_sect_acm.o pubf_base_mdm_draw.o \
	pubf_base_ac.o pubf_base_tel.o pubf_base_intst.o \
	pubf_base_intstDHLB.o pubf_base_intstHQ.o pubf_base_intst_com.o \
	pubf_base_intstLCZQ.o pubf_base_intstCBQX.o pubf_base_intstZCLQ.o \
	pubf_base_intstTZCK.o pubf_base_intstJYCX.o pubf_base_product.o \
	pubf_base_InsTab.o iGetAreaTax.o  \
	pubf_base_prt.o pubf_base_medium.o pubf_base_sysparm.o \
	pubf_base_GetParm.o pubf_base_acm.o pubf_base_log.o pubf_base_self.o \
    pubf_base_intstin.o pubf_base_creattable.o pubf_base_mystery.o \
	pubf_base_despack.o pubf_base_data.o  pubf_base_getacname.o \
	pubf_base_my_func.o pubf_base_output.o pubf_base_dbg.o pubf_base_new.o \
	pubf_base_debug.o pubf_base_card.o \
	pubf_base_free_buf.o pubf_3des.o 

#pubf_base_log.o

all:libbase.so libbase.a

libbase.so: $(OBJ)
	$(CC) -G  -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

libbase.a: $(OBJ)
	ar $(X64) -rv $@ $(OBJ)
	mv $@ $(HOME)/lib


#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
