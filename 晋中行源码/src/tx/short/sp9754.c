/*************************************************
* 文 件 名:    sp9754.c
* 功能描述：   
*              合同利率返还逾期和罚息利率
*
* 作    者:    lance
* 完成日期：   2003年05月25日
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
	
sp9754()  
{ 		
	int ret=0;
	char prod_no[4];
	double rate_val , over_rate_val , fine_rate_val;
	
	
	/** 数据初始化 **/
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0230" , prod_no );	/* 产品代码 */
	get_zd_double( "0840" , &rate_val );	/* 合同利率 */
	
	/* 根据产品代码 返回产品信息 */

	sprintf(acErrMsg,"产品参数[%s]",prod_no);
	WRITEMSG	
	ret = pub_base_get_prdtparm( prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"调用根据产品代码取产品参数函数错误");
		WRITEMSG
		goto ErrExit;
	}		  	
	/** 计算逾期利率值 **/	
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
	if( g_ln_parm.over_rate_type[0]=='1' )/* 逾期取逾期利率编号 */
	{
		over_rate_val = rate_val*(100+g_ln_parm.over_rate_def)/100;
	}
				  
	sprintf(acErrMsg,"逾期利率值[%lf]",over_rate_val);
	WRITEMSG

	/** 计算罚息利率值 **/
	if( g_ln_parm.fine_rate_type[0]=='0' )/* 罚息取罚息利率编号 */
	{
		ret = pub_base_getllz( g_ln_parm.fine_rate_no , g_ln_parm.cur_no , g_pub_tx.tx_date , &fine_rate_val );
		if( ret )
		{
			sprintf(acErrMsg,"调用根据利率编号取罚息利率值函数错误");
			WRITEMSG
			goto ErrExit;
		}
		fine_rate_val = fine_rate_val/1.2;/* 转换成月利率 */
	}
	if( g_ln_parm.fine_rate_type[0]=='1' )/* 罚息取罚息利率编号 */
	{
		fine_rate_val = rate_val*(100+g_ln_parm.fine_rate_def)/100;
	}
				  
	sprintf(acErrMsg,"罚息利率值[%lf]",fine_rate_val);
	WRITEMSG
	
	
	/* 输出变量 */
	set_zd_double( "0850", over_rate_val );	/* 逾期利率 */
	set_zd_double( "0940", fine_rate_val );	/* 罚息利率 */		
			
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
   
