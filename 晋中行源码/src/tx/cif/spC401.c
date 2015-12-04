/*************************************************
* 文 件 名: spC401.c
* 功能描述：维护客户与客户间关系
* 
* 作    者: andy
* 完成日期：2004年2月4日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_cif_rel_c.h"

spC401()
{
	struct 	cif_cif_rel_c     sCifCifRel;		/* 客户-客户关系表 */
	char	cOptcode[2];				/* 操作码 */
	char	comm[100];				/* 删除表记录的where条件 */
	char	cRelCode[4];				/* 客户关系代码 */
	int 	ret;

        memset( &sCifCifRel, 0x00, sizeof( struct cif_cif_rel_c ) );
        memset( cOptcode, 0x00, sizeof( cOptcode ) );
        memset( comm, 0x00, sizeof( comm ) );
        memset( cRelCode, 0x00, sizeof( cRelCode ) );
        
        /* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
        get_zd_data( "0670", cOptcode );       			/* 操作码: 1修改 2删除 */
        get_zd_long( "0290", &sCifCifRel.cif_no );		/* 客户ID */
        get_zd_long( "0280", &sCifCifRel.rel_cid_no );    	/* 关联客户号 */
        get_zd_data( "0230", cRelCode );      			/* 客户关系代码 */

       	/* 如果是修改操作 */
       	if( cOptcode[0] == '1' )
       	{
		/* 检查客户-客户关系表 */
		ret = Cif_cif_rel_Dec_Upd( g_pub_tx.reply , "cif_no=%ld and rel_cid_no=%ld", \
					sCifCifRel.cif_no, sCifCifRel.rel_cid_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Cif_cif_rel_Dec_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* 取出记录 */
		ret = Cif_cif_rel_Fet_Upd( &sCifCifRel, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "查询客户-客户关系表错，无记录!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "C046" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Cif_cif_rel_Fet_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* 修改客户与客户关系表 */
		strcpy( sCifCifRel.rel_code, cRelCode );
		
		ret = Cif_cif_rel_Upd_Upd( sCifCifRel, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改客户与客户关系表错[%d]", ret );
			WRITEMSG
			strcpy( g_pub_tx.reply, "C047" );
			goto ErrExit;	
		}
		
		Cif_cif_rel_Clo_Upd();
	}	
        else		/* 如果是删除操作 */
        {
        	/* 准备删除表记录的where条件 */
        	sprintf( comm, "cif_no=%ld and rel_cid_no=%ld and rel_code='%s'", \
        		sCifCifRel.cif_no, sCifCifRel.rel_cid_no, cRelCode );

        	ret = deldb( "cif_cif_rel", comm );
        	if( ret )
        	{
        		sprintf( acErrMsg, "删除表记录错[%d]", ret );
			WRITEMSG
			goto ErrExit;
        	}
	}
	
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
