/*****************************************************
* 文 件 名:  spJ051.c
* 功能描述： 定期借记业务录入
* 作    者:  liuyue 
* 完成日期： 2006-9-20 09:00 
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
int spJ051()
{
	int iRc = 0;
	int ret = 0;
	int iOrderno = 0;
	struct lv_pkgreg_c lv_pkgreg;

	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));

	vtcp_log("[%s][%d] 开始定期借记的录入程序!\n",__FILE__,__LINE__);


	/********************
	 * 1.初始化全局变量 *
	 ********************/
	pub_base_sysinit();	
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/*********************
	 * 2.获取前台8583域  *
	 *********************/
	ret = iLvTisToPayin(&payin);
	if( ret )
	{
	sprintf( acErrMsg,"调用函数iHvTistoNotpay出错,[%s][%d]", __FILE__,__LINE__);
	strcpy( g_pub_tx.reply, "P015" );
	WRITEMSG
	goto ErrExit;
	}
	

	/***********************
	 * 3. 进行基本检查     *
	 ***********************/

	/*基本检查*/
	ret = iLvPayinCheck0(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"基本检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/*要素检查*/
	ret = iLvPayinCheck4(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"要素检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/*行号检查*/
	ret = iLvPayinCheck1(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"行号检查出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	/* 手续费检查 */

	if(payin.F_feeflag[0]!='4')
	ret = iLvCountFeeamt(&payin);
	if(ret)
	{
		vtcp_log("%s,%d,计算手续费错误,sqlcode=%d!\n",__FILE__,__LINE__,ret);
		sprintf( acErrMsg,"计算手续费错误,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/*生成新的交易序号*/
	
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0)
	{
		sprintf( acErrMsg,"得到支付交易序号出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);


	/*********************
	 * 4. 数据库操作     *
	 *********************/

	ret = iLvNewRecFromPayin(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"生成一条新的纪录出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	/*插入新记录*/
	ret = iLvUpdRecFromPayin(&payin,1);
	if(ret)
	{
		sprintf( acErrMsg,"插入新记录,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}


	/************************
	 * 5.准备日志 		*
	 ************************/

	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		strncpy(g_pub_tx.reply,"S023",4);
		WRITEMSG
		goto ErrExit;
	}

	/********************
	 * 6.准备输出数据   *
	 ********************/
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

/******************************
 * FUNCTIONS :ilv10_PreTos    *
 * PREPARE DATA FOR INTERFACE *
 ******************************/	
ilv10_PreTos()
{
	int iRc;
	/****构造输出内容payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
