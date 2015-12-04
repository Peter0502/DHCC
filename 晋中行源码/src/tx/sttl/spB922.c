/*************************************************
* 文 件 名:  spB922.c
* 功能描述： 完成转贴现买断转入记账功能,分别挂提出挂账子交易、
*	    异地大额子交易;开贴现户(即贷款户129);修改登记簿状态。
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

static	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */

int spB922()
{
	char    cAccHrt[6];                     /* 同城科目控制字 */
	int		ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset ( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );		/* 贷款主文件 */
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );

	/* 数据初始化 */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*空介质介质代码*/
	get_zd_data( "0620", sMoDiscnt.pact_no );		/* 转贴现协议编号 */
	

	/* 根据转贴现协议编号查询贴现登记簿 */
	/**------- xxx ------- @20030817@ ---------**
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此转贴现协议编号不存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B133" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
	***----------------------------------------***/
	/* 根据转贴现协议编号和贴现类型修改贴现登记簿 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s' ",sMoDiscnt.pact_no);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此转贴现协议编号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B133" );
		goto ErrExit;
	}else if( ret )
   	{
		sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret );
		WRITEMSG
		goto ErrExit;
 	}
	get_zd_data( "0700", sMoDiscnt.get_ac_type );   /* 收款账户类型 */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* 流水日期 */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* 主机流水号 */

	if( sMoDiscnt.pn_type[0] != '2' )
	{
		sprintf( acErrMsg, "此协议非买断式转贴现转入类型!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B162" );
		goto ErrExit;
	}

	if( sMoDiscnt.sts[0] == '3' )
	{
		sprintf( acErrMsg, "此笔贴现已记帐!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B148" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "此笔贴现已还款!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	/* 调用贷款开户函数，产生帐号id，帐号序号 */
	if( ln_open_ac() )
	{
		sprintf(acErrMsg,"调用贷款开户函数错误!");
		WRITEMSG
		goto ErrExit;
	}

	/* 修改贴现登记薄，状态为正常 */
	sMoDiscnt.ac_id = g_ln_mst.ac_id;		/* 贷款账号ID */
	sMoDiscnt.ac_seqn = g_ln_mst.ac_seqn;	/* 贷款账号序号 */
	strcpy( sMoDiscnt.sts, "3" );			/* 标志：3正常 */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_discnt_Clo_Upd();

	/* 下面这个同城用不到，而且也不能用 * 前台给61域赋值了
	if( sMoDiscnt.get_ac_type[0] == '1' )
	{
		* 取同城科目产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) *
		ret = pub_base_GetParm_Str( "TCKM", 2, cAccHrt );
		if( ret )
		{
			sprintf( acErrMsg, "取同城科目产品编号错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B257" );
			goto ErrExit;
		}
		* 同城时传科目值到内部账贷方子交易 *
		set_zd_data( "0610", cAccHrt );
		sprintf( acErrMsg, "zxxxyu    cAccHrt[%s]", cAccHrt );
		WRITEMSG
	}
	*/
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

int ln_open_ac()
{
	long	lMtrDate, lWayDays, lTempDays;
	int		ret1;

	get_zd_data("0620",g_ln_mst.pact_no);		/* 合同号(此处为贴现协议编号) */
	get_zd_long("0450",&lMtrDate); 				/* 票据到期日期 */
	get_zd_long("0480",&lWayDays); 				/* 在途天数 */

	lTempDays = lWayDays ;					/* 贴现最后还款日期在票据到期日+在途天数之后再加上5天 */

	ret1 = pub_base_deadlineD( lMtrDate, lTempDays, &g_ln_mst.mtr_date );
	if( ret1 )
	{
		sprintf( acErrMsg, "计算到期日期错误!" );
		WRITEMSG
		return 1;
	}
/**	int	ret1; *****JYW*****/

	get_zd_data("0620",g_ln_mst.pact_no);		/* 合同号(此处为转贴现协议编号) */
	get_zd_long("0450",&g_ln_mst.mtr_date); 	/**到期日期**/
sprintf(acErrMsg,"到期日期=[%ld]",g_ln_mst.mtr_date);
WRITEMSG
	get_zd_double("0840",&g_ln_mst.rate); 		/**利率**/
	get_zd_data("0250",g_pub_tx.name); 			/**户名**/
	get_zd_double("1002",&g_ln_mst.amt_lmt);	/* 计划发放额，贷款限额 */

	/* Pub_tx结构赋值 */
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );		/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.add_ind , "1" );			/* 增减1加 */	
	strcpy ( g_pub_tx.ct_ind , "2" );			/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );			/* 是否打印存折(有折无折标志)0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );			/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	g_pub_tx.svc_ind = 3001;
	strcpy(g_pub_tx.brf,"转贴现转入开户");
	strcpy( g_pub_tx.prdt_code, sMoDiscnt.bkrl_code );/* 产品代码 */

	/* 根据贷款产品编号取贷款参数 */
	ret1 = Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", sMoDiscnt.bkrl_code );
	if( ret1 == 100 )
	{
		sprintf( acErrMsg, "此贴现产品编号不存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B240" );
		return 1;
   	}else if( ret1 )
   	{
   		sprintf( acErrMsg, "执行Ln_parm_Sel错![%d]", ret1 );
		WRITEMSG
		return 1;
   	}

	strcpy( g_pub_tx.intst_type, g_ln_parm.intst_type );	/* 计息类型 */
	strcpy( g_ln_mst.repay_type, g_ln_parm.ln_pay_type );	/* 贷款支付方式 */
	/**add by jane 2003-09-02********/
	strcpy( g_ln_mst.cal_code , "0000000000" );	/* 指标体系代码 */
	strcpy( g_ln_mst.cmpd_intst_ind , g_ln_parm.cmpd_intst_ind );	/*是否记复利标志*/

	/* 贴现账号开介质 */
	ret1 = pub_acct_opn_discnt_mdm();
	if( ret1 )
	{
		sprintf(acErrMsg,"开介质错误![%d]",ret1);
		WRITEMSG
		return 1;
	}

	/***开户***/
	ret1 = pub_acct_opn_ac();
	if( ret1 )
	{
		sprintf(acErrMsg,"开户错误![%d]",ret1);
		WRITEMSG
		return 1;
	}

	/*** 输出变量 ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("0330",g_pub_tx.ac_no);

	return 0;
}
