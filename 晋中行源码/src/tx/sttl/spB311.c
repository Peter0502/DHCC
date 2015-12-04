/***************************************************************
* 文 件 名: spB311.c
* 功能描述：本票签发信息录入子交易
*           登记本票签发登记簿(mo_cashier)
*
* 作    者: jane
* 完成日期：2003年11月07日
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
#include "com_parm_c.h"
#include "mo_cashier_c.h"

spB311()
{
	struct mo_cashier_c sMoCashier;
	struct com_parm_c com_parm_v;

	memset( &sMoCashier, 0x00, sizeof( struct mo_cashier_c ) );

	pub_base_sysinit();


	if( get_serialno_val( "14", sMoCashier.pact_no ) ) goto ErrExit;    /*业务编号*/
	strcpy( sMoCashier.tx_br_no, g_pub_tx.tx_br_no );    /*录入机构号*/
	sMoCashier.tx_date = g_pub_tx.tx_date;    /*录入日期*/
	get_zd_data( "0210", sMoCashier.ct_ind );    /*现转类型*/
	get_zd_data( "0690", sMoCashier.ration_ind );    /*定额类型*/
	get_zd_data( "0300", sMoCashier.requ_ac_no );    /*申请人帐号*/
	get_zd_data( "0250", sMoCashier.requ_name );    /*申请人名称*/
	get_zd_data( "0962", sMoCashier.requ_addr );    /*申请人地址*/
	get_zd_data( "0810", sMoCashier.payee_ac_no );    /*收款人帐号*/
	get_zd_data( "0961", sMoCashier.payee_name );    /*收款人名称*/
	get_zd_double( "0390", &sMoCashier.sign_amt );    /*出票金额*/
	get_zd_data( "0370", sMoCashier.rmk );    /*备注*/
	sMoCashier.rec_trace_no = g_pub_tx.trace_no;    /*录入流水号*/
	strcpy( sMoCashier.rec_tel, g_pub_tx.tel );    /*录入操作员*/
	strcpy( sMoCashier.cashier_sts, "0" );    /*本票状态*/
	strcpy( sMoCashier.use_ind, "0" );    /*核销状态*/
	strcpy( sMoCashier.los_rpt_ind, "0" );    /*挂失状态*/

	sprintf( acErrMsg, "mo_cashier: N pact_no[%s] N tx_br_no[%s] tx_date[%ld] sign_amt[%lf] ct_ind[%s] ration_ind[%s] N requ_ac_no[%s] N requ_name[%s] requ_addr[%s] payee_ac_no[%s] payee_name[%s] rmk[%s] rec_trace_no[%ld] rec_tel[%s] N cashier_sts[%s] N use_ind[%s] N los_rpt_ind[%s]", 
		sMoCashier.pact_no, sMoCashier.tx_br_no, sMoCashier.tx_date, 
		sMoCashier.sign_amt, sMoCashier.ct_ind, sMoCashier.ration_ind, 
		sMoCashier.requ_ac_no, sMoCashier.requ_name, sMoCashier.requ_addr, 
		sMoCashier.payee_ac_no, sMoCashier.payee_name, sMoCashier.rmk, 
		sMoCashier.rec_trace_no, sMoCashier.rec_tel, sMoCashier.cashier_sts, 
		sMoCashier.use_ind, sMoCashier.los_rpt_ind );
	WRITEMSG

	g_reply_int = Mo_cashier_Ins( sMoCashier, g_pub_tx.reply );
	if( g_reply_int )
   	{
       	sprintf( acErrMsg, "登记本票签发登记簿错误![%d]", g_reply_int );
       	WRITEMSG
       	goto ErrExit;
   	}

	set_zd_data( "0730", sMoCashier.pact_no );

	/*记账子交易传值*/
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='BPKM' and parm_seqn='1'" );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询公共参数表出错[%d]!", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data( "1211", com_parm_v.val );    /*账号*/

	/*记交易日志为撤销用*/
	strcpy( g_pub_tx.no_show, "1" );/*隐形*/
	strcpy( g_pub_tx.ac_no, sMoCashier.pact_no );
	strcpy( g_pub_tx.add_ind, "1" );
	strcpy( g_pub_tx.brf, "本票信息录入" );
	g_reply_int = pub_ins_trace_log();
	if( g_reply_int )
	{
		sprintf( acErrMsg," 记交易日志错误![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"本票信息录入成功![%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"本票信息录入失败![%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
