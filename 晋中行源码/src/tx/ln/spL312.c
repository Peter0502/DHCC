/*************************************************
* 文 件 名: spL312.c
* 功能描述：   
*              贷款产品转入
*
* 作    者:    lance
* 完成日期：   2003年06月06日
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
#include "cif_basic_inf_c.h"
#include "cif_prdt_rel_c.h"

static	long	 end_date_new;
static  double	 maga_amt_new;
static	char	 prod_no[4];
		
spL312()  
{ 	
	
	int ret=0;
	int tag=0;
	int amt_flag=0;
	
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	char type[2];
	long ac_id_t;
	double maga_amt_old=0.00;
	long end_date_old;

	struct	ln_mst_c	ln_mst_new;
	struct	ln_parm_c	ln_parm_new;	
	struct	cif_basic_inf_c	cif_basic_inf;	
	struct	cif_prdt_rel_c	cif_prdt_rel;	
			
	memset ( &ln_mst_new , 0x00 , sizeof( struct ln_mst_c ) );
	memset ( &ln_parm_new , 0x00 , sizeof( struct ln_parm_c ) );
	memset ( &cif_basic_inf , 0x00 , sizeof( struct cif_basic_inf_c ) );
	memset ( &cif_prdt_rel , 0x00 , sizeof( struct cif_prdt_rel_c ) );
				
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
	
	/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG

	/* 取新产品信息 */
	ret = Ln_parm_Sel( g_pub_tx.reply , &ln_parm_new, "prdt_no='%s'", prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款产品参数表无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret ){
    		sprintf( acErrMsg,"取贷款产品参数表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
	    		return -1;
    	}
	sprintf(acErrMsg,"取贷款产品参数信息 PASS!");
	WRITEMSG		
	sprintf(acErrMsg,"此两种贷款产品[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG
			
	/* del by lance 2003-11-11 判断贷款状态是否正常 *
	if( g_ln_mst.ac_sts[0]!='1'  )
	{
		sprintf(acErrMsg,"贷款帐户状态不正常,不可做此交易![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L137");
		goto ErrExit;
	}
	if( g_ln_mst.exp_ind[0]!='0'  )
	{
		sprintf(acErrMsg,"贷款帐户已展期,不可做此交易![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L138");
		goto ErrExit;
	}
	********/
	TRACE
	/* 客户类型必须相同 */
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
	ret = pub_base_check_cltprdt( cif_basic_inf.type, prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"新旧产品客户类型不相同,不可做此交易![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L139");
		ERR_DEAL;	
	}		
	TRACE
				
	set_zd_data("0210","01");	/* 币种 */
	maga_amt_old = g_ln_mst.gage_amt;
	end_date_old = g_ln_mst.mtr_date;
	sprintf(acErrMsg,"此两种贷款产品[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG

	/* 按揭不能转普通,普通不能转按揭 */	
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		if( ln_parm_new.ln_pay_type[0]!='3' && ln_parm_new.ln_pay_type[0]!='4' )
		{
			sprintf(acErrMsg,"按揭贷款和普通贷款间不可转换![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L141");
			goto ErrExit;			
		}
	}
				
	/* 几种转换方式详细处理 */
	/* 抵押 转 抵押(抵押金额变化) */
	if( g_ln_parm.ln_type[0]=='3' && ln_parm_new.ln_type[0]=='3' )
	{
		 amt_flag = pub_base_CompDblVal( maga_amt_old , maga_amt_new );
		 if( amt_flag!=0 )
		 {
			/* 对新产品记 贷704 */
			set_zd_double("107B",maga_amt_new);
			set_zd_data("107A","2");	/* 贷 */
			tag++;
		 }		  		 	                           
	}
	sprintf(acErrMsg,"此两种贷款产品ln_type=[%s][%s]",g_ln_parm.ln_type , ln_parm_new.ln_type);
	WRITEMSG
	/* 抵押 转 信用(本金科目) */
	if( g_ln_parm.ln_type[0]=='3' && ln_parm_new.ln_type[0]=='4' )
	{
		set_zd_double("1074",g_ln_mst.bal);
		tag++;			                           
	}					 		
	/* 抵押 转 担保(本金科目) */
	if( g_ln_parm.ln_type[0]=='3' && ln_parm_new.ln_type[0]=='5' )
	{
		set_zd_double("1074",g_ln_mst.bal);
		tag++;			                           
	}
	/* 担保 转 信用(本金科目) */
	if( g_ln_parm.ln_type[0]=='4' && ln_parm_new.ln_type[0]=='5' )
	{
		set_zd_double("1074",g_ln_mst.bal);
		tag++;			                           
	}
	/* 信用 转 担保(本金科目) */
	if( g_ln_parm.ln_type[0]=='5' && ln_parm_new.ln_type[0]=='4' )
	{
		set_zd_double("1074",g_ln_mst.bal);			                           
		tag++;		
	}				
	/* 信用 转 抵押(本金科目,贷抵押品) */
	if( g_ln_parm.ln_type[0]=='5' && ln_parm_new.ln_type[0]=='3' )
	{
		 amt_flag = pub_base_CompDblVal( maga_amt_old , maga_amt_new );
		 if( amt_flag!=0 )
		 {
			/* 对新产品记 贷704 */
			set_zd_double("107B",maga_amt_new);
			set_zd_data("107A","2");	/* 贷 */
			tag++;
		 }				
		set_zd_double("1074",g_ln_mst.bal);	
		tag++;					                           
	}
	/* 担保 转 抵押(本金科目,贷抵押品) */
	if( g_ln_parm.ln_type[0]=='4' && ln_parm_new.ln_type[0]=='3' )
	{
		 amt_flag = pub_base_CompDblVal( maga_amt_old , maga_amt_new );
		 if( amt_flag!=0 )
		 {
			/* 对新产品记 贷704 */
			set_zd_double("107B",maga_amt_new);
			set_zd_data("107A","2");	/* 贷 */
			tag++;
		 }				
		set_zd_double("1074",g_ln_mst.bal);	
		tag++;					                           
	}
	/* 期限变化 */
	sprintf(acErrMsg,"此两种贷款产品[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG	
	if( strcmp( g_ln_parm.time_type , ln_parm_new.time_type ) )
	{
		/* 本金科目改变 */
		set_zd_double("1074",g_ln_mst.bal);		
		tag++;
		/* 如果按揭贷款,需要重新生成还款计划 */
		if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
		{
			if( end_date_old != end_date_new )
			{
				ret = pub_ln_Chg_PayPlan( g_ln_mst , g_ln_parm , g_ln_mst.rate , g_ln_mst.bal , g_pub_tx.tx_date , end_date_new );
				if( ret ) goto ErrExit;
			}		
		}
	}
	
	/* 产品号变化 */
	if( strcmp( g_ln_parm.prdt_no, ln_parm_new.prdt_no) )
	{
		set_zd_double("1074",g_ln_mst.bal);
		tag++;
	}
	/*** 暂时屏蔽  gyf.20060810
	if( g_ln_mst.ac_id==100375336 )
	{
		;
	}else{
		tag=0;
	}***/	
	
	if( tag==0 )
	{
		sprintf(acErrMsg,"此两种贷款产品不可以转换,请检查![%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L140");
		goto ErrExit;
	}

	/* 贷款产品属性检查(开户) */

		
	if( pub_base_CompDblVal( g_ln_mst.gage_amt , maga_amt_new ) )
	{
		g_ln_mst.gage_amt = maga_amt_new;	/* 抵押品金额 */
	}
	if( end_date_new )
	{
		g_ln_mst.mtr_date = end_date_new;	/* 到期日 */
	}
	
	strcpy( type , "1" );
	strcpy(g_ln_mst.prdt_no,prod_no);	/* 贷款产品编号 */
	
	ret = pub_ln_check( type );
	if( ret )
	{
		sprintf(acErrMsg,"调用贷款产品属性检查函数错误!");
		WRITEMSG
		ERR_DEAL;	
	}
	sprintf(acErrMsg,"贷款产品属性检查(开户) PASS!");
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

	strcpy( g_ln_mst.prdt_no , prod_no );   		/* 产品代码 */
	
	if( pub_base_CompDblVal( g_ln_mst.gage_amt , maga_amt_new ) )
	{
		g_ln_mst.gage_amt = maga_amt_new;	/* 抵押品金额 */
	}
	if( end_date_new )
	{
		g_ln_mst.mtr_date = end_date_new;	/* 到期日 */
	}
		
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "修改贷款主文件错,存在重复记录!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret ){
		sprintf( acErrMsg,"执行Ln_mst_Upd_Upd错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
	Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"更新贷款主文件 PASS!");
	WRITEMSG
	
	/*  修改客户-产品关系表 */
	ret = Cif_prdt_rel_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and cif_no=%ld" ,g_ln_mst.ac_id , g_ln_mst.cif_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Cif_prdt_rel_Dec_Upd错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    
	ret = Cif_prdt_rel_Fet_Upd( &cif_prdt_rel,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "客户-产品关系表中无此记录" );
		WRITEMSG
		Cif_prdt_rel_Clo_Upd();
	}else if ( ret ){
		sprintf( acErrMsg, "执行Cif_prdt_rel_Fet_Upd出错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}else if( !ret ){

		strcpy( cif_prdt_rel.prdt_code , prod_no );   		/* 产品代码 */
			
		ret = Cif_prdt_rel_Upd_Upd( cif_prdt_rel, g_pub_tx.reply );
		if ( ret == -239 )
		{
			sprintf( acErrMsg, "修改客户-产品关系表错,存在重复记录!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L149");
			ERR_DEAL
		}else if( ret ){
			sprintf( acErrMsg,"执行Cif_prdt_rel_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
		Cif_prdt_rel_Clo_Upd();
		sprintf(acErrMsg,"更新客户-产品关系表 PASS!");
		WRITEMSG		
	}
	
	/* Pub_tx结构赋值 */
	g_pub_tx.tx_amt1 = g_ln_mst.bal;
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户ID */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 
	strcpy ( g_pub_tx.hst_ind , "1" );*/
	strcpy ( g_pub_tx.hst_ind , "0" );/*不入 根据晋中需求 modify by martin 2009/12/23 19:38:19*/
	g_pub_tx.svc_ind = 3060;			/* 贷款结转 */
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" ); 
	strcpy ( g_pub_tx.brf,"贷款产品转入(本金)" );
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
	
	/*** 登记交易流水 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"登记交易流水 PASS!");
	WRITEMSG
			
	/* Pub_tx结构赋值 */
	g_pub_tx.tx_amt1 = maga_amt_new;
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户ID */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 
	strcpy ( g_pub_tx.hst_ind , "1" );*/
	strcpy ( g_pub_tx.hst_ind , "0" );/*不入 根据晋中需求 modify by martin 2009/12/23 19:38:19*/
	g_pub_tx.svc_ind = 3060;			/* 贷款结转 */
	strcpy( g_pub_tx.ac_wrk_ind , "0000000" ); 
	strcpy ( g_pub_tx.brf,"贷品转入(抵部分)" );
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
	
	/*** 登记交易流水 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"登记交易流水 PASS!");
	WRITEMSG	

	/* 打印程序 */
	/* 贷款产品转换传票--转入部分 */
	ret = print_LNBKCPZR( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKCPZR,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE	

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款产品转入成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款产品转入失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    if ( strcmp( g_pub_tx.reply , "L018" )==0 )
    {
    	strcpy( g_pub_tx.reply , "L115" );
    }
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	return 0;	
}

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("0310" , g_pub_tx.ac_no); 		/* 贷款账号 */
	g_pub_tx.ac_seqn = 0;				/* 账号序号 */

	get_zd_data("0240" , prod_no); 			/* 新产品代码 */
	get_zd_double("0420" , &maga_amt_new); 		/* 新抵押品金额 */
	get_zd_long("0450" , &end_date_new); 		/* 新到期日期 */
	sprintf(acErrMsg,"到期日期=[%ld]",end_date_new);	
	WRITEMSG						
	return 0;
}
/* 打印--贷款产品转换传票--转入部分 */
int print_LNBKCPZR( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	char exp_ind[5];
	char ln_sts[5];	
	char dc_sts[3];	
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
		sprintf(acErrMsg,"LNBKCPZR打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "转入" );	
	
	/* 取科目号 */
	strcpy( data_code , "0152" );	/* 本金科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	/* 借贷标志 */
	strcpy( dc_sts , "借" );
	
	/* 贷款状态 */
	pub_base_dic_show_str(ln_sts,"ln_ac_sts",g_ln_mst.ac_sts);	
		
	/* 欠本总额 */	
	tol_lo = g_ln_mst.ttl_prvd_amt ;
	
	/* 累计还本 */	
	tol_pay = g_ln_mst.ttl_prvd_amt - g_ln_mst.bal;
			
	/* 尚欠本金 */		
	tol_need_pay = g_ln_mst.bal;

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
	/* 16.流水号、   17.借贷标志、 18.贷款状态  19.红字标志*/
	fprintf(fp,"LNBKCPZR%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	g_ln_mst.bal , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , 
	g_pub_tx.trace_no , dc_sts , ln_sts ,"");	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
