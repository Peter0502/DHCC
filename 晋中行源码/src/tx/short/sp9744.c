/*************************************************
* 文 件 名:    sp9744.c
* 功能描述：   
*              根据授权码回显相关信息
*
* 作    者:    lance
* 完成日期：   2003年03月25日
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
#include "auth_code_c.h"

sp9744()  
{ 		
	int ret=0;
	char auth_no[51];


    struct auth_code_c          auth_code;
    
TRACE	
	memset (&auth_code, 0x00, sizeof(struct auth_code_c));	



TRACE	
	/** 数据初始化 **/
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
TRACE	
	/** 取值、赋值 **/	
	
	get_zd_data( "0250" ,auth_no );                            	/* 授权码 */


	/*根据授权码查询未复核交易信息*/
    ret=Auth_code_Sel(g_pub_tx.reply,&auth_code,
				  " auth_no=%s " ,auth_no);
   	
	if( ret==100 )
    {
         sprintf(acErrMsg,"该授权码不存在[%s]",g_pub_tx.reply);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"E022");
		 goto ErrExit;
    }
    else if( ret )
   {
    	 sprintf(acErrMsg,"取授权码基本信息异常![%s]",g_pub_tx.reply);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"E023");
		 goto ErrExit;
   }



	/* 输出变量 */
	set_zd_long( "0290", auth_code.trace_no );                 /* 流水号 */
	set_zd_data( "0640", auth_code.tx_code );                  /* 交易代码 */
	set_zd_data( "0670", auth_code.auth_ind );                 /* 授权标志 */
	set_zd_long( "0340", auth_code.cond_seqn );                /* 条件序号 */
	set_zd_data( "0680", auth_code.sts );                    	 /* 使用状态 */
	set_zd_data( "0580", auth_code.tel );	                     /* 记账员 */	
	set_zd_data( "0590", auth_code.auth );                     /* 授权人 */
	set_zd_data( "0600", auth_code.aud );                      /* 审批人 */	
	set_zd_data( "0610", auth_code.chk );                      /* 复核员 */	
				
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


