/*************************************************
* 文 件 名:  spB433.c
* 功能描述： 完成登记委托收款清单核销功能。
*
* 作    者:  jane
* 完成日期： 2003年2月2日
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
#include "mo_trust_list_c.h"

int spB433()
{
	struct	mo_trust_pay_c	sMoTrustPay;	/* 委托收款登记薄 */
	struct	mo_trust_list_c	sMoTrustList;	/* 委托清单 */
	double	dAmt;
	int	ret;

	memset( &sMoTrustPay, 0x00, sizeof( struct mo_trust_pay_c ) );
	memset( &sMoTrustList, 0x00, sizeof( struct mo_trust_list_c ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0580", sMoTrustList.turst_pay_no );	/* 委托收款协议编号 */
	get_zd_data( "0300", sMoTrustList.payer_ac_no );	/* 付款人账号 */
	get_zd_double( "0390", &dAmt );				/* 金额 */

	/* 根据委托收款协议编号查委托收款登记薄，检查本交易机构是否为录入机构 */
	ret = Mo_trust_pay_Sel( g_pub_tx.reply, &sMoTrustPay, "turst_pay_no='%s' and br_no='%s'", \
				sMoTrustList.turst_pay_no, g_pub_tx.tx_br_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "委托收款登记薄中无此协议编号信息!turst_pay_no=[%s]",\
			sMoTrustPay.turst_pay_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B100" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mo_trust_pay_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 检查委托收款协议是否存在,交易金额是否符合数据库中的金额 */
	ret = Mo_trust_list_Dec_Upd( g_pub_tx.reply, "turst_pay_no='%s' and payer_ac_no='%s'", \
					sMoTrustList.turst_pay_no, sMoTrustList.payer_ac_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_trust_list_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_trust_list_Fet_Upd( &sMoTrustList, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "查询委托清单错，无记录!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B104" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mo_trust_list_Fet_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( pub_base_CompDblVal( sMoTrustList.amt, dAmt ) != 0 )
	{
		sprintf( acErrMsg, "交易金额与委托清单中金额不符!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B102" );
		goto ErrExit;
	}
	
	/* 修改该笔清单状态为核销 */
	strcpy( sMoTrustList.sts, "2" );			/* 状态：1登记 2注销*/

	ret = Mo_trust_list_Upd_Upd( sMoTrustList, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改委托清单错[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B101" );
		goto ErrExit;	
	}

	Mo_trust_list_Clo_Upd();

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
