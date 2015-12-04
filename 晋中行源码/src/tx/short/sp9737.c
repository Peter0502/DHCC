/*************************************************
* 文 件 名:    sp9737.c
* 功能描述：   
*              贷款产品代码返回产品信息
*
* 作    者:    lance
* 完成日期：   2003年06月06日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9737()  
{ 		
	int ret=0;
	char prod_no[4];
	double rate_val , over_rate_val;
	
	
	/** 数据初始化 **/
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0240" , prod_no );	/* 产品代码 */
	
	/* 根据产品代码 返回产品名称 */

	ret = pub_base_get_prdtparm( prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"调用根据产品代码取产品参数函数错误");
		WRITEMSG
		goto ErrExit;
	}		  
	
	/* 输出变量 */
	set_zd_data( "0710", g_ln_parm.time_type );	/* 产品名称 */

			
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
