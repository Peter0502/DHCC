/*************************************************
* 文 件 名:    sp9835.c
* 功能描述：   
*              根据贷款账号,账户序号返回贷款信息
*
* 作    者:    lance
* 完成日期：   2003年01月25日
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
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"

sp9835()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn,back_ac_seqn;
	
	struct cif_basic_inf_c        cif_basic_inf; 
	struct cif_id_code_rel_c      cif_id_code_rel; 
	
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));	
	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));	
		
	/** 数据初始化 **/
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0310" , ac_no );	/* 贷款帐号 */
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
	set_zd_data("0310",ac_no);	/* 处理旧帐号 */	
		
	get_zd_int( "0350" , &ac_seqn );	/* 账户序号 */
	
	/* 读取贷款户信息 */

	ret = pub_ln_lndis_file( ac_no, ac_seqn,&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret ) {return ret;}	
	
	/* 根据客户号查询客户信息 */		  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , g_ln_mst.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该客户号不存在[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}
	
	/* 根据客户号查询客户证件信息 */		  
	ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
					" cif_no=%ld " , g_ln_mst.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该客户号不存在[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取客户证件信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C004");
		goto ErrExit;
	}						  
	
	/* 查询还款账户信息 */
	ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &back_ac_seqn );
	if( ret ) {return ret;}	
	if( g_ln_mst.repay_ac_id !=0 )
	{
		ret = pub_base_currentid_file( g_ln_mst.repay_ac_id,back_ac_seqn, &g_dd_mst );
		if( ret ) {return ret;}

		Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , 
						" ac_id=%ld " , g_ln_mst.repay_ac_id );
		if( ret ) {return ret;}	
	}
	
	/* 输出变量 */
	set_zd_data( "0270", g_ln_mst.name );/* 贷款户名 */
	set_zd_data( "0230", g_ln_mst.prdt_no );/* 产品编号 */
	set_zd_data( "0810", g_ln_parm.title );	/* 产品名称 */
	set_zd_data( "0630", g_ln_mst.pact_no );	/* 合同号 */	
	set_zd_data( "0610", g_ln_mst.cal_code );	/* 指标体系代码 */		
	set_zd_data( "0210", g_ln_parm.cur_no );/* 币种 */
	set_zd_data( "0680", g_ln_parm.get_rate_type );/* 利率取得方式 */	
	set_zd_double( "0840", g_ln_mst.rate );	/* 正常利率 */
	set_zd_double( "0400", g_ln_mst.advis_amt_pln );	/* 财务顾问费 */	
	set_zd_double( "0850", g_ln_mst.over_rate );	/* 逾期利率 */		
	set_zd_data( "0690", g_ln_parm.ln_pay_type );/* 贷款支付方式 */	
	set_zd_data( "0700", g_ln_parm.auto_pay_ind );/* 自动还款标志 */	
	set_zd_data( "0710", g_ln_parm.intst_type );/* 计息类型 */
	set_zd_long( "0440", g_ln_mst.opn_date );	/* 开户日期 */	
	set_zd_long( "0450", g_ln_mst.mtr_date );	/* 到期日期 */		
	set_zd_double( "0430", g_ln_mst.amt_lmt );	/* 计划发放额 */
	set_zd_data( "0250", cif_basic_inf.name );	/* 客户名称 */	
	set_zd_data( "0670", cif_id_code_rel.id_type );	/* 证件类型 */		
	set_zd_data( "0620", cif_id_code_rel.id_no );	/* 证件号码 */		
	set_zd_data( "0260", g_dd_mst.name );	/* 还款账户户名 */
	if( g_ln_mst.repay_ac_id !=0 )
	{
		set_zd_int ( "0340", back_ac_seqn );	/* 还款账户序号 */
		set_zd_data( "0380", g_mdm_ac_rel.ac_no );	/* 还款账户 */	
	}
				
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
   
