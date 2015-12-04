/*************************************************
* 文 件 名: sp6681.c
* 功能描述：此交易完成信贷支付审批功能
*
* 作    者: chenchao
* 完成日期：2011-7-20 9:50:43
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6681','信贷支付审批功能','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6681','0','6681','#$');
insert into tx_sub_def values ('6681','信贷支付审批功能','sp6681','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "draw_loan_hst_c.h"
#include "mdm_ac_rel_c.h"

int sp6681()
{
	int	iRet = 0;
	char	cGet_data[201];
	
	struct	draw_loan_hst_c	sDraw_loan_hst;
	struct	mdm_ac_rel_c	sMdm_ac_rel;
	struct	cms_rollback_c	sCms_rollback;
	
	memset(&sDraw_loan_hst,0x00,sizeof(sDraw_loan_hst));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(cGet_data,0x00,sizeof(cGet_data));
	
	pub_base_sysinit();
	
	get_zd_data("0670",  sDraw_loan_hst.st_flag    );	/** 支取方式             **/
	get_zd_data("0370",  cGet_data    );			/** 借据号               **/
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.pact_no,cGet_data,sizeof(sDraw_loan_hst.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0300",  cGet_data);			/** 授权支付协议号       **/
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.draw_no,cGet_data,sizeof(sDraw_loan_hst.draw_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
/*****
	get_zd_data("0380",  sDraw_loan_hst.ac_no      );	** 贷款对应的存款账号   
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.draw_no,cGet_data,sizeof(sDraw_loan_hst.draw_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
*****/
	
	
	get_zd_double("0400",  &sDraw_loan_hst.draw_amt   );	/** 支取金额             **/
	get_zd_data("0320",  cGet_data);			/** 委托收款账号         **/
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.trust_ac_no,cGet_data,sizeof(sDraw_loan_hst.trust_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	
	get_zd_data("0250", cGet_data );	/** 委托收款户名         **/
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.trust_name,cGet_data,sizeof(sDraw_loan_hst.trust_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	sDraw_loan_hst.sts[0] = '1';				/** 状态为待支取状态 **/
	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id in (select save_ac_id from ln_mst where pact_no='%s' )",sDraw_loan_hst.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "查询介质关系表出错!pact_no[%s]ret=[%d]" ,sDraw_loan_hst.pact_no ,iRet);
		WRITEMSG
		set_zd_data(DC_GET_MSG,"此借据号未贷款开户，贷款支付申请不能继续!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	
	memcpy(sDraw_loan_hst.ac_no,sMdm_ac_rel.ac_no,sizeof(sDraw_loan_hst.ac_no)-1);
	sDraw_loan_hst.ac_id = sMdm_ac_rel.ac_id;
	sDraw_loan_hst.approve_date = g_pub_tx.tx_date;
	
	iRet = Draw_loan_hst_Ins(sDraw_loan_hst,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入支付明细表失败.请检查ret=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入支付明细表失败!!!");
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," draw_loan_hst ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," sts='1' and draw_no='%s' ",sDraw_loan_hst.draw_no);
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
	
OkExit:
	sprintf( acErrMsg, "信贷支付审批成功!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "信贷支付审批失败[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
