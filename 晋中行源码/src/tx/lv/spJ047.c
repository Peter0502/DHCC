/*****************************************************
* 文 件 名:  spJ047.c
* 功能描述： 贷记退汇业务录入
* 作    者:  xyy
* 完成日期： 2006-9-20 9:24
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
PAY_IN_AREA payin_th ,payin_lz;
int spJ047()
{	
	int gFindLzFlag=0;
	int iOrderno;
	char cRetcode[3];
	char cContent[61];
	char cPackid[9];
	char cPackday[9];
	
	int ret;
	struct lv_pkgreg_c o_lv_pkgreg;
	memset(&o_lv_pkgreg,0,sizeof(o_lv_pkgreg));
	
	memset(cPackid,0,sizeof(cPackid));
	memset(cPackday,0,sizeof(cPackday));
	memset(cRetcode,'\0',sizeof(cRetcode));
	memset(cContent,'\0',sizeof(cContent));
	memset(&payin_th,0,sizeof(payin_th));
	memset(&payin_lz,0,sizeof(payin_lz));
	
	pub_base_sysinit();	
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/****填充payin*********/
	g_reply_int = iLvTisToPayin(&payin_th);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调用函数iHvTistoNotpay出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d,交易金额[%s]",__FL__,payin_th.F_txamt);
	/***这里得到退汇原因和附言***/
	memcpy(cRetcode,payin_th.F_retcode,sizeof(cRetcode)-1);
	memcpy(cContent,payin_th.F_content,sizeof(cContent)-1);
	vtcp_log("[%s][%d]payin_lz.F_crdb[0]=[%c]\n",__FILE__,__LINE__,payin_lz.F_crdb[0]);
	vtcp_log("[%s][%d]pkgno=[%s],txnum=[%s]\n",__FL__,payin_lz.F_pkgno,payin_lz.F_txnum);
	
	g_reply_int = iLvPayinCheck0(&payin_th);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"调用函数iLvPayinCheck0出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d,交易金额[%s]",__FL__,payin_th.F_txamt);
	memcpy(payin_lz.F_wtday,payin_th.F_owtday,sizeof(payin_lz.F_wtday));
	memcpy(payin_lz.F_orbrno,payin_th.F_acbrno,sizeof(payin_lz.F_orbrno));
	memcpy(payin_lz.F_orderno,payin_th.F_oorderno,sizeof(payin_lz.F_orderno));
	memcpy(payin_lz.F_opcd,OPCD_RECEIVE,sizeof(payin_lz.F_opcd));
	payin_lz.F_lw_ind[0] = '2';
	vtcp_log("%s,%d,交易金额[%s]",__FL__,payin_th.F_txamt);
	vtcp_log("%s,%d,T_stat=[%c]begstat=[%c]",__FILE__,__LINE__,payin_lz.T_stat[0],payin_lz.T_begstat[0]);
	gFindLzFlag=iLvFetchRecToPayin(&payin_lz);
	if(gFindLzFlag)
	{
		/***未在原接收表中找到贷记来账业务****/
		vtcp_log("%s,%d,未找到来账记录n",__FILE__,__LINE__);
		sprintf( acErrMsg,"未找到来账记录n,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d,T_stat=[%c]begstat=[%c]",__FILE__,__LINE__,payin_lz.T_stat[0],payin_lz.T_begstat[0]);
	memcpy(cPackid  ,payin_lz.T_packid,sizeof(cPackid)-1);
	memcpy(cPackday ,payin_lz.T_packday,sizeof(cPackday)-1);
	if(payin_lz.T_stat[0]!=payin_lz.T_begstat[0])
	{
		vtcp_log("%s,%d,原交易状态不允许做退汇T_stat=[%c]begstat=[%c]",__FILE__,__LINE__,payin_lz.T_stat[0],payin_lz.T_begstat[0]);
		sprintf( acErrMsg,"原交易状态不允许做退汇,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P030" );
		WRITEMSG
		goto ErrExit;
	}
	iLvPayinAutoInfo(&payin_lz);

	
	vtcp_log("%s,%d,交易金额[%s]",__FL__,payin_th.F_txamt);
	iLvResetPayin(&payin_lz);
	vtcp_log("%s,%d,交易金额[%s]",__FL__,payin_th.F_txamt);
	g_reply_int = iLvPayinCheck10(&payin_lz,&payin_th);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,调用iLvPayinCheck10错误",__FILE__,__LINE__);
		sprintf( acErrMsg,"调用iLvPayinCheck10错误,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	iLvCreateTh(&payin_lz,&payin_th);
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0)
	{
		  vtcp_log("%s,%d,生成交易序号错误",__FILE__,__LINE__);
		  sprintf( acErrMsg,"生成交易序号错误,[%s][%d]", __FILE__,__LINE__);
		  strcpy( g_pub_tx.reply, "P019" );
		  WRITEMSG
		  goto ErrExit;
	}
	apitoa(iOrderno,sizeof(payin_th.F_orderno),payin_th.F_orderno);
	memcpy(payin_th.F_retcode,cRetcode,sizeof(payin_th.F_retcode));
	memcpy(payin_th.F_content,cContent,sizeof(payin_th.F_content));
	/***根据输入内容生成一条新的纪录****/
	g_reply_int = iLvNewRecFromPayin(&payin_th);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,调用iLvNewRecFromPayin函数错误",__FILE__,__LINE__);
		sprintf( acErrMsg,"调用iLvNewRecFromPayin函数错误,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	
	/*****add by liuyue   20061218更新原来帐的lv_o_sts字段，解决退汇时候 取消 要还原原来来帐状态的 问题**/
	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"orderno=%.8s and wt_date='%.8s' and or_br_no='%.12s' ",
			payin_th.T_oorderno,payin_th.T_owtday,payin_th.T_o_or_br_no);
	if(ret)
	{
		vtcp_log("[%s][%d]数据库操作错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	ret = Lv_pkgreg_Fet_Upd(&o_lv_pkgreg,g_pub_tx.reply);
	if(ret && ret !=100)
	{
		vtcp_log("[%s][%d]数据库操作错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	if(ret==100)
	{
		vtcp_log("[%s][%d]锁定元来帐 错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","锁定原来帐记录错误!");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]xier   lv_o_sts=[%c],lv_sts==[%c]\n",__FILE__,__LINE__,o_lv_pkgreg.lv_o_sts[0],o_lv_pkgreg.lv_sts[0]);
	o_lv_pkgreg.lv_o_sts[0] = o_lv_pkgreg.lv_sts[0];
	vtcp_log("[%s][%d]xier   lv_o_sts=[%c],lv_sts==[%c]\n",__FILE__,__LINE__,o_lv_pkgreg.lv_o_sts[0],o_lv_pkgreg.lv_sts[0]);
	ret = Lv_pkgreg_Upd_Upd(o_lv_pkgreg,g_pub_tx.reply);
	if(ret)
	{
		vtcp_log("[%s][%d]更新数据库 错误!\n",__FILE__,__LINE__);
		set_zd_data("0130","更新原来帐状态错误!\n");
		goto ErrExit;
	}
	
	Lv_pkgreg_Clo_Upd();

	vtcp_log("[%s][%d]更新原来帐交易状态 成功!\n",__FILE__,__LINE__);

	/*****add end by liuyue 20061218****/
	
	memcpy(payin_th.T_o_packid,cPackid,sizeof(cPackid)-1);
	memcpy(payin_th.T_o_pack_date,cPackday,sizeof(cPackday)-1);
	/**这里要添加sendco***/
  iGet_full_hv_no(payin_th.T_orbrno,payin_th.T_sendco,payin_th.T_payqsactno);
  iGet_full_hv_no(payin_th.T_acbrno,payin_th.T_receco,payin_th.T_cashqsactno);
	g_reply_int = iLvUpdRecFromPayin(&payin_th,1);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,插入贷记退汇表失败",__FILE__,__LINE__);
		sprintf( acErrMsg,"插入贷记退汇表失败,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
  g_reply_int = ilv10_PreTos();
  if(g_reply_int)
    {
        sprintf(acErrMsg,"返回值时出错!");
        strncpy(g_pub_tx.reply,"P079",4);
        WRITEMSG
        goto ErrExit;
    }
    g_reply_int=pub_ins_trace_log();
    if(g_reply_int)
    {
        sprintf(acErrMsg,"登记交易流水出错!");
        strncpy(g_pub_tx.reply,"S023",4);
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
ilv10_PreTos()
{
	int iRc;
	/****构造输出内容payout2****/
	iLvSetPayout1(&payin_th);
	return(0);
}
