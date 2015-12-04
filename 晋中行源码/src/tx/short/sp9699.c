/*************************************************
* 文 件 名:  sp9699.c
* 功能描述： 授权人返回 for SQ01
*
* 作    者:  rob
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

int sp9699()
{
	struct auth_code_c vauth_code;
	char vtxcode[5];
	int ret=0;

	get_zd_long( DC_AUTH_CODE , &g_pub_tx.auth_code );
	get_zd_data( DC_TEL , g_pub_tx.tel );
	get_zd_data( DC_AUTH , g_pub_tx.auth );
	get_zd_data( "0920" , vtxcode );

	vtcp_log("AUTH_CODE[%d]",g_pub_tx.auth_code );
	vtcp_log("TEL[%s]",g_pub_tx.tel );
	vtcp_log("AUTH[%s]",g_pub_tx.auth );
	vtcp_log("TXCODE[%s]",vtxcode );

	ret = Auth_code_Sel( g_pub_tx.reply,&vauth_code,
		" auth_no=%d ",g_pub_tx.auth_code );
	if( ret )
	{
		sprintf( acErrMsg,"授权码不存在[%d]",g_pub_tx.auth_code );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z101" );
		goto ErrExit;
	}

	if( vauth_code.sts[0]=='9' )
	{
		sprintf( acErrMsg,"此流水号对应的交易已经完成" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z104" );
		goto ErrExit;
	}
	else if( vauth_code.sts[0]=='4' )
	{
		sprintf( acErrMsg,"此交易需要复核，请按F11进行复核" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z10G" );
		goto ErrExit;
	}
	else if( vauth_code.sts[0]=='3' )
	{
		if( strcmp(g_pub_tx.tel,vauth_code.tel) )
		{
			sprintf( acErrMsg,"此交易已经授权了[%s] [%s]",
				g_pub_tx.tel,vauth_code.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply,"Z10J" );
			goto ErrExit;
		}
	}
	if( strcmp(vtxcode,vauth_code.tx_code) )
	{
		sprintf( acErrMsg,"流水号不是这个交易[%s] [%s]",
			vtxcode,vauth_code.tx_code);
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z10C" );
		goto ErrExit;
	}
		sprintf( acErrMsg,"授权存在[%s]",vauth_code.auth );
		WRITEMSG
	set_zd_data( DC_AUTH,vauth_code.auth );

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
