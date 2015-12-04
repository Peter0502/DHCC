/*************************************************
* 文 件 名:  spM204.c
* 功能描述： 提供柜长对柜员钱箱管理的功能。
*
* 作    者:  andy
* 完成日期： 2004年4月30日
*
* 修改记录： 
*   日   期: 2004年7月30日
*   修 改 人: andy
*   修改内容: 1.钱箱管理只能柜长才能做
*             2.只能在钱箱空的时候才能止用
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "com_tel_c.h"

int spM204()
{
	struct	cash_mst_c	sComMst;		/* 现金台账 */
	struct	com_tel_c	sComTel;		/* 柜员信息 */
	char	cOperCode[2];				/* 操作码 */
	int	ret;

	memset( &sComMst, 0x00, sizeof( struct cash_mst_c ) );
	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0670", cOperCode );		/* 操作码 */
	get_zd_data( "0920", sComMst.tel );		/* 柜员号码 */
	get_zd_data( "0210", sComMst.cur_no );		/* 币种 */

	/* 根据柜员号查询柜员信息 */
	ret = Com_tel_Sel( g_pub_tx.reply, &sComTel, "tel='%s'", g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "查询柜员信息出错!tel[%s]sqlcode=[%d]", g_pub_tx.tel, ret );
		WRITEMSG
		goto ErrExit;
	}
	if( sComTel.lvl[0] != '3' )
	{
		sprintf( acErrMsg, "钱箱管理只能由柜组长操作!tel[%s]", g_pub_tx.tel );
		WRITEMSG
       	strcpy( g_pub_tx.reply, "M024" );
		goto ErrExit;
	}

	/* 根据柜员号和币种查询现金台帐 */
	ret = Cash_mst_Dec_Upd( g_pub_tx.reply, "br_no='%s' and tel='%s' \
				and cur_no='%s'", g_pub_tx.tx_br_no,\
				sComMst.tel, sComMst.cur_no );
	if( ret )
	{
		sprintf( acErrMsg, "执行Cash_mst_Dec_Upd错!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Cash_mst_Fet_Upd( &sComMst, g_pub_tx.reply );
	if ( ret == 100 )
   	{
       	sprintf( acErrMsg, "现金台帐中不存在此柜员钱箱记录!" );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "M008" );
       	goto ErrExit;
   	}
	else if ( ret )
   	{
       	sprintf( acErrMsg, "执行Cash_mst_Fet_Upd错!ret=[%d]",ret );
       	WRITEMSG
       	goto ErrExit;
   	}
   	if( cOperCode[0]== sComMst.sts[0] ) /* 已经启用或者已经停用 禁止重复操作 */
   	{
   		if(cOperCode[0] == '0')
   		{
   		sprintf( acErrMsg, "该柜员已经是启用状态，请勿重复操作!" );
			WRITEMSG
			  strcpy( g_pub_tx.reply, "T050" );
   		goto ErrExit;
   	  }
   	  else if (cOperCode[0] == '1')
   	  {
   	  sprintf( acErrMsg, "该柜员已经是止用状态，请勿重复操作!" );
			WRITEMSG
			  strcpy( g_pub_tx.reply, "T051" );
   		goto ErrExit;
   	  }
   		else 
   		{
   			sprintf( acErrMsg, "无此状态!" );
			WRITEMSG
			  strcpy( g_pub_tx.reply, "M008" );
   		goto ErrExit;
   		}
   	}

   	if( cOperCode[0] == '0' )	/* 启用 */
   		strcpy( sComMst.sts, "0" );	/* 状态: 0：正常 */
   	else				/* 止用 */
	{
		/* 判断钱箱是否为空 */
		if( pub_base_CompDblVal(sComMst.bal, 0.00) == 1 )
		{
			sprintf( acErrMsg, "该柜员钱箱有余额,不能止用!" );
			WRITEMSG
   			strcpy( g_pub_tx.reply, "M025" );
			goto ErrExit;
		}

   		strcpy( sComMst.sts, "1" );	/* 状态: 1：不可用 */
	}

   	ret = Cash_mst_Upd_Upd( sComMst, g_pub_tx.reply );
   	if( ret )
	{
		sprintf( acErrMsg, "执行Cash_book_Upd_Upd错!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

    	Cash_mst_Clo_Upd( );

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
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
