CC			= cc  $(Q64)
LD			= cc $(Q64)
AR			= ar -rv
RM			= rm
MV			= mv
DBCOMPILE	= $(INFORMIXDIR)/bin/esql $(Q64) 
CCFLAGS		= -L/usr/lpp/encina/lib \
			-I$(HOME)/src/incl_c \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/pubf/base \
			-I$(HOME)/src/pubf/bal/tcp/incl \
			-I$(HOME)/src/pubf/bal/tuxe/incl \
			-I$(HOME)/src/pubf/bal/tuxe/a_incl \
			-DSELECT \
			-lm -qmaxmem=-1

LIBSRC = $(HOME)/src/pubf/comm
LIBDIR = $(HOME)/lib

OBJ =	pubf_comm_SvrPub.o \
        pubf_comm_GetTrad.o pubf_comm_app.o \
        pubf_comm_SysInit.o pubf_comm_TbLoad.o pubf_comm_tcpSvr.o \
        pubf_comm_net.o pubf_comm_pack.o pubf_comm_auth.o\
	pubf_comm_sock.o pubf_comm_chrg.o\
	pub_com_test.o

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_ec \
			-I$(HOME)/src/incl_c \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/comm/tuxe/incl \
			-I$(HOME)/src/comm/tuxe/a_incl \

all:libcomm.so

libcomm.so: $(OBJ)
	cc $(Q64) -G -o libcomm.so  $(OBJ)
	mv libcomm.* $(LIBDIR)

libcomm.a: $(OBJ)
	$(AR) $(X64) $@ $(OBJ)
	mv $@ $(HOME)/lib

#*******************************************/
.ec.o :
	$(RM) -f $*.c
	$(DBCOMPILE) $(CCFLAGS) -O -c -s $<
	$(RM) -f $*.c

.c.o:
	$(CC) $(CCFLAGS) -O -g -s -c $<

clear:
	$(RM) -f $*.o


clear:
	rm -f *.o
