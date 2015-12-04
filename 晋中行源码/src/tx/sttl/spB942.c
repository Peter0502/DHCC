/*************************************************
* 文 件 名:  spB942.c
* 功能描述： 完成买断式再贴现记账功能，分别挂提人挂账子交易（rob）、
*	     异地大额子交易（暂无，以NULL代替）；开内部帐户233；
*	    内部帐贷方记帐、内部帐借方记帐、贷款还款子交易，修改登记簿状态。
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
#include "mo_city_c.h"
#include "com_tx_br_rel_c.h"
#include "com_parm_c.h"

int spB942()
{
	struct	mo_discnt_c		sMoDiscnt;	/* 贴现登记薄 */
	struct 	mo_city_c 		sMoCity;	/* 同城登记簿 */
	struct 	com_tx_br_rel_c 	sComTxBrRel;	/* 交易机构关系表 */
	struct 	com_parm_c 		sComParm;	/* 公共参数 */
/*37域长20位	char	cRinAcNo[11];				 再贴现账号 */
	char	cRinAcNo[21];				/* 再贴现账号 */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoCity, 0x00, sizeof( struct mo_city_c ) );
	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cRinAcNo, 0x00, sizeof( cRinAcNo ) );

	/* 数据初始化 */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*空介质介质代码*/
	
	/* 从前台屏幕取值 */
	get_zd_data( "0620", sMoDiscnt.rpact_no );	/* 再贴现协议编号 */
vtcp_log( "62 sMoDiscnt.rpact_no[%s] ", sMoDiscnt.rpact_no );
	get_zd_data( "0370", cRinAcNo );		/* 再贴现帐号 */
vtcp_log( "37 cRinAcNo[%s] ", cRinAcNo );
vtcp_log( "37hou62 sMoDiscnt.rpact_no[%s] ", sMoDiscnt.rpact_no );

	/* 检查再贴现协议编号是否存在，状态是否为录入，贴现类型是否为买断再贴现 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "rpact_no='%s'", sMoDiscnt.rpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此再贴现协议编号不存在!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B138" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	if( sMoDiscnt.pn_type[0] != '6' )
	{
		sprintf( acErrMsg, "此协议非买断式再贴现类型!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B169" );
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

	/* 为同城提入记账做准备 借用里面的其他参数,但记帐记到110科目上,由前台传上来*/
	ret=Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='TCKM'\
			and parm_seqn=2" );
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
/*
        set_zd_data( "1201", sComParm.val );
*/
        set_zd_double( "1208", ( sMoDiscnt.par_amt - sMoDiscnt.tpn_int ) );
        set_zd_data( "1205", "2" );
        set_zd_data( "120A", "贴现网点提入" );

	/* 修改贴现登记薄，状态为正常 */
	strcpy( sMoDiscnt.rin_ac_no, cRinAcNo );	/* 再贴现帐号 */
	strcpy( sMoDiscnt.sts, "*" );			/* 标志：改为已销 因为转给人行了已经 */
    sMoDiscnt.wrk_date = g_pub_tx.tx_date;          /* 流水日期 */
    sMoDiscnt.trace_no = g_pub_tx.trace_no;         /* 主机流水号 */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

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
