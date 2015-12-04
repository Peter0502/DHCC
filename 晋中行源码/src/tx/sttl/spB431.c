/*************************************************
* 文 件 名:  spB431.c
* 功能描述： 完成登记委托收款协议薄功能。
*
* 作    者:  jane
* 完成日期： 2003年6月15日
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

int spB431()
{
	struct	mo_trust_pay_c	sMoTrustPay;	/* 委托收款登记薄 */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* 介质与账户关系表 */
	struct	ln_mst_c	sLnMst;		/* 活期存款主文件 */
	char	cPayeeAcNo[20];			/* 收款账号 */
	int	ret;
	
TRACE	
	memset( &sMoTrustPay, 0x00, sizeof( struct mo_trust_pay_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sLnMst, 0x00, sizeof( struct ln_mst_c ) );
	memset( cPayeeAcNo, 0x00, sizeof( cPayeeAcNo ) );
TRACE	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	strcpy( sMoTrustPay.br_no, g_pub_tx.tx_br_no );		/* 机构编码 */
	get_zd_data( "0580", sMoTrustPay.turst_pay_no );	/* 委托收款协议编号 */
	strcpy( sMoTrustPay.turst_fag, "1" );			/* 委托标志：1委托收款 2托收承付 */
	get_zd_data( "0260", sMoTrustPay.requ_name );		/* 申请人全称 */
	get_zd_data( "0670", sMoTrustPay.requ_id_type );	/* 委托人证件类型 */
	get_zd_data( "0620", sMoTrustPay.requ_id_no );		/* 申请人证件号码 */
	get_zd_data( "0370", cPayeeAcNo );			/* 收款人账号 */
	get_zd_long( "0340", &sMoTrustPay.payee_ac_seqn );	/* 收款人账号ID序号 */
	strcpy( sMoTrustPay.sts, "1" );				/* 状态：1登记 2注销*/

	/* 检查委托收款登记薄中是否已经存在此协议编号 */
	ret = Mo_trust_pay_Sel( g_pub_tx.reply, &sMoTrustPay, "turst_pay_no='%s'",\
			 	sMoTrustPay.turst_pay_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "委托收款登记薄中已经存在此协议编号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B246" );
                goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "执行Mo_trust_pay_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 检查收款帐号有效性：贷款账号、账户状态必须正常 *
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				cPayeeAcNo );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "所输收款帐号不存在!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B243" );
                goto ErrExit;
	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}
TRACE
	* 根据客户帐号ID、帐户序号查询贷款主文件 *
	ret =Ln_mst_Sel( g_pub_tx.reply, &sLnMst, "ac_id=%ld and ac_seqn=%d",\
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此收款帐号非贷款帐号![%s]", cPayeeAcNo );
    		WRITEMSG
    		strcpy( g_pub_tx.reply, "B244" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Ln_mst_Sel错![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
	}
TRACE

	if( sLnMst.ac_sts[0] != '1' )
	{
		sprintf( acErrMsg, "此收款帐号状态不正常!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B245" );
		goto ErrExit;
	}
TRACE
*/
	/* 登记委托收款协议登记薄 */
	sMoTrustPay.payee_ac_id= sMdmAcRel.ac_id ;	/* 收款人账号ID */

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
