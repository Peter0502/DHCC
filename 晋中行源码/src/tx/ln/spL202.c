/*************************************************
* 文 件 名: spL202.c
* 功能描述：   
*              贷款还息
*
* 作    者:    lance
* 完成日期：   2003年07月28日
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
#include "ln_acm_reg_c.h"
#include "cif_basic_inf_c.h"

char	repay_ind[9];
char 	ct_flag[2];
int 	ret=0;
int 	in_lo=0,out_lo=0,cmpd_lo=0,bal_acm=0;
int 	cmpd_acm=0,bal_acm_in=0,bal_acm_out=0;
double 	amt_in_lo=0.00 , amt_out_lo=0.00, amt_cmpd_lo=0.00,amt_bal_acm=0.00;
double	amt_cmpd_acm=0.00,amt_bal_acm_in=0.00,amt_bal_acm_out=0.00;
long	ln_ic_date=0;	/* 贷款上次交易日期 */
long	in_lo_beg_date=0 , out_lo_beg_date=0 , cmpd_lo_beg_date=0; 
long	bal_acm_beg_date=0, cmpd_acm_beg_date=0 ,acm_beg_date=0;
char	acm_ind[2];
long    pri_end_date=0;	/* 终止日期 */
long	need_pay_beg_day=0;
double	print_tol_amt=0.00;
char flag_typ[2];	/* 本金类型标志 */

	struct ln_acm_reg_c ln_acm_reg;

