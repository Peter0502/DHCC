/*************************************************
* 文 件 名:  spB912.c
* 功能描述： 完成贴现审核功能。
*
* 作    者:  jane
* 完成日期： 2003年4月8日
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

int spB912()
{
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0620", sMoDiscnt.pact_no );	/* 贴现协议编号 */

	/* 根据贴现协议编号查询贴现登记簿 */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此贴现协议编号不存在!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B113" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	if( sMoDiscnt.pn_type[0] != '1' )
	{
		sprintf( acErrMsg, "此协议编号为转贴现或再贴现协议编号!不需要审核!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B151" );
		goto ErrExit;
	}

	if( sMoDiscnt.sts[0] == '2' )
	{
		sprintf( acErrMsg, "此贴现协议已审核!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B152" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '3' )
	{
		sprintf( acErrMsg, "此笔贴现已记帐!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B148" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "此笔贴现已还款!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	/* 根据贴现协议编号和贴现类型修改贴现登记簿 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and pn_type='%s'", \
				sMoDiscnt.pact_no, "1" );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此贴现协议编号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B113" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 修改贴现登记薄，状态为已审核*/
	strcpy( sMoDiscnt.sts, "2" );			/* 标志：2已审核 */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_discnt_Clo_Upd();

	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "贴现审核" );
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!!!");
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
