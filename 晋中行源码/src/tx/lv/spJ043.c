/*****************************************************
* 文 件 名:  spJ043.c
* 功能描述： 定期贷记业务录入
* 作    者:  xyy
* 完成日期： 2006-9-4 17:02
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
static PAY_IN_AREA payin;
int spJ043()
{
	int iRc = 0;
	int ret = 0;
	struct lv_pkgreg_c lv_pkgreg;
	struct lv_pkgreg_c o_lv_pkgreg;
	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	memset(&o_lv_pkgreg,0,sizeof(o_lv_pkgreg));
	vtcp_log("[%s][%d] spJ043开始喽\n",__FILE__,__LINE__);

	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/****填充payin****/
	g_reply_int = iLvTisToPayin(&payin);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调用函数iHvTistoNotpay出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	/****不需要检查的要素直接拷贝****/
	memcpy(payin.T_feeflag,payin.F_feeflag,sizeof(payin.T_feeflag));
	memcpy(payin.T_feeamt,payin.F_feeamt,sizeof(payin.T_feeamt));
	memcpy(payin.T_vocnum,payin.F_vocnum,sizeof(payin.T_vocnum));
	memcpy(payin.T_voctype,payin.F_voctype,sizeof(payin.T_voctype));

	/****基本检查****/
	g_reply_int = iLvPayinCheck0(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"基本检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/****要素检查****/
	g_reply_int = iLvPayinCheck4(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"要素检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/****行号检查****/
	g_reply_int = iLvPayinCheck1(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"行号检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	int iOrderno;
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0){
		sprintf( acErrMsg,"得到支付交易序号出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	
	}
	apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);

	/***根据输入内容生成一条新的纪录****/
	g_reply_int = iLvNewRecFromPayin(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"生成一条新的纪录出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/*****add by liuyue   20061218更新原来帐的lv_o_sts字段，解决退汇时候 取消 要还原原来来帐状态的 问题
	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"orderno=%.8s and wt_date='%.8s' and or_br_no='%.12s' ",
			payin.T_oorderno,payin.T_owtday,payin.T_o_or_br_no);
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

	 不应该有这段 去掉了   add end by liuyue 20061218****/
	/***插入新记录****/
	g_reply_int = iLvUpdRecFromPayin(&payin,1);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"插入新记录,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
  /* 记流水日志 */
  g_reply_int = pub_ins_trace_log();
  if ( g_reply_int )
  {
          sprintf( acErrMsg, "记流水日志错" );
          strncpy(g_pub_tx.reply,"S023",4);
          WRITEMSG
          goto ErrExit;
  }
	/*****写交易明细表 ******/
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
*   DESCRIPTION:	   为返回包赋值											  *
******************************************************************************/
ilv10_PreTos()
{
	int iRc;
	/****构造输出内容payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
