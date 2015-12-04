/*************************************************
* 文 件 名:  sp9934.c
* 功能描述： 由票据类型和票据号回显贴现信息
*
* 作    者:  jane
* 完成日期： 2003年6月21日
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

int sp9934()
{
	struct	mo_discnt_c	sMoDiscnt1,sMoDiscnt2;	/* 贴现登记薄 */
	int	ret;
	char cKinbr[BRNO_LEN+1];
	memset(cKinbr, 0 , sizeof(cKinbr));

	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt2, 0x00, sizeof( struct mo_discnt_c ) );

	get_zd_data( "0900", sMoDiscnt1.note_type );	/* 票据类型 */
	get_zd_data( "0600", sMoDiscnt1.pnote_no );	/* 票据号 */
	get_zd_data("0030",cKinbr);

	/* 检查贴现登记薄中是否存在此票据 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "note_type='%s' \
			and pnote_no='%s' and pn_type='4' and br_no ='%s' order by wrk_date desc ", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no ,cKinbr);
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此票据信息!01" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B130" );
        goto ErrExit;
   	}else if( ret )
   	{
      	sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

sprintf(acErrMsg, "pn_type=[%s], [%s]", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no );
WRITEMSG

	/* 再检查贴现登记薄中此票据的基本信息 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt2, "note_type='%s' \
			and pnote_no='%s' and (pn_type='1' or pn_type='2' ) and br_no='%s' order by wrk_date desc ",\ /*神啊，这个先这样改一下吧*/
			sMoDiscnt1.note_type, sMoDiscnt1.pnote_no,cKinbr );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此票据信息!02" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B130" );
                goto ErrExit;
     }else if( ret )
     {
     	sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
        WRITEMSG
        goto ErrExit;
     }

sprintf(acErrMsg, "pn_type=[%s], [%s]", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no );
WRITEMSG
	 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_id=%ld and ac_seqn=0 ",sMoDiscnt2.ac_id);
	 if( ret )
	 {
			sprintf( acErrMsg, "贴现登记薄中不存在此票据信息!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "B130" );
            goto ErrExit;
	 }

	/* 向前台屏幕赋值 */
	set_zd_data( "0330" , g_mdm_ac_rel.ac_no );		/* 贷款帐号 */
    set_zd_double( "1002", sMoDiscnt2.par_amt );	/* 汇票金额 */
	set_zd_long( "0450", sMoDiscnt2.not_matr_date );/* 汇票到期日 */
	set_zd_data( "0310", sMoDiscnt2.rem_ac_no );	/* 出票人帐号 */
	set_zd_data( "0260", sMoDiscnt2.remitter );	/* 出票人全称 */
	set_zd_data( "0810", sMoDiscnt2.payer_brno_name );/* 付款行全称 */
	set_zd_data( "0320", sMoDiscnt2.payee_ac_no );	/* 收款人帐号 */
	set_zd_data( "0270", sMoDiscnt2.payee );		/* 收款人全称 */
	set_zd_data( "0820", sMoDiscnt2.payee_brno_name );/* 收款人开户行全称 */
	set_zd_data( "0680", sMoDiscnt1.local_ind );	/* 异地同城标志 */
	set_zd_double( "0850", sMoDiscnt1.tpn_rate );	/* 转贴现利率 */
	set_zd_double( "1001", sMoDiscnt1.tpn_int );	/* 转贴现利息 */
	set_zd_data( "0250", sMoDiscnt1.tpn_name );	/* 申请人名称 */

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
