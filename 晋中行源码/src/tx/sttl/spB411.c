/*************************************************
* 文 件 名:  spB411.c
* 功能描述： 完成传真提出录入、复核功能。
*
* 作    者:  jane
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
#include "com_branch_c.h"
#include "mo_fax_c.h"

int spB411()
{
	struct	com_branch_c	sComBranch;	/* 公共机构码表 */
	struct	mo_fax_c	sMoFax;		/* 传真登记薄 */
	char	cOpenBrno[9];			/* 申请人帐号开户机构 */
	char	cPayerAcNo[20];			/* 付款人帐号 */
	char	cOperCode[6];			/* 业务代码 */
	int	ret;
	
	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( &sMoFax, 0x00, sizeof( struct mo_fax_c ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cPayerAcNo, 0x00, sizeof( cPayerAcNo ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0580", sMoFax.fax_no );			/* 传真编号 */
	sMoFax.tx_date = g_pub_tx.tx_date;			/* 交易日期 */
	strcpy( sMoFax.tx_br_no, g_pub_tx.tx_br_no );		/* 交易机构号 */
	get_zd_data( "0250", sMoFax.requ_name );		/* 申请人全称 */
	get_zd_data( "0260", sMoFax.payer_name );		/* 付款人全称 */
	get_zd_data( "0300", cPayerAcNo );			/* 付款人帐号 */
	get_zd_long( "0340", &sMoFax.payer_ac_seqn );		/* 付款人账号序号 */
	get_zd_data( "0860", sMoFax.payer_br_no );		/* 付款人开户行行号 */
	get_zd_data( "0820", sMoFax.payer_br_name );		/* 付款人开户行行名 */
	get_zd_data( "0270", sMoFax.payee_name );		/* 收款人全称 */
	get_zd_data( "0310", sMoFax.payee_ac_no );		/* 收款人账号 */
	get_zd_data( "0870", sMoFax.payee_br_no );		/* 收款人开户行行号 */
	get_zd_data( "0810", sMoFax.payee_br_name );		/* 收款人开户行行名 */
	get_zd_double( "0390", &sMoFax.fax_amt );		/* 传真金额 */
	strcpy( sMoFax.fax_sts, "0" );				/* 传真状态0录入*/
	strcpy( cOperCode, "10005" );
	
	/* 检查交易机构是否具有权限 */
	ret = pub_sttl_ChkInputBrno( cOperCode, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查交易机构权限错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 检查付款帐号的有效性，并返回帐号id */
	ret = pub_base_ChkAc( cPayerAcNo, sMoFax.payer_ac_seqn, \
				cOpenBrno, &sMoFax.payer_ac_id );
	if ( ret )
	{
		sprintf( acErrMsg, "检查付款帐号有效性错" );
		WRITEMSG
		goto ErrExit;	
	}
sprintf( acErrMsg, "br_no=[%s]", g_pub_tx.tx_br_no );
WRITEMSG	
	/* 检查中心是否已经打印；如果已经打印，则不能录入 */
	ret = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", g_pub_tx.tx_br_no );
	if( ret == 100 )
    	{
        	sprintf( acErrMsg,"取公共机构码表com_branch错!br_no=[%s]", g_pub_tx.tx_br_no );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "S019" );
        	goto ErrExit;
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Com_branch_Sel错误!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
   	}
   	
   	if( sComBranch.spe_ind[0] == '1' )
   	{
   		sprintf( acErrMsg,"传真打印状态为已打印!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B077" );
        	goto ErrExit;
   	}
	
	/* 登记传真登记薄，状态为录入 */
	ret = pub_sttl_RegMoFax( sMoFax );
	if ( ret )
    	{
        	sprintf( acErrMsg, "执行pub_sttl_RegMoFax错误!ret=[%d]", ret );
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
