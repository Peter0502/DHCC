/*************************************************
* 文 件 名: spC501.c
* 功能描述: 黑名单信息录入
* 作    者: andy
* 完成日期：2005年01月25日
* 修改记录：
*     1. 日    期:2005年2月5日
*        修 改 人:andy
*        修改内容:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_blklst_rel_c.h"
#include "cif_basic_inf_c.h"

spC501()
{
	struct	cif_blklst_rel_c	sCifBlklstRel;		/* 客户黑名单关系 */
	struct 	cif_basic_inf_c   	sCifBasicInf;		/* 客户基本信息表 */
	int 	ret;
 	
	memset( &sCifBlklstRel, 0x00, sizeof( struct cif_blklst_rel_c ) );
   	memset( &sCifBasicInf, 0x00, sizeof( struct cif_basic_inf_c ) );

    /* 数据初始化 */
    pub_base_sysinit();

    /* 从前台屏幕取值 */
    get_zd_long( "0280", &sCifBlklstRel.cif_no );			/* 客户号 */
  	get_zd_data( "0670", sCifBlklstRel.blalist_origin );  	/* 信息来源 */
  	get_zd_data( "0810", sCifBlklstRel.rmk );             	/* 上单原因 */

    /* 检查客户号是否已经存在 */
    ret = Cif_basic_inf_Sel( g_pub_tx.reply, &sCifBasicInf, \
							"cif_no=%ld", sCifBlklstRel.cif_no );
    if( ret == 100 )
    {
		sprintf( acErrMsg, "无此客户号，请先到客户中心登记!![%ld]", \
				sCifBlklstRel.cif_no );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C007" );
		goto ErrExit;
    }
	else if( ret )
    {
       	sprintf( acErrMsg, "读取客户基本信息异常![%d]", ret );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C006" );
       	goto ErrExit;
    }
        
    /* 检查客户是否已经上过黑名单 */
    ret = Cif_blklst_rel_Sel( g_pub_tx.reply, &sCifBlklstRel, \
							"cif_no=%ld and blalist_origin='%s'", \
    						sCifBlklstRel.cif_no, sCifBlklstRel.blalist_origin);             
    if( ret == 0 )
    {
		sprintf( acErrMsg, "此客户已经登记黑名单![%ld]", sCifBlklstRel.cif_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "C030" );
		goto ErrExit;
	}
	else if( ret != 100 )
    {
       	sprintf( acErrMsg, "读取客户黑名单表异常![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
    }
       		
    /* 登记客户黑名单关系表 */
    strcpy( sCifBlklstRel.relation_sts, "1" );		/* 关系状态：1正常 2解除 */
    sCifBlklstRel.crt_date=g_pub_tx.tx_date;		/* 建立日期 */

    ret =Cif_blklst_rel_Ins( sCifBlklstRel, g_pub_tx.reply );
    if( ret == -239 )
    {
       	sprintf( acErrMsg, "登记客户黑名单信息失败!有重复记录!" );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C031" );
       	goto ErrExit;
   	}
	else if ( ret )
   	{
       	sprintf( acErrMsg, "执行Cif_blklst_rel_Ins错误!ret=[%d]", ret );
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

