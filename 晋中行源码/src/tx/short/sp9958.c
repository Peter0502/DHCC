/*****************************************************
* 文 件 名:  sp9958.c
* 功能描述： 旧账号转新账号
*
* 作    者:  jane
* 完成日期： 2003年7月23日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "mbfe_pub.h"

int sp9958()
{
	char ac_no[21];

	memset( ac_no, 0x00, sizeof( ac_no ) );

	get_zd_data( "0380", ac_no );
	if( strlen( ac_no ) )
	{
		pub_base_old_acno( ac_no );
		set_zd_data( "0380", ac_no );
		goto OkExit;
	}

	get_zd_data( "0370", ac_no );
	if( strlen( ac_no ) )
	{
		pub_base_old_acno( ac_no );
		set_zd_data( "0370", ac_no );
		goto OkExit;
	}

	get_zd_data( "0300", ac_no );
	if( strlen( ac_no ) )
	{
		pub_base_old_acno( ac_no );
		set_zd_data( "0300", ac_no );
		goto OkExit;
	}

	get_zd_data( "0310", ac_no );
	if( strlen( ac_no ) )
	{
		pub_base_old_acno( ac_no );
		set_zd_data( "0310", ac_no );
		goto OkExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
