PROC		= proc
CC			= cc $(Q64)
LD			= cc $(Q64)
AR			= ar -rv
RM			= rm
MV			= mv 
DBCOMPILE	= $(INFORMIXDIR)/bin/esql $(Q64)
CCFLAGS		= -L/usr/lpp/encina/lib \
			-I$(HOME)/src/incl_c -I$(DBSDIR)/src -I$(HOME)/src/incl_pub -I $(ORACLE_HOME)/precomp/public \
			-DSELECT 
PROCINCLUDE= include=$(HOME)/src/incl_c include=$(DBSDIR)/src include=$(HOME)/src/incl_pub 

OBJ	=	tblproc.o pubf_td_mst_hst.o trace_log_rol.o pubf_pubdb_db.o  \
		pub_eod_ln_mst.o pub_eod_dd_mst.o pubf_pubdb_deltable.o \
		pub_eod_in_mst.o pubf_td_mst_hst.o  \
	    pub_eod_dzzbj.o pubf_eod_check_dc.o pubf_pubdb_gl_prdt_dyn.o  pubf_eod_ass.o \
	    pubf_pubdb_gyywl.o pubf_pubdb_sumdclog.o  sequence.o pubf_pubdb_comsel.o \
        mdm_ac_rel_ac_no.o
		

all : libpubdb_m.so libpubdb_m.a
###注:为了编译去掉的文件:
#####  pubf_eod_cre_boco_27.o pubf_eod_cre_boco_27_1.o
#OBJ	=mdm_ac_rel_nomy.o \
#		od_mst_nomy.o td_mst_nomy.o ln_lo_nomy.o \
#		pubf_eod_check_dc.o \
#		impa_dep_reg.o \
#		ln_lo_hold.o ln_pay_pln_hold.o trace_log_rol.o \
#		target_statistic.o correl_brf.o pubf_cbs.o dd_mst_hst.o \
#		in_mst_hst.o com_item.o mq_tax_payment_hold.o 

libpubdb_m.so :	$(OBJ)
	$(CC) -G  -o libpubdb_m.so  $(OBJ) -L$(ORACLE_HOME)/lib -L$(HOME)/lib -lclntsh -lenvinfo
#-lenvinfo
	$(MV) $@ $(HOME)/lib
libpubdb_m.a :	$(OBJ)
	$(AR) $(X64) libpubdb_m.a  $(OBJ)
	$(MV) $@ $(HOME)/lib

.SUFFIXES:.pc 

.pc.o: 
	$(PROC) DYNAMIC=ANSI $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
	$(CC) -O -g -s $(CCFLAGS) -c $*.c
	@rm -f $*.c
	@rm -f $*.lis
	
.c.o:
	$(CC) $(CCFLAGS) -O -c -s $< 

clear:
	rm *.c
