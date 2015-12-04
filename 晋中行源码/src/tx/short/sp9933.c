/*************************************************************
* 文 件 名:  sp9933.c
* 功能描述： 大额支付快捷查询
*			 汇票信息录入汇票检查
*
* 作    者:  jane
* 完成日期： 2003年6月21日
*
* 修改记录： 
*   日   期: 2003年7月28日
*   修 改 人:jane
*   修改内容:增加判断只有信息录入行才能修改和删除该笔业务信息
*            详尽提示信息
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "com_parm_c.h"

int sp9933()
{
	struct mbfebillopnreg_c mbfebillopnreg_v;

	memset( &mbfebillopnreg_v, 0x00, sizeof( struct mbfebillopnreg_c ) );

	pub_base_sysinit();

	get_zd_data( "0860", mbfebillopnreg_v.pact_no );

	g_reply_int = Mbfebillopnreg_Sel( g_pub_tx.reply, &mbfebillopnreg_v,
		"pact_no='%s' and tx_br_no='%s' and billsts[1]!='*'", 
		mbfebillopnreg_v.pact_no, g_pub_tx.tx_br_no );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "没有登记该笔业务信息或该笔业务已签发汇票并核销!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "查询汇票登记簿数据库操作出错[%d]", g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( strcmp( mbfebillopnreg_v.tx_br_no, g_pub_tx.tx_br_no ) )
	{
		sprintf( acErrMsg, "非本行录入的业务信息!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( mbfebillopnreg_v.billsts[0] == '0' )
	;
	else if( mbfebillopnreg_v.billsts[0] == '1' )
	{
		sprintf( acErrMsg, "已经签发汇票!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( mbfebillopnreg_v.billsts[0] == '2' )
	{
		sprintf( acErrMsg, "已经签发并移存汇票!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else
	{
		sprintf( acErrMsg, "汇票状态异常!请联系中心机房" );
		MBFEWRITEMSG
		vtcp_log( "汇票状态异常!业务编号[%s]", mbfebillopnreg_v.pact_no );
		goto ErrExit;
	}

	set_zd_double( "0390", mbfebillopnreg_v.amount );
	set_zd_data( "0210", mbfebillopnreg_v.billtype );
	set_zd_data( "0300", mbfebillopnreg_v.payeracc );
	set_zd_data( "0250", mbfebillopnreg_v.payername );
	set_zd_data( "0962", mbfebillopnreg_v.payeraddr );
	set_zd_data( "0810", mbfebillopnreg_v.recipientacc );
	set_zd_data( "0961", mbfebillopnreg_v.recipientname );
	set_zd_data( "0590", mbfebillopnreg_v.cashbillbkno );
	set_zd_data( "0370", mbfebillopnreg_v.brf );

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
