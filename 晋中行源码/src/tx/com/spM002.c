/*************************************************
* 文 件 名:  spM002.c
* 功能描述： 现金付出子交易。
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

int spM002()
{
	int	ret;
	
	/* 系统初始化（初始化g_pub_tx)数据初始化 */
	init_pub_tx( );	

	strcpy( g_pub_tx.brf,"付出现金");
	
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
	
	get_zd_data( "1191" , g_pub_tx.cur_no );	/* 币种 */ 
	get_zd_data( "1192", g_pub_tx.note_type );	/* 凭证类型 */
	get_zd_data( "1193", g_pub_tx.beg_note_no );	/* 凭证号码 */
	get_zd_double( "1194", &g_pub_tx.tx_amt1 );	/* 发生额 */
	sprintf(acErrMsg,"Cur[%s] [%.2lf]", g_pub_tx.cur_no,g_pub_tx.tx_amt1 );
	WRITEMSG
	get_zd_data( "1195", g_pub_tx.cash_code );		/* 现金分析号 */
	get_zd_data( "1197", g_pub_tx.brf );		/* 摘要 */
	get_zd_data( "1198", g_pub_tx.name );		/* 对方账号 */
  pub_base_old_acno(g_pub_tx.name);
	strcpy( g_pub_tx.add_ind, "0" ); 	/**现金支出**/
	
	return	0;
}
