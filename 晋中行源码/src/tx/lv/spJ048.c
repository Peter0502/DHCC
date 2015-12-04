/*****************************************************
* 文 件 名:  spJ048.c
* 功能描述： 贷记退汇业务复核
* 作    者:  xyy
* 完成日期： 2006-9-21 17:25
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
extern char* pcLvGetDay();
int spJ048()
{	
	int gFindLzFlag=0;
	PAY_IN_AREA payin,payin_lz;
	char cAc_no[20];
	memset(cAc_no,0,sizeof(cAc_no));
	memset(&payin      ,0,sizeof(payin));
	memset(&payin_lz   ,0,sizeof(payin_lz));


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
		sprintf( acErrMsg,"调用函数iLvTisToPayin出错,[%s][%d]", __FL__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	if(memcmp(payin.F_orbrno,payin.F_acbrno,sizeof(payin.F_acbrno))==0)
	{
		sprintf( acErrMsg,"原发起行不能与原接收行一致,[%s][%d]", __FL__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int= iLvFetchRecToPayin(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"调用函数iLvFetchRecToPayin出错,[%s][%d]", __FL__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]T_ywtype=[%s]\n",__FILE__,__LINE__,payin.T_ywtype);
	/****直接拷贝一些不需要检查的字段****/
	memcpy(payin.T_feeflag,payin.F_feeflag,sizeof(payin.T_feeflag));
	memcpy(payin.F_cashno,payin.T_cashno,sizeof(payin.F_cashno));
	memcpy(payin.F_otxnum,payin.T_otxnum,sizeof(payin.F_otxnum));
	memcpy(payin.F_packday,payin.T_packday,sizeof(payin.F_packday));
	memcpy(payin.F_packid,payin.T_packid,sizeof(payin.F_packid));
	memcpy(payin.F_ywtype,payin.T_ywtype,sizeof(payin.F_ywtype));
	memcpy(payin.F_oorderno,payin.T_oorderno,sizeof(payin.F_oorderno));
	g_reply_int = iLvPayinCheck0(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"调用函数iLvPayinCheck0出错,[%s][%d]", __FL__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	/****要素检查****/
	g_reply_int = iLvPayinCheck4(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"调用函数iLvPayinCheck4出错,[%s][%d]", __FL__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	/****行号检查****/
	g_reply_int = iLvPayinCheck1(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"调用函数iLvPayinCheck1出错,[%s][%d]", __FL__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int = iLvPayinCheck6(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"调用函数iLvPayinCheck6出错,[%s][%d]", __FL__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin.T_owtday=[%s]",__FILE__,__LINE__,payin.T_owtday);
	memcpy(payin_lz.F_wtday,payin.F_owtday,	sizeof(payin_lz.F_wtday));
	
	memcpy(payin_lz.F_orbrno,payin.F_acbrno,sizeof(payin_lz.F_orbrno));
	memcpy(payin_lz.F_orderno,payin.F_oorderno,sizeof(payin_lz.F_orderno));
	memcpy(payin_lz.F_opcd,OPCD_RECEIVE,sizeof(payin_lz.F_opcd));
	payin_lz.F_lw_ind[0]='2';
	gFindLzFlag=iLvFetchRecToPayin(&payin_lz);
	if(gFindLzFlag)
	{
		/***未在原接收表中找到贷记来账业务****/
		vtcp_log("%s,%d,未找到来账记录n",__FILE__,__LINE__);
		sprintf( acErrMsg,"未找到来账记录n,[%s][%d]", __FILE__,__LINE__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	iLvPayinAutoInfo(&payin_lz);
	iLvResetPayin(&payin_lz);

	/***正交易，原来账状态应该是退回等待****/
	if(payin_lz.T_stat[0]!=payin_lz.T_begstat[0])
	{
		vtcp_log("%s,%d,不是退会等待状态T_stat[0]=[%c]T_begstat[0]=[%c]",__FILE__,__LINE__,payin_lz.T_stat[0],payin_lz.T_begstat[0]);
		sprintf( acErrMsg,"不是退会等待状态[%s][%d]", __FILE__,__LINE__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	get_fd_data(DC_TEL,payin.T_checkno);
	memcpy(payin.T_jzday,pcLvGetDay(),sizeof(payin.T_jzday));
	/***这里是走帐的地方***/
	g_reply_int = iLvPrepareTh_ac_no(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,设置退汇科目出错",__FILE__,__LINE__);
		sprintf( acErrMsg,"设置退汇科目出错,[%s][%d]", __FILE__,__LINE__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	payin.T_stat[0]=STAT_WZCHECK2;
	g_reply_int = iLvUpdRecFromPayin(&payin,0);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,更新登记簿失败",__FILE__,__LINE__);
		sprintf( acErrMsg,"更新登记簿失败,[%s][%d]", __FILE__,__LINE__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	payin_lz.T_stat[0]=STAT_LZBACK_WAIT;
	g_reply_int = iLvUpdRecFromPayin(&payin_lz,0);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,更新原来账登记簿失败",__FILE__,__LINE__);
		sprintf( acErrMsg,"更新原来账登记簿失败,[%s][%d]", __FILE__,__LINE__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
    g_reply_int =pub_ins_trace_log();
    if(g_reply_int)
    {
        sprintf(acErrMsg,"登记交易流水出错!");
        set_zd_data("0130",acErrMsg);
        strncpy(g_pub_tx.reply,"S023",4);
        WRITEMSG
        goto ErrExit;
    }
	/** add by LiuHuafeng 20061006 设置小额挂账帐户 */
	g_reply_int=pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_LVGZ,cAc_no);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"得到本机构大额清算帐号错误!\n");
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		vtcp_log("%s,%d 得到本机构大额清算帐号错误\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"T042");
		goto ErrExit;
	}
	set_zd_data("0300",cAc_no);
	set_zd_data("0680","6");

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
