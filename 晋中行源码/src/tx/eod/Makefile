
SHELL	=	sh

#Q64     =   -q64
CC		=	cc $(Q64)  -g
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
LIBAPL  = -lbaltcp -leod -lcomm -lbase -lcom -lpubdb -lpubdb_m -lass -lc -lm -lC -llv -lhv


OBJS	= gD007.so gD0161.so gD031.so gD044.so gD066.so gD086.so gD096.so gD112.so gD133.so gD291.so gD400.so gD812.so gDTSC.so \
          gB001.so gD008.so gD017.so gD032.so gD045.so gD067.so gD087.so gD097.so gD113.so gD134.so gD293.so gD407.so gD888.so gDX02.so \
	  gD001.so gD009.so gD019.so gD033.so gD048.so gD068.so gD088.so gD098.so gD114.so gD139.so gD301.so gD500.so gD901.so gDX04.so \
	  gD002.so gD010.so gD020.so gD034.so gD049.so gD069.so gD089.so gD101.so gD120.so gD188.so gD302.so gD501.so gD995.so gDX05.so \
	  gD003.so gD011.so gD021.so gD035.so gD050.so gD075.so gD090.so gD102.so gD189.so gD303.so gD996.so gDX06.so \
          gD0033.so gD012.so gD022.so gD036.so gD052.so gD080.so gD091.so gD103.so gD190.so gD304.so gD999.so gDX07.so \
          gD0034.so gD013.so gD023.so gD037.so gD082.so gD092.so gD104.so gD123.so gD198.so gD307.so gDCLL.so gDX08.so \
          gD004.so gD014.so gD024.so gD041.so gD054.so gD083.so gD093.so gD105.so gD124.so  gD322.so gD800.so gDCLN.so gDX10.so \
          gD005.so gD015.so gD025.so gD042.so gD055.so gD084.so gD094.so gD110.so gD125.so gD271.so gD334.so gD808.so gDFXPG.so gl_sum.so \
	  gD006.so gD016.so gD030.so gD043.so gD056.so gD085.so gD095.so gD111.so gD126.so gD281.so gD339.so gD811.so gDLRE.so \
	  gD156.so gD157.so gD158.so gDZH.so gDJZ.so
#gD130.so gD131.so gD132.so gD135.so gD136.so gD137.so gD138.so 缺源码(没有用)
#gD053.so gD701.so gD702.so gD121.so gD122.so gD703.so有问题？

TARGET	= \
		$(BIN)/lib.so

all: Mstart $(TARGET)  Mcomplete
 
Mstart:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) ..."
	$(ECHO)

Mcomplete:
	$(ECHO)
	$(ECHO) "MAKE lib.so:$(TARGET) completed."
	$(ECHO)

 
$(TARGET): $(OBJS) 


# implicit
 
.SUFFIXES: .so .c .o .x
.c.so:
	$(CC) $(CFLAGS) -c $*.c
	$(CC) -G -o $@ $(CFLAGS) $*.o $(LIBAPL)
	mv $(*F).so $(LIBDIR)
#	$(RM) $(*F).o

.c.o:
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


# default
 
#.DEFAULT:
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies

