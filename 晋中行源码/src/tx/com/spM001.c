/*************************************************
* 文 件 名:  spM001.c
* 功能描述： 现金收入子交易。
*
* 作    者:  andy
* 完成日期： 2004年1月12日
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
#include "cash_mst_c.h"

int spM001()
{
	int	ret;
	
	/* 系统初始化（初始化g_pub_tx)数据初始化 */
	init_pub_tx( );	

	if( !strlen(g_pub_tx.brf) )
	strcpy( g_pub_tx.brf,"收入现金");

	ret=pub_acct_cash();
	if( ret )
	{
		sprintf(acErrMsg,"调用现金收付主控失败!");
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
static int init_pub_tx( )
{
	pub_base_sysinit();

vtcp_log("TRACE [%d]",g_pub_tx.trace_no );
	
	get_zd_data( "1181" , g_pub_tx.cur_no );	/* 币种 */ 
	get_zd_data( "1182", g_pub_tx.note_type );	/* 凭证类型 */
	get_zd_data( "1183", g_pub_tx.beg_note_no );	/* 凭证号码 */
	get_zd_data( "1183", g_pub_tx.end_note_no );	/* 凭证号码 */
	get_zd_double( "1184", &g_pub_tx.tx_amt1 );	/* 发生额 */
	sprintf(acErrMsg,"Cur[%s] [%.2lf]", g_pub_tx.cur_no,g_pub_tx.tx_amt1 );
	WRITEMSG
	get_zd_data( "1185", g_pub_tx.cash_code );		/* 现金分析号 */
	get_zd_data( "1187", g_pub_tx.brf );		/* 摘要 */
	get_zd_data( "1188", g_pub_tx.name );		/* 对方账号 */
	pub_base_old_acno(g_pub_tx.name);
vtcp_log("对方帐号为:[%s]",g_pub_tx.name);
	strcpy( g_pub_tx.add_ind, "1" ); 	/**现金收入**/
	strcpy( g_pub_tx.ct_ind, "1" ); 	/**现转标志**/
	strcpy(g_pub_tx.hst_ind,"1");

	return	0;
}
