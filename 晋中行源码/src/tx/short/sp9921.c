/*************************************************
* 文 件 名:  sp9921.c
* 功能描述： 由客户帐号和帐户类型回显户名
*
* 作    者:  jane
* 完成日期： 2003年6月15日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9921()
{
	struct	mdm_ac_rel_c	sMdmAcRel;	/* 介质与账户关系表 */
	struct	dd_mst_c	sDdMst;		/* 活期存款主文件 */
	int	ret;

	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );

	get_zd_data( "0830", g_pub_tx.ac_no );		/* 客户帐号 */

	/* 查询客户帐号是否为本行帐号 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				g_pub_tx.ac_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此客户帐号不存在![%s]", g_pub_tx.ac_no );
    		WRITEMSG
    		strcpy( g_pub_tx.reply, "L113" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
	}

	/* 根据客户帐号ID和帐户序号查询活期存款主文件 */
	ret =Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此客户帐号非活期![%s]", g_pub_tx.ac_no );
    		WRITEMSG
    		strcpy( g_pub_tx.reply, "B114" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
	}

	/* 向前台屏幕赋值 */
	set_zd_data( "0250", sDdMst.name );	/* 户名 */

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




