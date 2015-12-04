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


OBJ = dmem.o lv_pub.o lvnet.o lvpack.o lv_tabcomm.o pubf_com_lv_lhf.o pubf_com_lv_xyy.o \
	lv_packcomm.o pubf_com_lv_chm.o pubf_com_lv_dxy.o pubf_notpay_in_dxy.o pubf_notpay_comm_dxy.o \
	pubf_com_lv_send.o lv_lzcmt.o lv_lzcomm.o pubf_notpay_comm_ly.o pubf_com_lv_czyh_dz.o \
	pubf_lv_cmt_r.o pubf_com_lv_send2.o lv_dsf_deal_xyy.o pubf_com_lv_wyw.o \
  des3.o HsmApi.o HsmLog.o HsmRal.o nxHsmApi.o UnionHsmCmd.o UnionSck.o pubf_lv_852.o
all:liblv.so liblv.a

liblv.so: $(OBJ)
	$(CC) -G -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

liblv.a: $(OBJ)
	$(AR) $(X64)  $@ $(OBJ)
	mv $@ $(HOME)/lib


#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
