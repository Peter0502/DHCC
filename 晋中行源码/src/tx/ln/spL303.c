/*************************************************
* 文 件 名: spL303.c
* 功能描述：   
*              贷款本金核销
*
* 作    者:    lance
* 完成日期：   2003年01月22日
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
#include "ln_reg_c.h"
#include "ln_lo_c.h"
#include "ln_auth_c.h"
 
static struct ln_reg_c		ln_reg;	
static struct ln_lo_c ln_lo;
double	print_tol_amt=0.00;

spL303()  
{ 	
	int ret=0;
	int flag=0;
	int pay_flag=0;
	char typ[2];
	double amt_tmp=0.00;
	
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
	
	/* 检查主文件中金额是否为0 */	
	if ( !pub_base_CompDblVal(g_ln_mst.bal , 0.00) )
	{
		sprintf(acErrMsg,"贷款本金为0,不能核销本金![%s]",g_ln_parm.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L057" );
		ERR_DEAL
	}		
	/*** 不要求做任何限制 ******/
	/* 检查贷款状态是否为呆帐 	*/
	if( g_ln_mst.ac_sts[0]!='4' )
	{
		sprintf(acErrMsg,"贷款状态非呆帐,不能核销![%s]",g_ln_parm.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L056" );
		ERR_DEAL
	}
	vtcp_log("核销本金[%.2f]\n",g_ln_mst.bal);
	/* 检查是否可以完全归还本金 */
	flag = pub_base_CompDblVal(g_ln_mst.bal , g_pub_tx.tx_amt1);
	if (flag==-1)	/* 输入金额过多 */ 
	{
		sprintf(acErrMsg,"输入[%.2f],实际余额[%.2f]",g_pub_tx.tx_amt1,g_ln_mst.bal);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L039" );
		ERR_DEAL				
	}		

	
	/* 更新贷款登记簿(包括普通贷款和按揭贷款) */
	strcpy ( typ ,"1" );
	ret = pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,typ,
										g_pub_tx.tx_amt1,0.00,"0",1);
	if( ret )
	{
		sprintf(acErrMsg,"更新贷款登记簿 PASS!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"更新贷款登记簿 PASS!");
	WRITEMSG

	/* 按揭贷款清理按揭贷款欠款表 */ 
	if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{	
		ret = Ln_lo_Dec_Upd ( g_pub_tx.reply , 
			"ac_id=%ld and ac_seqn=%d  and pay_type='0' ", 
			g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "Ln_lo_Dec_Sel错!ret=[%d]", ret);
			WRITEMSG		
			goto ErrExit;	
		}
		sprintf( acErrMsg, "ac_id[%ld]ac_seqn=[%d]", g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		WRITEMSG				
		while(1)
		{
			ret = Ln_lo_Fet_Upd( &ln_lo, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		sprintf( acErrMsg,"fetch Ln_lo_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		goto ErrExit;
	 		}
			else if( ret==100 ){
				TRACE
				break;	
			}
			TRACE
		 	sprintf( acErrMsg,"进入还欠款部分！！！");
		 	WRITEMSG
			/* 还款处理--只处理所欠本金 */
			amt_tmp = ln_lo.pay_lo_amt + g_pub_tx.tx_amt1 ;
			 
			/* 还款金额在归还本期后,还有剩余 */
			if ( pub_base_CompDblVal( ln_lo.lo_amt , amt_tmp ) == -1 )
			{
				g_pub_tx.tx_amt1= g_pub_tx.tx_amt1 - ( ln_lo.lo_amt - ln_lo.pay_lo_amt );
	
				ln_lo.pay_lo_amt = ln_lo.lo_amt; /* 已归还金额 */	
				sprintf( acErrMsg,"欠款金额小于(已归还金额+还款金额)，剩余金额[%lf]",g_pub_tx.tx_amt1);
		 		WRITEMSG
			}else{	
				/* 刚好归还本期或者不能还清本期 */
				g_pub_tx.tx_amt1= 0.00;
	
				ln_lo.pay_lo_amt = ln_lo.pay_lo_amt+g_pub_tx.tx_amt1 ;/* 已归还金额 */	
				sprintf( acErrMsg,"欠款金额大于等于(已归还金额+还款金额)，剩余金额[%lf]",g_pub_tx.tx_amt1);
		 		WRITEMSG
		 	}					
	         
	                pay_flag=0;    /*是否还清标志*/
	                ret = pub_base_CompDblVal(ln_lo.lo_amt, ln_lo.pay_lo_amt );
	                if( ret ) pay_flag++;  
	                ret = pub_base_CompDblVal(ln_lo.lo_intst, ln_lo.pay_lo_intst );
	                if( ret ) pay_flag++;             
	                ret = pub_base_CompDblVal(ln_lo.pun_intst, ln_lo.pay_pun_intst );
	                if( ret ) pay_flag++;
	                if( pay_flag==0 )     
	                {
	            	    /* 修改是否还清标志 0-未还清 1-还清 */
	            	    strcpy( ln_lo.pay_type , "1" );
	            	    flag++;
	                } 
	                ret = Ln_lo_Upd_Upd(ln_lo, g_pub_tx.reply  );
	                if ( ret )
	                {
	                     sprintf( acErrMsg,"Ln_lo_Upd_Upd 出错[%d]",ret);
		             WRITEMSG
	                     break;
	                }	                
	   	
		}
		Ln_lo_Clo_Upd ();
		sprintf( acErrMsg,"欠款表更新成功!!!!");
		WRITEMSG
	}	
	/*20110807 晋中商行 */
	double sum=0.0;
	char m_sts[2]={'\0','\0'};
	sum=g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst;
	if(sum){
		strcpy(m_sts,"B");
	}else{
		strcpy(m_sts,"1");
	}
	ret=sql_execute(" update ln_auth set sts='%s' where pact_no='%s' and sts in ('X','L')  ",m_sts,g_ln_mst.pact_no);
	if(ret){
		sprintf( acErrMsg,"更新ln_auth失败");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}


	/* Pub_tx结构赋值 */
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
	strcpy ( g_pub_tx.brf,"贷款本金核销" );
	g_pub_tx.svc_ind = 3010;			/* 贷款本金核销 */
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
			
	/* 打印程序 */
	/* 行内传票--本金部分 */
	ret = print_LNHXBJ( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHXBJ,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE	
	
OkExit:
    set_zd_data("0210",g_ln_parm.cur_no);/* 对币种赋值 */
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款本金核销[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款本金核销[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&ln_lo, 0x00, sizeof(struct ln_lo_c)); 	
	memset (&ln_reg, 0x00, sizeof(struct ln_reg_c));
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("1071",g_pub_tx.ac_no);			/* 帐号 */
	get_zd_int("1072",&g_pub_tx.ac_seqn);		/* 账户序号 */

	get_zd_double("107B",&g_pub_tx.tx_amt1);	/* 核销金额 */
	print_tol_amt = g_pub_tx.tx_amt1;	
	vtcp_log("核销金额[%.2f]\n",g_pub_tx.tx_amt1);
	
	get_zd_data("107C",g_pub_tx.note_type);		/* 凭证种类 */
	get_zd_data("107D",g_pub_tx.beg_note_no);	/* 起始凭证号码 */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *检查介质状态*
	if( ret ) return ret; */
					
	return 0;
}

/* 打印--贷款核销传票--本金部分 */
int print_LNHXBJ( )
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
		sprintf(acErrMsg,"LNHXBJ打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "本金核销" );	
	
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
	fprintf(fp,"LNHXBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
