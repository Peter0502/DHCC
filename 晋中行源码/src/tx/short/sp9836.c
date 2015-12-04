/*************************************************
* 文 件 名:    sp9836.c
* 功能描述：   
*              根据贷款账号取出账户序号
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
	
sp9836()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn,back_ac_seqn;
	int flag=0, redflag=0;
	char tx_code[8];
	
	
	struct ln_mst_c		ln_mst_tmp;
	struct cif_basic_inf_c        cif_basic_inf; 
	struct cif_id_code_rel_c      cif_id_code_rel; 
		
	/** 数据初始化 **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));	
	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));	
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0310" , ac_no );	/* 贷款帐号 */
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
	set_zd_data("0310",ac_no);	/* 处理旧帐号 */	
	
	get_zd_data( "0650" , tx_code ); /* 交易代码 */
	
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		ERR_DEAL
	} else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
    	}
	sprintf(acErrMsg,"取出的账户ID为[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	/* 根据贷款账户ID查询贷款主文件个数 */
	flag = sql_count("ln_mst", " ac_id=%ld " , g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg,"根据贷款账户ID查询贷款主文件个数[%d]",flag);
	WRITEMSG	
		
	if( flag==0 )
   	 {
 		set_zd_int( "0660", redflag );	/* 是否输入序号标志 */   
 		sprintf(acErrMsg,"贷款主文件表中无此记录[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L114");
 		goto ErrExit;
   	 }	

	 	
	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld " , g_mdm_ac_rel.ac_id );			
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL

		} else if( ret !=0 )
			{
    				sprintf( acErrMsg,"取贷款主文件异常 erro code=[%s]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    				ERR_DEAL
    			}	
    			
    	sprintf(acErrMsg,"取贷款主文件PASS");
	WRITEMSG 
	
	
	/* 非卡上开户专用通道 */

	
	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(ac_no,ln_mst_tmp.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("aaaaaaaaaaaaaaa[%ld]\n",g_ln_mst.cif_no);
	long tmpcif_no=0;
	tmpcif_no=g_ln_mst.cif_no;
	vtcp_log("bbbbbbbbbbbbbbbb[%ld]\n",tmpcif_no);
	if( strcmp( tx_code , "3308" )==0 )	
	{
			sprintf(acErrMsg,"交易3308!");
			WRITEMSG

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
			set_zd_double( "0850", g_ln_mst.over_rate );	/* 逾期利率 */
			set_zd_double( "0940", g_ln_mst.fine_rate );	/* 罚息利率 */					
			set_zd_data( "0690", g_ln_parm.ln_pay_type );/* 贷款支付方式 */	
			set_zd_data( "0700", g_ln_parm.auto_pay_ind );/* 自动还款标志 */	
			set_zd_data( "0710", g_ln_mst.intst_type );/* 计息类型 */
			set_zd_long( "0440", g_ln_mst.opn_date );	/* 开户日期 */	
			set_zd_long( "0450", g_ln_mst.mtr_date );	/* 到期日期 */		
			set_zd_double( "0430", g_ln_mst.amt_lmt );	/* 计划发放额 */
			set_zd_data( "0250", cif_basic_inf.name );	/* 客户名称 */	
			set_zd_data( "0670", cif_id_code_rel.id_type );	/* 证件类型 */		
			set_zd_data( "0620", cif_id_code_rel.id_no );	/* 证件号码 */		
			set_zd_double( "0560", g_ln_mst.intst_acm );	/* 本金积数 */					
			set_zd_data( "0930", g_ln_mst.cmpd_intst_ind );/* 复利标志 */
			if( g_ln_mst.repay_ac_id !=0 )
			{
				set_zd_long ( "0340", back_ac_seqn );	/* 还款账户序号 */
				set_zd_data( "0380", g_mdm_ac_rel.ac_no );	/* 还款账户 */	
				set_zd_data( "0260", g_dd_mst.name );	/* 还款账户户名 */
			}
		
	}			

	if( strcmp( tx_code , "3105" )==0 )	
	{
			sprintf(acErrMsg,"交易3105!");
			WRITEMSG
			/**
			if( g_ln_mst.ac_sts[0]!='8' )
			{
				sprintf(acErrMsg,"此贷款状态非待审核状态");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L116");
				goto ErrExit;
			}				
			**/
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
			if( g_ln_mst.repay_ac_id !=0 )
			{
				set_zd_long ( "0340", back_ac_seqn );	/* 还款账户序号 */
				set_zd_data( "0380", g_mdm_ac_rel.ac_no );	/* 还款账户 */	
				set_zd_data( "0260", g_dd_mst.name );	/* 还款账户户名 */
			}	
	}
	if( strcmp( tx_code , "3309" )==0 )	
	{
			sprintf(acErrMsg,"交易3309!");
			WRITEMSG
			/**
			if( g_ln_mst.ac_sts[0]!='8' )
			{
				sprintf(acErrMsg,"此贷款状态非待审核状态");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L116");
				goto ErrExit;
			}				
			**/
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
			/* 判断贷款是否是按揭贷款 */
			if( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
			{
				sprintf(acErrMsg,"此贷款非按揭贷款![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L042");
				goto ErrExit;				
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
			if( g_ln_mst.repay_ac_id !=0 )
			{
				set_zd_long ( "0340", back_ac_seqn );	/* 还款账户序号 */
				set_zd_data( "0380", g_mdm_ac_rel.ac_no );	/* 还款账户 */	
				set_zd_data( "0260", g_dd_mst.name );	/* 还款账户户名 */
			}	
	}					
	if( strcmp( tx_code , "3312" )==0 )	
	{
			sprintf(acErrMsg,"交易3312!");
			WRITEMSG
			
			/* del by lance 20031115
			if( g_ln_mst.ac_sts[0]!='1' )
			{
				sprintf(acErrMsg,"贷款帐户状态不正常,不可做此交易![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L137");
				goto ErrExit;
			}			
			******/
					
			/* 输出变量 */
			set_zd_data( "0250", g_ln_mst.name );/* 贷款户名 */
			set_zd_data( "0230", g_ln_mst.prdt_no );/* 产品编号 */
			set_zd_data( "0260", g_ln_parm.title );	/* 产品名称 */
			set_zd_data( "0670", g_ln_parm.ln_type );	/* 贷款方式 */	
			set_zd_data( "0680", g_ln_parm.cif_type );	/* 客户类型 */	
			set_zd_data( "0690", g_ln_parm.time_type );	/* 产品时限 */	
			set_zd_double( "0410", g_ln_mst.gage_amt );	/* 抵押品金额 */
			set_zd_long( "0440", g_ln_mst.mtr_date );	/* 到期日期 */
			set_zd_long( "0460", g_ln_mst.opn_date );	/* 开户日期 */												
	}
	if( strcmp( tx_code , "3322" )==0 )	
	{
			sprintf(acErrMsg,"交易3322!");
			WRITEMSG
			
		
					
			/* 输出变量 */
			set_zd_data( "0250", g_ln_mst.name );/* 贷款户名 */
			set_zd_data( "0230", g_ln_mst.prdt_no );/* 产品编号 */
			set_zd_data( "0260", g_ln_parm.title );	/* 产品名称 */
			set_zd_data( "0670", g_ln_parm.ln_type );	/* 贷款方式 */	
			set_zd_data( "0680", g_ln_parm.cif_type );	/* 客户类型 */	
			set_zd_data( "0690", g_ln_parm.time_type );	/* 产品时限 */	
			set_zd_double( "0410", g_ln_mst.bal );	/* 贷款余额 */
			set_zd_long( "0440", g_ln_mst.mtr_date );	/* 到期日期 */
			set_zd_long( "0460", g_ln_mst.opn_date );	/* 开户日期 */												
	}
		
	
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量");
	WRITEMSG
	redflag=1;
	set_zd_long( "0350", ln_mst_tmp.ac_seqn );	/* 贷款账户序号 */
	
	set_zd_int( "0660", redflag );	/* 是否输入序号标志 1-唯一记录*/    	
    	goto OkExit;
  }
    	
    	
  	redflag=9;
	set_zd_int( "0660", redflag );	/* 是否输入序号标志 9-多条记录*/ 	 		
		
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
   
