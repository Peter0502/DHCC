
SHELL	=	sh

#Q64     =   -q64
CC		=	cc $(Q64) -g
LD		=	ld

RM		=	@rm -f
MV		=	@mv
CP		=	@cp
ECHO	=	@echo


# paths and flags
 
BIN	= $(HOME)/bin
LIBDIR  = $(HOME)/lib

INC	= -I$(HOME)/src/incl_c -I$(HOME)/src/incl_ec -I$(HOME)/src/incl_pub
#CLIB	= -L$(HOME)/lib -L.
CLIB	= -L$(HOME)/lib 
CFLAGS	=  $(INC) $(CLIB)
#LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lc -lm
LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lc -lm

OBJS	=	sp1701.so \
sp1503.so \
sp1603.so \
sp1702.so \
sp1703.so \
sp1704.so \
sp1705.so \
sp1706.so \
sp1707.so \
sp1708.so \
sp1709.so \
sp1710.so \
sp1711.so \
sp1712.so \
sp1713.so sp1714.so\
sp1801.so \
sp1802.so \
sp1803.so \
sp1804.so \
sp1901.so \
sp1902.so \
sp1903.so \
sp1904.so \
sp1905.so \
sp8101.so \
sp8103.so \
sp8104.so \
sp8105.so \
sp8106.so \
sp8107.so \
sp8108.so \
sp8109.so \
sp8110.so \
sp8111.so \
sp8112.so \
sp8113.so \
sp8133.so \
sp8114.so \
sp8115.so \
sp8116.so \
sp8121.so \
sp8122.so \
sp8123.so \
sp8201.so \
sp8202.so \
sp8203.so \
sp8204.so \
sp8205.so \
sp8206.so \
sp8207.so \
sp8208.so \
sp8213.so \
sp8301.so \
sp8302.so \
sp8303.so \
sp8304.so \
sp8401.so \
sp8402.so \
sp8403.so \
sp8408.so \
sp8501.so \
sp8502.so \
sp8503.so \
sp8504.so \
sp8505.so \
sp8506.so \
sp8507.so \
sp8601.so \
sp8701.so \
sp8702.so \
sp8703.so \
sp8704.so \
sp8802.so \
sp8803.so \
sp8901.so \
sp8902.so \
spT003.so \
sp4338.so \
sp6324.so \
sp8915.so \
sp8916.so \
sp8917.so \
sp4339.so \
sp8117.so \
sp8118.so \
sp8119.so \
sp4915.so \
sp8922.so \
sp4341.so \
sp4342.so \
sp8210.so \
sp4926.so \
sp8305.so \
sp8211.so \
sp4348.so \
sp8610.so \
sp8611.so \
sp8406.so \
sp8602.so \
sp8102.so \
sp8404.so\
sp6654.so\
sp8994.so\
sp5923.so\
sp5924.so\
sp8923.so\
sp8924.so\
sp5893.so\
sp5894.so\
sp5623.so\
sp8925.so\
sp8926.so\
sp8927.so\
sp8928.so\
sp8929.so\
sp8921.so\
sp8805.so\
sp8804.so\
sp8209.so\
sp6655.so\
sp6111.so\
sp5895.so\
sp5888.so\
sp5887.so\
sp5708.so\
sp4946.so\
sp4936.so\
sp4805.so\
sp4806.so\
sp8120.so\
sp5925.so\
sp1712.so\
sp9102.so\
sp1717.so\
sp8100.so\
sp8130.so\
sp8131.so\
sp1799.so\
sp8129.so\
sp8903.so\
sp8136.so\
sp1907.so\
sp6383.so\
sp8215.so\
sp8990.so\
sp8991.so\
sp2717.so\
sp1718.so\
sp8423.so\
sp8216.so\
sp1906.so\
sp9338.so\
sp8134.so\
sp9207.so
TARGET	= \
		$(BIN)/lib.so

all: Mstart $(TARGET) Mcomplete
 
Mstart:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) ..."
	$(ECHO)

Mcomplete:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) completed."
	$(ECHO)
 
$(TARGET):$(OBJS)


# implicit
 
.SUFFIXES: .so .c
 
.c.so:
	$(CC) $(CFLAGS) -c $*.c
	$(CC) -G -o $@ $(CFLAGS) $*.o $(LIBAPL)
	mv $(*F).so $(LIBDIR)
#	$(RM) $(*F).o

.c:
	$(CC) -g -s -G -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so $(LIBDIR)

# default
 
#.DEFAULT:
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies

