/*************************************************
* 文 件 名:  spB925.c
* 功能描述： 完成回购式转贴现转入到期回购功能，
*	     包括修改贴现登记簿、贷款还本、
*	    贴现网点提入/异地大额子交易；
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
#include "mo_city_c.h"
#include "com_tx_br_rel_c.h"

int spB925()
{
	struct	mo_discnt_c		sMoDiscnt;		/* 贴现登记薄 */
	struct	mdm_ac_rel_c	sMdmAcRel;		/* 介质与账户关系 */
	struct 	com_parm_c 		sComParm;		/* 公共参数 */
	struct 	mo_city_c 		sMoCity;		/* 同城登记簿 */
	struct 	com_tx_br_rel_c sComTxBrRel;	/* 交易机构关系表 */
	char	cPayAcType[2];					/* 付款帐户类型 */
	char	cLnAcNo[25];					/* 贷款账号 */
	int		ret;
	double  T_amt=0.00;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &sMoCity, 0x00, sizeof( struct mo_city_c ) );
	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( cPayAcType, 0x00, sizeof( cPayAcType ) );
	memset( cLnAcNo, 0x00, sizeof( cLnAcNo ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0620", sMoDiscnt.pact_no );	/* 转贴现协议编号 */
	get_zd_data( "0690", cPayAcType );			/* 付款帐户类型 */

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
	***--------------------------------------------**/
	/* 根据转贴现协议编号和贴现类型修改贴现登记簿 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s' ",sMoDiscnt.pact_no);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_pay_discnt_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此记录!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B121" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	if( sMoDiscnt.pn_type[0] != '3' )
	{
		sprintf( acErrMsg, "此协议非回购式转贴现转入类型!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B161" );
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
	}

	/* 根据贴现登记簿中的贷款账号ID和贷款账号序号查询贷款账号 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_id=%ld and ac_seqn=%d", \
				sMoDiscnt.ac_id, sMoDiscnt.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "介质与账户关系表中不存在此帐户信息!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B117" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	/* 为贷款还款子交易准备 */
	strcpy( cLnAcNo, sMdmAcRel.ac_no );
	set_zd_data( "0330", cLnAcNo );				/* 贷款账号 */
	set_zd_long( "0350", sMoDiscnt.ac_seqn );		/* 贷款账号序号 */

	/* 修改贴现登记薄，金额＝金额-还款金额=0，状态为销户,登记付款帐户类型 */
	strcpy( sMoDiscnt.tpay_ac_type, cPayAcType );	/* 付款帐户类型 */
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

	/* 记流水日志 */
	/**---- xxx --- @20030818@ ---- 将协议编号付给帐号撤销时用 ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
	strcpy(g_pub_tx.no_show,"1");
	g_pub_tx.tx_amt1=T_amt;
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

	/*if( cPayAcType[0] == '1' )		 本地帐户 
	{
		sprintf(acErrMsg,"本地帐户");
		WRITEMSG
		goto	OkExit1;
	}
	else					 异地帐户
	{
		sprintf(acErrMsg,"异地帐户");
		WRITEMSG
		goto	OkExit2;
	}*//*20151202*/

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
/*OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;*//*20151202*/
}
