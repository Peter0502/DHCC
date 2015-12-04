/*************************************************
* 文 件 名:  spB107.c
* 功能描述： 完成银行汇票逾期兑付的功能。签发行付款给持票人
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

int spB107()
{
	struct	mo_bank_po_c	sMoBankPo;	/* 银行汇票登记簿 */
	char	cOpenBrno[9];			/* 帐号开户机构 */
	char	cFlag[2];			/* 是否是本行标志 */
	char	cOperCode[6];				/* 业务代码 */
	long	lAcId;
	int	num = 0;
	int	ret;
	
	/* 数据初始化 */
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0590", sMoBankPo.po_no );		/* 汇票号码 */
	get_zd_data( "0810", sMoBankPo.payee_name );	/* 收款人全称 */
	get_zd_data( "0310", sMoBankPo.payee_ac_no );	/* 收款人帐号 */
	get_zd_long( "0340", &sMoBankPo.payee_ac_seqn );	/* 收款人帐号序号 */
	
	get_zd_data( "0670", cFlag );			/* 收款人帐号所属行 */
	strcpy( cOperCode, "10001" );
	
	/* 如果收款人帐号是本行，需要检查帐号是否存在，状态是否正常 */
	if ( cFlag[0] == 'Y' )	/* 是本行 */
	{
		ret = pub_base_ChkAc( sMoBankPo.payee_ac_no, \
				sMoBankPo.payee_ac_seqn, cOpenBrno, &lAcId );
		if ( ret )
		{
			sprintf( acErrMsg, "检查帐户状态，取帐户id错[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
	}
	
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
	ret = sql_count( "mo_bank_po", "po_no = '%s' and po_sts = '2' \
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

	/* 声明打开游标 */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply , "po_no = '%s'and po_sts = '2' \
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
	sMoBankPo.bal_date = g_pub_tx.tx_date;		/* 清算日期 */
	strcpy( sMoBankPo.po_sts, "3" );		/* 汇票状态 */
	strcpy( sMoBankPo.po_ind, "1" );		/* 汇票标志 */	
	
	/* 修改汇票登记薄 */
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

