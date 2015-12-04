RM = rm
MV = mv
CC = cc $(Q64)
AR = ar -rv
LIBSRC = $(HOME)/src/pubf/hv
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c

CCFLAGS = $(INCLUDE) -qmaxmem=-1


OBJ = pubf_com_hv_xyy.o tag.o ejfno.o errlog.o pubf_com_hv.o hv_pub.o hv_tag.o hvini.o \
	hvnet.o hvpack.o  pubf_com_hv_chm.o  hv_hpmy.o pubf_com_hv_czyh_dz.o \
	pubf_com_hv_ly.o hv_status.o pubf_com_hv_lhf.o pubf_hv_cmt_r.o

all:libhv.so 

libhv.so: $(OBJ)
	$(CC) -G -o $@  $(OBJ) -L/usr/lib   -lpthread
	$(MV) $@ $(HOME)/lib

libhv.a: $(OBJ)
	$(AR) $(X64)  $@ $(OBJ)
	mv $@ $(HOME)/lib


#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
