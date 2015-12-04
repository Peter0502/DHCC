#define INT32 int
#define EJF_ITP 0
#define EJF_CARD 1
#define EJF_AGENT 2
#define EJF_OTHER1 3
#define EJF_OTHER2 4
#define ITP_SNO 1/*�ۺ�ҵ����ˮ��ʼ��*/
#define CARD_SNO 1
#define AGENT_SNO 1
#define OTHER1_SNO 1
#define OTHER2_SNO 1
/*Add by SSH,2004.9.7,Ϊ�˴�����֧������ˮ�ţ�����һ��ӳ��*/
#define EJF_IDTNO EJF_ITP
#define EJF_REFNO EJF_CARD
#define EJF_LTXNO EJF_AGENT
#define EJF_HTXNO EJF_OTHER1
#define EJF_CANCNO EJF_OTHER2
#define EJF_QUERYNO	 EJF_ITP
#define EJF_CALLBACKNO EJF_CARD
/***********************************************************/
typedef struct _t_ejfno{
 	INT32 tlrno;
 	INT32 ejfno_itp;/*�ۺ�ҵ����ˮ*/
 	INT32 ejfno_card;/*��ҵ����ˮ*/
 	INT32 ejfno_agent;/*�м�ҵ����ˮ*/
 	INT32 ejfno_other1;/*������ˮ1*/
 	INT32 ejfno_other2;/*������ˮ2*/
	/*Add by SSH,2004.9.1,��־��ˮ���Ƿ���Ĺ�*/
	char  itp_change;
	char  card_change;
	char  agent_change;
	char  other1_change;
	char  other2_change;
}TEJFNO;
/****************************************************************************
*��ˮ���ļ�(ejfno.dat)�Ľṹ��8�ֽ�����+TEJFNO...
*****************************************************************************/
int openEjfnoFile();
int closeEjfnoFile();
int setDate(char *pcDate);
char  *getDate();
int lock_area(int nOffset,int nLength);
int lock_all();
int unlock();
int findTlrno(INT32 nTlrno);
int insertTlrno(INT32 nTlrno);
void initEjfnoRecord(INT32 nTlrno,TEJFNO *pEjfno);
int initAllTlrno();
int initEjfnoFile(INT32 nTlrno,char *pcDate);
int fetchEjfno(INT32 nTlrno,int ejfType);
int rollbackEjfno(INT32 nTlrno);
int commitEjfno(INT32 nTlrno);
