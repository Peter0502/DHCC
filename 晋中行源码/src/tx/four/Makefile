
CC	= cc $(Q64) -g
RM	=	rm	-f
MV	=	mv
CP	=	cp

BIN = $(HOME)/bin
LIBDIR = $(HOME)/lib

INC = -I$(HOME)/src/incl_c -I$(HOME)/src/incl_pub
LIB = -L$(HOME)/lib
CFLAGS = $(INC) $(LIB)

LIBAPL = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lagent -lc -lm

OBJS	=	spF701.so spF702.so spF703.so spF704.so \
			spF707.so spF713.so spF777.so spF781.so \
			spF888.so 

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
	
