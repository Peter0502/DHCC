/*************************************************
* 文件名:    pubf_sttl_draft.c
* 功能描述： 银行汇票机构各种检查处理函数族          
*
* 作   者:   jack
* 完成日期： 2004年1月18日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "auth_oper_brno_c.h"

/**********************************************************************
 * 函  数 名：  pub_sttl_ChkInputBrno
 * 函数 功能:   检查汇票录入机构是否具有权限
 *             
 * 作者/时间：  jack/2004年1月17日
 * 
 * 参  数：
 *     输入： 
 *		char *tx_brno		需要检查的机构编号
 *     输出： 
 *		char	g_pub_tx.reply	响应码
 *
 *   返回值：  0: 成功  -1: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_sttl_ChkInputBrno( char *oper_code, char *tx_brno )
{
	int	ret;

	ret = sql_count( "auth_oper_brno", "oper_code='%s' and dw_brno='%s'",\
			 oper_code, tx_brno );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查询auth_oper_brno错,ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B015" );
		return(-1);	
	}else if ( ret == 0 )
	{
		sprintf( acErrMsg, "该机构无权作此交易" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B014" );
		return(-1);	
	}

	return 0;
}

/**********************************************************************
 * 函  数 名：  pub_sttl_ChkSignBrno
 * 函数 功能:   检查汇票签发机构是否具有签发权限
 *             
 * 作者/时间：  jack/2004年1月17日
 * 
 * 参  数：
 *     输入： 
 *		char *in_brno		录入机构编号
 *		char *sign_brno		签发机构编号
 *     输出： 
 *		char	g_pub_tx.reply	响应码
 *
 *   返回值：  0: 成功  -1: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_sttl_ChkSignBrno( char *oper_code, char *in_brno, char *sign_brno )
{
	int	ret;

	ret = sql_count( "auth_oper_brno", "oper_code='%s' and dw_brno='%s' \
			and up_brno = '%s'", oper_code, in_brno, sign_brno );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查询auth_oper_brno错,ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B015" );
		return(-1);	
	}else if ( ret == 0 )
	{
		sprintf( acErrMsg, "该机构无权作此交易" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B014" );
		return(-1);	
	}

	return 0;
}
