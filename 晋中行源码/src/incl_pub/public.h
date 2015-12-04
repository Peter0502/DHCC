#ifndef _PUBLIC_H_
#define _PUBLIC_H_
/*********************   ͷ�ļ�   ************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pub_tx.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "ln_mst_c.h"
#include "in_mst_c.h"
#include "od_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_eod_exec_c.h"

#include "dd_parm_c.h"
#include "td_parm_c.h"
#include "ln_parm_c.h"
#include "in_parm_c.h"
#include "od_parm_c.h"
#include "dic_data_c.h"

#include "hv_MBFETRAN.h"  /*MBFE�ӿڽ��׽ṹ����*/

#include "pubf_base_fun.h"           /*pubf/base�µĺ���*/
/*#include "pubf_com_fun.h"            *pubf/com�µĺ���*/
#include "pubf_ln_fun.h"            /*pubf/ln�µĺ���*/
#include "pubf_od_fun.h"            /*pubf/od�µĺ���*/
#include "pubf_cif_fun.h"            /*pubf/cif�µĺ���*/
#include "pubf_rept_fun.h"            /*pubf/rept*/

#ifndef ACC_NO_LVL3_LEN
#define ACC_NO_LVL3_LEN 8
#endif

#define     SYS_CODE_AGENT  "94491611"
#define     SYS_CODE_SEND  "94491610"

/*********************   �궨��   ************************/
#define MYKEY   "1234567890abcdef"               /*������Ѻ����Կ*/

#define 	OLD_AC_LEN 		20			/*���˺ų���*/ 
#define    	TRACELEN     	1024        /*�������ַ�������*/
#define		LENG			20			/*����20�ʴ�ӡ���˵�*/
#define		RECK_LENG		50			/*���˵�һҳ����*/
#define		BANK_BOOK_LENG	70			/*������ҳ��ӡ70��*/
#define		CHGBOOK_CODE	"2410"			/*��֤���״���*/
#define		DD_BOOK_AC_SEQN	1			/*���ڴ����˺����*/
#define		CRD_HEAD		"940071733" /*940071733-����*/
#define     CASH_HEAD       "9101"      /* Ǯ���в���Ա���ʺŵ�ǰ��λ */
#define		NULL_MDM_CODE	"0000"		/*�ս��ʽ��ʴ���*/
#define		NULL_MDM		"----"		/*�ս��ʽ��ʴ���,����mdm_prdt�б�ʾ��*/
#define		NULL_DC_CODE	"0000"		/*�ջ�ƴ��룺�ֽ��ո���*/

#define		ALL_SEQN	 9999	 	/* 9999��ʾ�������˺���Ŷ��ܺ��� */
#define		ZERO_SEQN	 -1	 		/* �˻����-1��ʾ�ѿ����ʣ�δ���� */
#define		SINGLE_SEQN	 0	 		/* ��������ĵ��˻����ʵ��˻���� */
#define		BEG_SEQN	 199	 	/* �˻���Ż��������С�� */

#define     HQLLBH       "100"        /* �������ʱ�� */
#define     ZCZQ03       "213"        /* ������������ȡ���ʱ�� */
#define     ZCZQ06       "216"        /* ������������ȡ���ʱ�� */
#define     ZCZQ12       "201"        /* 12����������ȡ���ʱ�� */
#define     ZCZQ24       "202"        /* 24����������ȡ���ʱ�� */
#define     ZCZQ36       "203"        /* 36����������ȡ���ʱ�� */
#define     BAOZHI       "Z01"        /* ��ֵ���ʱ�� */
#define     YTTZCK       "A00"        /* ��ǰһ��֪ͨ��� */
#define     QTTZCK       "B00"        /* ��ǰ����֪ͨ��� */
#define     SLBH         "001"        /* ��Ϣ˰˰�ʱ�� */
#define     L360         360          /* 360�� */
#define     L100         100          /* �ٷֺŵĵ��� */
#define     Lone         1            /* �������,���Ͳ��� */
#define		RMB			 "01"		  /* ����� */

