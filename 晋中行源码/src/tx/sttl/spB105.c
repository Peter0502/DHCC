/*************************************************
* 文 件 名:  spB105.c
* 功能描述： 完成银行汇票核销的功能。签发行接到代理付款行
*	    寄交的联行借方报单和解讫通知时，根据原来保管
*	    的卡片与之核对无误，执行该交易。
*
* 作    者:  Terry
* 完成日期： 2003年1月19日
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
#include "mo_bank_po_c.h"

int spB105()
{
	struct	mo_bank_po_c	sMoBankPo;	/* 银行汇票登记簿 */
	char	cOperCode[6];				/* 业务代码 */
	int	ret;
	
	/* 数据初始化 */
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0590", sMoBankPo.po_no );		/* 汇票号码 */
	get_zd_double( "0390", &sMoBankPo.sign_amt );	/* 签发金额 */
	get_zd_double( "0400", &sMoBankPo.amt );	/* 清算金额 */
	sMoBankPo.bal_date = g_pub_tx.tx_date;		/* 计算日期 */
	strcpy( cOperCode, "10001" );
	
	/* 查找汇票是否存在 */
	ret = sql_count( "mo_bank_po", "po_no = '%s'", sMoBankPo.po_no );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找汇票登记薄错[%d]", ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "无此银行汇票" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B012" );
		goto ErrExit;
	}
	
	/* 检查汇票状态或者标志是否正常 */
	ret = sql_count( "mo_bank_po", "po_no = '%s' and po_sts = '1' \
			and po_ind = '1'", sMoBankPo.po_no );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找汇票登记薄错[%d]", ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "银行汇票状态或者标志不正常ret=[%d]",ret );
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

	/* 检查清算金额是否小于等于签发金额 */
	if( pub_base_CompDblVal( sMoBankPo.sign_amt , sMoBankPo.amt ) == 1 )
	{
		sprintf( acErrMsg, "清算金额大于签发金额,不能兑付汇票!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B030" );
		goto ErrExit;
	}
	
	/* 声明打开游标 */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply , "po_no = '%s'and po_sts = '1' \
					and po_ind = '1'", sMoBankPo.po_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行	Mo_bank_po_Dec_Upd错[%d]", ret );
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
			
	/* 检查交易机构是否具有权限 */
	ret = pub_sttl_ChkSignBrno( cOperCode, sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查交易机构是否是签发行错" );
		WRITEMSG
		goto ErrExit;	
	}
	
	/* 修改汇票登记薄 */
	strcpy( sMoBankPo.po_sts, "6" );		/* 汇票状态 */
	
	ret = Mo_bank_po_Upd_Upd( sMoBankPo , g_pub_tx.reply  );
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
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


