/*************************************************
* 文 件 名:  spB803.c
* 功能描述： 完成再贷款还款信息录入功能，登记再贷款
*		 还款登记薄。
*
* 作	者:  jane
* 完成日期： 2003年8月23日
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
#include "mo_again_ln_c.h"
#include "mo_pay_again_ln_c.h"

int spB803()
{
	struct	mo_again_ln_c		sMoAgainLn;		/* 再贷款登记薄 */
	struct	mo_pay_again_ln_c	sMoPayAgainLn;	/* 再贷款还款登记薄 */
	double	dAmt=0;								/* 当前还款金额 */
	double	dSumAmt=0;							/* 还款总额 */
	int		ret;
	long	days=0;		/** 计算利息得天数 **/
	long	date;		/** 交易日期 **/
	double  dLx;		/** 计算出来得利息 **/

	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	memset( &sMoPayAgainLn, 0x00, sizeof( struct mo_pay_again_ln_c ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0580", sMoPayAgainLn.pact_no );	/* 协议编号 */
	get_zd_double( "1001", &dAmt );					/* 还款本金 */
	get_zd_long( "0050", &date );			/* 交易日期 */
 vtcp_log("%s,%d,date=[%d]\n",__FILE__,__LINE__,date);

	/* 检查再贷款还款登记薄中是否存在此协议编号并且状态为录入 */
	ret = Mo_again_ln_Sel( g_pub_tx.reply, &sMoAgainLn, "pact_no='%s'",\
					 sMoPayAgainLn.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "再贷款登记薄中不存在此协议编号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B118" );
		goto ErrExit;
	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_again_ln_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( sMoAgainLn.sts[0] == '0' )
	{
		sprintf( acErrMsg, "此笔再贷款还未记账!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B118" );
		goto ErrExit;
	}else if( sMoAgainLn.sts[0] == '2' )
	{
		sprintf( acErrMsg, "此笔再贷款已全部还清!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "" );
		goto ErrExit;
	}

	/* 检查已还款总金额+本次还款金额是否超过贷款金额 */
	dSumAmt = sMoAgainLn.sum_amt + dAmt;
	if ( pub_base_CompDblVal( dSumAmt, sMoAgainLn.amt ) == 1 )
	{
		sprintf( acErrMsg, "已还款总金额超过贷款金额!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B283" );
		goto ErrExit;
	}

	/* 检查再贷款还款登记薄中协议编号状态为录入的纪录是否存在 */
	ret = Mo_pay_again_ln_Sel( g_pub_tx.reply, &sMoPayAgainLn, "pact_no='%s'\
							and sts=0 ", sMoPayAgainLn.pact_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "此协议有还款录入但未记账记录!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B290" );
        goto ErrExit;
   	}
	else if( ret != 100 && ret )
    {
    	sprintf( acErrMsg, "执行Mo_pay_again_ln_Sel错![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }


	/** 添加计算利息 **/
	days = pub_base_CalTriDays(sMoAgainLn.reg_date,date);
 vtcp_log("%s,%d,计算得的天数=[%d]\n",__FILE__,__LINE__,days);
 vtcp_log("%s,%d,利率=[%f]\n",__FILE__,__LINE__,sMoAgainLn.rate);
	dLx = days * dAmt * sMoAgainLn.rate / 30000;
 vtcp_log("%s,%d,计算得的利息=[%f]\n",__FILE__,__LINE__,dLx);


	/* 登记再贷款还款登记薄，标志为录入 */
	sMoPayAgainLn.intst_amt = dLx;
	sMoPayAgainLn.amt = dAmt;					/* 还款金额 */
	strcpy( sMoPayAgainLn.sts, "0" );			/* 标志：0录入 */
	sMoPayAgainLn.pay_date = g_pub_tx.tx_date;	/* 还款日期 */

	ret = Mo_pay_again_ln_Ins( sMoPayAgainLn, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_pay_again_ln_Ins错误!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_double("0390", dLx);

	/* 记流水日志 */
	/**----- xxx ---- @20030821@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoAgainLn.pact_no);
	g_pub_tx.tx_amt1=dAmt;
	sprintf(g_pub_tx.beg_note_no,"%.2f",sMoPayAgainLn.intst_amt);
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
