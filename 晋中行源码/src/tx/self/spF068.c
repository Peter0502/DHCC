/******************************************************************
ģ������: ��д�ŵ�����
��������: �ṩ���⿨ҵ��Ĵŵ�д�빦��
   �ŵ����ݲμ�<<������ҵ���С�һ��ͨ��������������ʽ>>
��������: 
����ԭ��: spF068()
�������:����
�������:��2,3�ŵ���Ϣ
ʹ��˵��:
�� д ��: LiuHuafeng 2007-1-23 15:02
������Ϣ:
insert into tx_flow_def values('1805','0','F068','#$');
insert into tx_def values('1805','������д��','10000000000000000000000000000010000010010000000000000000000000000000000001110000000000000000000000000000000000000000000000000000','1','6','2');
*******************************************************************/
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "card.h"
#include "public.h"
#include "errlog.h"

static struct TOS4819_GROUP{
	char msr2[37] ;/* 2��  */
	char msr3[104];/* 3��  */
	char cardno[19];
}tos;

static char cCardno[CARD_NO_LEN+1];
extern void 	vPrepareMsr(char * cardno,char * msr1,char * msr2);
static char cMsr2[CARD_MSR2_LEN+1]; /* 40-3+\0*/
static char cMsr3[CARD_MSR3_LEN+1];/* 107-3+\0 */
/*************************************************************************
*                                                                        *
*   ������:         dh4819_Fetch()                                       *
*   ����˵��:       1.��ʼ��                                             *
*                   2.У����������ȷ��                                 *
*                   3.��������������ݿ�                                 *
*                   4.����ˮ                                             *
*                   5.��ǰ̨���ذ�                                       *
*                                                                        *
*************************************************************************/
int spF068()
{
	char cCheckflag;
	char cCardserial[CARD_NO_LEN+1];
	vtcp_log("AP MSG: Begin of dh4819_ChgPwd()\n");
	/*****************/
	/*     ��ʼ��    */
	/*****************/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}
	/****************************/
	/** �ȳ�ʼ�����б����ͽṹ **/
	/****************************/
	memset(&cCardno, 0 , sizeof(cCardno));
	/*****************/
	/*ѡ���Ӧ�ĺ��� */
	/*****************/
	memset(cCardserial, 0 , sizeof(cCardserial));
	/* ������в�д�������ɺ�̨����У��λ��Ȼ����ǰ̨�ж� */
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
	/*    ׼����ˮ   */
	/*****************/
	g_reply_int=pub_ins_trace_log();
	if( g_reply_int )
	{
		vtcp_log("%s,%d �Ǽǽ�����ˮ���� ret=[%d]",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����µĿ��ųɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�����µĿ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
