/*************************************************
* 文 件 名:  spB104.c
* 功能描述： 完成银行汇票兑付功能，本行客户或者他行客户
*		携带汇票到本行兑付汇票
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

int spB104()
{
	struct	mo_bank_po_c	sMoBankPo;	/* 银行汇票登记簿 */
	char	cOpenBrno[9];			/* 帐号开户机构 */
	char	cFlag[2];			/* 是否是本行标志 */
	long	lAcId;				/* 收款人帐号ID */
	int	num = 0;
	int	ret;
	
	/* 数据初始化 */
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cFlag, 0x00, sizeof( cFlag ) );
	
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0870", sMoBankPo.sign_br_no );	/* 签发行号 */
	get_zd_data( "0590", sMoBankPo.po_no );		/* 汇票号码 */
	get_zd_data( "0670", sMoBankPo.ct_ind );	/* 现转标志 */
	get_zd_long( "0440", &sMoBankPo.sign_date );	/* 签发日期 */
	get_zd_data( "0680", sMoBankPo.payee_id_type );	/* 证件类型 */
	get_zd_data( "0620", sMoBankPo.payee_id_no );	/* 证件号码 */
	get_zd_data( "0810", sMoBankPo.payee_name );	/* 收款人全称 */
	get_zd_data( "0310", sMoBankPo.payee_ac_no );	/* 收款人帐号 */
	get_zd_long( "0340", &sMoBankPo.payee_ac_seqn );	/* 收款人帐号序号 */
	get_zd_double( "0390", &sMoBankPo.sign_amt );	/* 签发金额 */
	get_zd_double( "0400", &sMoBankPo.amt );	/* 清算金额 */
	sMoBankPo.bal_date = g_pub_tx.tx_date;		/* 清算日期 */
	strcpy( sMoBankPo.po_sts, "4" );		/* 汇票状态 */
	strcpy( sMoBankPo.po_ind, "1" );		/* 汇票标志 */
	
	get_zd_data( "0690", cFlag );			/* 收款人帐号所属行 */
	
	/* 如果收款人帐号是本行，需要检查帐号是否存在，状态是否正常 */
	if ( cFlag[0] == 'Y' )	/* 是本行 */
	{
		ret = pub_base_ChkAc( sMoBankPo.payee_ac_no, sMoBankPo.payee_ac_seqn,\
					cOpenBrno, &lAcId );
		if ( ret )
		{
			sprintf( acErrMsg, "检查帐户状态，取帐户id错[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
	}
	
	/* 检查签发期限是否超过一个月 */
	if ( pub_base_CalTrueDays( sMoBankPo.sign_date, sMoBankPo.bal_date ) > 31 )
	{
		sprintf( acErrMsg, "汇票已经超过一个月，不能兑付" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B029" );
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
	
	/* 检查汇票标志是否已经挂失，如果不是继续执行 */
	ret = sql_count("mo_bank_po", "po_no = '%s' and po_ind='0'", \
			sMoBankPo.po_no);
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找汇票登记薄错![%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( ret > 0 )
	{
		sprintf( acErrMsg, "该银行汇票已经挂失，不能兑付汇票!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B031" );
		goto ErrExit;
	}
	
	/* 登记汇票登记薄 */
	ret = pub_sttl_RegMoBankPo( sMoBankPo );
	if ( ret )
    	{
        	sprintf( acErrMsg, "执行pub_sttl_RegMoBankPo错误!ret=[%d]", ret );
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
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

