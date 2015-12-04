/*************************************************
* 文 件 名:  sp9881.c
* 功能描述： 根据对方机构代码回显人行在我行内部账记账科目110(人行部分为现金收付所用)
*
* 作    者:  jane
* 完成日期： 2003年3月31日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"

int sp9881()
{
    	struct 	com_item_c 	sCom_item_c;		/* 公共科目表 */
    	char	cTw_br_no[2];				/* 对方机构代码 */
    	int ret;

    	memset( &sCom_item_c, 0x00, sizeof(struct com_item_c) );
    	memset( &cTw_br_no, 0x00, sizeof(cTw_br_no) );

    	get_zd_data( "0680", cTw_br_no );

	if( cTw_br_no[0] == '1' )			/* 人行 */
		set_zd_data( "0300", "110"); 		/* 客户账号 */
	else
		set_zd_data( "0300", ""); 		/* 客户账号 */

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
