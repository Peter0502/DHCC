
CC	=	cc $(Q64) -g
RM	=	rm	-f
MV	=	mv
CP	=	cp

BIN = $(HOME)/bin
LIBDIR = $(HOME)/lib

INC = -I$(HOME)/src/incl_c -I$(HOME)/src/incl_pub
LIB = -L$(HOME)/lib
CFLAGS = $(INC) $(LIB)

LIBAPL = $(LIB) -lacct -lcard -leod -lpubdb -lsttl -lagent -lcif -lln \
	-lpubdb_m -lass  -lcom -lrept -lbase -lde  -lod  -lrpt -lenvinfo

OBJS	=	spEB01.so \
		spEB06.so \
		spEB07.so \
		spEB08.so \
		spEB09.so \
		spW001.so \
		spW002.so \
		spW003.so \
		spW004.so \
		spW005.so \
		spW006.so \
		spW007.so \
		spW008.so \
		spW009.so \
		spW011.so \
		spW010.so \
		spW200.so \
		spW201.so \
		spW210.so \
		spW211.so \
		spW220.so \
		spW225.so \
		spMK01.so \
		spMK02.so \
		spMK03.so \
		spMK04.so \
		spMK05.so \
		spW223.so \
		spMK06.so \
		spMK07.so \
		spMK08.so \
                spW226.so
 
TARGET = $(BIN)/lib.so

all: $(TARGET)

$(TARGET):$(OBJS)

.SUFFIXES: .so .c

.c.so:
	$(CC) $(CFLAGS) -c $*.c
	$(CC) -G -o $@ $(CFLAGS) $*.o $(LIBAPL)
	mv $(*F).so $(LIBDIR)
#	$(RM) $(*F).o

.c:
	$(CC) -G -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so $(LIBDIR)
