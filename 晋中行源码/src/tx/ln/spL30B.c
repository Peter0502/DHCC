/*************************************************
* 文 件 名: spL30B.c
* 功能描述：   
*              贷款状态转出
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

	double old_ln_amt=0.00;
	double acm_rate=0.00;
				
spL30B()  
{ 	
	int ret=0;

			
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
	/* 取入分段积数表的利率 */
	ret = pub_ln_GetRate ( g_ln_mst , g_ln_parm , &acm_rate );
	if( ret )	
	{
		sprintf(acErrMsg,"取贷款利率失败!");
		WRITEMSG
		ERR_DEAL
	}
	/* 本金积数 */	
	ret = Insert_intst_acm();
	if( ret )	
	{
		sprintf(acErrMsg,"本金积数入分段积数表错误!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L215" );
		ERR_DEAL
	}
	
	
	/* 表内积数 */
	ret = Insert_in_acm();
	if( ret )	
	{
		sprintf(acErrMsg,"表内积数入分段积数表错误!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L215" );
		ERR_DEAL
	}	

	/* 表外积数 */
	ret = Insert_out_acm();
	if( ret )	
	{
		sprintf(acErrMsg,"表外积数入分段积数表错误!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L215" );
		ERR_DEAL
	}
	/* 复利积数 */
	ret = Insert_cmpd_acm();
	if( ret )	
	{
		sprintf(acErrMsg,"复利积数入分段积数表错误!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L215" );
		ERR_DEAL
	}
	
	/* Pub_tx结构赋值 */

	g_pub_tx.ac_id = g_ln_mst.ac_id;	/* 账户序号 */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "1" );	/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );	/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
	g_pub_tx.svc_ind = 3063;		/* 贷款结转--转出 */
	strcpy ( g_pub_tx.brf,"贷款状态转出" );

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
	
    	set_zd_data("0210",g_ln_parm.cur_no);/* 对币种赋值 */
    	g_ln_mst.bal=-g_ln_mst.bal;
    	set_zd_double("1074",g_ln_mst.bal);	/* 本金 */    	
    	/** 长沙商行 不结息而是入分段积数 ****/
    	/***
    	set_zd_double("1075",g_pub_tx.tx_amt2);	* 入表内利息 *				
    	set_zd_double("1076",g_pub_tx.tx_amt3);	* 入表外利息 *
    	set_zd_double("1077",g_pub_tx.tx_amt4);	* 入复利利息 *
	*****/
	
	sprintf(acErrMsg,"L30B[%lf][%lf][%lf][%lf]",g_ln_mst.bal,g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
	WRITEMSG    	

	/* 打印程序 */
	/* 贷款结转传票--转出部分 */
	ret = print_LNBKZC( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKZC,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE	
    	    	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"按揭还款成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"按揭还款失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("0370",g_pub_tx.ac_no);		/* 帐号 */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* 账户序号 */

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}

	old_ln_amt = g_ln_mst.bal ;
	
	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *检查介质状态*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*检查贷款户状态*/
	if( ret ) {return ret;}
					

	return 0;
}
/* 本金积数 */
int Insert_intst_acm()
{
	int  ret=0;
	char intst_type[2];
	char acm_type[2];
	double acm=0.00;
		
	/* 计算本金积数 */
	strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
	acm = g_ln_mst.intst_acm ;/* 利息积数 */
	
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
		g_ln_parm.intst_days_type , acm_type , 
		g_ln_mst.bal , 0.00 , &acm , "0",0,0);
	if( ret )
	{
		sprintf(acErrMsg,"调用函数计算积数错误!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"调用函数计算积数PASS为 [%lf]",acm );
	WRITEMSG	
	/* 登记分段积数--本金积数 */		
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '2' ,
			g_ln_mst.ic_date, g_pub_tx.tx_date , acm , acm_rate, g_pub_tx.reply);
	
	if( ret )	
	{
		sprintf(acErrMsg,"贷款本金积数登记分段积数失败!");
		WRITEMSG
		return -1;
	}
	
	return 0;
}
/* 表内积数 */
int Insert_in_acm()
{
	int  ret=0;
	char intst_type[2];
	char acm_type[2];
	double acm=0.00;
			
	/* 计算表内积数 */
	strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
	acm = g_ln_mst.in_lo_acm ;/* 利息积数 */
	
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
		g_ln_parm.intst_days_type , acm_type , 
		g_ln_mst.in_lo_intst , 0.00 , &acm , "0",0,0);
	if( ret )
	{
		sprintf(acErrMsg,"调用函数计算积数错误!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"调用函数计算积数PASS为 [%lf]",acm );
	WRITEMSG	
	/* 登记分段积数--表内积数 */	
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '3' ,
			g_ln_mst.ic_date, g_pub_tx.tx_date , acm ,acm_rate, g_pub_tx.reply);
	
	if( ret )	
	{
		sprintf(acErrMsg,"贷款表内积数登记分段积数失败!");
		WRITEMSG
		return -1;
	}
	
	return 0;
}

/* 表外积数 */
int Insert_out_acm()
{
	int  ret=0;
	char intst_type[2];
	char acm_type[2];
	double acm=0.00;
			
	/* 计算表外积数 */
	strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
	acm = g_ln_mst.out_lo_acm ;/* 利息积数 */
	
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
		g_ln_parm.intst_days_type , acm_type , 
		g_ln_mst.out_lo_intst , 0.00 , &acm , "0",0,0);
	if( ret )
	{
		sprintf(acErrMsg,"调用函数计算积数错误!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"调用函数计算积数PASS为 [%lf]",acm );
	WRITEMSG	
	/* 登记分段积数--表外积数 */		
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '4' ,
			g_ln_mst.ic_date, g_pub_tx.tx_date , acm ,acm_rate, g_pub_tx.reply);
	
	if( ret )	
	{
		sprintf(acErrMsg,"贷款表外积数登记分段积数失败!");
		WRITEMSG
		return -1;
	}
	return 0;
}

/* 复利积数 */
int Insert_cmpd_acm()
{
	int  ret=0;
	char intst_type[2];
	char acm_type[2];
	double acm=0.00;
			
	/* 计算复利积数 */
	strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
	acm = g_ln_mst.cmpd_lo_acm ;/* 利息积数 */
	
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
		g_ln_parm.intst_days_type , acm_type , 
		g_ln_mst.cmpd_lo_intst , 0.00 , &acm , "0",0,0);
	if( ret )
	{
		sprintf(acErrMsg,"调用函数计算积数错误!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"调用函数计算积数PASS为 [%lf]",acm );
	WRITEMSG	
	/* 登记分段积数--复利积数 */		
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '5' ,
			g_ln_mst.ic_date, g_pub_tx.tx_date , acm ,acm_rate, g_pub_tx.reply);
	
	if( ret )	
	{
		sprintf(acErrMsg,"贷款复利积数登记分段积数失败!");
		WRITEMSG
		return -1;
	}
	return 0;
}


	
/* 打印--贷款结转传票--转出部分 */
int print_LNBKZC( )
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
		sprintf(acErrMsg,"LNBKZC打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "转出" );	
	
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
	tol_pay = g_ln_mst.ttl_prvd_amt - old_ln_amt;
			
	/* 尚欠本金 */		
	tol_need_pay = old_ln_amt;

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
	fprintf(fp,"LNBKZC%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	-old_ln_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , 
	g_pub_tx.trace_no , dc_sts , ln_sts ,"(红字)");	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
