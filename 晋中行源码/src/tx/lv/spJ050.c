/*****************************************************
* 文 件 名:  spJ050.c
* 功能描述： 普通借记业务录入
* 作    者:  xyy
* 完成日期： 2006-9-4 9:24 
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
#include "lv_param_c.h"
static PAY_IN_AREA payin;
int spJ050()
{
	int iRc = 0;
	struct lv_pkgreg_c lv_pkgreg;
	struct lv_param_c  lv_param;
	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	memset(&lv_param,0,sizeof(lv_param));
	vtcp_log("[%s][%d] spJ050开始喽\n",__FILE__,__LINE__);

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
		strcpy( g_pub_tx.reply, "P311" );
		WRITEMSG
		goto ErrExit;
	}
		/* 手续费检查 */
	if(payin.F_feeflag[0]!='4')
	g_reply_int = iLvCountFeeamt(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,计算手续费错误,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"计算手续费错误,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	/****收款帐号检查****/
	g_reply_int=iLvPayinCheck3(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"收款帐号检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] payactno[%.32s]\n",__FILE__,__LINE__,payin.F_payactno);
	vtcp_log("[%s][%d] cashactno[%.32s]\n",__FILE__,__LINE__,payin.F_cashactno);
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
	vtcp_log("[%s][%d]===========wd_payin->F_txamt=[%.15s]\n",__FL__,payin.F_txamt);
	vtcp_log("[%s][%d]===========wd_payin->T_txamt=[%.15s]\n",__FL__,payin.T_txamt);
	vtcp_log("[%s][%d]===========wd_payin->T_stat=[%.1s]\n",__FL__,payin.T_stat);
	/***插入新记录****/
	g_reply_int = iLvUpdRecFromPayin(&payin,1);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"插入新记录,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	
	/******************** 处理手续费 
	memcpy(payin.T_jzday,payin.F_tx_date,sizeof(payin.T_jzday));
	g_reply_int = iLv_updAcmrAndAct(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,更新帐务失败,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"更新帐务失败,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	***************************/
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
	/****Add by SSH,2007.5.15,如果是支票业务发送到验印服务器****/
	/**影像添加了复核交易，发送改到复核交易里面发送
	if(memcmp(payin.F_txnum,TXNUM_ZPJL,5)==0||
		memcmp(payin.F_txnum,TXNUM_TYJL,5)==0){
	        ****取回执期限***
                memcpy(lv_param.par_code,"WDBRTDAY",8);
                g_reply_int = Lv_param_Sel(g_pub_tx.reply, &lv_param, "par_code= '%s'", lv_param.par_code);
                if(g_reply_int){
                        strcpy(payin.T_respday,"1");
                }else{
                        memcpy(payin.T_respday,lv_param.par_value,sizeof(payin.T_respday));
                }
		iRc=SendZpCifToYy(&payin);
		if(iRc){
			sprintf(acErrMsg,"%s,%d 发送到验印服务器失败!",__FL__);
			strncpy(g_pub_tx.reply,"LS88",4);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"无法发送报文到验印服务器");	
			goto ErrExit;
		}
		**如果成功直接更新状态为发送**
		payin.T_stat[0]=STAT_WZSEND;
		g_reply_int = iLvUpdRecFromPayin(&payin,0);
		if(g_reply_int)
		{
			sprintf( acErrMsg,"插入新记录,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
	}**/
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
	iLvSetPayout1(&payin);
	return(0);
}
