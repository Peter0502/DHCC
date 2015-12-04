/*************************************************
* 文 件 名:    sp9675.c
* 功能描述：   网点完工回显状态
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
	
sp9675()  
{ 		
	int ret=0;
	struct com_branch_c com_branch;
	
	/** 数据初始化 **/
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
		ret=Com_branch_Sel( g_pub_tx.reply,&com_branch,"br_no='%s'",
			g_pub_tx.tx_br_no );
		if( ret )
		{
			sprintf(acErrMsg,"br_no='%s'",g_pub_tx.tx_br_no );
			WRITEMSG
			strcpy( g_pub_tx.reply,"H063" );
	    	goto OkExit;
		}
		com_branch.wrk_sts[1]='\0';
		set_zd_data( "0680", com_branch.wrk_sts );		/* 还款账户户名 */	
	
			
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
   
