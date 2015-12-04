/***************************************************************************/
/* 文件名称: spJ046.c                                                      */
/* 功能描述: 实时贷记业务复核                                              */
/* 创建日期: 2006-9-22 21:25                                               */
/* 作者:     xyy                                                           */
/* 说明:                                                                   */
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
	vtcp_log("[%s][%d]实时贷记复核业务开始喽!!",__FILE__,__LINE__);	


	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/****填充payin*********/
	g_reply_int = iLvTisToPayin(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"调用函数iHvTistoNotpay出错,[%s][%d]", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/* 从表中取数据 */
	g_reply_int = iLvFetchRecToPayin(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,调用iLvFetchRecToPayin错误!\n", __FILE__,__LINE__);
		WRITEMSG
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	memcpy(payin.T_feeflag,payin.F_feeflag,	sizeof(payin.T_feeflag));
	memcpy(payin.F_packday,payin.A_tbsdy,sizeof(payin.F_packday));
	/****基本检查****/
	g_reply_int = iLvPayinCheck0(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,调用iLvPayinCheck0错误!\n", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/****要素检查****/
	g_reply_int = iLvPayinCheck4(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,调用iLvPayinCheck4错误!\n", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/****行号检查****/
	g_reply_int = iLvPayinCheck1(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,调用iLvPayinCheck1错误!\n", __FILE__,__LINE__);
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
		vtcp_log("%s,%d,调用iLvPayinCheck6错误!\n", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/* 进行账务处理 */
	get_fd_data(DC_TEL,payin.T_checkno);
	get_fd_data(DC_TRACE_NO,payin.T_checkwssrno);

	get_fd_data(DC_DATE,payin.T_jzday);
	payin.T_stat[0]=STAT_WZCHECK2;

	vtcp_log("[%s][%d]begin iLvUpdRecFromPayin \n",__FILE__,__LINE__);
	g_reply_int = iLvUpdRecFromPayin(&payin,0);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,更新登记簿失败!\n", __FILE__,__LINE__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
	/*****写交易明细表 ******/
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
        sprintf(acErrMsg,"登记交易流水出错!");
        strncpy(g_pub_tx.reply,"S023",4);
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int iLv14_PreTos()
{
	/****构造输出内容payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
