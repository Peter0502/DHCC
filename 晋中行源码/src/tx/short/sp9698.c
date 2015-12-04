/*************************************************
* 文 件 名:  sp9698.c
* 功能描述： 复核码快查 for SQ02
*
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "auth_code_c.h"
#include "auth_chk_def_c.h"

int sp9698()
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
		if( !strcmp(g_pub_tx.tel,vauth_code.tel) )
		{
			sprintf( acErrMsg,"不能为自己复核[%s] [%s]",
				g_pub_tx.tel,vauth_code.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply,"Z102" );
			goto ErrExit;
		}
	}
	else if( vauth_code.sts[0]=='3' )
	{
		sprintf( acErrMsg,"此交易授权了，不需要复核[%s] [%s]",
			g_pub_tx.tel,vauth_code.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z10H" );
		goto ErrExit;
	}
	else if( vauth_code.sts[0]=='0' )
	{
		sprintf( acErrMsg,"此交易已经授权了[%s] [%s]",
			g_pub_tx.tel,vauth_code.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z10I" );
		goto ErrExit;
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
	if( sub_chk_fied( vtxcode ) )
		goto ErrExit;

	set_zd_data( DC_AUTH,vauth_code.tel );

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
int sub_chk_fied( char *tx_code )
{
	int ret=0;
	char reply[5];
	struct auth_chk_def_c auth_chk_def_c;
	char rtstr[101];
	int i=0;

	ret = Auth_chk_def_Dec_Sel( reply , "tx_code='%s'" , tx_code );
	if (ret)
	{
		strcpy( g_pub_tx.reply,"H063" );
		WRITEMSG
		goto ErrExit;
	}

	i=0;
	memset( rtstr,0,sizeof(rtstr) );
	while( 1 )
	{
		ret = Auth_chk_def_Fet_Sel( &auth_chk_def_c , reply );
		if( ret==100 ) break;
		if (ret)
		{
			strcpy( g_pub_tx.reply,"H063" );
			WRITEMSG
			goto ErrExit;
		}
		i++;
		if( i>15 ) break;
		strcat(rtstr,auth_chk_def_c.data_code);
		strcat(rtstr,",");
	}

	Auth_chk_def_Clo_Sel();

vtcp_log(" CHECK 0760 [%s]",rtstr );
	set_zd_data("0760",rtstr);

	return 0;
ErrExit:
	return 1;
}
