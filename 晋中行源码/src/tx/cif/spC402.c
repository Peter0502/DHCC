/*************************************************
* 文 件 名: spC402.c
* 功能描述：删除客户与客户经理间联系
* 
* 作    者: andy
* 完成日期：2004年2月5日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_mger_rel_c.h"

spC402()
{
	struct 	cif_mger_rel_c		sCifMgerRel;		/* 客户-客户经理关系表 */
	char	comm[100];					/* 删除表记录的where条件 */
	int 	ret;

    memset ( &sCifMgerRel, 0x00, sizeof( struct cif_mger_rel_c ) );
    memset( comm, 0x00, sizeof( comm ) );
        
    /* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
    get_zd_long( "0290", &sCifMgerRel.cif_no );		/* 客户代码 */
    get_zd_data( "0920", sCifMgerRel.mang );    		/* 经理代码 */
    get_zd_data( "0700", sCifMgerRel.mang_type );      	/* 客户经理类型 */

   	/* 检查客户-客户经理关系表中是否存在相关记录 */
   	ret = Cif_mger_rel_Dec_Upd( g_pub_tx.reply, "cif_no=%ld and mang='%s' \
       				and mang_type='%s' and canl_date=%ld", sCifMgerRel.cif_no, \
				sCifMgerRel.mang, sCifMgerRel.mang_type, 0 );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Cif_mger_rel_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Cif_mger_rel_Fet_Upd( &sCifMgerRel, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "客户-客户经理关系表中不存在相关记录!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "C054" );
        	goto ErrExit;
    	}
	else if( ret )
   	{
   		sprintf( acErrMsg, "执行Cif_mger_rel_Dec_Upd错![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
   	}

	/* 修改客户-客户经理关系表 */
	sCifMgerRel.canl_date = g_pub_tx.tx_date;		/* 解除日期 */

	ret = Cif_mger_rel_Upd_Upd( sCifMgerRel, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改客户-客户经理关系表错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Cif_mger_rel_Clo_Upd();

    	/* 记流水日志 */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"个人客户信息录入成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"个人客户信息录入失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
