#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_cur_no.c
* 功能描述：    币种检查文件
*              1、
*            
* 作    者:    Terry
* 完成日期：    2003年1月12日
*
* 修改记录：    
* 1. 日    期: 	
*    修 改 人: 	
*    修改内容: 	
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_cur_no_code_c.h"

/**********************************************************************
* 函数名：    	pub_base_ChkCurno
* 函数功能：    	根据币种检查该是否存在，是否启用
* 作者/时间：    Terry//2003年1月12日
* 
* 参数：
*     输入： 
*	    	char 	*cur_no  	币种号码
*           
*     输出： 
*		g_pub_tx.reply		响应码
* 返 回 值: 0 成功,-1 失败
*           
* 修改历史：
*
********************************************************************/

int pub_base_ChkCurno( char * cur_no )
{    
	struct com_cur_no_code_c sComCur;
	
	int	ret;
 	
 	MEMSET_DEBUG( &sComCur, 0x00, sizeof( struct com_cur_no_code_c ) );
 	
 	ret = Com_cur_no_code_Sel( g_pub_tx.reply, &sComCur, \
 					"cur_no = '%s'", cur_no );
 	if ( ret == 100 )
 	{
 		sprintf( acErrMsg, "不存在此币种cur_no=[%s]", cur_no );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "O002" );
 		return(-1);
 	}else if ( ret )
 	{
 		sprintf( acErrMsg, "查找币种表错误ret=[%d]", ret );
 		WRITEMSG
 		return(-1); 		
 	}
 	
 	if ( sComCur.use_ind[0] == 'N' )
 	{
 		sprintf( acErrMsg, "此币种处于停用状态[%s]", cur_no );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "O003" );
 		return(-1); 		
 	}
 	
	return 0; 
}
