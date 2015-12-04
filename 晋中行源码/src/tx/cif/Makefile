
CC	=	cc $(Q64) -g
RM	=	rm	-f
MV	=	mv
CP	=	cp

BIN = $(HOME)/bin
LIBDIR = $(HOME)/lib

INC = -I$(HOME)/src/incl_c -I$(HOME)/src/incl_pub
LIB = -L$(HOME)/lib
CFLAGS = $(INC) $(LIB)

LIBAPL = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lc -lm


OBJS	= sp0996.so sp0997.so sp5577.so spC101.so spC102.so spC103.so spC104.so spC105.so spC106.so spC107.so spC108.so spC109.so \
	  spC110.so spC111.so spC112.so spC113.so spC114.so spC115.so spC117.so spC118.so spC119.so spC201.so spC202.so \
	  spC203.so spC204.so spC301.so spC302.so spC303.so spC304.so spC305.so spC306.so spC307.so spC308.so spC309.so spC310.so \
	  spC311.so spC312.so spC313.so spC314.so spC315.so spC316.so spC317.so spC318.so spC320.so spC321.so spC401.so spC402.so \
	  spC403.so spC501.so spC502.so spC601.so spC602.so  
TARGET = $(BIN)/lib.so

all: $(TARGET)

$(TARGET):$(OBJS)

.SUFFIXES: .so .c

.c.so:
	$(CC) $(CFLAGS) -c $*.c
	$(CC) -G  -o $@ $(CFLAGS) $*.o $(LIBAPL)
	mv $(*F).so $(LIBDIR)
	$(RM) $(*F).o

.c:
	$(CC) -G  -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so $(LIBDIR)
	
