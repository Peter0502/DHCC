/*************************************************
* 文 件 名:  spB932.c
* 功能描述： 完成买断式转贴现记账（转出）功能，分别挂
*	     提人挂账子交易（rob）、异地大额子交易
*	    （暂无，以NULL代替）；开内部帐户233；内部帐
*	     贷方记帐、内部帐借方记帐、贷款还款子交易，
*	     修改登记簿状态。
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
#include "com_parm_c.h"

int spB932()
{
	struct	mo_discnt_c		sMoDiscnt,sMoDiscnt1;	/* 贴现登记薄 */
	struct	mdm_ac_rel_c	sMdmAcRel;				/* 介质与账户关系表 */
	struct 	com_parm_c 		sComParm;				/* 公共参数 */
	char	cTinAcNo[11];							/* 转贴现帐号 */
	double	dAmt1, dAmt2, dAmt3;
	int		ret;
   char    cKinbr[BRNO_LEN+1];
	char	cPn_knd[2];							/* 电子票标志 */

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cTinAcNo, 0x00, sizeof( cTinAcNo ) );
	memset( cPn_knd, 0x00, sizeof( cPn_knd ) );
	memset(cKinbr      , 0x00, sizeof(cKinbr));

	/* 数据初始化 */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );	/*空介质介质代码*/

	/* 从前台屏幕取值 */
	get_zd_data( "0900", sMoDiscnt.note_type );		/* 票据类型 */
	get_zd_data( "0600", sMoDiscnt.pnote_no );		/* 票据号 */
	get_zd_data( "0370", cTinAcNo );				/* 转贴现帐号 */
	get_zd_data( "0670", sMoDiscnt.tpay_ac_type );  /* 付款账户类型 */
	get_zd_data( "0980", cPn_knd );		/* 票据号 */
  get_zd_data("0910", cKinbr);
	if(cPn_knd[0]=='E')
	get_zd_data("1032",sMoDiscnt.pnote_no);

sprintf( acErrMsg, "cTinAcNo=[%s]", cTinAcNo );
WRITEMSG
	/* 检查票据是否存在，状态是否为录入，贴现类型是否为买断转出,修改贴现登记簿 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "note_type='%s' and pnote_no='%s' \
				and pn_type='4' and sts!='*' ", sMoDiscnt.note_type, sMoDiscnt.pnote_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此票据未作买断式转贴现转出!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B249" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	if( sMoDiscnt.sts[0] == '3' )
	{
		sprintf( acErrMsg, "此笔贴现已记帐!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B148" );
		goto ErrExit;
	}

	/* 根据票据类型和票据号查询贴现登记薄中票据录入时的信息 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "note_type='%s' and pnote_no='%s' \
		and (pn_type='1' or pn_type='2') and sts='*' ", sMoDiscnt.note_type,sMoDiscnt.pnote_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此票据信息!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	/* 根据贷款账号ID和账号序号查询贷款账号，为贷款还款作准备 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_id=%ld and ac_seqn=%d", \
				sMoDiscnt1.ac_id, sMoDiscnt1.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "介质账号对照表中不存在此账号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
	set_zd_data( "0330", sMdmAcRel.ac_no );
	set_zd_long( "0340", sMoDiscnt1.ac_seqn );

	/* 修改贴现登记薄，状态为正常 */
	strcpy( sMoDiscnt.tin_ac_no, cTinAcNo );	/* 转贴现帐号 */
	strcpy( sMoDiscnt.sts, "*" );				/* 标志：* 销户 */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* 流水日期 */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* 主机流水号 */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

	/* 为给大额J003前台传金额用 */
	get_zd_double( "1001", &dAmt1 );		/* 转贴现利息 */
	get_zd_double( "1002", &dAmt2 );		/* 汇票金额 */
	dAmt3 = dAmt2 - dAmt1;					/* 提入金额 */
	set_zd_double( "1003", dAmt3 );

	/* 记流水日志 */
	strcpy(g_pub_tx.cur_no,"01");
	strcpy(g_pub_tx.no_show,"1");
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
