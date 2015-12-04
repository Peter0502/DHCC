#
# NAME:       makefile.aix
# AUTHOR:
# VERSION:    1.1
# DATE:       98/03/19
#

#ESQL=$(INFORMIXDIR)/bin/esql -q64
ESQL=$(INFORMIXDIR)/bin/esql 
MV=mv

INCLUDE=-I$(HOME)/src/incl_pub -I$(HOME)/src/incl_ec -I$(HOME)/src/incl_c

CFLAGS	= -O -c -g
#USERLDFLAGS= -lc -lm -L$(HOME)/lib -lbase -lpubdb -lpubdb_m
USERLDFLAGS= -lc -lm -L$(HOME)/lib -lln -lbase -lacct -lpubdb -lass -lcom -lpubdb_m -lcif -lde -lc -lm -lagent -lcard -lod

#LDFLAGS= -lc -lm -L./lib   -linit 
LDFLAGS= $(USERLDFLAGS)

CCFLAGS= -L/usr/lpp/encina/lib \
		-L$(INFORMIXDIR)/lib \
		-L$(INFORMIXDIR)/lib/esql \
		-I$(INFORMIXDIR)/incl/esql \
		-DSELECT  


OBJ01=mycl.o
OBJ02=robcs.o
OBJ03=do_cif_name.o

all :  mycl do_cif_name robcs clear

mycl: $(OBJ01) 
	$(ESQL) $(CCFLAGS) -s -O -o $@ $(OBJ01) $(LDFLAGS)
	$(MV) $@ $(BINDIR)/$@
mycl.o : mycl.c
	$(ESQL) ${CFLAGS}  $(INCLUDE) mycl.c

do_cif_name: $(OBJ03) 
	$(ESQL) $(CCFLAGS) -s -O -o $@ $(OBJ03) $(LDFLAGS)
	$(MV) $@ $(BINDIR)/$@
do_cif_name.o : do_cif_name.c
	$(ESQL) ${CFLAGS}  $(INCLUDE) do_cif_name.c

robcs: $(OBJ02) 
	$(ESQL) $(CCFLAGS) -s -O -o $@ $(OBJ02) $(LDFLAGS)
	$(MV) $@ $(BINDIR)/$@
robcs.o : robcs.c
	$(ESQL) ${CFLAGS}  $(INCLUDE) robcs.c

clear:
	rm *.o 
