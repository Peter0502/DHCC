/*************************************************
* 文 件 名:  spB941.c
* 功能描述： 完成买断式再贴现信息录入功能，同买断式
*	     转贴现，但是记账时不归还原129账户，对方只有同城。
*
* 作    者:  jane
* 完成日期： 2003年4月15日
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
#include "mo_discnt_c.h"

int spB941()
{
	struct	mo_discnt_c	sMoDiscnt1, sMoDiscnt2;	/* 贴现登记薄 */
	long	lPnDays;				/* 贴现天数 */
	int	ret;
	char	payName[51];

	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt2, 0x00, sizeof( struct mo_discnt_c ) );
	memset( payName, 0x00, sizeof( payName ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0620", sMoDiscnt1.rpact_no );	/* 再贴现协议编号 */
	strcpy( sMoDiscnt1.br_no, g_pub_tx.tx_br_no );	/* 机构编码 */
	get_zd_double( "1002", &sMoDiscnt1.par_amt );	/* 汇票金额 */
	get_zd_double( "0840", &sMoDiscnt1.rtpn_rate );	/* 再贴现率 */
	sMoDiscnt1.rtpn_date = g_pub_tx.tx_date;	/* 再贴现日 */
	get_zd_long( "0450", &sMoDiscnt1.not_matr_date );/* 汇票到期日 */
	get_zd_data( "0310", sMoDiscnt1.rem_ac_no );	/* 出票人帐号 */
	get_zd_data( "0260", sMoDiscnt1.remitter );	/* 出票人全称 */
	get_zd_data( "0810", sMoDiscnt1.payer_brno_name );/* 付款行全称 */
	get_zd_data( "0320", sMoDiscnt1.payee_ac_no );	/* 收款人帐号 */
	get_zd_data( "0270", sMoDiscnt1.payee );	/* 收款人全称 */
/*字段长度有问题	get_zd_data( "0820", sMoDiscnt1.payee_brno_name ); 收款人开户行全称 */
	get_zd_data( "0820", payName );/* 收款人开户行全称 */
	memcpy(sMoDiscnt1.payee_brno_name,payName,sizeof(sMoDiscnt1.payee_brno_name)-1);
	get_zd_data( "0900", sMoDiscnt2.note_type );	/* 票据类型 */
	get_zd_data( "0600", sMoDiscnt2.pnote_no );	/* 票据号 */
	get_zd_long( "0480", &sMoDiscnt1.way_days );	/* 在途天数 */

	/* 检查贴现登记薄中是否已经存在此再贴现协议编号 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "rpact_no='%s'", sMoDiscnt1.rpact_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "贴现登记薄中已经存在此再贴现协议编号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B137" );
                goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 根据票据类型和票据号找出记录 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "note_type='%s' and pnote_no='%s'",\
			 	sMoDiscnt2.note_type, sMoDiscnt2.pnote_no );
	if( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt2, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此票据信息!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B130" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 检查票据状态是否为正常，且贴现类型为贴现或者买断转入 */
	if( sMoDiscnt2.pn_type[0] != '1' && sMoDiscnt2.pn_type[0] != '2' )
	{
		sprintf( acErrMsg, "此票据非贴现或买断转入所得，不能做买断式再贴现!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B168" );
		goto ErrExit;
	}

	if( sMoDiscnt2.pn_type[0] == '1' )	/* 贴现 */
	{
		if( sMoDiscnt2.sts[0] == '1' )
		{
			sprintf( acErrMsg, "此笔贴现未审核!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B147" );
			goto ErrExit;
		}else if( sMoDiscnt2.sts[0] == '2' )
		{
			sprintf( acErrMsg, "此笔贴现未记账!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	else					/* 买断转入 */
	{
		if( sMoDiscnt2.sts[0] == '1' )
		{
			sprintf( acErrMsg, "此笔贴现未记账!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	if( sMoDiscnt2.sts[0] == '*' )
	{
		sprintf( acErrMsg, "此笔贴现已做还款!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	/* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
	ret = pub_base_GetParm_Str( "TXCP", 4, sMoDiscnt1.bkrl_code );
	if( ret )
	{
		sprintf( acErrMsg, "取贴现产品编号错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}

	/* 计算贴现利息：其中，贴现天数＝ 票据到期日 -再贴现日期+ 在途天数,
		再贴现利息 ＝ 汇票金额 * 贴现天数 * （月再贴现率/30天/1000）） */
	ret = pub_base_CalDays( sMoDiscnt1.rtpn_date, sMoDiscnt1.not_matr_date, "0", &lPnDays );
	if( ret )
	{
		sprintf( acErrMsg, "计算贴现天数错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}

	sMoDiscnt1.rpn_int = sMoDiscnt1.par_amt * (lPnDays + sMoDiscnt1.way_days) * (sMoDiscnt1.rtpn_rate/30/1000);

	set_zd_double( "1001", sMoDiscnt1.rpn_int );		/* 再贴现利息 */

	/* 登记贴现登记薄，状态为录入 */
	strcpy( sMoDiscnt1.note_type, sMoDiscnt2.note_type );	/* 票据类型 */
	strcpy( sMoDiscnt1.pnote_no, sMoDiscnt2.pnote_no );	/* 票据号 */
	strcpy( sMoDiscnt1.sts, "1" );				/* 标志：1录入 */
	strcpy( sMoDiscnt1.pn_type, "6" );			/* 贴现类型: 6买断再贴现 */
	sMoDiscnt1.tranx_bal = sMoDiscnt1.par_amt;		/* 金额 */

	ret = Mo_discnt_Ins( sMoDiscnt1, g_pub_tx.reply );
	if ( ret )
    	{
        	sprintf( acErrMsg, "执行Mo_discnt_Ins错误!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
    	}

	/* 修改贴现登记簿原贴现纪录 */
	strcpy( sMoDiscnt2.sts, "*" );		/* 标志：* 销户 */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt2, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

    strcpy(g_pub_tx.ac_no,sMoDiscnt1.rpact_no);
    g_pub_tx.tx_amt1=sMoDiscnt1.par_amt;
    strcpy(g_pub_tx.note_type,sMoDiscnt1.note_type);
    strcpy(g_pub_tx.beg_note_no,sMoDiscnt1.pnote_no);
    strcpy(g_pub_tx.brf,"买断式再贴现录入");

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
