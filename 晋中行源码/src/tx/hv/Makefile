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
#	-lpubdb_m -lass  -lcom -lrept -lbase -lde  -lod  -lrpt  -lhv
LIBAPL = $(LIB) -lacct -lcard -leod -lpubdb -lsttl -lagent -lcif -lln \
	-lpubdb_m -lass  -lcom -lrept -lbase -lde  -lod  -lrpt

OBJS =  spJ001.so spJ002.so spJ003.so spJ004.so spJ005.so spJ006.so spJ007.so spJ008.so spJ009.so \
	spJ010.so spJ011.so spJ012.so spJ013.so spJ014.so spJ015.so spJ016.so spJ017.so spJ018.so \
	spJ019.so spJ020.so spJ021.so spJ022.so spJ023.so spJ024.so spJ025.so spJ026.so spJ027.so \
	spJ998.so spJ999.so spJ086.so spJ911.so spJ912.so \
	spJLZ1.so spJ069.so \
	spJ081.so spJ090.so spJ091.so spJ020.so spHPSP.so spJ999.so spPARM.so spJ620.so spJ610.so spJ013.so hv_telmsg.so
		
TARGET = $(BIN)/lib.so

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
	cp *.so $(LIBDIR)
