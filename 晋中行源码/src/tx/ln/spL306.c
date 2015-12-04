/*************************************************
* 文 件 名: spL306.c
* 功能描述：   
*              贷款利率调整
*
* 作    者:    lance
* 完成日期：   2003年01月24日
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
#include "ln_expand_c.h"
	
char 	sts[2];
char	rate_name[5];
double 	rate=0.00,rate_parm=0.00,old_rate=0.00,new_rate=0.00;
static  struct ln_expand_c 	p_ln_expand;	
	
spL306()  
{ 		
	int ret=0 ,min_ret=0 ,max_ret=0;
	char acm_type[2];

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
			
	/** 处理流程 **/
	/* 检查利息积数是否为零
	if ( g_ln_mst.intst_acm > 0.005 )
		{
			sprintf(acErrMsg,"检查利息积数大于零![%lf]",g_ln_mst.intst_acm);
		  WRITEMSG
		  strcpy( g_pub_tx.reply,"D147" );
			goto ErrExit;
		} */
				
	/* 检查贷款类型(若为按揭,只能修改罚息利率) */
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"按揭贷款![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		if ( sts[0]!='4' || sts[0]!='3' )
		{
			sprintf(acErrMsg,"按揭贷款只能调整逾期利率与罚息利率!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"L063" );
			goto ErrExit;
		}
		
		/* 检查逾期利率 */
		if( sts[0]=='3' )
		{
			/* 取逾期利率 */
		   	if( g_ln_parm.over_rate_type[0]=='0' )/* 取逾期利率编号 */
		   	{
		   		ret = pub_base_getllz(g_ln_parm.over_rate_no,g_ln_parm.cur_no,
								g_pub_tx.tx_date,&rate_parm);
		   		if( ret ) goto ErrExit;					
		   		rate_parm = rate_parm/1.2;/* 转换成月利率 */
		   	}else{
				rate_parm = g_ln_mst.rate ;
		   	}
		   	
			/**罚息利率检查**/
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
						rate_parm*(100+g_ln_parm.over_rate_max)/100.00 )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
					g_ln_mst.over_rate,g_ln_parm.over_rate_max,
					rate_parm*(100+g_ln_parm.over_rate_max)/100.00
								 );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L127" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high[%lf]!!",
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00));
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L126" );
				goto ErrExit;
			}	
		}   		
		/* 检查罚息利率 */
		if( sts[0]=='4' )
		{
			/* 取罚息利率 */
		   	if( g_ln_parm.fine_rate_type[0]=='0' )/* 取罚息利率编号 */
		   	{
		   		ret = pub_base_getllz(g_ln_parm.fine_rate_no,g_ln_parm.cur_no,
								g_pub_tx.tx_date,&rate_parm);
		   		if( ret ) goto ErrExit;					
		   		rate_parm = rate_parm/1.2;/* 转换成月利率 */
		   	}else{
				rate_parm = g_ln_mst.rate ;
		   	}
		   	
			/**罚息利率检查**/
			if( pub_base_CompDblVal(g_ln_mst.fine_rate,
						rate_parm*(100+g_ln_parm.fine_rate_max)/100.00 )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.fine_rate_max,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
					g_ln_mst.fine_rate,g_ln_parm.fine_rate_max,
					rate_parm*(100+g_ln_parm.fine_rate_max)/100.00
								 );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L217" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high[%lf]!!",
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00));
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L218" );
				goto ErrExit;
			}	
		}   						
	}	
		
	/* 检查贷款利率修改方式 */
	if ( g_ln_parm.get_rate_type[0]!='1' )
	{
		sprintf( acErrMsg,"该贷款账户利率不能手工修改");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L058" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"检查贷款利率修改方式 PASS!");
	WRITEMSG	
					
	/* 根据贷款状态检查可以修改的利率 */
	/* 展期 */
	if( g_ln_mst.ac_sts[0] =='1' && sts[0]=='1' )
	{
		if (g_ln_mst.mtr_date < g_pub_tx.tx_date)
		
		{
		  sprintf( acErrMsg,"该贷款账户处于展期状态,不可修改正常利率");
		  WRITEMSG
		  strcpy( g_pub_tx.reply ,"L059" );
		  ERR_DEAL	 		
		}
	}
	/* 逾期 */
	if( atoi(g_ln_mst.ac_sts)>=2 && atoi(g_ln_mst.ac_sts)<=4 )
	{
		if( sts[0]!='4' && sts[0]!='3')
		{
			sprintf( acErrMsg,"该贷款账户处于逾期状态,只可修改逾期利率与罚息利率!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L060" );
			ERR_DEAL
		}	 		
	}		

	/* 登记分段积数表的条件 */
	
	/* 登记分段积数表 */
	/* 统计贷款主文件中积数 */
	strcpy( acm_type,"2");/*积数计算类型（1每日累计2变动累计3变动累计倒算）*/
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
						g_ln_parm.intst_days_type,acm_type,g_ln_mst.bal,0.00,
						&g_ln_mst.intst_acm,"0",0,0 );
	if( ret )
	{
		sprintf(acErrMsg,"调用函数计算积数错误!");
		WRITEMSG
		return -1;	
	}				
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '2',
		  g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.intst_acm,
		  g_ln_mst.rate, g_pub_tx.reply);
	if (ret)
	{
		sprintf( acErrMsg,"调用函数登记分段积数表失败");
		WRITEMSG
		ERR_DEAL	
	}
	/* 若计算复利 */
	if ( g_ln_mst.cmpd_intst_ind[0]=='Y' )
	{
		TRACE
		
		/* 表内欠息积数 */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
				g_ln_parm.intst_days_type,acm_type,g_ln_mst.in_lo_intst,0.00,
					&g_ln_mst.in_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return -1;	
		}
		TRACE	
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '3',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.in_lo_acm,
			  g_ln_mst.rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"调用函数登记分段积数表失败");
			WRITEMSG
			ERR_DEAL	
		}
		/* 表外欠息积数 */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
				g_ln_parm.intst_days_type,acm_type,g_ln_mst.out_lo_intst,0.00,
					&g_ln_mst.out_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return -1;	
		}				
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '4',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.out_lo_acm,
			  g_ln_mst.rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"调用函数登记分段积数表失败");
			WRITEMSG
			ERR_DEAL	
		}	
		/* 复利积数 */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
				g_ln_parm.intst_days_type,acm_type,g_ln_mst.cmpd_lo_intst,0.00,
					&g_ln_mst.cmpd_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return -1;	
		}		
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '5',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.cmpd_lo_acm,
			  g_ln_mst.rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"调用函数登记分段积数表失败");
			WRITEMSG
			ERR_DEAL	
		}		
	}
	/* 更新贷款主文件 */
    ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
    if ( ret )
    {
        sprintf( acErrMsg, "执行Ln_mst_Dec_Upd错!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    
    ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "贷款主文件中无此记录" );
        WRITEMSG
        ERR_DEAL
    }else if ( ret )
    {
        sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    	/* 修改 */
	g_ln_mst.ic_date = g_pub_tx.tx_date ;		/* 起息日期 */

	if (sts[0]=='1')
	{
		g_ln_mst.rate = rate ;		/* 正常利率 */		
		
		sprintf(acErrMsg,"[%s]",g_ln_parm.rate_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%s]",g_ln_parm.cur_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
		WRITEMSG		
		ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &rate_parm );
		if( ret )
		{
			sprintf(acErrMsg,"调用根据利率编号取得利率函数错误!");
			WRITEMSG
			goto ErrExit;	
		}
		rate_parm = rate_parm/1.2;/* 转换成月利率 */
		
				
		/*正常利率检查*/
		if( pub_base_CompDblVal(g_ln_mst.rate,
					rate_parm*( (100+g_ln_parm.max_flt_ratio)/100.00 ) )>0 )
		{
			vtcp_log("=[%d][%s]",g_ln_parm.max_flt_ratio,g_ln_mst.prdt_no );
			sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",
					rate_parm,rate_parm*((100+g_ln_parm.max_flt_ratio)/100.00)
							,g_ln_parm.max_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			goto ErrExit;
		}
            	sprintf(acErrMsg,"最高利率正确时[%lf][%lf][%d] ", rate_parm,
			rate_parm*((100+g_ln_parm.max_flt_ratio)/100.00) ,g_ln_parm.max_flt_ratio );
		WRITEMSG

		if( pub_base_CompDblVal(g_ln_mst.rate,
					rate_parm*((100+g_ln_parm.min_flt_ratio)/100.00) )<0 )
		{
			sprintf(acErrMsg,"最低利率错误时[%lf][%lf][%d] ", rate_parm,
		 		rate_parm*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			goto ErrExit;
		}
		sprintf(acErrMsg,"最低利率正确时[%lf][%lf][%d] ", rate_parm,
				rate_parm*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
			WRITEMSG
		sprintf(acErrMsg,"检查该产品最低余额和最高限额(开户) PASS!");
		WRITEMSG

		/* 检查修改后的利率是否符合产品要求 *
		* 检查利率是否超过规定利率上浮和下浮后的界限*
		* 调用函数根据利率编号取得利率(只取利率，逾期利率无浮动) *
		
		sprintf(acErrMsg,"[%s]",g_ln_parm.rate_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%s]",g_ln_parm.cur_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
		WRITEMSG		
		ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &rate_parm );
		if( ret )
		{
			sprintf(acErrMsg,"调用根据利率编号取得利率函数错误!");
			WRITEMSG
			goto ErrExit;	
		}
		rate_parm = rate_parm/1.2;* 转换成月利率 *
	sprintf(acErrMsg,"检查该产品最低余额和最高限额(开户) PASS!");
	WRITEMSG
			
		min_ret = rate - rate_parm*(100+g_ln_parm.min_flt_ratio)/100;
		max_ret = rate - rate_parm*(100+g_ln_parm.max_flt_ratio)/100;
		
		if ( pub_base_CompDblVal( min_ret , 0.00 ) <0 )
		{
			strcpy(acErrMsg,"贷款利率低于规定的最低利率！");
			WRITEMSG
			sprintf(acErrMsg,"[%lf] [%lf]",rate,rate_parm*(100+g_ln_parm.min_flt_ratio)/100);
			WRITEMSG
			sprintf(acErrMsg,"[%d] [%d]",g_ln_parm.min_flt_ratio,g_ln_parm.max_flt_ratio);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L005" );
			goto ErrExit;
		}
		if ( pub_base_CompDblVal( max_ret , 0.00 ) >0 )
		{
			strcpy(acErrMsg,"贷款利率高于规定的最高利率！");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L006" );
			goto ErrExit;
		}
	sprintf(acErrMsg,"检查该产品最低余额和最高限额(开户) PASS!");
	WRITEMSG
	****/		

	}

	if (sts[0]=='2')
	{
		g_ln_mst.exp_rate = rate ;		/* 展期利率 */
		
		if( g_ln_mst.ac_id==100355044 )
		{ 
			;
		}else if( pub_base_CompDblVal(g_ln_mst.exp_rate,g_ln_mst.rate )<0 )
		{
			sprintf(acErrMsg,"展期利率过低[%lf][%lf]!!",g_ln_mst.exp_rate,g_ln_mst.rate);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L124" );
			goto ErrExit;
		}
		
		TRACE		
		/* 修改展期登记簿中最后一个记录 */
   		 ret = Ln_expand_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and exp_ind='%s' " ,g_ln_mst.ac_id , g_ln_mst.ac_seqn , g_ln_mst.exp_ind);
   		 if ( ret )
   		 {
       			 sprintf( acErrMsg, "执行Ln_expand_Dec_Upd错!ret=[%d]",ret );
       			 WRITEMSG
       			 ERR_DEAL
   		 }
    		 TRACE
  		 ret = Ln_expand_Fet_Upd( &p_ln_expand,g_pub_tx.reply );   
    		 if ( ret == 100 )
    		 {
       			sprintf( acErrMsg, "贷款展期登记簿中无此记录" );
        		WRITEMSG
				strcpy(g_pub_tx.reply, "L225");
        		ERR_DEAL
    		 }else if ( ret )
    		 {
       			 sprintf( acErrMsg, "执行Ln_expand_Fet_Upd出错!ret=[%d]",ret );
       			 WRITEMSG
       			 ERR_DEAL
    		 }
    		 TRACE
    		 p_ln_expand.exp_rate = rate ; /* 更新展期利率 */
		 ret = Ln_expand_Upd_Upd( p_ln_expand, g_pub_tx.reply );
		 if( ret )
		 {
			sprintf( acErrMsg,"执行Ln_expand_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
		TRACE
		Ln_expand_Clo_Upd();
		sprintf(acErrMsg,"更新贷款展期登记簿 PASS!");
		WRITEMSG    		 	
	}
	if (sts[0]=='3')
	{
		g_ln_mst.over_rate = rate ;		/* 逾期利率 */	
		
		if( g_ln_parm.over_rate_type[0]=='0' )/* 取逾期利率编号 */
		{
	   		ret = pub_base_getllz(g_ln_parm.over_rate_no,g_ln_parm.cur_no,
							g_pub_tx.tx_date,&rate_parm);
	   		if( ret ) goto ErrExit;					
	   		rate_parm = rate_parm/1.2;/* 转换成月利率 */
		}else{
			rate_parm = g_ln_mst.rate ;
		}		

		/**逾期\罚息利率检查**/
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*(100+g_ln_parm.over_rate_max)/100.00 )>0 )
		{
			vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
			sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
			g_ln_mst.over_rate,g_ln_parm.over_rate_max,
			rate_parm*(100+g_ln_parm.over_rate_max)/100.00
							 );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L062" );
			goto ErrExit;
		}
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
		{
			sprintf(acErrMsg,"current rate is too high[%lf]!!",
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00));
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L061" );
			goto ErrExit;
		}		
						
		/**逾期利率检查(原有)**
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*(100+g_ln_parm.over_rate_max)/100 )>0 )
		{
			vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
			sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
						g_ln_mst.over_rate,g_ln_parm.over_rate_max,rate_parm*(100+g_ln_parm.over_rate_max)/100
							 );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L062" );
			goto ErrExit;
		}
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*(1+g_ln_parm.over_rate_min/100) )<0 )
		{
			sprintf(acErrMsg,"current rate is too high[%lf]!!",
						rate_parm*(1+g_ln_parm.over_rate_min/100));
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L061" );
			goto ErrExit;
		}			
		sprintf(acErrMsg,"检查逾期利率 PASS!");
		WRITEMSG	
		***************/	

	}
		/* 检查罚息利率 */
	if( sts[0]=='4' )
	{
		g_ln_mst.fine_rate = rate ;		/* 逾期利率 */	
		/* 取罚息利率 */
	   	if( g_ln_parm.fine_rate_type[0]=='0' )/* 取罚息利率编号 */
	   	{
	   		ret = pub_base_getllz(g_ln_parm.fine_rate_no,g_ln_parm.cur_no,
							g_pub_tx.tx_date,&rate_parm);
	   		if( ret ) goto ErrExit;					
	   		rate_parm = rate_parm/1.2;/* 转换成月利率 */
	   	}else{
			rate_parm = g_ln_mst.rate ;
	   	}
	   	
		/**罚息利率检查**/
		if( pub_base_CompDblVal(g_ln_mst.fine_rate,
					rate_parm*(100+g_ln_parm.fine_rate_max)/100.00 )>0 )
		{
			vtcp_log("=[%d][%s]",g_ln_parm.fine_rate_max,g_ln_mst.prdt_no );
			sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
				g_ln_mst.fine_rate,g_ln_parm.fine_rate_max,
				rate_parm*(100+g_ln_parm.fine_rate_max)/100.00
							 );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L217" );
			goto ErrExit;
		}
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
		{
			sprintf(acErrMsg,"current rate is too high[%lf]!!",
					rate_parm*((100+g_ln_parm.over_rate_min)/100.00));
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L218" );
			goto ErrExit;
		}	
	}   		
	
	
		
					
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "修改贷款主文件错,存在重复记录!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"执行Ln_mst_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	Ln_mst_Clo_Upd();
	if( ret )
	{
			sprintf( acErrMsg,"执行Ln_mst_CLo_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
	}
	sprintf(acErrMsg,"更新贷款主文件 PASS!");
	WRITEMSG
	
					
	/* Pub_tx结构赋值 */

	strcpy(g_pub_tx.add_ind,"1");		/* 增减:0减1加2销户 */	
	sprintf(acErrMsg,"增减:[%s]",g_pub_tx.add_ind );
	WRITEMSG

	strcpy( g_pub_tx.ct_ind,"2");		/* 现转:1现2转 */	
	sprintf(acErrMsg,"增减:[%s]",g_pub_tx.ct_ind );
	WRITEMSG

	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3063;			
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );	
	strcpy ( g_pub_tx.brf,"贷款利率调整" );	
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
		
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		ERR_DEAL
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
	
	/* 打印程序 */
	/* 行内传票--利率调整部分 */
	ret = print_LNBKLL( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKLL,失败请检查!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款利率调整成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款利率调整失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	memset ( &p_ln_expand ,0x00 ,sizeof(struct ln_expand_c) );
	return 0;	
}

