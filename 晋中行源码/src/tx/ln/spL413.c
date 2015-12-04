/*************************************************
* 文 件 名: spL413.c
* 功能描述：   
*              委托贷款还息
*
* 作    者:    zgf
* 完成日期：   2013年05月28日
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

int 	ret=0;
long	ln_ic_date=0;	/* 贷款上次交易日期 */ 
long  pri_end_date=0;	/* 终止日期 */
long	need_pay_beg_day=0;
char flag_typ[2];	/* 本金类型标志 */
spL413()  
{ 	
	int ret =0;
	char cif_type[2];
	char clear_sts[2];
	char print_str[31];

			
	struct cif_basic_inf_c cif_basic_inf;
	
	memset( &cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c) );
			
	TRACE
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	
		ERR_DEAL;
		
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_get_struct() )
		ERR_DEAL;
		
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
			
	/* 客户回单--汇总回单 */	
	ret = print_LNHDHZ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHDHZ,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE

OkExit:
    sprintf(acErrMsg,"委托贷款还息成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
        sprintf(acErrMsg,"委托贷款还息失败![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	return 0;
}
int data_get_struct()
{    
	int ret=0;
	double tmp_tx_amt=0.00;

	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	/* 委托贷款还息贷款信息记录 */
	ret = pay_acm_intst();
	if( ret )
	{
		sprintf(acErrMsg,"调用pay_acm_intst函数异常出错,请与中心机房联系!" );
		WRITEMSG
		if( strlen(g_pub_tx.reply)==0 )
	  {
	     strcpy( g_pub_tx.reply,"L193" );
	  }
		return 1;
	}
	 
  /*代扣代缴税记内部帐D413*/ 
  /*    
  strcpy(g_pub_tx.ac_no,"26505"); 
  get_zd_double("0420",&g_pub_tx.tx_amt1);
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.brf,"委托还息代缴税");
  
	strcpy( g_pub_tx.ac_id_type,"9" ); 
  strcpy( g_pub_tx.ac_get_ind,"00" ); 
  strcpy( g_pub_tx.ac_wrk_ind,"000" ); 
  strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
  strcpy(g_pub_tx.add_ind,"1");
  strcpy( g_pub_tx.prt_ind,"0" ); 
  g_pub_tx.svc_ind=9001;  
  strcpy(g_pub_tx.hst_ind,"1"); 
	*/
	/***金额判断，为0不进行还息操作**/  	
	/*
  ret = pay_in_lo_intst( &g_pub_tx.tx_amt1);
	if( ret )
	{
		sprintf(acErrMsg,"调用金额判断函数异常出错,请与中心机房联系!" );
		WRITEMSG
		if( strlen(g_pub_tx.reply)==0 )
	  {
	     strcpy( g_pub_tx.reply,"L193" );
	  }
		return 1;
	}

  ret=pub_acct_trance();
  if( ret )
  {
      sprintf(acErrMsg,"调用代扣代缴税L413主控失败!");
      WRITEMSG
      return 1;
  } 
	sprintf(acErrMsg,"代扣代缴税L413,交易记帐处理 PASS!");
	WRITEMSG 
*/	    
	/*取款交易D003赋值*/
	get_zd_data("1021",g_pub_tx.ac_no);  /*账号 */
	sprintf(acErrMsg,"委托贷款还息取款账号![%s]",g_pub_tx.ac_no);
	WRITEMSG											
	get_zd_double("102F",&g_pub_tx.tx_amt1);  /*交易金额 */
	sprintf(acErrMsg,"委托贷款还息取款金额![%.2lf]",g_pub_tx.tx_amt1);
	WRITEMSG
	get_zd_data("1023",g_pub_tx.note_type);  /*凭证种类 */
	sprintf(acErrMsg,"取款账号凭证种类![%s]",g_pub_tx.note_type);
	WRITEMSG
	get_zd_data("1024",g_pub_tx.beg_note_no);  /*凭证号码 */
	sprintf(acErrMsg,"取款账号凭证号码![%s]",g_pub_tx.beg_note_no);
	WRITEMSG
	/*对私证件类型赋值*/
	get_zd_data("102F",g_pub_tx.id_type);  /*证件类型 */
	sprintf(acErrMsg,"证件类型![%s]",g_pub_tx.id_type);
	WRITEMSG
	get_zd_data("102A",g_pub_tx.id_no);  /*证件号码 */
	sprintf(acErrMsg,"证件号码![%s]",g_pub_tx.id_no);
	WRITEMSG
	
	set_zd_data( "102J","01" ); /*币种*/
	set_zd_data( "102K","2" ); /* 现转标志*/
	set_zd_data("102B","Y"); 
	set_zd_data("102Y","委托贷款利息归还");
	
	/*存款交易D099赋值*/
	get_zd_data("1011",g_pub_tx.ac_no);  /*账号 */
	sprintf(acErrMsg,"委托贷款还息存款账号![%s]",g_pub_tx.ac_no);
	WRITEMSG
	get_zd_double("1013",&g_pub_tx.tx_amt1);  /*金额 */
	sprintf(acErrMsg,"委托贷款还息存款金额![%.2lf]",g_pub_tx.tx_amt1);
	WRITEMSG
	set_zd_int("1012",1);
	set_zd_data("101A","01");
	set_zd_data("101B","2");
	set_zd_data("101C","031");				/***摘要***/	
	set_zd_data("1016","委托还息");				/***摘要***/	
	
	sprintf(acErrMsg,"委托贷款还息1111111111111 pass");
	WRITEMSG	

	return 0;
}

/* 委托贷款还息金额判断，写贷款登记簿 */
int pay_in_lo_intst( double *amt )	
{
	double acm_intst=0.00;
	int flag;			/* 表内/表外标志 */
	
	double bal=0.00;
	
	bal = *amt;

	sprintf(acErrMsg,"进入委托贷款还息---金额[%lf]",bal);
	WRITEMSG
	
	ret = pub_base_CompDblVal( bal , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"委托贷款还息金额为零，不用归还." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L069" );
	        return 1;
	}	
	return 0;
	
}

/* 委托贷款还息贷款信息记录 */
int pay_acm_intst()	
{

	double acm_intst=0.00;
	int flag;			/* 表内/表外标志 */
	
	double bal=0.00;
	get_zd_data("1081",g_pub_tx.ac_no);  /*委托贷款账号 */
	get_zd_double("102F",&g_pub_tx.tx_amt1);
	bal = g_pub_tx.tx_amt1;

	sprintf(acErrMsg,"进入委托贷款还息---金额[%lf]",bal);
	WRITEMSG
	
	ret = pub_base_CompDblVal( bal , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"委托贷款还息金额为零，不用归还." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L069" );
	        return 1;
	}
	
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , g_pub_tx.ac_no );				
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		return 1;
	} else if( ret !=0 )
	{
  	sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    return 1;
  }
	sprintf(acErrMsg,"取出的账户ID为[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , " ac_id=%ld " , g_mdm_ac_rel.ac_id );			
	if( ret==100 )
	{
			sprintf( acErrMsg,"贷款主文件表中无此委托贷款记录 erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			return 1;
	} else if( ret !=0 )
	{
    		sprintf( acErrMsg,"取贷款主文件异常 erro code=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    		return 1;
  }	
  sprintf(acErrMsg,"取贷款主文件PASS");
	WRITEMSG 
	
	
	/* 贷款本金利息类型判断 */
	flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );
	sprintf( flag_typ , "%d" , flag+1 ); 
	
	sprintf(acErrMsg,"ly贷款本金利息类型[%s]",flag_typ );	
	WRITEMSG	
	
	get_zd_data("1081",g_pub_tx.ac_no);  /*委托贷款账号 */
  sprintf(acErrMsg,"委托贷款还息贷款账号![%s]",g_pub_tx.ac_no);
	WRITEMSG
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.brf,"委托贷款利息归还");
  
	strcpy( g_pub_tx.ac_id_type,"3" ); /*账户类型为内部*/
  strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
  strcpy(g_pub_tx.note_type,"299");/*贷方标志*/
  strcpy( g_pub_tx.beg_note_no,"null" ); /*不登折*/
  strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
  strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
  strcpy ( g_pub_tx.ac_wrk_ind , "0000" );/* 零金额不记流水,明细*/		
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
	g_pub_tx.svc_ind=3011;
	sprintf(acErrMsg,"记账成功后，更新贷款表中积数字段及最近交易日期字段 START!"); 
	WRITEMSG  
	/*写贷款登记簿*/	
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",bal,g_ln_mst.intst_acm,"0",2 );
	if( ret ) 
		return 1;
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		return 1;
	}	
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG	
	
	return 0;
}

