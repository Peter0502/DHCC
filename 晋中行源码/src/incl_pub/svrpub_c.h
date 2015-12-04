/**********************************************************************
    程序名:svrpub.h
-----------------------------------------------------------------------
    本程序包括系统公用变量.
**********************************************************************/
#define	QDJY	"8101"

static char sndfiles[100];
static char rcvfiles[100];

long	TraDate;       /* 前台当日营业日期 */
long	SysDate;       /* 系统当日营业日期 */

char RcvPack[1000];

int PackLen;

int RespCode;

struct PVal_St {
	char	sjbm[5];
	int	length;
	int	prec;
	int	lx;
	unsigned char	*val;
	};

struct Bit_St	{
	int	val;   
	char	lx;
	int	dcd;
	int	len;
	};

#ifndef EXTERN
int	PuVal_num;		/*公用变量数目*/
int	Pub_leng;		/*公用变量缓冲区长度*/
char   *Pub_Buf;		/*公用变量缓冲区头指针*/
char    Rec_Bitp[129];          /*接收的比特图表*/
struct  PVal_St *Pu_Val;	/*共用变量指针*/
struct  Bit_St Bit_Arr[128];	/*交易包位图与公用变量对照表*/
char   *Old_Pub_Buf;		/*公用变量缓冲区头指针*/
struct  PVal_St *Old_Pu_Val;	/*共用变量指针*/
#else
extern int PuVal_num;/*公用变量数目*/
extern int Pub_leng;  /*公用变量缓冲区长度*/
extern char   *Pub_Buf;/*公用变量缓冲区头指针*/
extern char    Rec_Bitp[129];/*接收的比特图表*/
extern struct  PVal_St *Pu_Val;    /*共用变量指针*/
extern struct  Bit_St Bit_Arr[128];    /*交易包位图与公用变量对照表*/
extern char   *Old_Pub_Buf;		/*公用变量缓冲区头指针*/
extern struct  PVal_St *Old_Pu_Val;	/*共用变量指针*/
#endif