spL202()  
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
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_get_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
	
	/* 取客户类型 */
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",
														g_ln_mst.cif_no);
	if( ret )
	{
		goto ErrExit;
	}
	if( cif_basic_inf.type[0]=='1' || cif_basic_inf.type[0]=='5' )
	{
		strcpy( cif_type , "0" );	/* 对私 */
	}else{
		strcpy( cif_type , "1" );	/* 对公 */
	}		
	set_zd_data("108P" , cif_type);	
	set_zd_double("108L",g_pub_tx.tmp_amt1);	/* 总还款金额 */
	sprintf(acErrMsg,"还款顺序108J[%s]还款金额[%.2lf]",repay_ind,
													g_pub_tx.tmp_amt1);
	WRITEMSG

	/* 还息函数 */
	ret = ln_pay_seq( repay_ind , &g_pub_tx.tmp_amt1 );
	if( ret )
	{
		sprintf(acErrMsg,"调用还息函数异常出错,请与中心机房联系!" );
		WRITEMSG
		goto ErrExit;
	}	
	sprintf(acErrMsg,"调用还息函数PASS!" );
	WRITEMSG	
	
	/* 有多余金额 */	
	if( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
	{
		sprintf(acErrMsg,"还款金额大于欠款金额,请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L205");
		goto ErrExit;
	}		
	
	/* 检查是否还清 */
	ret = pub_ln_chk_clear(g_ln_mst,g_ln_parm,clear_sts);
	if(ret)	goto ErrExit;
	if( pub_base_CompDblVal( g_ln_mst.gage_amt , 0.00 ) )
	{			
		set_zd_data("108O",clear_sts);
		/* 贷款有抵押品 */
	}else{
		set_zd_data("108O","*");
		/* 贷款有抵押品 */
	}
	
	set_zd_data("0210",g_ln_parm.cur_no);/* 对币种赋值 */

	/* 打印程序 */
	if( acm_ind[0]=='1' )
	{
		pri_end_date = g_pub_tx.tx_date;
	}else{
		pri_end_date = g_ln_mst.ic_date;
	}
			
	/* 对起始日期赋值 若空 则为99999999 否则不变 */
	if( !in_lo_beg_date )
	{
		in_lo_beg_date = 99999999;
	}
	if( !out_lo_beg_date )
	{
		out_lo_beg_date = 99999999;
	}
	if( !cmpd_lo_beg_date )
	{
		cmpd_lo_beg_date = 99999999;
	}
	if( !bal_acm_beg_date )
	{
		bal_acm_beg_date = 99999999;
	}
	if( !cmpd_acm_beg_date )
	{
		cmpd_acm_beg_date = 99999999;
	}
	if( !acm_beg_date )
	{
		acm_beg_date = 99999999;
	}
	/* 若是老数据则赋值为0  老数据中不明确的beg_date为19000101*/
	if( in_lo_beg_date==19000101 )
	{
		in_lo_beg_date = 0;
	}
	if( out_lo_beg_date==19000101 )
	{
		out_lo_beg_date = 0;
	}
	if( cmpd_lo_beg_date==19000101 )
	{
		cmpd_lo_beg_date = 0;
	}
	if( bal_acm_beg_date==19000101 )
	{
		bal_acm_beg_date = 0;
	}
	if( cmpd_acm_beg_date==19000101 )
	{
		cmpd_acm_beg_date = 0;
	}
	if( acm_beg_date==19000101 )
	{
		acm_beg_date = 0;
	}

	sprintf(acErrMsg,"in_lo_beg_date[%ld]",in_lo_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"out_lo_beg_date[%ld]",out_lo_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"cmpd_lo_beg_date[%ld]",cmpd_lo_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"bal_acm_beg_date[%ld]",bal_acm_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"cmpd_acm_beg_date[%ld]",cmpd_acm_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"acm_beg_date[%ld]",acm_beg_date );
	WRITEMSG			

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
	
	/* 客户回单--利息回单 *
	if( pub_base_CompDblVal( in_lo+out_lo+bal_acm , 0.00 ) )* 归还表内欠息，表外欠息，本金积数利息 *	
	{	
		ret = print_LNHDLX( );
		if(ret)
		{
			sprintf(acErrMsg,"print_LNHDLX,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}
	TRACE
	*******/
	
	/* 客户回单--复利回单 *
	if( pub_base_CompDblVal( cmpd_lo+cmpd_acm , 0.00 ) )	* 归还复利欠息，复利积数利息 *
	{
		ret = print_LNHDFL( );		
		if(ret)
		{
			sprintf(acErrMsg,"print_LNHDFL,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}
	TRACE
	*****/
	
	/* 若归还了本金积数 */
	if( bal_acm )
	{
		if( flag_typ[0]=='2' )	/* 入表内 */
		{
			bal_acm_in = 1;
		}else if ( flag_typ[0]=='3' ){	/* 入表外 */
			bal_acm_out = 1;			
		}
	}
	TRACE	
	/* 行内传票--表内部分 */
	if(pub_base_CompDblVal(in_lo+bal_acm_in, 0.00))	/*归还表内欠息，本金积数入表内 */	
	{
		ret = print_LNBKBBN( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNBKBBN,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE	
	/* 行内传票--表外部分 */
	if( pub_base_CompDblVal(out_lo+bal_acm_out, 0.00))/*归还表外欠息，本金积数入表外 */	
	{
		ret = print_LNBKBBW( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNBKBBW,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}
	TRACE	
	/* 行内传票--复利部分 */
	if( pub_base_CompDblVal(cmpd_lo+cmpd_acm, 0.00))/*归还复利欠息，复利积数*/	
	{
		ret = print_LNBKBFL( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNBKBFL,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}
	TRACE
	/* 行内传票-积数利息单 */
	if( pub_base_CompDblVal( bal_acm+cmpd_acm , 0.00 ))	/* 本金积数,复利积数 */
	{
		ret = print_LNJS( );				
		if(ret)
		{
			sprintf(acErrMsg,"print_LNJS,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	

	/*pub_mob_sendmail_ln("归还利息",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);*/
	pub_mob_sendmail_ln("归还利息",g_pub_tx.tx_code,g_ln_mst.pact_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);
	
	TRACE	
	if( ct_flag[0]=='1' )	/** 现金 **/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*转账*/
		goto ErrExit2;
    else if (ct_flag[0] == '3')		/*提出*/
        goto ErrExit3;
    else if (ct_flag[0] == '8')		/*扣划*/
        goto ErrExit8;

ErrExit:
    sprintf(acErrMsg,"贷款还息失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"贷款还息成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"贷款还息成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"贷款还息成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
ErrExit8:
    strcpy(g_pub_tx.reply,"0008");
    sprintf(acErrMsg,"贷款还息成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
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

	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("1081",g_pub_tx.ac_no);			/* 帐号 */
	get_zd_int("1082",&g_pub_tx.ac_seqn);		/* 账户序号 */
	get_zd_data("108A",g_pub_tx.tmp_ind);			/* 利随本清 */
	get_zd_double("108B",&g_pub_tx.tmp_amt1);			/* 还款金额 */
	print_tol_amt = g_pub_tx.tmp_amt1 ;
	get_zd_data("108C",g_pub_tx.note_type);			/* 凭证种类 */
	get_zd_data("108D",g_pub_tx.beg_note_no);			/* 起始凭证号码 */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );
	get_zd_data("108J",repay_ind);
	sprintf(acErrMsg,"顺序[%s]",repay_ind);
	WRITEMSG
	
	get_zd_data("0660",g_pub_tx.ct_ind);			/* 现转标志 */
	get_zd_data("0660",ct_flag);				/* 现转标志 */	
	sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
	WRITEMSG

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) 
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		return ret;
	}
	ln_ic_date = g_ln_mst.ic_date;		/* 起息日期 */
	
	/* 确定最早欠息日期 */
	ret = sql_min_long( "ln_reg" , "reg_date_beg" , &need_pay_beg_day , "ac_id=%ld and ac_seqn=%d  and type in('2','3','4') and sts='0' " , g_ln_mst.ac_id,g_ln_mst.ac_seqn);
	if( ret )	return 1;
	
	/* 若为19000101,则是老数据 */
	if( need_pay_beg_day==19000101 )
	{
		need_pay_beg_day = 0;
	}
	sprintf(acErrMsg,"need_pay_beg_day=[%ld]",need_pay_beg_day);
	WRITEMSG	
		
	/* 修改最小归还金额为 0.00 */
	g_ln_parm.min_intst = 0.00 ;

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *检查介质状态*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*检查贷款户状态*/
	if( ret ) 
	{
		sprintf(acErrMsg,"检查贷款户状态error");
		WRITEMSG
		return ret;
	}
					
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"按揭贷款不在此处理![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L038" );
		return 1;
	}

	g_pub_tx.ac_id = g_ln_mst.ac_id;	/* 账户序号 */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_get_ind,"10" );  /*已经读取了mdm_ac_rel*/
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/

	return 0;
}

/* 总控程序 */
int ln_pay_seq( char *pay_seq , double *tol_amt )
{
	int k=0;

	char sts[2];
	long beg_date_tmp=0;
	
	double amt=0.00;
	double need_pay=0.00;
	
	amt = *tol_amt;
	
	g_ln_parm.min_intst =0.00;
	
	sprintf(acErrMsg,"进入还息总控总金额[%lf]顺序[%s]",amt,pay_seq);
	WRITEMSG
	
	if( pub_base_CompDblVal( g_ln_parm.min_intst , g_pub_tx.tmp_amt1 )>0 )
	{
		need_pay = g_pub_tx.tmp_amt1 - g_ln_parm.min_intst ;
	}
	
	if( strlen( pay_seq )==0 )
	{
		sprintf(acErrMsg,"请录入还款顺序!" );
		WRITEMSG
        	strcpy( g_pub_tx.reply,"L191" );
		return 1;
	}
					
	for( k=0 ; k<5 ; k++ )
	{
		TRACE
		if( pay_seq[k]!='\0' )
		{
			TRACE
			g_pub_tx.tx_amt1 =0.00;
			g_pub_tx.tx_amt2 =0.00;
			g_pub_tx.tx_amt3 =0.00;
			g_pub_tx.tx_amt4 =0.00;		
			g_pub_tx.acm_beg_date = 0;
			
			sprintf( sts , "%c" , pay_seq[k] );
			sprintf(acErrMsg,"调用还息函数前金额[%lf]顺序[%s]",amt,sts);
			WRITEMSG	
			ret = pay_intst( sts , &amt );
			if( ret )
			{
				sprintf(acErrMsg,"调用还息函数异常出错,请与中心机房联系!" );
				WRITEMSG
				if( strlen(g_pub_tx.reply)==0 )
	        		{
		        		strcpy( g_pub_tx.reply,"L192" );
					
				}
				return 1;
			}
			
			if( pub_base_CompDblVal( amt , 0.00 ) <= 0 )
			{
				sprintf(acErrMsg,"无还款金额了。amt[%.2lf]!", amt);
				WRITEMSG
				break;
			}				
		}	
	}
	/* 考虑对未达到最低还款金额处理 */
	if( pub_base_CompDblVal (need_pay , 0.00 ) )
	{
		return 0;
	}
	
	*tol_amt = amt;
	return 0;
}
				

/* 还息函数 */
/* 入口：
   利息类型 1-表内欠息 2-表外欠息 3-复利欠息 4-未结利息
   还款金额
*/
int pay_intst( char *type , double *amt )	
{
	double	pay_amt=0.00;
	double	min_pay=0.00;	
	
	pay_amt=*amt;
	
	sprintf(acErrMsg,"调用具体还息函数前金额[%lf]顺序[%s]",pay_amt,type);
	WRITEMSG	
	
	if( type[0]=='1' )	/* 表内欠息 */
	{
		TRACE
		ret = pay_in_lo_intst( &pay_amt );
		if( ret )
		{
			sprintf(acErrMsg,"调用表内欠息还息函数异常出错,请与中心机房联系!" );
			WRITEMSG
			if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L193" );
	        	}
			return 1;
		}
		in_lo=1;
						
	}
	if( type[0]=='2' )	/* 表外欠息 */
	{
		TRACE
		ret = pay_out_lo_intst( &pay_amt );
		if( ret )
		{
			sprintf(acErrMsg,"调用表外欠息还息函数异常出错,请与中心机房联系!" );
			WRITEMSG
	        	if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L194" );
			}
			return 1;
		}
		out_lo=1;				
	}
	if( type[0]=='3' )	/* 复利欠息 */
	{
		TRACE
		ret = pay_cmpd_lo_intst( &pay_amt );
		if( ret )
		{
			sprintf(acErrMsg,"调用复利欠息还息函数异常出错,请与中心机房联系!" );
			WRITEMSG
			if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L195" );
			}
			return 1;
		}
		cmpd_lo=1;				
	}
	/****
	if( type[0]=='4' )	* 本金积数 *
	{
		TRACE
		ret = pay_bal_acm_intst( &pay_amt  );
		if( ret )
		{
			sprintf(acErrMsg,"调用本金积数还息函数异常出错,请与中心机房联系!" );
			WRITEMSG
	        	if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L196" );
	        	}
			return 1;
		}
		bal_acm=1;				
	}
	if( type[0]=='5' )	* 复利积数 *
	{
		TRACE
		ret = pay_cmpd_acm_intst( &pay_amt );
		if( ret )
		{
			sprintf(acErrMsg,"调用复利积数还息函数异常出错,请与中心机房联系!" );
			WRITEMSG
	        	if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L197" );
	        	}
			return 1;
		}
		cmpd_acm=1;				
	}	
	
	****/
	if( type[0]=='4' )	/* 未结利息 */
	{
		TRACE
		strcpy( acm_ind , "1" );

		/* 暂时屏蔽 *  解除屏蔽del by lance 20031103夜
		if( g_ln_mst.ac_id!=100354509 )
		{
			strcpy( g_pub_tx.reply,"L208" );
			return 1;
		}
		*******/

		ret = pay_acm_intst( &pay_amt  );
		if( ret )
		{
			sprintf(acErrMsg,"调用未结利息还息函数异常出错,请与中心机房联系!" );
			WRITEMSG
	        	if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L208" );
	        	}
			return 1;
		}				
	}
		
				
	*amt = pay_amt ;
	
	return 0;
}

