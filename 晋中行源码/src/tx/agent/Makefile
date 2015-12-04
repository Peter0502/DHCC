
CC	=	cc $(Q64)
RM	=	rm	-f
MV	=	mv
CP	=	cp

BIN = $(HOME)/bin
LIBDIR = $(HOME)/lib

INC = -I$(HOME)/src/incl_c -I$(HOME)/src/incl_pub
LIB = -L$(HOME)/lib
CFLAGS = $(INC) $(LIB)

LIBAPL = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lagent -lc -lm -lbase

OBJS =  spF323.so  spF328.so  spF403.so  spF408.so  spF413.so  spF606.so  spF617.so  spF626.so  spF650.so \
	spF680.so  spH601.so  spH608.so  spH621.so  spH628.so  spH633.so  spH661.so  spH673.so  spH804.so \
    	spF324.so  spF329.so  spF404.so  spF409.so  spF414.so  spF610.so  spF620.so  spF627.so  spF651.so  spF685.so \
	spH602.so  spH614.so  spH622.so  spH629.so  spH651.so  spH664.so  spH676.so  spH805.so \
	sp7605.so  spF325.so  spF330.so  spF405.so  spF410.so  spF415.so  spF611.so  spF621.so  spF630.so  spF660.so \
	spF686.so  spH625.so  spH630.so  spH652.so  spH666.so  spH801.so  spH806.so \
	spF321.so  spF326.so  spF401.so  spF406.so  spF411.so  spF416.so  spF613.so  spF624.so  spF640.so  spF661.so \
 	spF687.so  spH606.so  spH619.so  spH626.so  spH631.so  spH653.so  spH667.so  spH802.so  spH807.so \
	spF322.so  spF327.so  spF402.so  spF407.so  spF412.so  spF417.so  spF615.so  spF625.so  spF641.so  spF670.so \
	spF690.so  spH607.so  spH627.so  spH632.so  spH671.so  spH803.so	spR001.so	spR002.so
#spH618.so spH654.so
#spH604.so
#spH620.so
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
	$(CC) -G  -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so $(LIBDIR)
	
