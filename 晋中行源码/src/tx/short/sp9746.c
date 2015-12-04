/*************************************************
* 文 件 名:    sp9746.c
* 功能描述：   
*              查询合同号是否存在
*
* 作    者:    lance
* 完成日期：   2003年05月03日
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
#include "ln_auth_c.h"
	
sp9746()  
{ 		
	int ret=0;
	struct ln_auth_c  ln_auth;
	
	memset( &ln_auth , 0x00 , sizeof(struct ln_auth_c) );
	
	/** 取值、赋值 **/	
	get_zd_data( "0630" , g_ln_mst.pact_no );	/* 证件类型 */
	
	/* 检查 合同号 */
	ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth , 
					" pact_no='%s' and sts!='0' " , g_ln_mst.pact_no );
	if( ret!=0 && ret!=100)
	{
		sprintf(acErrMsg,"读取贷款主文件信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L164");
		ERR_DEAL;
	}else if ( ret == 0 ){
		sprintf(acErrMsg,"合同号已存在![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L020");			
		ERR_DEAL;
	}
	
	sprintf(acErrMsg,"检查 合同号 PASS!");
	WRITEMSG
	

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
   
