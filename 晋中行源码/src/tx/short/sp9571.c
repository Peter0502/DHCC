/*************************************************
* 文 件 名:    sp4518.c
* 功能描述：   
*              修改抵押品类型
*
* 作    者:    
* 完成日期：   
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "ln_gage_reg_c.h"

sp4518()
{ 		
	int ret;
	char  var1[20];
	struct ln_mst_c   s_ln_mst;
	struct mdm_ac_rel_c    s_mdm_ac_rel;
	struct ln_gage_reg_c    s_ln_gage_reg;
	
	memset( &s_ln_mst , 0x00 , sizeof(struct ln_mst_c) );
	memset( &s_mdm_ac_rel , 0x00 , sizeof(struct mdm_ac_rel_c) );
	memset( &s_ln_gage_reg , 0x00 , sizeof(struct ln_gage_reg_c) );
	
	
	memset(var1,0x00,sizeof(var1));
	
	
	get_zd_data("0300",var1);
	vtcp_log("[%s][%d]var1===[%s]\n",__FILE__,__LINE__,var1);
	
	
	/** 取值、赋值 **/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no='%s'",var1);/* 获取贷款帐号 */
	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"[%s][%d]贷款帐号不存在!!ac_no=[%s]",__FILE__,__LINE__,var1);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"HV01");
		 	set_zd_data("0130","贷款帐号不存在");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"查询介质与账户关系表异常!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}
	
	
	/****锁定贷款登记主表****/
	ret = Ln_mst_Sel(g_pub_tx.reply,&s_ln_mst,"ac_id=%ld",s_mdm_ac_rel.ac_id);/* 获取贷款帐号 */
	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"[%s][%d]贷款帐号不存在!!ac_id=[%ld]",__FILE__,__LINE__,s_ln_mst.ac_id);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O075");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"锁定贷款登记主表!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}
	 	
	 	vtcp_log("[%s][%d]获取到的合同号====[%s]\n",__FILE__,__LINE__,s_ln_mst.pact_no);
	 	
	 	/*******锁定贷款抵押品登记簿******/
	 	
	 ret = Ln_gage_reg_Sel(g_pub_tx.reply,&s_ln_gage_reg,"ac_id=%ld and  pact_no='%s' ",s_ln_mst.ac_id,s_ln_mst.pact_no);/* 获取贷款帐号 */
	 if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"[%s][%d]锁定贷款抵押品登记簿错误!!ac_id=[%ld]",__FILE__,__LINE__,s_ln_mst.ac_id);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"HV01");
		 	set_zd_data("0130","锁定贷款抵押品登记簿错误");    
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"锁定贷款抵押品登记簿错误!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}



	  set_zd_data("0250",s_ln_mst.name);
	  set_zd_data("0630",s_ln_mst.pact_no);
	  set_zd_double("0410",s_ln_mst.bal);
	  set_zd_double("0400",s_ln_mst.gage_amt);
		set_zd_data("0220",s_ln_gage_reg.gage_type);
		set_zd_double("0420",s_ln_gage_reg.amt);
		set_zd_data("0830",s_ln_gage_reg.gage_inf);
		


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