/* 还息--表内欠息 */
int pay_in_lo_intst( double *amt )	
{
	double bal=0.00;
	
	bal = *amt;

	sprintf(acErrMsg,"进入还息--表内欠息[%lf]",bal);
	WRITEMSG
	
	ret = pub_base_CompDblVal( g_ln_mst.in_lo_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"表内欠息为零,不用归还!请重新设置还款计划表." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L201" );
	        return 1;
	}
		
	/* 确定还款金额 */
	/* 若不够还，或者刚好够还 */
	if( pub_base_CompDblVal( g_ln_mst.in_lo_intst , bal ) >=0 )
	{
		g_pub_tx.tx_amt2 = bal ;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt2 = g_ln_mst.in_lo_intst ;
		bal = bal - g_ln_mst.in_lo_intst;
	}
	
	/*写贷款登记簿*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",g_pub_tx.tx_amt2,0.00,"0",1 );
	if( ret ) 	return 1;			

	in_lo_beg_date = g_pub_tx.acm_beg_date ;
	sprintf(acErrMsg,"表内欠息起始日期[%ld]",g_pub_tx.acm_beg_date);
	WRITEMSG	
		
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );	/* 增减:0减1加2销户 */	
	g_pub_tx.svc_ind=3050;
	strcpy( g_pub_tx.brf, "");
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
		
	*amt = bal ;
	set_zd_double("108F",g_pub_tx.tx_amt2);
	amt_in_lo=g_pub_tx.tx_amt2;
		
	return 0;
	
}