#define INVEST_LONG  "934"         /* �г���ծȯͶ�ʲ�Ʒ��� */
#define INVEST_OVER  "935"         /* �г���ծȯͶ����۲�Ʒ��� */
#define INVEST_ABATE "936"         /* �г���ծȯͶ���ۼ۲�Ʒ��� */
#define INVEST_LX    "937"         /* Ͷ��Ӧ����Ϣ��Ʒ��� */
#define INVEST_LX1   "960"         /* Ӧ��ծȯ��Ϣ��Ʒ���*/

#define REBUY_LOAN_NT "920"         /* ����ծȯ�ع�������Ʒ��� */
#define REBUY_LOAN_ENT "921"        /* ��������ծȯ�ع�������Ʒ��� */
#define REBUY_LOAN_QT "922"         /* ����֤ȯ�ع�������Ʒ��� */
#define REBUY_LOAN_FT "923"         /* ���뷵��Ʊ�ݽ�����Ʒ��� */
#define REBUY_BORROW_NT   "925"         /* ����ծȯ�ع�Э�������Ʒ��� */
#define REBUY_BORROW_ENT  "926"         /* ��������ծȯ�ع�Э�������Ʒ��� */
#define REBUY_BORROW_QT  "927"          /* ����֤ȯ�ع�Э�������Ʒ��� */
#define REBUY_BORROW_FT  "928"          /* Ʊ�ݻع�Э�������Ʒ��� */

#define REST_AC_RECE    "907"         /* ����Ӧ�տ��Ʒ���(13903) */ 

#define CITY         "ZZS"           /* ���������ڳ�ɳ�ı�־ */

#define DC_TX_CODE 		"0160"
#define DC_OPN_BR_NO 	"0020"
#define DC_TX_BR_NO 	"0030"
#define DC_SUB_TX_CODE  "0180"
#define DC_REPLY 		"0120"
#define DC_TRACE_NO 	"0040"
#define DC_TRACE_CNT 	"0170"
#define DC_TEL 			"0070"
#define DC_CHK 			"0080"
#define DC_AUTH 		"0091"
#define DC_AUTH_PSW 	"0092"
#define DC_TIME 		"0060"
#define DC_DATE 		"0050"
#define DC_TTY 			"0100"
#define DC_FILE_SND 	"0110"
#define DC_GET_MSG 		"0130"
#define DC_CNT 			"0010"
#define DC_AUTH_CODE 	"0140"
#define DC_CODE 		"0151"

#define OD_PRDT_CODE	"4ZZ"
#define OD_DR_AMT		"015B"
#define OD_CR_AMT		"015C"
#define OD_INTST_AMT	"015D"

#define DCBT_TX_CODE 	16
#define DCBT_AUTH_CODE 	14
#define DCBT_TRACE_NO 	4
#define DCBT_FILE_SND 	11
#define DCBT_REPLY 		12
#define DCBT_GET_MSG 	13
#define DCBT_CNT 		1
#define DCBT_TEL 		7
#define DCBT_CHK 		8
#define DCBT_TYY         10

#define LN_INTST_TYPS   15 	/*������Ϣ������*/

#define TOT_BR_NO   "10000" 	/* ���л����� */
#define QS_BR_NO    "10001" 	/* �������� */
#define CW_BR_NO    "19801" 	/* ���� */
#define KJ_BR_NO    "19999" 	/* �Ƽ� */

#define BANK_ENAME	"JINZ"
#define BANK_CNAME	"�����г���������"

#define TESTCARD    "9115000000353" 	/*��ʱ������ؿ�������ʺ� */
#define TESTCARDID    900000198 	/* ��ʱ������ؿ�������ʺ� */

struct st_money_dtl {		/* ȯ��Ľṹ */
        char money100[17];
        char money50[17];
        char money20[17];
        char money10[17];
        char money5[17];
        char money2[17];
        char money1[17];
        char money05[17];
        char money02[17];
        char money01[17];
        char money005[17];
        char money002[17];
        char money001[17];
};
/*********************  ȫ�ֱ���  ************************/
/*�����е�����*/
#ifndef EXTERN  
struct pub_tx g_pub_tx;              /*ͨ�ü��ʽṹ*/
struct mdm_ac_rel_c g_mdm_ac_rel;    /*�����ʺŶ��ձ�*/
struct dd_mst_c g_dd_mst;            /*���ڲ�Ʒ*/
struct td_mst_c g_td_mst;            /*���ڲ�Ʒ*/
struct ln_mst_c g_ln_mst;            /*�����Ʒ*/
struct in_mst_c g_in_mst;            /*�����Ʒ*/
struct od_mst_c g_od_mst;            /*͸֧��Ʒ*/

