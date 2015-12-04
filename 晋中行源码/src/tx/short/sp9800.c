/*************************************************
* 文 件 名:  sp9800.c
* 功能描述： 
*
* 作    者:  jack
* 完成日期： 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "auth_code_c.h"
#include "tx_log_c.h"

int sp9800()
{
	struct auth_code_c vauth_code;
	struct tx_log_c vtx_log;
	int ret=0;
TRACE
	get_zd_long( "0330" , &vauth_code.auth_no );	/*  授权号码 */
vtcp_log("AUTH_CODE[%d]",vauth_code.auth_no );
	set_zd_long( DC_AUTH_CODE , vauth_code.auth_no );

vtcp_log("AUTH_CODE[%d]",vauth_code.auth_no );
TRACE
	ret = Auth_code_Sel( g_pub_tx.reply,&vauth_code,
		" auth_no=%d ",vauth_code.auth_no );
	if( ret )
	{
		sprintf( acErrMsg,"授权码不存在[%d]",vauth_code.auth_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z101" );
		goto ErrExit;
	}
TRACE
	ret = Tx_log_Sel( g_pub_tx.reply,&vtx_log,
		" trace_no=%d ",vauth_code.trace_no );
	if( ret )
	{
		sprintf( acErrMsg,"交易不存在[%d]",vauth_code.trace_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z106" );
		goto ErrExit;
	}
TRACE

	ret = pub_comm_chan_oldpack( vtx_log.bit_map,vtx_log.tx_pack );
	if( ret )
	{
		sprintf( acErrMsg,"error" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z106" );
		goto ErrExit;
	}

TRACE
	ret = pub_comm_show_pack( vtx_log.bit_map );
	if( ret )
	{
		sprintf( acErrMsg,"error" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z106" );
		goto ErrExit;
	}
TRACE

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
