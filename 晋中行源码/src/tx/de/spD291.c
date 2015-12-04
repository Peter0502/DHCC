/***************************************************************
* 文 件 名:     spD291.c
* 功能描述：	同城提出信息录入
* 作    者:     jack
* 完成日期：    2004年1月24日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_city_c.h"
#include "com_branch_c.h"

spD291()
{
	char tmp_memo1[50],tmp_memo2[50];
	long date1,date2;

	struct mo_city_c	g_mo_city;
	struct com_branch_c	g_com_branch;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct dd_parm_c	g_dd_parm;
	struct td_parm_c	g_td_parm;

	memset(&g_mo_city,0x00,sizeof(struct mo_city_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));

	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0670",g_mo_city.trans_mode);			/*取帐号*/
	get_zd_data("0680",g_mo_city.in_out_ind);			/*提出提入标志*/
	get_zd_data("0240",g_mo_city.bill_type);			/*票据类型*/
	get_zd_data("0580",g_mo_city.bill_no);				/*票据号码*/
	get_zd_data("0590",g_mo_city.bill_pact_no);			/*合同编号*/
	get_zd_data("0310",g_mo_city.payer_ac_no);			/*付款人帐号*/
	get_zd_data("0250",g_mo_city.payer_name);			/*付款人名称*/
	get_zd_data("0860",g_mo_city.payer_br_no);			/*付款人开户行号*/
	get_zd_data("0810",g_mo_city.payer_br_name);		/*付款人开户行名*/
	get_zd_data("0320",g_mo_city.payee_ac_no);			/*收款人帐号*/
	get_zd_data("0260",g_mo_city.payee_name);			/*收款人名称*/
	get_zd_data("0870",g_mo_city.payee_br_no);			/*收款人开户行号*/
	get_zd_data("0820",g_mo_city.payee_br_name);		/*收款人开户行名*/
	get_zd_double("0400",&g_mo_city.bill_amt);			/*票面金额*/
	get_zd_double("0410",&g_mo_city.sttl_amt);			/*结算金额*/
	get_zd_long("0440",&g_mo_city.sign_date);			/*出票日期*/
	get_zd_long("0450",&g_mo_city.matr_date);			/*到期日期*/

	get_zd_data("0730",tmp_memo1);						/*摘要*/
	get_zd_data("0260",tmp_memo2);						/*备注*/

	strcpy(g_mo_city.tx_br_no,g_pub_tx.tx_br_no);		/*机构号*/
	g_mo_city.tx_date = g_pub_tx.tx_date;				/*交易日期*/
	g_mo_city.tx_no = g_pub_tx.trace_no;				/*流水号*/

		sprintf(acErrMsg,"g_pub_tx.tx_br_no=[%s]!!",g_pub_tx.tx_br_no);
		WRITEMSG

	g_reply_int=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",
								g_pub_tx.tx_br_no);
	if (g_reply_int==100)
	{
		sprintf(acErrMsg,"公共机构表中不存在该记录!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O093");
		goto ErrExit;
	}
	else if (g_reply_int)
	{
		sprintf(acErrMsg,"查询公共机构表异常!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O092");
		goto ErrExit;
	}

	g_mo_city.no = g_com_branch.city_no;				/*场次*/

	g_reply_int=Mo_city_Ins(g_mo_city,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"登记同城登记薄出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O091");
		goto ErrExit;
	}

	if(g_mo_city.in_out_ind[0]=='1')
	{
		
	}
	else if(g_mo_city.in_out_ind[0]=='2')
	{
		
	}
	else
	{
	 	sprintf(acErrMsg,"标志选择错误!");
	  	WRITEMSG
	    goto ErrExit;
	}

	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"同城提出信息录入成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"同城提出信息录入失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