/* 还息--表外欠息 */
int pay_out_lo_intst( double *amt )	
{
	double bal=0.00;
	
	bal = *amt;
	
	sprintf(acErrMsg,"进入还息--表外欠息[%lf]",bal);
	WRITEMSG
	ret = pub_base_CompDblVal( g_ln_mst.out_lo_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"表外欠息为零,不用归还!请重新设置还款计划表." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L200" );
	        return 1;
	}
	
	/* 确定还款金额 */
	/* 若不够还，或者刚好够还 */
	if( pub_base_CompDblVal( g_ln_mst.out_lo_intst , bal ) >=0 )
	{
		g_pub_tx.tx_amt3 = bal ;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt3 = g_ln_mst.out_lo_intst ;
		bal = bal - g_ln_mst.out_lo_intst;
	}
	
	/*写贷款登记簿*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",g_pub_tx.tx_amt3,0.00,"0",1 );
	if( ret ) 	return 1;	
			
	out_lo_beg_date = g_pub_tx.acm_beg_date ;
	sprintf(acErrMsg,"表外欠息起始日期[%ld]",g_pub_tx.acm_beg_date);
	WRITEMSG
		
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );	/* 增减:0减1加2销户 */	
	g_pub_tx.svc_ind=3050;
	strcpy( g_pub_tx.brf, "");	
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
		
	*amt = bal ;
	set_zd_double("108H",g_pub_tx.tx_amt3);
	amt_out_lo=g_pub_tx.tx_amt3;
	
	return 0;
	
}

/* 还息--复利欠息 */
int pay_cmpd_lo_intst( double *amt )	
{
	double bal=0.00;
	
	bal = *amt;
	
	sprintf(acErrMsg,"进入还息--复利欠息[%lf]",bal);
	WRITEMSG
		
	ret = pub_base_CompDblVal( g_ln_mst.cmpd_lo_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"复利欠息为零,不用归还!请重新设置还款计划表." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L199" );
	        return 1;
	}
	
	/* 确定还款金额 */
	/* 若不够还，或者刚好够还 */
	if( pub_base_CompDblVal( g_ln_mst.cmpd_lo_intst , bal ) >=0 )
	{
		g_pub_tx.tx_amt4 = bal ;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt4 = g_ln_mst.cmpd_lo_intst ;
		bal = bal - g_ln_mst.cmpd_lo_intst;
	}
	
	/*写贷款登记簿*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",g_pub_tx.tx_amt4,0.00,"0",1 );
	if( ret ) 	return 1;	

	cmpd_lo_beg_date = g_pub_tx.acm_beg_date ;
	sprintf(acErrMsg,"复利欠息起始日期[%ld]",g_pub_tx.acm_beg_date);
	WRITEMSG
			
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );	/* 增减:0减1加2销户 */	
	g_pub_tx.svc_ind=3050;
	strcpy( g_pub_tx.brf, "");	
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
		
	*amt = bal ;
	set_zd_double("108I",g_pub_tx.tx_amt4);
	amt_cmpd_lo=g_pub_tx.tx_amt4;
			
	return 0;
	
}

/* 还息--本金积数 */
int pay_bal_acm_intst( double *amt )	
{
	double bal=0.00;
	double acm_intst=0.00;
	int flag;			/* 表内/表外标志 */
			
	bal = *amt;
	
	sprintf(acErrMsg,"进入还息--本金积数[%lf]",bal);
	WRITEMSG
		
	/* 本金积数计算利息 */
	ret = pub_ln_CalIntst ( g_ln_mst , g_ln_parm , "0" , &acm_intst , 0 );
	if( ret )	return 1;
	
	/* 检查金额是否为零 */
	ret = pub_base_CompDblVal( acm_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"本金积数利息为零,不用归还!请重新设置还款计划表." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L198" );
	        return 1;
	}
	
	/* 检查金额是否可以一次还清 */
	/* 确定还款金额 */
	/* 若不够还，或者刚好够还 */
	
	if( pub_base_CompDblVal( acm_intst , bal ) >0 ){
		
		sprintf(acErrMsg,"本金积数利息必须一次还清!请重新设置还款计划表." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L202" );
	        return 1;		
	}else if( pub_base_CompDblVal( acm_intst , bal ) ==0 )
	{
		g_pub_tx.tx_amt1 = acm_intst ;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt1 = acm_intst ;
		bal = bal - acm_intst;
	}
		
	
	/* 归还本金积数产生的利息记账 */
	/* 贷款本金利息类型判断 */	
	flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );
	sprintf( flag_typ , "%d" , flag+1 ); 


	/*写贷款登记簿*/
	ret = pub_ln_ln_reg(g_ln_mst,ln_ic_date,g_pub_tx.tx_date,flag_typ,acm_intst,g_ln_mst.intst_acm,"0",2);
	if( ret ) return 1;
	
	bal_acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"本金积数起始日期[%ld]",ln_ic_date);
	WRITEMSG
	
	strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
	strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
	strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	g_pub_tx.svc_ind=3011;
	strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
	strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/
	strcpy( g_pub_tx.brf,"还本金积数" );

	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
				
	*amt = bal ;
	set_zd_double("108K",acm_intst);

	if( flag==1 )
	{
		set_zd_double("108M",acm_intst);	/* 表内 */		
		amt_bal_acm=acm_intst;
		amt_bal_acm_in=acm_intst;

	}
	if( flag==2 )
	{
		set_zd_double("108N",acm_intst);	/* 表外 */
		amt_bal_acm=acm_intst;
		amt_bal_acm_out=acm_intst;
	}
			
	return 0;	
}

