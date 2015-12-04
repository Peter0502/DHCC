/*************************************************
* 文 件 名:  spB352.c
* 功能描述： 完成保证金存款功能
*
* 作    者:  jane
* 完成日期： 2003年7月28日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_sg_cash_po_c.h"
#include "mo_bank_acc_po_c.h"

int spB352()
{
	struct	mo_sg_cash_po_c sMoSgCash;	/* 保兑仓登记簿 */
	struct  mo_bank_acc_po_c sMoBankAcc;/* 承兑汇票基本信息登记簿 */
	double	dAmt;						/* 存入金额 */
	int	ret;

	memset( &sMoSgCash, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cSgCashNo, 0x00, sizeof( cSgCashNo ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0330", sMoSgCash.pact_no );	/* 承兑协议编号 */
	get_zd_double( "0390", &dAmt ); 			/* 存入金额 */
	
	/* 检查协议编号是否存在 */
	ret = sql_count("mo_sg_cash_po", "sg_cash_no = '%s'", cSgCashNo );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找保兑仓协议编号错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "无保兑仓协议编号" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B072" );
		goto ErrExit;
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "存在多条保兑仓协议编号" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B073" );
		goto ErrExit;
	}
	
	/* 取保兑仓登记薄记录 */
	ret = Mo_sg_cash_po_Dec_Upd( g_pub_tx.reply , "sg_cash_no = '%s'", \
					cSgCashNo );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_sg_cash_po_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	
	ret = Mo_sg_cash_po_Fet_Upd( &sMoSgCash , g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "无此保兑仓协议记录" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B072" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_sg_cash_po_Fet_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	
	/* 查询承兑汇票基本信息登记簿，检查承兑协议编号是否存在，承兑类型是否为保兑仓 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply , &sMoBankAcc , \
				"pact_no = '%s'", sMoSgCash.pact_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "此承兑协议编号不存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B199" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( strcmp( sMoBankAcc.acc_type, "4" ) != 0 )
	{
		sprintf( acErrMsg, "此承兑类型非保兑仓!acc_type=[%s]", sMoBankAcc.acc_type );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B256" );
		goto ErrExit;
	}

	sMoSgCash.safeg_sum += dAmt;
	
	/* 检查已存保证金总金额是否超过承兑汇票金额 */
	if ( pub_base_CompDblVal(sMoSgCash.safeg_sum, sMoBankAcc.bail_amt) == 1 )
	{
		sprintf( acErrMsg, "已存保证金金额大于承兑汇票保证金金额" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B074" );
		goto ErrExit;
	}
	
	/* 修改保兑仓登记薄已存入保证金总金额 */
	ret = Mo_sg_cash_po_Upd_Upd( sMoSgCash , g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg, "修改保兑仓登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	
	Mo_sg_cash_po_Clo_Upd( );
	
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
