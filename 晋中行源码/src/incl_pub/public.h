#ifndef _PUBLIC_H_
#define _PUBLIC_H_
/*********************   头文件   ************************/
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

#include "hv_MBFETRAN.h"  /*MBFE接口交易结构定义*/

#include "pubf_base_fun.h"           /*pubf/base下的函数*/
/*#include "pubf_com_fun.h"            *pubf/com下的函数*/
#include "pubf_ln_fun.h"            /*pubf/ln下的函数*/
#include "pubf_od_fun.h"            /*pubf/od下的函数*/
#include "pubf_cif_fun.h"            /*pubf/cif下的函数*/
#include "pubf_rept_fun.h"            /*pubf/rept*/

#ifndef ACC_NO_LVL3_LEN
#define ACC_NO_LVL3_LEN 8
#endif

#define     SYS_CODE_AGENT  "94491611"
#define     SYS_CODE_SEND  "94491610"

/*********************   宏定义   ************************/
#define MYKEY   "1234567890abcdef"               /*生成密押的密钥*/

#define 	OLD_AC_LEN 		20			/*旧账号长度*/ 
#define    	TRACELEN     	1024        /*跟踪用字符串长度*/
#define		LENG			20			/*超过20笔打印对账单*/
#define		RECK_LENG		50			/*对账单一页笔数*/
#define		BANK_BOOK_LENG	70			/*存折满页打印70笔*/
#define		CHGBOOK_CODE	"2410"			/*换证交易代码*/
#define		DD_BOOK_AC_SEQN	1			/*活期存折账号序号*/
#define		CRD_HEAD		"940071733" /*940071733-株洲*/
#define     CASH_HEAD       "9101"      /* 钱箱中操作员的帐号的前几位 */
#define		NULL_MDM_CODE	"0000"		/*空介质介质代码*/
#define		NULL_MDM		"----"		/*空介质介质代码,用于mdm_prdt中表示空*/
#define		NULL_DC_CODE	"0000"		/*空会计代码：现金收付用*/

#define		ALL_SEQN	 9999	 	/* 9999表示对所有账号序号都能好用 */
#define		ZERO_SEQN	 -1	 		/* 账户序号-1表示已开介质，未开户 */
#define		SINGLE_SEQN	 0	 		/* 除活期外的单账户介质的账户序号 */
#define		BEG_SEQN	 199	 	/* 账户序号活期外的最小号 */

#define     HQLLBH       "100"        /* 活期利率编号 */
#define     ZCZQ03       "213"        /* 三个月整存整取利率编号 */
#define     ZCZQ06       "216"        /* 六个月整存整取利率编号 */
#define     ZCZQ12       "201"        /* 12个月整存整取利率编号 */
#define     ZCZQ24       "202"        /* 24个月整存整取利率编号 */
#define     ZCZQ36       "203"        /* 36个月整存整取利率编号 */
#define     BAOZHI       "Z01"        /* 保值利率编号 */
#define     YTTZCK       "A00"        /* 提前一天通知存款 */
#define     QTTZCK       "B00"        /* 提前七天通知存款 */
#define     SLBH         "001"        /* 利息税税率编号 */
#define     L360         360          /* 360天 */
#define     L100         100          /* 百分号的倒数 */
#define     Lone         1            /* 遇高则高,遇低不低 */
#define		RMB			 "01"		  /* 人民币 */

#define INVEST_LONG  "934"         /* 中长期债券投资产品编号 */
#define INVEST_OVER  "935"         /* 中长期债券投资溢价产品编号 */
#define INVEST_ABATE "936"         /* 中长期债券投资折价产品编号 */
#define INVEST_LX    "937"         /* 投资应收利息产品编号 */
#define INVEST_LX1   "960"         /* 应计债券利息产品编号*/

#define REBUY_LOAN_NT "920"         /* 国家债券回购借出款产品编号 */
#define REBUY_LOAN_ENT "921"        /* 中央银行债券回购借出款产品编号 */
#define REBUY_LOAN_QT "922"         /* 其他证券回购借出款产品编号 */
#define REBUY_LOAN_FT "923"         /* 买入返售票据借出款产品编号 */
#define REBUY_BORROW_NT   "925"         /* 国家债券回购协议借入款产品编号 */
#define REBUY_BORROW_ENT  "926"         /* 中央银行债券回购协议借入款产品编号 */
#define REBUY_BORROW_QT  "927"          /* 其他证券回购协议借入款产品编号 */
#define REBUY_BORROW_FT  "928"          /* 票据回购协议借入款产品编号 */

#define REST_AC_RECE    "907"         /* 其他应收款产品编号(13903) */ 

#define CITY         "ZZS"           /* 株洲商行在长沙的标志 */

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

#define LN_INTST_TYPS   15 	/*贷款利息种类数*/

