/*************************************************************
* 文 件 名:  sp9670.c
* 功能描述： 授权之机构显示名称
*
*************************************************************/

#define EXTERN
#include "public.h"
#include "com_branch_c.h"

int sp9670()
{
	struct com_branch_c com_branch_v;
	char brno[6];

	memset( &com_branch_v, 0x00, sizeof( struct com_branch_c ) );

	pub_base_sysinit();

	get_zd_data( "0910", brno );
	if( !strlen(brno) ) goto OkExit;

	g_reply_int = Com_branch_Sel( g_pub_tx.reply, &com_branch_v, 
		"br_no='%s'", brno );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询录入机构名称出错![%d]", g_reply_int );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S019" );
		goto ErrExit;
	}

	set_zd_data( "0260", com_branch_v.br_name );

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
