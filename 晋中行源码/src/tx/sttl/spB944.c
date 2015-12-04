/*************************************************
* 文 件 名:  spB944.c
* 功能描述： 完成回购式再贴现记账功能，挂贴现网点提入（408）；
*	     开内部帐户233；内部帐贷方记帐子交易，修改登记簿状态。
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
#include "com_parm_c.h"

int spB944()
{
	struct	mo_discnt_c		sMoDiscnt;	/* 贴现登记薄 */
	struct 	com_parm_c 		sComParm;	/* 公共参数 */
	struct  mdm_ac_rel_c    sMdmAcRel;	/* 介质与账户关系表 */
	struct  in_mst_c		sInMst;		/* 内部台账主文件 */
/*	char	cRinAcNo[14];				 再贴现账号 */
	char	cRinAcNo[21];				/* 再贴现账号 */
/*	char	cAcNo[14];					 人行准备金账号 */
	char	cAcNo[25];					/* 人行准备金账号 */
	int		ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sInMst, 0x00, sizeof( struct in_mst_c ) );
	memset( cRinAcNo, 0x00, sizeof( cRinAcNo ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );

	/* 数据初始化 */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*空介质介质代码*/
	
	/* 从前台屏幕取值 */
	get_zd_data( "0620", sMoDiscnt.rpact_no );	/* 再贴现协议编号 */
	get_zd_data( "0370", cRinAcNo );			/* 再贴现账号 */
	get_zd_data( "0300", cAcNo );				/* 人行准备金账号 */

	/* 检查再贴现协议编号是否存在，状态是否为录入，贴现类型是否为回购再贴现 */
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

	if( sMoDiscnt.pn_type[0] != '7' )
	{
		sprintf( acErrMsg, "此协议非回购式再贴现类型!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B170" );
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

	/* 检查人行准备金账号有效性 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", cAcNo );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "所输人行准备金账号有误!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B291" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel有误![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 根据账号ID、帐户序号查询内部台账主文件 */
	ret =In_mst_Sel( g_pub_tx.reply, &sInMst, "ac_id=%ld and ac_seqn=%d",\
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "所输人行准备金账号非内部账号![%s]", cAcNo );
   		WRITEMSG
   		strcpy( g_pub_tx.reply, "B292" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行In_mst_Sel错![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}

	if( strcmp( sInMst.prdt_no, "902" ) )
	{
		sprintf( acErrMsg, "所输账号非存放中央银行准备金存款产品!prdt_no[%s]", sInMst.prdt_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B293" );
		goto ErrExit;
	}

	/* 修改贴现登记薄，状态为正常 */
	strcpy( sMoDiscnt.rin_ac_no, cRinAcNo );	/* 再贴现帐号 */
	strcpy( sMoDiscnt.sts, "3" );				/* 标志：3正常 */
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

	/* 记流水日志 */
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
