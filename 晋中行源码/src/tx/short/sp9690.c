/*************************************************
* 文 件 名:  sp9690.c
* 功能描述： 由贴现协议编号回显贴现登记薄信息
*
* 作    者:  rob 
* 完成日期： 2003年9月27日
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

int sp9690()
{
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* 介质与账户关系表 */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	
	get_zd_data( "0630", sMoDiscnt.pact_no );	/* 贴现协议编号 */
vtcp_log( "zy   pact_no[%s]",sMoDiscnt.pact_no );
	/* 检查贴现登记薄中是否存在此贴现协议编号 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此贴现协议编号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B113" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
vtcp_log( "zy    ac_id[%ld]  ac_seqn[%d]",sMoDiscnt.ac_id, sMoDiscnt.ac_seqn );
	/* 查询贴现帐号 *
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_id=%ld and ac_seqn=%d",\
				sMoDiscnt.ac_id, sMoDiscnt.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现帐号不存在!" );
   		WRITEMSG
   		strcpy( g_pub_tx.reply, "B242" );
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}*/

	/* 向前台屏幕赋值 */
/*
	if( sMoDiscnt.pn_type[0] == '2' || sMoDiscnt.pn_type[0] == '4' )	* 贴现类型  *
		set_zd_data( "0690", "1" );			* 买断转入（转出） *
	if( sMoDiscnt.pn_type[0] == '3' || sMoDiscnt.pn_type[0] == '5' )	* 贴现类型  *
		set_zd_data( "0690", "2" );			* 回购转入（转出） */
    	set_zd_double( "1002", sMoDiscnt.par_amt );	/* 汇票金额 */
	set_zd_double( "0840", sMoDiscnt.pn_rate );	/* 贴现率 */
	set_zd_double( "0850", sMoDiscnt.tpn_rate );	/* 转贴现率 */
	set_zd_double( "1005", sMoDiscnt.rtpn_rate );	/* 再贴现率 */
	set_zd_long( "0450", sMoDiscnt.not_matr_date );/* 汇票到期日 */
	set_zd_data( "0310", sMoDiscnt.rem_ac_no );	/* 出票人帐号 */
	set_zd_data( "0260", sMoDiscnt.remitter );	/* 出票人全称 */
	set_zd_data( "0810", sMoDiscnt.payer_brno_name );/* 付款行全称 */
	set_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* 收款人帐号 */
	set_zd_data( "0270", sMoDiscnt.payee );		/* 收款人全称 */
	set_zd_data( "0820", sMoDiscnt.payee_brno_name );/* 收款人开户行全称 */
	set_zd_data( "0900", sMoDiscnt.note_type );	/* 票据类型 */
	set_zd_data( "0600", sMoDiscnt.pnote_no );	/* 票据号 */
	set_zd_data( "0670", sMoDiscnt.ac_type );	/* 收款账户类型 */
	set_zd_data( "0830", sMoDiscnt.pn_ac_no );	/* 贴现申请人账号 */
	set_zd_data( "0250", sMoDiscnt.pn_name );	/* 贴现申请人户名 */
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* 异地同城标志 */
	set_zd_long( "0480", sMoDiscnt.way_days );	/* 在途天数 */
	set_zd_double( "1001", sMoDiscnt.pn_int );	/* 贴现利息 */
	set_zd_long( "0440", sMoDiscnt.pn_date );	/* 转贴现日期 */
	set_zd_long( "0460", sMoDiscnt.return_date );	/* 回购日期 */
	set_zd_data( "0330", sMdmAcRel.ac_no );		/* 贴现账号 */

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
