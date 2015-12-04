/*************************************************
* 文 件 名: spL10C.c
* 功能描述：   
*              还款计划(自动生成部分)
*
* 作    者:    lance
* 完成日期：   2003年03月29日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"

spL10C()  
{ 		
	int ret=0;
	long pay_date,term_opn_date;
	int i=0;
	int ln_cnt=0,month_cnt=0;
	int a=1;
	
	
        struct ln_pay_pln_c	ln_pay_pln;	
	memset (&ln_pay_pln,0x00,sizeof(struct ln_pay_pln_c) );	
		
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
	
	/** 取值、赋值 **/
	if ( data_set_struct() )    ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG	
	
	sprintf(acErrMsg,"读取贷款户信息[%s][%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG		
	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%d] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG
	
	/* 判断还款计划类型 N无 M手动 */
	if ( g_ln_parm.pay_pln_crt[0]!='A' )
	{
		TRACE
		goto OkExit;
	}
  
	/* 计算总期数 */
	month_cnt = pub_base_CalMouths( g_ln_mst.opn_date , g_ln_mst.mtr_date ); 
	if( ret ) goto ErrExit;
		
	if( g_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%g_ln_parm.pay_amt_term==0 )
		{
			ln_cnt = month_cnt/g_ln_parm.pay_amt_term ;
		}else{
			ln_cnt = month_cnt/g_ln_parm.pay_amt_term +1;
		}	
	}
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	TRACE
	/* 生成还款计划 */
	ret = pub_ln_PayPlan( g_ln_mst , g_ln_parm , a , &ln_pay_pln ,0 );	
	if (ret)
	{
		sprintf(acErrMsg,"调用函数pub_ln_PayPlan错误!");
		WRITEMSG
		ERR_DEAL	
	}
	TRACE											
	/* 添加还款计划表 */	
	ret = Ln_pay_pln_Ins( ln_pay_pln , g_pub_tx.reply );
	if(ret)
	{
		sprintf(acErrMsg,"登记还款计划表失败");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L077");
		goto ErrExit;
	}
	TRACE
	/* Pub_tx结构赋值 */
	strcpy ( g_pub_tx.ac_wrk_ind,"0" );
	g_pub_tx.tx_amt1 = 0.00 ;
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.add_ind , "1" );	/* 增减1加 */	
	strcpy ( g_pub_tx.ct_ind , "2" );	/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/* 是否打印存折(有折无折标志)0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	g_pub_tx.svc_ind = 3062 ;
	strcpy( g_pub_tx.brf,"贷款还款计划(自动)" );
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG

	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"自动生成还款计划成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"自动生成还款计划失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	
	return 0;
	
}

int data_set_struct()
{    
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	
	get_zd_data("0370",g_pub_tx.ac_no);		/* 贷款账号 */	
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* 贷款账号序号 */

	sprintf(acErrMsg,"进入赋值函数贷款账号[%s]序号[%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG						
	return 0;
}