/* 打印--客户回单--利息和复利汇总回单 */
int  print_LNHDHZ( )	
{
	TRACE
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	TRACE
    	char filename[51],tmpname[101];
    	long start_day=0;
    	char start_day_str[15];
    	double tol_amt=0.00;
     	double lx_tol=0.00;
    	double fl_tol=0.00;     	   	
    	 
    	FILE *fp;
 	TRACE   		
	pub_base_AllDwnFilName( tmpname );
	TRACE
	fp=fopen(tmpname,"a");
    if( fp == NULL )
    {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDLX打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
    }
  	TRACE
  	      	
	/* 赋值部分 */
	strcpy( tx_type , "委托贷款还息" );			

	/* 确定起始日期 */
	sprintf(acErrMsg,"ly确定起始日期:[%ld]!",need_pay_beg_day);
	WRITEMSG	
	start_day = need_pay_beg_day;
	if( start_day )
	{
		sprintf( start_day_str, "%4d-%02d-%02d", start_day/10000, start_day%10000/100, start_day%100 );
	}else{
		strcpy( start_day_str, "");
	}	

	sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf‰" , g_ln_mst.over_rate);
	TRACE		
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	TRACE			
	/*  1.交易类型、  3.借据号、  5.产品名称 	*/
	/*  7.起始日期、  10.累计还款     */
	/*  12.正常利率、 13.逾期利率、*/
	fprintf(fp,"LNHDHZ%s|%s|%s|%ld|%.2lf|%s|%s|\n",
	tx_type , g_ln_mst.pact_no ,  g_ln_parm.title ,
  g_ln_mst.ic_date  , tol_pay ,
  rate , over_rate );	
	TRACE
vtcp_log("怎么不显示呢？");	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}	