/* 还息--复利积数 */
int pay_cmpd_acm_intst( double *amt )	
{
	double	vbnlx=0.00,vbwlx=0.00,vfllx=0.00;
	double bal=0.00;
			
	bal = *amt;
	
	sprintf(acErrMsg,"进入还息--复利积数[%lf]",bal);
	WRITEMSG
		
	/* 表内/表外/复利积数计算利息 */		
	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"1",&vbnlx,0 );
	if( ret ) return 1;

	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"2",&vbwlx,0 );
	if( ret ) return 1;

	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"3",&vfllx,0 );
	if( ret ) return 1;
	
	cmpd_acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"复利积数起始日期[%ld]",ln_ic_date);
	WRITEMSG
	
	/* 检查金额是否为零 */
	ret = pub_base_CompDblVal( vbnlx+vbwlx+vfllx , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"复利积数利息为零,不用归还!请重新设置还款计划表." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L203" );
	        return 1;
	}
	
	/* 检查金额是否可以一次还清 */
	/* 确定还款金额 */
	/* 若不够还，或者刚好够还 */
	
	if( pub_base_CompDblVal( vbnlx+vbwlx+vfllx , bal ) >0 ){
		
		sprintf(acErrMsg,"复利积数利息必须一次还清!请重新设置还款计划表." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L204" );
	        return 1;		
	}else if( pub_base_CompDblVal( vbnlx+vbwlx+vfllx , bal ) ==0 )
	{
		g_pub_tx.tx_amt2=vbnlx;
		g_pub_tx.tx_amt3=vbwlx;
		g_pub_tx.tx_amt4=vfllx;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt2=vbnlx;
		g_pub_tx.tx_amt3=vbwlx;
		g_pub_tx.tx_amt4=vfllx;
		bal = bal - (vbnlx+vbwlx+vfllx);
	}
	
	/*写贷款登记簿*/
	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vbnlx,g_ln_mst.in_lo_acm,"0",2 );
	if( ret ) return 1;

	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vbwlx,g_ln_mst.out_lo_acm,"0",2 );
	if( ret ) return 1;
	
	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vfllx,g_ln_mst.cmpd_lo_acm,"0",2 );
	if( ret ) return 1;
	
	strcpy ( g_pub_tx.ac_wrk_ind , "0000" );/* 零金额不记流水,明细*/		
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.brf,"还利息积数产生的复利" );
	g_pub_tx.svc_ind=3051;

	vtcp_log("=========[%.2lf][%.2lf][%.2lf]",vbnlx,vbwlx,vfllx);
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		return 1;
	}
			
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
				
	*amt = bal ;
	set_zd_double("108E",vbnlx+vbwlx+vfllx);
	amt_cmpd_acm=vbnlx+vbwlx+vfllx;		
	return 0;	
}

/* 还息--未结利息 */
int pay_acm_intst( double *amt )	
{
	double bal=0.00;
	double acm_intst=0.00;
	double	vbnlx=0.00,vbwlx=0.00,vfllx=0.00;
	int flag;			/* 表内/表外标志 */
			
	bal = *amt;
	
	sprintf(acErrMsg,"进入还息--未结利息[%lf]",bal);
	WRITEMSG
		
	/* 本金积数计算利息 */
	ret = pub_ln_CalIntst ( g_ln_mst , g_ln_parm , "0" , &acm_intst , 0 );
	if( ret )	return 1;
	
	sprintf(acErrMsg,"本金积数计算利息--未结利息[%lf][%lf][%lf][%lf]",acm_intst,vbnlx,vbwlx,vfllx);
	WRITEMSG
		
	acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"未结利息起始日期[%ld]",ln_ic_date);
	WRITEMSG
	cmpd_acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"未结复利起始日期[%ld]",cmpd_acm_beg_date);
	WRITEMSG
	bal_acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"本金积数起始日期[%ld]",ln_ic_date);
	WRITEMSG
			
	/* 表内/表外/复利积数计算利息 */		
	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"1",&vbnlx,0 );
	if( ret ) return 1;
	sprintf(acErrMsg,"本金积数计算利息--未结利息[%lf][%lf][%lf][%lf]",acm_intst,vbnlx,vbwlx,vfllx);
	WRITEMSG
	
	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"2",&vbwlx,0 );
	if( ret ) return 1;
	sprintf(acErrMsg,"本金积数计算利息--未结利息[%lf][%lf][%lf][%lf]",acm_intst,vbnlx,vbwlx,vfllx);
	WRITEMSG
	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"3",&vfllx,0 );
	if( ret ) return 1;
	sprintf(acErrMsg,"本金积数计算利息--未结利息[%lf][%lf][%lf][%lf]",acm_intst,vbnlx,vbwlx,vfllx);
	WRITEMSG	
	
	if( pub_base_CompDblVal( acm_intst , 0.00 ))
	{
		bal_acm=1;
	}
	if( pub_base_CompDblVal( vbnlx+vbwlx+vfllx , 0.00 ))
	{
		cmpd_acm=1;
	}	
	
	/* 检查金额是否为零 */
	ret = pub_base_CompDblVal( acm_intst+vbnlx+vbwlx+vfllx , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"未结积数利息为零,不用归还!请重新设置还款计划表." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L209" );
	        return 1;
	}
	
	/* 检查金额是否可以一次还清 */
	/* 确定还款金额 */
	/* 若不够还，或者刚好够还 */
	
	if( pub_base_CompDblVal( acm_intst+vbnlx+vbwlx+vfllx , bal ) >0 ){
		
		sprintf(acErrMsg,"未结积数利息必须一次还清!请重新设置还款次序.需[%lf]bal[%lf]",acm_intst+vbnlx+vbwlx+vfllx , bal );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L210" );
	        return 1;		
	}else if( pub_base_CompDblVal( acm_intst+vbnlx+vbwlx+vfllx , bal ) ==0 )
	{
		g_pub_tx.tx_amt1 = acm_intst ;
		g_pub_tx.tx_amt2=vbnlx;
		g_pub_tx.tx_amt3=vbwlx;
		g_pub_tx.tx_amt4=vfllx;
		
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt1 = acm_intst ;		
		g_pub_tx.tx_amt2=vbnlx;
		g_pub_tx.tx_amt3=vbwlx;
		g_pub_tx.tx_amt4=vfllx;
		
		bal = bal - (acm_intst+vbnlx+vbwlx+vfllx);
	}
		
	/* 贷款本金利息类型判断 */
	flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );
	sprintf( flag_typ , "%d" , flag+1 ); 
	
	sprintf(acErrMsg,"ly贷款本金利息类型[%s]",flag_typ );	
	WRITEMSG	
	

	/*写贷款登记簿*/
	ret = pub_ln_ln_reg(g_ln_mst,ln_ic_date,g_pub_tx.tx_date,flag_typ,acm_intst,g_ln_mst.intst_acm,"0",2);
	if( ret ) return 1;
	
	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vbnlx,g_ln_mst.in_lo_acm,"0",2 );
	if( ret ) return 1;

	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vbwlx,g_ln_mst.out_lo_acm,"0",2 );
	if( ret ) return 1;
	
	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vfllx,g_ln_mst.cmpd_lo_acm,"0",2 );
	if( ret ) return 1;
		
	/*** 归还其他积数产生的利息记账 ***/
	vtcp_log("=========[%.2lf][%.2lf][%.2lf]",vbnlx,vbwlx,vfllx);
	vtcp_log("=========[%.2lf][%.2lf][%.2lf]",g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
		
	strcpy ( g_pub_tx.ac_wrk_ind , "0000" );/* 零金额不记流水,明细*/		
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.brf,"还利息积数产生的复利" );
	g_pub_tx.svc_ind=3051;
	
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		return 1;
	}
			
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
	set_zd_double("108E",vbnlx+vbwlx+vfllx);	
	amt_cmpd_acm=vbnlx+vbwlx+vfllx;	

	/* 归还本金积数产生的利息记账 */
	vtcp_log("=========[%.2lf]",acm_intst );
	vtcp_log("=========[%.2lf]",g_pub_tx.tx_amt1 );

	strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
	strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
	strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	if( flag==1 )
	{
		g_pub_tx.svc_ind=3011;
	}else if( flag==2 ){
		g_pub_tx.svc_ind=3012;	
	}	
	strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
	strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/
	strcpy( g_pub_tx.brf,"还本金积数" );

	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG

	set_zd_double("108K",acm_intst);	/* 本金未结利息 */

	if( flag==1 )
	{
		set_zd_double("108M",acm_intst);	/* 表内 */		
		amt_bal_acm=acm_intst;
		amt_bal_acm_in=acm_intst;
	}
	if( flag==2 )
	{
		set_zd_double("108N",acm_intst);	/* 表外 */
		amt_bal_acm=acm_intst;
		amt_bal_acm_out=acm_intst;
	}
					
	*amt = bal ;

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
	
	strcpy( tx_type , "贷款还息" );	
	
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( tx_type );
			
	/* 1.贷款账号、	     2.户名、	      3.类型、    4.总还款金额 	*/
	fprintf(fp,"LNDF%s|%s|%s|%.2lf|\n",g_pub_tx.ac_no , g_ln_mst.name , tx_type , print_tol_amt );	
	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
		
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
	strcpy( tx_type , "贷款还息" );		
	
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
		
	/* 欠息总额 */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type in('2','3','4')" , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* 累计还款 */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type in('2','3','4')" , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* 尚欠金额 */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***改为在主文件中取值****/
	tol_need_pay = g_ln_mst.in_lo_intst + g_ln_mst.out_lo_intst + g_ln_mst.cmpd_lo_intst ;
	if( pub_base_CompDblVal( tol_need_pay , 0.00 ) ==0 )
	{
		tol_need_pay= 0.00;
	}
	/* 交易金额明细 */
	lx_tol = amt_in_lo + amt_out_lo + amt_bal_acm ;	/* 利息 */
	fl_tol = amt_cmpd_lo + amt_cmpd_acm  ;		/* 复利 */
	
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
	/*  6.交易金额、  7.起始日期、  8.终止日期、 9.欠息总额、 10.累计还款     */
	/* 11.尚欠金额、 12.正常利率、 13.逾期利率、14.流水号、   15.明细利息  */
	/* 16.明细复利  */
	fprintf(fp,"LNHDHZ%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%.2lf|%.2lf|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , g_pub_tx.trace_no , lx_tol ,
	fl_tol );	
	TRACE
