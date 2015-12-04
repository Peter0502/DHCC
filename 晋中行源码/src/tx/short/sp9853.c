/*************************************************
* 文 件 名:  sp9853.c
* 功能描述： 由客户号和信息来源回显上单原因(黑名单)
*
* 作    者:  jane
* 完成日期： 2003年2月5日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_blklst_rel_c.h"

int sp9853()
{
	struct	cif_blklst_rel_c	sCifBlklstRel;		/* 客户黑名单关系 */
	int	ret;

	memset( &sCifBlklstRel, 0x00, sizeof( struct cif_blklst_rel_c ) );
	
	get_zd_long( "0280", &g_pub_tx.cif_no );		/* 客户号 */
    	get_zd_data( "0670", g_pub_tx.ct_ind );  		/* 信息来源 */

	/* 检查客户黑名单关系表中是否存在 */
       	ret = Cif_blklst_rel_Sel( g_pub_tx.reply, &sCifBlklstRel, "cif_no=%ld and blalist_origin='%s'", \
        			g_pub_tx.cif_no, g_pub_tx.ct_ind );             
        if( ret == 100 )
        {
		sprintf( acErrMsg, "查询客户黑名单关系表错，无记录!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "C059" );
		goto ErrExit;
	}else if( ret )
        {
            	sprintf( acErrMsg, "读取客户黑名单表异常![%d]", ret );
            	WRITEMSG
            	goto ErrExit;
        }
	
	/* 向前台屏幕赋值 */
	set_zd_data( "0810", sCifBlklstRel.rmk );		/* 上单原因 */

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


