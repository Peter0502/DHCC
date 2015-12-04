
SHELL	=	sh

#Q64     =   -q64
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
#LIBAPL  = -lbaltcp -leod -lcomm -lbase -lcom -lpubdb -lpubdb_m -lass -lrpt -lc -lm 
LIBAPL  = -lbaltcp -leod -lcomm -lbase -lcom -lpubdb -lpubdb_m -lass -lrpt -lc -lm -llv -lhv -lenvinfo 
#-bnoquiet -lenvinfo
OBJS =  rpt009.so rpt020.so rpt030.so rpt041.so rpt053.so rpt065.so rpt111.so rpt505.so rpt516.so rpt910.so rpt9291.so rpt957.so rpt986.so rpt995.so rptHP2.so rptLC6.so \
	rpt010.so rpt021.so rpt031.so rpt042.so rpt055.so rpt075.so rpt130.so rpt506.so rpt517.so rpt911.so rpt930.so rpt970.so rpt987.so  rptHP3.so rptLC7.so \
	rpt011.so rpt022.so rpt032.so rpt043.so rpt057.so rpt081.so rpt133.so rpt507.so rpt518.so rpt912.so rpt931.so rpt971.so rpt988.so rpt997.so rptHP4.so rptLC8.so \
	rpt012.so rpt023.so rpt033.so rpt044.so rpt058.so rpt082.so rpt134.so rpt508.so rpt519.so rpt913.so rpt935.so rpt972.so rpt989.so rpt998.so rptHP5.so rptLC9.so \
	rpt001.so rpt013.so rpt024.so rpt034.so rpt045.so rpt059.so rpt083.so rpt193.so rpt509.so rpt520.so rpt91a.so rpt936.so rpt973.so rpt990.so rpt999.so rptHP6.so \
	rpt003.so rpt014.so rpt025.so rpt035.so rpt046.so rpt060.so rpt090.so rpt295.so rpt510.so rpt555.so rpt91b.so rpt937.so rpt975.so rpt991.so rpt99T.so rptHP7.so \
	rpt004.so rpt015.so rpt026.so rpt036.so rpt047.so rpt061.so rpt092.so rpt303.so rpt511.so rpt55a.so rpt91c.so rpt938.so rpt981.so rpt992.so  rptLC1.so \
	rpt005.so rpt016.so rpt027.so rpt037.so rpt048.so rpt100.so rpt501.so rpt512.so rpt666.so rpt91d.so rpt939.so rpt982.so rpt993.so rpt9AB.so rptLC2.so \
	rpt006.so rpt017.so rpt028.so rpt038.so rpt049.so rpt063.so rpt101.so rpt502.so rpt513.so rpt801.so rpt927.so rpt93a.so rpt983.so rpt994.so rpt9AC.so rptLC3.so \
	rpt007.so rpt018.so rpt029.so rpt039.so rpt050.so rpt063.so rpt103.so rpt503.so rpt514.so rpt802.so rpt928.so rpt955.so rpt984.so rpt9941.so rpt9AD.so rptLC4.so \
	rpt008.so rpt019.so rpt02a.so rpt040.so  rpt064.so rpt104.so rpt504.so rpt515.so rpt901.so rpt929.so rpt956.so rpt985.so rpt9943.so rptHP1.so rptLC5.so

##rpt052.so rpt076.so »±‘¥¬Î(‘› ±√ª”–)
#rpt996.so   ”
#”–Œ Ã‚  rpt9AA.c
#…˙≤˙…œ≤ª”√  rpt062.so rpt051.so
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
	mv $(*F).so $(LIBDIR)
	$(RM) $(*F).o

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

