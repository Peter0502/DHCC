/******************************************************************
模块名称: 卡写磁道交易
函数功能: 提供开封卡业务的磁道写入功能
   磁道内容参见<<长治商业银行“一卡通”银练卡磁条格式>>
程序名称: 
函数原型: spF068()
输入参数:卡号
输出参数:卡2,3磁道信息
使用说明:
编 写 者: LiuHuafeng 2007-1-23 15:02
配置信息:
insert into tx_flow_def values('1805','0','F068','#$');
insert into tx_def values('1805','卡单独写磁','10000000000000000000000000000010000010010000000000000000000000000000000001110000000000000000000000000000000000000000000000000000','1','6','2');
*******************************************************************/
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "card.h"
#include "public.h"
#include "errlog.h"

static struct TOS4819_GROUP{
	char msr2[37] ;/* 2磁  */
	char msr3[104];/* 3磁  */
	char cardno[19];
}tos;

static char cCardno[CARD_NO_LEN+1];
extern void 	vPrepareMsr(char * cardno,char * msr1,char * msr2);
static char cMsr2[CARD_MSR2_LEN+1]; /* 40-3+\0*/
static char cMsr3[CARD_MSR3_LEN+1];/* 107-3+\0 */
/*************************************************************************
*                                                                        *
*   函数名:         dh4819_Fetch()                                       *
*   函数说明:       1.初始化                                             *
*                   2.校验旧密码的正确性                                 *
*                   3.把新密码记入数据库                                 *
*                   4.记流水                                             *
*                   5.给前台返回包                                       *
*                                                                        *
*************************************************************************/
int spF068()
{
	char cCheckflag;
	char cCardserial[CARD_NO_LEN+1];
	vtcp_log("AP MSG: Begin of dh4819_ChgPwd()\n");
	/*****************/
	/*     初始化    */
	/*****************/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}
	/****************************/
	/** 先初始化所有变量和结构 **/
	/****************************/
	memset(&cCardno, 0 , sizeof(cCardno));
	/*****************/
	/*选入对应的函数 */
	/*****************/
	memset(cCardserial, 0 , sizeof(cCardserial));
	/* 如果集中补写磁条则由后台产生校验位，然后由前台判断 */
	get_zd_data("0370",cCardserial);
	iCheckCardno(cCardserial);
	pub_card_par_1212(1,cCardserial,cCardno);
	vPrepareMsr(cCardno,cMsr2,cMsr3);
	errLog(LOG_INFO, 0, RPT_TO_TTY, cMsr2, sizeof(cMsr2), "msr2 is");
	vtcp_log("[%s][%d] cardno==[%s]\n",__FILE__,__LINE__,cCardno);
	errLog(LOG_INFO, 0, RPT_TO_TTY, cMsr3, sizeof(cMsr3), "msr3 is");
	set_zd_data("0750",cMsr2);
	set_zd_data("0760",cMsr3);
	/*****************/
	/*    准备流水   */
	/*****************/
	g_reply_int=pub_ins_trace_log();
	if( g_reply_int )
	{
		vtcp_log("%s,%d 登记交易流水错误 ret=[%d]",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"产生新的卡号成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"产生新的卡号失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