vtcp_log("怎么不显示呢？？tx_type=[%s]ac_no=[%s]pact_no[%s]name[%s]",tx_type,g_pub_tx.ac_no,g_ln_mst.pact_no,g_ln_mst.name);	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}	
/* 打印--客户回单--利息回单 */
int  print_LNHDLX( )	
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
    	double tol_amt=0.00;
    	 
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
	strcpy( tx_type , "贷款还息" );		
	/* 确定起始日期 */
	/******
	if( flag_typ[0]=='2' )	* 入表内 *
	{
		ret = min_day( in_lo_beg_date , out_lo_beg_date , bal_acm_beg_date , &start_day );
		if( ret )	return 1;		
	}else if ( flag_typ[0]=='3' ){	* 入表外 *
		ret = min_day( in_lo_beg_date , out_lo_beg_date , bal_acm_beg_date , &start_day );
		if( ret )	return 1;					
	}
	*****/
	ret = sql_min_long( "ln_reg" , "reg_date_beg" , &start_day , "ac_id=%ld and ac_seqn=%d  and type in('2','3') " , g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )	return 1;
	TRACE
		
	/* 欠息总额 */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type in('2','3')" , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* 累计还款 */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type in('2','3')" , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* 尚欠金额 */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***改为在主文件中取值****/
	tol_need_pay = g_ln_mst.in_lo_intst + g_ln_mst.out_lo_intst;	
	
	/* 交易金额 */
	tol_amt = amt_in_lo + amt_out_lo + amt_bal_acm ;
	
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
	/*  6.交易金额、  7.起始日期、  8.终止日期、 9.欠息总额、 10.累计还款     */
	/* 11.尚欠金额、 12.正常利率、 13.逾期利率、14.流水号 */
	fprintf(fp,"LNHDLX%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , g_pub_tx.trace_no );	
	TRACE	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}