struct dd_parm_c g_dd_parm;
struct td_parm_c g_td_parm;
struct ln_parm_c g_ln_parm;
struct in_parm_c g_in_parm;
struct od_parm_c g_od_parm;


struct S_pub_intst g_pub_intst;      /* ��Ϣ�ṹ */

struct MBFETRAN mbfetran;

char   acErrMsg[TRACELEN];			 /*�����õı���*/
int	   g_reply_int;					 /*��������ֵ*/
char RtCode[5];      /* ������Ӧ���� */
char g_rcvpack[3000];
int Gl_file_first;
double g_acc_amt[2];
struct dic_data_c g_dic_data_a[100]; /*������*/
int g_dic_data_i;

/*һ����������*/
#else

extern struct pub_tx g_pub_tx;              /*ͨ�ü��ʽṹ*/
extern struct mdm_ac_rel_c g_mdm_ac_rel;    /*�����ʺŶ��ձ�*/
extern struct dd_mst_c g_dd_mst;            /*���ڲ�Ʒ*/
extern struct td_mst_c g_td_mst;            /*���ڲ�Ʒ*/
extern struct ln_mst_c g_ln_mst;            /*�����Ʒ*/
extern struct od_mst_c g_od_mst;            /*͸֧��Ʒ*/
extern struct in_mst_c g_in_mst;            /*�ڲ���Ʒ*/

extern struct dd_parm_c g_dd_parm;
extern struct td_parm_c g_td_parm;
extern struct ln_parm_c g_ln_parm;
extern struct od_parm_c g_od_parm;
extern struct in_parm_c g_in_parm;

struct com_eod_exec_c grz_com_eod_exec;/*ȫ��,�������ӳ��򴫵ݿ�������*/
extern struct S_pub_intst g_pub_intst;      /* ��Ϣ�ṹ */

extern struct MBFETRAN mbfetran;

extern char   acErrMsg[TRACELEN];			 /*�����õı���*/
extern int	   g_reply_int;					 /*��������ֵ*/
extern	char RtCode[5];      /* ������Ӧ���� */
extern char g_rcvpack[3000];
extern int Gl_file_first;
extern double g_acc_amt[2];
extern struct dic_data_c g_dic_data_a[100]; /*������*/
extern int g_dic_data_i;

#endif

/*******���������־�궨�� TRACE��WRITEMSG*********/ 


/* �������LOG�궨�� */
#define TRACE \
        sprintf( acErrMsg,"File : [ %15s ] Line : [ %5d ]",__FILE__,__LINE__); \
                vtcp_log(acErrMsg); \
                memset( acErrMsg , 0x00 , sizeof ( acErrMsg ) );

#define WRITEMSG \
        vtcp_log( "File : [ %15s ] Line : [ %5d ]",__FILE__,__LINE__); \
        if ( strlen( acErrMsg ) ) \
        vtcp_log( ">>> [%s]", acErrMsg  ); \
        memset( acErrMsg , 0x00 , sizeof ( acErrMsg ) ); 
        

/*********************  ��������  ************************/

#endif

#ifndef PARM_CWKH       /* �ۻ�������ţ���lv_define.h hv_define.h�е������ͬ���ʵ� */
#define PARM_CWKH 9
#endif
long int iGlobal_Pu_Val_Addr;
#define TEL_LENGTH 4	/* ��Ա�ų��� Add by hxc 091110 */
#define CDK_IND  "92" /*���̿�Ǽǲ���� add by hxc 091110*/
/**�������Ʊ�յ�����ʱ������Ӧ�տ�ר��***/
#ifndef PARM_ECDS_TR_PAY_AC
#define PARM_ECDS_TR_PAY_AC 54
#endif
/***�������Ʊ������Ӧ����ר��2011****/
#ifndef PARM_ECDS_TC_AC
#define PARM_ECDS_TC_AC 53
#endif
