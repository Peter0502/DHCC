#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_GetParm.c
* 功能描述：   
*          pub_base_GetParm_Str     读取字符型参数
*	       pub_base_GetParm_double  读取金额型参数
*	       pub_base_GetParm_long	读取长整型参数
*	       pub_base_GetParm_int		读取整型参数
*
* 作    者:    lance
* 完成日期：   2003年01月11日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "com_parm_c.h"

/**********************************************************************
* 函 数 名：  pub_base_GetParm_Str
* 函数功能：  读取字符型参数
*            
* 作    者：  lance
* 完成时间：  2003年01月18日
*
* 参    数： 
*     输  入：parm_code  char(5)  	参数代码    
*      	      seqn  	 smallint	序号 
*     输  出: val    	 char(20)	参数值
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_base_GetParm_Str( char *parm_code , int parm_seqn , char *val )
{	
	int ret=0;
	int flag=0;
	struct com_parm_c	com_parm;

	sprintf(acErrMsg,"进入读取字符型参数函数!");
	WRITEMSG	
	
	MEMSET_DEBUG (&com_parm, 0x00, sizeof(struct com_parm_c));

	/* 根据输入的参数代码和序号查询公共参数表 */	
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , 
					" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"公共参数无对应信息，请检查![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O004");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"读取公共参数表异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O005");
			goto ErrExit;
		}
	sprintf(acErrMsg,"根据输入的参数代码和序号查询公共参数表 PASS!");
	WRITEMSG
		
	/* 检验输出的参数类型 */		
	flag = atoi(com_parm.val_type);
	
	if ( flag==2 || flag==3 || flag==4 )
	{
		sprintf(acErrMsg, "取出的参数类型错误[%s]", com_parm.val_type );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"检验输出的参数类型 PASS!");
	WRITEMSG	
	
	pub_base_strpack( com_parm.val );

	strcpy( val , com_parm.val );
	sprintf(acErrMsg, "取出的参数值为[%s] ", val );
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}


/**********************************************************************
* 函 数 名：  pub_base_GetParm_double
* 函数功能：  读取金额型参数
*            
* 作    者：  lance
* 完成时间：  2003年01月18日
*
* 参    数： 
*     输  入：parm_code  char(5)  	参数代码    
*      	      seqn  	 smallint	序号 
*     输  出: val    	 double 	参数值
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_base_GetParm_double( char *parm_code , int parm_seqn , double *val )
{	
	int ret=0;
	int flag=0;
	struct com_parm_c	com_parm;

	MEMSET_DEBUG (&com_parm, 0x00, sizeof(struct com_parm_c));
	
	sprintf(acErrMsg,"进入读取金额型参数函数!");
	WRITEMSG	

	/* 根据输入的参数代码和序号查询公共参数表 */	
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , 
					" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"公共参数无对应信息，请检查![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O004");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"读取公共参数表异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O005");
			goto ErrExit;
		}
	sprintf(acErrMsg,"根据输入的参数代码和序号查询公共参数表 PASS!");
	WRITEMSG
		
	/* 检验输出的参数类型 */		
	flag = atoi(com_parm.val_type);
	
	if ( flag!=4 )
	{
		sprintf(acErrMsg, "取出的参数类型错误[%s]", com_parm.val_type );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"检验输出的参数类型 PASS!");
	WRITEMSG	
	
	*val = atof(com_parm.val) ;
	sprintf(acErrMsg, "取出的参数值为[%lf] ", *val );
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}


/**********************************************************************
* 函 数 名：  pub_base_GetParm_long
* 函数功能：  读取长整型参数
*            
* 作    者：  lance
* 完成时间：  2003年01月18日
*
* 参    数： 
*     输  入：parm_code  char(5)  	参数代码    
*      	      seqn  	 smallint	序号 
*     输  出: val    	 long		参数值
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_base_GetParm_long( char *parm_code , int parm_seqn , long *val )
{	
	int ret=0;
	int flag=0;
	
	struct com_parm_c	com_parm;
	MEMSET_DEBUG (&com_parm, 0x00, sizeof(struct com_parm_c));

	sprintf(acErrMsg,"进入读取长整型参数函数!");
	WRITEMSG	

	/* 根据输入的参数代码和序号查询公共参数表 */	
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , 
					" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"公共参数无对应信息，请检查![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O004");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"读取公共参数表异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O005");
			goto ErrExit;
		}
	sprintf(acErrMsg,"根据输入的参数代码和序号查询公共参数表 PASS!");
	WRITEMSG
		
	/* 检验输出的参数类型 */		
	flag = atoi(com_parm.val_type);
	
	if ( flag!=2 )
	{
		sprintf(acErrMsg, "取出的参数类型错误[%s]", com_parm.val_type );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"检验输出的参数类型 PASS!");
	WRITEMSG	
	
	*val = atol(com_parm.val) ;
	sprintf(acErrMsg, "取出的参数值为[%ld] ", *val );
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}


/**********************************************************************
* 函 数 名：  pub_base_GetParm_int
* 函数功能：  读取整型参数
*            
* 作    者：  lance
* 完成时间：  2003年01月18日
*
* 参    数： 
*     输  入：parm_code  char(5)  	参数代码    
*      	      seqn  	 smallint	序号 
*     输  出: val    	 int		参数值
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_base_GetParm_int( char *parm_code , int parm_seqn , int *val )
{	
	
	int ret=0;
	int flag=0;
	
	struct com_parm_c	com_parm;
	MEMSET_DEBUG (&com_parm, 0x00, sizeof(struct com_parm_c));

	sprintf(acErrMsg,"进入读取整型参数函数!");
	WRITEMSG	
	/* 根据输入的参数代码和序号查询公共参数表 */	
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , 
					" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
	if( ret==100 )
	{
		vtcp_log(" parm_code='%s' and parm_seqn=%d" , parm_code ,parm_seqn );
		sprintf(acErrMsg,"公共参数无对应信息，请检查![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O004");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"读取公共参数表异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O005");
			goto ErrExit;
		}
	sprintf(acErrMsg,"根据输入的参数代码和序号查询公共参数表 PASS!");
	WRITEMSG
		
	/* 检验输出的参数类型 */		
	flag = atoi(com_parm.val_type);
	
	if ( flag!=3 )
	{
		sprintf(acErrMsg, "取出的参数类型错误[%s]", com_parm.val_type );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"检验输出的参数类型 PASS!");
	WRITEMSG	
	
	*val = atoi(com_parm.val) ;
	sprintf(acErrMsg, "取出的参数值为[%ld] [%s]", *val,com_parm.val );
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}
