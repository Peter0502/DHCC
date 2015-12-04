/*************************************************************
* 文 件 名: gD086.c
* 功能描述：日终－按揭贷款欠款表每日检查
*
* 作    者: lance
* 完成日期: 2003年3月30日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_holiday_c.h"
#include "ln_pay_pln_c.h"
#include "ln_lo_c.h"
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}

gD086()
{
	int cnt=0;
	int payed_cnt=0;
	int ins_rol_ind=0;
	char amt_str[5];
	char ct_flag[2];

    	int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0,pay_flag=0;
    	int    ac_seqn,parm_seqn,flag=0;

	double tx_amt1=0.00 , tx_amt2=0.00 , tx_amt3=0.00 , tx_amt4=0.00 ;
	double tx_amt2_new=0.00 , tx_amt2_old=0.00 , tx_amt3_old=0.00;
	double old_dd_bal=0.00;

	double amt_tmp=0.00,amt_lo=0.00,pay_amt=0.00;
    double amt_reg=0.00,amt=0.00,pun_intst=0.00;
	double	cmpd_intst_aj=0.00; /* 复利总数 */
    char ln_ac_no[20],dd_ac_no[20],parm_code[6],old_ac_ind[2];  
    long day_tmp=0,be_date=0;
	long	ac_id_old=0;
	double	old_amt1=0.00,old_amt2=0.00,old_amt3=0.00,old_amt4=0.00;
    struct S_ln_intst vln_intst[LN_INTST_TYPS];
    struct com_holiday_c com_holiday;
    struct ln_mst_c ln_mst_tmp;
    struct ln_lo_c ln_lo;    
	struct ln_pay_pln_c  ln_pay_pln;
	struct ln_pay_pln_c  v_ln_pay_pln;
         	 
    memset( vln_intst,0,sizeof(vln_intst) );    
    memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
    memset(&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
    memset(&ln_lo, 0x00, sizeof(struct ln_lo_c)); 
	old_amt1=old_amt2=old_amt3=old_amt4=0.00;
         
    /* 初始化全局变量 */
    pub_base_sysinit();

	ret=sql_begin(); /*打开事务*/
    if (ret != 0)
    {
        sprintf(acErrMsg,"打开事务出错[%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    
    /* 读贷款欠款表(到期日期小于等于当天,状态为未还清) */    
	/*
    ret = Ln_lo_Dec_Upd_Hold(g_pub_tx.reply," pay_type='0' and shld_pay_date<=%ld ",g_pub_tx.tx_date);
	*/
    ret = Ln_lo_Dec_Upd(g_pub_tx.reply," pay_type='0' and shld_pay_date<=%ld ",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	TRACE
	i=0;
	ac_id_old=0;
    while(1)
    {
		/*数据初始化*/
		ret=0;bj_flag=0;chg_flag=0;i=0;dc_flag=0;pay_flag=0;
		tx_amt1=0.00 ; tx_amt2=0.00 ; tx_amt3=0.00 ; tx_amt4=0.00 ;
		tx_amt2_new=0.00 ; tx_amt2_old=0.00 ; tx_amt3_old=0.00;
		old_dd_bal=0.00;

		amt_tmp=0.00;amt_lo=0.00;pay_amt=0.00;
		amt_reg=0.00;amt=0.00;pun_intst=0.00;
		cmpd_intst_aj=0.00; /* 复利总数 */
		day_tmp=0;be_date=0;
		ac_id_old=0;
		old_amt1=0.00;old_amt2=0.00;old_amt3=0.00;old_amt4=0.00;

		g_pub_tx.ac_no[0]='\0';
		g_pub_tx.ac_id=0;      
		g_pub_tx.ac_seqn=0;

		memset(dd_ac_no,0x00,sizeof(dd_ac_no));
		memset( vln_intst,0,sizeof(vln_intst) );

		g_pub_tx.tmp_amt1=0.00;
		g_pub_tx.tx_amt1=0.00;
		g_pub_tx.tx_amt2=0.00;
		g_pub_tx.tx_amt3=0.00;
		g_pub_tx.tx_amt4=0.00;

    	if(ret != 0)
  		{
			sprintf( acErrMsg, "打开事务失败!!!" );
			WRITEMSG
			goto ErrExit;
    	}
     	strcpy( g_pub_tx.tx_code, "G086");
	/*
      	ret = Ln_lo_Fet_Upd_Hold(&ln_lo, g_pub_tx.reply);
	*/
      	ret = Ln_lo_Fet_Upd(&ln_lo, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }else if (ret == 100){
			TRACE
			/*
			sql_commit();
			*/
			break;
        }   
		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
            sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
            WRITEMSG
            goto ErrExit;
		}
		TRACE
		if ( i ==0 ) ac_id_old = ln_lo.ac_id;
		i++;
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld", 
																ln_lo.ac_id);
        if ( ret == 100 )
        {
            sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
        }
        else if ( ret && ret!=100)
        {
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
        }
		TRACE
        strcpy( ln_ac_no ,g_mdm_ac_rel.ac_no );
		strcpy( g_pub_tx.ac_no , ln_ac_no );

        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
        
        /* 取贷款信息 */
        ret = pub_ln_lndis_file(ln_ac_no,ln_lo.ac_seqn, &g_mdm_ac_rel,
									&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
		if( ret ) { 
			sprintf(acErrMsg,"交易错误!%s %d",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit; }

		/*交易机构赋值*/
		strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);

		/* 判断节假日是否处理 */
		ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
		if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ) { /*sql_rollback();*/continue;}
		TRACE
	
      	/******** 取贷款状态 
        chg_flag = pub_ln_check_stsdate();
		if( chg_flag!=2 )	* 只处理正常贷款 *
		{
			continue;
		}	
		****************/
	
		/* 是否自动还款 */
		if ( g_ln_parm.auto_pay_ind[0]!='Y')
		{
			TRACE
			/*sql_rollback();*/
			continue;

		}
		/* 检查活期账号状态 */
		/* 根据币种取序号 */
		/* 根据币种生成活期户的账号ID */ 
		ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			goto ErrExit;	
		}
		ret = pub_base_currentid_file( g_ln_mst.repay_ac_id,ac_seqn,&g_dd_mst );
		if( ret )
		{
			sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
			WRITEMSG
			goto ErrExit;
		}
		TRACE
		sprintf(acErrMsg,"amt1[%lf]amt2[%lf]amt4[%lf]dd_bal[%lf]",g_pub_tx.tx_amt1,
			g_pub_tx.tx_amt2,g_pub_tx.tx_amt4,g_dd_mst.bal);
		WRITEMSG	
		TRACE	
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld order by note_sts",
															g_dd_mst.ac_id);
        if ( ret == 100 )
        {
            sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
        }
        else if ( ret && ret!=100)
        {
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
        }
		TRACE
        strcpy( dd_ac_no ,g_mdm_ac_rel.ac_no );
        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
        	
        ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
															g_dd_mst.prdt_no);
        if( ret==100 )
        {
            sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%s]",g_dd_mst.prdt_no);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P145");
            goto ErrExit;
        }else if( ret ){
            sprintf(acErrMsg,"查询活期参数表异常!!ret=[%d]",ret);
            WRITEMSG
            goto ErrExit;
        }		
		TRACE
		if( g_dd_mst.ac_sts[0]!='1' )
		{
			sprintf(acErrMsg,"账号状态非正常状态!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P109");
            goto ErrExit;
		}
		/* 账号完全冻结或者只进不出 */
		if( g_dd_mst.hold_sts[0]=='1'||g_dd_mst.hold_sts[0]=='2' )
		{
			sprintf(acErrMsg,"该账号已被冻结!");
			WRITEMSG
            goto ErrExit;
		}
		if( g_dd_parm.draw_ind[0]=='N' )
		{
			sprintf(acErrMsg,"不可续取[%s]!",g_dd_parm.draw_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A023");
			sql_rollback();
			continue;
		}
		/* 不检查(客户要求)  	
		if( g_dd_parm.thr_draw_ind[0]=='N' && strcmp(g_pub_tx.tx_br_no,g_dd_mst.opn_br_no) )
		{
			sprintf(acErrMsg,"不可通存[%s]!",g_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A011");
			sql_rollback();
			continue;
		}
		************/
		
		/* 金额检查 */
		ret = pub_base_CompDblVal(g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal,0.00);
		if( ret<=0 )
		{	
			continue;
		}else{
			g_pub_tx.tmp_amt1 = g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;
		}
	
		/* 考虑部分冻结状态 */
		if( g_dd_mst.hold_sts[0]=='3' )
		{
			ret=pub_base_CompDblVal(g_pub_tx.tmp_amt1-g_dd_mst.hold_amt , 0.00);
			if( ret<=0 )
			{
				continue;
			}else{
				g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	
			}
		}
		old_dd_bal= g_pub_tx.tmp_amt1;	/*原始可用存款户余额*/
		sprintf(acErrMsg,"原始可用存款户余额【%lf】!",old_dd_bal);
		WRITEMSG


		TRACE

		/* 更新贷款主文件 */
            /*计算罚息*/
            /* 若是旧系统数据 */
vtcp_log("AAAAAAAAA[%d][%d][%s]",ln_lo.shld_pay_date,g_pub_tx.tx_date,ln_lo.old_ac_ind);
		/* 还款处理 */
		if(ln_lo.old_ac_ind[0]=='1'||ln_lo.old_ac_ind[0]=='2' )	
		/* 老系统数据，先预提了已经入表内或者表外。现需要记 贷133 贷705*/
		{
	 		sprintf(acErrMsg,"进入老数据欠息【金额【%lf】!",g_pub_tx.tmp_amt1);
	 		WRITEMSG
			memset( vln_intst,0,sizeof(vln_intst) );
			/* 计算罚息金额 */
			/****
			ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
			   ln_lo.lo_amt-ln_lo.pay_lo_amt,ln_lo.lo_intst-ln_lo.pay_lo_intst,
			   ln_lo.shld_pay_date,g_pub_tx.tx_date,&pun_intst,&cmpd_intst_aj);            	
			   ******/
			ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
			   ln_lo.lo_amt,ln_lo.lo_intst,
			   ln_lo.shld_pay_date,g_pub_tx.tx_date,&pun_intst,&cmpd_intst_aj);            	
	        if (ret) {
			sprintf(acErrMsg,"交易错误!%s %d",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit;        	
			}
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
	        if( ret ) {
	 		sprintf( acErrMsg,"老数据欠息【金额【%lf】】!",g_pub_tx.tmp_amt1);
	 		WRITEMSG	
			goto ErrExit;
			}
	                
	 		               
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
			    if( ret ) {	
				sprintf(acErrMsg,"交易错误!%s %d",__FILE__,__LINE__);
				WRITEMSG
				goto ErrExit;	
				}
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
	
	        }
	        else if( ln_lo.old_ac_ind[0]=='0' )
	      {
				memset( vln_intst,0,sizeof(vln_intst) );
	 			sprintf(acErrMsg,"新数据欠息【金额【%lf】!",g_pub_tx.tmp_amt1);
	 			WRITEMSG			
				/* 新系统数据处理方式 */
				/* 计算罚息金额 */
				/*****
				ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
					ln_lo.lo_amt-ln_lo.pay_lo_amt ,
					ln_lo.lo_intst-ln_lo.pay_lo_intst , ln_lo.shld_pay_date , 
					g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);   
				******/
				ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
					ln_lo.lo_amt ,
					ln_lo.lo_intst , ln_lo.shld_pay_date , 
					g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);   				         
	            if (ret) { /*sql_commit();*/break;}
	            /*调用还款计划函数确定各项欠息的还款金额*/
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
	           	if( ret ) {
		 		sprintf( acErrMsg,"新数据欠息【金额【%lf】!",g_pub_tx.tmp_amt1);
	 			WRITEMSG
				goto ErrExit;
				}
	 		
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
	            } 	        
	        }
	/*
            ret = Ln_lo_Upd_Upd_Hold(ln_lo, g_pub_tx.reply  );
	*/
            ret = Ln_lo_Upd_Upd(ln_lo, g_pub_tx.reply  );
            if ( ret )
            {
                 sprintf( acErrMsg,"Ln_lo_Upd_Upd 出错[%d]",ret);
	             WRITEMSG
                 goto ErrExit;
            }
            memset( &ln_lo , 0x00 , sizeof( struct ln_lo_c ) );	                
			/** 帐务处理  **/
			if ( ln_lo.ac_id != ac_id_old )
		
	/* 比较还款 */	
	g_pub_tx.tmp_amt1 = pub_base_PubDround_1(g_pub_tx.tmp_amt1);
	if ( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
	{
		sprintf(acErrMsg,"还款金额与应还金额不符[%lf]",g_pub_tx.tmp_amt1);
		WRITEMSG
		/**
		strcpy(g_pub_tx.reply,"L046");
		ERR_DEAL				
		**/
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
  if( ret ){
	sprintf(acErrMsg,"交易错误!%s %d",__FILE__,__LINE__);
	WRITEMSG
 	goto ErrExit;
	}
  /* 贷款登记簿--利息 */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"2",g_pub_tx.tx_amt2,0.00,"0",2 );
  if( ret ){
	sprintf(acErrMsg,"交易错误!%s %d",__FILE__,__LINE__);
	WRITEMSG
 	goto ErrExit;	
	}
  /* 贷款登记簿--罚息 */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"4",g_pub_tx.tx_amt4,0.00,"0",2 );
  if( ret ) {	
	sprintf(acErrMsg,"交易错误!%s %d",__FILE__,__LINE__);
	WRITEMSG
	goto ErrExit;	
	}
  		
 	
	/* 本金记流水 */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt1 , 0.00 ) )        
	{ 
		/*登记贷款日终打印登记簿*
		ret=Ln_eod_pri_InsReg( ln_ac_no, g_ln_mst, dd_ac_no, g_dd_mst.ac_seqn, g_dd_mst.name, 
				g_pub_tx.tx_amt1, 0, g_pub_tx.tx_code, "1", "1", ln_lo.shld_pay_date, g_pub_tx.tx_date, 
				g_pub_tx.trace_no , g_pub_tx.tx_date );
		if(ret)	goto ErrExit;
		***/
			
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/		
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
		/*登记贷款日终打印登记簿*
		ret=Ln_eod_pri_InsReg( ln_ac_no, g_ln_mst, dd_ac_no, g_dd_mst.ac_seqn, g_dd_mst.name, 
				g_pub_tx.tx_amt1, 0, g_pub_tx.tx_code, "1", "1", ln_lo.shld_pay_date, g_pub_tx.tx_date, 
				g_pub_tx.trace_no, g_pub_tx.tx_date );
		if(ret)	goto ErrExit;
		*******/
		
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/
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
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/		
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
	
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG

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
		amt_str[0]='1';
	}else{
		amt_str[0]='*';
	}	
	if( pub_base_CompDblVal( tx_amt2_new , 0.00 ) )/* 新数据表内利息 */
	{
		amt_str[1]='1';
	}else{
		amt_str[1]='*';
	}
	if( pub_base_CompDblVal( tx_amt4 , 0.00 ) )/*罚息*/
	{
		amt_str[2]='1';
	}else{
		amt_str[2]='*';
	}
	if( pub_base_CompDblVal( tx_amt2_old , 0.00 ) )/* 旧数据表内利息 */
	{
		amt_str[3]='1';
	}else{
		amt_str[3]='*';
	}
	if( pub_base_CompDblVal( tx_amt3_old , 0.00 ) )/* 旧数据表外利息 */
	{
		amt_str[4]='1';
	}else{
		amt_str[4]='*';
	}	
	set_zd_data( "0630" , amt_str );      
	
	                  
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
		if( ret ){ 	
			sprintf(acErrMsg,"交易错误!%s %d",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit;	
			}
	}
	/* 表外 */
	if( pub_base_CompDblVal(old_amt3, 0.00 ) )
	{
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
				,g_pub_tx.tx_date,"3",old_amt3,0.00,"0",2 );
		if( ret ){
			sprintf(acErrMsg,"交易错误!%s %d",__FILE__,__LINE__);
			WRITEMSG
		 	goto ErrExit;	
			}	
	}
        /***
        g_pub_tx.tx_amt3 = old_amt3 ;
        ****/
        g_pub_tx.tx_amt4 = old_amt4+old_amt3 ;   
		    strcpy( g_pub_tx.opn_br_no, g_ln_mst.opn_br_no); /*交易机构赋值2009/2/26 19:07:34 by martin*/
        /* 记账----只有老系统数据罚息和复利独立记账 */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )        
	{
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/		
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
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/		
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
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
				ac_id_old=ln_lo.ac_id;
	
		/*记会计流水*/
		set_zd_data( "0660" , "2" );		
		strcpy(g_pub_tx.sub_tx_code,"L203");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
		/*记客户帐*/
		sprintf(acErrMsg,"记客户帐金额tx_amt[%lf] bal[%lf]",g_pub_tx.tmp_amt1,g_dd_mst.bal);
		WRITEMSG
		/***del by lance
		g_pub_tx.tmp_amt1=g_dd_mst.bal-g_pub_tx.tmp_amt1;
		***/
		g_pub_tx.tmp_amt1=old_dd_bal-g_pub_tx.tmp_amt1; /*扣款金额*/

		sprintf(acErrMsg,"OVER记客户帐金额tx_amt[%lf] bal[%lf]",g_pub_tx.tmp_amt1,g_dd_mst.bal);
		WRITEMSG
		sprintf(acErrMsg,"OVER记客户帐金额tmp_tx_amt[%lf] bal[%lf]",g_pub_tx.tmp_amt1,old_dd_bal);
		WRITEMSG

	   	/**********取款部分************/
        strcpy( g_pub_tx.ac_no , dd_ac_no);
		sprintf(acErrMsg,"dd_ac_no=[%s][%ld][%ld]",dd_ac_no ,g_dd_mst.ac_id ,g_dd_mst.ac_seqn);
		WRITEMSG

		sprintf(acErrMsg,"tx_amt1=[%lf][%lf][%lf][%lf]",
		g_pub_tx.tx_amt1,g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
		WRITEMSG
		g_pub_tx.ac_id = g_dd_mst.ac_id ; 
        g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
	   	strcpy( g_pub_tx.ac_get_ind,"00");	/*未读取了mdm_ac_rel*/
		strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*为日终传票打印赋值dd_ac_no*/			   	
	   	strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-活期 */
	   	strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否*/
	   	strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入
																3日终汇总入*/ 
        strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.ac_wrk_ind[9]='1';     /*对于非结算户强行扣款*/
		g_pub_tx.ac_wrk_ind[8]='1';         /*日终记账标志 */

	   	g_pub_tx.svc_ind=1001;
		/***
	   	g_pub_tx.tx_amt1=tx_amt1;	
		***/
	   	g_pub_tx.tx_amt1=g_pub_tx.tmp_amt1;	
        strcpy ( g_pub_tx.brf,"日终按揭还款借活期户" );
	
	   	/*** 交易记帐处理 ***/
	   	if( pub_acct_trance() )
	   	{
             sprintf(acErrMsg,"交易记帐处理错误!");
             WRITEMSG
             goto ErrExit;
	   	}
	    /* 进行会计记帐 */
        set_zd_data("102J",g_ln_parm.cur_no);      
		/***
        set_zd_double("102F",g_pub_tx.tmp_amt1);             
		****/
        set_zd_double("102F",g_pub_tx.tmp_amt1);             
		g_pub_tx.tx_amt1=g_pub_tx.tmp_amt1;
        strcpy(g_pub_tx.sub_tx_code,"D003");/**取款子交易**/
        strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
             sprintf(acErrMsg,"会计记帐不成功!!");
             WRITEMSG
             goto ErrExit;
        }
	/*		sql_commit();	*--提交事务--*/
    }
	/*
    Ln_lo_Clo_Upd_Hold( );    		                         
	*/
    Ln_lo_Clo_Upd( );    		                         
    sprintf(acErrMsg,"日终归还按揭欠款表记录 PASS");
    WRITEMSG	
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终归还按揭欠款表记录处理程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	sql_commit();	/*--提交事务--*/
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* 待修改 */
    }
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终归还按揭欠款表记录处理程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
