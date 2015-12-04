/*************************************************
* 文件名:    pubf_sttl_draft.c
* 功能描述： 银行汇票处理函数族           
*
* 作   者:   jack
* 完成日期： 2004年1月17日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"
#include "mo_po_qry_c.h"

/**********************************************************************
 * 函  数 名：  pub_sttl_RegMoBankPo
 * 函数 功能:   登记银行汇票登记薄mo_bank_po
 *             
 * 作者/时间：  jack/2004年1月17日
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

int pub_sttl_RegMoBankPo( struct mo_bank_po_c mo_bank_po )
{
	int	ret;

	ret=Mo_bank_po_Ins( mo_bank_po, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "向银行汇票登记薄插入记录错误!有重复记录!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B002" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Mo_bank_po_Ins错误!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}

/**********************************************************************
 * 函  数 名：  pub_sttl_RegMoPoQry
 * 函数 功能:   登记查询查复登记薄mo_po_qry
 *             
 * 作者/时间：  jack/2004年1月19日
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

int pub_sttl_RegMoPoQry( struct mo_po_qry_c mo_po_qry )
{
	int	ret;

	ret=Mo_po_qry_Ins( mo_po_qry, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "向查询查复登记薄插入记录错误!有重复记录!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B020" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Mo_po_qry_Ins错误!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}
