/*************************************************
* 文 件 名:    sp9823.c
* 功能描述：   
*              根据贷款账号,账户序号以及偿还期次算出按揭还款金额
*
* 作    者:    lance
* 完成日期：   2003年01月20日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"
	
sp9823()  
{ 		
	int ret=0;
	int flag=0, redflag=0;
	long dqr=0;
	double to_amt;	/* 本期应还金额 */
	double tol,tol_lo;
	int cnt,i;
	double amt;	/* 提前偿还金额 */
	double pay_amt,pay_lo_amt; /* 偿还款 */
	double	pun_intst=0.00,cmpd_intst_aj=0.00;
	
		int ttl_cnt=0,curr_cnt=0;
		double bal=0.00,lo_tol_amt=0.00,lo_pay_amt=0.00;
				
	struct ln_mst_c		v;
	struct mdm_ac_rel_c	w;
	struct ln_lo_c		ln_lo;
	struct ln_lo_c		ln_lo_tmp;
	struct ln_pay_pln_c	ln_pay_pln;
	struct ln_pay_pln_c	t_ln_pay_pln;	
	struct ln_pay_pln_c	v_ln_pay_pln;	
	struct ln_pay_pln_c	l_ln_pay_pln;	
		
	/** 数据初始化 **/
	memset(&ln_lo , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_lo_tmp , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&t_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&v_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );	
	memset(&l_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );	
		
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "1091" , w.ac_no );	/* 贷款帐号 */
	pub_base_old_acno( w.ac_no );  	/** 对旧帐号的处理 **/
	set_zd_data("1091",w.ac_no);	/* 处理旧帐号 */

	get_zd_long( "1092" , &v.ac_seqn );	/* 帐号序号 */
	get_zd_int( "1099" , &cnt );		/* 偿还期数 */

	sprintf( acErrMsg," senf acno [%s] [%d]",w.ac_no,v.ac_seqn );
	WRITEMSG
	
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s' ",w.ac_no);
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
    }

	sprintf(acErrMsg,"取出的账户ID为[%ld]seqn[%d]",
		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	WRITEMSG	
	
	if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && g_mdm_ac_rel.ac_seqn!=v.ac_seqn)
	{
    	sprintf( acErrMsg,"[%d]!=[%d]",g_mdm_ac_rel.ac_seqn,v.ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
	}
	else
	{
		ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst," ac_id=%ld and ac_seqn=%d ",
			g_mdm_ac_rel.ac_id,v.ac_seqn );
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL
		} 
		else if( ret ) { ERR_DEAL }	

		Ln_mst_Clo_Sel ();
		ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm," prdt_no='%s' ",
			g_ln_mst.prdt_no);
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"W030" );
			ERR_DEAL
		} 
		else if( ret ) { ERR_DEAL }	

		/* 检查贷款类型是否是按揭贷款 */
		if ( g_ln_parm.ln_pay_type[0]!='3'&& g_ln_parm.ln_pay_type[0]!='4' )
		{
			sprintf( acErrMsg,"此贷款非按揭贷款",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L042" );
			ERR_DEAL
		}	
		sprintf(acErrMsg,"检查贷款类型是否是按揭贷款 PASS!");
		WRITEMSG		

		/* 查询贷款是否放款 */ 
		ret = pub_base_CompDblVal( g_ln_mst.bal, 0.00 );
		if( ret==0 )
		{
    			sprintf( acErrMsg,"此按揭贷款尚未放款!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L109" );		
			goto ErrExit;
		}		
		sprintf(acErrMsg,"检查贷款是否放款 PASS!");
		WRITEMSG				
		
		/* 查询按揭贷款欠款表中的欠款信息 */	
		ret = Ln_lo_Dec_Sel( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and \
							pay_type='0'" , g_mdm_ac_rel.ac_id,v.ac_seqn );
		if(ret)
		{
				sprintf( acErrMsg, "执行Ln_lo_Dec_Sel错!ret=[%d]", ret);
				WRITEMSG		
				return(1);	
		}			
		
		flag=0;	/* 共欠期数 */
		while(1)
		{
			ret = Ln_lo_Fet_Sel( &ln_lo_tmp, g_pub_tx.reply );
   			if( ret && ret!=100 )
 			{
	 			sprintf( acErrMsg,"fetch Ln_lo_Fet_Sel error code = [%d]",ret);
	 			WRITEMSG
	 			return 1;
 			}
			else if( ret==100 )
			break;
			
			if( ln_lo_tmp.old_ac_ind[0]=='1' || ln_lo_tmp.old_ac_ind[0]=='2' )
			{
				/* 老系统数据处理方式 */
				/* 计算罚息金额 */
				TRACE
				/***
				ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm , ln_lo_tmp.lo_amt-ln_lo_tmp.pay_lo_amt , ln_lo_tmp.lo_intst-ln_lo_tmp.pay_lo_intst , ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);            	
		                ****/
				ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm , ln_lo_tmp.lo_amt , ln_lo_tmp.lo_intst , ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);            	
		                
		                if (ret) goto ErrExit;   
		        }
		        if( ln_lo_tmp.old_ac_ind[0]=='0' )
			{
				/* 新系统数据处理方式 */
				/* 计算罚息金额 */
				TRACE
				/******
				ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm , ln_lo_tmp.lo_amt-ln_lo_tmp.pay_lo_amt , ln_lo_tmp.lo_intst-ln_lo_tmp.pay_lo_intst , ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);            	
	                	******/
				ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm , ln_lo_tmp.lo_amt , ln_lo_tmp.lo_intst , ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);            	
	                	
	                	if (ret) goto ErrExit;	
	                }
	                
			sprintf( acErrMsg, "pun_intst=[%lf]!cmpd_intst_aj=[%lf]", pun_intst,cmpd_intst_aj);
			WRITEMSG
			flag++;
			ln_lo.lo_amt+=ln_lo_tmp.lo_amt; 	/* 欠款本金 */
			ln_lo.lo_intst+=ln_lo_tmp.lo_intst; 	/* 欠利息 */
			ln_lo.pun_intst+=pun_intst+cmpd_intst_aj; 	/* 罚息 */
			ln_lo.pay_lo_amt+=ln_lo_tmp.pay_lo_amt; 			/* 已还欠款本金 */
			ln_lo.pay_lo_intst+=ln_lo_tmp.pay_lo_intst; 		/* 已还欠利息 */
			ln_lo.pay_pun_intst+=ln_lo_tmp.pay_pun_intst ; 	/* 已还罚息 */				
			/***
			pay_lo_amt+=ln_lo.pay_lo_amt+ln_lo.pay_lo_intst+pun_intst+cmpd_intst_aj;* 已还金额 *
			****/
		}
		
		Ln_lo_Clo_Sel ();
		
		/* 取出当期应还金额 */
	
  		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &ln_pay_pln , "ac_id=%ld and ac_seqn=%d"
					 , g_mdm_ac_rel.ac_id,v.ac_seqn );
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%d"
					 , g_mdm_ac_rel.ac_id,v.ac_seqn );
		if( ret==100 )
		{
			sprintf(acErrMsg,"贷款还款计划表信息中无此信息![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L043");
			goto ErrExit;
		}else if( ret )
		  {
			sprintf(acErrMsg,"读取贷款还款计划表信息异常![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		  }		  
		sprintf(acErrMsg,"取贷款还款计划表信息 PASS");
		WRITEMSG
		
		/* 当期应还金额 */
		pub_base_deadlineM( ln_pay_pln.beg_date, -1, &dqr);		
		if(dqr > g_pub_tx.tx_date )
		{
			sprintf(acErrMsg,"预先已还款!");
			WRITEMSG
			ln_pay_pln.curr_amt=0;	/* 当期应还本金 */
			ln_pay_pln.curr_intst=0;/* 当期应还利息 */

			/****
			if( (g_ln_mst.opn_date==g_ln_mst.ic_date) && ln_pay_pln.curr_cnt==1 )*第一期*
			{						
			
				sprintf(acErrMsg,"第一期!");
				WRITEMSG
			***/
			/*由于长沙过渡数据不准确,重新计算当期还款计划*/
			ret = pub_ln_PayPlan_Qry( g_ln_mst , g_ln_parm , ln_pay_pln.curr_cnt, ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1,g_ln_mst.bal,&l_ln_pay_pln);	
			if (ret)
			{
				sprintf(acErrMsg,"调用函数pub_ln_PayPlan_qry错误!");
				WRITEMSG
				return 1;	
			}
			ln_pay_pln.curr_amt= l_ln_pay_pln.curr_amt;				
			ln_pay_pln.curr_intst= l_ln_pay_pln.curr_intst;
			/***
			}
			****/
			sprintf(acErrMsg,"预先已还款!amt[%lf]intst[%lf]",ln_pay_pln.curr_amt,ln_pay_pln.curr_intst);
			WRITEMSG
			to_amt = ln_pay_pln.curr_amt+ln_pay_pln.curr_intst; /* 当期应还金额 */
		}else{
				/*由于长沙过渡数据不准确,重新计算当期还款计划*/
				ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*总期数*/
				curr_cnt= ln_pay_pln.curr_cnt;		/*当前期数*/
				/* 取欠款表数据*/
				ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
				if(ret)	return 1;
				ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
				if(ret)	return 1;	
				bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);			/*剩余本金*/				
				ret = pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt, bal, &l_ln_pay_pln);				
				if (ret)
				{
					sprintf(acErrMsg,"调用函数pub_ln_PayPlan_qry错误!");
					WRITEMSG
					return 1;	
				}
				ln_pay_pln.curr_amt= l_ln_pay_pln.curr_amt;				
				ln_pay_pln.curr_intst= l_ln_pay_pln.curr_intst;
				to_amt = ln_pay_pln.curr_amt+ln_pay_pln.curr_intst; /* 当期应还金额 */
				TRACE
		}
	
		/* 偿还款 */
		if ( (cnt-flag) <=0 )
		{
			pay_amt = 0;
		}else if ( (cnt-flag) ==1 ){
			/**由于长沙商行数据转换差异,所以还款计划中还款金额要重新计算 */
			/****
			pay_amt =t_ln_pay_pln.curr_amt+t_ln_pay_pln.curr_intst ;
			*****/
			sprintf(acErrMsg,"前:curr_amt=[%lf]curr_intst=[%lf]!",ln_pay_pln.curr_amt,ln_pay_pln.curr_intst);
			WRITEMSG			
			ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*总期数*/
			curr_cnt= ln_pay_pln.curr_cnt;		/*当前期数*/
			/* 取欠款表数据*/
			ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	return 1;
			ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	return 1;	
			bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);			/*剩余本金*/				
			ret = pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt, bal, &v_ln_pay_pln);			
			if (ret)
			{
				sprintf(acErrMsg,"调用函数pub_ln_PayPlan_qry错误!");
				WRITEMSG
				return 1;	
			}
			sprintf(acErrMsg,"后:curr_amt=[%lf]curr_intst=[%lf]!",v_ln_pay_pln.curr_amt,v_ln_pay_pln.curr_intst);
			WRITEMSG
			pay_amt=v_ln_pay_pln.curr_amt+v_ln_pay_pln.curr_intst;						

		}else{
			TRACE
			i=t_ln_pay_pln.curr_cnt+1;
			/***
			pay_amt =l_ln_pay_pln.curr_amt+l_ln_pay_pln.curr_intst ;
			****/
			ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*总期数*/
			curr_cnt= ln_pay_pln.curr_cnt;		/*当前期数*/
			/* 取欠款表数据*/
			ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	return 1;
			ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	return 1;	
			bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);
			while ( curr_cnt<(t_ln_pay_pln.curr_cnt+(cnt-flag)) )
			{
				sprintf(acErrMsg,"进入计算函数第[%ld]期!!!!!",curr_cnt);
				WRITEMSG				
				ret = pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt, bal, &v_ln_pay_pln);						
				if (ret)
				{
					sprintf(acErrMsg,"调用函数pub_ln_PayPlan_qry错误!");
					WRITEMSG
					return 1;	
				}
				pay_amt+=v_ln_pay_pln.curr_amt+v_ln_pay_pln.curr_intst;
				i++;
				bal=bal-v_ln_pay_pln.curr_amt;
				curr_cnt++;
				ttl_cnt--;		
				memset(&v_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
				sprintf(acErrMsg,"++++++++++++++over++++++++++++++++++");
				WRITEMSG				
			}
		}
									
		/* 算总金额 */
		pay_lo_amt=ln_lo.pay_lo_amt+ln_lo.pay_lo_intst+ln_lo.pay_pun_intst;
		tol_lo=ln_lo.lo_amt+ln_lo.lo_intst+ln_lo.pun_intst-pay_lo_amt;/* 欠款总计 */

		tol=tol_lo+pay_amt;	/* 总计 */ 
		sprintf(acErrMsg,"tol_lo【%lf】pay_amt[%lf]!",tol_lo,pay_amt);
		WRITEMSG		
		
		/* 输出 */ 
		set_zd_double("1094",g_ln_mst.bal);/* 贷款本金 */
		set_zd_long("109E",ln_pay_pln.ttl_cnt);	/* 总期数 */
		set_zd_long("109F",ln_pay_pln.curr_cnt-1-flag);	/* 已还期次 */

		set_zd_int("1096",flag);	/* 拖欠期数 */
		set_zd_double("1097",ln_lo.lo_amt+ln_lo.lo_intst-ln_lo.pay_lo_amt-ln_lo.pay_lo_intst);/* 拖欠总额 */
		set_zd_double("1098",ln_lo.pun_intst-ln_lo.pay_pun_intst);/* 罚息 */		
		set_zd_double("109G",tol_lo);/* 欠款总计 */	
		
		set_zd_double("109I",ln_pay_pln.curr_amt);	/* 本期应还本金 */		
		set_zd_double("109J",ln_pay_pln.curr_intst);	/* 本期应还利息 */
		set_zd_double("1095",to_amt);	/* 本期金额 */				
		set_zd_double("109H",tol);	/* 总计金额 */		
		
		set_zd_data("1093",g_ln_mst.name);/* 户名 */			
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
