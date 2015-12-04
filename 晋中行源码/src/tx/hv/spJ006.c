/***************************************************************
* 文 件 名:     spJ006.c
* 功能描述：    银行汇票信息修改			
*
* 作    者:   LiuYue
* 完成日期：  2006-8-17 16:51
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"

spJ006()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin;
	struct hv_poinfo_c hv_poinfo;
	char optype[2];/***操作类型 1-修改 0－查询****/
	char cDeal_mode [2];
	HV_FD123_AREA fd123;

	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));
	memset(optype   , 0x00 ,  sizeof(optype));
	memset(&fd123    , 0x00 ,  sizeof(fd123));

	/**************************
         *PART 1 获取前台输入数据 *
         **************************/

	/***得到大额支付业务8583数据***/
	get_zd_data("0710",optype);/****操作类型****/
	get_zd_data("0720",cDeal_mode);/***转出类型***/

	vtcp_log("optype===[%c]\n",optype[0]);
	vtcp_log("optype===[%c]\n",optype[0]);
	
	/****如果是查询的话,直接成功结束程序***/
	if(optype[0]=='0')
	{
		vtcp_log("[%s][%d]你选择了查询功能!程序正常结束!\n",__FILE__,__LINE__);
		goto OkExit;
	}
	/***获取前台数据到payin结构体中*******/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{	
		sprintf(acErrMsg,"从前台获取数据失败!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}

	/***po_no,po_type字段都是放在fd123中进行传递的**/	
	get_fd_data("1230",(char *)&fd123);

	/*******************
         *PART 2 基本判断  *
         *******************/
	 MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));
	 MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	 ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s' ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"数据库操作错误!ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"数据库操作错误!ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==100)
	{
		sprintf(acErrMsg,"查找的记录不存在!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P001");
		goto ErrExit;
	}
	if(hv_poinfo.bank_type[0]=='1')
	{
		sprintf(acErrMsg,"非本行登记的汇票,无权修改!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	if(hv_poinfo.po_sts[0]!='0')
	{
		sprintf(acErrMsg,"汇票不是登记状态,无法进行修改!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P002");
		goto ErrExit;
	}

	/****开始更新 记录*****/	
	/****记录不存在,开始写汇票信息表****/
	dot_str2dbl(hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt),&hv_poinfo.po_bal);     /***出票金额**/
	if(hv_poinfo.l_tx_date==apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date)))
	{
		hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;/**是当天就+1****/	
	}
	else
	{
		hv_poinfo.td_cnt = 1;
	}
        hv_poinfo.l_tx_date = apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date));
        /***上次交易日期,放当前日期***/
	 MEMCPY_DEBUG(hv_poinfo.pay_ac_no,hv_payin.F_pay_ac_no,sizeof(hv_poinfo.pay_ac_no)-1); /***付款人帐号**/
        MEMCPY_DEBUG(hv_poinfo.pay_name,hv_payin.F_pay_name,sizeof(hv_poinfo.pay_name)-1);    /***付款人姓名**/
        MEMCPY_DEBUG(hv_poinfo.pay_addr,hv_payin.F_pay_addr,sizeof(hv_poinfo.pay_addr)-1);   /***付款人地址***/
        MEMCPY_DEBUG(hv_poinfo.cash_ac_no,hv_payin.F_cash_ac_no,sizeof(hv_poinfo.cash_ac_no)-1);/***收款人帐号**/
        MEMCPY_DEBUG(hv_poinfo.cash_name,hv_payin.F_cash_name,sizeof(hv_poinfo.cash_name)-1);/***收款人姓名**/
        MEMCPY_DEBUG(hv_poinfo.hold_ac_no,hv_payin.F_cash_ac_no,sizeof(hv_poinfo.hold_ac_no)-1);/**当前持票人帐号**/
        MEMCPY_DEBUG(hv_poinfo.hold_name,hv_payin.F_cash_name,sizeof(hv_poinfo.hold_name)-1);  /**当前持票人姓名**/
        hv_poinfo.l_po_sts[0] = HV_HP_REG;
        /***上次帐户状态***/
        hv_poinfo.po_sts[0] =HV_HP_REG;
        /***当前汇票的状态***/
        /***登记日期***/
        hv_poinfo.po_ind[0]       = fd123.po_type[0];
        /***汇票类型***/
	 hv_poinfo.deal_mode[0]    =cDeal_mode[0];
        /***转帐现金处理方式***/
        MEMCPY_DEBUG(hv_poinfo.cash_br_no,hv_payin.F_cash_opn_br_no,sizeof(hv_poinfo.cash_br_no)-1);/***兑付行行号**/
        MEMCPY_DEBUG(hv_poinfo.brf,   hv_payin.F_hv_brf,sizeof(hv_poinfo.brf)-1);
        /***附言***/
	
	/****开始更新记录*****/
	vtcp_log("开始更新数据\n");
	vtcp_log("开始更新数据\n");
	vtcp_log("开始更新数据\n");
	ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
	if(ret)
	{	
		sprintf(acErrMsg,"更新汇票基本信息表错误!ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("更新数据成功!\n");
	vtcp_log("更新数据成功!\n");
	vtcp_log("更新数据成功!\n");
	vtcp_log("更新数据成功!\n");

	
	/*********************
	 *PART 4  结尾工作   *
     *********************/	 
	
	/* 写入业务流水 */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	/*** 输出变量 ***/

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
