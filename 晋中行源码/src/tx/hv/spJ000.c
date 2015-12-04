/*************************************************
* 文 件 名:  spJ000.c
* 功能描述： 大额来账公用子交易
*			 记帐错误 处理出口
*			 
* 作    者:  janeg
* 完成日期： 2003年8月15日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#define  EXTERN
#include <string.h>
#include "public.h"
#include "mbfe_pub.h"
#include "find_debug.h"

int spJ000()
{
	char reply[5];
	struct mbfe_hang_c hang_v;

	memset( &hang_v, 0x00, sizeof( struct mbfe_hang_c ) );

	pub_base_sysinit();

	get_zd_data( "0120", g_pub_tx.reply );
	vtcp_log( "大额来帐记帐失败 返回码[%s]", g_pub_tx.reply );
	
	/*  非支付类 大额来帐 账号和账号ID都为空*/
	if(memcmp(g_pub_tx.reply,"P101",4) == 0) 
	{
		vtcp_log( "大额来帐记帐失败 返回码[%s]", g_pub_tx.reply );
		goto GoodExit;
	}
  
	hang_v.consigndate = g_pub_tx.tx_date;
	strcpy( hang_v.txssno, mbfetran.txssno );
	strcpy( hang_v.odficode, mbfetran.odficode );
	strcpy( hang_v.hang_reply, g_pub_tx.reply );

	sprintf(acErrMsg, "[%ld], [%s], [%s],[%s]",hang_v.consigndate, 
		hang_v.txssno, hang_v.hang_reply, hang_v.odficode);
	WRITEMSG


	sql_rollback();    /*回滚之前的事务*/

	sql_begin();    /*开始登记登记挂账业务临时表的事务*/
	g_reply_int = Mbfe_hang_Ins( hang_v, reply );
	if( g_reply_int )
	{
		strcpy( g_pub_tx.reply, reply );
       		sprintf( acErrMsg, "登记登记挂帐业务临时表错误![%d]", g_reply_int );
       		WRITEMSG
		sql_rollback();    /*回滚登记登记挂账业务临时表的事务*/
	}
	else if( g_reply_int == 0 )
		sql_commit();    /*提交登记登记挂账业务临时表的事务*/

	sql_begin();    /*开始准备返回失败的事务*/

ErrExit:    /*一定是错误出口*/
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
	
GoodExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
	
}

