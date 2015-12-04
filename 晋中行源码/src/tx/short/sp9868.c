/*************************************************
* 文 件 名:  sp9868.c
* 功能描述： 由票据类型和票据号回显贴现信息
*
* 作    者:  jane
* 完成日期： 2003年3月7日
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

int sp9868()
{
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );

	get_zd_data( "0900", sMoDiscnt.note_type );	/* 票据类型 */
	get_zd_data( "0600", sMoDiscnt.pnote_no );	/* 票据号 */

	/* 检查贴现登记薄中是否存在此票据 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "note_type='%s' \
			and pnote_no='%s' and (pn_type='1' or pn_type='2')",\
			sMoDiscnt.note_type, sMoDiscnt.pnote_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此票据信息!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B130" );
                goto ErrExit;
   	}
	else if( ret )
    {
    	sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

	/* 向前台屏幕赋值 */
	set_zd_data( "0620", sMoDiscnt.pact_no );	/* 贴现协议编号 */
    set_zd_double( "1002", sMoDiscnt.par_amt );	/* 汇票金额 */
	set_zd_long( "0450", sMoDiscnt.not_matr_date );/* 汇票到期日 */
	set_zd_data( "0310", sMoDiscnt.rem_ac_no );	/* 出票人帐号 */
	set_zd_data( "0260", sMoDiscnt.remitter );	/* 出票人全称 */
	set_zd_data( "0810", sMoDiscnt.payer_brno_name );/* 付款行全称 */
	set_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* 收款人帐号 */
	set_zd_data( "0270", sMoDiscnt.payee );		/* 收款人全称 */
	set_zd_data( "0820", sMoDiscnt.payee_brno_name );/* 收款人开户行全称 */
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* 异地同城标志 */
	set_zd_double( "0850", sMoDiscnt.tpn_rate );	/* 转贴现利率 */
	set_zd_double( "1001", sMoDiscnt.tpn_int );	/* 转贴现利息 */
	set_zd_data( "0250", sMoDiscnt.tpn_name );	/* 申请人名称 */
	/*20101217 yanqq 回显持票机构号*/
	set_zd_data( "0910", sMoDiscnt.br_no );	/* 持票机构编号 */

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
