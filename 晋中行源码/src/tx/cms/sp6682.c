/*************************************************
* 文 件 名: sp6682.c
* 功能描述：此交易完成信贷核销审批功能
*
* 作    者: chenchao
* 完成日期：2011-8-3 14:58:29
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6682','信贷核销审批','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6682','0','6682','#$');
insert into tx_sub_def values ('6682','信贷核销审批','sp6682','0','0');
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "ln_auth_c.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "ln_parm_c.h"

int sp6682()
{
	int	iRet = 0;
	char	cGet_data[201];
	
	struct	mdm_ac_rel_c	sMdm_ac_rel;
	struct	ln_mst_c	sLn_mst;
	struct	cms_rollback_c	sCms_rollback;
	struct	ln_auth_c	sLn_auth;
	struct	ln_parm_c	sLn_parm;
	
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sLn_mst,0x00,sizeof(sLn_mst));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(&sLn_auth,0x00,sizeof(sLn_auth));
	memset(&sLn_parm,0x00,sizeof(sLn_parm));
	
	pub_base_sysinit();
	
	get_zd_data("0300",cGet_data);
	zip_space(cGet_data);
	memcpy(sLn_auth.pact_no,cGet_data,sizeof(sLn_auth.pact_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_double("0400",&sLn_auth.amt);		/** 核销本金金额 **/
	get_zd_double("0410",&sLn_auth.intst_amt);	/** 核销利息金额 **/
	
	iRet = Ln_mst_Sel(g_pub_tx.reply,&sLn_mst," pact_no ='%s' ",sLn_auth.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "查询贷款主表出错!pact_no[%s]ret=[%d]" ,sLn_auth.pact_no ,iRet);
		WRITEMSG
		set_zd_data(DC_GET_MSG,"查询贷款主表出错!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;		
	}
	iRet = Ln_parm_Sel(g_pub_tx.reply,&sLn_parm,"prdt_no='%s'",sLn_mst.prdt_no);
	if(iRet)
	{
		sprintf( acErrMsg, "查询贷款参数表出错!pact_no[%s]ret=[%d]" ,sLn_mst.prdt_no ,iRet);
		WRITEMSG
		set_zd_data(DC_GET_MSG,"查询贷款参数表出错!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;		
	}
	if(pub_base_CompDblVal(sLn_auth.amt,0.00)>0)		/** 本金核销 **/
	{
		/* 检查主文件中金额是否为0 */	
		if ( !pub_base_CompDblVal(sLn_mst.bal , 0.00) )
		{
			sprintf(acErrMsg,"贷款本金为0,不能核销本金![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"贷款本金为0,不能核销本金!");
			strcpy( g_pub_tx.reply,"L057" );
			goto ErrExit;
		}		
		/* 检查贷款状态是否为呆帐 	*/
		if( sLn_mst.ac_sts[0]!='4' )
		{
			sprintf(acErrMsg,"贷款状态非呆帐,不能核销![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"贷款状态非呆帐,不能核销!!!!");
			strcpy( g_pub_tx.reply,"L056" );
			goto ErrExit;
		}
		/* 检查是否可以完全归还本金 */
		if (-1 == pub_base_CompDblVal(sLn_mst.bal , sLn_auth.amt))	/* 输入金额过多 */ 
		{
			sprintf(acErrMsg,"输入[%.2f],实际余额[%.2f]",g_pub_tx.tx_amt1,sLn_mst.bal);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"输入金额过大!!!");
			strcpy( g_pub_tx.reply,"L039" );
			goto ErrExit;				
		}
	}
	if ( sLn_parm.ln_pay_type[0]!='3' && sLn_parm.ln_pay_type[0]!='4' && pub_base_CompDblVal(sLn_auth.amt,0.00)>0)/** 利息核销 **/
	{
		/* 检查主文件中欠息金额之和是否为0 del 20120628 郭总说不加这个判断了
		if ( !pub_base_CompDblVal(sLn_mst.in_lo_intst+sLn_mst.out_lo_intst+sLn_mst.cmpd_lo_intst , 0.00) )
		{
			sprintf(acErrMsg,"贷款欠息为0,不能核销![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"贷款欠息为0,不能核销!");
			strcpy( g_pub_tx.reply,"L074" );
			goto ErrExit;
		}*/	
		if ( pub_base_CompDblVal(sLn_mst.in_lo_intst+sLn_mst.out_lo_intst+sLn_mst.cmpd_lo_intst ,sLn_auth.intst_amt) )
		{
			sprintf(acErrMsg,"所欠利息必须一次性核销![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"所欠利息必须一次性核销!");
			strcpy( g_pub_tx.reply,"L232" );
			goto ErrExit;
		}
	}
	sLn_auth.sts[0] = 'X';
	memcpy(sLn_auth.type,"11",2);/** 本金+利息核销 **/
	memcpy(sLn_auth.cms_tx_br_no,g_pub_tx.tx_br_no,sizeof(sLn_auth.cms_tx_br_no)-1);
	iRet = Ln_auth_Ins(sLn_auth,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]插入审批表失败.请检查ret=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"插入审批表失败!!!");
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** 授权号码 **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," ln_auth ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," sts='X' and pact_no='%s' and type='11' ",sLn_auth.pact_no);
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
	sprintf( acErrMsg, "信贷核销审批成功!!!1");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "信贷核销审批失败[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
