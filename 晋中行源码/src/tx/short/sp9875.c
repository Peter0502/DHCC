/*************************************************
* 文 件 名:    sp9875.c
* 功能描述：   
*              根据贷款显示账号和账号序号取贷款主文件,并判断还款计划标志
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
	
sp9875()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	long beg_end_date;  /* 到期日- 开户日 */
	long beg_end_days;  /* 到期日- 开户日 */
	long max_exp_date;  /* 最大展期日期 */
	
	/** 数据初始化 **/
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0370" , ac_no );	/* 贷款帐号 */
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
		
	get_zd_int( "0340" , &ac_seqn );	/* 贷款账户序号s */
	
	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(ac_no,ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%d] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG
	/* 判断放款计划类型 N无 M手动 */
	if ( g_ln_parm.pay_pln_crt[0]!='M' )
	{
		sprintf(acErrMsg,"还款计划类型非手动[%s],不可做此交易!",g_ln_parm.pay_pln_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L080");
		goto ErrExit;
	}	
		
		
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量[%s]",g_ln_mst.name);
	WRITEMSG
	set_zd_double( "0400", g_ln_mst.amt_lmt );	/* 计划放款额 */	
	set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* 累计发放额 */
	set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
	set_zd_double( "0420", g_ln_mst.bal );		/* 余额 */				
	set_zd_data("0370",ac_no);	/* 处理旧帐号 */
				
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
   
