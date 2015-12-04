/*************************************************
* 文 件 名:  spM005.c
* 功能描述： 现金调剂子交易	[收回]
*
* 作    者:  andy
* 完成日期： 2004年3月12日
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
#include "com_tel_connect_c.h"

int spM005()
{
	int ret;
	char teller[6];

	struct	com_tel_connect_c	g_com_tel_connect;
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));

	/*** 系统初始化（初始化g_pub_tx)数据初始化 ***/
	pub_base_sysinit();

	get_zd_data("0920",teller);					/* 对方柜员 */ 
	get_zd_data("0210",g_pub_tx.cur_no);		/* 币种 */ 

	strcpy(g_pub_tx.add_ind,"1");				/*现金收入*/
	strcpy( g_pub_tx.brf, "现金调剂" );

	if( !strcmp(g_pub_tx.tel,teller) )
	{
		sprintf(acErrMsg,"收回时不可输入自己的柜员号!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O045" );
		goto ErrExit;
	}

	/*** 收回之前检查是否有付出但未收入的记录 ***/
	ret=Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect,
		"tel='%s' and tw_tel='%s' and cur_no='%s' and ind='A'", \
		g_pub_tx.tel,teller,g_pub_tx.cur_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"没有向该柜员付出此现金记录!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O044" );
		goto ErrExit;
	}
	else if( ret==0 )
	{
		g_pub_tx.tx_amt1 = g_com_tel_connect.amt;
		strcpy( g_pub_tx.ct_ind, "1" );    /*现转1-现2-转*/

		ret = pub_acct_cash();
		if( ret )
		{
			sprintf(acErrMsg,"调用现金收付主控失败!");
			WRITEMSG
			goto ErrExit;
		}

		/*** 修改登记薄状态 ***/
		ret = sql_execute("update com_tel_connect set ind='1' where \
				tel='%s' and tw_tel='%s' and cur_no='%s' and ind='A'", \
				g_pub_tx.tel,teller,g_pub_tx.cur_no);
		if( ret )
		{
			sprintf(acErrMsg,"修改登记薄状态失败!");
			WRITEMSG
			goto ErrExit;
		}
	}
	else 
	{
		sprintf(acErrMsg,"查找出错!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	/**
	set_zd_data("1191",g_com_tel_connect.cur_no);
	set_zd_double("1194",g_com_tel_connect.amt);
	**/

	strcpy( g_pub_tx.cur_no, "01" );	/* 币种 */
	strcpy( g_pub_tx.ac_no, "10104" );	/* 在途人民币现金 */
	strcpy( g_pub_tx.ct_ind, "1" );	/* 现转 */
	strcpy( g_pub_tx.hst_ind, "1" );	/* 现转 */
	strcpy( g_pub_tx.brf, "现金调剂" );
	g_pub_tx.tx_amt1 = g_com_tel_connect.amt;
	strcpy(g_pub_tx.add_ind,"0");				/* 减标志 */
	
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("1191",g_com_tel_connect.cur_no);
	set_zd_double( "1184",g_pub_tx.tx_amt1 );

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

