/*************************************************
* 文 件 名: spL304.c
* 功能描述：   
*              贷款利息核销
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
#include "ln_pay_pln_c.h"
#include "ln_auth_c.h"

static struct ln_lo_c ln_lo;
static struct ln_reg_c		ln_reg;	
static struct ln_pay_pln_c	pri_ln_pay_pln;

double print_tol_amt=0.00;
double in_lo_beg_date= 0.00, out_lo_beg_date= 0.00, cmpd_lo_beg_date= 0.00;
double 	amt_in_lo=0.00, amt_out_lo=0.00, amt_cmpd_lo=0.00;
long    pri_end_date=0;	/* 终止日期 */
int ret=0;
char print_flag[11];/*第一位:普通贷款表内;第二位:普通贷款表外;第三位:普通贷款复利;*/
			

spL304()  
{ 	
	double amt_tmp=0.00;
	int pay_flag=0;
			
	struct S_ln_intst vln_intst[LN_INTST_TYPS];
	struct ln_auth_c	sLn_auth;
	int i=0;
	int flag=0;
	
	double amt1=0.00,amt2=0.00;

	memset( vln_intst,0,sizeof(vln_intst) );
	memset(&sLn_auth,0x00,sizeof(sLn_auth));
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_get_struct() )        ERR_DEAL
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
	
		
	/* 长沙商业银行－－不检查 ****/
	/* 检查贷款状态是否为呆帐 	
	if( g_ln_mst.ac_sts[0]!='4' )
	{
		sprintf(acErrMsg,"贷款状态非呆帐,不能核销![%s]",g_ln_parm.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L056" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"检查贷款状态是否为呆帐 PASS!");
	WRITEMSG
	*****************/
	
	/** 普通贷款 **/
	if ( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
	{
		/* 检查主文件中欠息金额之和是否为0 */	
		if ( !pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst
											+g_ln_mst.cmpd_lo_intst , 0) )
		{
			sprintf(acErrMsg,"贷款欠息为0,不能核销![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			strcpy( g_pub_tx.reply,"L074" );
			ERR_DEAL
		}
		sprintf(acErrMsg,"检查主文件中欠息金额之和是否为0 PASS!");
		WRITEMSG
		
		/* 检查主文件中欠息金额之和是否全还清 */	
		if ( pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst
											+g_ln_mst.cmpd_lo_intst , g_pub_tx.tmp_amt1) )
		{
			sprintf(acErrMsg,"所欠利息必须一次性核销![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			strcpy( g_pub_tx.reply,"L232" );
			ERR_DEAL
		}
		sprintf(acErrMsg,"所欠利息必须一次性核销 PASS!");
		WRITEMSG		
		
		/* 检查是否可以完全归还本金 */
		flag = pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst
								+g_ln_mst.cmpd_lo_intst , g_pub_tx.tmp_amt1);
		if (flag<0)	/* 输入金额过多 */ 
		{
			sprintf(acErrMsg,"还款金额过大![%lf][%lf]", g_ln_mst.in_lo_intst
			+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst,g_pub_tx.tmp_amt1);
			WRITEMSG
			strcpy( g_pub_tx.reply,"L219" );
			ERR_DEAL				
		}	
		sprintf(acErrMsg,"检查是否可以完全归还本金 PASS!");
		WRITEMSG
		
		/* 检查是否可以完全归还 */
		flag = pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst
								+g_ln_mst.cmpd_lo_intst , g_pub_tx.tmp_amt1);
		if (!flag)	/* 可以完全归款 */ 
		{
			ret = pub_ln_CleanAcm ( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "1" );
			if (ret)
			{
				sprintf(acErrMsg,"调用函数清理积数失败");
				WRITEMSG
				ERR_DEAL	
			}
			strcpy ( g_pub_tx.intst_type , "0" );	/* 计息类型 */			
			
		}		
		sprintf(acErrMsg,"检查是否可以完全归还 PASS!");
		WRITEMSG
			
		/* 确定各项欠息的还款金额 */
		i=0;
		vln_intst[i].intst_amt=g_ln_mst.in_lo_intst; vln_intst[i].type=2;
		i++;
		vln_intst[i].intst_amt=g_ln_mst.out_lo_intst; vln_intst[i].type=4;
		i++;
		vln_intst[i].intst_amt=g_ln_mst.cmpd_lo_intst; vln_intst[i].type=6;
		i++;
		
		ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
												&g_pub_tx.tmp_amt1 );
		if( ret ) goto ErrExit;
	
		g_pub_tx.tx_amt1=0.00;
		g_pub_tx.tx_amt2=vln_intst[0].pay_amt;
		g_pub_tx.tx_amt3=vln_intst[1].pay_amt;
		g_pub_tx.tx_amt4=vln_intst[2].pay_amt;
		
		amt_in_lo= g_pub_tx.tx_amt2;
		amt_out_lo= g_pub_tx.tx_amt3;
		amt_cmpd_lo= g_pub_tx.tx_amt4;
		pri_end_date = g_ln_mst.ic_date;
		
		sprintf(acErrMsg,"pay_amt[%.2lf][%.2lf][%.2lf][%.2lf]",
		g_pub_tx.tx_amt1,g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4 );
		WRITEMSG
		
		/*写贷款登记簿*/
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",
									g_pub_tx.tx_amt2,g_ln_mst.in_lo_acm,"0",1 );
		if( ret ) goto ErrExit;
		in_lo_beg_date = g_pub_tx.acm_beg_date ;
		
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",
								g_pub_tx.tx_amt3,g_ln_mst.out_lo_acm,"0",1 );
		if( ret ) goto ErrExit;
		out_lo_beg_date = g_pub_tx.acm_beg_date ;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",
								g_pub_tx.tx_amt4,g_ln_mst.cmpd_lo_acm,"0",1 );
		if( ret ) goto ErrExit;
		cmpd_lo_beg_date = g_pub_tx.acm_beg_date ;	
					
		set_zd_double( "108F" , g_pub_tx.tx_amt2 );	/* 表内 */
		set_zd_double( "108H" , g_pub_tx.tx_amt3 );	/* 表外 */
		set_zd_double( "108I" , g_pub_tx.tx_amt4 );	/* 复利 */			
		set_zd_double( "108L" , g_pub_tx.tx_amt4+g_pub_tx.tx_amt3 );/* 表外+复利 记入利息科目 */			
		
		/* Pub_tx结构赋值 */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
		strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
		strcpy ( g_pub_tx.prt_ind , "0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.hst_ind , "1" );/*入明细:1日间入2日终单笔入
												3日终汇总入*/
		g_pub_tx.svc_ind = 3050;			/* 贷款利息核销 */	
		strcpy ( g_pub_tx.brf,"贷款利息核销" );
	
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
		
		/*打印程序*/
		/* 行内传票--表内利息部分 */
		if( pub_base_CompDblVal(g_pub_tx.tx_amt2, 0.00))	
		{
			print_flag[0]='1';
			ret = print_LNHXBN( );			
			if(ret)
			{
				sprintf(acErrMsg,"print_LNHXBN,失败请检查!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L207");
				goto ErrExit;
			}	
			TRACE	
		}else{
			print_flag[0]='*';
		}		
		/* 行内传票--表外利息部分 */
		if( pub_base_CompDblVal(g_pub_tx.tx_amt3, 0.00))	
		{		
			print_flag[1]='1';
			ret = print_LNHXBW( );			
			if(ret)
			{
				sprintf(acErrMsg,"print_LNHXBW,失败请检查!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L207");
				goto ErrExit;
			}	
			TRACE
		}else{
			print_flag[1]='*';
		}
		/* 行内传票--复利利息部分 */
		if( pub_base_CompDblVal(g_pub_tx.tx_amt4, 0.00))	
		{
			print_flag[2]='1';	
			ret = print_LNHXFL( );			
			if(ret)
			{
				sprintf(acErrMsg,"print_LNHXFL,失败请检查!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L207");
				goto ErrExit;
			}	
			TRACE	
		}else{
			print_flag[2]='*';
		}				
	}
	/*暂未考虑按揭核销*/
	if( 1==0 )
	{
		/* 取还款计划表中，本期当还期数 */
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &pri_ln_pay_pln , "ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id , g_ln_mst.ac_seqn );
		if( ret ) {return ret;}
		
		return 0;
		/* 按揭贷款 */	
		ret = Ln_lo_Dec_Upd ( g_pub_tx.reply , 
					"ac_id=%ld and ac_seqn=%d  and pay_type='0'", 
					 g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "Ln_lo_Dec_Sel错!ret=[%d]", ret);
			WRITEMSG		
			goto ErrExit;	
		}
		sprintf( acErrMsg, "ac_id[%ld]ac_seqn=[%d]", 
								g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		WRITEMSG				

		while(1)
		{
			ret = Ln_lo_Fet_Upd( &ln_lo, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		sprintf( acErrMsg,"fetch Ln_lo_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		goto ErrExit;
	 		}else if( ret==100 ){
				TRACE
				break;	
			}
			TRACE
		 	sprintf( acErrMsg,"进入还欠款部分！！！");
		 	WRITEMSG
			/* 还款处理 */
			/* 老系统数据，先预提了已经入表内。现需要记 贷133 贷705*/
  			/* 还款处理--只处理所欠利息 */
			amt_tmp = ln_lo.pay_lo_intst + g_pub_tx.tmp_amt1 ;
			 
			/* 还款金额在归还本期后,还有剩余 */
			if( pub_base_CompDblVal(ln_lo.lo_intst, amt_tmp)<=0 )
			{
				g_pub_tx.tmp_amt1=g_pub_tx.tmp_amt1-
										(ln_lo.lo_intst-ln_lo.pay_lo_intst );
				ln_lo.pay_lo_intst = ln_lo.lo_intst; /* 已归还金额 */	
				sprintf( acErrMsg,"欠款金额小于(已归还金额+还款金额),\
									剩余金额[%lf]",g_pub_tx.tmp_amt1);
		 		WRITEMSG
			
	           	/* 表内 */
	           	if( ln_lo.old_ac_ind[0]=='1' )
	           	{
	           		ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
						ln_lo.shld_pay_date,"2",ln_lo.lo_intst,0.00,"0",2 );
					if( ret ) 	return 1;
					amt1 += ln_lo.lo_intst - ln_lo.pay_lo_intst;
	            }
	           	/* 表外 */
	            if( ln_lo.old_ac_ind[0]=='2' )
	            {
	            	ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
						ln_lo.shld_pay_date,"3",ln_lo.lo_intst,0.00,"0",2 );
					if( ret ) 	return 1;	
					amt2 += ln_lo.lo_intst - ln_lo.pay_lo_intst;
	   	    	}
			}else{	
				/* 刚好归还本期或者不能还清本期 */
				g_pub_tx.tmp_amt1= 0.00;
				/* 已归还金额 */
				ln_lo.pay_lo_intst=ln_lo.pay_lo_intst+g_pub_tx.tmp_amt1;
				sprintf( acErrMsg,"欠款金额大于等于(已归还金额+还款金额),\
										剩余金额[%lf]",g_pub_tx.tmp_amt1);
		 		WRITEMSG
	            /* 表内 */
	           	if( ln_lo.old_ac_ind[0]=='1' )
	            {
	            	amt1+=g_pub_tx.tmp_amt1;
	            }
	            /* 表外 */
	            if( ln_lo.old_ac_ind[0]=='2' )
	            {	
	            	amt2+=g_pub_tx.tmp_amt1;	
	            }            	    				 		
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
		
		/* 比较还款 */	
		g_pub_tx.tmp_amt1 = pub_base_PubDround_1(g_pub_tx.tmp_amt1);
		if ( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
		{
			sprintf(acErrMsg,"还款金额与应还金额不符[%lf]",g_pub_tx.tmp_amt1);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L046");
			ERR_DEAL				
		}
		
		set_zd_double( "1085" , amt1 );
		set_zd_double( "1086" , amt2 );
		
		/* 记账处理 */
	    g_pub_tx.tx_amt2 = amt1;
	    g_pub_tx.tx_amt4 = amt2 ;   
	           
	    /* 记账----只有老系统数据罚息和复利独立记账 */
	    if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )        
		{
			g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
			strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
			strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
			strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
			strcpy (g_pub_tx.prt_ind,"0");/*是否打印存折(有折无折标志):1是0否*/
			strcpy ( g_pub_tx.hst_ind , "1" );/*入明细:1日间入2日终单笔入
														3日终汇总入*/
			g_pub_tx.svc_ind = 3071 ;	
			strcpy ( g_pub_tx.brf,"老数据按揭利息部分" );
			sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
			WRITEMSG
			
			/*** 交易记帐处理(老数据利息)***/
			if( pub_acct_trance() )
			{
				sprintf(acErrMsg,"交易记帐处理错误!");
				WRITEMSG
				goto ErrExit;
			}
			sprintf(acErrMsg,"交易记帐处理(老数据利息) PASS!");
			WRITEMSG	                
	     }
	     if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )        
		 {       
			g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
			strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
			strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
			strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
			strcpy (g_pub_tx.prt_ind,"0");/*是否打印存折(有折无折):1是0否*/
			strcpy ( g_pub_tx.hst_ind , "1" );/*入明细:1日间入2日终单笔入
															3日终汇总入*/
			g_pub_tx.svc_ind = 3072 ;	
			strcpy ( g_pub_tx.brf,"老数据按揭利息部分" );
			sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
			WRITEMSG
			/*** 交易记帐处理(老数据罚息)***/			
			if( pub_acct_trance() )
			{
				sprintf(acErrMsg,"交易记帐处理错误!");
				WRITEMSG
				goto ErrExit;
			}
			sprintf(acErrMsg,"交易记帐处理(老数据罚息) PASS!");
			WRITEMSG
		  }
	}
	/** begin 开始处理信贷发起的审批记录 add by chenchao 2011-8-7 11:14:49 **/
	ret = Ln_auth_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and type='11' and sts in ('X','B') ",g_ln_mst.pact_no);
	if(ret)
	{
		sprintf( acErrMsg, "Ln_auth_Dec_Upd出错!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;	
	}
	ret = Ln_auth_Fet_Upd(&sLn_auth,g_pub_tx.reply);
	if(ret && ret != 100)
	{
		sprintf( acErrMsg, "Ln_auth_Fet_Upd出错!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;	
	}else if (ret == 100)
	{
		sprintf( acErrMsg, "没有找到审批记录!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L175");
		goto ErrExit;
	}
	if(sLn_auth.sts[0] == 'B' || g_ln_mst.bal < 0.005 || sLn_auth.amt<0.005)
	{
		sLn_auth.sts[0] = '1';			/** 本金也已经核销,置为核销状态 **/
	}else
	{
		sLn_auth.sts[0] = 'L';			/** 本金还未核销,置为利息核销状态 **/
	}
	ret =  Ln_auth_Upd_Upd(sLn_auth,g_pub_tx.reply);
	if(ret)
	{
		sprintf( acErrMsg, "更新审批记录出错!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L175");
		goto ErrExit;
	}
	Ln_auth_Clo_Upd();
	/** end 处理信贷发起的审批记录 add by chenchao 2011-8-7 11:14:49 **/
	set_zd_data("0630",print_flag);
	
OkExit:
    set_zd_data("0210",g_ln_parm.cur_no);/* 对币种赋值 */
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款利息核销[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款利息核销[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&ln_lo, 0x00, sizeof(struct ln_lo_c)); 		
	memset (&ln_reg, 0x00, sizeof(struct ln_reg_c));
	memset (&pri_ln_pay_pln, 0x00, sizeof(struct ln_pay_pln_c));
	return 0;
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("1081",g_pub_tx.ac_no);			/* 帐号 */
	get_zd_int("1082",&g_pub_tx.ac_seqn);			/* 账户序号 */

	get_zd_double("108B",&g_pub_tx.tmp_amt1);		/* 还款金额 */
	print_tol_amt= g_pub_tx.tmp_amt1;
	get_zd_data("108C",g_pub_tx.note_type);			/* 凭证种类 */
	get_zd_data("108D",g_pub_tx.beg_note_no);		/* 起始凭证号码 */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *检查介质状态*
	if( ret ) return ret; */
					
	return 0;
}

/* 打印--贷款核销传票--表内利息部分 */
int print_LNHXBN( )
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
		sprintf(acErrMsg,"LNHXBN打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "表内利息" );		
	/* 确定起始日期 */
	start_day= in_lo_beg_date;
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
	
	/* 交易金额 */
	tol_amt = amt_in_lo;

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
	fprintf(fp,"LNHXBN%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
/* 打印--贷款核销传票--表外利息部分 */
int print_LNHXBW( )
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
		sprintf(acErrMsg,"LNHXBW打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "表外利息" );			
	/* 确定起始日期 */
	start_day= out_lo_beg_date;
		
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
	
	/* 交易金额 */
	tol_amt = amt_out_lo;

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
	fprintf(fp,"LNHXBW%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
/* 打印--贷款核销传票--复利利息部分 */
int print_LNHXFL( )
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
		sprintf(acErrMsg,"LNHXFL打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "复利利息" );		
	/* 确定起始日期 */
	start_day= cmpd_lo_beg_date;
		
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
	
	/* 交易金额 */
	tol_amt = amt_cmpd_lo;

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
	fprintf(fp,"LNHXFL%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
