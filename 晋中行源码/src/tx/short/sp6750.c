/***************************************************************
* 文 件 名: sp6750.c
* 功能描述：根据还款账号返回贷款审批表中的利率
* 作    者: wzw
* 完成日期：2013年1月4日

insert into TX_DEF       values ('6750', '返回贷款审批表中的利率', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000', '1', '6', '2');
insert into TX_FLOW_DEF  values ('6750', 0, '6750', '#$');
insert into TX_SUB_DEF   values ('6750', '返回贷款审批表中的利率', 'sp6750', '0', '0');

**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ln_auth_c.h"

sp6750()
{   
	int ret; 
	char cAc_no[20];

	struct ln_auth_c	g_ln_auth;
	memset(&g_ln_auth,0x00,sizeof(struct ln_auth_c));
	memset(cAc_no,0x00,sizeof(cAc_no));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0300", cAc_no);          	
	get_zd_long( "0050", &g_pub_tx.tx_date);

	ret = Ln_auth_Sel(g_pub_tx.reply,&g_ln_auth, "pay_ac_no='%s' and type='12' and sts='X' ",  cAc_no);
	if( ret==100 )
  {
		sprintf(acErrMsg,"贷款审批表中不存在该记录");
		WRITEMSG
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	set_zd_double( "0840", g_ln_auth.rate);
	vtcp_log("[%s][%d]  g_ln_auth.rate = %.2f",__FILE__,__LINE__,g_ln_auth.rate);
				

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"返回利率成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"返回利率失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
