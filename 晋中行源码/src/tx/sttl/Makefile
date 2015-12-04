
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
LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lsttl -lpubdb -lpubdb_m -lc -lm -lC
OBJS =  spB207.so  spB335.so spB346.so spB356.so spB380.so spB432.so spB503.so spB510.so \
	spB558.so  spB606.so spB705.so spB903.so spB916.so spB932.so spB943.so \
	sp9571.so  spB336.so spB347.so spB357.so spB381.so spB433.so \
	spB504.so spB511.so spB559.so spB592.so spB607.so spB706.so spB904.so spB921.so spB933.so spB944.so \
	sp9572.so spB330.so spB337.so spB371.so spB383.so spB434.so spB505.so \
	spB552.so spB561.so spB601.so spB608.so spB801.so spB905.so spB922.so spB934.so spB945.so \
	sp9573.so spB331.so spB338.so spB372.so spB411.so spB491.so \
	spB506.so spB554.so spB562.so spB602.so spB701.so spB802.so spB911.so spB923.so spB935.so spB947.so \
	spB332.so spB343.so spB353.so spB373.so spB412.so spB492.so spB507.so \
	spB555.so spB563.so spB603.so spB702.so spB803.so spB912.so spB924.so spB937.so \
	spB333.so spB344.so spB354.so spB374.so spB413.so spB501.so spB508.so spB556.so \
	spB564.so spB604.so spB703.so spB804.so spB913.so spB925.so spB941.so \
	spB334.so spB345.so spB355.so spB376.so spB431.so spB502.so spB509.so \
	spB557.so spB565.so spB605.so spB704.so spB901.so spB914.so spB931.so spB942.so

#	spB101.so spB102.so spB103.so spB106.so spB107.so spB108.so spB109.so spB110.so spB104.so spB105.so结构问题 
#	spB436.so缺源码 spB316.so  spB566.so spB310.so spB317.so
# 	spB311.so spB312.so spB313.so spB314.so spB315.so spB351.so spB352.so结构问题 
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
	$(CC) -G  -o $@ $(CFLAGS) $*.o $(LIBAPL)
	cp $(*F).so $(LIBDIR)

#	$(RM) $(*F).o

.c:
	$(CC) -G  -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so $(LIBDIR)

# default
 
#.DEFAULT:
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies

