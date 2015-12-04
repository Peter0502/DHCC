/*************************************************
* 文 件 名: spL315.c
* 功能描述：   
*              贷款冲正与补记
*
* 作    者:    lance
* 完成日期：   2003年06月13日
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

	char	 cb_flag[2];
	char	 dc_flag[2];
	char	 r_str[7];
	char	 bal_dc_ind[2];
	char	 in_dc_ind[2];
	char 	 out_dc_ind[2];
	char	 cmpd_dc_ind[2];

static  double	 bal=0.00,bal_acm=0.00;
static  double	 in_lo_intst=0.00,in_acm=0.00;
static  double	 out_lo_intst=0.00,out_acm=0.00;
static  double	 cmpd_lo_intst=0.00,cmpd_acm=0.00;
static  double	 gage_amt=0.00;
		
spL315()  
{ 	
	int ret=0;
	double acm=0.00;
	char acm_type[2];
		
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
	
	/* 限制当日不可以做冲补交易 */
	if( g_pub_tx.tx_date == g_pub_tx.erro_date )
	{
	        sprintf(acErrMsg,"当日错账不可做冲补交易,请做撤销交易!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L158");
			goto ErrExit;
	}
	
	/* 取贷款信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}

	/* 按揭贷款不可做冲补交易 */
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"按揭贷款不可做冲正补记交易!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L223" );
		goto ErrExit;
	}		
	sprintf(acErrMsg,"ac_id[%ld] [%d] [%lf]",g_ln_mst.ac_id,g_ln_mst.ac_seqn,g_ln_mst.ttl_prvd_amt );
	WRITEMSG	
   	sprintf(acErrMsg,"1标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG	
	set_zd_data("0210","01");	/* 币种 */

	strcpy ( g_pub_tx.ac_get_ind,"111" );  /*已经读取了mdm_ac_rel, g_ln_mst*/
	/* 贷款本金金额冲补 */
	if( pub_base_CompDblVal( bal , 0.00 ) )
	{
		TRACE
		acm = 0.00;
		/* 计算积数  */
		strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
		
		ret = pub_base_CalAcm_Ln( g_pub_tx.erro_date , g_pub_tx.tx_date , 
			g_ln_parm.intst_days_type , acm_type , 
			bal ,g_pub_tx.tx_amt1 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			goto ErrExit;	
		}
		sprintf(acErrMsg,"调用函数计算积数(本金积数)PASS为 [%lf]",acm );
		WRITEMSG	
		bal_acm = acm;	
		/****
		if( pub_base_CompDblVal(g_ln_mst.ttl_prvd_amt,0.00)==0 )
		****/
		if( pub_base_CompDblVal(g_ln_mst.amt_lmt,0.00)==0 )
		{
			sprintf(acErrMsg,"贷款尚未放款！");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L155");
			goto ErrExit;				
		}
		TRACE
		if( cb_flag[0]=='1' )	/* 冲正 */
		{
			g_pub_tx.tx_amt1 = -bal ;			
		}
		if( cb_flag[0]=='2' )	/* 补记 */
		{
			g_pub_tx.tx_amt1 = bal ;
		}
		
		set_zd_double( "1084" , g_pub_tx.tx_amt1 );
		set_zd_data( "0210" , "01" );	
			
		if( g_pub_tx.add_ind[0]=='0' )	/* 增(冲补----贷款放款) 原为 1*/
		{
			set_zd_data( "0660" , "2" ); /* 借 原为1*/
			strcpy( bal_dc_ind, "2" );
			g_pub_tx.svc_ind = 3091 ;/*3091*/
		}
		if( g_pub_tx.add_ind[0]=='1' )	/* 减(冲补----贷款还本)原为0 */
		{
			set_zd_data( "0660" , "1" ); /* 贷 原为2*/;
			strcpy( bal_dc_ind, "1" );
			g_pub_tx.svc_ind = 3091	;	/*3019*/	
		}
		
		/* 冲补积数 */
		ret = ln_acm_modify( cb_flag , g_pub_tx.add_ind , "1" , acm );
		if( ret ) 	goto ErrExit;
		
			
		/* Pub_tx结构赋值 */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户ID */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.ac_get_ind,"111" );  /*已经读取了mdm_ac_rel, g_ln_mst*/
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */	
		strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
		/* 是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.prt_ind , "1" );	
		/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"贷款本金冲正" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"贷款本金补记" );
		}			
		sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
		WRITEMSG
			sprintf(acErrMsg,"后(本金积数)PASS为 [%lf]",g_ln_mst.intst_acm );
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
			sprintf(acErrMsg,"后(本金积数)PASS为 [%lf]",g_ln_mst.intst_acm );
			WRITEMSG
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;
		TRACE
   	sprintf(acErrMsg,"2标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG
		ret = ln_reg_modify( cb_flag , g_pub_tx.add_ind , "1" , bal );
		if( ret ) 	goto ErrExit;
		TRACE		
	}
	
   	sprintf(acErrMsg,"2标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG	
	/* 贷款表内欠息金额冲补 */
	if( pub_base_CompDblVal( in_lo_intst , 0.00 ) )
	{
		TRACE
		acm = 0.00;
		/* 计算积数  */
		strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
		
		ret = pub_base_CalAcm_Ln( g_pub_tx.erro_date , g_pub_tx.tx_date , 
			g_ln_parm.intst_days_type , acm_type , 
			in_lo_intst ,g_pub_tx.tx_amt2 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			goto ErrExit;	
		}
		sprintf(acErrMsg,"调用函数计算积数(表内积数)PASS为 [%lf]",acm );
		WRITEMSG
		
		in_acm = acm;
		
		if( cb_flag[0]=='1' )	/* 冲正 */
		{
			g_pub_tx.tx_amt2 = -in_lo_intst ;
		}
		if( cb_flag[0]=='2' )	/* 补记 */
		{
			g_pub_tx.tx_amt2 = in_lo_intst ;
		}
		
		set_zd_double( "1085" , g_pub_tx.tx_amt2 );
		set_zd_data( "0210" , "01" );	
			
		if( g_pub_tx.add_ind[0]=='1' )	/* 增(冲补----贷款挂息) */
		{
			set_zd_data( "0690" , "1" ); /* 借 */
			strcpy( in_dc_ind, "1" );
			g_pub_tx.svc_ind = 3020 ;							
		}
		if( g_pub_tx.add_ind[0]=='0' )	/* 减(冲补----贷款还息) */
		{
			set_zd_data( "0690" , "2" ); /* 贷 */;
			strcpy( in_dc_ind, "2" );
			g_pub_tx.svc_ind = 3020	;		
		}
		
		/* 冲补积数 */
		ret = ln_acm_modify( cb_flag , g_pub_tx.add_ind , "2" , acm );
		if( ret ) 	goto ErrExit;
					
		/* Pub_tx结构赋值 */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户ID */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.ac_get_ind,"111" );  /*已经读取了mdm_ac_rel, g_ln_mst*/
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */		
		strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
		/* 是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.prt_ind , "1" );	
		/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"贷款表内欠息冲正" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"贷款表内欠息补记" );
		}			
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
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;		
		TRACE
	   	sprintf(acErrMsg,"2标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG
		ret = ln_reg_modify( cb_flag , g_pub_tx.add_ind , "2" , in_lo_intst );
		if( ret ) 	goto ErrExit;
		TRACE	
	}
   	sprintf(acErrMsg,"3标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG	

	/* 贷款表外欠息金额冲补 */
	if( pub_base_CompDblVal( out_lo_intst , 0.00 ) )
	{
		TRACE
		acm = 0.00;
		/* 计算积数  */
		strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
		
		ret = pub_base_CalAcm_Ln( g_pub_tx.erro_date , g_pub_tx.tx_date , 
			g_ln_parm.intst_days_type , acm_type , 
			out_lo_intst ,g_pub_tx.tx_amt3 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			goto ErrExit;	
		}
		sprintf(acErrMsg,"调用函数计算积数(表外积数)PASS为 [%lf]",acm );
		WRITEMSG
		
		out_acm = acm;
				
		if( cb_flag[0]=='1' )	/* 冲正 */
		{
			g_pub_tx.tx_amt3 = -out_lo_intst ;
		}
		if( cb_flag[0]=='2' )	/* 补记 */
		{
			g_pub_tx.tx_amt3 = out_lo_intst ;
		}

		set_zd_double( "1086" , g_pub_tx.tx_amt3 );
		set_zd_data( "0210" , "01" );	

		/* 表外的借贷性质和本金、133相反,所以add_ind的值和本金、133相反分户才能记对 */
		char tmp_add_ind;
		if( g_pub_tx.add_ind[0]=='1' )	/* 分户增 */
		{
		 	tmp_add_ind='0';
			/*****XRB 1-2*****/
			set_zd_data( "0700" , "1" ); /* 借 */
			strcpy( out_dc_ind, "1" );
			g_pub_tx.svc_ind = 3030 ;							
		}
		if( g_pub_tx.add_ind[0]=='0' )	/* 分户减 */
		{
		 	tmp_add_ind='1';
			set_zd_data( "0700" , "2" ); /* 贷 */;
			strcpy( out_dc_ind, "2" );
			g_pub_tx.svc_ind = 3030	;		
		}
		g_pub_tx.add_ind[0]=tmp_add_ind;
		
		/* 冲补积数 */
		ret = ln_acm_modify( cb_flag , g_pub_tx.add_ind , "3" , acm );
		if( ret ) 	goto ErrExit;
					
		/* Pub_tx结构赋值 */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户ID */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.ac_get_ind,"111" );  /*已经读取了mdm_ac_rel, g_ln_mst*/
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */		
		strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
		/* 是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.prt_ind , "1" );	
		/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"贷款表外欠息冲正" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"贷款表外欠息补记" );
		}			
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
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;		
		TRACE
	   	sprintf(acErrMsg,"2标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG
		ret = ln_reg_modify( cb_flag , g_pub_tx.add_ind , "3" , out_lo_intst );
		if( ret ) 	goto ErrExit;
		TRACE
	   	sprintf(acErrMsg,"2后后标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG		
		
	}

   	sprintf(acErrMsg,"前－－cmpd_lo_intst[%lf]",cmpd_lo_intst);
    	WRITEMSG	
   	sprintf(acErrMsg,"标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG
	/* 贷款复利欠息金额冲补 */
	if( pub_base_CompDblVal( cmpd_lo_intst , 0.00 ) )
	{
		TRACE
		acm = 0.00;
		/* 计算积数  */
		strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
		
		ret = pub_base_CalAcm_Ln( g_pub_tx.erro_date , g_pub_tx.tx_date , 
			g_ln_parm.intst_days_type , acm_type , 
			cmpd_lo_intst ,g_pub_tx.tx_amt4 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			goto ErrExit;	
		}
		sprintf(acErrMsg,"调用函数计算积数(复利积数)PASS为 [%lf]",acm );
		WRITEMSG
	
		cmpd_acm = acm;
		
		if( cb_flag[0]=='1' )	/* 冲正 */
		{
			g_pub_tx.tx_amt4 = -cmpd_lo_intst ;
		}
		if( cb_flag[0]=='2' )	/* 补记 */
		{
			g_pub_tx.tx_amt4 = cmpd_lo_intst ;
		}
		
   	sprintf(acErrMsg,"中－－cmpd_lo_intst[%lf]g_pub_tx.tx_amt4=[%lf]",cmpd_lo_intst,g_pub_tx.tx_amt4);
    	WRITEMSG
		set_zd_double( "1087" , g_pub_tx.tx_amt4 );
		set_zd_data( "0210" , "01" );	

		char tmp_add_ind;
		if( g_pub_tx.add_ind[0]=='1' )	/* 分户增 */
		{
			tmp_add_ind='0';
			set_zd_data( "0710" , "1" ); /* 借 */
			strcpy( cmpd_dc_ind, "1" );
			g_pub_tx.svc_ind = 3040 ;							
		}
		if( g_pub_tx.add_ind[0]=='0' )	/* 分户减 */
		{
			tmp_add_ind='1';
			set_zd_data( "0710" , "2" ); /* 贷 */;
			strcpy( cmpd_dc_ind, "2" );
			g_pub_tx.svc_ind = 3040	;		
		}
		g_pub_tx.add_ind[0]=tmp_add_ind;

		/* 冲补积数 */
		ret = ln_acm_modify( cb_flag , g_pub_tx.add_ind , "4" , acm );
		if( ret ) 	goto ErrExit;
					
		/* Pub_tx结构赋值 */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户ID */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.ac_get_ind,"111" );  /*已经读取了mdm_ac_rel, g_ln_mst*/
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */		
		strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
		/* 是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.prt_ind , "1" );	
		/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"贷款复利欠息冲正" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"贷款复利欠息补记" );
		}			
		sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
		WRITEMSG

   	sprintf(acErrMsg,"zhongzhong－－cmpd_lo_intst[%lf]g_pub_tx.tx_amt4=[%lf]",cmpd_lo_intst,g_pub_tx.tx_amt4);
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
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;		
		TRACE
	   	sprintf(acErrMsg,"2标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG
		ret = ln_reg_modify( cb_flag , g_pub_tx.add_ind , "4" , cmpd_lo_intst );
		if( ret ) 	goto ErrExit;
		TRACE	
	}
	
	/* 贷款抵押品金额冲补 *
	if( pub_base_CompDblVal( gage_amt , 0.00 ) )
	{
		if( cb_flag[0]=='1' )	* 冲正 *
		{
			g_pub_tx.tx_amt1 = -gage_amt ;
		}
		if( cb_flag[0]=='2' )	* 补记 *
		{
			g_pub_tx.tx_amt1 = gage_amt ;
		}
		
		set_zd_double( "1088" , g_pub_tx.tx_amt1 );
		set_zd_data( "0210" , "01" );	
			
		if( g_pub_tx.add_ind[0]=='1' )	* 增(冲补----抵押品金额增加) *
		{
			set_zd_data( "0720" , "2" ); * 贷 *
			g_pub_tx.svc_ind = 3092 ;							
		}
		if( g_pub_tx.add_ind[0]=='0' )	* 减(冲补----抵押品金额减少) *
		{
			set_zd_data( "0720" , "1" ); * 借 *;
			g_pub_tx.svc_ind = 3092	;		
		}
			
		* Pub_tx结构赋值 *
		g_pub_tx.ac_id = g_ln_mst.ac_id;		* 账户ID *
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	* 账户序号 *
		strcpy ( g_pub_tx.ac_id_type , "3" );	* 账号类型 3-贷款 *
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	* 计息类型 *		
		strcpy ( g_pub_tx.ct_ind , "2" );		* 现转:1现2转 *	
		* 是否打印存折(有折无折标志):1是0否 *
		strcpy ( g_pub_tx.prt_ind , "1" );	
		* 入明细帐标志:1日间入2日终单笔入3日终汇总入 *
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"贷款抵押品金额冲补" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"贷款抵押品金额冲补" );
		}			
		sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
		WRITEMSG
	
		*** 登记交易流水 ***
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"登记交易流水错误!");
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"登记交易流水 PASS!");
		WRITEMSG
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;
			
	}
	*********************/	
	/* 打印处理 */
	/* 行内传票--本金部分 */
	if( pub_base_CompDblVal( bal , 0.00 ) )		
	{
		ret = print_LNCBBJ( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNCBBJ,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE	
	/* 行内传票--表内部分 */
	if( pub_base_CompDblVal( in_lo_intst , 0.00 ) )		
	{
		ret = print_LNCBBN( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNCBBN,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE	
	/* 行内传票--表外部分 */
	if( pub_base_CompDblVal( out_lo_intst , 0.00 ) )		
	{
		ret = print_LNCBBW( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNCBBW,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE	
	/* 行内传票--复利部分 */
	if( pub_base_CompDblVal( cmpd_lo_intst , 0.00 ) )		
	{
		ret = print_LNCBFL( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNCBFL,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE				
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款冲正补记成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款冲正补记失败！[%s]",g_pub_tx.reply);
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
	
	get_zd_data("0370" , g_pub_tx.ac_no); 		/* 贷款账号 */	
	g_pub_tx.ac_seqn = 0;				/* 账号序号 */

	get_zd_data("0670" , cb_flag); 			/* 冲补标志 */
   	sprintf(acErrMsg,"标志－－cb_flag[%s]",cb_flag);
    	WRITEMSG
    	TRACE
    	TRACE
	get_zd_data("0680" , g_pub_tx.add_ind); 	/* 增减标志 */
	get_zd_double("0390" , &bal); 			/* 贷款本金 */
	vtcp_log("aaaaaaaaaaaaaa[%lf]\n",bal);
	get_zd_double("0400" , &in_lo_intst); 		/* 表内欠息 */
	get_zd_double("0410" , &out_lo_intst); 		/* 表外欠息 */
	get_zd_double("0420" , &cmpd_lo_intst); 	/* 复利欠息 */
   	sprintf(acErrMsg,"取值－－cmpd_lo_intst[%lf]",cmpd_lo_intst);
    	WRITEMSG	
	get_zd_double("0430" , &gage_amt); 		/* 抵押品金额 */				
	get_zd_long("0440" , &g_pub_tx.erro_date); 	/* 错账日期 */
/*	get_zd_data("0650",g_pub_tx.tx_code);这是干什么???? */
	return 0;
}
/* 修改贷款登记簿 */
/**********************************************************************
* 函 数 名：  ln_reg_modify
* 函数功能：  修改贷款登记簿
* 作    者：  lance
* 完成时间：  2003年06月14日
*
* 参    数：
*     输  入：   cb_flag_tmp   char[2]        冲补标志
*                add_ind       char[2]        增减标志    
*                type     char[2]             类型：1本金、2表内欠息、3表外欠息、4复利欠息
*	         pamt	  double	      金额
*      
*     输  出:   
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int ln_reg_modify( char *cb_flag_tmp , char *add_ind , char *type , double pamt )
{
	int ret=0;
	long date_tmp;
	char cb_flag_ind[2];
	
	date_tmp = g_pub_tx.tx_date ;
	g_pub_tx.tx_date = g_pub_tx.erro_date ;

	if( cb_flag_tmp[0]=='1')	/* 冲正 */
	{
			strcpy( cb_flag_ind , "0" );/*0*/
		
	}
	if( cb_flag_tmp[0]=='2')	/* 补记 */
	{
			strcpy( cb_flag_ind , "1" );/*0*/
		
	}
	                           
	if( strcmp(cb_flag_ind , add_ind ) )	/* 补记 减 或 冲正 增*/
	{
		TRACE
		ret = pub_ln_Udp_LnReg ( g_ln_mst.ac_id , g_ln_mst.ac_seqn , type , pamt );
		if(ret) return 1;
		TRACE
	}
	if( strcmp(cb_flag_ind , add_ind )==0 )	/* 补记 增 或 冲正 减*/
	{
		TRACE
		ret = pub_ln_Ins_ln_reg( g_ln_mst , g_ln_parm , type , pamt , 0.00 ,"0" );
		if(ret) return 1;
		TRACE
	}
	
	g_pub_tx.tx_date = date_tmp ;
	return 0;
}

/* 修改贷款登记簿 */
/**********************************************************************
* 函 数 名：  ln_acm_modify
* 函数功能：  修改贷款积数
* 作    者：  lance
* 完成时间：  2003年08月11日
*
* 参    数：
*     输  入：   cb_flag_tmp   char[2]        冲补标志
*                add_ind       char[2]        增减标志    
*                type     char[2]             类型：1本金积数、2表内积数、3表外积数、4复利积数
*                acm      double	      修改的积数
*                
*     输  出:   
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int ln_acm_modify( char *cb_flag_tmp , char *add_ind , char *type , double acm )
{
	int ret=0;
	char cb_flag_ind[2];

	if( cb_flag_tmp[0]=='1')	/* 冲正 */
	{
		strcpy( cb_flag_ind , "0" );
	}
	if( cb_flag_tmp[0]=='2')	/* 补记 */
	{
		strcpy( cb_flag_ind , "1" );
	}	
	
	if( strcmp(cb_flag_ind , add_ind ) )	/* 补记 减 或 冲正 增*/
	{
		/* 积数减少 */
		TRACE
		if( type[0]=='1' )
		{
			g_ln_mst.intst_acm = g_ln_mst.intst_acm - acm;
		}else if( type[0]=='2' ){
			g_ln_mst.in_lo_acm = g_ln_mst.in_lo_acm - acm;
		}else if( type[0]=='3' ){
			g_ln_mst.out_lo_acm = g_ln_mst.out_lo_acm - acm;
		}else if( type[0]=='4' ){
			g_ln_mst.cmpd_lo_acm = g_ln_mst.cmpd_lo_acm - acm;
		}
		TRACE
	}
	if( strcmp(cb_flag_ind , add_ind )==0 )	/* 补记 增 或 冲正 减*/
	{
		/* 积数增加 */
		TRACE
		if( type[0]=='1' )
		{
			g_ln_mst.intst_acm = g_ln_mst.intst_acm + acm;
		}else if( type[0]=='2' ){
			g_ln_mst.in_lo_acm = g_ln_mst.in_lo_acm + acm;
		}else if( type[0]=='3' ){
			g_ln_mst.out_lo_acm = g_ln_mst.out_lo_acm + acm;
		}else if( type[0]=='4' ){
			g_ln_mst.cmpd_lo_acm = g_ln_mst.cmpd_lo_acm + acm;
		}
		TRACE
	}
	
	return 0;
}


/* 打印--冲补凭证--本金部分 */
int print_LNCBBJ( )
{
	char tx_type[9];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	double pri_amt=0.00,pri_acm=0.00;
	char cb_str[5];
	char dc_str[5];
	char add_str[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNCBBJ打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "贷款本金" );			
	
	/* 取科目号 */
	strcpy( data_code , "0152" );	/* 本金科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	/* 核销金额 */	
	pri_amt = bal ;		/* 贷款本金 */
	pri_acm = bal_acm;	/* 冲补积数 */
	/* 冲补标志 */	
	if( cb_flag[0]=='1' )
	{
		strcpy( cb_str , "冲正" );
		strcpy( r_str, "(红字)");
		pri_amt=-pri_amt;
	}else if( cb_flag[0]=='2' ){
		strcpy( cb_str , "补记" );	
		strcpy( r_str, "");
		pri_amt=pri_amt;
	}
	/* 借贷标志 */	
	if( bal_dc_ind[0]=='1' )
	{
		strcpy( dc_str , "借方" );
	}else if( bal_dc_ind[0]=='2' ){
		strcpy( dc_str , "贷方" );	
	}	
	/* 增减标志 */	
	if( g_pub_tx.add_ind[0]=='0' )
	{
		strcpy( add_str , "贷" );
	}else if( g_pub_tx.add_ind[0]=='1' ){
		strcpy( add_str , "借" );	
	}
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( cb_str );
	pub_base_strpack( dc_str );
				
	/*  1.金额类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本   */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.科目号、   15.增减标志  */
	/* 16.冲补标志、 17.流水号、   18.借贷标志、19.冲补积数、 20.错账日期  */
	/* 21.红字标志 */
	fprintf(fp,"LNCBBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%.2lf|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	pri_amt , 0 , 0 , 0.00 , 0.00 ,
	0.00 , "" , "" , ln_acc_hrt, add_str , 
	cb_str , g_pub_tx.trace_no , dc_str , pri_acm , g_pub_tx.erro_date,
	r_str );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}

/* 打印--冲补凭证--表内部分 */
int print_LNCBBN( )
{
	char tx_type[9];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	double pri_amt=0.00,pri_acm=0.00;
	char cb_str[5];
	char dc_str[5];
	char add_str[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNCBBN打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "表内利息" );			
	
	/* 取科目号 */
	strcpy( data_code , "0155" );	/* 表内利息科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	/* 核销金额 */	
	pri_amt = in_lo_intst ;	/* 表内金额 */
	pri_acm = in_acm;	/* 表内积数 */
	/* 冲补标志 */	
	if( cb_flag[0]=='1' )
	{
		strcpy( cb_str , "冲正" );
		strcpy( r_str, "(红字)");
		pri_amt=-pri_amt;
	}else if( cb_flag[0]=='2' ){
		strcpy( cb_str , "补记" );	
		strcpy( r_str, "");
		pri_amt=pri_amt;
	}
	/* 借贷标志 */	
	if( in_dc_ind[0]=='1' )
	{
		strcpy( dc_str , "借方" );
	}else if( in_dc_ind[0]=='2' ){
		strcpy( dc_str , "贷方" );	
	}	
	/* 增减标志 */	
	if( g_pub_tx.add_ind[0]=='0' )
	{
		strcpy( add_str ,"贷");
	}else if( g_pub_tx.add_ind[0]=='1' ){
		strcpy( add_str , "借" );	
	}
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( cb_str );
	pub_base_strpack( dc_str );
				
	/*  1.金额类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本   */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.科目号、   15.增减标志  */
	/* 16.冲补标志、 17.流水号、   18.借贷标志、19.冲补积数、 20.错账日期  */
	/* 21.红字标志 */
	fprintf(fp,"LNCBBN%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%.2lf|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	pri_amt , 0 , 0 , 0.00 , 0.00 ,
	0.00 , "" , "" , ln_acc_hrt, add_str , 
	cb_str , g_pub_tx.trace_no , dc_str , pri_acm , g_pub_tx.erro_date,
	r_str );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
/* 打印--冲补凭证--表外部分 */
int print_LNCBBW( )
{
	char tx_type[9];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	double pri_amt=0.00,pri_acm=0.00;
	char cb_str[5];
	char dc_str[5];
	char add_str[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNCBBW打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "表外利息" );			
	
	/* 取科目号 */
	strcpy( data_code , "0156" );	/* 表外利息科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	/* 核销金额 */	
	pri_amt = out_lo_intst ;	/* 表外金额 */
	pri_acm = out_acm;	/* 表外积数 */
	/* 冲补标志 */	
	if( cb_flag[0]=='1' )
	{
		strcpy( cb_str , "冲正" );
		strcpy( r_str, "(红字)");
		pri_amt=-pri_amt;
	}else if( cb_flag[0]=='2' ){
		strcpy( cb_str , "补记" );	
		strcpy( r_str, "");
		pri_amt=pri_amt;
	}
	/* 借贷标志 */	
	if( out_dc_ind[0]=='1' )
	{
		strcpy( dc_str , "借方" );
	}else if( out_dc_ind[0]=='2' ){
		strcpy( dc_str , "贷方" );	
	}	
	/* 增减标志 */	
	if( g_pub_tx.add_ind[0]=='0' )
	{
		strcpy( add_str , "贷" );
	}else if( g_pub_tx.add_ind[0]=='1' ){
		strcpy( add_str , "借" );	
	}
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( cb_str );
	pub_base_strpack( dc_str );
				
	/*  1.金额类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本   */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.科目号、   15.增减标志  */
	/* 16.冲补标志、 17.流水号、   18.借贷标志、19.冲补积数、 20.错账日期  */
	/* 21.红字标志 */
	fprintf(fp,"LNCBBW%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%.2lf|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	pri_amt , 0 , 0 , 0.00 , 0.00 ,
	0.00 , "" , "" , ln_acc_hrt, add_str , 
	cb_str , g_pub_tx.trace_no , dc_str , pri_acm , g_pub_tx.erro_date ,
	r_str );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
/* 打印--冲补凭证--复利部分 */
int print_LNCBFL( )
{
	char tx_type[9];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	double pri_amt=0.00,pri_acm=0.00;
	char cb_str[5];
	char dc_str[5];
	char add_str[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNCBFL打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "复利利息" );			
	
	/* 取科目号 */
	strcpy( data_code , "0157" );	/* 复利利息科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	/* 核销金额 */	
	pri_amt = cmpd_lo_intst ;	/* 复利金额 */
	pri_acm = cmpd_acm;	/* 复利积数 */
	/* 冲补标志 */	
	if( cb_flag[0]=='1' )
	{
		strcpy( cb_str , "冲正" );
		strcpy( r_str, "(红字)");
		pri_amt=-pri_amt;
	}else if( cb_flag[0]=='2' ){
		strcpy( cb_str , "补记" );	
		strcpy( r_str, "");
		pri_amt=pri_amt;
	}
	/* 借贷标志 */	
	if( cmpd_dc_ind[0]=='1' )
	{
		strcpy( dc_str , "借方" );
	}else if( cmpd_dc_ind[0]=='2' ){
		strcpy( dc_str , "贷方" );	
	}	
	/* 增减标志 */	
	if( g_pub_tx.add_ind[0]=='0' )
	{
		strcpy( add_str , "贷" );
	}else if( g_pub_tx.add_ind[0]=='1' ){
		strcpy( add_str , "借" );	
	}
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( cb_str );
	pub_base_strpack( dc_str );
				
	/*  1.金额类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本   */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.科目号、   15.增减标志  */
	/* 16.冲补标志、 17.流水号、   18.借贷标志、19.冲补积数、 20.错账日期  */
	/* 21.红字标志 */
	fprintf(fp,"LNCBFL%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%.2lf|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	pri_amt , 0 , 0 , 0.00 , 0.00 ,
	0.00 , "" , "" , ln_acc_hrt, add_str , 
	cb_str , g_pub_tx.trace_no , dc_str , pri_acm , g_pub_tx.erro_date ,
	r_str );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
