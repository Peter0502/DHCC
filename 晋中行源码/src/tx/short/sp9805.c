/*************************************************
* 文 件 名:    sp9805.c
* 功能描述：   
*              贷款产品代码返回产品信息
*
* 作    者:    lance
* 完成日期：   2003年01月13日
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
	
sp9805()  
{ 		
	int ret=0;
	char prod_no[4];
	double rate_val , over_rate_val;
	/* begin add by LiuHuafeng 2008-11-27 12:18:03 */
	rate_val=0.0;
	over_rate_val=0.0;
	/* end by LiuHuafeng 2008-11-27 12:18:10 */
	/** 数据初始化 **/
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0230" , prod_no );	/* 产品代码 */
	
	/* 根据产品代码 返回产品名称 */

	ret = pub_base_get_prdtparm( prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"调用根据产品代码取产品参数函数错误");
		WRITEMSG
		goto ErrExit;
	}		  
	sprintf(acErrMsg,"名称[%s]",g_ln_parm.title);
	WRITEMSG

	sprintf(acErrMsg,"利率编号[%s]",g_ln_parm.rate_no);
	WRITEMSG
	sprintf(acErrMsg,"币种[%s]",g_ln_parm.cur_no);
	WRITEMSG
	sprintf(acErrMsg,"日期[%ld]",g_pub_tx.tx_date);
	WRITEMSG
	sprintf(acErrMsg,"自动还款[%s]",g_ln_parm.auto_pay_ind);
	WRITEMSG	
	
	/* 根据利率编号取利率值 */
	ret = pub_base_getllz( g_ln_parm.rate_no , g_ln_parm.cur_no , g_pub_tx.tx_date , &rate_val );
	if( ret )
	{
		sprintf(acErrMsg,"调用根据利率编号取正常利率值函数错误");
		WRITEMSG
		goto ErrExit;
	}		  
	sprintf(acErrMsg,"正常利率值[%lf]",rate_val);
	WRITEMSG
	rate_val = rate_val/1.2;/* 转换成月利率 */
	vtcp_log("%s,%d over_rate_type==%s,over_rate_def=%f",__FILE__,__LINE__,g_ln_parm.over_rate_type,g_ln_parm.over_rate_def*1.0);
	if( g_ln_parm.over_rate_type[0]=='0' )/* 逾期取逾期利率编号 */
	{
		ret = pub_base_getllz( g_ln_parm.over_rate_no , g_ln_parm.cur_no , g_pub_tx.tx_date , &over_rate_val );
		if( ret )
		{
			sprintf(acErrMsg,"调用根据利率编号取逾期利率值函数错误");
			WRITEMSG
			goto ErrExit;
		}
		over_rate_val = over_rate_val/1.2;/* 转换成月利率 */
	}
	if( g_ln_parm.over_rate_type[0]=='0' )/* 逾期取逾期利率编号 */
	{
		over_rate_val = rate_val*(100+g_ln_parm.over_rate_def)/100;
	}
				  
	sprintf(acErrMsg,"逾期利率值[%lf]",over_rate_val);
	WRITEMSG


	sprintf(acErrMsg,"贷款类型[%s]",g_ln_parm.ln_type);
	WRITEMSG
	/* 输出变量 */
	set_zd_data( "0810", g_ln_parm.title );	/* 产品名称 */
	set_zd_data( "0210", g_ln_parm.cur_no );/* 币种 */
	set_zd_data( "0680", g_ln_parm.get_rate_type );/* 利率取得方式 */	
	set_zd_double( "0970", rate_val );	/* 正常利率 */
	set_zd_double( "0840", rate_val );	/* 正常利率 */
	set_zd_double( "0850", over_rate_val );	/* 逾期利率 */	
	set_zd_data( "0690", g_ln_parm.ln_pay_type );/* 贷款支付方式 */	
	set_zd_data( "0700", g_ln_parm.auto_pay_ind );/* 自动还款标志 */	
	set_zd_data( "0710", g_ln_parm.intst_type );/* 计息类型 */	
	set_zd_data( "0720", g_ln_parm.pay_pln_type );/* 放款计划类型 */	
	set_zd_data( "0660", g_ln_parm.pay_pln_crt );/* 还款计划生成类型 */		
	set_zd_data( "105A", g_ln_parm.trust_prot_ind );/* 是否委托协议贷款 */	
	set_zd_data( "1145", g_ln_parm.ln_type );/* 贷款类型 */
			
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
   
