/*************************************************
* 文 件 名:  spB103.c
* 功能描述： 完成汇票未用退回的功能，必须在汇票期限内使用，
*	    如果超过期限使用逾期兑付。
*	    该交易必须签发行执行
*
* 作    者:  Terry
* 完成日期： 2003年1月18日
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

int spB103()
{
	struct	mo_bank_po_c	sMoBankPo;	/* 银行汇票登记簿 */
	char	cPoNo[17];			/* 汇票号码 */
	char	cOpenBrno[9];			/* 帐号开户机构 */
	char	cOperCode[6];				/* 业务代码 */
	double	dAcBal;				/* 帐户余额 */
	int	ret;
	
	/* 数据初始化 */
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0590", cPoNo );			/* 汇票号码 */
	get_zd_data( "0300", g_pub_tx.ac_no );		/* 退款帐号 */
	get_zd_int( "0340", &(g_pub_tx.ac_seqn) );	/* 帐号序号 */

	strcpy( cOperCode, "10001" );
	
	/* 检查退款帐号是否存在，状态是否正常，返回帐号id */
	ret = pub_base_ChkAc( g_pub_tx.ac_no, g_pub_tx.ac_seqn, cOpenBrno, &sMoBankPo.undo_ac_id );
	if( ret )
	{
		sprintf( acErrMsg, "根据退回帐号和帐号序号取退回帐号ID错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 查找汇票是否存在 */
	ret = sql_count( "mo_bank_po", "po_no = '%s'", cPoNo );
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
			and po_ind = '1'", cPoNo );
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
	
	/* 声明打开游标 */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply , "po_no = '%s' and po_sts = '1' \
					and po_ind = '1'", cPoNo );
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
			
	/* 检查交易机构是否具有权限 */
	ret = pub_sttl_ChkSignBrno( cOperCode, sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查交易机构是否是签发行错" );
		WRITEMSG
		goto ErrExit;	
	}
	
	/* 修改汇票状态:填写退款帐号、帐号序号、退票*/
	sMoBankPo.undo_ac_seqn = g_pub_tx.ac_seqn;
	strcpy( sMoBankPo.po_sts, "5" );		/* 5表示退票 */
	
	ret = Mo_bank_po_Upd_Upd( sMoBankPo , g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg, "修改银行汇票登记薄错[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B017" );
		goto ErrExit;	
	}
	
	Mo_bank_po_Clo_Upd( );
	
	/* 返回退款帐号余额 */
	ret = pub_base_ChkAcBal( g_pub_tx.ac_no, g_pub_tx.ac_seqn, &dAcBal );
	if( ret )	
	{
		sprintf( acErrMsg, "检查帐户余额错" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_double( "0390", dAcBal );
	
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

