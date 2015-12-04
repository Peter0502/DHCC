/*************************************************
* 文 件 名:  sp9855.c
* 功能描述： 由产品编号回显期限类型（内部产品参数表）
*
* 作    者:  jane
* 完成日期： 2003年2月20日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9855()
{
	struct	in_parm_c sInParm;	/* 内部产品参数表 */
	int	ret;

	memset( &sInParm, 0x00, sizeof( struct in_parm_c ) );
	
	get_zd_data( "0890", g_pub_tx.prdt_code );		/* 产品编号 */

	/* 检查产品编号是否存在 */
	ret = In_parm_Sel( g_pub_tx.reply, &sInParm, "prdt_no='%s'", g_pub_tx.prdt_code );
	if( ret == 100 )
        {
		sprintf( acErrMsg, "无此产品编号![%s]", g_pub_tx.prdt_code );
            	WRITEMSG
            	strcpy( g_pub_tx.reply, "A028" );
		goto ErrExit;
        }else if( ret )
        {
            	sprintf( acErrMsg, "执行In_parm_Sel错![%d]", ret );
            	WRITEMSG
            	goto ErrExit;
        }

	/* 向前台屏幕赋值 */
	if( sInParm.term_type[0] == 'Y' )			/* 期限类型 */
		set_zd_data( "0210", "年" );
	else if( sInParm.term_type[0] == 'M' )
		set_zd_data( "0210", "月" );
	else if( sInParm.term_type[0] == 'Q' )
		set_zd_data( "0210", "季" );
	else	set_zd_data( "0210", "日" );

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




