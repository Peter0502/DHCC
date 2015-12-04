/*************************************************
* 文 件 名:  spB353.c
* 功能描述： 完成开提货单功能
*
* 作    者:  jane 
* 完成日期： 2003年8月9日
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
#include "mo_sg_cash_po_bill_c.h"
#include "mo_bank_acc_po_c.h"

int spB353()
{
	struct	mo_sg_cash_po_c 		sMoSgCash;			/* 保兑仓登记簿 */
	struct	mo_sg_cash_po_bill_c	sMoSgCashBill;		/* 保兑仓提货单登记簿 */
	struct  mo_bank_acc_po_c		sMoBankAccPo;		/* 承兑汇票基本信息登记簿 */
	char	cTemp[20];									/* 提货单号码 */
	double	dAmt;										/* 存入金额 */
	double	dUseAmt;									/* 可以开提货单金额 */
	int		ret;

	memset( &sMoSgCash, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoSgCashBill, 0x00, sizeof( struct mo_sg_cash_po_bill_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cTemp, 0x00, sizeof( cTemp ) );

	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0330", sMoSgCashBill.pact_no );	/* 承兑协议编号 */
	get_zd_double( "0400", &dAmt ); 				/* 开提货单金额 */
	
	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoSgCashBill.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此承兑协议编号!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B187" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 判断只有在承兑汇票全部签发完毕之后才能开提货单 */
	if( sMoBankAccPo.acc_po_sts[0] == '0' )
	{
		sprintf( acErrMsg, "此承兑协议为录入状态!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B213" );
		goto ErrExit;
	}else if( sMoBankAccPo.acc_po_sts[0] == '1' )
	{
		sprintf( acErrMsg, "此承兑协议未做签发或未全部签发!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B211" );
		goto ErrExit;
	}

	/* 检查只有承兑汇票录入行才能去做本汇票的开提货单交易 */
	if( strcmp( sMoBankAccPo.tx_br_no, g_pub_tx.tx_br_no ) )
	{
		sprintf( acErrMsg, "交易机构非此承兑协议录入行!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B262" );
		goto ErrExit;
	}

	/* 为打印提货通知书传承兑协议录入日期 */
	set_zd_long( "0520", sMoBankAccPo.req_date );
vtcp_log( "签订日期[%ld]",sMoBankAccPo.req_date );

	/* 取保兑仓登记薄记录 */
	ret = Mo_sg_cash_po_Dec_Upd( g_pub_tx.reply, "pact_no = '%s'", sMoSgCashBill.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "执行Mo_sg_cash_po_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Mo_sg_cash_po_Fet_Upd( &sMoSgCash, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "此承兑非保兑仓!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B267" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_sg_cash_po_Fet_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}

	dUseAmt = sMoSgCash.safeg_sum - sMoSgCash.pickup_sum;	/* 可开提货单金额 */
vtcp_log( "janejanejane    safeg_sum[%lf]  pickup_sum[%lf]  dUseAmt[%lf]",sMoSgCash.safeg_sum, sMoSgCash.pickup_sum, dUseAmt );
	/* 检查金额是否超过可开提货单金额 */
	if ( pub_base_CompDblVal( dAmt, dUseAmt ) == 1 )
	{
		sprintf( acErrMsg, "开提货单金额大于可开提货单金额!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B076" );
		goto ErrExit;
	}

	sMoSgCash.pickup_sum += dAmt;		/* 已开提货单金额，含本次金额 */
vtcp_log( "已开提货单含本次金额为[%lf]", sMoSgCash.pickup_sum );

	ret = Mo_sg_cash_po_Upd_Upd( sMoSgCash , g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg, "修改保兑仓登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}

	Mo_sg_cash_po_Clo_Upd( );

	/* 登记保兑仓提货单登记薄 */
	
	/* 读取保兑仓提货单登记簿中对应此协议编号最大的提货单编号 */
	ret = Mo_sg_cash_po_bill_Sel( g_pub_tx.reply, &sMoSgCashBill, "pact_no='%s'", sMoSgCashBill.pact_no );
	if( ret != 100 && ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}else if( ret == 100 )
	{
		sMoSgCashBill.bill_num = 1;		/* 第一次开提货单，提货单编号为1 */
	}else
	{
		pub_base_strpack( sMoSgCashBill.pact_no );
		/* 查最大编号 */
		ret = sql_max_int( "mo_sg_cash_po_bill", "bill_num", "pact_no='%s'", sMoSgCashBill.pact_no );
		if( ret < 0 )
		{
			sprintf(acErrMsg,"执行sql_min_string错误![%d]", ret) ;
			WRITEMSG
			strcpy( g_pub_tx.reply, "D110" ) ;
			goto ErrExit;
		}
		
		/* 登记提货单号码:承兑协议编号＋编号 */
		sMoSgCashBill.bill_num = ret + 1;		/* 再开提货单时，编号自动加1 */
	}

	pub_base_strpack( sMoSgCashBill.pact_no );
	sprintf( cTemp, " %s%03d ", sMoSgCashBill.pact_no, sMoSgCashBill.bill_num );
	pub_base_strpack( cTemp );
	strcpy( sMoSgCashBill.lading_bill, cTemp );		/* 提货单号码 */
vtcp_log("lading_bill[%s]",sMoSgCashBill.lading_bill);

	sMoSgCashBill.pickup_amt = dAmt;		/* 提货单金额 */

    vtcp_log("JYW pact_no=%s,bill=%d,,ladbill=%s,pick_amt=%lf",sMoSgCashBill.pact_no,
                 sMoSgCashBill.bill_num,sMoSgCashBill.lading_bill,sMoSgCashBill.pickup_amt);

	ret = Mo_sg_cash_po_bill_Ins( sMoSgCashBill, g_pub_tx.reply );
	if( ret )
	{
		sprintf( acErrMsg, "执行Mo_sg_cash_po_bill_Ins错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 为打印赋值 */
	set_zd_double( "1001", sMoSgCash.pickup_sum );
	set_zd_long( "0500", sMoSgCashBill.bill_num );

	/* 记流水日志 */
	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoSgCashBill.pact_no);
	g_pub_tx.tx_amt1=dAmt;
	strcpy(g_pub_tx.beg_note_no,sMoSgCashBill.lading_bill);
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