/* 打印--客户回单--复利回单 */
int  print_LNHDFL( )	
{
  	TRACE  
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	
	char rate[11];
	char over_rate[11];
	long start_day=0;
	
	TRACE
    	char filename[51],tmpname[101];
    	FILE *fp;
     	double tol_amt=0.00;
    	    	
  	TRACE  	
	pub_base_AllDwnFilName( tmpname );
	TRACE
	
	sprintf(acErrMsg,"文件名:[%s]!",tmpname);
	WRITEMSG
		
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDFL打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        TRACE
        	
	/* 赋值部分 */
	strcpy( tx_type , "贷款复利" );		
	TRACE
	/* 确定起始日期 */		
	/*** 
	sprintf(acErrMsg,"cmpd_acm_beg_date:[%ld]!",cmpd_acm_beg_date);
	WRITEMSG
	sprintf(acErrMsg,"cmpd_lo_beg_date:[%ld]!",cmpd_lo_beg_date);
	WRITEMSG÷
	
	ret = min_day( cmpd_acm_beg_date , cmpd_lo_beg_date , 99999999 , &start_day );
	if( ret )	return 1;					
	*****/
	
	ret = sql_min_long( "ln_reg" , "reg_date_beg" , &start_day , "ac_id=%ld and ac_seqn=%d  and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )	return 1;
	TRACE
		
	/* 欠息总额 */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* 累计还款 */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* 尚欠金额 */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***改为在主文件中取值****/
	tol_need_pay = g_ln_mst.cmpd_lo_intst ;	

	/* 交易金额 */
	tol_amt = amt_cmpd_lo + amt_cmpd_acm  ;

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
	/*  6.交易金额、  7.起始日期、  8.终止日期、 9.欠息总额、 10.累计还款     */
	/* 11.尚欠金额、 12.正常利率、 13.逾期利率、14.流水号*/
	fprintf(fp,"LNHDFL%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no);	
	TRACE
		
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");		
	TRACE
	return 0;
}

/* 打印--行内传票--表内部分 */
int print_LNBKBBN( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	
	int ret_tmp=0;
	long start_day=0;
	char start_day_str[15];
	char ln_acc_hrt[8];
	char data_code[5];
			
    	char filename[51],tmpname[101];
    	FILE *fp;
      	double tol_amt=0.00;
    	   	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKBBN打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "表内利息" );		
	/* 确定起始日期 */
	sprintf(acErrMsg,"表内，表外flag_typ=[%s]!",flag_typ);
	WRITEMSG	
	sprintf(acErrMsg,"in_lo_beg_date=[%ld],bal_acm_beg_date=[%ld]",in_lo_beg_date,bal_acm_beg_date);
	WRITEMSG
	
	if( acm_ind[0]=='1' )	/* 归还了未结利息 */
	{
		if( flag_typ[0]=='2' )	/* 入表内 */
		{
			ret = min_day( 99999999 , in_lo_beg_date , bal_acm_beg_date , &start_day );
			if( ret )	return 1;		
		}else if ( flag_typ[0]=='3' ){	/* 入表外 */
			ret = min_day( 99999999 , in_lo_beg_date , 99999999 , &start_day );
			if( ret )	return 1;					
		}
	}else{
		ret = min_day( 99999999 , in_lo_beg_date , 99999999 , &start_day );
		if( ret )	return 1;
	}	
	/* 转换成字符串,若旧数据日期不确定的,则不显示 */
	if( start_day )
	{
		sprintf( start_day_str, "%4d-%02d-%02d", start_day/10000, start_day%10000/100, start_day%100 );
	}else{
		strcpy( start_day_str, "");
	}	

	/* 取科目号 */
	strcpy( data_code , "0155" );	/* 表内科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}
	
	/* 欠息总额 */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , " ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='2' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* 累计还款 */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='2' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* 尚欠金额 */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***改为在主文件中取值****/
	tol_need_pay = g_ln_mst.in_lo_intst ;	
	if( pub_base_CompDblVal( tol_need_pay , 0.00 )==0 )
	{
		tol_need_pay= 0.00;
	}
		
	/* 交易金额 */
	tol_amt = amt_in_lo + amt_bal_acm_in  ;

	sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf‰" , g_ln_mst.over_rate);

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	pub_base_strpack( ln_acc_hrt );	
			
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.终止日期、 9.欠息总额、 10.累计还款     */
	/* 11.尚欠金额、 12.正常利率、 13.逾期利率、14.流水号、   15.科目号*/
	fprintf(fp,"LNBKBBN%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
	
/* 打印--行内传票--表外部分 */
int print_LNBKBBW( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	
	long start_day=0;
	char start_day_str[15];
	
	char ln_acc_hrt[8];
	char data_code[5];
	
	int ret_tmp=0;		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
    	double tol_amt=0.00;
    	     	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKBBW打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "表外利息" );		
	sprintf(acErrMsg,"表内，表外flag_typ=[%s]!",flag_typ);
	WRITEMSG	
	sprintf(acErrMsg,"out_lo_beg_date=[%ld],bal_acm_beg_date=[%ld]",out_lo_beg_date,bal_acm_beg_date);
	WRITEMSG	
	/* 确定起始日期 */
	if( acm_ind[0]=='1' )	/* 归还了未结利息 */
	{
		TRACE
		if( flag_typ[0]=='2' )	/* 入表内 */
		{
			TRACE
			ret = min_day( 99999999 , out_lo_beg_date , 99999999 , &start_day );
			if( ret )	return 1;		
		}else if ( flag_typ[0]=='3' ){	/* 入表外 */
			TRACE
			ret = min_day( 99999999 , out_lo_beg_date , bal_acm_beg_date , &start_day );
			if( ret )	return 1;					
		}
	}else{
		TRACE
		ret = min_day( 99999999 , out_lo_beg_date , 99999999 , &start_day );
		if( ret )	return 1;
	}	
	/* 转换成字符串,若旧数据日期不确定的,则不显示 */
	if( start_day )
	{
		sprintf( start_day_str, "%4d-%02d-%02d", start_day/10000, start_day%10000/100, start_day%100 );
	}else{
		strcpy( start_day_str, "");
	}	
	
	/* 取科目号 */
	strcpy( data_code , "0156" );	/* 表外科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}	

	/* 欠息总额 */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='3' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* 累计还款 */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='3' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* 尚欠金额 */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***改为在主文件中取值****/
	tol_need_pay =  g_ln_mst.out_lo_intst ;	
	if( pub_base_CompDblVal( tol_need_pay , 0.00 )==0 )
	{
		tol_need_pay= 0.00;
	}
		
	/* 交易金额 */
	tol_amt = amt_out_lo + amt_bal_acm_out  ;

	sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf‰" , g_ln_mst.over_rate);
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	pub_base_strpack( ln_acc_hrt );	
			
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.终止日期、 9.欠息总额、 10.累计还款     */
	/* 11.尚欠金额、 12.正常利率、 13.逾期利率、14.流水号、   15.科目号*/
	fprintf(fp,"LNBKBBW%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
	
/* 打印--行内传票--复利部分 */
int print_LNBKBFL( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	int ret_tmp=0;	
	long start_day=0;
	char start_day_str[15];
	char ln_acc_hrt[8];
	char data_code[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
     	double tol_amt=0.00;
    	    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKBFL打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "复利利息" );		
	/* 确定起始日期 */
	if( acm_ind[0]=='1' )	/* 归还了未结利息 */
	{
		if( flag_typ[0]=='2' )	/* 入表内 */
		{
			ret = min_day( cmpd_acm_beg_date , cmpd_lo_beg_date , 99999999 , &start_day );
			if( ret )	return 1;		
		}else if ( flag_typ[0]=='3' ){	/* 入表外 */
			ret = min_day( cmpd_acm_beg_date , cmpd_lo_beg_date , 99999999 , &start_day );
			if( ret )	return 1;					
		}
	}else{
		ret = min_day( 99999999 , cmpd_lo_beg_date , 99999999 , &start_day );
		if( ret )	return 1;
	}	
	/* 转换成字符串,若旧数据日期不确定的,则不显示 */
	if( start_day )
	{
		sprintf( start_day_str, "%4d-%02d-%02d", start_day/10000, start_day%10000/100, start_day%100 );
	}else{
		strcpy( start_day_str, "");
	}	
	
	/* 取科目号 */
	strcpy( data_code , "0157" );	/* 复利科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}	
	/* 欠息总额 */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* 累计还款 */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* 尚欠金额 */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***改为在主文件中取值****/
	tol_need_pay = g_ln_mst.cmpd_lo_intst ;	
	if( pub_base_CompDblVal( tol_need_pay , 0.00 )==0 )
	{
		tol_need_pay= 0.00;
	}
		
	/* 交易金额 */
	tol_amt = amt_cmpd_lo + amt_cmpd_acm  ;

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
	/*  6.交易金额、  7.起始日期、  8.终止日期、 9.欠息总额、 10.累计还款   */
	/* 11.尚欠金额、 12.正常利率、 13.逾期利率、14.流水号、   15.科目号*/
	fprintf(fp,"LNBKBFL%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}

/* 打印--积数利息单 */
int print_LNJS( )
{
	char tmp_type[2];
	long true_days;	
	char ln_intst_type[11];
	char acm_intst_type[21];

    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	memset( &ln_acm_reg , 0x00 , sizeof(struct ln_acm_reg_c) );
		
    	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNJS打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        
	ret = Ln_acm_reg_Dec_Sel( g_pub_tx.reply , "trace_no=%ld ORDER BY intst_type" , g_pub_tx.trace_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_acm_reg_Dec_Sel错!ret=[%d]", ret);
		WRITEMSG		
		return 1;	
	}
	while(1)
	{
		ret = Ln_acm_reg_Fet_Sel( &ln_acm_reg, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_acm_reg_Fet_Sel error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}else if( ret==100 ){
			break;
		}
		
		pub_base_dic_show_str(ln_intst_type,"ln_intst_knd",g_ln_parm.intst_type);	/* 计息类型 */
		
	 	sprintf( acErrMsg,"fetch Ln_acm_reg_Fet_Sel error code = [%d]",ret);
	 	WRITEMSG
	 					
		pub_base_dic_show_str(acm_intst_type,"ln_acm_type",ln_acm_reg.intst_type);	/* 利息类型 */				
		true_days = pub_base_CalTrueDays(ln_acm_reg.beg_date,ln_acm_reg.end_date);	/* 天数计算 */

		pub_base_strpack(g_ln_parm.title);
		pub_base_strpack(g_pub_tx.ac_no);		
		pub_base_strpack(ln_intst_type);
		pub_base_strpack(g_ln_mst.name);
		pub_base_strpack(acm_intst_type);
										
		/* 1.产品名称、	     2.账号、	      3.计息方式、    4.户名、	      5.利息类型、 */
		/* g_ln_parm.title , g_pux_tx.ac_no , ln_intst_type , g_ln_mst.name , acm_intst_type ,*/
		/* 6.起始日期、	         7.终止日期、	       8.天数、    9.积数、		  10.利率、	    11.金额 */
		/* ln_acm_reg.beg_date , ln_acm_reg.end_date , true_days , ln_acm_reg.intst_acm , ln_acm_reg.rate , ln_acm_reg.intst */
		fprintf(fp,"LNJS%s|%s|%s|%s|%s|%ld|%ld|%ld|%.2lf|%.5lf|%.2lf|\n",
		g_ln_parm.title , g_pub_tx.ac_no , ln_intst_type , g_ln_mst.name , acm_intst_type , 
		ln_acm_reg.beg_date , ln_acm_reg.end_date , true_days , ln_acm_reg.intst_acm , ln_acm_reg.rate , 
		ln_acm_reg.intst);	
	}	
	Ln_acm_reg_Clo_Sel ();
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}

/* 三个日期排序 返回非零日期最小值*/	
int min_day( long day1 , long day2 , long day3 , long *min_day )
{
	long tmp_day=0 ;
	
	
	if( day1 <= day2 )
	{
		tmp_day = day1;
	}else{
		tmp_day = day2;
	}
	if( tmp_day <= day3 )
	{
		tmp_day = tmp_day;
	}else{
		tmp_day = day3;
	}
	
	*min_day = tmp_day;
	
	return 0;
}	
