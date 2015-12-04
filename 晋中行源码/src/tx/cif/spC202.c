/*************************************************
* 文 件 名: spC202.c
* 功能描述：客户经理指派
*              
*
* 作    者: andy
* 完成日期：2004年01月10日
* 修改记录：   
*     1. 日    期: 2004年2月4日
*        修 改 人: andy
*        修改内容:
*************************************************/
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "cif_mger_rel_c.h"

spC202()  
{
	struct 	cif_mger_rel_c		sCifMgerRel;		/* 客户-客户经理关系表 */
	int ret=0;

	memset ( &sCifMgerRel, 0x00, sizeof( struct cif_mger_rel_c ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_long( "0290", &sCifMgerRel.cif_no );		/* 客户号 */
	get_zd_data( "0920", sCifMgerRel.mang );		/* 客户经理代码 */
	get_zd_data( "0700", sCifMgerRel.mang_type );	/* 客户经理类型 */

	/* 检查客户-客户经理关系表中是否已经存在相关记录 */
	ret = Cif_mger_rel_Sel( g_pub_tx.reply, &sCifMgerRel, \
					"cif_no=%ld and mang='%s' and mang_type='%s' and \
					 canl_date=0 ", \
					sCifMgerRel.cif_no, sCifMgerRel.mang,sCifMgerRel.mang_type);
	if( ret == 0 )
	{
		sprintf( acErrMsg, "客户-客户经理关系表中已经存在相关记录!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C043" );
                goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "执行Cif_mger_rel_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 登记客户-客户经理关系表 */
	sCifMgerRel.crt_date = g_pub_tx.tx_date ;	 /* 建立日期 */
			
	ret = Cif_mger_rel_Ins( sCifMgerRel, g_pub_tx.reply );
	if( ret == -239 )
    {
       	sprintf( acErrMsg, "登记客户-客户经理关系表失败!有重复记录!" );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C016" );
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
    sprintf(acErrMsg,"建立客户识别ID成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"建立客户识别ID失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
