/*************************************************
* 文 件 名: spL204.c
* 功能描述：   
*              按揭贷款提前还本
*
* 作    者:    lance
* 完成日期：   2003年01月21日
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
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"
#include "ln_mst_hst_c.h"

static struct ln_lo_c		ln_lo;
static struct ln_pay_pln_c	ln_pay_pln;
char ct_flag[2];
double print_tol_amt=0.00;
	
spL204()  
{ 	
	int ret=0;
	int pay_clear=0;
	long dqr=0;
	char clear_sts[2];	

	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_get_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
	
	/** 处理流程 **/
	
	/* 查询按揭欠款表 */
	ret=sql_count("ln_lo","ac_id=%ld and ac_seqn=%d  and pay_type='0'" , 
								g_pub_tx.ac_id,g_pub_tx.ac_seqn );
	if (ret)
	{
		sprintf( acErrMsg, "该按揭贷款存在欠款,不能提前还本[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L047");
		ERR_DEAL
	}		
	
	/* 检查当期是否以还 */

	ret=Ln_pay_pln_Sel(g_pub_tx.reply, &ln_pay_pln, "ac_id=%ld and ac_seqn=%d"
					 , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款还款计划表信息中无此信息 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L043" );
		ERR_DEAL
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"取贷款还款计划表信息异常erro code=[%s]",ret);
		WRITEMSG
    	ERR_DEAL
  	}	
  if(ln_pay_pln.curr_cnt==1)
  {
  		sprintf( acErrMsg, "按揭贷款当期未还,不能提前还本");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L048");
		ERR_DEAL
  }	
  else
  {
  	   long tmp_date=0;
  	   if(g_pub_tx.tx_date%100<g_ln_parm.fst_pay_day)
  	   {
  	   	if((g_pub_tx.tx_date/100*100+g_ln_parm.fst_pay_day)==ln_pay_pln.beg_date)
  	   	{
  				sprintf( acErrMsg, "按揭贷款当期未还,不能提前还本");
		    	WRITEMSG
		    	strcpy(g_pub_tx.reply,"L048");
		    	ERR_DEAL
  	   	}
  	   }
  	   else
  	   {
  	   	ret=pub_base_deadlineM(g_pub_tx.tx_date,1,&tmp_date);
  	   	if((tmp_date/100*100+g_ln_parm.fst_pay_day)==ln_pay_pln.beg_date)
  	   	{
  				sprintf( acErrMsg, "按揭贷款当期未还,不能提前还本");
		    	WRITEMSG
		    	strcpy(g_pub_tx.reply,"L048");
		    	ERR_DEAL
  	   	}
  	  }
  }	
  /*
	pub_base_deadlineM( ln_pay_pln.end_date, -1, &dqr);
	sprintf( acErrMsg, "到期日【%ld】end_date[%ld]",dqr,ln_pay_pln.end_date);
	WRITEMSG
  	if( g_pub_tx.tx_date > dqr )
  	{	
  		sprintf( acErrMsg, "按揭贷款当期未还,不能提前还本");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L048");
		ERR_DEAL
	}	
	*/
	
	/* 登记按揭欠款撤销登记簿 */
	ret = Ins_ln_pay_pln_rol( ln_pay_pln );
	if( ret )
	{
 		sprintf( acErrMsg,"调用登记按揭还款计划撤销登记簿函数异常出错!");
 		WRITEMSG
 		goto ErrExit;
	}
		
	/* 扣本金 */
	g_pub_tx.tx_amt1 = g_pub_tx.tmp_amt1 ;
	
 	sprintf( acErrMsg,"前余额bal=[%lf]扣本金=[%lf]",g_ln_mst.bal,g_pub_tx.tx_amt1);
 	WRITEMSG
 	
 	/* 检查是否还清 */
 	pay_clear = pub_base_CompDblVal( g_ln_mst.bal-g_pub_tx.tx_amt1 , 0.00 );
 	if(pay_clear<0)
 	{
  		sprintf( acErrMsg,"还款金额过大,大于贷款余额!");
 		WRITEMSG
 		strcpy( g_pub_tx.reply , "L226");
 		goto ErrExit;
 	}else if(pay_clear>0){
		TRACE
		/***** 生成新的还款计划 *********/
		ret = pub_ln_Chg_PayPlan(g_ln_mst, g_ln_parm, g_ln_mst.rate, 
				g_ln_mst.bal-g_pub_tx.tx_amt1, g_pub_tx.tx_date, g_ln_mst.mtr_date);
		if( ret ) goto ErrExit;
		
	 	sprintf( acErrMsg,"中余额bal=[%lf]!扣本金=[%lf]",g_ln_mst.bal,g_pub_tx.tx_amt1);
	 	WRITEMSG			
		/***
		g_ln_mst.bal = g_ln_mst.bal + g_pub_tx.tx_amt1;
		****/
	 	sprintf( acErrMsg,"后余额bal=[%lf]扣本金=[%lf]",g_ln_mst.bal,g_pub_tx.tx_amt1);
	 	WRITEMSG
	 }else if(pay_clear==0){
	 	TRACE
	 	/* 更新还款计划中标志为已还清 */
		ret = sql_execute( "UPDATE ln_pay_pln SET pay_ind='1'  WHERE ac_id=%ld and ac_seqn=%d " , 
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn );
		if( ret ) goto ErrExit;	
		/*修改贷款本金余额*/
		g_ln_mst.bal=0.00;
		ret = sql_execute( "UPDATE ln_mst SET bal=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn );
		if( ret ) goto ErrExit;
	}
 		
	/* 登记贷款登记簿 */
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"1",
											g_pub_tx.tmp_amt1,0.00,"1",1 );
	if( ret ) 	goto ErrExit;	
	
	/* Pub_tx结构赋值 */
	sprintf(acErrMsg,"g_pub_tx.tx_amt1[%lf]bal[%lf]",g_pub_tx.tx_amt1
																,g_ln_mst.bal);
	WRITEMSG	
	set_zd_data( "0210" , g_ln_parm.cur_no );
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
	g_pub_tx.svc_ind = 3018 ;
	strcpy ( g_pub_tx.brf,"按揭提前还本" );

	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
	
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
	
	/* 检查是否还清 */
	ret = pub_ln_chk_clear(g_ln_mst,g_ln_parm,clear_sts);
	if(ret)	goto ErrExit;
	if( pub_base_CompDblVal( g_ln_mst.gage_amt , 0.00 ) )
	{			
		set_zd_data("109K",clear_sts);
		/* 贷款有抵押品 */
	}else{
		set_zd_data("109K","*");
		/* 贷款有抵押品 */
	}	
	
	/* 转账进账单,现金缴款单 */
	ret = print_LNDF( );
	if(ret)		
	{
		sprintf(acErrMsg,"print_LNDF,失败请检查!" );
		WRITEMSG		
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE
	
	/* 客户回单--按揭提前还本回单 */
	ret = print_LNHDAJTQ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHDAJTQ,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE
	
	/* 行内传票--本金部分 */
	ret = print_LNBKAJBJ( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJBJ,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE

	pub_mob_sendmail_ln("按揭贷款提前归还本金",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);
	
	if( ct_flag[0]=='1' )	/** 现金 **/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*转账*/
		goto ErrExit2;
    else if (ct_flag[0] == '3')		/*提出*/
        goto ErrExit3;

ErrExit:
    sprintf(acErrMsg,"按揭提前还本失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"按揭提前还本成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"按揭提前还本成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"按揭提前还本成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	memset(&ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;
	struct ln_mst_hst_c 	S_ln_mst_hst;
	memset( &S_ln_mst_hst,0x00 , sizeof( struct ln_mst_hst_c));
	
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("0370",g_pub_tx.ac_no);		/* 帐号 */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* 账户序号 */
	get_zd_double("0400",&g_pub_tx.tmp_amt1);	/* 还款金额 */
	
	print_tol_amt = g_pub_tx.tmp_amt1;
	
	get_zd_data("0230",g_pub_tx.note_type);		/* 凭证种类 */
	get_zd_data("0580",g_pub_tx.beg_note_no);	/* 起始凭证号码 */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );

	get_zd_data("0660",g_pub_tx.ct_ind);			/* 现转标志 */
	get_zd_data("0660",ct_flag);							/* 现转标志 */	
	sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
	WRITEMSG
	
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *检查介质状态*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*检查贷款户状态*/
	if( ret ) {return ret;}
					
	if( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
	{
		sprintf(acErrMsg,"该贷款非按揭贷款![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L042" );
		return 1;
	}
	/* 限制旧数据中转行账号做业务 */
	ret= Ln_mst_hst_Sel( g_pub_tx.reply, &S_ln_mst_hst, "ac_id=%ld and tx_code='2498'",g_ln_mst.ac_id );
	if( ret!=100 )
	{
		sprintf(acErrMsg,"限制旧数据中转行账号做业务[%ld]",g_ln_mst.ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L043" );
		return ret;
	}
	return 0;
}

/* 打印--转账进账单,现金缴款单 */
int  print_LNDF()
{ 
    	char tx_type[9];

    	char filename[51],tmpname[101];
    	FILE *fp;
    
    	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNDF打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
	
	strcpy( tx_type , "按揭还本" );	
	
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( tx_type );
			
	/* 1.贷款账号、	     2.户名、	      3.类型、    4.总还款金额 	*/
	fprintf(fp,"LNDF%s|%s|%s|%.2lf|\n",g_pub_tx.ac_no , g_ln_mst.name , tx_type , print_tol_amt );	
	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
		
	return 0;
}

/* 打印--客户回单--按揭提前还本回单 */
int  print_LNHDAJTQ( )	
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	long end_date =0;
	char exp_ind[5];
	  
    	char filename[51],tmpname[101];
    	FILE *fp;
 	TRACE   		
	TRACE
	pub_base_AllDwnFilName( tmpname );
	TRACE
	TRACE
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDAJTQ打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
  	TRACE      	
	/* 赋值部分 */
	strcpy( tx_type , "按揭提前还本" );		

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
	sprintf( fine_rate , "%8.5lf‰" , g_ln_mst.fine_rate);
	TRACE		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
	TRACE			
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.展期标志、 15.流水号	*/
	fprintf(fp,"LNHDAJTQ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate ,exp_ind , g_pub_tx.trace_no );	
	TRACE	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}
/* 打印--行内传票--按揭提前还本部分 */
int print_LNBKAJBJ( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	char exp_ind[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJBJ打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "按揭提前还本" );	
	
	/* 取科目号 */
	strcpy( data_code , "0152" );	/* 本金科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	
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
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.正常利率、 13.逾期利率、14.科目号、   15.展期标志  */
	/* 16.流水号 */	
	fprintf(fp,"LNBKAJBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind,
	g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
