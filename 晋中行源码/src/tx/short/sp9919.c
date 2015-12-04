/*************************************************************
* 文 件 名:  sp9919.c
* 功能描述： 大额支付快捷查询
*			 汇票申请退回根据汇票号码反显汇票信息
*
* 作    者:  jane
* 完成日期： 2003年6月11日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "com_branch_c.h"

int sp9919()
{
	struct mbfebillopnreg_c mbfebillopnreg_v;
	struct com_branch_c com_branch_v;
	struct mbfenetbank_c mbfenetbank_v;

	memset(&mbfebillopnreg_v, 0x00, sizeof(struct mbfebillopnreg_c));
	memset(&com_branch_v, 0x00, sizeof(struct com_branch_c));
	memset(&mbfenetbank_v, 0x00, sizeof(struct mbfenetbank_c));

	pub_base_sysinit();

	get_zd_data("0860",mbfebillopnreg_v.billcode);
	sprintf(mbfebillopnreg_v.billcode,"%08ld",atol(mbfebillopnreg_v.billcode));

	g_reply_int=Mbfebillopnreg_Sel(g_pub_tx.reply, &mbfebillopnreg_v,
		"billcode='%s'", mbfebillopnreg_v.billcode);
	if(g_reply_int==100)
	{
		sprintf( acErrMsg, "没有签发该张汇票!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询汇票登记簿数据库操作出错[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if(mbfebillopnreg_v.billsts[0]!='2')
	{
		if(mbfebillopnreg_v.billsts[0]=='1')
		{
			sprintf( acErrMsg, "该张汇票尚未移存资金!", mbfebillopnreg_v.billcode);
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if(mbfebillopnreg_v.billsts[0]=='*')
		{
			sprintf( acErrMsg, "该张汇票已核销,请检查核销记录!");
			MBFEWRITEMSG
			goto ErrExit;
		}
		else
		{
			sprintf( acErrMsg, "该汇票状态不正常,不能申请退回,请联系中心机房![%s]",mbfebillopnreg_v.billsts);
			MBFEWRITEMSG
			goto ErrExit;
		}
	}
	if(mbfebillopnreg_v.los_rpt_ind[0]=='1')
	{
		sprintf( acErrMsg, "该汇票已挂失!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if (get_bkno()) goto ErrExit;

	if (strcmp(mbfebillopnreg_v.payeropenaccbkcode, mbfetran.cnapsodfi))
	{
		sprintf( acErrMsg, "不能申请退回他行签发汇票![%s]",mbfebillopnreg_v.payeropenaccbkcode);
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Com_branch_Sel(g_pub_tx.reply, &com_branch_v,
		"br_no='%s'", mbfebillopnreg_v.tx_br_no);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "查询录入机构名称出错![%d]", g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if(mbfebillopnreg_v.billtype[0]=='2')
	{
		g_reply_int=Mbfenetbank_Sel(g_pub_tx.reply, &mbfenetbank_v,
			"nbkcode='%s'", mbfebillopnreg_v.cashbillbkno);
		if(g_reply_int)
		{
			sprintf( acErrMsg, "查询现金汇票兑付行名出错![%d]", g_reply_int );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}

	set_zd_long("0460",mbfebillopnreg_v.tx_date);
	set_zd_long("0450",mbfebillopnreg_v.signdate);
	set_zd_double("0390",mbfebillopnreg_v.amount);
	set_zd_data("0910",mbfebillopnreg_v.tx_br_no);
	set_zd_data("0620",com_branch_v.br_name);
	set_zd_data("0210",mbfebillopnreg_v.billtype);
	if(mbfebillopnreg_v.billtype[0]=='0')
		set_zd_data("0870","可转让");
	else if(mbfebillopnreg_v.billtype[0]=='1')
		set_zd_data("0870","不可转让");
	else if(mbfebillopnreg_v.billtype[0]=='2')
		set_zd_data("0870","现金汇票");
	set_zd_data("0300",mbfebillopnreg_v.payeracc);
	set_zd_data("0250",mbfebillopnreg_v.payername);
	set_zd_data("0260",mbfebillopnreg_v.payeraddr);
	set_zd_data("0810",mbfebillopnreg_v.recipientacc);
	set_zd_data("0961",mbfebillopnreg_v.recipientname);
	set_zd_data("0590",mbfebillopnreg_v.cashbillbkno);
	set_zd_data("0963",mbfenetbank_v.nbkname);
	get_sys_par_val("14", mbfetran.rdficode);
	set_zd_data("0580",mbfetran.rdficode);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
