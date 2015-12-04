/***************************************************************************/
/* �ļ�����: spJ046.c                                                      */
/* ��������: ʵʱ����ҵ�񸴺�                                              */
/* ��������: 2006-9-22 21:25                                               */
/* ����:     xyy                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "lv_define.h"
#include "hv_define.h"

static PAY_IN_AREA payin;
int spJ046()
{
	int iRc = 0;
	memset(&payin,'\0',sizeof(payin));
	vtcp_log("[%s][%d]ʵʱ���Ǹ���ҵ��ʼ�!!",__FILE__,__LINE__);	


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
		sprintf( acErrMsg,"���ú���iHvTistoNotpay����,[%s][%d]", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/* �ӱ���ȡ���� */
	g_reply_int = iLvFetchRecToPayin(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,����iLvFetchRecToPayin����!\n", __FILE__,__LINE__);
		WRITEMSG
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	memcpy(payin.T_feeflag,payin.F_feeflag,	sizeof(payin.T_feeflag));
	memcpy(payin.F_packday,payin.A_tbsdy,sizeof(payin.F_packday));
	/****�������****/
	g_reply_int = iLvPayinCheck0(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,����iLvPayinCheck0����!\n", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/****Ҫ�ؼ��****/
	g_reply_int = iLvPayinCheck4(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,����iLvPayinCheck4����!\n", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/****�кż��****/
	g_reply_int = iLvPayinCheck1(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,����iLvPayinCheck1����!\n", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	payin.F_feeflag[0]=payin.T_feeflag[0];
	memcpy(payin.F_feeamt,payin.T_feeamt,sizeof(payin.T_feeamt));
	g_reply_int = iLvPayinCheck6(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,����iLvPayinCheck6����!\n", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/* ���������� */
	get_fd_data(DC_TEL,payin.T_checkno);
	get_fd_data(DC_TRACE_NO,payin.T_checkwssrno);

	get_fd_data(DC_DATE,payin.T_jzday);
	payin.T_stat[0]=STAT_WZCHECK2;

	vtcp_log("[%s][%d]begin iLvUpdRecFromPayin \n",__FILE__,__LINE__);
	g_reply_int = iLvUpdRecFromPayin(&payin,0);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,���µǼǲ�ʧ��!\n", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/*****д������ϸ�� ******/
	iLv14_PreTos();
        if(iRc)
        {
                sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
                strncpy(g_pub_tx.reply,"P079",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
        }


    iRc=pub_ins_trace_log();
    if(iRc)
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        strncpy(g_pub_tx.reply,"S023",4);
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int iLv14_PreTos()
{
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
