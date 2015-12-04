/*************************************************
* 文 件 名:  sp9901.c
* 功能描述： 由现金收付标志回显现金分析号和现金用途
*
* 作    者:  jane
* 完成日期： 2003年4月27日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9901()
{
	char	cOperCode[2];				/* 操作码 */	

	memset( cOperCode, 0x00, sizeof( cOperCode ) );

	get_zd_data( "0670", cOperCode );		/* 操作码 */

	if( strcmp( cOperCode, "0" ) == 0 )		/* 付出 */
	{
		set_zd_data( "1185", "220" );		/* 现金分析号 */
		set_zd_data( "1186", "23" );		/* 用途 */
	}
	else						/* 收入 */
	{
		set_zd_data( "1185", "120" );		/* 现金分析号 */
		set_zd_data( "1186", "22" );		/* 用途 */
	}

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
