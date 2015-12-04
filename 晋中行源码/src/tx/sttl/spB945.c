/*************************************************
* 文 件 名:  spB945.c
* 功能描述： 完成再贴现到期回购（包括买断式再贴现和回购式再贴现）功能，
*	     包括修改贴现登记簿、贴现网点提出、内部帐借方子交易（233取款）。
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

int spB945()
{
	struct	mo_discnt_c		sMoDiscnt;		/* 贴现登记薄 */
	struct  mdm_ac_rel_c    sMdmAcRel;  	/* 介质与账户关系表 */
	struct  in_mst_c        sInMst;			/* 内部台账主文件 */
	char    cAcNo[14];						/* 人行准备金账号 */
	char    cAccHrt[6];                     /* 同城科目控制字 */	
	long	lPnDays;						/* 贴现天数 */
	int		ret;
	double  T_amt=0.00;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sInMst, 0x00, sizeof( struct in_mst_c ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );

	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0620", sMoDiscnt.rpact_no );	/* 再贴现协议编号 */
	get_zd_data( "0300", cAcNo );               /* 人行准备金账号 */

	/* 检查贴现登记簿中再贴现协议编号、状态为正常的记录是否存在 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "rpact_no='%s'", sMoDiscnt.rpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_pay_discnt_Dec_Upd错[%d]", ret );
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

sprintf( acErrMsg, "rtpn_date=[%ld],return_date=[%ld]", sMoDiscnt.rtpn_date,sMoDiscnt.return_date);
WRITEMSG

	get_zd_double( "0840", &sMoDiscnt.rtpn_rate );		/* 再贴现率 */

	/*如果贴现类型为回购式再贴现则计算再贴现利息=汇总票据金额*（回购当天交易日期-再贴日期*再贴现率/30/1000）*/
	if( sMoDiscnt.pn_type[0] == '7' )
	{
		ret = pub_base_CalDays( sMoDiscnt.rtpn_date, g_pub_tx.tx_date, "0", &lPnDays );
		if( ret )
		{
			sprintf( acErrMsg, "计算贴现天数错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B127" );
			goto ErrExit;
		}
sprintf( acErrMsg, "lPnDays=[%ld],par_amt=[%lf],rtpn_rate=[%lf]", lPnDays,sMoDiscnt.par_amt,sMoDiscnt.rtpn_rate);
WRITEMSG
		sMoDiscnt.rpn_int = sMoDiscnt.par_amt * lPnDays * (sMoDiscnt.rtpn_rate/30/1000);
sprintf( acErrMsg, "rpn_int=[%lf]", sMoDiscnt.rpn_int);
WRITEMSG
		set_zd_double( "1001", sMoDiscnt.rpn_int );		/* 再贴现利息 */
	}

	/* 修改贴现登记薄，金额＝金额-还款金额=0，状态为销户 */
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

	set_zd_data( "0370", sMoDiscnt.rin_ac_no );		/* 再贴现帐号 */
	/* 取同城科目产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) *
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
sprintf( acErrMsg, "xxxxxx    cAccHrt[%s]", cAccHrt );
WRITEMSG*/

	/* 记流水日志 */
	/**---- xxx -- @20030818@ ---- 将协议编号付给帐号撤销时用 ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.rpact_no);
	strcpy(g_pub_tx.no_show,"1");
	g_pub_tx.tx_amt1=T_amt;
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
