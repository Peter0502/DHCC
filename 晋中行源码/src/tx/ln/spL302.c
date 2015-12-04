/*************************************************
* 文 件 名: spL302.c
* 功能描述：   
*              贷款状态转入
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

	char typ[2];
		
spL302()  
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
	/* 检查贷款是否到期 */
	if ( typ[0]=='*' || typ[0]=='9' )
	{
		sprintf( acErrMsg, "修改的状态不符合要求[%s]",typ);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L055");
		ERR_DEAL
	}else{				
		/* 只允许正向修改 modified by liuxuan 20071012*/
		if ( atoi(g_ln_mst.ac_sts)!=5){		/* 5是逾期90天外,所以得特别处理 */
			if ( atoi(g_ln_mst.ac_sts) >  atoi(typ) )
			{
				sprintf( acErrMsg, "修改的状态不符合要求,只能正向修改[%s]",typ);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L112");
				ERR_DEAL				
			}
		}
		else if ( atoi(g_ln_mst.ac_sts)==5 && atoi(typ)<3 ){
				sprintf( acErrMsg, "修改的状态不符合要求,只能正向修改[%s]",typ);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L112");
				ERR_DEAL				
		}
	}
	
	/* 当贷款状态未做改变时,提示错误 */
	if( strcmp(g_ln_mst.ac_sts , typ)==0 )
	{
		sprintf( acErrMsg, "贷款状态未做任何改变[%s]",typ);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L214");
		ERR_DEAL
	}		
		
	if ( g_ln_mst.mtr_date >= g_pub_tx.tx_date  )
	{	
		if ( typ[0]!='4' &&  typ[0]!='1' )
		{
			sprintf( acErrMsg, "贷款未到期,修改的状态不符合要求[%s]",typ);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L054");
			ERR_DEAL			
		}
	}
	

	/* 修改主文件状态 */
	
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d ", g_ln_mst.ac_id , g_pub_tx.ac_seqn );
	if (ret)
	{
		sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd 游标出错[%d]",ret);
		WRITEMSG
		ERR_DEAL
	}
	
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
			
	ret = Ln_mst_Fet_Upd( &g_ln_mst , g_pub_tx.reply );
	if (ret==100)
	{
		sprintf( acErrMsg,"贷款主文件中无相应记录[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L014");
		ERR_DEAL
	}else if( ret )
 		{
	 		sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply ,"D103" );
	 		ERR_DEAL
 		}
	
	strcpy( g_ln_mst.ac_sts , typ );
	
	ret = Ln_mst_Upd_Upd( g_ln_mst , g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_mst_Upd_Upd 出错[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
		ERR_DEAL
	}
	
	Ln_mst_Clo_Upd( );
	sprintf(acErrMsg,"更新贷款主文件表中状态标志 PASS");
	WRITEMSG		
	
	/* Pub_tx结构赋值 */

	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
	strcpy ( g_pub_tx.brf,"贷款状态转入" );
	g_pub_tx.svc_ind = 3060;			/* 贷款结转 */
	sprintf(acErrMsg,"Pub_tx结构赋值[%d]",g_pub_tx.svc_ind);
	WRITEMSG
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
    	set_zd_double("107F",g_ln_mst.bal);	/* 本金 */
	sprintf(acErrMsg,"L302[%lf]",g_ln_mst.bal);
	WRITEMSG  
	   					
	/* 打印程序 */
	/* 贷款结转传票--转入部分 */
	ret = print_LNBKZR( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKZR,失败请检查!" );
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
	get_zd_data("0700",typ);			/* 转换状态 */

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *检查介质状态*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*检查贷款户状态*/
	if( ret ) {return ret;}
					

	return 0;
}

/* 打印--贷款结转传票--转入部分 */
int print_LNBKZR( )
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
		sprintf(acErrMsg,"LNBKZR打开文件失败!");
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
	fprintf(fp,"LNBKZR%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	g_ln_mst.bal , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , 
	g_pub_tx.trace_no , dc_sts , ln_sts ,"");	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
