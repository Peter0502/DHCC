/*************************************************
* 文 件 名:  spB802.c
* 功能描述： 完成再贷款记账功能。登记再贷款登记薄、
*	         开再贷款内部帐、内部帐存款三个子交易。
*	         本处完成登记再贷款登记薄功能。
*
* 作    者:  jane
* 完成日期： 2003年7月23日
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
#include "com_branch_c.h"
#include "mo_again_ln_c.h"

int spB802()
{
	struct	com_branch_c	sComBranch;	/* 公共机构码表 */
	struct	mo_again_ln_c	sMoAgainLn;	/* 再贷款登记薄 */
	struct	in_parm_c		sInParm;	/* 内部产品参数表 */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* 介质与账户关系 */
	int	ret;

	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	memset( &sInParm, 0x00, sizeof( struct in_parm_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0580", sMoAgainLn.pact_no );		/* 协议编号 */
	get_zd_data( "0300", g_pub_tx.ac_no );			/* 生成的内部帐号 */

	/* 检查再贷款登记薄中是否存在此协议编号 */
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
	if(sMoAgainLn.sts[0]!='0'){
		sprintf( acErrMsg, "该笔再贷款非录入状态，请先撤销记账交易!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "B285" );
		goto ErrExit;
	}

	/* 根据内部帐开户子交易生成的内部帐号查询帐号ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", g_pub_tx.ac_no );
	if( ret == 100 )
    {
		sprintf( acErrMsg, "无此内部帐号![%s]", g_pub_tx.ac_no );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "A029" );
		goto ErrExit;
	}else if( ret )
	{
       	sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* 修改再贷款登记薄，标志为正常 */
	strcpy( sMoAgainLn.sts, "1" );				/* 标志：1正常 */
	sMoAgainLn.ac_id = sMdmAcRel.ac_id;			/* 账户ID */

	ret = Mo_again_ln_Upd_Upd( sMoAgainLn, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改再贷款登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_again_ln_Clo_Upd();

	set_zd_data( "0300", g_pub_tx.ac_no );
	/* 记流水日志 */
	/**----- xxx ---- @20030821@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoAgainLn.pact_no);
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
