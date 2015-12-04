/*************************************************
* 文件名:    pubf_sttl_trustPay.c
* 功能描述： 银行委托收款托收承付处理函数族           
*
* 作   者:   jack
* 完成日期： 2004年2月2日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_trust_pay_c.h"

/**********************************************************************
 * 函  数 名：  pub_sttl_RegMoTrustPay
 * 函数 功能:   登记委托收款登记薄mo_trust_pay
 *             
 * 作者/时间：  jack/2004年2月2日
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

int pub_sttl_RegMoTrustPay( struct mo_trust_pay_c mo_trust_pay )
{
	int	ret;

	ret=Mo_trust_pay_Ins( mo_trust_pay, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "向委托收款登记薄插入记录错误!有重复记录!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B098" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Mo_trust_pay_Ins错误!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}
