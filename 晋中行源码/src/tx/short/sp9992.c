/*************************************************************
* 文 件 名:  sp9992.c
* 功能描述： 本票签发根据业务编号反显录入信息
*
* 作    者:  jane
* 完成日期： 2003年11月08日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "mo_cashier_c.h"
#include "com_branch_c.h"

int sp9992()
{
	struct mo_cashier_c mo_cashier_v;
	struct com_branch_c com_branch_v;

	memset( &mo_cashier_v, 0x00, sizeof( struct mo_cashier_c ) );
	memset( &com_branch_v, 0x00, sizeof( struct com_branch_c ) );

	pub_base_sysinit();

	get_zd_data( "0730", mo_cashier_v.pact_no );

	g_reply_int = Mo_cashier_Sel( g_pub_tx.reply, &mo_cashier_v, 
		"pact_no='%s' and cashier_sts!='*'", mo_cashier_v.pact_no );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "没有登记该笔业务信息!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "查询本票登记簿数据库操作出错[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

		if( mo_cashier_v.cashier_sts[0] == '0' )
		NULL;
		else if(mo_cashier_v.cashier_sts[0]=='1')
		{
			sprintf( acErrMsg, "已签发本票，本票号码：【%s】", mo_cashier_v.cashier_no );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else
		{
			sprintf( acErrMsg, "该笔录入信息状态不正常,请联系中心机房![%s]",mo_cashier_v.cashier_sts );
			MBFEWRITEMSG
			goto ErrExit;
		}

	g_reply_int = Com_branch_Sel( g_pub_tx.reply, &com_branch_v, 
		"br_no='%s'", mo_cashier_v.tx_br_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询录入机构名称出错![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0910", mo_cashier_v.tx_br_no );
	set_zd_data( "0620", com_branch_v.br_name );
	set_zd_data( "0630", mo_cashier_v.rmk );
	set_zd_long( "0460", mo_cashier_v.tx_date );
	set_zd_double( "0390", mo_cashier_v.sign_amt );
	set_zd_data( "0210", mo_cashier_v.ct_ind );
	set_zd_data( "0690", mo_cashier_v.ration_ind );
	set_zd_data( "0300", mo_cashier_v.requ_ac_no );
	set_zd_data( "0250", mo_cashier_v.requ_name );
	set_zd_data( "0260", mo_cashier_v.requ_addr );
	set_zd_data( "0810", mo_cashier_v.payee_ac_no );
	set_zd_data( "0961", mo_cashier_v.payee_name );

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
