CC	=	cc $(Q64) -g
RM	=	rm	-f
MV	=	mv
CP	=	cp

BIN = $(HOME)/bin
LIBDIR = $(HOME)/lib

INC = -I$(HOME)/src/incl_c -I$(HOME)/src/incl_pub
LIB = -L$(HOME)/lib
CFLAGS = $(INC) $(LIB)

LIBAPL = -lbaltcp -lcomm -lbase -lacct -lcom -lpubdb -lpubdb_m -lc -lm -lC 
OBJS =  spD005.so spD029.so spD085.so spD096.so spD111.so spD203.so spD221.so \
	spD243.so spD261.so spD269.so spD285.so spD296.so spD305.so spD403.so \
	spD412.so spD442.so spD610.so spD702.so \
	spC703.so spD006.so spD033.so spD086.so spD097.so spD112.so spD204.so \
	spD222.so spD251.so spD262.so spD271.so spD286.so spD306.so spD405.so \
	spD413.so spD505.so spD611.so spD704.so \
	spC706.so spD007.so spD034.so spD087.so spD098.so spD113.so spD205.so \
	spD223.so spD252.so spD263.so spD272.so spD290.so spD307.so spD406.so \
	spD414.so spD601.so spD612.so spD888.so \
	spC708.so spD010.so spD034.so spD088.so spD099.so spD114.so spD209.so \
	spD231.so spD253.so spD264.so spD273.so spD2A8.so spD308.so spD407.so \
	spD415.so spD602.so spD666.so spDGJJ.so \
	spD011.so spD035.so spD089.so spD107.so spD120.so spD211.so spD232.so \
	spD254.so spD265.so spD281.so spD292.so spD301.so spD398.so spD408.so \
	spD432.so spD603.so spD667.so spJC01.so \
	spD001.so spD012.so spD035.so spD090.so spD108.so spD200.so spD212.so \
	spD233.so spD255.so \
	spD266.so spD282.so spD293.so spD302.so spD399.so spD409.so spD439.so \
	spD604.so spD698.so spJC02.so \
	spD002.so spD013.so spD036.so spD091.so spD201.so spD213.so \
	spD241.so spD256.so spD267.so spD283.so spD303.so spD400.so spD410.so \
	spD440.so spD607.so spD699.so spL998.so \
	spD003.so spD021.so spD084.so spD095.so spD110.so spD202.so spD214.so \
	spD242.so spD257.so spD268.so spD284.so spD304.so spD401.so spD411.so \
	spD441.so spD609.so spD701.so \
	spD313.so spD316.so
#spD109.so
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


