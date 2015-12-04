/*************************************************
* 文 件 名:  spB935.c
* 功能描述： 完成回购式转贴现转出到期回购功能，包括修改
*	     贴现登记簿、贴现网点提出/异地大额子交易、
*	     内部帐借方子交易（233取款）；
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

int spB935()
{
	struct	mo_discnt_c		sMoDiscnt;		/* 贴现登记薄 */
	char    cAccHrt[6];                     /* 同城科目控制字 */
	int		ret;
	double  T_amt=0.00;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );

	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0630", sMoDiscnt.tpact_no );		/* 转贴现协议编号 */

	/* 检查贴现登记簿中转贴现协议编号、贴现类型为回购转出、状态为正常的记录是否存在 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "tpact_no='%s' and sts!='*'", sMoDiscnt.tpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_pay_discnt_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此转贴现协议编号不存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B133" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	if( sMoDiscnt.pn_type[0] != '5' )
	{
		sprintf( acErrMsg, "此协议非回购式转贴现转出类型!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B166" );
		goto ErrExit;
	}

	if( sMoDiscnt.sts[0] == '1' )
	{
		sprintf( acErrMsg, "此笔贴现未记帐!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B163" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "此笔贴现已到期回购!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B160" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] != '3' )/***改为3 add by ligl 20100513***/
	{
		sprintf( acErrMsg, "此笔贴现回购未录入!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B327" );
		goto ErrExit;
	}

	/* 修改贴现登记薄，金额＝金额-还款金额=0，状态为销户 */
	strcpy( sMoDiscnt.sts, "*" );
	T_amt=sMoDiscnt.tranx_bal;
	sMoDiscnt.tranx_bal = 0;

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

	set_zd_data( "0370", sMoDiscnt.tin_ac_no );		/* 转贴现帐号 */
	get_zd_data( "0670", sMoDiscnt.get_ac_type );   /* 收款账户类型 */
	if( sMoDiscnt.get_ac_type[0] == '1' )
	{
		/* 取同城科目产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
		ret = pub_base_GetParm_Str( "TCKM", 2, cAccHrt );
		if( ret )
		{
			sprintf( acErrMsg, "取同城科目产品编号错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B257" );
			goto ErrExit;
		}

		/* 同城时传科目值到内部账贷方子交易 */
		set_zd_data( "0610", cAccHrt );
sprintf( acErrMsg, "zxxxyu    cAccHrt[%s]", cAccHrt );
WRITEMSG
	}

	set_zd_long("0450",sMoDiscnt.not_matr_date);        /* 票据到期日期，为授权所加 */

	/* 记流水日志 */
	/**---- xxx --- @20030818@ ---- 将协议编号付给帐号撤销时用 ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.tpact_no);
	strcpy(g_pub_tx.no_show,"1");
	g_pub_tx.tx_amt1=T_amt;
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