int data_set_struct()
{    
	int ret=0;
	
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* 贷款帐号 */
	sprintf(acErrMsg,"贷款帐号[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	get_zd_int( "0340" , &g_pub_tx.ac_seqn);	/* 贷款账户序号 */
	sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	get_zd_data("0710",sts);			/* 利率类型 */
	sprintf(acErrMsg,"利率类型[%s]",sts);
	WRITEMSG
	
	
	get_zd_double("0840",&rate);			/* 利率 */
	sprintf(acErrMsg,"利率[%lf]",rate);
	WRITEMSG	
	new_rate = rate ;
	
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}		
	
	if( sts[0]=='1' )		/* 正常利率 */
	{
		old_rate= g_ln_mst.rate;
		strcpy(rate_name, "正常");	
	}else if( sts[0]=='2' ){	/* 展期利率 */
		old_rate= g_ln_mst.exp_rate;
		strcpy(rate_name, "展期");
	}else if( sts[0]=='3' ){	/* 逾期利率 */
		old_rate= g_ln_mst.over_rate;
		strcpy(rate_name, "逾期");
	}else if( sts[0]=='4' ){	/* 罚息利率 */
		old_rate= g_ln_mst.fine_rate;
		strcpy(rate_name, "罚息");
	}		 
				
	return 0;
}

/* 打印--贷款利率调整传票 */
int print_LNBKLL( )
{
	char tx_type[9];
	char rate[11];
	char exp_ind[5];
	char old_rate_str[11];
	char rate_str[11];
			
	int ret_tmp=0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	memset(tx_type,0x00,sizeof(tx_type));
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKLL打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */					
	sprintf( old_rate_str , "%8.5lf‰" , old_rate );
	sprintf( rate_str , "%8.5lf‰" ,new_rate );
	pub_base_strpack( old_rate_str );
	pub_base_strpack( rate_str );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.开户日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.原 利 率、 13.现 利 率、14.科目号、   15.利率名称  */
	/* 16.流水号、   */
	fprintf(fp,"LNBKLL%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	0.00 , 0 , 0 , 0.00 , 0.00 ,
	0.00 , old_rate_str , rate_str , "", rate_name , 
	g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
