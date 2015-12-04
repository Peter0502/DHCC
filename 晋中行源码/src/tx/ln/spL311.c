/*************************************************
* 文 件 名: spL311.c
* 功能描述：   
*              按揭贷款还款计划修改
*
* 作    者:    lance
* 完成日期：   2003年04月01日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"
#include "cif_basic_inf_c.h"

static struct ln_lo_c		ln_lo;
static struct ln_pay_pln_c	ln_pay_pln;
static struct cif_basic_inf_c	cif_basic_inf;
static double chg_rate;
static long end_day;
	
spL311()  
{ 	
	int ret;
	

	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_get_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
	
	/** 处理流程 **/	
	if( pub_base_CompDblVal(g_ln_mst.bal , 0.00 )==0 )
	{
		sprintf(acErrMsg,"按揭贷款已还清!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L157");
	}
	
	/*检查是否符合产品要求*/	
	/* 根据客户号查询客户名称 */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , g_ln_mst.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该证件对应的客户号不存在[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C020");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}				
	TRACE
	/* 客户类型是否合乎产品定义 */
	ret = pub_base_check_cltprdt( cif_basic_inf.type, g_ln_parm.prdt_no );
	if( ret )
	{
		sprintf(acErrMsg,"此调整不符合贷款产品要求,请检查是否日期或利率超限![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L233");
		ERR_DEAL;	
	}		
	TRACE
	
	
						
	/***** 生成新的还款计划 *********/
	ret = pub_ln_Chg_PayPlan( g_ln_mst , g_ln_parm , chg_rate , g_ln_mst.bal-g_pub_tx.tx_amt1 , g_pub_tx.tx_date , end_day );
	if( ret ) goto ErrExit;
	
	/* Pub_tx结构赋值 */
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );	
	g_pub_tx.svc_ind = 3060 ;
	strcpy ( g_pub_tx.brf,"按揭贷款还款计划修改" );

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
    sprintf(acErrMsg,"按揭贷款还款计划修改成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"按揭贷款还款计划修改失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
		
	memset(&ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("0310",g_pub_tx.ac_no);		/* 帐号 */
	get_zd_int("0350",&g_pub_tx.ac_seqn);		/* 账户序号 */
	get_zd_long("0470",&end_day);			/* 到期日 */
	get_zd_double("1123",&chg_rate);		/* 利率 */

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *检查介质状态*
	if( ret ) return ret; */
	
	/**
	ret = pub_ln_check_sts( g_ln_mst,0 ); *检查贷款户状态*
	if( ret ) {return ret;}
	**/
					
	if( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
	{
		sprintf(acErrMsg,"该贷款非按揭贷款![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L042" );
		return 1;
	}
	return 0;
}

