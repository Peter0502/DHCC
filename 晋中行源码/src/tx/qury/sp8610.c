/*************************************************
* 文 件 名:  sp8610.c
* 功能描述： 每日实时存款余额查询
*
* 作    者:  rob
* 完成日期： 20031220
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"

int sp8610()
{
	int ret = 0;

	double dAcbl1 = 0.00;	/*活期余额*/	
	double dAcbl2 = 0.00;	/*定期余额*/	
	double dAcbl = 0.00;	/*总余额*/	

	pub_base_sysinit();

	ret = sql_sum_double( "dd_mst", "bal", &dAcbl1, "1=1" );
	if ( ret )
	{
		sprintf( acErrMsg, "统计活期余额失败!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D101" );
		goto ErrExit;
	}

	ret = sql_sum_double( "td_mst", "bal", &dAcbl2, "1=1" );
	if ( ret )
	{
		sprintf( acErrMsg, "统计定期余额失败!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D101" );
		goto ErrExit;
	}

	dAcbl = dAcbl1 + dAcbl2;

	set_zd_double( "0400", dAcbl );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
