/*************************************************
* 文 件 名:  sp9869.c
* 功能描述： 由转贴现协议编号回显贴现登记薄信息
*
* 作    者:  jane
* 完成日期： 2003年3月11日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int sp9869()
{
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	
	get_zd_data( "0630", sMoDiscnt.tpact_no );	/* 转贴现协议编号 */

	/* 检查贴现登记薄中是否存在此贴现协议编号 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "tpact_no='%s'", sMoDiscnt.tpact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此转贴现协议编号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B133" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 向前台屏幕赋值 */
	set_zd_data( "0620", sMoDiscnt.pact_no );	/* 贴现协议编号 */
    	set_zd_double( "1002", sMoDiscnt.par_amt );	/* 汇票金额 */
	set_zd_double( "0840", sMoDiscnt.tpn_rate );	/* 转贴现率 */
	set_zd_long( "0450", sMoDiscnt.not_matr_date );/* 汇票到期日 */
	set_zd_data( "0310", sMoDiscnt.rem_ac_no );	/* 出票人帐号 */
	set_zd_data( "0260", sMoDiscnt.remitter );	/* 出票人全称 */
	set_zd_data( "0810", sMoDiscnt.payer_brno_name );/* 付款行全称 */
	set_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* 收款人帐号 */
	set_zd_data( "0270", sMoDiscnt.payee );		/* 收款人全称 */
	set_zd_data( "0820", sMoDiscnt.payee_brno_name );/* 收款人开户行全称 */
	set_zd_data( "0900", sMoDiscnt.note_type );	/* 票据类型 */
	set_zd_data( "0600", sMoDiscnt.pnote_no );	/* 票据号 */
	set_zd_data( "0370", sMoDiscnt.tin_ac_no );	/* 转贴现账户 */
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* 异地同城标志 */
	set_zd_long( "0480", sMoDiscnt.way_days );	/* 在途天数 */
	set_zd_double( "1001", sMoDiscnt.tpn_int );	/* 转贴现利息 */
	set_zd_data( "0670", sMoDiscnt.tpay_ac_type );	/* 付款账户类型 */
	set_zd_long( "0440", sMoDiscnt.tpn_date );	/* 转贴现日期 */
	set_zd_long( "0460", sMoDiscnt.return_date );	/* 回购日期 */
	set_zd_data( "0250", sMoDiscnt.tpn_name );	/* 票据号 */
	pub_base_strpack( sMoDiscnt.tpn_name );
	if( strlen( sMoDiscnt.tpn_name ) == 0)
		set_zd_data( "0250", "长沙市商业银行" );

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




