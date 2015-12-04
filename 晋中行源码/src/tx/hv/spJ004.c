/***************************************************************
* 文 件 名:     spJ004.c
* 功能描述：	大额支付退汇复核
*
* 作    者:   LiuHuafeng
* 完成日期：  2006-8-9 9:28
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "find_debug.h"

spJ004()
{
	int ret = 0;
	char cAc_no[25];
	char cKinbr[BRNO_LEN+1];
	HV_PAYIN_AREA hv_payin,hv_lz_payin;
	char cWssrno[7];
	char cTxday[9];
	memset(cTxday, 0 , sizeof(cTxday));
	
	memset(cAc_no, 0 , sizeof(cAc_no));
	memset(cKinbr, 0 , sizeof(cKinbr));
	get_zd_data(DC_TX_BR_NO,cKinbr);
	get_zd_data(DC_DATE,cTxday);
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_lz_payin, 0 , sizeof(hv_lz_payin));
	memset(&hv_payin, 0 , sizeof(hv_payin));
	/***得到大额支付业务8583数据***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	ret=FetchRecToPayin(&hv_payin);
	if(ret)
	{
		WRITEMSG
		goto ErrExit;
	}
	ret=iHvCheckInput(&hv_payin,acErrMsg);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	iHvResetPayin(&hv_payin);
	/* 检查非法字符 */
	/**********退汇交易不用检查这个了   by liuyue 20070205
	ret=iHvPayin_characterCheck(&hv_payin,acErrMsg);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	**********************************************/
	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	ret=iHv_checkstat(hv_payin.F_cmtno,hv_payin.F_yw_type);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* 得到原来来帐信息并更新原来来帐状态为退回等待状态 */
	ret=iHvGetThMsg(&hv_payin,&hv_lz_payin,acErrMsg);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* 准备写入登记簿的数据 */
	ret=iHvFrontToTable(&hv_payin);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* 写入登记簿 */
	ret=iHvUpdZfRecFromPayin(&hv_payin,0);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d 开始准备帐务",__FILE__,__LINE__);
	iPrepareTh_ac_no(&hv_payin);
		/** 得到清算帐号 */
		ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
		if(ret)
		{
			sprintf(acErrMsg,"得到本机构大额清算帐号错误!\n");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			vtcp_log("%s,%d 得到本机构大额清算帐号错误\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"T042");
			goto ErrExit;
		}
		set_zd_data("0980","6");
		set_zd_data("0310",cAc_no);
		set_zd_data("0990","2");
		/** 得到挂帐帐号  **/
	vtcp_log("%s,%d 开始准备帐务",__FILE__,__LINE__);

	/* 写入业务流水 */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	/*** 输出变量 ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("112A",g_pub_tx.ac_no);
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
