#include "find_debug.h" 
/*************************************************
* 文 件 名:	pubf_base_tel
* 功能描述
*                
* 作    者:	
* 完成日期：	2003年12月16日
* 修改记录：	
* 1. 日    期:
*    修 改 人:
*    修改内容:
*************************************************/

#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

/**********************************************************************
* 函数名：	pub_base_GetTelInfo()
* 函数功能：	根据柜员号码获得该操作员的记录
* 作者/时间：	Terry/2003.1.6
* 
* 参数：
*     输入：
*	    	char	*tel	操作员
*  
*     输出：
*		struct com_tel_c *com_tel  柜员档案
*		g_pub_tx.reply	相应代码
*     
* 返 回 值: 0 成功  -1 失败
*
* 修改历史：
*
********************************************************************/
int	pub_base_GetTelInfo( char *tel, struct com_tel_c *com_tel )
{
	int	ret;
	
	ret = Com_tel_Sel(g_pub_tx.reply, com_tel, "tel = '%s'", tel );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "此柜员不存在" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "S009" );
		return(-1);
	}else if ( ret )
	{
		sprintf( acErrMsg, "查询柜员表错!ret=[%d]", ret );
		WRITEMSG
		return(-1);
	}
	return(0);
}
