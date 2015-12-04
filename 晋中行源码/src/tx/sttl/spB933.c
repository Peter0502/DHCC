/*************************************************
* 文 件 名:  spB933.c
* 功能描述： 完成回购式转贴现信息录入（转出）功能。
*
* 作    者:  jane
* 完成日期： 2003年4月14日
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

int spB933()
{
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	struct	mo_discnt_c	sMoDiscnt1;	/* 贴现登记薄 */
	long	lPnDays;				/* 贴现天数 */
	int	ret;
	char	cOperCode[2];				/* 操作码 */

	/* 数据初始化 */
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0690", cOperCode );				/* 操作码 */
	get_zd_data( "0630", sMoDiscnt.tpact_no );		/* 转贴现协议编号 */
	strcpy( sMoDiscnt.br_no, g_pub_tx.tx_br_no );	/* 机构编码 */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* 转贴现信息录入时交易日期 */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* 录入流水号 */
	get_zd_double( "0850", &sMoDiscnt.tpn_rate );	/* 转贴现率 */
	get_zd_double( "1002", &sMoDiscnt.par_amt );	/* 汇总票据金额 */
	get_zd_long( "0440", &sMoDiscnt.tpn_date );		/* 转贴现日期 */
	get_zd_long( "0460", &sMoDiscnt.return_date );	/* 回购日期 */
	get_zd_data( "0250", sMoDiscnt.tpn_name );		/* 申请人名称 */
	pub_base_strpack(sMoDiscnt.tpact_no);

	if( cOperCode[0] == '0' )			/* 录入 */
	{
		/* 检查贴现登记薄中是否已经存在此转贴现协议编号 */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "tpact_no='%s'", sMoDiscnt.tpact_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "贴现登记薄中已经存在此转贴现协议编号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B132" );
			goto ErrExit;
   		}
		else if( ret != 100 )
   		{
   			sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
   		}
   	}
	else if( cOperCode[0] == '1' || cOperCode[0] == '2' )			/* 修改,删除 */
	{
		vtcp_log( "tpact_no[%s]", sMoDiscnt.tpact_no );
		/* 检查贴现登记薄中是否存在此贴现协议编号 */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "tpact_no='%s'",sMoDiscnt.tpact_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "贴现登记薄中不存在此转贴现协议编号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B113" );
			goto ErrExit;
       	}
		else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
vtcp_log( "pn_type[%s]", sMoDiscnt1.pn_type );
		if( sMoDiscnt1.pn_type[0] != '5' )
		{
			sprintf( acErrMsg, "非贴现类型!pn_type[%s]", sMoDiscnt1.pn_type );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B276" );
			goto ErrExit;
		}

		/* 检查此贴现协议状态 */
		if( sMoDiscnt1.sts[0] == '2' )
		{
			sprintf( acErrMsg, "此贴现协议已审核!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B152" );
			goto ErrExit;
		}else if( sMoDiscnt1.sts[0] == '3' )
		{
			sprintf( acErrMsg, "此笔贴现已记帐!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B148" );
			goto ErrExit;
		}else if( sMoDiscnt1.sts[0] == '*' )
		{
			sprintf( acErrMsg, "此笔贴现已还款!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B149" );
			goto ErrExit;
		}

		/* 删除此贴现协议编号对应的纪录 */
		ret=sql_execute("DELETE from mo_discnt WHERE tpact_no='%s'",sMoDiscnt.tpact_no );
		if( ret ) 
		{
      		sprintf( acErrMsg, "执行sql_execute错![%d]", ret );
			WRITEMSG
			goto ErrExit;
      	}

		if( cOperCode[0] == '2' )		/*** 删除 ***/
		{
      		sprintf( acErrMsg, "删除成功" );
			WRITEMSG
			goto OkExit;
		}
	}

	/* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
	ret = pub_base_GetParm_Str( "TXCP", 3, sMoDiscnt.bkrl_code );
	if( ret )
	{
		sprintf( acErrMsg, "取贴现产品编号错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}

	/* 计算贴现利息：贴现利息 ＝ 汇总票据金额*(回购日期-转贴日期*月转贴现率/360/100)*/
	ret = pub_base_CalDays( sMoDiscnt.tpn_date, sMoDiscnt.return_date, "0", &lPnDays );
	if( ret )
	{
		sprintf( acErrMsg, "计算贴现天数错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}

	sMoDiscnt.tpn_int = sMoDiscnt.par_amt * lPnDays * (sMoDiscnt.tpn_rate/360/100);
	get_zd_double( "1001", &sMoDiscnt.tpn_int );		/* 转贴现利息 */
	set_zd_double( "1001", sMoDiscnt.tpn_int );		/* 转贴现利息 */
	/* 登记贴现登记薄，状态为录入 */
	strcpy( sMoDiscnt.sts, "1" );				/* 标志：1录入 */
	strcpy( sMoDiscnt.pn_type, "5" );			/* 贴现类型: 5 回购转出 */
	sMoDiscnt.tranx_bal = sMoDiscnt.par_amt;	/* 金额 */

	ret = Mo_discnt_Ins( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
   	{
       	sprintf( acErrMsg, "执行Mo_discnt_Ins错误!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
		
	/* 记流水日志 */
	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.tpact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "回购式转贴现转出录入" );
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
