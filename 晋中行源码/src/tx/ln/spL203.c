/*************************************************
* 文 件 名: spL203.c
* 功能描述：   
*              按揭贷款还款
*
* 作    者:    bob
* 完成日期：   2003年01月19日
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
static struct ln_pay_pln_c	v_ln_pay_pln;
static struct ln_pay_pln_c	pri_ln_pay_pln;
static struct S_ln_intst vln_intst[LN_INTST_TYPS];
int cnt=0;
int payed_cnt=0;

char ct_flag[2];
double print_tol_amt=0.00;
double tx_amt1=0.00 , tx_amt2=0.00 , tx_amt3=0.00 , tx_amt4=0.00 ;
double tx_amt2_new=0.00 , tx_amt2_old=0.00 , tx_amt3_old=0.00;
long   ln_bj_beg_date=0, ln_bj_end_date=0;
long   ln_lx_beg_date=0, ln_lx_end_date=0;
long   ln_fx_beg_date=0, ln_fx_end_date=0;	

spL203()  
{ 	
	double amt_tmp=0.00,amt_lo=0.00,pay_amt=0.00;
	double pun_intst=0.00,cmpd_intst_aj=0.00;
	char sts[2];
	char amt_str[5];
	int ret=0,i=0,flag=0,pay_flag=0;
	double old_amt1=0.00 , old_amt2=0.00, old_amt3=0.00 , old_amt4=0.00;
	char clear_sts[2];
	int ins_rol_ind=0;	
	
	int ttl_cnt=0,curr_cnt=0;
	double bal=0.00,lo_tol_amt=0.00,lo_pay_amt=0.00;
		
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
	/* 取欠款表数据*/
	ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if(ret)	goto ErrExit;
	ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if(ret)	goto ErrExit;	
	/* 更新按揭欠款表 */
 	ins_rol_ind=0;	 	/* 登记撤销辅助表标志(0-登记 非0-不登记) */	
	ret = Ln_lo_Dec_Upd ( g_pub_tx.reply , 
				"ac_id=%ld and ac_seqn=%d  and pay_type='0'", 
				g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "Ln_lo_Dec_Sel错!ret=[%d]", ret);
		WRITEMSG		
		goto ErrExit;	
	}
	sprintf(acErrMsg,"ac_id[%ld]ac_seqn=[%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
	WRITEMSG				
	flag=0;	/* 共欠期数 */
	TRACE
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
 		/* 登记按揭欠款撤销登记簿 */
 		ret = Ins_ln_lo_rol( ln_lo );
 		if( ret )
 		{
	 		sprintf( acErrMsg,"调用登记按揭欠款撤销登记簿函数异常出错!");
	 		WRITEMSG
	 		goto ErrExit;
		}
 		/***
 		ins_rol_ind++;	 			
	 	*****/
	 	
		/* 还款处理 */
		if(ln_lo.old_ac_ind[0]=='1'||ln_lo.old_ac_ind[0]=='2' )	
		/* 老系统数据，先预提了已经入表内或者表外。现需要记 贷133 贷705*/
		{
	 		sprintf(acErrMsg,"进入老数据欠息【金额【%lf】!",g_pub_tx.tmp_amt1);
	 		WRITEMSG
			/* 计算罚息金额 */
			/****
			ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
			   ln_lo.lo_amt-ln_lo.pay_lo_amt,ln_lo.lo_intst-ln_lo.pay_lo_intst,
			   ln_lo.shld_pay_date,g_pub_tx.tx_date,&pun_intst,&cmpd_intst_aj);  
			   ****/
			ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
			   ln_lo.lo_amt,ln_lo.lo_intst,
			   ln_lo.shld_pay_date,g_pub_tx.tx_date,&pun_intst,&cmpd_intst_aj); 			             	
	        if (ret) goto ErrExit;
	 		sprintf(acErrMsg,"【%lf】!【%lf】",pun_intst,cmpd_intst_aj);
	 		WRITEMSG	                	
	        /*调用还款计划函数确定各项欠息的还款金额*/
	        i=0;
	        vln_intst[i].intst_amt=ln_lo.lo_amt-ln_lo.pay_lo_amt; 
			vln_intst[i].type=0;
	        i++;
	        vln_intst[i].intst_amt=ln_lo.lo_intst-ln_lo.pay_lo_intst; 
			vln_intst[i].type=1;
	        i++;
	        vln_intst[i].intst_amt=ln_lo.pun_intst+(pun_intst+cmpd_intst_aj)-
									ln_lo.pay_pun_intst;
			vln_intst[i].type=2;
	        i++;
	        ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
											&g_pub_tx.tmp_amt1 );
	        if( ret ) goto ErrExit;
	                
	 		sprintf( acErrMsg,"老数据欠息【金额【%lf】】!",g_pub_tx.tmp_amt1);
	 		WRITEMSG	
	 		               
	        old_amt1=vln_intst[0].pay_amt;
	        if( ln_lo.old_ac_ind[0]=='1' )
	        {
		 		sprintf( acErrMsg,"!!!!!!!lance!old_amt2=[%lf]",old_amt2);
		 		WRITEMSG	                	
	        	old_amt2+=vln_intst[1].pay_amt;
		 		sprintf( acErrMsg,"!!!!!!!lance!old_amt2=[%lf]",old_amt2);
		 		WRITEMSG
	        }else if( ln_lo.old_ac_ind[0]=='2' ){
	            old_amt3+=vln_intst[1].pay_amt;	                	
	        }
	        if( pub_base_CompDblVal(vln_intst[2].pay_amt,ln_lo.pun_intst )>0)
	        {
	           	old_amt4+= ln_lo.pun_intst;
	        }else{
	           	old_amt4+= vln_intst[2].pay_amt;
	        }	                
		    ln_lo.pay_lo_amt+=vln_intst[0].pay_amt;
		    ln_lo.pay_lo_intst+=vln_intst[1].pay_amt;
		    ln_lo.pay_pun_intst+=vln_intst[2].pay_amt;
	 		sprintf( acErrMsg,"pay_lo_amt【金额【%lf】】ln_lo.lo_amt=[%lf]!",
									ln_lo.pay_lo_amt,ln_lo.lo_amt);
	 		WRITEMSG
	 		sprintf( acErrMsg,"pay_lo_intst【金额【%lf】ln_lo.lo_intst=[%lf]!",
									ln_lo.pay_lo_intst,ln_lo.lo_intst);
	 		WRITEMSG
	 		sprintf( acErrMsg,"pay_pun_intst【金额【%lf】ln_lo.pun_intst=[%lf]\
						+++[%lf]",ln_lo.pay_pun_intst,ln_lo.pun_intst,
						pun_intst+cmpd_intst_aj);
	 		WRITEMSG	 			 				            

		    pay_flag=0;    /*是否还清标志*/
	        ret = pub_base_CompDblVal(ln_lo.lo_amt, ln_lo.pay_lo_amt );
	        if( ret ) pay_flag++;  
	        ret = pub_base_CompDblVal(ln_lo.lo_intst, ln_lo.pay_lo_intst );
	        if( ret ) pay_flag++;             
	        ret = pub_base_CompDblVal(pun_intst+cmpd_intst_aj,
												ln_lo.pay_pun_intst);
	        if( ret ) pay_flag++;
	        if( pay_flag==0 )     
	        {
	            TRACE
	            /* 修改是否还清标志 0-未还清 1-还清 */
	            strcpy( ln_lo.pay_type , "1" );
	            flag++;
	            /* 需要登记贷款登记簿 */
	            /* 本金 
	            ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,
												"1",ln_lo.lo_amt,0.00,"0",1 );
			    if( ret ) 	goto ErrExit;	
		    */            	    
	            /* 利息 */
	          	/* 表内 */
	           	/*********
	           	if( ln_lo.old_ac_ind[0]=='1' )
	           	{
	           		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
							,g_pub_tx.tx_date,"2",ln_lo.lo_intst,0.00,"0",2 );
					if( ret ) 	goto ErrExit;	
	            }
	            ******/
	            /* 表外 */
	            /*******
	            if( ln_lo.old_ac_ind[0]=='2' )
	            {
	            	ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
							,g_pub_tx.tx_date,"3",ln_lo.lo_intst,0.00,"0",2 );
					if( ret ) 	goto ErrExit;	
	           	}
	           ************/	          	    	
	            /* 不区分新老系统 只记一笔 */
	            /* 罚息 *
	           		* 表内 *
	             	if( ln_lo.old_ac_ind[0]=='1' )
	              	{
	              		ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
							g_pub_tx.tx_date,"2",ln_lo.pun_intst +
							(pun_intst+cmpd_intst_aj),0.00,"0",2 );
						if( ret ) 	goto ErrExit;	
	              	}
	               	* 表外 *
	               	if( ln_lo.old_ac_ind[0]=='2' )
	               	{
	               		ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
							g_pub_tx.tx_date,"3",ln_lo.pun_intst + 
							(pun_intst+cmpd_intst_aj),0.00,"0",2 );
						if( ret ) 	goto ErrExit;	
	              	}	
	           	********/	            	    
	       	}            
	                
	        /* 需要按新的记账方式记账的金额 */
	        g_pub_tx.tx_amt1+=old_amt1;
	        if( pub_base_CompDblVal(vln_intst[2].pay_amt,ln_lo.pun_intst )>0 )
	        {
	           	/* 除了还老系统金额外，也还了新算的复息和罚息 */ 
	           	g_pub_tx.tx_amt4 = g_pub_tx.tx_amt4 + 
								(vln_intst[2].pay_amt - ln_lo.pun_intst) ;
	        }  
	
	    }else if( ln_lo.old_ac_ind[0]=='0' ){
	 			sprintf(acErrMsg,"新数据欠息【金额【%lf】!",g_pub_tx.tmp_amt1);
	 			WRITEMSG			
				/* 新系统数据处理方式 */
				/* 计算罚息金额 */
				/****
				ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
					ln_lo.lo_amt-ln_lo.pay_lo_amt ,
					ln_lo.lo_intst-ln_lo.pay_lo_intst , ln_lo.shld_pay_date , 
					g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);  
				*******/
				
				ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
					ln_lo.lo_amt ,
					ln_lo.lo_intst , ln_lo.shld_pay_date , 
					g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);  
								          
	            if (ret) break;
	            /*调用还款计划函数确定各项欠息的还款金额*/
				memset( vln_intst,0,sizeof(vln_intst) );
	            i=0;
	            vln_intst[i].intst_amt=ln_lo.lo_amt-ln_lo.pay_lo_amt; 
				vln_intst[i].type=0;
	            i++;
	            vln_intst[i].intst_amt=ln_lo.lo_intst-ln_lo.pay_lo_intst; 
				vln_intst[i].type=1;
	            i++;
	            vln_intst[i].intst_amt=pun_intst+cmpd_intst_aj-
											ln_lo.pay_pun_intst; 
				vln_intst[i].type=2;
	            i++;
	            ret=pub_ln_OrderPay(&vln_intst,g_ln_parm.pay_seq_code,
														&g_pub_tx.tmp_amt1 );
	           	if( ret ) goto ErrExit;
		 		sprintf( acErrMsg,"新数据欠息【金额【%lf】!",g_pub_tx.tmp_amt1);
	 			WRITEMSG
	 		
	            g_pub_tx.tx_amt1+=vln_intst[0].pay_amt;
	            g_pub_tx.tx_amt2+=vln_intst[1].pay_amt;
	            g_pub_tx.tx_amt4+=vln_intst[2].pay_amt; 
	            
	            ln_lo.pay_lo_amt+=vln_intst[0].pay_amt;
	            ln_lo.pay_lo_intst+=vln_intst[1].pay_amt; 
	            ln_lo.pay_pun_intst+=vln_intst[2].pay_amt;
	                
	 			sprintf( acErrMsg,"pay_lo_amt【金额【%lf】ln_lo.lo_amt=[%lf]!",
											ln_lo.pay_lo_amt,ln_lo.lo_amt);
	 			WRITEMSG
	 			sprintf(acErrMsg,"pay_lo_intst金额【%lf】ln_lo.lo_intst=[%lf]!",
											ln_lo.pay_lo_intst,ln_lo.lo_intst);
	 			WRITEMSG
	 			sprintf(acErrMsg,"pay_pun_intst【金额【%lf】】ln_lo.pun_intst\
								=[%lf]++[%lf]!",ln_lo.pay_pun_intst,
								ln_lo.pun_intst,pun_intst+cmpd_intst_aj);
	 			WRITEMSG
	 			            
	            pay_flag=0;    /*是否还清标志*/
	            ret = pub_base_CompDblVal(ln_lo.lo_amt, ln_lo.pay_lo_amt );
	            if( ret ) pay_flag++;  
	            ret = pub_base_CompDblVal(ln_lo.lo_intst, ln_lo.pay_lo_intst );
	            if( ret ) pay_flag++;             
	            ret = pub_base_CompDblVal( pun_intst+cmpd_intst_aj , 
														ln_lo.pay_pun_intst );
	            if( ret ) pay_flag++;
	            if( pay_flag==0 )     
	            {
	                TRACE
	                /* 修改是否还清标志 0-未还清 1-还清 */
	                strcpy( ln_lo.pay_type , "1" );
	                flag++;
	                /* 登记贷款登记簿 */
	                /* 贷款登记簿--本金 *
	                ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
							ln_lo.shld_pay_date,"1",ln_lo.lo_amt,0.00,"0",1 );
			   		if( ret ) 	goto ErrExit;
	            	* 贷款登记簿--利息 *
	            	ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
						ln_lo.shld_pay_date,"2",ln_lo.lo_intst,0.00,"0",2 );
			   		if( ret ) 	goto ErrExit;	
	            	* 贷款登记簿--罚息 *
	            	ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
					   g_pub_tx.tx_date,"4",pun_intst+cmpd_intst_aj,0.00,"0",2);
			   	 	if( ret ) 	goto ErrExit;		
			    	*************/	                	    
	            } 	        
	        }
            ret = Ln_lo_Upd_Upd(ln_lo, g_pub_tx.reply  );
            if ( ret )
            {
                 sprintf( acErrMsg,"Ln_lo_Upd_Upd 出错[%d]",ret);
	             WRITEMSG
                 goto ErrExit;
            }
            memset( &ln_lo , 0x00 , sizeof( struct ln_lo_c ) );	                
            memset( vln_intst,0,sizeof(vln_intst) );        

