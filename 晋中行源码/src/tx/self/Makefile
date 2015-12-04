
SHELL	=	sh

CC		=	cc $(Q64) -g
LD		=	ld

RM		=	@rm -f
MV		=	@mv
CP		=	@cp
ECHO	=	@echo


# paths and flags
 
BIN	= $(HOME)/bin
LIBDIR  = $(HOME)/lib

INC	= -I$(HOME)/src/incl_c -I$(HOME)/src/incl_ec -I$(HOME)/src/incl_pub
CLIB	= -L$(HOME)/lib -L.
CFLAGS	= $(INC) $(CLIB)
#LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lpubdb -lpubdb_m -lc -lm
LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lsttl -lpubdb -lpubdb_m -lc -lm

OBJS	=		spF001.so spF002.so spF003.so spF004.so spF005.so spF006.so \
					spF007.so spF008.so spF009.so spF010.so spF011.so spF012.so \
					spF013.so spF014.so spF015.so spF016.so spF017.so \
					spF020.so spF021.so spF022.so spF023.so spF024.so spF025.so \
					spF026.so spF027.so spF028.so spF029.so spF030.so spF031.so \
					spF032.so spF033.so spF034.so spF035.so spF036.so spF037.so \
					spF038.so spF039.so spF040.so spF041.so spF042.so spF043.so \
					spF044.so spF045.so spF046.so spF047.so spF048.so spF049.so \
					spF050.so spF051.so spF060.so spF061.so spF062.so spF063.so \
					spF064.so sp9999.so spF068.so spF070.so spF071.so \
					spF072.so spF074.so spF081.so spF090.so spF091.so\
					spF075.so sp6240.so sp6239.so sp6236.so sp6238.so \
					spF254.so spF253.so spF252.so spF251.so spF250.so spF249.so spF248.so \
					spF246.so spF245.so spF241.so spF238.so spF237.so spF236.so spF230.so \
					sp6244.so sp6242.so sp6240.so sp6239.so sp6235.so \
					sp6234.so sp6233.so sp6232.so sp4999.so sp6255.so sp6253.so sp6254.so\
					sp6243.so sp6247.so \
					sp7020.so sp7021.so sp7022.so sp7023.so sp7024.so sp7025.so \
					sp7026.so sp7030.so sp7032.so \
					spF000.so spF059.so sp8995.so sp9210.so sp6258.so sp6612.so
					
#spF080.so
TARGET	= \
		$(BIN)/lib.so

all: Mstart $(TARGET) Mcomplete
 
Mstart:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) ..."
	$(ECHO)

Mcomplete:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) completed."
	$(ECHO)
 
$(TARGET):$(OBJS)


# implicit
 
.SUFFIXES: .so .c
 
.c.so:
	$(CC) $(CFLAGS) -c $*.c
	$(CC) -G -o $@ $(CFLAGS) $*.o $(LIBAPL)
	mv $(*F).so $(LIBDIR)
	$(RM) $(*F).o

.c:
	$(CC) -G -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so $(LIBDIR)

# default
 
#.DEFAULT:
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies

