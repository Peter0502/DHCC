/*************************************************
* 文 件 名:    sp9940.c
* 功能描述：   
*              根据借据号取贷款审批信息
*
* 作    者:    lance
* 完成日期：   2003年06月28日
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
	
sp9940()  
{ 		
	int ret=0;
	char sts[2];
	char pact_no[21];
	char cBr_no[6];
	char cMpact_no[21];
	struct ln_auth_c	ln_auth;
	memset( &ln_auth , 0x00 , sizeof( struct ln_auth_c ) );	
	memset(cBr_no,0x00,sizeof(cBr_no));
	memset(cMpact_no,0x00,sizeof(cMpact_no));
	/** 取值、赋值 **/	
	
	get_zd_data( "0660" , sts );	/* 操作标志 0－录入 1－修改 2－删除 */
	get_zd_data( "0630" , pact_no );	/* 借据号 */
  get_zd_data("0030",cBr_no);
	if( sts[0]=='0' )
	{
		goto OkExit;
	}
	
	/* 取审核信息 */
	ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth , "pact_no='%s'" , pact_no );
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"读取贷款主文件信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L164");
		ERR_DEAL;		
	}else if( ret==100 ){
		sprintf(acErrMsg,"此借据号不存在!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L176");
		ERR_DEAL;		
	}
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量");
	WRITEMSG

	set_zd_data("0250" , ln_auth.name);	/* 贷款户名 */
	set_zd_data("0380" , ln_auth.pay_ac_no);/* 还款账号 */
	set_zd_data("0320" , ln_auth.save_ac_no);/* 存款账户 */
	set_zd_double("0400" , ln_auth.amt);	/* 贷款金额 */
	set_zd_double("0840" , ln_auth.rate);	/* 合同利率 */	
	set_zd_long("0350" , ln_auth.time_lmt);	/* 贷款期限 */
		
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
   
