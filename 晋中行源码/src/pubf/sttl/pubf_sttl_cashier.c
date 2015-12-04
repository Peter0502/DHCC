/*************************************************
* 文件名:    pubf_sttl_cashier.c
* 功能描述： 银行本票处理函数族           
*
* 作   者:   jack
* 完成日期： 2004年1月21日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_cashier_c.h"

/**********************************************************************
 * 函  数 名：  pub_sttl_RegMoCashier
 * 函数 功能:   登记银行本票登记薄mo_cashier
 *             
 * 作者/时间：  jack/2004年1月21日
 * 
 * 参  数：
 *     输入： 
 *
 *     输出： char	g_pub_tx.reply	响应码
 *
 *   返回值：  0: 成功  -1: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/

int pub_sttl_RegMoCashier( struct mo_cashier_c mo_cashier )
{
	int	ret;

	ret=Mo_cashier_Ins( mo_cashier, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "向银行本票登记薄插入记录错误!有重复记录!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B040" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Mo_cashier_Ins错误!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}
