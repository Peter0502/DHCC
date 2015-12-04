/*************************************************
* 文 件 名:  spB351.c
* 功能描述： 完成保兑仓协议录入功能
*
* 作    者:  Terry
* 完成日期： 2003年1月26日
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

int spB351()
{
	struct	mo_sg_cash_po_c sMoSgCash;		/* 保兑仓登记簿 */
	struct  mo_bank_acc_po_c sMoBankAcc;		/* 承兑汇票 */
	int	ret;

	memset( &sMoSgCash, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0580", sMoSgCash.acc_po_no );	/* 承兑汇票号码 */
	get_zd_data( "0590", sMoSgCash.sg_cash_no );/* 保兑仓协议编号 */
	sMoSgCash.pickup_sum = 0;
	strcpy( sMoSgCash.br_no, g_pub_tx.tx_br_no );
	
	/* 取承兑汇票登记薄保证金金额 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply , &sMoBankAcc, \
			"acc_po_no = '%s'", sMoSgCash.acc_po_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "无此承兑汇票号码" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B061" );
		goto ErrExit;	
	}else if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;		
	}
	

	pub_base_strpack( sMoBankAcc.bail_ac_no );
	
	if ( !strlen( sMoBankAcc.bail_ac_no ) )
	{
		sprintf( acErrMsg, "承兑汇票无保证金帐号" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B070" );	
	}
	
	sMoSgCash.safeg_sum = sMoBankAcc.bail_amt;
	
	/* 登记汇票登记薄 */
	ret = pub_sttl_RegMoSgCash( sMoSgCash );
	if ( ret )
    	{
        	sprintf( acErrMsg, "执行pub_sttl_RegMoSgCash错误![%d]", ret );
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
