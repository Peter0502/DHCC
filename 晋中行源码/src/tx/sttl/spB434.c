/*************************************************
* 文 件 名:  spB434.c
* 功能描述： 完成托收承付登记功能。
*
* 作    者:  jane
* 完成日期： 2003年2月3日
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
#include "mo_trust_pay_c.h"

int spB434()
{
	struct	mo_trust_pay_c	sMoTrustPay;	/* 委托收款登记薄 */
	char	cOpenBrno[9];			/* 收款账号开户机构 */
	char	cPayeeAcNo[20];			/* 收款账号 */
	char	cOperCode[6];			/* 业务代码 */
	int	ret;
	
	memset( &sMoTrustPay, 0x00, sizeof( struct mo_trust_pay_c ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cPayeeAcNo, 0x00, sizeof( cPayeeAcNo ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	strcpy( sMoTrustPay.br_no, g_pub_tx.tx_br_no );		/* 机构编码 */
	get_zd_data( "0580", sMoTrustPay.turst_pay_no );	/* 委托收款协议编号 */
	strcpy( sMoTrustPay.turst_fag, "2" );			/* 委托标志：1委托收款 2托收承付 */
	get_zd_data( "0250", sMoTrustPay.requ_name );		/* 申请人全称 */
	get_zd_data( "0670", sMoTrustPay.requ_id_type );	/* 委托人证件类型 */
	get_zd_data( "0620", sMoTrustPay.requ_id_no );		/* 申请人证件号码 */
	get_zd_data( "0300", cPayeeAcNo );			/* 收款人账号 */
	get_zd_long( "0340", &sMoTrustPay.payee_ac_seqn );	/* 收款人账号ID序号 */
	strcpy( sMoTrustPay.sts, "1" );				/* 状态：1登记 2注销*/
	strcpy( cOperCode, "10006" );
	
	/* 检查交易机构是否具有权限 */
	ret = pub_sttl_ChkInputBrno( cOperCode, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查交易机构权限错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 检查收款帐号有效性，并返回帐号id */
	ret = pub_base_ChkAc( cPayeeAcNo, sMoTrustPay.payee_ac_seqn, \
				cOpenBrno, &sMoTrustPay.payee_ac_id );
	if ( ret )
	{
		sprintf( acErrMsg, "检查收款帐号有效性错" );
		WRITEMSG
		goto ErrExit;	
	}
	
	/* 登记委托收款协议登记薄，状态为录入 */
	ret = pub_sttl_RegMoTrustPay( sMoTrustPay );
	if ( ret )
    	{
        	sprintf( acErrMsg, "执行pub_sttl_RegMoTrustPay错误!ret=[%d]", ret );
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
