/*************************************************
* 文 件 名: spL001.c
* 功能描述：   
*              贷款还本－贷款审批表更新
*
* 作    者:    尹志磊
* 完成日期：   2015年03月30日

insert into tx_flow_def values('3201','5','L001','#$');
insert into tx_sub_def values('L001','更新贷款审批表','spL001','0','0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "ln_auth_c.h"

static	struct ln_auth_c	ln_auth;
spL001()  
{ 	
	int ret = 0;
	char jjh[20];
	memset (&ln_auth, 0x00, sizeof(struct ln_auth_c));
	memset(jjh,0x00,sizeof(jjh));
	/** 初始化全局变量 **/
	pub_base_sysinit();	
	/** 取值、赋值 **/
	get_zd_data("0240",g_pub_tx.prdt_code);	/* 产品号 */
	get_zd_data("0370",jjh);	/* 借据号 */
	sprintf(acErrMsg,"产品号为[%s]!!",g_pub_tx.prdt_code);
	WRITEMSG	
	sprintf(acErrMsg,"借据号为[%s]!!",jjh);
	WRITEMSG	
	if(!strcmp(g_pub_tx.prdt_code,"142") || !strcmp(g_pub_tx.prdt_code,"144"))
		{
			ret = Ln_auth_Dec_Upd(g_pub_tx.reply," pact_no='%s' " ,jjh);
				if ( ret )
				{
					sprintf( acErrMsg, "执行Ln_auth_Dec_Upd错!ret=[%d]",ret );
					WRITEMSG
					return(-1);
				}
				ret = Ln_auth_Fet_Upd( &ln_auth, g_pub_tx.reply );
				if( ret==100)
				{
					sprintf(acErrMsg,"贷款审批表无此记录",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D104");
					ERR_DEAL;
				}
				else if ( ret )
				{
					sprintf(acErrMsg,"取贷款审批表信息异常",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"L181");
					ERR_DEAL;
				}	
				strcpy( ln_auth.sts , "*" );	 /* 修改状态:失效 */
				ret = Ln_auth_Upd_Upd( ln_auth, g_pub_tx.reply);
				if ( ret )
				{
					sprintf( acErrMsg,"Ln_auth_Upd_Upd 出错[%d]",ret);
					WRITEMSG
					return 1;
				}		
	
				Ln_auth_Clo_Upd( );
				sprintf(acErrMsg,"更新贷款审批表 PASS");
				WRITEMSG
		}
	else
		{
			goto OkExit;
		}
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款还款成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款还款失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}