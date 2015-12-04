
CC	= cc $(Q64) -g
RM	=	rm	-f
MV	=	mv
CP	=	cp

BIN = $(HOME)/bin
LIBDIR = $(HOME)/lib

INC = -I$(HOME)/src/incl_c -I$(HOME)/src/incl_pub -I$(HOME)/src/pubf/bal/tcp/incl
LIB = -L$(HOME)/lib
CFLAGS = $(INC) $(LIB)

LIBAPL = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lagent -lc -lm

OBJS	=	sp6336.so sp6340.so sp6332.so sp6330.so

TARGET = $(BIN)/lib.so

all: $(TARGET)

$(TARGET):$(OBJS)

.SUFFIXES: .so .c

.c.so:
	$(CC) $(CFLAGS) -c $*.c
	$(CC) -G -o $@ $(CFLAGS) $*.o $(LIBAPL)
	mv $(*F).so $(LIBDIR)
	$(RM) $(*F).o

.c:
	$(CC) -G -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so $(LIBDIR)
	
