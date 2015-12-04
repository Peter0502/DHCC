/***************************************************************
* 文 件 名:     spJ017.c
* 功能描述：    逾期退回银行汇票解付  
*
* 作    者:   LiuYue
* 完成日期：  2006-8-27 21:08
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


spJ017()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin;
	struct hv_poinfo_c hv_poinfo;
	char cPwd[11];/**获取密押***/
	HV_FD123_AREA  fd123;	

	memset(&hv_payin,0x00,sizeof(hv_payin));
	memset(&hv_poinfo,0x00,sizeof(hv_poinfo));
	memset(cPwd,0x00,sizeof(cPwd));
	memset(&fd123,0x00,sizeof(fd123));




	/******************
	 * 初始化系统变量 *
	 ******************/

	  pub_base_sysinit();
	
     /************************ 
     *PART 1 获取到前台数据 *
     ************************/

	/***得到大额支付业务8583数据***/
	get_zd_data("0880",cPwd);   /****获取到密押******/
	get_fd_data("1230",(char *)&fd123);
	vtcp_log("fd123=[%s]\n",(char *)&fd123);
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		WRITEMSG("从前台获取数据错误!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	/****不是本行签发的汇票不可以办理次业务****/
	if(memcmp(fd123.pay_opn_br_no,hv_payin.F_pay_opn_br_no,sizeof(fd123.pay_opn_br_no)))
	{
		sprintf(acErrMsg,"非本行签发汇票,不可以兑付该笔逾期退回资金\n");		WRITEMSG
		vtcp_log("fd123=[%s] hv_payin=[%s]\n",fd123.pay_opn_br_no,hv_payin.F_pay_opn_br_no);
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	/*vtcp_log("+++++++++cPwd=[%s]=[%d]++++++[%s]\n",__FILE__,__LINE__,cPwd);*/
	vtcp_log("%s,%d fd123.po_no==[%s]\n",__FILE__,__LINE__,fd123.po_no);
	vtcp_log("%s,%d fd123.po_no==[%s]\n",__FILE__,__LINE__,fd123.po_no);
	vtcp_log("%s,%d fd123.po_no==[%s]\n",__FILE__,__LINE__,fd123.po_no);
	vtcp_log("%s,%d fd123.po_no==[%s]\n",__FILE__,__LINE__,fd123.po_no);
  /***将从前台获取的值放入到hv_poinfo结构体中**/
	MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,HV_POLEN);
	MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
		
	vtcp_log("po_no==[%s]\n",hv_poinfo.po_no);
	vtcp_log("pay_br_no=[%s]\n",hv_poinfo.pay_br_no);
	/*vtcp_log("pwd===[%s]\n",cPwd);*/		


     /***********************
     *PART 3   操作数据表  *
     ***********************/	
	/******获取主键********************/
	ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s' ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数hv_poinfo_dec_upd错误!ret==[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P001");
		goto ErrExit;
	}
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);

	ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数Hv_poinfo_Fet_Upd错误！ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}
	/*vtcp_log("+[%d]+pwd==[%s]++++++\n",__LINE__,cPwd);*/
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
	if(hv_poinfo.po_sts[0]!=HV_HP_OVERDATE_UNTREAD)
	{
		sprintf(acErrMsg,"汇票不是逾期未用退回状态!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P004");
		goto ErrExit;
	}
	/*vtcp_log("+[%d]+pwd==[%s]++++++\n",__LINE__,cPwd);*/
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
	if(hv_poinfo.bank_type[0]!='0')
	{
		sprintf(acErrMsg,"非本行的汇票,无法解付!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P004");
		goto ErrExit;
	}
	if(memcmp(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1))
	{
		sprintf(acErrMsg,"输入的密押错误!\n");
		WRITEMSG
		/*vtcp_log("表=[%s],输入=[%s]\n",hv_poinfo.pwd,cPwd);*/
		strcpy(g_pub_tx.reply,"P005");
		goto ErrExit;
	}
	/****************************************************
	 ******************帐务处理**************************
		261--cust
		24403 -- 24401
	****************************************************/	
	/** 准备帐务处理分录,从参数配置表中得到相应的帐号 */
	set_zd_data("0990","0");
	set_zd_double("0400",hv_poinfo.po_bal);
	/***帐务处理完毕!开始更新表**************************/
	hv_poinfo.po_sts[0]  = HV_HP_UNUSED_TOCUST;/**更新汇票状态**/
	if(hv_poinfo.l_tx_date==apatoi(hv_payin.F_tx_date,sizeof(hv_payin.F_tx_date)))
		hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
	else
		hv_poinfo.td_cnt = 1;
	hv_poinfo.l_tx_date  = apatoi(hv_payin.F_tx_date,sizeof(hv_payin.F_tx_date));
	/******现在开始更新那些字段吧*************************/		
	ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数hv_poinfo_upd_upd错误!ret==[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	/*******************
     * PART 4 结尾工作 *
     ******************/

	/* 写入业务流水 */
	MEMCPY_DEBUG(g_pub_tx.beg_note_no,hv_poinfo.po_no,sizeof(g_pub_tx.beg_note_no));
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
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
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);

	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
