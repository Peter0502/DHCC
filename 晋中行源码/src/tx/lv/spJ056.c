/*****************************************************
* 文 件 名:  spJ056.c
* 功能描述： 定期借记业务回执复核
* 作    者:  xyy
* 完成日期： 2006-9-6 10:13
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
#include "lv_lbctl_c.h"
static PAY_IN_AREA payin;
int spJ056()
{
	int iRc = 0;
	int ret = 0;
	long iPackday=0;
	char cPackid[9];
	char cPayqsactno[13];
	LV_FD125_AREA fd125;
	struct lv_pkgreg_c lv_pkgreg;
	struct lv_lbctl_c  lv_lbctl;

	memset(&fd125, 0 , sizeof(fd125));
	memset(cPackid		,0,sizeof(cPackid));
	memset(cPayqsactno,0,sizeof(cPayqsactno));
	memset(&payin			,0,sizeof(payin));
	memset(&lv_pkgreg	,0,sizeof(lv_pkgreg));
	memset(&lv_lbctl	,0,sizeof(lv_lbctl));

	vtcp_log("[%s][%d] spJ056开始喽\n",__FILE__,__LINE__);

	/**********************
	 * 1.初始化全局变量   *
	 **********************/
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}


	/*********************
	 * 2.获取8583域      *
	 *********************/
	ret = iLvTisToPayin(&payin);
	if( ret )
	{
		sprintf( acErrMsg,"调用函数iHvTistoNotpay出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	get_fd_data("1250",(char*)&fd125);
	vtcp_log("[%s],[%d] 125",__FILE__,__LINE__);
	vtcp_log("[%s]",(char*)&fd125);


	/******************
	 * 3. 交易检查    *
	 ******************/
	ret = iLvFetchRecToPayin(&payin);
	if( ret )
	{
		sprintf( acErrMsg,"调用函数iLvFetchRecToPayin出错,[%s][%d]", __FILE__,__LINE__);
		WRITEMSG
		goto ErrExit;
	}
	/****把一些不需要检查的字段直接拷贝****/
	memcpy(payin.F_ywtype,payin.T_ywtype,sizeof(payin.F_ywtype));
	memcpy(payin.T_feeflag,payin.F_feeflag,sizeof(payin.T_feeflag));
	memcpy(payin.F_packid,payin.T_packid,sizeof(payin.T_packid));
	memcpy(payin.F_packday,payin.T_packday,sizeof(payin.T_packday));

	/****基本检查****/
	ret = iLvPayinCheck0(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"基本检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/****要素检查****/
	ret = iLvPayinCheck4(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"要素检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/****行号检查****/
	ret = iLvPayinCheck1(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"行号检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/***Add by SSH,2006.1.10,检查是否已经拼包****/
	iPackday	= apatoi(fd125.pack_date,8);
	memcpy(cPackid		,fd125.packid			,sizeof(fd125.packid));
	memcpy(cPayqsactno,fd125.pay_qs_no	,sizeof(fd125.pay_qs_no));
  vtcp_log("[%s][%d]iPackday==[%d]",__FILE__,__LINE__,iPackday);
	vtcp_log("[%s][%d]cPackid==[%s]",__FILE__,__LINE__,cPackid);
	vtcp_log("[%s][%d]cPayqsactno==[%s]",__FILE__,__LINE__,cPayqsactno);
	ret = Lv_lbctl_Sel(g_pub_tx.reply,&lv_lbctl,"pack_date='%d' and packid='%s' and pay_qs_no='%s'",iPackday,cPackid,cPayqsactno);
	if(ret)
	{
		vtcp_log("%s,%d,数据库没有此记录,sqlcode=%d!\n",__FILE__,__LINE__,ret);
		sprintf( acErrMsg,"数据库没有此记录,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	if(lv_lbctl.o_packid[0]!='\0')
	{
		vtcp_log("%s,%d,此交易已经拼包,不能复核 ,sqlcode=%d!\n",__FILE__,__LINE__,ret);
		sprintf( acErrMsg,"此交易已经拼包,不能复核 ,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/****付款帐号检查****/
	if(payin.T_stat[0]!=STAT_WZLR)
	{
		vtcp_log("%s,%d,交易不为录入状态,不能复核 ,sqlcode=%d!\n",__FILE__,__LINE__,ret);
		sprintf( acErrMsg,"交易不为录入状态,不能复核 ,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	/***Add by SSH,2006.1.10,要素一致型检查****/
	ret = iLvPayinCheck6(&payin);
	if(ret)
	{
		vtcp_log("%s,%d,要素一致型检查,sqlcode=%d!\n",__FILE__,__LINE__,ret);
		sprintf( acErrMsg,"要素一致型检查,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	payin.T_stat[0]=STAT_WZCHECK2;
	ret = iLvUpdRecFromPayin(&payin,0);
	if(ret)
	{
			vtcp_log("%s,%d,更新登记簿失败,sqlcode=%d!\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg,"更新登记簿失败,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
	}
	iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
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

	if(memcmp(payin.T_rcpstat,"00",2)==0)
	{
		goto OkExit1;
	}
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"回执不成功[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
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
