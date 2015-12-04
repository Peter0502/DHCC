/*************************************************
* 文 件 名: spL314.c
* 功能描述：   
*              贷款抵押品出库
*
* 作    者:    lance
* 完成日期：   2003年07月31日
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
#include "trace_log_c.h"
#include "ln_gage_reg_c.h"

struct	ln_gage_reg_c	ln_gage_reg;
struct	ln_gage_reg_c	s_ln_gage_reg;
			
spL314()  
{ 		
	int ret=0;
	char sts[2];

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
			
	/** 处理流程 **/ 				
 
	/* 更新贷款抵押品登记簿 */
/*
	ret = Ln_gage_reg_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and pact_no='%s' and amt=%lf and gage_type='%s' and sts='0' " ,g_ln_mst.ac_id , g_ln_mst.ac_seqn ,g_ln_mst.pact_no ,ln_gage_reg.amt ,ln_gage_reg.gage_type);
*/
	ret = Ln_gage_reg_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and amt=%lf and gage_type='%s' and sts='0' " ,g_ln_mst.ac_id , g_ln_mst.ac_seqn ,ln_gage_reg.amt ,ln_gage_reg.gage_type);

	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_mst_Dec_Upd错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    
	ret = Ln_gage_reg_Fet_Upd( &s_ln_gage_reg,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "抵押品登记簿中无此记录,请检查!" );
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L211" );		
		ERR_DEAL
	}else if ( ret ){
		sprintf( acErrMsg, "执行Ln_gage_reg_Fet_Upd出错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    	/* 修改 */    	
	s_ln_gage_reg.out_tx_date = g_pub_tx.tx_date;
	s_ln_gage_reg.out_trace_no = g_pub_tx.trace_no;	
	vtcp_log("%d@%s  更改前s_ln_gage_reg.amt [%lf] \n", __LINE__, __FILE__, s_ln_gage_reg.amt);
	s_ln_gage_reg.amt -= ln_gage_reg.amt;
	vtcp_log("%d@%s  更改后s_ln_gage_reg.amt [%lf] \n", __LINE__, __FILE__, s_ln_gage_reg.amt);

	strcpy( s_ln_gage_reg.sts , "1" );		/* 状态改为－－出库 */
	
	
	ret = Ln_gage_reg_Upd_Upd( s_ln_gage_reg, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "贷款抵押品登记簿,存在重复记录!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L212");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"执行Ln_gage_reg_Upd_Upd出错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	Ln_gage_reg_Clo_Upd();
	sprintf(acErrMsg,"更新贷款抵押品登记簿 PASS!");
	WRITEMSG	



	/* 更新贷款主文件 */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_mst_Dec_Upd错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    
	ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "贷款主文件中无此记录" );
		WRITEMSG
		ERR_DEAL
	}else if ( ret ){
		sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
	
	/* 金额检查( 对于2003-08-20以前旧系统开户的数据不检查 ) 
	if( g_ln_mst.opn_date > 20030820 )
    	{
	    	if( pub_base_CompDblVal( g_ln_mst.gage_amt , ln_gage_reg.amt)<0 )
		{
			sprintf( acErrMsg, "交易金额大于贷款现有抵押品金额,请检查!",ret );
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L190" );		
			ERR_DEAL
	    	}
	}
	*****/
	
    	/* 修改 */    	
	vtcp_log("%d@%s  更改前s_ln_mst.gage_amt [%lf] \n", __LINE__, __FILE__, g_ln_mst.gage_amt);
	g_ln_mst.gage_amt = g_ln_mst.gage_amt - ln_gage_reg.amt;		/* 抵押品金额 */
	vtcp_log("%d@%s  更改后s_ln_mst.gage_amt [%lf], g_ln_mst.bal[%lf] -[%lf] \n", __LINE__, __FILE__, g_ln_mst.gage_amt, g_ln_mst.bal, g_ln_mst.bal-g_ln_mst.gage_amt);

	if( g_ln_mst.bal - g_ln_mst.gage_amt > 0 ){
		sprintf( acErrMsg, "抵质金额不能小于贷款金额  bal-> %lf gage_amt -> %lf", g_ln_mst.bal, g_ln_mst.gage_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"T156");
		ERR_DEAL
	}
	
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "修改贷款主文件错,存在重复记录!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"执行Ln_mst_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	vtcp_log("%d@%s  更改后s_ln_mst.gage_amt [%lf] \n", __LINE__, __FILE__, g_ln_mst.gage_amt);
	Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"更新贷款主文件 PASS!");
	WRITEMSG	


	/* 记账 借70400 */
	set_zd_data( "0210" , "01" );
	g_pub_tx.tx_amt1 = ln_gage_reg.amt;
		
	/* Pub_tx结构赋值 */
 
	strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3061;			/* 贷款抵押品金额出库 */
	strcpy ( g_pub_tx.brf,"贷款抵押品金额出库" );	
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG	
	
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		ERR_DEAL
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
	
	/* 打印程序  */
	/* 打印----抵押品出库借方传票 */
	ret = print_LNDYCK( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNDYCK,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE
				

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款抵押品出库成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款抵押品出库失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	memset(&ln_gage_reg , 0x00 , sizeof(struct ln_gage_reg_c) );
	memset(&s_ln_gage_reg , 0x00 , sizeof(struct ln_gage_reg_c) );
	return 0;	
}

int data_set_struct()
{  
	int ret=0;  
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* 贷款帐号 */
	sprintf(acErrMsg,"贷款帐号[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	get_zd_double("0420",&ln_gage_reg.amt);			/* 出库金额 */
	sprintf(acErrMsg,"交易金额[%ld]",ln_gage_reg.amt);
	WRITEMSG	
	
	get_zd_data("0220",ln_gage_reg.gage_type);		/* 抵押品类型 */
	get_zd_data("0810",ln_gage_reg.gage_inf);		/* 抵押品信息 */
			
	g_pub_tx.ac_seqn = 0 ;	/* 贷款账户序号 */
		
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
				&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}
						
	return 0;
}

/* 打印--抵押品出库借方传票*/
int print_LNDYCK( )
{
	char tx_type[9];

	char exp_ind[5];
	char in_out_ind[5];
	char dc_ind[5];
	char gage_name[21];
	
	char rate[11];
	char over_rate[11];
			
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNDYCK打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "抵押品出库" );	
	
	/* 取科目号 */
	strcpy( ln_acc_hrt , "704" );	/* 本金科目 */		
	
	/* 出入库标志 */
	strcpy( in_out_ind , "出库");
	
	/* 借贷标志 */
	strcpy( dc_ind , "借方");
	
	/* 抵押品类型 */
	pub_base_dic_show_str(gage_name,"gage_type",ln_gage_reg.gage_type);		
	
	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(展)" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.exp_rate );		
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	}
		
	sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf‰" , g_ln_mst.over_rate);

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.开户日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.正常利率、 13.逾期利率、14.科目号、   15.展期标志  */
	/* 16.流水号、   17.出入库、   18.借贷标志、19.抵押品类型、20.备注  */
	/* 21.抵押品价值、	*/
	fprintf(fp,"LNDYCK%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|%s|%.2lf|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	ln_gage_reg.amt , g_ln_mst.opn_date , end_date , 0.00 , 0.00 ,
	g_ln_mst.bal , rate , over_rate , ln_acc_hrt , exp_ind , 
	g_pub_tx.trace_no , in_out_ind , dc_ind , gage_name , s_ln_gage_reg.gage_inf ,
	g_ln_mst.gage_amt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
