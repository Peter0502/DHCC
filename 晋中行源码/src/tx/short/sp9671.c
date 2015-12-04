/*************************************************************
* 文 件 名:  sp9671.c
* 功能描述： 授权之柜员显示名称
*
*************************************************************/

#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int sp9671()
{
	struct com_tel_c com_tel_v;
	char brno[6];
	char tel[5];

	pub_base_sysinit();

	get_zd_data( "0910", brno );
	get_zd_data( "0920", tel );

	if( !strlen(tel) ) goto OkExit;

	g_reply_int = Com_tel_Sel( g_pub_tx.reply, &com_tel_v, 
		"tel='%s'", tel );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询录入机构名称出错![%d]", g_reply_int );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S019" );
		goto ErrExit;
	}
	if( com_tel_v.csts[0]=='3' )
	{
		WRITEMSG
		strcpy( g_pub_tx.reply,"S012" );
		goto ErrExit;
	}
	if( com_tel_v.csts[0]=='4' )
	{
		WRITEMSG
		strcpy( g_pub_tx.reply,"S011" );
		goto ErrExit;
	}
	if( strncmp(com_tel_v.br_no,brno,5) )
	{
		sprintf( acErrMsg, "查询录入机构出错![%s][%s]",com_tel_v.br_no,brno );
		WRITEMSG
		strcpy( g_pub_tx.reply,"B273" );
		goto ErrExit;
	}

	set_zd_data( "0580", com_tel_v.name );

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
