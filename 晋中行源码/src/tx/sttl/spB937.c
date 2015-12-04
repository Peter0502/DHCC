/*************************************************
* 文 件 名:  spB937.c
* 功能描述： 完成回购式转贴现转出到期回购功能，包括修改
*	     贴现登记簿、贴现网点提出/异地大额子交易、
*	     内部帐借方子交易（233取款）；
*
* 作    者:  jane
* 完成日期： 2003年4月15日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容: 增加录入交易
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int spB937()
{
	struct	mo_discnt_c		sMoDiscnt;		/* 贴现登记薄 */
	char    cAccHrt[6];                     /* 同城科目控制字 */
	int		ret;
	double  T_amt=0.00;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );

	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0630", sMoDiscnt.tpact_no );		/* 转贴现协议编号 */

	/* 检查贴现登记簿中转贴现协议编号、贴现类型为回购转出、状态为正常的记录是否存在 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "tpact_no='%s'", sMoDiscnt.tpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_pay_discnt_Dec_Upd错[%d]", ret );
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

	if( sMoDiscnt.pn_type[0] != '5' )
	{
		sprintf( acErrMsg, "此协议非回购式转贴现转出类型!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B166" );
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
	}else if( sMoDiscnt.sts[0] == '4' )
	{
		sprintf( acErrMsg, "此笔贴现已录入!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B326" );
		goto ErrExit;
	}

	/* 修改贴现登记薄，状态为转贴现转出到期回购录入 */
	strcpy( sMoDiscnt.sts, "4" );

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

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
