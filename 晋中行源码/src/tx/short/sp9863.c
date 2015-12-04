/*************************************************
* 文 件 名:  sp9863.c
* 功能描述： 由客户帐号回显户名、帐户类型及余额
*
* 作    者:  jane
* 完成日期： 2003年3月9日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9863()
{
	struct	mdm_ac_rel_c	sMdmAcRel;	/* 介质与账户关系表 */
	struct	dd_mst_c	sDdMst;		/* 活期存款主文件 */
	int	ret;

	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );

	get_zd_data( "0830", g_pub_tx.ac_no );		/* 客户帐号 */
	get_zd_data( "0670", g_pub_tx.ct_ind );		/* 收款账户类型 */
	get_zd_data( "0250", g_pub_tx.name );		/* 收款账户户名 */

	pub_base_old_acno( g_pub_tx.ac_no );   	/* 对旧帐号的处理 */

	/*if( strcmp( g_pub_tx.ac_no, "" )  == 0 )	 为满足有现转标志情况所加 *
	{
		set_zd_data( "0250", "" );	* 户名 *
		set_zd_data( "0670", "" );	* 帐户类型 *
		set_zd_double( "1002", 0 );	* 还款账号余额 *
                goto	OkExit;
	}*/

	if( g_pub_tx.ct_ind[0] == '1' )
	{
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
		set_zd_double( "1002", sDdMst.bal );	/* 还款账号余额 */
	}
	else
	{
		set_zd_data( "0250", g_pub_tx.name );	/* 户名 */
	}
	set_zd_data("0830",g_pub_tx.ac_no);		/* 处理旧帐号 */
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




