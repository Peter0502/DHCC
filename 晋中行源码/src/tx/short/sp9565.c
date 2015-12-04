/*****************************************************
* 文 件 名:  sp9565.c
* 功能描述： 通过非支付交易查询对应的支付交易 小额专用
* 作    者:   dadary 
* 完成日期： 2006年9月9日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_pkgreg_c.h"
#include "hv_fzf_c.h"
int sp9565()
{
	int ret=0,i;
	char cDate[9];
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;
	struct lv_pkgreg_c lv_pkgreg;
	HV_FD123_AREA fd123;

	int iCmtno = 0;

	memset(cDate, 0 , sizeof(cDate));
	memset(&fd123, 0,sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(&hv_fzf,0x00,sizeof(hv_fzf));
	memset(&lv_pkgreg,0x00,sizeof(lv_pkgreg));


	vtcp_log("sp9565 开始！[%s][%d]",__FILE__,__LINE__);

	/**********************
	 * 1.初始化全局变量   *
	 **********************/
 	
	pub_base_sysinit();


	/*********************
	 * 2.获取8583域内容  *
	 *********************/

	get_zd_long("0460",&hv_fzf.wt_date);		/***原申请日期***/
	get_fd_data("1230",(char*)&fd123);
	memcpy(hv_fzf.cmtno,fd123.cmtno,sizeof(hv_fzf.cmtno)-1);	/**交易的cmt编号**/
	memcpy(hv_fzf.orderno,fd123.qr_tx_num,8);      	/**原非支付序号**/

	memcpy(hv_fzf.or_br_no,	fd123.ac_br_no,	sizeof(hv_fzf.or_br_no)-1);
									/**应答类的交易中,申请行就是该交易的接受行号********/
	iCmtno = atoi(hv_fzf.cmtno);
	sprintf(hv_fzf.cmtno,"%3d",iCmtno-1);
	vtcp_log("[%s][%d]hv_fzf.cmtno=[%s]\n",__FILE__,__LINE__,hv_fzf.cmtno);

	/**********************
	 * 3.检测原非支付交易 *
	 **********************/
	ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, " cmtno='%.3s' and  orderno='%s' and or_br_no='%s' and wt_date=%ld  and otype='2' ", hv_fzf.cmtno, hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
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
	vtcp_log("fzf......o_cmtno=[%3s],o_ordenro=[%8s],o_or_br_no=[%12s],wt_date=[%ld]",hv_fzf.o_cmtno,hv_fzf.o_orderno,hv_fzf.o_or_br_no,hv_fzf.o_tx_date);	
    vtcp_log("查询非支付记录成功! [%s][%d]",__FILE__,__LINE__);

	/*****如果是整包的情况是无法弹出详细信息的****/
	if(hv_fzf.reqtype[0]=='1')/***如果是整包的话**/
	{
		vtcp_log("[%s][%d]你选择的交易是整包交易!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"ZBAO");
		goto ErrExit1; 
	}


	/*************************
	 * 4. 检测对应的支付交易 *
	 *************************/

	memcpy(lv_pkgreg.orderno,hv_fzf.o_orderno,sizeof(lv_pkgreg.orderno)-1);		/*支付交易序号*/
	memcpy(lv_pkgreg.or_br_no,hv_fzf.o_or_br_no,sizeof(lv_pkgreg.or_br_no)-1);	/*支付发起行号*/
	lv_pkgreg.wt_date = hv_fzf.o_wt_date;						/*支付交易委托日期*/

	vtcp_log("开始查询支付交易表![%s][%d]",__FILE__,__LINE__);

	
	ret = Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, " orderno='%s' and or_br_no='%s' and wt_date=%d", lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"找不到对应的支付交易");
		WRITEMSG
		strcpy(g_pub_tx.reply,"NROW");
		goto	ErrExit1;
	}
	else if ( ret&&ret!=100 )
	{
		sprintf(acErrMsg,"查询大额支付交易基本信息出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
  	  vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
   

	/******************
	 * 5.准备返回值   *
	 ******************/ 
	memcpy(fd123.or_br_no        ,lv_pkgreg.ac_br_no        ,sizeof(fd123.or_br_no        ));/**发起行号 原发起行号 */
	memcpy(fd123.pay_ac_no       ,lv_pkgreg.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));/**付款人帐号(原收款人帐号)**/
	memcpy(fd123.pay_name        ,lv_pkgreg.pay_name        ,sizeof(fd123.pay_name        ));/**付款人名称(原收款人名称)**/
	memcpy(fd123.ac_br_no        ,lv_pkgreg.or_br_no        ,sizeof(fd123.ac_br_no        ));/**接受行号(原发起行号)**/
	memcpy(fd123.cash_ac_no      ,lv_pkgreg.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));/**收款人帐号,原付款人帐号**/
	memcpy(fd123.cash_name       ,lv_pkgreg.cash_name       ,sizeof(fd123.cash_name       ));/**收款人姓名,原付款人姓名**/
	memcpy(fd123.o_orderno       ,lv_pkgreg.orderno       ,sizeof(fd123.o_orderno       ));/**原支付交易序号**/
	memcpy(fd123.notpay_content  ,hv_fzf.req_content      ,sizeof(fd123.notpay_content       ));/**申请附言**/
	vtcp_log("附言=[%s]\n",fd123.notpay_content);
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	/***********送原委托日期***********************/
	set_zd_long("0450",lv_pkgreg.wt_date);
	/*****送交易金额***********/
        set_zd_double("0390",lv_pkgreg.tx_amt);   /* 交易金额 */
 
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
ErrExit1:
	memcpy(fd123.notpay_content  ,hv_fzf.req_content      ,sizeof(fd123.notpay_content       ));/**申请附言**/
	vtcp_log("附言=[%s]\n",fd123.notpay_content);
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	Hv_fzf_Clo_Sel();
	Hv_zf_Clo_Sel();
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


