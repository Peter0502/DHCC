
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
	-lpubdb_m -lass  -lcom -lrept -lbase -lde  -lod  -lrpt 

OBJS	=	spA011.so spA013.so spA015.so spA017.so spA019.so spA021.so \
			spA012.so spA014.so spA016.so spA018.so spA020.so spA999.so \
			spWNJZ.so spSPEC.so spDEWZ.so spDELZ.so spHPQF.so spHPDF.so \
			spXEJZ.so spXEWZ.so spa016.so spa017.so spDEDZ.so spA507.so \
			spA022.so spA688.so spA689.so spPLKD.so spPLKH.so spZHJX.so

		
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
