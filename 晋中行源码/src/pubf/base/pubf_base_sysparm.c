#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_sysparm
* 功能描述：各种公共系统表的检查；
* 作    者:	Terry
* 完成日期：2003年1月15日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include "stdio.h"
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"

/**********************************************************************
 * 函 数 名：   pub_base_GetSysparm
 * 函数功能：   取公共参数表
 * 作者/时间：
 *
 * 参数：
 *     输入: 
 *     输出: 公共参数结构
 *     返回: -1失败 0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_GetSysparm( struct com_sys_parm_c *com_sys_parm )
{
	int	ret;

	ret = Com_sys_parm_Sel( g_pub_tx.reply, com_sys_parm , "1 = 1");
 	if ( ret == 100 )
 	{
 		sprintf(acErrMsg,"公共参数表中没有数据!");
 		WRITEMSG
 		strcpy ( g_pub_tx.reply , "G001" );
 		return -1;
 	}
 	else if ( ret )
 	{
 		sprintf(acErrMsg,"取公共参数表错!" );
 		WRITEMSG
 		return -1;
 	}

 	return (0);
}
