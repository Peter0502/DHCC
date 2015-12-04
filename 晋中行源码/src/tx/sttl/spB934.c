/*************************************************
* 文 件 名:  spB934.c
* 功能描述： 完成回购式转贴现记账（转出）功能，分别挂
*	     贴现网点提入（408）、异地大额子交易（暂无，
*	     以NULL代替）；开内部帐户233；内部帐贷方记
*	     帐子交易，修改登记簿状态。
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
#include "mo_city_c.h"
#include "com_tx_br_rel_c.h"
#include "com_parm_c.h"

int spB934()
{
	struct	mo_discnt_c		sMoDiscnt;	/* 贴现登记薄 */
	struct 	mo_city_c 		sMoCity;	/* 同城登记簿 */
	struct 	com_tx_br_rel_c 	sComTxBrRel;	/* 交易机构关系表 */
	struct 	com_parm_c 		sComParm;	/* 公共参数 */
	char	cTinAcNo[14];				/* 转贴现帐号 */
	double	dAmt1, dAmt2, dAmt3;
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoCity, 0x00, sizeof( struct mo_city_c ) );
	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cTinAcNo, 0x00, sizeof( cTinAcNo ) );

	/* 数据初始化 */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*空介质介质代码*/
	
	/* 从前台屏幕取值 */
	get_zd_data( "0630", sMoDiscnt.tpact_no );		/* 转贴现协议编号 */
	get_zd_data( "0370", cTinAcNo );				/* 转贴现帐号 */
	get_zd_data( "0670", sMoDiscnt.tpay_ac_type );  /* 付款账户类型 */

	/* 检查转贴现协议编号是否存在，状态是否为录入，贴现类型是否为回购转出 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "tpact_no='%s'", sMoDiscnt.tpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret );
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
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* 流水日期 */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* 主机流水号 */

	if( sMoDiscnt.pn_type[0] != '5' )
	{
		sprintf( acErrMsg, "此协议非回购式转贴现转出类型!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B166" );
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
		sprintf( acErrMsg, "此笔贴现已到期回购!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B160" );
		goto ErrExit;
	}

	/* 修改贴现登记薄，状态为正常 */
	strcpy( sMoDiscnt.tin_ac_no, cTinAcNo );	/* 转贴现帐号 */
	strcpy( sMoDiscnt.sts, "3" );				/* 标志：3正常 */

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
