/*************************************************
* 文 件 名:  spB914.c
* 功能描述： 完成贴现、转贴现（买断转入）到期收回的记账功能，
*	     包括修改贴现登记簿、贷款还本、取款子交易
*	    （或：贴现网点提入、异地大额子交易）；
*
* 作    者:  jane
* 完成日期： 2003年4月9日
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

int spB914()
{
	struct	mo_discnt_c		sMoDiscnt;		/* 贴现登记薄 */
	struct	dd_mst_c		sDdMst;			/* 活期存款主文件 */
	struct 	com_parm_c 		sComParm;		/* 公共参数 */
	char	cPayAcType[2];					/* 还款帐户类型 */
	char	cLnAcNo[25];					/* 贷款账号 */
	int	ret;
	double	T_amt=0.00;
        
        char    tmp[20];

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( cPayAcType, 0x00, sizeof( cPayAcType ) );
	memset( cLnAcNo, 0x00, sizeof( cLnAcNo ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	/* 从前台屏幕取值 */
	get_zd_data( "0620", sMoDiscnt.pact_no );	/* 贴现协议编号 */
	get_zd_data( "0670", cPayAcType );			/* 还款帐户类型 */
	get_zd_data( "0330", cLnAcNo );				/* 贷款账号 */
	/*--- xxx --- @20030818@ ---- 不要修改流水日期和主机流水号 ***
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;		* 流水日期 *
	sMoDiscnt.trace_no = g_pub_tx.trace_no;		* 主机流水号 *
	*/
vtcp_log( "贴现账号[%s]", cLnAcNo );
	/* 根据贴现协议编号查找贴现登记簿 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and sts!='*'", sMoDiscnt.pact_no );
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

	/* 检查状态是否为正常，且贴现类型为贴现或者买断转入 *
	if( sMoDiscnt.pn_type[0] != '1' && sMoDiscnt.pn_type[0] != '2' )
	{
		sprintf( acErrMsg, "此贴现协议类型非贴现或买断转入，不能做此交易!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B165" );
		goto ErrExit;
	}**/
if(sMoDiscnt.po_knd[0]=='P')/**NEWDP**/
	{
        	if (sMoDiscnt.pn_type[0] != '1' && sMoDiscnt.pn_type[0] != '2') {
            	sprintf(acErrMsg, "此贴现协议类型非贴现或买断转入，不能做此交易!");
            	WRITEMSG
            	strcpy(g_pub_tx.reply, "B165");
            	goto ErrExit;
        	}
    	}
    	else
    	{
        	if (sMoDiscnt.pn_type[0] != '1' && sMoDiscnt.pn_type[0] != '2' && sMoDiscnt.pn_type[0] != '3') {
            sprintf(acErrMsg, "此贴现协议类型非贴现或买断转入，不能做此交易!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B165");
            goto ErrExit;
        	}
    	}/****end******/
	if( sMoDiscnt.pn_type[0] == '1' )	/* 贴现 */
	{
		if( sMoDiscnt.sts[0] == '1' )
		{
			sprintf( acErrMsg, "此笔贴现未审核!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B147" );
			goto ErrExit;
		}else if( sMoDiscnt.sts[0] == '2' )
		{
			sprintf( acErrMsg, "此笔贴现未记账!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}else			/* 买断转入 */
	{
		if( sMoDiscnt.sts[0] == '1' )
		{
			sprintf( acErrMsg, "此笔贴现未记账!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "此笔贴现已做还款!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	/* 修改贴现登记薄，金额＝金额-还款金额=0，状态为销户，还款帐户类型 */
	strcpy( sMoDiscnt.pay_ac_type, cPayAcType );	/* 还款帐户类型 */
	strcpy( sMoDiscnt.sts, "*" );
	T_amt=sMoDiscnt.tranx_bal;
	sMoDiscnt.tranx_bal = 0;
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;		/* 流水日期 */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;		/* 主机流水号 */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

vtcp_log( "janejanejane   [0450]=[%ld]", sMoDiscnt.not_matr_date );
	set_zd_long("0450",sMoDiscnt.not_matr_date);		/* 票据到期日期，为授权所加 */
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
	g_pub_tx.tx_amt1=0.00;
	if(cPayAcType[0] == '0' || cPayAcType[0] == '1' )		/* 本地帐户 */
	{
		sprintf(acErrMsg,"内部帐与科目");
		WRITEMSG
		goto	OkExit1;
	}else if( cPayAcType[0] == '2' )	/* 异地帐户 */
	{		
		sprintf(acErrMsg,"对公活期帐户");
		WRITEMSG
		goto	OkExit2;
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
OkExit1:
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
	return 0;
}
