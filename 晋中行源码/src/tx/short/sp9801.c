/*************************************************
* 文 件 名:  sp9801.c
* 功能描述： 由账号回显户名余额
*
* 作    者:  jack
* 完成日期： 
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9801()
{
	get_zd_data( "0300", g_pub_tx.ac_no );			/* 帐号 */

	set_zd_data( DC_FILE_SND,"1" );
	set_zd_double( "0400",800.00 );

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
