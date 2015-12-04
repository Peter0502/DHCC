/*************************************************
* 文 件 名: spL105.c
* 功能描述：   
*              贷款审批
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
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "ln_auth_c.h"
			
spL105()  
{ 		
	int ret=0;
	char sts[2];
	char pay_ac_no[20];
	char save_ac_no[20];
	long cif_no;
	
	struct ln_auth_c	ln_auth;
	struct ln_auth_c	ln_auth_old;	
	
	memset( &ln_auth , 0x00 , sizeof( struct ln_auth_c ) );
	memset( &ln_auth_old , 0x00 , sizeof( struct ln_auth_c ) );
	pub_base_sysinit();
	get_zd_data("0660" , sts);		/* 操作类型 */
	get_zd_data("0630" , ln_auth.pact_no);	/* 借 据 号 */
	get_zd_data("0250" , ln_auth.name);	/* 贷款户名 */
	get_zd_data("0380" , ln_auth.pay_ac_no);/* 还款账号 */
	get_zd_data("0320" , ln_auth.save_ac_no);/* 存款账户 */
	get_zd_double("0400" , &ln_auth.amt);	/* 贷款金额 */
	get_zd_double("0840" , &ln_auth.rate);	/* 合同利率 */	
	get_zd_int("0350" , &ln_auth.time_lmt);	/* 贷款期限 */
  pub_base_old_acno(ln_auth.pay_ac_no);
  pub_base_old_acno(ln_auth.save_ac_no);
 	/**** 
	DBG_printf("~~Start %d@%s  in(%s)  --> trace_no[%ld] \n", __LINE__, __FILE__, __FUNCTION__, g_pub_tx.trace_no);
	****/
	
	/* 根据还款账号取还款账号ID */
	pub_base_strpack( ln_auth.pay_ac_no );
	if( strlen(ln_auth.pay_ac_no) )
	{
		ret = pub_ln_AcCheck( ln_auth.pay_ac_no,1,&cif_no,&ln_auth.pay_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L184");
			}
			goto  ErrExit;
		}

	}	
	/* 根据存款账号取存款账号ID */	
	pub_base_strpack( ln_auth.save_ac_no );
	if( strlen( ln_auth.save_ac_no ) )
	{
		ret = pub_ln_AcCheck( ln_auth.save_ac_no,1,&cif_no,&ln_auth.save_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L185");
			}
			goto ErrExit;
		}

	}
	/* 根据存款账号取存款账号介质类型 */
	strcpy( ln_auth.save_mdm_code , g_mdm_ac_rel.mdm_code );
		 
	
	if( sts[0]=='0')	/* 录入 */
	{				
		/** 添加贷款审批信息表 **/
		strcpy( ln_auth.sts ,"0" );		/* 借据状态----审批 */
		ret = Ln_auth_Ins( ln_auth , g_pub_tx.reply );
		if (ret&&ret!=-239)
		{
			sprintf(acErrMsg,"登记贷款审批信息簿异常出错,请与中心机房联系![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L164");
			goto ErrExit;
		}else if(ret==-239){
			sprintf(acErrMsg,"贷款审批信息簿有重复记录![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L165");
			goto ErrExit;
		}							
		sprintf(acErrMsg,"登记贷款审批信息簿 PASS!");
		WRITEMSG
		/* 查询保函登记簿中是否有重复的合同号 */
		/* 查询承兑登记簿中是否有重复的合同号 */
						
	}else if( sts[0]=='1' ){	/* 修改 */
	
		ret = Ln_auth_Dec_Upd( g_pub_tx.reply , "pact_no='%s'" , ln_auth.pact_no );
		if( ret )
		{
			sprintf( acErrMsg,"Prepare Ln_auth_Dec_Upd 游标出错[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}
		ret = Ln_auth_Fet_Upd( &ln_auth_old , g_pub_tx.reply );
		if( ret==100 )
		{
			sprintf( acErrMsg,"无此审批记录![%d]",ret);
			WRITEMSG
		 	strcpy( g_pub_tx.reply ,"L175" );
			ERR_DEAL ;
		}else if( ret ){
		 	sprintf( acErrMsg,"fetch Ln_auth_Fet_Upd error code = [%d]",ret);
		 	WRITEMSG
		 	strcpy( g_pub_tx.reply ,"D103" );
		 	ERR_DEAL ;
		}
		/*MIS 传过来得实xX*/
		if('x' == ln_auth_old.sts[0] || 'X' == ln_auth_old.sts[0]) 
			strcpy( ln_auth.sts ,"0" );	
		else		
			strcpy( ln_auth.sts ,ln_auth_old.sts );		/* 借据状态 */	
		strcpy(ln_auth.rowid, ln_auth_old.rowid); /*rowid   没有加上, gujy 20060824*/
		ln_auth.opn_date=ln_auth_old.opn_date;
		ln_auth.mtr_date=ln_auth_old.mtr_date;

		ret = Ln_auth_Upd_Upd( ln_auth, g_pub_tx.reply  );
		if ( ret )
		{
			sprintf( acErrMsg,"Ln_auth_Upd_Upd 出错[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
		Ln_auth_Clo_Upd( );
		/****
		DBG_printf("%d@%s ====%d \n", __LINE__, __FILE__, ret);
		****/
		sprintf(acErrMsg,"修改审批信息PASS!");
		WRITEMSG			
	}else if( sts[0]=='2' ){	/* 删除 */
		ret=sql_execute("delete from ln_auth where pact_no='%s'",ln_auth.pact_no);
		if (ret)	ERR_DEAL;	
	
	}
	vtcp_log("~~Start[%d][%s]--> trace_no[%ld] \n", __LINE__, __FILE__,g_pub_tx.trace_no);
	 /*** 登记交易流水 ***/
	 	strcpy(g_pub_tx.brf,"贷款审批");
    if( pub_ins_ln_trace_log(ln_auth.amt))
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg,"登记交易流水 PASS!");
    WRITEMSG

			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款审批成功！[%s]",g_pub_tx.reply);
    WRITEMSG
/****
DBG_printf("~~END 贷款审批成功 %d@%s  in(%s)  --> trace_no[%ld] \n", __LINE__, __FILE__, __FUNCTION__, g_pub_tx.trace_no);
****/
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款审批失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    

