/*************************************************
* 文 件 名: spC502.c
* 功能描述: 黑名单信息维护
* 作    者: andy
* 完成日期：2005年2月5日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_blklst_rel_c.h"

spC502()
{
	struct	cif_blklst_rel_c	sCifBlklstRel;		/* 客户黑名单关系 */
	char	cOptcode[2];					/* 操作码 */
	char	cRmk[41];					/* 上单原因 */
	char	comm[100];					/* 删除表记录的where条件 */
	int 	ret;
 	
	memset( &sCifBlklstRel, 0x00, sizeof( struct cif_blklst_rel_c ) );
	memset( cOptcode, 0x00, sizeof( cOptcode ) );
	memset( cRmk, 0x00, sizeof( cRmk ) );
	memset( comm, 0x00, sizeof( comm ) );

        /* 数据初始化 */
        pub_base_sysinit();

        /* 从前台屏幕取值 */
        get_zd_data( "0680", cOptcode );  			/* 操作码: 1修改 2删除 */
        get_zd_long( "0280", &sCifBlklstRel.cif_no );		/* 客户号 */
    	get_zd_data( "0670", sCifBlklstRel.blalist_origin );  	/* 信息来源 */
    	get_zd_data( "0810", cRmk );             		/* 上单原因 */
       	
       	/* 如果是修改操作 */
       	if( cOptcode[0] == '1' )
       	{
       		/* 检查客户黑名单关系表 */
		ret = Cif_blklst_rel_Dec_Upd( g_pub_tx.reply , "cif_no=%ld and blalist_origin='%s'", \
						sCifBlklstRel.cif_no, sCifBlklstRel.blalist_origin );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Cif_blklst_rel_Dec_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* 取出记录 */
		ret = Cif_blklst_rel_Fet_Upd( &sCifBlklstRel, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "查询客户黑名单关系表错，无记录!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "C059" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Cif_blklst_rel_Fet_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* 修改客户黑名单关系表 */
		strcpy( sCifBlklstRel.rmk, cRmk );
		
		ret = Cif_blklst_rel_Upd_Upd( sCifBlklstRel, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改客户黑名单关系表错[%d]", ret );
			WRITEMSG
			strcpy( g_pub_tx.reply, "C060" );
			goto ErrExit;	
		}
		
		Cif_blklst_rel_Clo_Upd();
	}
	else		/* 如果是删除操作 */
        {
        	/* 准备删除表记录的where条件 */
        	sprintf( comm, "cif_no=%ld and blalist_origin='%s' and rmk='%s'", \
        		sCifBlklstRel.cif_no, sCifBlklstRel.blalist_origin, cRmk );

        	ret = deldb( "cif_blklst_rel", comm );
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
