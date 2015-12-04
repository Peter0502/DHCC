CC	=	cc $(Q64) -g
RM	=	rm	-f
MV	=	mv
CP	=	cp

BIN = $(HOME)/bin
LIBDIR = $(HOME)/lib

INC = -I$(HOME)/src/incl_c -I$(HOME)/src/incl_pub
LIB = -L$(HOME)/lib
CFLAGS = $(INC) $(LIB)

#LIBAPL = $(LIB) -lacct -lcard -leod -lpubdb -lsttl -lagent -lcif -lln \
LIBAPL = $(LIB) -lacct -lcard -leod -lpubdb -lsttl -lagent -lcif -lln \
	-lpubdb_m -lass  -lcom -lrept -lbase -lde  -lod  -lrpt

OBJS  = sp6341.so sp6342.so sp6343.so sp6344.so sp6345.so sp6346.so sp6347.so \
	sp6348.so sp6349.so sp6350.so sp6351.so sp6352.so sp6353.so sp6354.so sp6356.so

TARGET= $(BIN)/lib.so

all: $(TARGET)

$(TARGET):$(OBJS)

.SUFFIXES: .so .c

.c.so:
	$(CC) $(CFLAGS) -c $*.c
	$(CC) -G -o $@ $(CFLAGS) $*.o $(LIBAPL)
	cp $(*F).so $(LIBDIR)
	$(RM) $(*F).o
	$(RM) $(*F).so

.c:
	$(CC) -G -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so  $(LIBDIR)
	#cp *.so $(LIBDIR)
