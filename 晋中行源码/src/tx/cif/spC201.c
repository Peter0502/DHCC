/*************************************************
* 文 件 名: spC201.c
* 功能描述：录入客户与客户间关系
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
#include "cif_basic_inf_c.h"

spC201()
{
	struct cif_cif_rel_c     sCifCifRel;		/* 客户-客户关系表 */
	struct cif_basic_inf_c   sCifBasicInf;		/* 客户基本信息表 */
	int ret;

        memset( &sCifCifRel, 0x00, sizeof( struct cif_cif_rel_c ) );
        memset( &sCifBasicInf, 0x00, sizeof( struct cif_basic_inf_c ) );
        
        /* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
        get_zd_long( "0290", &sCifCifRel.cif_no );        /* 客户ID */
        get_zd_long( "0280", &sCifCifRel.rel_cid_no );    /* 关联客户号 */
        get_zd_data( "0230", sCifCifRel.rel_code );       /* 客户关系代码 */

       	/* 检查录入的客户ID是否重复 */
       	if( sCifCifRel.cif_no == sCifCifRel.rel_cid_no )
       	{
       		sprintf( acErrMsg, "录入的客户ID重复!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C042" );
                goto ErrExit;
       	}
       
	/* 检查客户-客户关系表中是否已经存在相关记录 */
	ret = Cif_cif_rel_Sel( g_pub_tx.reply, &sCifCifRel, "( cif_no=%ld and rel_cid_no=%ld ) \
       				or ( cif_no=%ld and rel_cid_no=%ld) ", sCifCifRel.cif_no, \
       				sCifCifRel.rel_cid_no, sCifCifRel.rel_cid_no, sCifCifRel.cif_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "客户-客户关系表中已经存在相关记录!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C041" );
                goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "执行Cif_cif_rel_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

        /* 登记客户与客户关系表 */
	strcpy(	sCifCifRel.rel_sts, "1" );		/* 关系状态：1正常 2解除 */
	sCifCifRel.crt_date = g_pub_tx.tx_date;		/* 建立日期 */

        ret = Cif_cif_rel_Ins( sCifCifRel, g_pub_tx.reply ); 
        if( ret == -239 )
    	{
        	sprintf( acErrMsg, "登记客户与客户关系表失败!有重复记录!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "C027" );
        	goto ErrExit;
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Cif_cif_rel_Ins错误!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
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
