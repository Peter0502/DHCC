
#define DEFAULT_SIZE 10

#define NULLPOS  -1

typedef struct {
	void **buf;
	long buf_size;
	long init_size;
	long size;
} OBJLIST;

typedef OBJLIST* POBJLIST;

typedef long OLPOSITION;
typedef OLPOSITION* POLPOSITION;

extern int OLInit(POBJLIST, int);
extern int OLIsEmpty(POBJLIST);
extern long OLLen(POBJLIST);
extern void OLEmpty(POBJLIST);
extern void *OLAdd(POBJLIST, void *);
extern void *OLInsert(POBJLIST, void *);
extern int OLRemoveAt(POBJLIST, POLPOSITION);
extern void *OLGetHead(POBJLIST);
extern void *OLGetTail(POBJLIST);
extern OLPOSITION OLGetHeadPos(POBJLIST);
extern OLPOSITION OLGetTailPos(POBJLIST);
extern void *OLGetNext(POBJLIST, POLPOSITION);
extern void *OLGetPrev(POBJLIST, POLPOSITION);
extern void *OLGetAt(POBJLIST, POLPOSITION);
