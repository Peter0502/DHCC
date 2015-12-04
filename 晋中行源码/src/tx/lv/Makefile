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
	-lpubdb_m -lass  -lcom -lrept -lbase -lde  -lod  -lrpt  -lhv -llv
OBJS =  spJ042.so spJ049.so spJ056.so spJ063.so spJ071.so spJ078.so spJ088.so spJ097.so spJ398.so spJ405.so spJ416.so spJ500.so \
	spJ043.so spJ050.so spJ057.so spJ064.so spJ072.so spJ079.so spJ089.so spJ098.so spJ399.so spJ407.so spJ419.so spJ501.so \
	spJ044.so spJ051.so spJ058.so spJ065.so spJ073.so spJ080.so spJ092.so spJ099.so spJ400.so spJ411.so spJ421.so spJJJ1.so \
	sp0120.so spJ045.so spJ052.so spJ059.so spJ066.so spJ074.so spJ083.so spJ093.so spJ100.so spJ401.so spJ412.so spJ422.so spJLZ0.so \
	sp5380.so spJ046.so spJ053.so spJ060.so spJ067.so spJ084.so spJ094.so spJ101.so spJ402.so spJ413.so spJ423.so spJtst.so \
	sp5891.so spJ047.so spJ054.so spJ061.so spJ068.so spJ076.so spJ095.so spJ105.so spJ403.so spJ414.so spJ425.so \
	spJ041.so spJ048.so spJ055.so spJ062.so spJ070.so spJ077.so spJ087.so spJ096.so spJ112.so spJ404.so spJ415.so spJ428.so
#spJ085 
#spJ075.so
		
TARGET = $(BIN)/lib.so

all: $(TARGET)

$(TARGET):$(OBJS)

.SUFFIXES: .so .c

.c.so:
	$(CC) $(CFLAGS) -c $*.c
	$(CC) -G -o $@ $(CFLAGS) $*.o $(LIBAPL)
	cp $(*F).so $(LIBDIR)
	$(RM) $(*F).o

.c:
	$(CC) -G -o $@ $(CFLAGS) $*.c $(LIBAPL)
	cp *.so $(LIBDIR)
