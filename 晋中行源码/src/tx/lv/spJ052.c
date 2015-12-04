/*****************************************************
* 文 件 名:  spJ052.c
* 功能描述： 实时借记业务录入
* 作    者:  xyy
* 完成日期： 2006-9-2 14:06 
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
#include "lv_wbctl_c.h"
static	PAY_IN_AREA payin;
extern char * pcLvDesPswdCode(char *code);

int spJ052()
{
	int iRc = 0;
	struct lv_wbctl_c lv_wbctl;
	struct lv_pkgreg_c lv_pkgreg;
	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	memset(&lv_wbctl,0,sizeof(lv_wbctl));
	memset(&lvca, 0 , sizeof(lvca));
	vtcp_log("[%s][%d] spJ052开始喽\n",__FILE__,__LINE__);

	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}
	/****填充payin*********/
	g_reply_int = iLvTisToPayin(&payin);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调用函数iHvTistoNotpay出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
  vtcp_log("[%s][%d] payin.F_lw_ind = [%c]",__FL__,payin.F_lw_ind[0]);
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
	/* 手续费检查 */
	iLvCountFeeamt(&payin);

	/****收款帐号检查****/
	g_reply_int=iLvPayinCheck3(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"收款帐号检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] payactno[%s]\n",__FILE__,__LINE__,payin.F_payactno);
	vtcp_log("[%s][%d] cashactno[%s]\n",__FILE__,__LINE__,payin.F_cashactno);
	int iOrderno;
	/*******
	int iPackid;
	iPackid = iLvGetPackid();
	if(iPackid<=0){
		sprintf( acErrMsg,"得到支付交易序号出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	apitoa(iPackid,sizeof(payin.F_packid),payin.F_packid);
	memcpy(payin.F_packday,payin.F_wtday,sizeof(payin.F_wtday));
	*******************/
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
	/***插入新记录****/
	g_reply_int = iLvUpdRecFromPayin(&payin,1);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"插入新记录,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int = iSetSSJJ_Addit();
	if(g_reply_int) 
	{
		vtcp_log("%s,%d,更新实时借记附加域错误,[%d]",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"更新实时借记附加域错误,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/*****包检查****/
	vtcp_log("[%s][%d]F_packid=[%s]\n",__FILE__,__LINE__,payin.F_packid);
	payin.F_packend[0]='1';
	iRc=iLvPayinCheck8(&payin,&lv_wbctl,NULL);
	if(iRc!=PCHK_UPDATE && iRc!=PCHK_NEW)
	{
		vtcp_log("[%s][%d]======0新包,1已存在,2已更新===iRc=[%d]\n",__FILE__,__LINE__,iRc);
		sprintf( acErrMsg,"无法拼到指定包,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	memcpy(payin.T_packday,payin.A_tbsdy,sizeof(payin.T_packday));
	memcpy(payin.F_packday,payin.A_tbsdy,sizeof(payin.F_packday));
	vtcp_log("[%s][%d] pkgid====[%s]\n",__FILE__,__LINE__,payin.F_packid);
	vtcp_log("[%s][%d] packday==[%s]\n",__FILE__,__LINE__,payin.T_packday);
	g_reply_int = iLvUpdRecFromPayin(&payin,0);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"iLvUpdRecFromPayin错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/** 包检查结束 **/

	/******************************* 处理手续费
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
	******************************************/
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

int iSetSSJJ_Addit()
{
	TD_ADD_AREA wd_tctd;
	FILE *fp;
	char cFilename[128];
	char *ptr=(char *)&wd_tctd;
	int i;
	int iRc=0;
	/****Add by SSH,2006.1.25,只有通存通兑的在此设置附加域，其他在公共函数中*/
	if(memcmp(payin.F_txnum,TXNUM_TD,sizeof(payin.F_txnum))!=0 && \
		memcmp(payin.F_txnum,TXNUM_DGTD,sizeof(payin.F_txnum))!=0)
	{
		return(0);
	}
	/****NEWTCTD:处理通兑业务附加域****/
	memset(&wd_tctd,0,sizeof(wd_tctd));
	memset(cFilename,0,sizeof(cFilename));
	pub_base_AllDwnFilName(cFilename);	
	vtcp_log("[%s][%d],附加域文件名cFilename[%s]\n",__FL__,cFilename);
	fp=fopen(cFilename,"rb");
	if(fp==NULL){
		vtcp_log("[%s][%d],open add_area file[%s] failed!\n",__FL__,cFilename);	
		sprintf(acErrMsg,"open add_area file[%s] failed!\n",cFilename);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data(DC_GET_MSG,"未发现附加域数据");
		return(-1);
	}
	fread((char *)&wd_tctd,sizeof(wd_tctd),1,fp);	
	fclose(fp);
	vtcp_log("[%s][%d],附加域内容:\n[%s]\n",__FILE__,__LINE__,&wd_tctd);
	if(memcmp(wd_tctd.pswdmethod,"01",2)==0){
		iRc=nBepsEncrypt(&payin,&wd_tctd);
		if(iRc){
			vtcp_log("%s,%d,加密失败!******\n",__FILE__,__LINE__);
			sprintf(acErrMsg,"加密失败请重新加密!iRc[%d]",iRc);	
			WRITEMSG
			return(-1);
		}
		vtcp_log("%s,%d,加密成功!******\n",__FILE__,__LINE__);
	}	
	iRc=iLvSetAddit(&payin,"000",&wd_tctd,strlen((char *)&wd_tctd));
	vtcp_log("%s,%d,payin.T_addid=%.8s\n",__FL__,payin.T_addid);
	if(iRc)
	{
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	return(0);
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
