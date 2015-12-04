/*************************************************
* 文 件 名:  spB804.c
* 功能描述： 完成发展再贷款还款记帐功能，修改再贷款
*	     登记薄的总还款金额＋＝当前还款金额。
*
* 作    者:  jane
* 完成日期： 2003年2月21日
*
* 修改记录： 
*   日   期: 20031011
*   修 改 人:
*   修改内容: 增加检查屏幕输入的帐号是否为人行准备金帐号
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "mo_again_ln_c.h"
#include "mo_pay_again_ln_c.h"

int spB804()
{
	struct	com_branch_c		sComBranch;		/* 公共机构码表 */
	struct	mo_again_ln_c		sMoAgainLn;		/* 再贷款登记薄 */
	struct	mo_pay_again_ln_c	sMoPayAgainLn;		/* 再贷款还款登记薄 */
	char zbj_ac_no[30];
	int	ret;

	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	memset( &sMoPayAgainLn, 0x00, sizeof( struct mo_pay_again_ln_c ) );
	memset( zbj_ac_no, 0x00, sizeof( zbj_ac_no ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0580", sMoAgainLn.pact_no );		/* 协议编号 */
	get_zd_double( "1001", &g_pub_tx.tmp_amt1 );	/* 当前还款金额 */
	get_zd_double( "1002", &g_pub_tx.tx_amt1 );		/* 当前还款利息 */
	vtcp_log("还款本金是[%.2f],利息是[%.2f]\n",g_pub_tx.tmp_amt1,g_pub_tx.tx_amt1);
	set_zd_double( "1218", g_pub_tx.tmp_amt1+g_pub_tx.tx_amt1);
	get_zd_data( "0310", zbj_ac_no );		/* 人行准备金帐号 */
        pub_base_strpack(zbj_ac_no);
	g_reply_int = sql_count( "mdm_ac_rel a,in_mst b", 
		"a.ac_no='%s' and b.ac_id=a.ac_id and b.ac_seqn=a.ac_seqn and b.prdt_no='902'", zbj_ac_no );
	if( g_reply_int < 0 )
	{
		sprintf( acErrMsg, "执行sql_count Error[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int == 0 )
	{
		strcpy( g_pub_tx.reply, "B324" );
		sprintf( acErrMsg, "帐号不是人行准备金帐号![%s][%d]", zbj_ac_no, ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 检查再贷款还款登记薄中协议编号、金额、状态为录入的纪录是否存在 */
	/*
	ret=  Mo_pay_again_ln_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and amt=%lf \
		and sts=0 ", sMoAgainLn.pact_no, g_pub_tx.tmp_amt1);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_pay_again_ln_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_pay_again_ln_Fet_Upd( &sMoPayAgainLn, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "再贷款还款登记薄中不存在此协议编号!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B109" );
        goto ErrExit;
   	}
	else if( ret )
    {
    	sprintf( acErrMsg, "执行Mo_pay_again_ln_Fet_Upd错![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }
	*/
	/* 修改再贷款登记薄（总还款金额＝总还款金额＋当前还款金额）,
	   检查是否已经全部还完，如果是，状态为2全部还款，否则仍为1正常 */
	ret = Mo_again_ln_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoAgainLn.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_again_ln_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_again_ln_Fet_Upd( &sMoAgainLn, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "再贷款登记薄中不存在此协议编号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B107" );
                goto ErrExit;
    }else if( ret )
    {
    	sprintf( acErrMsg, "执行Mo_again_ln_Fet_Upd错![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }
	/**----- xxx ---- @20030821@ ----- 取再借款帐号记账用 ----**/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_id=%ld ",sMoAgainLn.ac_id);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_again_ln_Dec_Upd错[%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	/**------------------------------------------------------**/
	set_zd_data("0300",g_mdm_ac_rel.ac_no);

	/* 修改再贷款还款登记簿,将还完的这笔贷款状态置为1全部还清 不用这个表了 liuxuan 
	strcpy( sMoPayAgainLn.sts, "1" );
	ret = Mo_pay_again_ln_Upd_Upd( sMoPayAgainLn, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改再贷款还款登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	Mo_pay_again_ln_Clo_Upd();
	*/

	/* 总还款金额＝总还款金额＋当前还款金额+ 利息 */
	sMoAgainLn.sum_amt = sMoAgainLn.sum_amt + g_pub_tx.tmp_amt1 +g_pub_tx.tx_amt1;
	ret = pub_base_CompDblVal( sMoAgainLn.sum_amt, sMoAgainLn.amt );
	if( ret >= 0 )
		strcpy( sMoAgainLn.sts, "2" );					/* 标志：2全部还款 */
	sMoAgainLn.pay_date=g_pub_tx.tx_date;				/* 还款日期 */
	sMoAgainLn.cur_amt=g_pub_tx.tmp_amt1 +g_pub_tx.tx_amt1; /* 当期还款额 */
	ret = Mo_again_ln_Upd_Upd( sMoAgainLn, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改再贷款登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	Mo_again_ln_Clo_Upd();

	/* 记流水日志 */
	/**----- xxx ---- @20030821@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoAgainLn.pact_no);
	strcpy(g_pub_tx.no_show,"1");
	sprintf(g_pub_tx.beg_note_no,"%.2f",g_pub_tx.tx_amt1);
	g_pub_tx.tx_amt1=g_pub_tx.tmp_amt1;
	vtcp_log("vvvvvvv g_pub_tx.tx_amt1[%.2f]\n",g_pub_tx.tx_amt1);
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
