RM = rm
MV = mv
CC = cc $(Q64)

AR = ar -rv
LIBSRC = $(HOME)/src/pubf/acct
LIBDIR = $(HOME)/lib

INCLUDE =	-I$(INFORMIXDIR)/incl/esql \
			-I$(HOME)/src/incl_pub \
			-I$(HOME)/src/incl_c 

CCFLAGS = $(INCLUDE) -qmaxmem=-1


OBJ =	pubf_agent_base.o pubf_message.o pubf_mob_msg.o  pubf_mob_data_proc.o

all:libagent.so 

libagent.so: $(OBJ)
	$(CC) -G -o $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

libagent.a: $(OBJ)
	$(AR) $(X64)  $@  $(OBJ)
	$(MV) $@ $(HOME)/lib

#*******************************************/
.SUFFIXES : .c

.c.o:
		$(CC)  $(CCFLAGS) -O -g -s -c $<

clear:
	rm -f *.o
