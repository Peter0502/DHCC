
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
#LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lcif -lpubdb -lpubdb_m -lc -lm -ln
LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lcif -lpubdb -lpubdb_m -lc -lm -lln

OBJS = 	spL102.so  spL202.so spL205.so spL302.so spL305.so spL308.so spL310.so spL313.so spL316.so spL319.so spL3A1.so spL413.so spLT01.so spLT04.so \
	spL001.so spL103.so spL10C.so spL203.so spL206.so spL303.so spL306.so spL309.so spL311.so spL314.so spL317.so spL320.so spL3A2.so spL888.so spLT02.so spLT12.so \
	spL101.so spL104.so spL201.so spL204.so spL301.so spL304.so spL307.so spL30B.so spL312.so spL315.so spL318.so spL399.so spL401.so spL999.so spLT03.so spL322.so spL323.so

#spL105.so

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

