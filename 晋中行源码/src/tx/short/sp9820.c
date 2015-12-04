/*************************************************
* 文 件 名:    sp9820.c
* 功能描述：   
*              根据贷款账号取出账户序号
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
#include "cif_basic_inf_c.h"
	
sp9820()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	int flag=0, redflag=0;
	long dqr=0;
	char tx_code[5];
	double to_amt;	/* 本期应还金额 */
	double tol,tol_lo,pay_amt;
	char cif_type[2];
	
	double	pun_intst=0.00,cmpd_intst_aj=0.00;
		int ttl_cnt=0,curr_cnt=0;
		double bal=0.00,lo_tol_amt=0.00,lo_pay_amt=0.00;		
  	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	char dc_flg[2];
	char dc_inf[5];
	char dc_flg1[2];
	char dc_inf1[5];
	char dc_flg2[2];
	char dc_inf2[5];
		
	FILE *fp;
	struct ln_mst_c		v;
	struct mdm_ac_rel_c		w;
	struct ln_lo_c		ln_lo;
	struct ln_lo_c		ln_lo_tmp;
	struct ln_pay_pln_c	ln_pay_pln;	
	struct ln_pay_pln_c	ln_pay_pln_s;
	struct ln_pay_pln_c	l_ln_pay_pln;	
	struct cif_basic_inf_c cif_basic_inf;	
	struct ln_mst_c		ln_mst_tmp;
	
	/** 数据初始化 **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );

	memset(&ln_lo , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_lo_tmp , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&ln_pay_pln_s , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset( &cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c) );
	memset(&l_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );	
		
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "1091" , ac_no );	/* 贷款帐号 */
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
	set_zd_data("1091",ac_no);	/* 处理旧帐号 */	
	
	get_zd_data( "0650" , tx_code ); /* 交易代码 */
	
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " ,
																ac_no );
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		ERR_DEAL
	}else if( ret !=0 ){
    		sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
   	}
	sprintf(acErrMsg,"取出的账户ID为[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	/* 根据贷款账户ID查询贷款主文件个数 */
	flag = sql_count("ln_mst", " ac_id=%ld " , g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg,"根据贷款账户ID查询贷款主文件个数[%d]",flag);
	WRITEMSG	
		
	if( flag==0 )
   	{
		set_zd_int( "109D", redflag );	/* 是否输入序号标志 1-唯一记录*/
 		sprintf(acErrMsg,"贷款主文件表中无此记录[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L114");
 		goto ErrExit;
   	}	

	 	
	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , 
									" ac_id=%ld " , g_mdm_ac_rel.ac_id );		
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL
		}else if(ret){
    		sprintf( acErrMsg,"取贷款主文件异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
    	}	
    	sprintf(acErrMsg,"取贷款主文件PASS");
		WRITEMSG 

	
		/* 输出变量 */
		sprintf(acErrMsg,"进入输出变量");
		WRITEMSG
		redflag=1;
		set_zd_long( "1092", g_mdm_ac_rel.ac_seqn );	/* 贷款账户序号 */
		set_zd_int( "109D", redflag );	/* 是否输入序号标志 1-唯一记录*/ 	
	
		/* 非卡上开户专用通道 */

	
		/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
		ret = pub_ln_lndis_file(ac_no,g_mdm_ac_rel.ac_seqn,
					&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
		if( ret )
		{
			sprintf(acErrMsg,"读取贷款户信息error");
			WRITEMSG
			goto ErrExit;
		}
	
		/* 现转标志判断 */
		/* 生成下传全路经文件名(批量) */
		pub_base_AllDwnFilName( filename );

		fp=fopen(filename,"w");
		if( fp==NULL )
		{
			sprintf( acErrMsg,"open file error [%s]",filename );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047");
			goto ErrExit;
		}
	
		strcpy( dc_flg1 , "1" );
  		strcpy( dc_inf1 , "现金" );
		strcpy( dc_flg , "2" );
  		strcpy( dc_inf , "转账" );
		strcpy( dc_flg2 , "8" );
  		strcpy( dc_inf2 , "扣划" );
		set_zd_data("0660",dc_flg);
		set_zd_data("0810",dc_inf);  
	
		/***长沙商行贷款还款时无论对私对公客户均支持现金和转账
		if( g_ln_parm.cif_type[0]=='2' )
		{		    
			pub_base_strpack(dc_inf);
			fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
		}else{
			pub_base_strpack(dc_inf1);
			pub_base_strpack(dc_inf);
			fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
			fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
		}	
		******/
		fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
		fprintf( fp,"0660|%s|%s|\n",dc_inf2,dc_flg2 );

		set_zd_data( DC_FILE_SND,"2" );
		fclose(fp);	
		
		if( strcmp( tx_code , "3203" )==0 )	
		{
			sprintf(acErrMsg,"交易3203!");
			WRITEMSG
			
			/* 检查贷款类型是否是按揭贷款 */
			if( g_ln_parm.ln_pay_type[0]!='3'&& g_ln_parm.ln_pay_type[0]!='4' )
			{
				sprintf( acErrMsg,"此贷款非按揭贷款[%s]",g_ln_parm.ln_pay_type);
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
    			ret = pub_base_CompDblVal( g_ln_mst.ys_bal, 0.00 );
    			if( ret==0 )
    			{
    				sprintf( acErrMsg,"此按揭贷款尚未放款!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L109" );		
					goto ErrExit;
				}
			}		
			sprintf(acErrMsg,"检查贷款是否放款 PASS!");
			WRITEMSG		
		
		
			/* 查询按揭贷款欠款表中的欠款信息 */	
			ret= Ln_lo_Dec_Sel( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d and \
							pay_type='0'" , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)
			{
				sprintf( acErrMsg, "执行Ln_lo_Dec_Sel错!ret=[%d]", ret);
				WRITEMSG		
				return(1);	
			}			
			TRACE
			flag=0;	/* 共欠期数 */
			while(1)
			{
				ret = Ln_lo_Fet_Sel( &ln_lo_tmp, g_pub_tx.reply );
   				if( ret && ret!=100 )
 				{
	 				sprintf(acErrMsg,"fetch Ln_lo error code = [%d]",ret);
	 				WRITEMSG
	 				return 1;
 				}else if(ret==100){
					break;
				}
				flag++;
			
	 			sprintf( acErrMsg,"old_ac_ind= [%s]",ln_lo_tmp.old_ac_ind );
	 			WRITEMSG
	 			sprintf( acErrMsg,"mac= [%s]",ln_lo_tmp.mac );
	 			WRITEMSG			
	 			sprintf(acErrMsg,"pay_pun_intst=%lf",ln_lo_tmp.pay_pun_intst );
	 			WRITEMSG	
	 				
				if(ln_lo_tmp.old_ac_ind[0]=='1'|| ln_lo_tmp.old_ac_ind[0]=='2')
				{
					/* 老系统数据处理方式 */
					/* 计算罚息金额 */
					TRACE
					/*****
					ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm ,
								ln_lo_tmp.lo_amt-ln_lo_tmp.pay_lo_amt , 
								ln_lo_tmp.lo_intst-ln_lo_tmp.pay_lo_intst , 
								ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , 
								&pun_intst , &cmpd_intst_aj);    
					****/
					ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm ,
								ln_lo_tmp.lo_amt , 
								ln_lo_tmp.lo_intst , 
								ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , 
								&pun_intst , &cmpd_intst_aj);  					        	
		            if (ret) goto ErrExit;   
		        }
		        if( ln_lo_tmp.old_ac_ind[0]=='0' )
				{
					/* 新系统数据处理方式 */
					/* 计算罚息金额 */
					TRACE
					/****
					ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm ,
								ln_lo_tmp.lo_amt-ln_lo_tmp.pay_lo_amt ,
								ln_lo_tmp.lo_intst-ln_lo_tmp.pay_lo_intst ,
								ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date ,
								&pun_intst , &cmpd_intst_aj); 
					****/
					ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm ,
								ln_lo_tmp.lo_amt ,
								ln_lo_tmp.lo_intst ,
								ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date ,
								&pun_intst , &cmpd_intst_aj); 					           	
	               	if (ret) goto ErrExit;	
	            }
	                
				sprintf(acErrMsg,"pun_intst=[%lf]!cmpd_intst_aj=[%lf]", 
													pun_intst,cmpd_intst_aj);
				WRITEMSG	  
			              	        
				ln_lo.lo_amt+=ln_lo_tmp.lo_amt; 			/* 欠款本金 */
				ln_lo.lo_intst+=ln_lo_tmp.lo_intst; 		/* 欠利息 */
				ln_lo.pun_intst+=pun_intst+cmpd_intst_aj ; 	/* 罚息 */
				ln_lo.pay_lo_amt+=ln_lo_tmp.pay_lo_amt; 			/* 已还欠款本金 */
				ln_lo.pay_lo_intst+=ln_lo_tmp.pay_lo_intst; 		/* 已还欠利息 */
				ln_lo.pay_pun_intst+=ln_lo_tmp.pay_pun_intst ; 	/* 已还罚息 */				
				/***
				pay_amt+=ln_lo_tmp.pay_lo_amt+ln_lo_tmp.pay_lo_intst
								   +pun_intst+cmpd_intst_aj;* 已还金额 *
				****/				
	 			sprintf( acErrMsg,"pay_amt[%lf]",pay_amt);
	 			WRITEMSG
			}
		Ln_lo_Clo_Sel ();
		TRACE
		/* 长沙商行旧数据问题 */
		/* 取出当期应还金额 */
  		ret = Ln_pay_pln_Sel(g_pub_tx.reply, &ln_pay_pln, 
				"ac_id=%ld and ac_seqn=%d", g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		if( ret==100 )
		{
			sprintf(acErrMsg,"还款计划表信息中无此信息![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L043");
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"读取贷款还款计划表信息异常![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}		  
		sprintf(acErrMsg,"取贷款还款计划表信息 PASS");
		WRITEMSG	
		TRACE
		
		/* 当期应还金额 */
		ret = pub_base_deadlineM( ln_pay_pln.beg_date, -1, &dqr);
		if( ret ) 	goto ErrExit;
			
		if(dqr > g_pub_tx.tx_date )
		{
			sprintf(acErrMsg,"预先已还款!");
			WRITEMSG
			ln_pay_pln.curr_amt=0;	/* 当期应还本金 */
			ln_pay_pln.curr_intst=0;/* 当期应还利息 */

			if( (g_ln_mst.opn_date==g_ln_mst.ic_date) && ln_pay_pln.curr_cnt==1 )/*第一期*/
			{						
				sprintf(acErrMsg,"第一期!");
				WRITEMSG				
				/*由于长沙过渡数据不准确,重新计算当期还款计划*/
				ret = pub_ln_PayPlan_Qry( g_ln_mst , g_ln_parm , ln_pay_pln.curr_cnt, ln_pay_pln.ttl_cnt,g_ln_mst.bal,&l_ln_pay_pln);	
				if (ret)
				{
					sprintf(acErrMsg,"调用函数pub_ln_PayPlan_qry错误!");
					WRITEMSG
					return 1;	
				}
				ln_pay_pln.curr_amt= l_ln_pay_pln.curr_amt;				
				ln_pay_pln.curr_intst= l_ln_pay_pln.curr_intst;
			}
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
		}
		
		if((ln_pay_pln.curr_cnt==ln_pay_pln.ttl_cnt)&&
									ln_pay_pln.pay_ind[0]=='1' )
		{
			to_amt = 0;
		}
		/* 算总金额 */
		pay_amt=ln_lo.pay_lo_amt+ln_lo.pay_lo_intst+ln_lo.pay_pun_intst;
		tol_lo=ln_lo.lo_amt+ln_lo.lo_intst+ln_lo.pun_intst-pay_amt;/*欠款总计*/
		sprintf(acErrMsg,"tol_lo[%lf],lo_amt[%lf], lo_intst[%lf] ,\
							pun_intst[%lf] , pay_amt[%lf]",tol_lo,ln_lo.lo_amt,
							ln_lo.lo_intst,ln_lo.pun_intst,pay_amt);
		WRITEMSG		
		tol=tol_lo+to_amt;	/* 总计 */ 
		sprintf(acErrMsg,"tol_lo[%lf], to_amt[%lf]",tol_lo,to_amt);
		WRITEMSG		
		TRACE
		/* 输出 */ 
		set_zd_double("1094",g_ln_mst.bal);/* 贷款本金 */
		set_zd_long("109E",ln_pay_pln.ttl_cnt);	/* 总期数 */
		set_zd_long("109F",ln_pay_pln.curr_cnt-1-flag);	/* 已还期数 */

		set_zd_int("1096",flag);	/* 拖欠期数 */
		set_zd_double("1097",ln_lo.lo_amt+ln_lo.lo_intst-ln_lo.pay_lo_amt-ln_lo.pay_lo_intst);/* 拖欠总额 */
		set_zd_double("1098",ln_lo.pun_intst-ln_lo.pay_pun_intst);/* 罚息 */		
		set_zd_double("109G",tol_lo);/* 欠款总计 */	
		
		set_zd_double("1095",to_amt);	/* 本期金额 */			
		set_zd_double("109I",ln_pay_pln.curr_amt);	/* 本期应还本金 */		
		set_zd_double("109J",ln_pay_pln.curr_intst);	/* 本期应还利息 */							
		set_zd_double("109H",tol);	/* 总计金额 */		
		
		set_zd_data("1093",g_ln_mst.name);/* 户名 */

		/* 取客户类型 */
		ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
										"cif_no=%ld" , g_ln_mst.cif_no);
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
		set_zd_data("109L" , cif_type);					
	
    	goto OkExit;
    	}
  }  	
    	
  	redflag=9;
	set_zd_int( "109D", redflag );	/* 是否输入序号标志 1-唯一记录*/	 		
		
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
   