#define TOT_BR_NO   "10000" 	/* 总行机构号 */
#define QS_BR_NO    "10001" 	/* 清算中心 */
#define CW_BR_NO    "19801" 	/* 财务 */
#define KJ_BR_NO    "19999" 	/* 科技 */

#define BANK_ENAME	"JINZ"
#define BANK_CNAME	"晋中市城市信用社"

#define TESTCARD    "9115000000353" 	/*暂时测试异地卡用这个帐号 */
#define TESTCARDID    900000198 	/* 暂时测试异地卡用这个帐号 */

struct st_money_dtl {		/* 券别的结构 */
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
/*********************  全局变量  ************************/
/*主控中的声明*/
#ifndef EXTERN  
struct pub_tx g_pub_tx;              /*通用记帐结构*/
struct mdm_ac_rel_c g_mdm_ac_rel;    /*介质帐号对照表*/
struct dd_mst_c g_dd_mst;            /*活期产品*/
struct td_mst_c g_td_mst;            /*定期产品*/
struct ln_mst_c g_ln_mst;            /*贷款产品*/
struct in_mst_c g_in_mst;            /*贷款产品*/
struct od_mst_c g_od_mst;            /*透支产品*/

struct dd_parm_c g_dd_parm;
struct td_parm_c g_td_parm;
struct ln_parm_c g_ln_parm;
struct in_parm_c g_in_parm;
struct od_parm_c g_od_parm;


struct S_pub_intst g_pub_intst;      /* 计息结构 */

struct MBFETRAN mbfetran;

char   acErrMsg[TRACELEN];			 /*跟踪用的变量*/
int	   g_reply_int;					 /*函数返回值*/
char RtCode[5];      /* 返回响应代码 */
char g_rcvpack[3000];
int Gl_file_first;
double g_acc_amt[2];
struct dic_data_c g_dic_data_a[100]; /*加速用*/
int g_dic_data_i;

/*一般程序的声明*/
#else

extern struct pub_tx g_pub_tx;              /*通用记帐结构*/
extern struct mdm_ac_rel_c g_mdm_ac_rel;    /*介质帐号对照表*/
extern struct dd_mst_c g_dd_mst;            /*活期产品*/
extern struct td_mst_c g_td_mst;            /*定期产品*/
extern struct ln_mst_c g_ln_mst;            /*贷款产品*/
extern struct od_mst_c g_od_mst;            /*透支产品*/
extern struct in_mst_c g_in_mst;            /*内部产品*/

extern struct dd_parm_c g_dd_parm;
extern struct td_parm_c g_td_parm;
extern struct ln_parm_c g_ln_parm;
extern struct od_parm_c g_od_parm;
extern struct in_parm_c g_in_parm;

struct com_eod_exec_c grz_com_eod_exec;/*全局,用于向子程序传递控制序列*/
extern struct S_pub_intst g_pub_intst;      /* 计息结构 */

extern struct MBFETRAN mbfetran;

extern char   acErrMsg[TRACELEN];			 /*跟踪用的变量*/
extern int	   g_reply_int;					 /*函数返回值*/
extern	char RtCode[5];      /* 返回响应代码 */
extern char g_rcvpack[3000];
extern int Gl_file_first;
extern double g_acc_amt[2];
extern struct dic_data_c g_dic_data_a[100]; /*加速用*/
extern int g_dic_data_i;

#endif

/*******定义跟踪日志宏定义 TRACE和WRITEMSG*********/ 


/* 程序跟踪LOG宏定义 */
#define TRACE \
        sprintf( acErrMsg,"File : [ %15s ] Line : [ %5d ]",__FILE__,__LINE__); \
                vtcp_log(acErrMsg); \
                memset( acErrMsg , 0x00 , sizeof ( acErrMsg ) );

#define WRITEMSG \
        vtcp_log( "File : [ %15s ] Line : [ %5d ]",__FILE__,__LINE__); \
        if ( strlen( acErrMsg ) ) \
        vtcp_log( ">>> [%s]", acErrMsg  ); \
        memset( acErrMsg , 0x00 , sizeof ( acErrMsg ) ); 
        

/*********************  函数声明  ************************/

#endif

#ifndef PARM_CWKH       /* 扣划参数序号，与lv_define.h hv_define.h中的序号是同性质的 */
#define PARM_CWKH 9
#endif
long int iGlobal_Pu_Val_Addr;
#define TEL_LENGTH 4	/* 柜员号长度 Add by hxc 091110 */
#define CDK_IND  "92" /*长短款登记簿序号 add by hxc 091110*/
/**各网点电票收到还款时的其他应收款专户***/
#ifndef PARM_ECDS_TR_PAY_AC
#define PARM_ECDS_TR_PAY_AC 54
#endif
/***各网点电票贴出的应解汇款专户2011****/
#ifndef PARM_ECDS_TC_AC
#define PARM_ECDS_TC_AC 53
#endif
