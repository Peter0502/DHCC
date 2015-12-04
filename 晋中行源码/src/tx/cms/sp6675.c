/*************************************************
* 文 件 名: sp6675.c
* 功能描述：完成信贷发放贷款的通知
*
* 作    者: chenchao
* 完成日期：2011-7-21 15:54:14
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6675','信贷发放贷款通知','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6675','0','6675','#$');
insert into tx_sub_def values ('6675','信贷发放贷款通知','sp6675','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "ln_auth_c.h"
#include "mo_po_co_c.h"
#include "draw_loan_limit_c.h"

int sp6675()
{
	int	iRet = 0;
	long	cif_no = 0;
	char	cGet_data[201];
	char	cLn_type[3];
	
	struct	cms_rollback_c		sCms_rollback;
	struct	ln_auth_c		sLn_auth;
	struct	mo_po_co_c		sMo_po_co;
	struct	draw_loan_limit_c	sDraw_loan_limit;
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sLn_auth,0x00,sizeof(sLn_auth));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(cLn_type,0x00,sizeof(cLn_type));
	memset(&sMo_po_co,0x00,sizeof(sMo_po_co));
	memset(&sDraw_loan_limit,0x00,sizeof(sDraw_loan_limit));
	
	pub_base_sysinit();
	
	get_zd_data("0370",cGet_data);	/** 借据号 **/
	zip_space(cGet_data);
	memcpy(sLn_auth.pact_no,cGet_data,sizeof(sLn_auth.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0220",cLn_type);	/** 贷款:1:普通 2:按揭 **/
	
	get_zd_data("0250",cGet_data);	/** 户名   **/
	zip_space(cGet_data);
	memcpy(sLn_auth.name,cGet_data,sizeof(sLn_auth.name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_long("0530",&sLn_auth.time_lmt);	/** 期限  **/
	get_zd_double("0390",&sLn_auth.amt);	/** 借据金额 **/
	
	get_zd_double("0840",&sLn_auth.rate);	/** 合同利率 **/
	get_zd_double("1005",&sLn_auth.fine_rate);	/** 罚息利率 **/
	get_zd_data("0230",sLn_auth.prdt_no);	/** 产品编号 **/
	get_zd_data("0300",cGet_data);		/** 付款帐号 **/
	zip_space(cGet_data);
	memcpy(sLn_auth.pay_ac_no,cGet_data,sizeof(sLn_auth.pay_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	pub_base_old_acno(sLn_auth.pay_ac_no);
	get_zd_data("0310",cGet_data);	/** 收款账号 **/
	zip_space(cGet_data);
	memcpy(sLn_auth.save_ac_no,cGet_data,sizeof(sLn_auth.save_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	pub_base_strpack( sLn_auth.save_ac_no );
	pub_base_old_acno(sLn_auth.save_ac_no);
	/** 检查账号的合法性 **/
	if( strlen( sLn_auth.pay_ac_no ) )
	{
		iRet = pub_ln_AcCheck( sLn_auth.pay_ac_no,1,&cif_no,&sLn_auth.pay_ac_id );
		if( iRet )
		{
			sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L185");
			}
			goto ErrExit;
		}
	}
	/** 检查账号的合法性 **/
	if( strlen( sLn_auth.save_ac_no ) )
	{
		iRet = pub_ln_AcCheck( sLn_auth.save_ac_no,1,&cif_no,&sLn_auth.save_ac_id );
		if( iRet )
		{
			sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L185");
			}
			goto ErrExit;
		}
	}
	memcpy( sLn_auth.save_mdm_code , g_mdm_ac_rel.mdm_code ,sizeof(sLn_auth.save_mdm_code)-1);	/** 介质 **/
	sLn_auth.sts[0] = 'X';	/** 状态 **/
	get_zd_double("0850",&sLn_auth.over_rate );	/** 逾期利率 **/
	get_zd_long("0450",&sLn_auth.mtr_date);		/** 到期日期 **/
	get_zd_long("0280",&sLn_auth.cif_no);		/** 客户号  **/
	memcpy(sLn_auth.type,"01",2);
	get_zd_data("0690",sLn_auth.ln_type);		/** 贷款种类 **/
	get_zd_double("0970",&sLn_auth.flt_rate);	/** 浮动比例 **/
	get_zd_long("0440",&sLn_auth.beg_date);		/** 审批日期 **/
	get_zd_data("0680",sLn_auth.intst_type);	/** 计息类型 **/
	get_zd_data("0890",sLn_auth.repay_type);	/** 还款方式 **/

	get_zd_data("0830",cGet_data);			/** 担保客户号 **/
	zip_space(cGet_data);
	memcpy(sLn_auth.bail_cif_no,cGet_data,sizeof(sLn_auth.bail_cif_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data("0380",cGet_data);			/** 委托协议编号**/
	zip_space(cGet_data);
	memcpy(sLn_auth.trust_no,cGet_data,sizeof(sLn_auth.trust_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data("0810",cGet_data);			/** 产品名称**/
	zip_space(cGet_data);
	memcpy(sLn_auth.filler1,cGet_data,sizeof(sLn_auth.filler1)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data("0820",cGet_data);			/** 贷款用途**/
	zip_space(cGet_data);
	memcpy(sLn_auth.filler2,cGet_data,sizeof(sLn_auth.filler2)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_double("0430",&sLn_auth.intst_amt);	/** 合同总金额**/
	
	
	/*memcpy(sLn_auth.cms_tx_br_no,g_pub_tx.tx_br_no,sizeof(sLn_auth.cms_tx_br_no)-1);*/
	/* 贷款把机构号附为实际房贷的机构 原来是10000号 */
	char sCms_tx_br_no[11];
	memset(sCms_tx_br_no, 0x00, sizeof(sCms_tx_br_no));
	get_zd_data("0880",sCms_tx_br_no); /*放贷实际机构号*/
	memcpy(sLn_auth.cms_tx_br_no,sCms_tx_br_no,sizeof(sLn_auth.cms_tx_br_no)-1);
	
	iRet = Ln_auth_Ins(sLn_auth,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入审批登记薄失败.请检查",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入审批登记薄失败!!!");
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	memcpy(sCms_rollback.beg_table,"ln_auth",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and type='01' and sts='X' ",sLn_auth.pact_no);
	sCms_rollback.sts[0] = '0';
	get_zd_data("1230",sCms_rollback.filler);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{ 
		Cms_rollback_Debug(&sCms_rollback);
		vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
		goto ErrExit;
	}
	
	memcpy(sDraw_loan_limit.pact_no,sLn_auth.pact_no,sizeof(sDraw_loan_limit.pact_no)-1);
	memcpy(sDraw_loan_limit.ac_no,sLn_auth.save_ac_no,sizeof(sDraw_loan_limit.ac_no)-1);
	sDraw_loan_limit.ac_id = sLn_auth.save_ac_id;
	sDraw_loan_limit.save_amt = sLn_auth.amt;
	get_zd_double("0420",&sDraw_loan_limit.trust_amt);
	get_zd_double("0410",&sDraw_loan_limit.self_amt);
	sDraw_loan_limit.sts[0] = 'X';
	memcpy(sDraw_loan_limit.br_no,g_pub_tx.tx_br_no,sizeof(sDraw_loan_limit.br_no)-1);
	sDraw_loan_limit.tx_date = g_pub_tx.tx_date;
	
	iRet = Draw_loan_limit_Ins(sDraw_loan_limit,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入支付登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入支付登记薄出错!!!"); 
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	memcpy(sCms_rollback.beg_table," draw_loan_limit ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and sts='X' ",sLn_auth.pact_no);
	sCms_rollback.sts[0] = '0';
	get_zd_data("1230",sCms_rollback.filler);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
		goto ErrExit;
	}
	get_zd_data("0330",cGet_data);	/** 合同号 **/
	zip_space(cGet_data);
	memcpy(sMo_po_co.contract_no,cGet_data,sizeof(sMo_po_co.contract_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	memcpy(sMo_po_co.pact_no,sLn_auth.pact_no,sizeof(sMo_po_co.pact_no)-1);
	iRet = Mo_po_co_Ins(sMo_po_co,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入合同借据关系表出错",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入合同借据关系表出错!!!"); 
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	memcpy(sCms_rollback.beg_table," mo_po_co",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and contract_no='%s' ",sMo_po_co.pact_no,sMo_po_co.contract_no);
	sCms_rollback.sts[0] = '0';
	sCms_rollback.filler[0]='#';
	get_zd_data("1230",sCms_rollback.filler+1);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入回滚登记薄出错.请检查iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入回滚登记薄出错!!!"); 
		goto ErrExit;
	}
	
OkExit:
	sprintf( acErrMsg, "信贷发放贷款的通知成功!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:	
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "信贷发放贷款的通知失败[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
