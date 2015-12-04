/*************************************************
* 文 件 名:  spB338.c
* 功能描述： 本行企业拿他行的承兑汇票来抵押,对他行的承兑汇票上的内容进行登记 对应前台5621
*
* 作    者:  liuxuan
* 完成日期： 2006年10月7日
*
* 修改记录： 
*   日   期: 
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_dep_reg_c.h"
#include "mo_bank_split_po_c.h"
#include "mo_po_co_c.h"
#include "whwtsk_reg_c.h"
#include "prdt_ac_id_c.h"
#include "dd_parm_c.h" 
#include "dc_acc_rel_c.h"
#include "mdm_ac_rel_c.h"

int spB338()
{
	char	cBailAcNo[25];				/* 保证金帐号 */
	int		ret, num;
	char	Tmp_flag[2];
	struct whwtsk_reg_c sWhwtsk_reg,tmp;
	char    opr[2];
	struct  prdt_ac_id_c sPrdt_ac_id;
	struct  dd_parm_c sDd_parm;
	struct  dc_acc_rel_c sDc_acc_rel;
	struct  mdm_ac_rel_c sMdm_ac_rel;

	memset( &sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset( &sDc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
	memset( &sDd_parm,0x00,sizeof(struct dd_parm_c));
	memset( &sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset( &opr,0x0,sizeof(opr));  
	memset( &sWhwtsk_reg,0x00,sizeof(struct whwtsk_reg_c));
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );
	memset( &tmp,0x00,sizeof(struct whwtsk_reg_c));

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data("0680",opr);
	get_zd_data("0330",sWhwtsk_reg.pact_no);
	pub_base_strpack( sWhwtsk_reg.pact_no );
		/* 从前台屏幕取值 */
		strcpy( sWhwtsk_reg.tx_br_no, g_pub_tx.tx_br_no );/* 交易机构 */
		get_zd_long( "0450", &sWhwtsk_reg.req_date);/* 申请日期 在这里作为起息日 */
		get_zd_data( "0260", sWhwtsk_reg.payer_name );	/* 付款人全称 */
		get_zd_data( "0250", sWhwtsk_reg.payer_br_name );/* 付款人开户行行名 */
		get_zd_data( "0270", sWhwtsk_reg.payee_name );	/* 收款人全称 */
		get_zd_data( "0830", sWhwtsk_reg.payee_ac_no );	/* 收款人账号 */
		get_zd_data( "0810", sWhwtsk_reg.payee_br_name );/* 收款人开户行行名 */
		get_zd_double( "1001", &sWhwtsk_reg.sign_amt );	/* 签发票面金额 */
		get_zd_long( "0440", &sWhwtsk_reg.matr_date );	/* 到期日期 */
		get_zd_double( "1002", &sWhwtsk_reg.tot_mort_amt );/* 质押金额 */
		get_zd_double( "0400", &sWhwtsk_reg.bail_amt_ratio );/* 保证金比例 */
		strcpy( sWhwtsk_reg.acc_po_ind, "0" );	/* 汇票状态: 0未到期处理*/
		get_zd_data( "0380", sWhwtsk_reg.payer_ac_no );	/* 付款人帐号 */
		get_zd_double( "1003", &sWhwtsk_reg.bail_amt );	/* 保证金金额 */
		get_zd_data( "0670", sWhwtsk_reg.acc_type );	/* 承兑类型 */
		get_zd_data( "0320", sWhwtsk_reg.bail_ac_no);/* 在本交易里作为本行企业帐号 */
		strcpy( sWhwtsk_reg.acc_po_sts,"9");/* 用来表示外行过来的质押票 */
	vtcp_log("协议号是[%s],票面金额:[%lf]",sWhwtsk_reg.pact_no,sWhwtsk_reg.sign_amt );
	
	strncpy(g_pub_tx.ac_no,"717",3);
	strncpy(g_pub_tx.ct_ind,"2",1);
	
	if(opr[0]=='1') /*登记*/
	{
vtcp_log("进入登记操作:opr=[%s] [%s]--[%d]",opr,__FILE__,__LINE__);
		ret=Whwtsk_reg_Ins(sWhwtsk_reg,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"修改承兑汇票基本信息登记表出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0210","1"); /*币种*/
		set_zd_data("0660","2"); /*1,借 2,贷 登记  记贷*/
		set_zd_data("0670","2"); /*1,现 2 转*/
		set_zd_double("0400",sWhwtsk_reg.sign_amt); /*发生额*/
	strncpy(g_pub_tx.add_ind,"1",1);
	}
	else if(opr[0]=='2') /*核销*/
	{
vtcp_log("进入核销的操作:opr=[%s] [%s]--[%d]",opr,__FILE__,__LINE__);
		ret=Whwtsk_reg_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",sWhwtsk_reg.pact_no);
		if(ret && ret != 100)
		{
			sprintf(acErrMsg,"查询whwtsk_reg表出错![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
vtcp_log("进入核销Dec_Upd:=[%s] [%s]--[%d]",sWhwtsk_reg.pact_no,__FILE__,__LINE__);
		ret=Whwtsk_reg_Fet_Upd(&sWhwtsk_reg,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"查询whwtsk_reg表出错![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		/* 从前台屏幕取值 */
		strcpy( sWhwtsk_reg.tx_br_no, g_pub_tx.tx_br_no );/* 交易机构 */
		get_zd_long( "0450", &sWhwtsk_reg.req_date);/* 申请日期 在这里作为起息日 */
		get_zd_data( "0260", sWhwtsk_reg.payer_name );	/* 付款人全称 */
		get_zd_data( "0250", sWhwtsk_reg.payer_br_name );/* 付款人开户行行名 */
		get_zd_data( "0270", sWhwtsk_reg.payee_name );	/* 收款人全称 */
		get_zd_data( "0830", sWhwtsk_reg.payee_ac_no );	/* 收款人账号 */
		get_zd_data( "0810", sWhwtsk_reg.payee_br_name );/* 收款人开户行行名 */
		get_zd_double( "1001", &sWhwtsk_reg.sign_amt );	/* 签发票面金额 */
		vtcp_log("票面金额[%.0f]\n",sWhwtsk_reg.sign_amt);
		get_zd_long( "0440", &sWhwtsk_reg.matr_date );	/* 到期日期 */
		get_zd_double( "1002", &sWhwtsk_reg.tot_mort_amt );/* 质押金额 */
		get_zd_double( "0400", &sWhwtsk_reg.bail_amt_ratio );/* 保证金比例 */
		strcpy( sWhwtsk_reg.acc_po_ind, "0" );	/* 汇票状态: 0未到期处理*/
		get_zd_data( "0380", sWhwtsk_reg.payer_ac_no );	/* 付款人帐号 */
		get_zd_double( "1003", &sWhwtsk_reg.bail_amt );	/* 保证金金额 */
		get_zd_data( "0670", sWhwtsk_reg.acc_type );	/* 承兑类型 */
		get_zd_data( "0320", sWhwtsk_reg.bail_ac_no);/* 在本交易里作为本行企业帐号 */
		strcpy( sWhwtsk_reg.acc_po_sts,"9");/* 用来表示外行过来的质押票 */
		
		ret=Whwtsk_reg_Upd_Upd(sWhwtsk_reg,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"更新表出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		Whwtsk_reg_Clo_Upd();
		
		set_zd_data("0210","1"); /*币种*/
		set_zd_data("0660","1"); /*1,借 2,贷 核销记借*/
		set_zd_data("0670","2"); /*1,现 2 转*/
		set_zd_double("0400",sWhwtsk_reg.sign_amt); /*发生额*/
	strncpy(g_pub_tx.add_ind,"0",1);
	}		
	ret=pub_acct_trance();
	if(ret)
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}


	/* 为记流水赋值 */
	strcpy( g_pub_tx.brf, "质押承兑汇票信息录入" );	/* 备注 */

	/* 记流水日志 */
	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sWhwtsk_reg.pact_no);
	g_pub_tx.tx_amt1=0.00;
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}


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
