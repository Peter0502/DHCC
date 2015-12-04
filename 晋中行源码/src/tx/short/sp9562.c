/*****************************************************
* 文 件 名:  sp9562.c
* 功能描述： 通过非支付交易查询对应的支付交易 
* 作    者:   dadary 
* 完成日期： 2006年8月20日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
int sp9562()
{
	int ret=0,i;
	int iCmtno = 0;
	char cOrderno[9];
	char cDate[9];
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;
	HV_FD123_AREA fd123;

	memset(cDate, 0 , sizeof(cDate));
	memset(&fd123, 0,sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(&hv_fzf,0x00,sizeof(hv_fzf));
	memset(cOrderno,0x00,sizeof(cOrderno));

	vtcp_log("sp9562 开始！[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();

	get_zd_long("0460",&hv_fzf.wt_date);/***原申请日期***/
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("[%s][%d]1230==[%s]",__FILE__,__LINE__,(char*)&fd123);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	vtcp_log("fd123.o_notpay_orderno==[%3s]\n",fd123.o_notpay_orderno);
	memcpy(hv_fzf.cmtno,	fd123.o_cmtno,		sizeof(hv_fzf.cmtno)-1);
	memcpy(cOrderno,"00000",5);
	memcpy(hv_fzf.orderno,"00000",5);
	memcpy(hv_fzf.orderno+5,fd123.o_notpay_orderno,3);
	vtcp_log("[%s][%d]orderno==[%s]\n",__FILE__,__LINE__,hv_fzf.orderno);
	memcpy(hv_fzf.or_br_no,	fd123.ac_br_no,		sizeof(hv_fzf.or_br_no)-1);/**应答类的交易中,申请行就是该交易的接受行号********/


	iCmtno	= apatoi(hv_fzf.cmtno,sizeof(hv_fzf.cmtno)-1);

	vtcp_log("sp9562 开始！[%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]cmtno=[%s]orderno=[%s],or_br_no=[%s]wt_date=[%ld]",__FILE__,__LINE__,hv_fzf.cmtno,hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
	vtcp_log("sp9562 开始！[%s][%d]",__FILE__,__LINE__);
	
	if(iCmtno!=0)
	{
	ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='1' and cmtno='%s' and orderno='%s' and or_br_no='%s' and wt_date=%ld ", hv_fzf.cmtno,hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
	}
	else if(iCmtno==0)
	{
	ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='1' and  orderno='%s' and or_br_no='%s' and wt_date=%ld ", hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
	}
	
	if(ret)
	{
		sprintf(acErrMsg,"操作数据库错误!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	ret = Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此非支付交易");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P020" ); 
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询大额非支付交易基本信息出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P020");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_fzf.o_tx_date==[%ld]\n",__FILE__,__LINE__,hv_fzf.o_tx_date);
	vtcp_log("[%s][%d]hv_fzf.o_tx_date==[%ld]\n",__FILE__,__LINE__,hv_fzf.o_tx_date);
	vtcp_log("fzf......o_cmtno=[%3s],o_ordenro=[%8s],o_or_br_no=[%12s],wt_date=[%ld]",hv_fzf.o_cmtno,hv_fzf.o_orderno,hv_fzf.o_or_br_no,hv_fzf.o_tx_date);	
    vtcp_log("查询非支付记录成功! [%s][%d]",__FILE__,__LINE__);




 	   /***从非支付表中获取到支付记录的信息******/
   	memcpy(hv_zf.cmtno,hv_fzf.o_cmtno,sizeof(hv_zf.cmtno)-1);
	memcpy(hv_zf.orderno,hv_fzf.o_orderno,sizeof(hv_zf.orderno)-1);
	memcpy(hv_zf.or_br_no,hv_fzf.o_or_br_no,sizeof(hv_zf.or_br_no)-1);
	hv_zf.wt_date = hv_fzf.o_wt_date;

	vtcp_log("开始查询支付交易表![%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]====cmtno=[%s],orderno=[%s],or_br_no=[%s],wt_date=[%ld]\n",__FILE__,__LINE__,hv_zf.cmtno,hv_zf.orderno,hv_zf.or_br_no,hv_zf.wt_date);

	/****把cmt这个字段不作为where条件,其他三个因素已经足够锁定一个表了**/
	
	ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, " orderno='%s' and or_br_no='%s' and wt_date=%d", hv_zf.orderno,hv_zf.or_br_no,hv_zf.wt_date);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"找不到对应的支付交易");
		WRITEMSG
		strcpy(g_pub_tx.reply,"9999");
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询大额支付交易基本信息出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
    vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
    
	 memcpy(fd123.cmtno,hv_zf.cmtno,sizeof(fd123.cmtno));
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	memcpy(fd123.o_cmtno,hv_zf.o_cmtno,sizeof(fd123.cmtno));
	/*******cmt编号*********/
	memcpy(fd123.or_br_no        ,hv_zf.ac_br_no        ,sizeof(fd123.or_br_no        ));/**发起行号 原发起行号 */
	memcpy(fd123.pay_ac_no       ,hv_zf.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));/**付款人帐号(原收款人帐号)**/
	memcpy(fd123.pay_name        ,hv_zf.pay_name        ,sizeof(fd123.pay_name        ));/**付款人名称(原收款人名称)**/
	memcpy(fd123.ac_br_no        ,hv_zf.or_br_no        ,sizeof(fd123.ac_br_no        ));/**接受行号(原发起行号)**/
	vtcp_log("原发起行行号=[%s]\n",fd123.ac_br_no);
	vtcp_log("原发起行行号=[%s]\n",fd123.ac_br_no);
	vtcp_log("原发起行行号=[%s]\n",fd123.ac_br_no);
	vtcp_log("原发起行行号=[%s]\n",fd123.ac_br_no);
	vtcp_log("原发起行行号=[%s]\n",fd123.ac_br_no);
	memcpy(fd123.cash_ac_no      ,hv_zf.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));/**收款人帐号,原付款人帐号**/
	memcpy(fd123.cash_name       ,hv_zf.cash_name       ,sizeof(fd123.cash_name       ));/**收款人姓名,原付款人姓名**/
	memcpy(fd123.o_orderno       ,hv_zf.orderno       ,sizeof(fd123.o_orderno       ));/**原支付交易序号**/
	memcpy(fd123.notpay_content       ,hv_fzf.req_content      ,sizeof(fd123.notpay_content       ));/**申请附言**/
	vtcp_log("附言=[%s]\n",fd123.notpay_content);
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	/***********送原委托日期***********************/
	set_zd_long("0450",hv_zf.wt_date);
	/*****送交易金额***********/
        set_zd_double("0390",hv_zf.tx_amt);   /* 交易金额 */
 
OkExit:
	Hv_fzf_Clo_Sel();
	Hv_zf_Clo_Sel();
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	Hv_fzf_Clo_Sel();
	Hv_zf_Clo_Sel();
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


