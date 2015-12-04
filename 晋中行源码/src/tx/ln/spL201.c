/*************************************************
* 文 件 名: spL201.c
* 功能描述：   
*              贷款还本
*
* 作    者:    lance
* 完成日期：   2003年01月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define EXTERN
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "public.h"
#include "ln_reg_c.h"
#include "mo_infrm_c.h"

static struct ln_reg_c		ln_reg;	
static struct mo_infrm_c	mo_infrm;

	char repay_ind[2];
	char ct_flag[2];
	double print_tol_amt = 0.00;
		
spL201()  
{ 	
	int ret=0;
	double acm_intst=0.0;	/* 本金积数利息 */
	int flag;			/* 表内/表外标志 */
	char flag_typ[2];
	int day_flag=0;		/* 提前还款标志 */
	long note_day;		/* 通知日期 */
	char typ[2];
	double vbjlx,vbnlx,vbylx,vfllx;
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
		
	/* 判断是否可做还本交易 */
	/* del in 长沙商行
	if( repay_ind[0]=='L' )
	{
			vbjlx=vbnlx=vbylx=vfllx=0.00;
			TRACE
			
			* 不检查本金利息 
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"0",&vbjlx,1 );
			if( ret ) { ERR_DEAL }
			**	
			
			TRACE		
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"1",&vbnlx,1 );
			if( ret ) { ERR_DEAL }	
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"2",&vbylx,1 );
			if( ret ) { ERR_DEAL }	
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"3",&vfllx,1 );
			if( ret ) { ERR_DEAL }		
			
		  ret = pub_base_CompDblVal(vbjlx+vbnlx+vbylx+vfllx+g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst, 0.00 );
		  if( ret>0 )
	   	  {
				sprintf(acErrMsg,"贷款有欠息未还清,不能做还本交易!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"L117" );
				goto ErrExit;	   
		  } 	
	}
	*******************/
	
	if( g_pub_tx.tmp_ind[0]=='2' ) /* 利随本清 */
	{
		/* 本金积数计算利息 */
		ret = pub_ln_CalIntst ( g_ln_mst , g_ln_parm , "0" , &acm_intst , 0 );
		if( ret ) goto ErrExit;
			
		sprintf(acErrMsg,"金额[%.2lf] [%.2lf]",g_pub_tx.tmp_amt1,acm_intst );
		WRITEMSG

		/* 还款金额是否大于本金利息金额 */
		ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1, acm_intst );
		if( ret<0 )
		{
			sprintf(acErrMsg,"金额过小[%.2lf] [%.2lf]",g_pub_tx.tmp_amt1 ,
								acm_intst );
			WRITEMSG
			strcpy( g_pub_tx.reply,"L040" );
			goto ErrExit;
		}
	
		/* 登记贷款登记簿 */
		/* 贷款本金利息类型判断 */
		flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );
		sprintf( flag_typ , "%d" , flag ); 
		
		ret = pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,flag_typ,acm_intst,g_ln_mst.intst_acm,"0",2);
		if( ret ) goto ErrExit;
		
		g_pub_tx.tx_amt1= acm_intst;	/* 可归还利息金额 */	

		strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
		g_pub_tx.svc_ind=3011;
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/
		strcpy( g_pub_tx.brf,"本金利税本清" );

		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
		}

		sprintf(acErrMsg,"登记贷款登记簿成功 PASS!");
		WRITEMSG

		g_pub_tx.tmp_amt1 -= g_pub_tx.tx_amt1;
	}

	/* 金额必须小于贷款本金余额和本金利息之和 */
	ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,g_ln_mst.bal);
	if( ret>0 ) 
	{
		sprintf(acErrMsg,"还款金额大于贷款本金余额[%.2lf] [%.2lf] ac_id[%ld]",g_pub_tx.tmp_amt1,
			g_ln_mst.bal ,g_ln_mst.ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L039" );
		goto ErrExit;
	}
	sprintf(acErrMsg,"金额必须小于贷款本金余额 PASS!");
	WRITEMSG

	if( pub_base_CompDblVal(g_pub_tx.tmp_amt1,0.00)>0 )
	{
		/* 提前还款检查 */
		if ( g_ln_mst.mtr_date > g_pub_tx.tx_date )
		{
			/* 检查提前还款限额 */ 	
			if ( pub_base_CompDblVal(g_ln_parm.fdrq_lmt_amt,0.00)>0 )
			{
			  if ( pub_base_CompDblVal(g_ln_parm.fdrq_lmt_amt,g_ln_mst.bal)<0 )
			  {
					ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,
												g_ln_parm.fdrq_lmt_amt );
					if( ret<0 )
					{
						sprintf(acErrMsg,"金额小于提前还款限额[%.2lf] [%.2lf]" ,
										g_pub_tx.tmp_amt1,g_ln_parm.fdrq_lmt_amt );
						WRITEMSG
						strcpy( g_pub_tx.reply,"L040" );
						goto ErrExit;
					}
			  }
			}
			sprintf(acErrMsg,"检查提前还款限额 PASS!");
			WRITEMSG
					sprintf(acErrMsg,"通知天数[%s]",
									g_ln_parm.fdrp_inf_days );			
			sprintf(acErrMsg,"检查提前还款限额 PASS!");
			WRITEMSG
						
			/* 提前还款提前通知天数 */
			if ( g_ln_parm.fdrp_inf_days )
			{
				/* 取通知天数 */
				note_day = pub_base_daynumSUB(g_pub_tx.tx_date,
												g_ln_parm.fdrp_inf_days);
				
				ret = Mo_infrm_Dec_Upd(g_pub_tx.reply, 
						" ac_id=%ld and ac_seqn=%d and infrm_date=%ld and \
							prm_draw_date=%ld and sts='0' " , g_ln_mst.ac_id,
							g_ln_mst.ac_seqn,note_day,g_pub_tx.tx_date );
				if ( ret )
				{
					sprintf( acErrMsg, "执行Mo_infrm_Dec_Upd错!ret=[%d]",ret );
					WRITEMSG
					return(-1);
				}
				ret = Mo_infrm_Fet_Upd( &mo_infrm, g_pub_tx.reply );
				if( ret==100)
				{
					sprintf(acErrMsg,"提前还款需通知,通知登记薄中无相应记录"
								,g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"O008");
					ERR_DEAL;
				}else if ( ret )
				{
					sprintf(acErrMsg,"取通知登记薄信息异常",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"O009");
					ERR_DEAL;
				}
				
				/* 比较通知的还款的金额和现在要还的金额 */	
				ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,
											mo_infrm.prm_draw_amt );
				if ( !ret )
				{
					sprintf(acErrMsg,"还款金额与通知金额不符",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"O010");
					ERR_DEAL;				
				}	
							
				strcpy( mo_infrm.sts , "2" );			/* 修改状态:按期支取 */
				mo_infrm.occ_date = g_pub_tx.tx_date;	/* 实际发生日期 */
				mo_infrm.fct_wd_amt = g_pub_tx.tmp_amt1;/* 实际取款金额 */				
				ret = Mo_infrm_Upd_Upd( mo_infrm, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg,"Mo_infrm_Upd_Upd 出错[%d]",ret);
					WRITEMSG
					return 1;
				}		
	
				Mo_infrm_Clo_Upd( );
				sprintf(acErrMsg,"更新通知登记薄 PASS");
				WRITEMSG
			}
			sprintf(acErrMsg,"提前还款提前通知天数 PASS!");
			WRITEMSG
			
			/* ???是否进行收费检查和处理 */
		}
		
		g_pub_tx.tx_amt1 = g_pub_tx.tmp_amt1 ;
		
		sprintf(acErrMsg,"判断是否是提前还款 PASS!");
		WRITEMSG				

		/* 更新贷款登记簿 */
		strcpy ( typ ,"1");
		ret = pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,typ,g_pub_tx.tx_amt1,0.00,"0",1);
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"更新贷款登记簿 PASS!");
		WRITEMSG

		/* Pub_tx结构赋值 */
		strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
		strcpy( g_pub_tx.intst_type,"1" );	/* 计息类型 */	
		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
		g_pub_tx.svc_ind=3010;
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/
		strcpy( g_pub_tx.brf,"贷款还本金" );

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
	}
	
	/* 输出 */
	/* 检查是否还清 */
	ret = pub_ln_chk_clear(g_ln_mst,g_ln_parm,clear_sts);
	if(ret)	goto ErrExit;
	if( pub_base_CompDblVal( g_ln_mst.gage_amt , 0.00 ) )
	{			
		set_zd_data("107J",clear_sts);
		/* 贷款有抵押品 */
	}else{
		set_zd_data("107J","*");
		/* 贷款有抵押品 */
	}	
	
	set_zd_double("107E",acm_intst); /*还息金额*/
	set_zd_double("107F",g_pub_tx.tx_amt1); /*还本金额*/
	g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+acm_intst;
	set_zd_double("107H",g_pub_tx.tx_amt1);	/*合计*/
	set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);
	
	sprintf(acErrMsg,"输出 PASS!g_pub_tx.tx_amt1[%lf],acm_intst[%lf]",
							g_pub_tx.tx_amt1,acm_intst);
	WRITEMSG	


	set_zd_data("0210",g_ln_parm.cur_no);/* 对币种赋值 */
	
	/* 打印程序 */
	
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
	
	/* 客户回单--本金回单 */
	ret = print_LNHDBJ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHDBJ,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE
	
	/* 行内传票--本金部分 */
	ret = print_LNBKBJ( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKBJ,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE	

/*pub_mob_sendmail_ln("本金归还",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);	*/	
pub_mob_sendmail_ln("本金归还",g_pub_tx.tx_code,g_ln_mst.pact_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);			

	if( ct_flag[0]=='1' )	/** 现金 **/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*转账*/
		goto ErrExit2;
	else if (ct_flag[0] == '3')		/*提出*/
        goto ErrExit3;
    else if (ct_flag[0] == '9')	/* 贴现还款 */
        goto GoodExit;
    else if (ct_flag[0] == '8')	/* 扣划 */
        goto ErrExit8;       
ErrExit:
	strcpy(g_pub_tx.reply,"L153");
    sprintf(acErrMsg,"贷款还本失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"贷款还本成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"贷款还本成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"贷款还本成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
ErrExit8:
    strcpy(g_pub_tx.reply,"0008");
    sprintf(acErrMsg,"贷款还本成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
GoodExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"贷款还本成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	memset (&ln_reg, 0x00, sizeof(struct ln_reg_c));
	memset (&mo_infrm, 0x00, sizeof(struct mo_infrm_c));
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("1071",g_pub_tx.ac_no);			/* 帐号 */
	get_zd_int("1072",&g_pub_tx.ac_seqn);		/* 账户序号 */
	get_zd_data("107A",g_pub_tx.tmp_ind);		/* 利随本清 */
	get_zd_double("107B",&g_pub_tx.tmp_amt1);	/* 还本金金额 */
	print_tol_amt = g_pub_tx.tmp_amt1;
	get_zd_data("107C",g_pub_tx.note_type);		/* 凭证种类 */
	get_zd_data("107D",g_pub_tx.beg_note_no);	/* 起始凭证号码 */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );
	get_zd_data("107I",repay_ind);

	get_zd_data("0660",g_pub_tx.ct_ind);			/* 现转标志 */
	get_zd_data("0660",ct_flag);				/* 现转标志 */	
	if( g_pub_tx.ct_ind[0]=='9' )
	{
		strcpy( g_pub_tx.ct_ind , "2" );
	}
	sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
	WRITEMSG
	
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}

	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *检查介质状态*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*检查贷款户状态*/
	if( ret ) {return ret;}
					
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"按揭贷款不在此处理![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L038" );
		return 1;
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
	
	strcpy( tx_type , "贷款还本" );	
	
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( tx_type );
			
	/* 1.贷款账号、	     2.户名、	      3.类型、    4.总还款金额 	*/
	fprintf(fp,"LNDF%s|%s|%s|%.2lf|\n",g_pub_tx.ac_no , g_ln_mst.name , tx_type , print_tol_amt );	
	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
		
	return 0;
}

/* 打印--客户回单--本金回单 */
int  print_LNHDBJ( )	
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	long end_date =0;
	char exp_ind[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
	TRACE
	TRACE
 	TRACE   		
	pub_base_AllDwnFilName( tmpname );
	TRACE
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDBJ打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
  	TRACE      	
	/* 赋值部分 */
	strcpy( tx_type , "贷款还本" );		

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
	TRACE		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	TRACE			
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.开户日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.正常利率、 13.逾期利率、14.展期标志、 15.流水号	*/
	fprintf(fp,"LNHDBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,exp_ind ,g_pub_tx.trace_no );	
	TRACE	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}

/* 打印--行内传票--本金部分 */
int print_LNBKBJ( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	char exp_ind[5];
		
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
		sprintf(acErrMsg,"LNBKBJ打开文件失败!");
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
	/* 16.流水号  */
	fprintf(fp,"LNBKBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
