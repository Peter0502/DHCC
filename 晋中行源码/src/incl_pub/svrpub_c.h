/**********************************************************************
    ������:svrpub.h
-----------------------------------------------------------------------
    ���������ϵͳ���ñ���.
**********************************************************************/
#define	QDJY	"8101"

static char sndfiles[100];
static char rcvfiles[100];

long	TraDate;       /* ǰ̨����Ӫҵ���� */
long	SysDate;       /* ϵͳ����Ӫҵ���� */

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
int	PuVal_num;		/*���ñ�����Ŀ*/
int	Pub_leng;		/*���ñ�������������*/
char   *Pub_Buf;		/*���ñ���������ͷָ��*/
char    Rec_Bitp[129];          /*���յı���ͼ��*/
struct  PVal_St *Pu_Val;	/*���ñ���ָ��*/
struct  Bit_St Bit_Arr[128];	/*���װ�λͼ�빫�ñ������ձ�*/
char   *Old_Pub_Buf;		/*���ñ���������ͷָ��*/
struct  PVal_St *Old_Pu_Val;	/*���ñ���ָ��*/
#else
extern int PuVal_num;/*���ñ�����Ŀ*/
extern int Pub_leng;  /*���ñ�������������*/
extern char   *Pub_Buf;/*���ñ���������ͷָ��*/
extern char    Rec_Bitp[129];/*���յı���ͼ��*/
extern struct  PVal_St *Pu_Val;    /*���ñ���ָ��*/
extern struct  Bit_St Bit_Arr[128];    /*���װ�λͼ�빫�ñ������ձ�*/
extern char   *Old_Pub_Buf;		/*���ñ���������ͷָ��*/
extern struct  PVal_St *Old_Pu_Val;	/*���ñ���ָ��*/
#endif
