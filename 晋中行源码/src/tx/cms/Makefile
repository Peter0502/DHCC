SHELL	=	sh

CC		=	cc $(Q64) -g
#CC		=	xlc $(Q64) 
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
LIBAPL  = -lbaltcp -lcomm -lbase -lcom -lpubdb -lln -lcif -lpubdb_m -lcard -lc -lm -lhv -lC -llv -lsttl

OBJS	= sp6670.so \
	sp6671.so\
	sp6672.so\
	sp6673.so\
	sp6674.so\
	sp6675.so\
	sp6676.so\
	sp6677.so\
	sp6678.so\
	sp6679.so\
	sp6680.so\
	sp6681.so\
	sp6682.so\
	sp6683.so\
	sp6684.so\
	sp6685.so\
	sp6687.so\
	sp6688.so\
	sp4541.so\
	spL542.so\
	spL543.so
TARGET	= $(BIN)/lib.so

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
	$(CC) $(CFLAGS) -g -s -c $*.c
	$(CC) -G  -o $@ $(CFLAGS) $*.o $(LIBAPL)
	mv $(*F).so $(LIBDIR)
	$(RM) $(*F).o

.c:
	$(CC) -g -s -G  -o $@ $(CFLAGS) $*.c $(LIBAPL)
	mv *.so $(LIBDIR)

# default
 
#.DEFAULT:
 
# clearing object codes
 
clean: cleanup
	$(RM) $(TARGET)
 
cleanup:
	$(RM) $(OBJS)
 
# dependencies

