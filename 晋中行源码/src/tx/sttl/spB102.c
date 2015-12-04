/*************************************************
* 文 件 名:  spB102.c
* 功能描述： 完成签发银行汇票功能，系统日期即为出票日期，打印到汇票上。
*
* 作    者:  jane
* 完成日期： 2003年1月18日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"

int spB102()
{
	struct	mo_bank_po_c	sMoBankPo;		/* 银行汇票登记簿 */
	char	cPoRequNo[17];				/* 申请书号码 */
	char	cPoNo[17];				/* 汇票号码 */
	char	cOperCode[6];				/* 业务代码 */
	int	ret;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cPoRequNo, 0x00, sizeof( cPoRequNo ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0580", cPoRequNo );
	get_zd_data( "0590", cPoNo );
	strcpy( cOperCode, "10001" );
	
	/* 根据汇票号码、申请书号码查找汇票登记薄 */
	ret = sql_count( "mo_bank_po", "po_no='%s' and po_requ_no='%s'",\
			cPoNo, cPoRequNo );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查询汇票登记薄错,ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B018" );
		goto ErrExit;	
	}else if ( ret == 0 )
	{
		sprintf( acErrMsg, "无此银行汇票" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B012" );
		goto ErrExit;	
	}
	
	/* 检查汇票状态是否为录入0或者标志是否为正常1 */
	ret = sql_count( "mo_bank_po", "po_no='%s' and po_requ_no='%s' and po_sts='%s' \
			and po_ind='%s'",cPoNo, cPoRequNo, "0", "1" );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查询汇票登记薄错,ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B018" );
		goto ErrExit;	
	}else if ( ret == 0 )
	{
		sprintf( acErrMsg, "银行汇票状态或者标志不正常" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B032" );
		goto ErrExit;
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "银行汇票登记薄存在多条记录" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B019" );
		goto ErrExit;	
	}
	
	/* 声明打开游标 */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply , "po_no='%s' \
				and po_requ_no='%s' and po_sts='%s' \
				and po_ind='%s'", cPoNo, cPoRequNo ,"0", "1" );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_po_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "查找汇票登记薄错，无此银行汇票" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B012" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_po_Fet_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 检查柜员所在机构是否具有签发支票权限,申请行是否属于本行签发 */
	ret = pub_sttl_ChkSignBrno( cOperCode, sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查机构权限错!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 银行汇票登记簿状态位改为签发 */
	strcpy( sMoBankPo.sign_br_no, g_pub_tx.tx_br_no );	/* 签发行行号 */
	strcpy( sMoBankPo.po_sts, "1" );			/* "1"表示签发状态 */
	sMoBankPo.sign_date = g_pub_tx.tx_date;			/* 签发日期 */
	
	ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
    	if ( ret )
	{
		sprintf( acErrMsg, "修改银行汇票登记薄错[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B017" );
		goto ErrExit;	
	}
	
	Mo_bank_po_Clo_Upd( );
	
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
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
