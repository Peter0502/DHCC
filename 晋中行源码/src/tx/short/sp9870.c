/*************************************************
* 文 件 名:  sp9870.c
* 功能描述： 由再贴现协议编号回显贴现登记薄信息
*
* 作    者:  jack
* 完成日期： 2005年3月14日
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
#include "mdm_ac_rel_c.h"

int sp9870()
{
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	int	ret;
	char cAc_no[25];
	memset(cAc_no,0,sizeof(cAc_no));
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	
	get_zd_data( "0630", sMoDiscnt.rpact_no );	/* 再贴现协议编号 */
	/** add by LiuHuafeng 20061027 如果传来的协议号是空，那么看看账号是否存在，
		如果存在那么根据账号找到id号，然后在根据id号在找协议号 **/
	pub_base_strpack(sMoDiscnt.rpact_no);
	if(strlen(sMoDiscnt.rpact_no)==0)
	{
		struct mdm_ac_rel_c sMdm_ac_rel;
		memset(&sMdm_ac_rel, 0 , sizeof(sMdm_ac_rel));
		get_zd_data("0300",cAc_no);
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cAc_no);
		if(ret)
		{
			vtcp_log("%s,%d 查询账户介质关系表错误,ret=[%d]",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "账号不存在!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B242" );
			goto ErrExit;
		}
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "ac_id=%d ", sMdm_ac_rel.ac_id );
	}
	else
	{/* end by LiuHuafeng 2006-10-27 22:34 */
		/* 检查贴现登记薄中是否存在此贴现协议编号 */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "rpact_no='%s'", sMoDiscnt.rpact_no );
	}
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此再贴现协议编号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B138" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 向前台屏幕赋值 */
	if( sMoDiscnt.pn_type[0] == '6' )	/* 贴现类型  */
		set_zd_data( "0690", "1" );	/* 买断 */
	if( sMoDiscnt.pn_type[0] == '7' )	/* 贴现类型  */
		set_zd_data( "0690", "2" );	/* 回购 */
	set_zd_data( "0620", sMoDiscnt.pact_no );	/* 贴现协议编号 */
    	set_zd_double( "1002", sMoDiscnt.par_amt );	/* 汇票金额 */
	set_zd_double( "0850", sMoDiscnt.rtpn_rate );	/* 再贴现率 */
	/**
		set_zd_long( "0440", sMoDiscnt.out_date );	* 汇票出票日 *
		set_zd_data( "0580", sMoDiscnt.payer_brno );* 付款行行号 *
		set_zd_data( "0590", sMoDiscnt.payee_brno );收款人开户行行号
		set_zd_data( "0330", sMoDiscnt.in_ac_no ); * 客户帐号 *
	**/
	set_zd_long( "0450", sMoDiscnt.not_matr_date );/* 汇票到期日 */
	set_zd_data( "0310", sMoDiscnt.rem_ac_no );	/* 出票人帐号 */
	set_zd_data( "0260", sMoDiscnt.remitter );	/* 出票人全称 */
	set_zd_data( "0810", sMoDiscnt.payer_brno_name );/* 付款行全称 */
	set_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* 收款人帐号 */
	set_zd_data( "0270", sMoDiscnt.payee );		/* 收款人全称 */
	set_zd_data( "0820", sMoDiscnt.payee_brno_name );/* 收款人开户行全称 */

	set_zd_data( "0900", sMoDiscnt.note_type );	/* 票据类型 */
	set_zd_data( "0600", sMoDiscnt.pnote_no );	/* 票据号 */

	set_zd_data( "0370", sMoDiscnt.rin_ac_no );	/* 再贴现账户 */
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* 异地同城标志 */
	set_zd_long( "0480", sMoDiscnt.way_days );	/* 在途天数 */
	set_zd_double( "1001", sMoDiscnt.pn_int );	/* 贴现利息 */
	set_zd_data("0910",sMoDiscnt.br_no);

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
