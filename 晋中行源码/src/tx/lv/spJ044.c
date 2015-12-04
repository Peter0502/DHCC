/*****************************************************
* �� �� ��:  spJ044.c
* ���������� ���ڴ���ҵ�񸴺�
* ��    ��:  xyy
* ������ڣ� 2006-9-4 19:53
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
static PAY_IN_AREA payin;
int spJ044()
{
	int iRc = 0;
	struct lv_pkgreg_c lv_pkgreg;
	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	vtcp_log("[%s][%d] spJ044��ʼ�\n",__FILE__,__LINE__);

	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}
	/****���payin*********/
	g_reply_int = iLvTisToPayin(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����iLvTisToPayin����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] F_txamt=[%s]",__FL__,payin.F_txamt);
	vtcp_log("[%s][%d] T_txamt=[%s]",__FL__,payin.T_txamt);
	/****��ȡ��������****/
	g_reply_int = iLvFetchRecToPayin(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����iLvFetchRecToPayin����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] F_txamt=[%s]",__FL__,payin.F_txamt);
	vtcp_log("[%s][%d] T_txamt=[%s]",__FL__,payin.T_txamt);
	/****�������****/
	g_reply_int = iLvPayinCheck0(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����������,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] F_txamt=[%s]",__FL__,payin.F_txamt);
	vtcp_log("[%s][%d] T_txamt=[%s]",__FL__,payin.T_txamt);
	/****Ҫ�ؼ��****/
	g_reply_int = iLvPayinCheck4(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"Ҫ�ؼ�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] F_txamt=[%s]",__FL__,payin.F_txamt);
	vtcp_log("[%s][%d] T_txamt=[%s]",__FL__,payin.T_txamt);
	/****�кż��****/
	g_reply_int = iLvPayinCheck1(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�кż�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/****���������ѽ��****/
	g_reply_int = iLvCountFeeamt(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,���������Ѵ���,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"���������Ѵ���,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] F_txamt=[%s]",__FL__,payin.F_txamt);
	vtcp_log("[%s][%d] T_txamt=[%s]",__FL__,payin.T_txamt);
	/***һ���Լ��****/
	g_reply_int = iLvPayinCheck6(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"һ���Լ�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/******��������*****/
	payin.T_stat[0]=STAT_WZCHECK2; /**����״̬**/
	g_reply_int=iLvUpdRecFromPayin(&payin,0);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,���µǼǲ�ʧ��,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"���µǼǲ�ʧ��,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/* ����ˮ��־ */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		strncpy(g_pub_tx.reply,"S023",4);
		WRITEMSG
		goto ErrExit;
	}
	/*****д������ϸ�� ******/
	iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
/******************************************************************************
*  																			  *
*	FUNCTION:		   lv10_PreTos()										  *
*   DESCRIPTION:	   Ϊ���ذ���ֵ											  *
******************************************************************************/
ilv10_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