/***************************                                                                                                        
		* 若欠款金额大于(已归还金额+还款金额) *
		amt_tmp = ln_lo.pay_amt + g_pub_tx.tmp_amt1 ;
		amt_lo = ln_lo.lo_amt + ln_lo.lo_intst + ln_lo.pun_intst ;
		
		if ( pub_base_CompDblVal( amt_lo , amt_tmp ) == -1 )
		{
			g_pub_tx.tx_amt1+= ln_lo.lo_amt;
			g_pub_tx.tmp_amt1-=(amt_tmp-amt_lo);

			ln_lo.pay_amt = amt_lo ;* 已归还金额 *
			strcpy( sts,"1" );
			
			sprintf( acErrMsg,"欠款金额小于(已归还金额+还款金额)[%lf]",g_pub_tx.tmp_amt1);
	 		WRITEMSG
		}		
		else if ( pub_base_CompDblVal( amt_lo , amt_tmp ) == 1 )
		{
			g_pub_tx.tmp_amt1 = 0;
			ln_lo.pay_amt = amt_tmp ;	* 已归还金额 *
			strcpy( sts,"0" );

			sprintf( acErrMsg,"欠款金额大于(已归还金额+还款金额)[%lf]",g_pub_tx.tmp_amt1);
	 		WRITEMSG
			break;
		}
		* 若欠款金额等于(已归还金额+还款金额) *		
		if ( pub_base_CompDblVal( amt_lo , amt_tmp ) == 0 )
		{
			g_pub_tx.tx_amt1+= ln_lo.lo_amt;
			g_pub_tx.tmp_amt1 = 0;
			ln_lo.pay_amt = amt_tmp ;	* 已归还金额 *
			strcpy( sts,"1" );
			
			sprintf( acErrMsg,"欠款金额等于(已归还金额+还款金额)[%lf]",g_pub_tx.tmp_amt1);
	 		WRITEMSG
			break;
		}	
	
		* 更新 已归还金额,欠款状态 *
		ret = sql_execute( "UPDATE ln_lo SET pay_amt=%lf,sts='%s' WHERE ac_id=%ld and ac_seqn=%d and pay_type='0' " , 
			ln_lo.pay_amt,sts,ln_lo.ac_id,ln_lo.ac_seqn );
		if( ret ) return ret;
**************************/		
	}
	Ln_lo_Clo_Upd ();
	sprintf( acErrMsg,"!!!!!cnt[%d]flag[%d]!!!!!!!",cnt,flag);
	WRITEMSG	
	sprintf( acErrMsg,"欠息结束剩余【金额【%lf】】!",g_pub_tx.tmp_amt1);
	WRITEMSG
	if ( (cnt-flag)<=0 )
	{
		if ( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
		{
			sprintf( acErrMsg,"金额多余需要还款金额[%lf]",g_pub_tx.tmp_amt1);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply,"L039" );
			goto ErrExit;
		}
	}
	sprintf( acErrMsg,"!!!!!cnt[%d]flag[%d]",cnt,flag);
	WRITEMSG
	
		
	if (cnt-flag)
	{
		ins_rol_ind=0;	 	/* 登记撤销辅助表标志(0-登记 非0-不登记) */	
 	
		/* 取贷款还款计划表信息	*/
		ret = Ln_pay_pln_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d"
					 , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Ln_pay_Dec_Upd错!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}
		ret = Ln_pay_pln_Fet_Upd( &ln_pay_pln, g_pub_tx.reply );
		if( ret==100)
		{
			sprintf(acErrMsg,"按揭还款计划表中无相应记录",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L044");
			ERR_DEAL;
		}else if ( ret ){
			sprintf(acErrMsg,"取按揭还款计划表信息异常",g_pub_tx.reply);
			WRITEMSG
			ERR_DEAL;
		} 

		/* 登记撤销辅助表 */
 		if( !ins_rol_ind )
 		{
 			/* 登记按揭欠款撤销登记簿 */
 			ret = Ins_ln_pay_pln_rol( ln_pay_pln );
 			if( ret )
 			{
	 			sprintf(acErrMsg,"调用登记按揭还款计划撤销登记簿函数异常出错!");
	 			WRITEMSG
	 			goto ErrExit;
			}
		}
		ins_rol_ind++;	
		i=ln_pay_pln.curr_cnt;
		
		ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*总期数*/
		curr_cnt= ln_pay_pln.curr_cnt;		/*当前期数*/

		bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);			/*剩余本金*/
				
		while(i<=(ln_pay_pln.curr_cnt+cnt-flag))/*当前期数+还几期数-欠几期*/
		{
			ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt,bal, &v_ln_pay_pln );

			/****
			ret = pub_ln_PayPlan_qry( g_ln_mst , g_ln_parm , i , &v_ln_pay_pln ,0);	
			*****/
			if (ret)
			{
				sprintf(acErrMsg,"调用函数pub_ln_PayPlan错误!");
				WRITEMSG
				ERR_DEAL	
			}
			if ( i<(ln_pay_pln.curr_cnt+cnt-flag) )
			{
				sprintf(acErrMsg,"v_ln_pay_pln.curr_amt=[%lf],\
						v_ln_pay_pln.curr_intst=[%lf]!",v_ln_pay_pln.curr_amt,
						v_ln_pay_pln.curr_intst);
				WRITEMSG			
				pay_amt=v_ln_pay_pln.curr_amt+v_ln_pay_pln.curr_intst;
				g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1 - pay_amt ;
			}
		
			i++;
			bal=bal-v_ln_pay_pln.curr_amt;
			curr_cnt++;
			ttl_cnt--;			
			
			if ( i <= (ln_pay_pln.curr_cnt+cnt-flag) )
			{
				g_pub_tx.tx_amt1+= v_ln_pay_pln.curr_amt;
				g_pub_tx.tx_amt2+= v_ln_pay_pln.curr_intst;
			
	           	/* 贷款登记簿--本金 *
	           	ret = pub_ln_ln_reg( g_ln_mst,v_ln_pay_pln.end_date,
					v_ln_pay_pln.end_date,"1",v_ln_pay_pln.curr_amt,0.00,"0",1);
				if( ret ) 	goto ErrExit;
	           	* 贷款登记簿--利息 *		
			ret = pub_ln_ln_reg( g_ln_mst,v_ln_pay_pln.end_date,v_ln_pay_pln.end_date,"2",v_ln_pay_pln.curr_intst,0.00,"0",2 );
			if( ret ) 	goto ErrExit;	
								**************/
			}
		
		WRITEMSG

		strcpy(v_ln_pay_pln.rowid, ln_pay_pln.rowid);

		if ( i< ln_pay_pln.ttl_cnt )
		{
			/* 修改还款计划 */
			ret = Ln_pay_pln_Upd_Upd( v_ln_pay_pln , g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"更新还款计划失败![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L045");
				goto ErrExit;
			}
			
			sprintf(acErrMsg,"更新下一期还款计划表 PASS!");
			WRITEMSG						
			memset(&v_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );		
		}else if (i==ln_pay_pln.ttl_cnt ){
			/* 修改还款计划 */
			v_ln_pay_pln.pay_ind[0] = '1'; 
			ret = Ln_pay_pln_Upd_Upd( v_ln_pay_pln , g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"更新还款计划失败![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L045");
				goto ErrExit;
			}			
			sprintf(acErrMsg,"更新下一期还款计划表 PASS!");
			WRITEMSG						
			memset(&v_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );	
		}
	}
	
	Ln_pay_pln_Clo_Upd();
}
		
	/* 比较还款 */	
	g_pub_tx.tmp_amt1 = pub_base_PubDround_1(g_pub_tx.tmp_amt1);
	if ( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
	{
		sprintf(acErrMsg,"还款金额与应还金额不符[%lf]",g_pub_tx.tmp_amt1);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L046");
		ERR_DEAL				
	}
	
	sprintf(acErrMsg,"g_pub_tx.tx_amt1[%lf]",g_pub_tx.tx_amt1);
	WRITEMSG
	/* Pub_tx结构赋值 */
	set_zd_data("0210",g_ln_parm.cur_no);    
	set_zd_double("0430",g_pub_tx.tx_amt2); 
	set_zd_double("0410",g_pub_tx.tx_amt4);
	set_zd_double("0400",g_pub_tx.tx_amt1);
	

	
	sprintf(acErrMsg,"g_pub_tx.tx_amt1[%lf][%lf][%lf]",g_pub_tx.tx_amt1,g_pub_tx.tx_amt2,g_pub_tx.tx_amt4);
	WRITEMSG	
	
	/*　登记贷款登记簿,新系统数据只登记一笔 */
	/*** 起始日期问题  *****/
  /* 贷款登记簿--本金 */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"1",g_pub_tx.tx_amt1,0.00,"0",1 );
  if( ret ) 	goto ErrExit;
  /* 贷款登记簿--利息 */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"2",g_pub_tx.tx_amt2,0.00,"0",2 );
  if( ret ) 	goto ErrExit;	
  /* 贷款登记簿--罚息 */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"4",g_pub_tx.tx_amt4,0.00,"0",2 );
  if( ret ) 	goto ErrExit;			
  
	/* 本金记流水 */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt1 , 0.00 ) )        
	{   
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
		g_pub_tx.svc_ind = 3010 ;	
		strcpy ( g_pub_tx.brf,"按揭还款本金部分" );
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
	/* 利息记流水 */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )        
	{   
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
		g_pub_tx.svc_ind = 3068 ;	
		strcpy ( g_pub_tx.brf,"按揭还款利息部分" );
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
	/* 罚息记流水 */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )        
	{   
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
		g_pub_tx.svc_ind = 3069 ;	
		strcpy ( g_pub_tx.brf,"按揭还款罚息部分" );
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
	
	/* 打印用数据 */
	tx_amt1 = g_pub_tx.tx_amt1;		/* 本金 */
	tx_amt2 = g_pub_tx.tx_amt2+old_amt2+old_amt3; /* 利息 */
	tx_amt2_new = g_pub_tx.tx_amt2;
	tx_amt2_old = old_amt2;          
	tx_amt3_old = old_amt3;/*表外息*/
	tx_amt4 = g_pub_tx.tx_amt4+old_amt4;/*罚息*/

	memset( amt_str , 0x00 , sizeof( amt_str ) );
	 
	if( pub_base_CompDblVal( tx_amt1 , 0.00 ) )/* 本金 */
	{
		TRACE
		amt_str[0]='1';
	}else{
		TRACE
		amt_str[0]='*';
	}	
	if( pub_base_CompDblVal( tx_amt2_new , 0.00 ) )/* 新数据表内利息 */
	{
		TRACE
		amt_str[1]='1';
	}else{
		TRACE
		amt_str[1]='*';
	}
	if( pub_base_CompDblVal( tx_amt4 , 0.00 ) )/*罚息*/
	{
		TRACE
		amt_str[2]='1';
	}else{
		TRACE
		amt_str[2]='*';
	}
	if( pub_base_CompDblVal( tx_amt2_old , 0.00 ) )/* 旧数据表内利息 */
	{
		TRACE
		amt_str[3]='1';
	}else{
		TRACE
		amt_str[3]='*';
	}
	if( pub_base_CompDblVal( tx_amt3_old , 0.00 ) )/* 旧数据表外利息 */
	{
		TRACE
		amt_str[4]='1';
	}else{
		TRACE
		amt_str[4]='*';
	}	
	set_zd_data( "0630" , amt_str );      
	sprintf(acErrMsg,"打印参数amt_str=[%s] !",amt_str);
	WRITEMSG	
	                  
	/** 老系统数据处理部分 */
	set_zd_data("0210",g_ln_parm.cur_no);    
	set_zd_double("1001",old_amt2); 
	set_zd_double("1002",old_amt3);
	set_zd_double("1003",old_amt4);
		        

        g_pub_tx.tx_amt1 = 0.00;
        g_pub_tx.tx_amt2 = old_amt2 ;
        
	/* 表内 */
	if( pub_base_CompDblVal(old_amt2, 0.00 ) )
	{
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
			,g_pub_tx.tx_date,"2",old_amt2,0.00,"0",2 );
		if( ret ) 	goto ErrExit;	
	}
	/* 表外 */
	if( pub_base_CompDblVal(old_amt3, 0.00 ) )
	{
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
				,g_pub_tx.tx_date,"3",old_amt3,0.00,"0",2 );
		if( ret ) 	goto ErrExit;	
	}	
        /***
        g_pub_tx.tx_amt3 = old_amt3 ;
        ****/
        g_pub_tx.tx_amt4 = old_amt4+old_amt3 ;   
           
        /* 记账----只有老系统数据罚息和复利独立记账 */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )        
	{
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
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
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
		g_pub_tx.svc_ind = 3072 ;	
		strcpy ( g_pub_tx.brf,"老数据按揭罚息部分" );
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

	/* 客户回单--按揭还款 */
	ret = print_LNHDAJ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHDAJ,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
	
	/* 行内传票--按揭还款(本金) */
	ret = print_LNBKAJBJ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJBJ,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
	
	/* 行内传票--按揭还款(利息) */
	ret = print_LNBKAJLX( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJLX,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
	
	/* 行内传票--按揭还款(罚息) */
	ret = print_LNBKAJFX( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJFX,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
/* 打印--行内传票--旧数据表内利息部分 */
	ret = print_LNBKAJBN( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJBN,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
/* 打印--行内传票--旧数据表外利息部分 */
	ret = print_LNBKAJBW( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJFW,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE		
	
	pub_mob_sendmail_ln("按揭贷款归还本金",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);
	
					
	if( ct_flag[0]=='1' )	/** 现金 **/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*转账*/
		goto ErrExit2;
    else if (ct_flag[0] == '3')		/*提出*/
        goto ErrExit3;
    else if (ct_flag[0] == '8')		/*扣划*/
        goto ErrExit8;

ErrExit:
    sprintf(acErrMsg,"按揭还款失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"按揭还款成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"按揭还款成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"按揭还款成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
ErrExit8:
    strcpy(g_pub_tx.reply,"0008");
    sprintf(acErrMsg,"按揭还款成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
  	memset( vln_intst,0,sizeof(vln_intst) );   	
	memset(&ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&v_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&pri_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;
	struct ln_mst_hst_c 	S_ln_mst_hst;
	memset( &S_ln_mst_hst,0x00 , sizeof( struct ln_mst_hst_c));

	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("1091",g_pub_tx.ac_no);		/* 帐号 */
	get_zd_int("1092",&g_pub_tx.ac_seqn);		/* 账户序号 */
	get_zd_int("1099",&cnt);			/* 偿还期数 */
	get_zd_double("109C",&g_pub_tx.tmp_amt1);	/* 还款金额 */
	print_tol_amt = g_pub_tx.tmp_amt1;
	get_zd_data("109A",g_pub_tx.note_type);		/* 凭证种类 */
	get_zd_data("109B",g_pub_tx.beg_note_no);	/* 起始凭证号码 */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );

	get_zd_int("109F",&payed_cnt);			/* 已还期数 */
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
		sprintf(acErrMsg,"按揭贷款不在此处理![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L038" );
		return 1;
	}
	TRACE
	/* 限制旧数据中提前还款账号做业务 
	if( g_ln_mst.opn_date!=g_ln_mst.ic_date )
	{
		sprintf(acErrMsg,"限制旧数据中提前还款账号做业务[%ld]",g_ln_mst.ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L043" );
		return 1;
	}
	**/
	/* 限制旧数据中转行账号做业务  
	ret= Ln_mst_hst_Sel( g_pub_tx.reply, &S_ln_mst_hst, "ac_id=%ld and tx_code='2498'",g_ln_mst.ac_id );
	if( ret!=100 )
	{
		sprintf(acErrMsg,"限制旧数据中转行账号做业务[%ld]",g_ln_mst.ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L043" );
		return 1;
	}
	****/

	/* 取还款计划表中，本期当还期数 */
	ret = Ln_pay_pln_Sel( g_pub_tx.reply , &pri_ln_pay_pln , "ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id , g_ln_mst.ac_seqn );
	if( ret ) {return ret;}
	
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
	
	strcpy( tx_type , "按揭还款" );	
	
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( tx_type );
			
	/* 1.贷款账号、	     2.户名、	      3.类型、    4.总还款金额 	*/
	fprintf(fp,"LNDF%s|%s|%s|%.2lf|\n",g_pub_tx.ac_no , g_ln_mst.name , tx_type , print_tol_amt );	
	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
		
	return 0;
}
/* 打印--客户回单--按揭还款回单 */
int  print_LNHDAJ( )	
{
	struct ln_pay_pln_c	pr_ln_pay_pln;
	
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
	long end_date =0;
	int tmp_cnt=0;
	int tmp_day=0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
 	TRACE   		
	memset(&pr_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	pub_base_AllDwnFilName( tmpname );
	TRACE
	TRACE
	TRACE
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDAJ打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
  	TRACE      	
	/* 赋值部分 */
	strcpy( tx_type , "按揭还款" );		

	/* 欠本总额 */	
	tol_lo = g_ln_mst.amt_lmt ;
	
	/* 累计还本 */	
	tol_pay = g_ln_mst.ttl_prvd_amt - g_ln_mst.bal;
			
	/* 尚欠本金 */		
	tol_need_pay = g_ln_mst.bal;
	
	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(展)" );
		if( g_pub_tx.tx_date > g_ln_mst.mtr_date )
		{
			sprintf( rate , "%8.5lf‰" , g_ln_mst.exp_rate );
		}else{
			sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );	
		}			
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	}
	
	/* 应还期数 */
	tmp_cnt = pri_ln_pay_pln.curr_cnt;
	
	/* 以还期数 */	
	payed_cnt = payed_cnt + cnt;
/******
	ret = Ln_pay_pln_Sel( g_pub_tx.reply , &pr_ln_pay_pln , "ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id , g_ln_mst.ac_seqn );
	if( ret==100)
	{
		sprintf(acErrMsg,"按揭还款计划表中无相应记录",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L044");
		return 1;
	}else if ( ret )
	{
		sprintf(acErrMsg,"取按揭还款计划表信息异常",g_pub_tx.reply);
		WRITEMSG
		return 1;
	}
*****/
		
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
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.贷款本金、 10.累计还本    */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.展期标志、 15.偿还期数    */
	/* 16.偿还本金、 17.偿还利息、 18.偿还罚息、19.总期数、   20.应还期数    */
	/* 21.已还期数   22.流水号 */
	fprintf(fp,"LNHDAJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%d|%.2lf|%.2lf|%.2lf|%d|%d|%d|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , exp_ind , cnt ,
	tx_amt1 , tx_amt2 , tx_amt4 , ln_pay_pln.ttl_cnt , tmp_cnt ,
	payed_cnt , g_pub_tx.trace_no );	
	TRACE	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}

/* 打印--行内传票--本金部分 */
int print_LNBKAJBJ( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
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
	strcpy( tx_type , "本金" );	
	strcpy( intro , "归还本金" );		
	
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
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本   */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.科目号、   15.展期标志  */
	/* 16.金额说明、 15.流水号  */
	fprintf(fp,"LNBKAJBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt1 , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
	
/* 打印--行内传票--利息部分 */
int print_LNBKAJLX( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJLX打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "利息" );	
	strcpy( intro , "归还利息" );		
	
	/* 取科目号 */
	strcpy( data_code , "0158" );	/* 利息收入501 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	
	/* 欠本总额 */	
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.科目号、   15.展期标志  */
	/* 16.金额说明、 17.流水号*/
	fprintf(fp,"LNBKAJLX%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt2_new , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	

/* 打印--行内传票--罚息部分 */
int print_LNBKAJFX( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJFX打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "罚息" );	
	strcpy( intro , "归还罚息" );		
	
	/* 取科目号 */
	strcpy( data_code , "0158" );	/* 利息收入501 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	
	/* 欠本总额 */	
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.科目号、   15.展期标志  */
	/* 16.金额说明、 17.流水号*/
	fprintf(fp,"LNBKAJFX%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt4 , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
/* 打印--行内传票--旧数据表内利息部分 */
int print_LNBKAJBN( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJBN打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "表内利息(旧)" );	
	strcpy( intro , "归还利息" );		
	
	/* 取科目号 */
	strcpy( data_code , "0155" );	/* 贷133 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	
	/* 欠本总额 */	
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.科目号、   15.展期标志  */
	/* 16.金额说明、 17.流水号*/
	fprintf(fp,"LNBKAJBN%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt2_old , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
/* 打印--行内传票--旧数据表外利息部分 */
int print_LNBKAJBW( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJBW打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "表外利息(旧)" );	
	strcpy( intro , "归还利息" );		
	
	/* 取科目号 */
	strcpy( data_code , "0156" );	/* 收705 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	
	/* 欠本总额 */	
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.起始日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.正常利率、 13.罚息利率、14.科目号、   15.展期标志  */
	/* 16.金额说明、 17.流水号*/
	fprintf(fp,"LNBKAJBW%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt3_old , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
