/***************************************************************
* 文 件 名: spB310.c
* 功能描述：本票核销子交易(判断是否有多余金额)
*
* 作    者: jane
* 完成日期：2003年11月09日
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
#include "mbfe_pub.h"
#include "mo_cashier_c.h"

spB310()
{
	struct mo_cashier_c sMoCashier;

	memset( &sMoCashier, 0x00, sizeof( struct mo_cashier_c ) );

	get_zd_data( "0580", sMoCashier.cashier_no );
	get_zd_double( "0410", &sMoCashier.rem_amt );

	g_reply_int = pub_base_CompDblVal( sMoCashier.rem_amt, 0.00 );
	if( g_reply_int == 0 )
	{
		strcpy( g_pub_tx.reply, "0000" );
		goto OkExit;
	}

	vtcp_log( "本票结清多余金额[%lf]", sMoCashier.rem_amt );

	get_zd_data( "0320", sMoCashier.rem_ac_no );
	pub_base_strpack( sMoCashier.rem_ac_no );
	if( strlen( sMoCashier.rem_ac_no ) == 0 )
	{
		g_reply_int = Mo_cashier_Sel( g_pub_tx.reply, &sMoCashier, "cashier_no='%s'", sMoCashier.cashier_no );
		if( g_reply_int != 0 )
		{
			sprintf( acErrMsg, "查询本票登记簿出错![%d]", g_reply_int );
			MBFEWRITEMSG
			goto ErrExit;
		}
		strcpy( sMoCashier.rem_ac_no, sMoCashier.requ_ac_no );
	}

	set_zd_data( "0191", "1" );
	set_zd_data( "0192", "0001" );
	set_zd_data( "0193", "1001" );
	set_zd_data( "1011", sMoCashier.rem_ac_no );
	set_zd_double( "1013", sMoCashier.rem_amt );
	set_zd_data( "1016", "本票多余退回" );
	set_zd_data( "101A", "01" );
	set_zd_data( "101B", "2" );

	strcpy( g_pub_tx.reply, "0001" );

OkExit:
	sprintf(acErrMsg,"更新本票签发登记簿成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"更新本票签发登记簿失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
