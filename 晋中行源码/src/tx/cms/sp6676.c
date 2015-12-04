/*************************************************
* 文 件 名: sp6676.c
* 功能描述：完成信贷贷款展期的通知
*
* 作    者: chenchao
* 完成日期：2011-7-23 10:52:36
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6676','信贷贷款展期通知','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6676','0','6676','#$');
insert into tx_sub_def values ('6676','信贷贷款展期通知','sp6676','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "ln_auth_c.h"

int sp6676()
{
	int	iRet = 0;
	long	cif_no = 0;
	char	cGet_data[201];
	
	struct	cms_rollback_c	sCms_rollback;
	struct	ln_auth_c	sLn_auth;
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sLn_auth,0x00,sizeof(sLn_auth));
	
	pub_base_sysinit();
	
	get_zd_data("0370",cGet_data);	/** 借据号 **/
	zip_space(cGet_data);
	memcpy(sLn_auth.pact_no,cGet_data,sizeof(sLn_auth.pact_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_long("0530",&sLn_auth.time_lmt);	/** 期限 **/
	get_zd_long("0440",&sLn_auth.mtr_date);	/** 到期日期 **/
	get_zd_double("0840",&sLn_auth.rate);	/** 展期利率 **/
	get_zd_long("0520",&sLn_auth.cif_no);	/** 客户号 **/
	
	get_zd_data("0250",cGet_data);	/** 客户名称   **/
	zip_space(cGet_data);
	memcpy(sLn_auth.name,cGet_data,sizeof(sLn_auth.name));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_double("0400",&sLn_auth.amt);	/** 展期金额  **/
	memcpy(sLn_auth.type,"02",2);		/**02-标识展期通知 **/
	sLn_auth.sts[0] = 'X';			/** 信贷发起 **/
	
	memcpy(sLn_auth.cms_tx_br_no,g_pub_tx.tx_br_no,sizeof(sLn_auth.cms_tx_br_no)-1);
	iRet = Ln_auth_Ins(sLn_auth,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入审批登记薄失败.请检查ret=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入审批登记薄失败!!!");
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," ln_auth ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and type='02' and sts='X' ",sLn_auth.pact_no);
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
	sprintf( acErrMsg, "信贷贷款展期的通知成功!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)){
	memcpy(g_pub_tx.reply,"H034",4);}
	sprintf( acErrMsg, "信贷贷款展期的通知失败[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
