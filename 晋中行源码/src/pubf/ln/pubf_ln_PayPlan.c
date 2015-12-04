/*************************************************
* 文 件 名: pubf_ln_PayPlan.c
* 功能描述：   
* 			pub_ln_cal_PayPlan     计算按揭贷款每期需还本金和利息
* 			pub_ln_PayPlan         生成还款计划
* 			pub_ln_Chg_PayPlan     要素变化,重新生成还款计划
*
* 作    者:    rob
* 完成日期：   2003年12月28日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"

/**********************************************************************
* 函 数 名：  pub_ln_cal_PayPlan
* 函数功能：  计算每期需还本金和利息
* 作    者：  rob
* 完成时间：  2003年12月28日
*
* 参    数：
*     输  入：ttl_amt   decimail(16,2) 贷款总额   
*             ttl_cnt   smailint       总期次
*             pay_typ   char(1)        按揭方式（等额、等本）
				0等额还本付息1等额还本按期还息
*             rate      decimal(8,5)   利率
*	          cnt       smallint       本期期数
*      
*     输  出:           
*	          amt       decimal(16,2)  本期应还本金			
*	          intst     decimal(16,2)  本期应还利息
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
********************************************************************/
int pub_ln_cal_PayPlan ( double ttl_amt , int ttl_cnt , char *pay_typ , 
	double rate , int cnt , double *amt , double *intst )
{
    double tmp1,tmp2;

	/***
	rate = rate/1000;
	****/
	sprintf(acErrMsg,"ttl_amt=[%lf],ttl_cnt=[%ld],rate=[%lf],cnt=[%ld]",ttl_amt,ttl_cnt,rate,cnt);
	WRITEMSG
	switch( atoi( pay_typ ) )
	{
	case 0:		/*等额还款*/
		tmp1 = pow((1+rate),ttl_cnt);
		
		tmp2 = ttl_amt * rate * tmp1 /(tmp1-1);
		tmp1 = pow((1+rate),(ttl_cnt - cnt+1));
		*amt = pub_base_PubDround_1(tmp2/tmp1);
		*intst = tmp2 - *amt;
		*intst = pub_base_PubDround_1(*intst);
		break;

	case 1:		/*等本还款*/
		*amt = pub_base_PubDround_1(ttl_amt / ttl_cnt);
		*intst = pub_base_PubDround_1((1.0-(cnt-1.0)/ttl_cnt)*ttl_amt*rate);
		break;
	
	default:
		strcpy(g_pub_tx.reply,"L013");
		sprintf(acErrMsg,"按揭方式参数错误 pay_typ= '%s' ", *pay_typ);
		WRITEMSG
		return 1;

	}		

	return 0;
}


/**********************************************************************
* 函 数 名：  pub_ln_PayPlan
* 函数功能：  生成还款计划
* 作    者：  rob
* 完成时间：  2004年01月20日
*
* 参    数：
*     输  入：
*	      ln_mst	贷款主文件结构
*             ln_parm   贷款参数表结构
*	      cnt        smallint      本期期数
*         type        生成原因 0正常 1因为修改而生成
*      
*     输  出:  
*             ln_pay_pln 按揭贷款还款计划结构 
*                    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
********************************************************************/
int pub_ln_PayPlan( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , int cnt , 
				struct ln_pay_pln_c *p_ln_pay_pln , int type )
{
	long fir_day=0,term_opn_date=0;
	long month_cnt=0;
	double tmp1=0.00,tmp2=0.00,ln_amt=0.00;
	int ret=0;
	int iAc_seqn = 0; /***Add By zgf 2015/4/16 ***/
	struct ln_pay_pln_c	t_ln_pay_pln;
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	TRACE
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	memset(p_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	memset( &t_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	TRACE
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);

	if( p_ln_mst.opn_date==p_ln_mst.ic_date )
	{
		type=0;
	}else{
		type=1;
	}
	
	/* 计算总期数 */
	month_cnt = pub_base_CalMouths( p_ln_mst.ic_date , p_ln_mst.mtr_date ); 
	if( ret ) return 1;
	sprintf(acErrMsg,"总期数month_cnt[%ld]",month_cnt);
	WRITEMSG	
	/* 长沙系统－若是修改后的数据*/
	if( type==1 )
	{
		/* 若在还款日当日前 */
		if( term_opn_date%100 < p_ln_parm.fst_pay_day )
		{
			month_cnt=month_cnt;
		}else{
			month_cnt=month_cnt+1;	
		}	
	}
		
	if( p_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%p_ln_parm.pay_amt_term==0 )
		{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term ;
		}else{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term +1;
		}	
	}
	sprintf(acErrMsg,"总期数p_ln_pay_pln->ttl_cnt[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG	
	/* 计算每期开始归还日期、到期日期 */
	if(cnt==1)
	{
		term_opn_date = p_ln_mst.ic_date;	
	}else{
		ret = pub_base_deadlineM(p_ln_mst.ic_date,(cnt-1)*p_ln_parm.pay_amt_term,&term_opn_date);
		if(ret) return 1;
	}	
	sprintf(acErrMsg,"term_opn_date[%ld]",term_opn_date);
	WRITEMSG			
	if( p_ln_parm.pay_term_type[0]=='M' )
	{	
		if( p_ln_parm.fst_pay_ind[0]=='M' )
		{
			/* 若在还款日当日前 */
			if( term_opn_date%100 <= p_ln_parm.fst_pay_day )
			{
				p_ln_pay_pln->beg_date=term_opn_date/100*100+p_ln_parm.fst_pay_day;
				p_ln_pay_pln->end_date=term_opn_date/100*100+p_ln_parm.mon_pay_day;	
			}else{
				/* 若在还款日当日后 */
				if( term_opn_date%10000<=1200 )/*若非12月份*/
				{
			 		p_ln_pay_pln->beg_date=term_opn_date/100*100+100+p_ln_parm.fst_pay_day;
			   		p_ln_pay_pln->end_date=term_opn_date/100*100+100+p_ln_parm.mon_pay_day;
				}else{
					p_ln_pay_pln->beg_date=term_opn_date/10000*10000+10100+p_ln_parm.fst_pay_day;
					p_ln_pay_pln->end_date=term_opn_date/10000*10000+10100+p_ln_parm.mon_pay_day;
				}

			}
		}else if( p_ln_parm.fst_pay_ind[0]=='F' ){
			p_ln_pay_pln->beg_date = term_opn_date;
			p_ln_pay_pln->end_date = term_opn_date;			
		}
	}	
	/* 若非修改 */
	if( type==0 )
	{
		if( p_ln_mst.opn_date%10000<=1221 )	/*若是1221号之前*/
		{
			/* 按照长沙商行老数据要求，日期顺延一个月 */
			pub_base_deadlineM( p_ln_pay_pln->beg_date , 1 , &p_ln_pay_pln->beg_date );
			pub_base_deadlineM( p_ln_pay_pln->end_date , 1 , &p_ln_pay_pln->end_date );	
		}
	}
	
	/* 若最后一期修改为到期日期 */
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->beg_date = p_ln_mst.mtr_date;
		p_ln_pay_pln->end_date = p_ln_mst.mtr_date;			
	}
	sprintf(acErrMsg,"Jameslybeg_date[%ld]end_date[%ld]",p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date);
	WRITEMSG			
	/***************************
	 				
	if ( p_ln_parm.pay_term_type[0]=='Y' )
	{
		* 确定首日期 *
		sprintf(acErrMsg,"年!周期[%d]",p_ln_parm.pay_amt_term);
		WRITEMSG
		sprintf(acErrMsg,"期次[%d]",cnt);
		WRITEMSG
		fir_day = (p_ln_mst.ic_date/10000)*10000 + 101;
		sprintf(acErrMsg,"首日期[%ld]",fir_day);
		WRITEMSG
		* 开始归还日期 *
		month = p_ln_parm.pay_amt_term * cnt * 12;
		sprintf(acErrMsg,"月数[%d]",month);
		WRITEMSG
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->beg_date);
		if (ret)
		{
			sprintf(acErrMsg,"调用函数pub_base_deadlineM错误!");
			WRITEMSG
			return 1;	
		}
		sprintf(acErrMsg,"开始归还日期[%ld]",p_ln_pay_pln->beg_date);
		WRITEMSG
		
		* 到期日期 *
		month = p_ln_parm.pay_amt_term * (cnt+1) * 12;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->end_date );
		if (ret)
		{
			sprintf(acErrMsg,"调用函数pub_base_deadlineM错误!");
			WRITEMSG
			return 1;	
		}	
		p_ln_pay_pln->end_date = pub_base_daynumSUB(p_ln_pay_pln->end_date, 1);
		sprintf(acErrMsg,"到期日期[%ld]",p_ln_pay_pln->end_date);
		WRITEMSG
		
		* 总期数 *
		if ( (p_ln_mst.mtr_date - fir_day) > 10000*p_ln_parm.pay_amt_term )
		{
				if ( (p_ln_mst.mtr_date - fir_day) % (10000*p_ln_parm.pay_amt_term) )	
				{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(10000*p_ln_parm.pay_amt_term);
				}else{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(10000*p_ln_parm.pay_amt_term)-1;
				}
		}else{
			p_ln_pay_pln->ttl_cnt=1;
		}					
	}
	
	if ( p_ln_parm.pay_term_type[0]=='Q' )
	{
		* 确定首日期 *
		switch((p_ln_mst.ic_date%10000)/100)
		{
			case 1:
			case 2:
			case 3:	fir_day=(p_ln_mst.ic_date/10000)*10000 + 101;
				break;
			case 4:
			case 5:
			case 6: fir_day=(p_ln_mst.ic_date/10000)*10000 + 401;
				break;
			case 7:
			case 8:
			case 9: fir_day=(p_ln_mst.ic_date/10000)*10000 + 701;
				break;		
			case 10:
			case 11:
			case 12: fir_day=(p_ln_mst.ic_date/10000)*10000 + 1001;
				break;	
		}	
		* 开始归还日期 *
		month = p_ln_parm.pay_amt_term * cnt * 3;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->beg_date );
		if (ret)
		{
			sprintf(acErrMsg,"调用函数pub_base_deadlineM错误!");
			WRITEMSG
			return 1;	
		}
		* 到期日期 *
		month = p_ln_parm.pay_amt_term * (cnt+1) * 3;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->end_date );
		if (ret)
		{
			sprintf(acErrMsg,"调用函数pub_base_deadlineM错误!");
			WRITEMSG
			return 1;	
		}	
		p_ln_pay_pln->end_date = pub_base_daynumSUB(p_ln_pay_pln->end_date, 1);
		* 总期数 *
		if ( (p_ln_mst.mtr_date - fir_day) > 300*p_ln_parm.pay_amt_term )
		{
				if ( (p_ln_mst.mtr_date - fir_day) % (300*p_ln_parm.pay_amt_term) )	
				{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(300*p_ln_parm.pay_amt_term);
				}else{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(300*p_ln_parm.pay_amt_term)-1;
				}
		}else{
			p_ln_pay_pln->ttl_cnt=1;
		}					
	}						
	if ( p_ln_parm.pay_term_type[0]=='M' )
	{
		* 确定首日期 *
		fir_day = p_ln_mst.ic_date/100+1;
		* 开始归还日期 *
		month = p_ln_parm.pay_amt_term * cnt;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->beg_date );
		if (ret)
		{
			sprintf(acErrMsg,"调用函数pub_base_deadlineM错误!");
			WRITEMSG
			return 1;	
		}
		* 到期日期 *
		month = p_ln_parm.pay_amt_term * (cnt+1) * 3;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->end_date );
		if (ret)
		{
			sprintf(acErrMsg,"调用函数pub_base_deadlineM错误!");
			WRITEMSG
			return 1;	
		}	
		p_ln_pay_pln->end_date = pub_base_daynumSUB(p_ln_pay_pln->end_date, 1);
		* 总期数 *
		if ( (p_ln_mst.mtr_date - fir_day) > 100*p_ln_parm.pay_amt_term )
		{
				if ( (p_ln_mst.mtr_date - fir_day) % (100*p_ln_parm.pay_amt_term) )	
				{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(100*p_ln_parm.pay_amt_term);
				}else{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(100*p_ln_parm.pay_amt_term)-1;
				}
		}else{
			p_ln_pay_pln->ttl_cnt=1;
		}					
	}							
	*****************************/
	/* 计算当期应还 */
	
	p_ln_mst.rate = p_ln_mst.rate/1000;
	
	ret = pub_base_CompDblVal( p_ln_mst.bal, 0.00 );
	if ( ret==0 )
	{
		TRACE
		if(p_ln_mst.ic_date = g_pub_tx.tx_date )
		{
			TRACE
			ln_amt = p_ln_mst.amt_lmt;
		}
	}else{
		if( type==1)
		{
			TRACE
			ln_amt = p_ln_mst.bal;
		}else if ( type==0 ){						
		/* 取原还款计划表 */
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%d and pay_ind='0' " , p_ln_mst.ac_id , p_ln_mst.ac_seqn); 
		if( ret==100 )
		{
			sprintf(acErrMsg,"还款计划表无相关记录[%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L135");
			return 1;
		}else if( ret ){
			sprintf(acErrMsg,"查询还款计划表是异常出错![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L136");
			return 1;
		}			
		sprintf(acErrMsg,"rob!!![%lf]",t_ln_pay_pln.ttl_amt);
		WRITEMSG
		ln_amt = t_ln_pay_pln.ttl_amt;	
		TRACE
		}
	}
	sprintf(acErrMsg,"ln_amt=[%lf]!",ln_amt);
	WRITEMSG	
	switch( atoi( p_ln_parm.ln_pay_type ) )
	{
	case 3:		/*等额还款*/
		tmp1 = pow((1+p_ln_mst.rate),p_ln_pay_pln->ttl_cnt);
		
		tmp2 = ln_amt * p_ln_mst.rate * tmp1 /(tmp1-1);
		tmp1 = pow((1+p_ln_mst.rate),(p_ln_pay_pln->ttl_cnt - cnt+1));
		p_ln_pay_pln->curr_amt= pub_base_PubDround_1(tmp2/tmp1);
		p_ln_pay_pln->curr_intst = tmp2 - p_ln_pay_pln->curr_amt; 
		p_ln_pay_pln->curr_intst = pub_base_PubDround_1(p_ln_pay_pln->curr_intst);
		break;

	case 4:		/*等本还款*/
		p_ln_pay_pln->curr_amt = pub_base_PubDround_1(ln_amt / p_ln_pay_pln->ttl_cnt);
		p_ln_pay_pln->curr_intst = pub_base_PubDround_1((1.0-(cnt-1.0)/p_ln_pay_pln->ttl_cnt)*ln_amt*p_ln_mst.rate);
		break;
	
	default:
		strcpy(g_pub_tx.reply,"O002");
		sprintf(acErrMsg,"贷款支付方式与按揭贷款支付方式不符,请联系中心机房! pay_typ= '%s' ",p_ln_parm.ln_pay_type );
		WRITEMSG
		return 1;

	}		
	/* 若最后一期修改所欠本金为贷款余额 */
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->curr_amt = p_ln_mst.bal;		
	}
		
	/* 对还款计划中其他值赋值 */
	
	p_ln_pay_pln->ttl_amt=ln_amt; /* 原始本金 */
	p_ln_pay_pln->ac_id=p_ln_mst.ac_id; /* 账号ID */
	p_ln_pay_pln->ac_seqn=p_ln_mst.ac_seqn; /* 账号序号 */	
	p_ln_pay_pln->curr_cnt=cnt; /* 当前期次 */		
			p_ln_pay_pln->wrk_date=g_pub_tx.tx_date; /* 登记日期 */	
			strcpy ( p_ln_pay_pln->pay_ind , "0" );	/* 还款状态0-未还 */
			p_ln_pay_pln->de_ac_id = p_ln_mst.repay_ac_id ;	/* 还款账户ID */
			/* 根据币种生成活期户的账号ID */ 
			/**Mod by zgf 2014/4/2 32位-64位迁移,long指针赋值给int型指针错误 start*/
			/*ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &p_ln_pay_pln->de_ac_seqn );*/
                        ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &iAc_seqn );
                        /**Mod by zgf 2015/4/16 32位-64位迁移,long指针赋值给int型指针错误 end **/
			if ( ret )
			{
				sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
				WRITEMSG
				return 1;	
			}			
			p_ln_pay_pln->de_ac_seqn = iAc_seqn;/**Mod by zgf 2015/4/16***/		

			sprintf(acErrMsg,"原始本金[%lf]",p_ln_pay_pln->ttl_amt);
			WRITEMSG
			sprintf(acErrMsg,"账号ID[%ld]",p_ln_pay_pln->ac_id);
			WRITEMSG
	sprintf(acErrMsg,"账号序号[%d]",p_ln_pay_pln->ac_seqn);
	WRITEMSG
	sprintf(acErrMsg,"当前期次[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG
	sprintf(acErrMsg,"登记日期[%ld]",p_ln_pay_pln->wrk_date);
	WRITEMSG
	sprintf(acErrMsg,"还款状态0[%s]",p_ln_pay_pln->pay_ind);
	WRITEMSG
	
	
	sprintf(acErrMsg,"总期数[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG
	sprintf(acErrMsg,"当前期次应还本金[%lf]",p_ln_pay_pln->curr_amt);
	WRITEMSG
	sprintf(acErrMsg,"当前期次应还利息[%lf]",p_ln_pay_pln->curr_intst);
	WRITEMSG
	sprintf(acErrMsg,"开始归还日期[%ld]",p_ln_pay_pln->beg_date);
	WRITEMSG
	sprintf(acErrMsg,"到期归还日期[%ld]",p_ln_pay_pln->end_date);
	WRITEMSG
				
	return 0;	
}
/**********************************************************************
* 函 数 名：  pub_ln_Chg_PayPlan
* 函数功能：  还款计划变更
* 作    者：  rob
* 完成时间：  2004年04月01日
*
* 参    数：
*     输  入：
*	      ln_mst	贷款主文件结构
*             ln_parm   贷款参数表结构
*	      rate      更改后的利率
*	      ln_bal	更改后的本金
*	      beg_date	起始日期 
*	      end_date	终止日期   
*    
*     输  出:   
*                    
*
*     返回值： 0 成功
*            -1  失败
*             1  由于限制不能修改
*
* 修改历史：
********************************************************************/
int pub_ln_Chg_PayPlan( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , double rate , 
			double ln_bal , long beg_date , long end_date )
{
	long fir_day=0,term_opn_date=0,max_day=0,old_opn_date=0;
	long month_cnt=0;
	int ret=0 , cnt=0 , ln_cnt=0,now_ttl_cnt=0;
	int chg_flag=0;
	long dqr=0;
	double rate_val=0.00;
	struct ln_mst_c    ln_mst_tmp;
	struct ln_mst_c    ln_mst_upd;	
	struct ln_pay_pln_c    t_ln_pay_pln;
	struct ln_pay_pln_c    ln_pay_pln;
	
	memset ( &ln_mst_upd , 0x00 , sizeof(struct ln_mst_c) );			
	memset ( &ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );
	memset ( &t_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset ( &ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
		
	/* 判断是否有欠款 */
	/* 查询按揭欠款表 */
	ret = sql_count( "ln_lo" , "ac_id=%ld and ac_seqn=%d  and pay_type='0'" , p_ln_mst.ac_id,p_ln_mst.ac_seqn );
	if (ret)
	{
		sprintf( acErrMsg, "该贷款存在欠款,不能重新生成还款计划[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L084");
		return 1;
	}		
	
	/* 检查当期是否以还 */

	ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%d and pay_ind='0'"
					 , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款还款计划表信息中无此信息 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L043" );
		return -1;
	}else if( ret ){
    		sprintf( acErrMsg,"取贷款还款计划表信息异常erro code=[%s]",ret);
		WRITEMSG
    		return -1;
  	}
  				
	ret = pub_base_deadlineM( t_ln_pay_pln.beg_date, -1, &dqr);
	if( ret )	return -1;
	sprintf( acErrMsg,"dqr =[%ld]",dqr);
	WRITEMSG 	
	/***	20040830
  	if( g_pub_tx.tx_date >= dqr )
  	****/
  	if( g_pub_tx.tx_date > dqr )
  	{	
  		sprintf( acErrMsg, "按揭贷款当期未还,不能重新生成还款计划");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L085");
		return 1;
	}	
	/* 若是第一期 */
	if( t_ln_pay_pln.curr_cnt==1 && p_ln_mst.opn_date==p_ln_mst.ic_date )
	{
  		sprintf( acErrMsg, "按揭贷款当期未还,不能重新生成还款计划");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L085");
		return 1;
	}		
	
	/* 对修改项进行检查 */
	/* 金额 */
	ret = pub_base_CompDblVal( p_ln_mst.bal, ln_bal );
	if ( ret )	/*不能还清*/
	{
		p_ln_mst.bal = ln_bal; 
		chg_flag++;
	}else{
		/*还清的情况*/
		p_ln_mst.bal=0;
	}
	/* 到期日期(提前) */
	if( end_date < p_ln_mst.mtr_date )
	{
		p_ln_mst.mtr_date = end_date ;
		chg_flag++;		
	}
	/* 到期日期(滞后) */
	if( end_date > p_ln_mst.mtr_date )
	{
		p_ln_mst.mtr_date = end_date ;
		chg_flag++;	
		/******暂时不考虑展期问题,可以随便延期,不受限制 
		if ( p_ln_parm.exp_ind[0]=='N' )
		{
			sprintf(acErrMsg,"此贷款不允许展期,输入日期不能超过贷款到期日!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L086");
			return 1;
		}					
		memcpy( ln_mst_tmp , g_ln_mst , sizeof( ln_mst_c ) );
		g_ln_mst.mtr_date = end_date;
		* 贷款展期检查 *
   		*检查可否展期标志*
		if( !strcmp( p_ln_mst.exp_ind[0],p_ln_parm.exp_ind[0] ) )
		{
			strcpy(acErrMsg,"该贷款产品已超过展期次数，不可展期!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L008" );
			return 1;
		}
		* 检查贷款展期后是否超过贷款的最大期限 *		
		if ( p_ln_parm.term_type[0]=='Y' )    		* 期限类型：年 *
		{	
			p_ln_parm.max_term = p_ln_parm.max_term*12;
			pub_base_deadlineM( p_ln_mst.ic_date,p_ln_parm.max_term,&max_day);
		}else if ( p_ln_parm.term_type[0]=='M' ){ 	* 期限类型：月 *
			pub_base_deadlineM( p_ln_mst.ic_date,p_ln_parm.max_term,&max_day);
		}else if ( p_ln_parm.term_type[0]=='D' ){ 	* 期限类型：日 *		  
			pub_base_deadlineD( p_ln_mst.ic_date,p_ln_parm.max_term,&max_day);
		}
        	if ( p_ln_mst.mtr_date>max_day )
		{
                	strcpy( acErrMsg,"贷款期限大于规定的最大期限!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L004" );
			return 1;
		}
		* 修改展期次数 *	
		if ( p_ln_mst.exp_ind[0]=='Y' )
		{
			strcpy ( p_ln_mst.exp_ind , "1" );
		}else{
			cnt = atoi ( p_ln_mst.exp_ind );
			sprintf( p_ln_mst.exp_ind , "%d" , cnt+1 );
		}
		*************************************/	
	}
	/* 利率 */
	ret = pub_base_CompDblVal( p_ln_mst.rate, rate );
	if ( ret!=0 )
	{
		p_ln_mst.rate = rate;
		/* 利率检查 */
		/* 检查利率是否超过规定利率上浮和下浮后的界限 */
		sprintf(acErrMsg,"[%s]",p_ln_parm.rate_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%s]",p_ln_parm.cur_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
		WRITEMSG		
		ret = pub_base_getllz( p_ln_parm.rate_no, p_ln_parm.cur_no, g_pub_tx.tx_date , &rate_val );
		if( ret )
		{
			sprintf(acErrMsg,"调用根据利率编号取得利率函数错误!");
			WRITEMSG
			return -1;	
		}
		rate_val = rate_val/1.2;/* 转换成月利率 */
 	    	if( pub_base_CompDblVal(p_ln_mst.rate,rate_val*(1+p_ln_parm.max_flt_ratio/100) )>0 )
	    	{
                	sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",rate_val,rate_val*(1+p_ln_parm.min_flt_ratio/100),p_ln_parm.min_flt_ratio );
                	WRITEMSG
               	 	strcpy ( g_pub_tx.reply , "A006" );
                	return -1;
            	}
            	if( pub_base_CompDblVal(p_ln_mst.rate,rate_val*(1+p_ln_parm.min_flt_ratio/100) )<0 )
            	{
                	sprintf(acErrMsg,"current rate is too high[%lf]!!",rate_val*(1-p_ln_parm.min_flt_ratio/100));
               	 	WRITEMSG
                	strcpy ( g_pub_tx.reply , "A005" );
                	return -1;
            	}							
		chg_flag++;
	}
	/* 没有修改任何信息 */
	if ( chg_flag==0 )
	{
		sprintf(acErrMsg,"未修改任何信息,请检查!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L087");
		return 1;
	}
	/* 更新贷款主文件记录 */	
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d " , p_ln_mst.ac_id,p_ln_mst.ac_seqn );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		return -1;
	}
	ret = Ln_mst_Fet_Upd( &ln_mst_upd , g_pub_tx.reply );
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
		WRITEMSG
		return -1;
	}else if (ret == 100){
		return -1;
	}
	p_ln_mst.ic_date = beg_date;
	
	p_ln_mst.ic_date = dqr+1;

	p_ln_mst.lst_date = g_pub_tx.tx_date;
	ret = Ln_mst_Upd_Upd( p_ln_mst , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"更新贷款主文件错误!!");
               WRITEMSG
               return -1;
        }          
	Ln_mst_Clo_Upd();
/*	ret = sql_execute( "UPDATE ln_mst SET bal=%lf , rate=%lf , mtr_date=%ld , lst_date=%ld  WHERE ac_id=%ld and ac_seqn=%d " , 
			p_ln_mst.bal,p_ln_mst.rate,p_ln_mst.mtr_date,g_pub_tx.tx_date, p_ln_mst.ac_id,p_ln_mst.ac_seqn );
	if( ret ) return ret;	
*/
	/* 删除当期还款计划 */
	ret = sql_execute( "DELETE from ln_pay_pln WHERE ac_id=%ld and ac_seqn=%d and pay_ind='0' " , 
			 		p_ln_mst.ac_id,p_ln_mst.ac_seqn );
	if( ret ) return ret;			 							
	/* 生成新的还款计划 */
	/*****
	old_opn_date = p_ln_mst.opn_date; * 保留原来的开户日期 *
	p_ln_mst.ic_date = beg_date;	
	*****/
	/* 计算总期数 */
	month_cnt = pub_base_CalMouths( p_ln_mst.ic_date , p_ln_mst.mtr_date ); 
	if( ret ) return -1;
		
	if( p_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%g_ln_parm.pay_amt_term==0 )
		{
			ln_cnt = month_cnt/p_ln_parm.pay_amt_term ;
		}else{
			ln_cnt = month_cnt/p_ln_parm.pay_amt_term +1;
		}	
	}	
	/* 生成还款计划 */
	ret = pub_ln_PayPlan( p_ln_mst , p_ln_parm , 1 , &ln_pay_pln , 1 );	
	if (ret)
	{
		sprintf(acErrMsg,"调用函数pub_ln_PayPlan错误!");
		WRITEMSG
		return -1;	
	}
	/* 修改还款计划归还日期 */
	ln_pay_pln.beg_date= t_ln_pay_pln.beg_date;
	ln_pay_pln.end_date= t_ln_pay_pln.end_date;
	
	/* 修改期数 */
	ln_pay_pln.ttl_cnt = t_ln_pay_pln.ttl_cnt - t_ln_pay_pln.curr_cnt + 1;

	sprintf(acErrMsg,"ttl_cnt=[%d]++curr_cnt=[%d]!",t_ln_pay_pln.ttl_cnt,t_ln_pay_pln.curr_cnt);
	WRITEMSG	
	sprintf(acErrMsg,"now_ttl_cnt=[%d]!",ln_pay_pln.ttl_cnt);
	WRITEMSG
														
	/* 添加还款计划表 */	
	ret = Ln_pay_pln_Ins( ln_pay_pln , g_pub_tx.reply );
	if(ret)
	{
		sprintf(acErrMsg,"登记还款计划表失败");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L077");
		return -1;
	}	
				
	return 0;	
}
/**********************************************************************
* 函 数 名：  pub_ln_PayPlan_qry
* 函数功能：  查询还款计划
* 作    者：  rob
* 完成时间：  2004年01月20日
*
* 参    数：
*     输  入：
*	      ln_mst	贷款主文件结构
*             ln_parm   贷款参数表结构
*	      cnt        smallint      本期期数
*         type        生成原因 0正常 1因为修改而生成
*      
*     输  出:  
*             ln_pay_pln 按揭贷款还款计划结构 
*                    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
********************************************************************/
int pub_ln_PayPlan_qry( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , int cnt , 
				struct ln_pay_pln_c *p_ln_pay_pln , int type )
{
	long fir_day=0,term_opn_date=0;
	long month_cnt=0;
	double tmp1=0.00,tmp2=0.00,ln_amt=0.00;
	int ret=0;
	int iAc_seqn = 0; /***Add By zgf 2015/4/16 ***/
	struct ln_pay_pln_c	t_ln_pay_pln;
	TRACE
	memset(p_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset( &t_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	TRACE
	
	if( p_ln_mst.opn_date==p_ln_mst.ic_date )
	{
		type=0;
	}else{
		type=1;
	}
	
	/* 计算总期数 */
	month_cnt = pub_base_CalMouths( p_ln_mst.ic_date , p_ln_mst.mtr_date ); 
	if( ret ) return 1;
	sprintf(acErrMsg,"总期数month_cnt[%ld]",month_cnt);
	WRITEMSG	
	/* 长沙系统－若是修改后的数据*/
	if( type==1 )
	{
		/* 若在还款日当日前 */
		if( term_opn_date%100 < p_ln_parm.fst_pay_day )
		{
			month_cnt=month_cnt;
		}else{
			month_cnt=month_cnt+1;	
		}	
	}
		
	if( p_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%p_ln_parm.pay_amt_term==0 )
		{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term ;
		}else{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term +1;
		}	
	}
	sprintf(acErrMsg,"总期数p_ln_pay_pln->ttl_cnt[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG	
	/* 计算每期开始归还日期、到期日期 */
	if(cnt==1)
	{
		term_opn_date = p_ln_mst.ic_date;	
	}else{
		ret = pub_base_deadlineM(p_ln_mst.ic_date,(cnt-1)*p_ln_parm.pay_amt_term,&term_opn_date);
		if(ret) return 1;
	}	
	sprintf(acErrMsg,"term_opn_date[%ld]",term_opn_date);
	WRITEMSG			
	if( p_ln_parm.pay_term_type[0]=='M' )
	{	
		if( p_ln_parm.fst_pay_ind[0]=='M' )
		{
			/* 若在还款日当日前 */
			if( term_opn_date%100 <= p_ln_parm.fst_pay_day )
			{
				p_ln_pay_pln->beg_date=term_opn_date/100*100+p_ln_parm.fst_pay_day;
				p_ln_pay_pln->end_date=term_opn_date/100*100+p_ln_parm.mon_pay_day;	
			}else{
				/* 若在还款日当日后 */
				if( term_opn_date%10000<=1200 )/*若非12月份*/
				{
			 		p_ln_pay_pln->beg_date=term_opn_date/100*100+100+p_ln_parm.fst_pay_day;
			   		p_ln_pay_pln->end_date=term_opn_date/100*100+100+p_ln_parm.mon_pay_day;
				}else{
					p_ln_pay_pln->beg_date=term_opn_date/10000*10000+10100+p_ln_parm.fst_pay_day;
					p_ln_pay_pln->end_date=term_opn_date/10000*10000+10100+p_ln_parm.mon_pay_day;
				}

			}
		}else if( p_ln_parm.fst_pay_ind[0]=='F' ){
			p_ln_pay_pln->beg_date = term_opn_date;
			p_ln_pay_pln->end_date = term_opn_date;			
		}
	}	
	/* 若非修改 */
	if( type==0 )
	{
		if( p_ln_mst.opn_date%10000<=1221 )
		{
			/* 按照长沙商行老数据要求，日期顺延一个月 */
			pub_base_deadlineM( p_ln_pay_pln->beg_date , 1 , &p_ln_pay_pln->beg_date );
			pub_base_deadlineM( p_ln_pay_pln->end_date , 1 , &p_ln_pay_pln->end_date );	
		}
	}
	
	/* 若最后一期修改为到期日期 */
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->beg_date = p_ln_mst.mtr_date;
		p_ln_pay_pln->end_date = p_ln_mst.mtr_date;			
	}
	sprintf(acErrMsg,"Jameslybeg_date[%ld]end_date[%ld]",p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date);
	WRITEMSG	
			
	/* 计算当期应还 */
	p_ln_mst.rate = p_ln_mst.rate/1000;
	
	ret = pub_base_CompDblVal( p_ln_mst.bal, 0.00 );
	if ( ret==0 )
	{
		TRACE
		if(p_ln_mst.ic_date = g_pub_tx.tx_date )
		{
			TRACE
			ln_amt = p_ln_mst.amt_lmt;
		}
	}else{						
		/* 取原还款计划表 */
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%d and pay_ind='0' " , p_ln_mst.ac_id , p_ln_mst.ac_seqn); 
		if( ret==100 )
		{
			sprintf(acErrMsg,"还款计划表无相关记录[%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L135");
			return 1;
		}else if( ret ){
			sprintf(acErrMsg,"查询还款计划表是异常出错![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L136");
			return 1;
		}			
		sprintf(acErrMsg,"rob!!![%lf]",t_ln_pay_pln.ttl_amt);
		WRITEMSG
		ln_amt = t_ln_pay_pln.ttl_amt;	
		TRACE
	}
	sprintf(acErrMsg,"ln_amt=[%lf]!",ln_amt);
	WRITEMSG	
	/****************
	switch( atoi( p_ln_parm.ln_pay_type ) )
	{
	case 3:		*等额还款*
		tmp1 = pow((1+p_ln_mst.rate),p_ln_pay_pln->ttl_cnt);
		
		tmp2 = ln_amt * p_ln_mst.rate * tmp1 /(tmp1-1);
		tmp1 = pow((1+p_ln_mst.rate),(p_ln_pay_pln->ttl_cnt - cnt+1));
		p_ln_pay_pln->curr_amt= pub_base_PubDround_1(tmp2/tmp1);
		p_ln_pay_pln->curr_intst = tmp2 - p_ln_pay_pln->curr_amt; 
		p_ln_pay_pln->curr_intst = pub_base_PubDround_1(p_ln_pay_pln->curr_intst);
		break;

	case 4:		*等本还款*
		p_ln_pay_pln->curr_amt = pub_base_PubDround_1(ln_amt / p_ln_pay_pln->ttl_cnt);
		p_ln_pay_pln->curr_intst = pub_base_PubDround_1((1.0-(cnt-1.0)/p_ln_pay_pln->ttl_cnt)*ln_amt*p_ln_mst.rate);
		break;

	default:
		strcpy(g_pub_tx.reply,"O002");
		sprintf(acErrMsg,"贷款支付方式与按揭贷款支付方式不符,请联系中心机房! pay_typ= '%s' ",p_ln_parm.ln_pay_type );
		WRITEMSG
		return 1;

	}
	*******/
	
	/* 计算还款计划 *
	if(  p_ln_parm.ln_pay_type[0]=='3' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , p_ln_pay_pln->ttl_cnt , "0" , 
			p_ln_mst.rate , cnt , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}
	if(  p_ln_parm.ln_pay_type[0]=='4' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , p_ln_pay_pln->ttl_cnt , "1" , 
			p_ln_mst.rate , cnt , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}			
	***/


	p_ln_pay_pln->ttl_cnt = p_ln_pay_pln->ttl_cnt - p_ln_pay_pln->curr_cnt;
	ln_amt= p_ln_mst.bal;
	
	if(  p_ln_parm.ln_pay_type[0]=='3' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , p_ln_pay_pln->ttl_cnt , "0" , 
			p_ln_mst.rate , 1 , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}
	if(  p_ln_parm.ln_pay_type[0]=='4' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , p_ln_pay_pln->ttl_cnt , "1" , 
			p_ln_mst.rate , 1 , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}		
	

	/* 若最后一期修改所欠本金为贷款余额 */
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->curr_amt = p_ln_mst.bal;		
	}
		
	/* 对还款计划中其他值赋值 */
	p_ln_pay_pln->ttl_amt=ln_amt; /* 原始本金 */
	p_ln_pay_pln->ac_id=p_ln_mst.ac_id; /* 账号ID */
	p_ln_pay_pln->ac_seqn=p_ln_mst.ac_seqn; /* 账号序号 */	
	p_ln_pay_pln->curr_cnt=cnt; /* 当前期次 */		
	p_ln_pay_pln->wrk_date=g_pub_tx.tx_date; /* 登记日期 */	
	strcpy ( p_ln_pay_pln->pay_ind , "0" );	/* 还款状态0-未还 */
	p_ln_pay_pln->de_ac_id = p_ln_mst.repay_ac_id ;	/* 还款账户ID */
	/* 根据币种生成活期户的账号ID */ 
	/**Mod by zgf 2015/4/16 32位-64位迁移,long指针赋值给int型指针错误 start*/	
	/*ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &p_ln_pay_pln->de_ac_seqn );*/
	ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &iAc_seqn );
	if ( ret )
	{
		sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
		WRITEMSG
		return 1;	
	}			
	p_ln_pay_pln->de_ac_seqn = iAc_seqn;/**Mod by zgf 2015/4/16***/	
	sprintf(acErrMsg,"orange!!!!!!!!!第[%d]期",p_ln_pay_pln->curr_cnt);
	WRITEMSG	
	sprintf(acErrMsg,"当前期次[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG				
	sprintf(acErrMsg,"原始本金[%lf]",p_ln_pay_pln->ttl_amt);
	WRITEMSG
	sprintf(acErrMsg,"账号ID[%ld]",p_ln_pay_pln->ac_id);
	WRITEMSG
	sprintf(acErrMsg,"账号序号[%d]",p_ln_pay_pln->ac_seqn);
	WRITEMSG
	sprintf(acErrMsg,"当前期次[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG
	sprintf(acErrMsg,"登记日期[%ld]",p_ln_pay_pln->wrk_date);
	WRITEMSG
	sprintf(acErrMsg,"还款状态0[%s]",p_ln_pay_pln->pay_ind);
	WRITEMSG
	
	
	sprintf(acErrMsg,"总期数[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG
	sprintf(acErrMsg,"当前期次应还本金[%lf]",p_ln_pay_pln->curr_amt);
	WRITEMSG
	sprintf(acErrMsg,"当前期次应还利息[%lf]",p_ln_pay_pln->curr_intst);
	WRITEMSG
	sprintf(acErrMsg,"开始归还日期[%ld]",p_ln_pay_pln->beg_date);
	WRITEMSG
	sprintf(acErrMsg,"到期归还日期[%ld]",p_ln_pay_pln->end_date);
	WRITEMSG
				
	return 0;	
}






/**********************************************************************
* 函 数 名：  pub_ln_PayPlan_Qry
* 函数功能：  查询还款计划
* 作    者：  rob
* 完成时间：  2004年01月20日
*
* 参    数：
*     输  入：
*	      ln_mst	贷款主文件结构
*             ln_parm   贷款参数表结构
*	      cnt       smallint      本期期数
*             ttl_cnt   smallint      剩余总期数
*             ttl_bal   double        剩余总金额
*      
*     输  出:  
*             ln_pay_pln 按揭贷款还款计划结构 
*                    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
********************************************************************/
int pub_ln_PayPlan_Qry( struct ln_mst_c p_ln_mst, struct ln_parm_c p_ln_parm, int cnt, int ttl_cnt,
                             double ttl_bal, struct ln_pay_pln_c *p_ln_pay_pln )
{
	long fir_day=0,term_opn_date=0;
	long month_cnt=0;
	double tmp1=0.00,tmp2=0.00,ln_amt=0.00,lo_tol_amt=0.00,lo_pay_amt=0.00,tol_amt=0.00;
	int ret=0;
	int type=0;
	int iAc_seqn = 0; /***Add By zgf 2015/4/16 ***/
	struct ln_pay_pln_c	t_ln_pay_pln;

	memset(p_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset( &t_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	
	if( p_ln_mst.opn_date==p_ln_mst.ic_date )
	{
		TRACE
		type=0;
	}else{
		TRACE
		type=1;
	}
	
	/* 计算总期数 */
	month_cnt = pub_base_CalMouths( p_ln_mst.ic_date , p_ln_mst.mtr_date ); 

	/* if( ret ) return 1; */
	sprintf(acErrMsg,"总期数month_cnt[%ld]",month_cnt);
	WRITEMSG	

	/* 长沙系统－若是修改后的数据*/
	if( type==1 )
	{
		/* 若在还款日当日前 */
		if( p_ln_mst.opn_date%100 <= p_ln_parm.fst_pay_day )
		{
			TRACE
			month_cnt=month_cnt;
		}else{
			TRACE
			month_cnt=month_cnt+1;	
		}	
	}
		
	if( p_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%p_ln_parm.pay_amt_term==0 )
		{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term ;
		}else{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term +1;
		}	
	}

	sprintf(acErrMsg,"总期数p_ln_pay_pln->ttl_cnt[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG	
	/* 计算每期开始归还日期、到期日期 */
	if(cnt==1)
	{
		term_opn_date = p_ln_mst.ic_date;	
	}else{
		ret = pub_base_deadlineM(p_ln_mst.ic_date,(cnt-1)*p_ln_parm.pay_amt_term,&term_opn_date);
		if(ret) return 1;
	}
	sprintf(acErrMsg,"term_opn_date[%ld]",term_opn_date);
	WRITEMSG			
	if( p_ln_parm.pay_term_type[0]=='M' )
	{	
		if( p_ln_parm.fst_pay_ind[0]=='M' )
		{
			/* 若在还款日当日前 */
			if( term_opn_date%100 <= p_ln_parm.fst_pay_day )
			{
				p_ln_pay_pln->beg_date=term_opn_date/100*100+p_ln_parm.fst_pay_day;
				p_ln_pay_pln->end_date=term_opn_date/100*100+p_ln_parm.mon_pay_day;	
			}else{
				/* 若在还款日当日后 */
				if( term_opn_date%10000<=1200 )/*若非12月份*/
				{
			 		p_ln_pay_pln->beg_date=term_opn_date/100*100+100+p_ln_parm.fst_pay_day;
			   		p_ln_pay_pln->end_date=term_opn_date/100*100+100+p_ln_parm.mon_pay_day;
				}else{
					p_ln_pay_pln->beg_date=term_opn_date/10000*10000+10100+p_ln_parm.fst_pay_day;
					p_ln_pay_pln->end_date=term_opn_date/10000*10000+10100+p_ln_parm.mon_pay_day;
				}

			}
		}else if( p_ln_parm.fst_pay_ind[0]=='F' ){
			p_ln_pay_pln->beg_date = term_opn_date;
			p_ln_pay_pln->end_date = term_opn_date;			
		}
	}	
	sprintf(acErrMsg,"Jameslybeg_date[%ld]end_date[%ld]",p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date);
	WRITEMSG
	/* 若非修改 */
	if( type==0 )
	{
		TRACE
		if( p_ln_mst.opn_date%10000<=1221 )	/*若是12月21日以后开户*/
		{
			/* 按照长沙商行老数据要求，日期顺延一个月 */
			pub_base_deadlineM( p_ln_pay_pln->beg_date , 1 , &p_ln_pay_pln->beg_date );
			pub_base_deadlineM( p_ln_pay_pln->end_date , 1 , &p_ln_pay_pln->end_date );	
		}
	}
	
	/* 若最后一期修改为到期日期 */
	if(cnt==month_cnt)
	{
		TRACE
		p_ln_pay_pln->beg_date = p_ln_mst.mtr_date;
		p_ln_pay_pln->end_date = p_ln_mst.mtr_date;			
	}
	sprintf(acErrMsg,"Jameslybeg_date[%ld]end_date[%ld]",p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date);
	WRITEMSG	
			
	/* 计算当期应还 */
	p_ln_mst.rate = p_ln_mst.rate/1000;
	
	vtcp_log("[%s][%d] 贷款ID=[%ld],起始日期=[%ld],终止日期=[%ld],还款标志=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
	ret = pub_base_CompDblVal( p_ln_mst.bal, 0.00 );
	if ( ret==0 )
	{
		TRACE
		if(p_ln_mst.ic_date = g_pub_tx.tx_date )
		{
			vtcp_log("[%s][%d] 贷款ID=[%ld],起始日期=[%ld],终止日期=[%ld],还款标志=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
			TRACE
			ln_amt = p_ln_mst.amt_lmt;
			tol_amt= p_ln_mst.amt_lmt;
		}else{
			vtcp_log("[%s][%d] 贷款ID=[%ld],起始日期=[%ld],终止日期=[%ld],还款标志=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
			sprintf(acErrMsg,"查询还款计划表是异常出错![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L136");
			return 1;
		}			
	}else{						
		/* 取原还款计划表 */
		vtcp_log("[%s][%d] 贷款ID=[%ld],起始日期=[%ld],终止日期=[%ld],还款标志=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%ld and pay_ind='0' " , p_ln_mst.ac_id , p_ln_mst.ac_seqn);
		if( ret==100 )
		{
			Ln_pay_pln_Debug(&t_ln_pay_pln);
			vtcp_log("[%s][%d] 贷款ID=[%ld],起始日期=[%ld],终止日期=[%ld],还款标志=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
			sprintf(acErrMsg,"还款计划表无相关记录[%d-%d] %d",
				p_ln_mst.ac_id,p_ln_mst.ac_seqn,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L135");
			return 1;
		}else if( ret ){
			sprintf(acErrMsg,"查询还款计划表是异常出错![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L136");
			return 1;
		}			
		sprintf(acErrMsg,"rob!!![%lf]",t_ln_pay_pln.ttl_amt);
		WRITEMSG
		/* 取欠款表数据*
		ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", p_ln_mst.ac_id,p_ln_mst.ac_seqn );
		if(ret)	return 1;
		ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", p_ln_mst.ac_id,p_ln_mst.ac_seqn );
		if(ret)	return 1;
		*/		
		tol_amt= t_ln_pay_pln.ttl_amt;
		ln_amt = ttl_bal;	
	}
	sprintf(acErrMsg,"ln_amt=[%lf]!",ln_amt);
	WRITEMSG	
	
	if(  p_ln_parm.ln_pay_type[0]=='3' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , ttl_cnt , "0" , 
			p_ln_mst.rate , 1 , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}
	if(  p_ln_parm.ln_pay_type[0]=='4' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , ttl_cnt , "1" , 
			p_ln_mst.rate , 1 , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}		
	

	/* 若最后一期修改所欠本金为贷款余额 */
	/****
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->curr_amt = p_ln_mst.bal;		
	}
	****/
	if(cnt==ttl_cnt && ttl_cnt==1)
	{
		p_ln_pay_pln->curr_amt = p_ln_mst.bal;		
	}	
		
	/* 对还款计划中其他值赋值 */
	p_ln_pay_pln->ttl_amt=tol_amt; /* 原始本金 */
	p_ln_pay_pln->ac_id=p_ln_mst.ac_id; /* 账号ID */
	p_ln_pay_pln->ac_seqn=p_ln_mst.ac_seqn; /* 账号序号 */	
	p_ln_pay_pln->curr_cnt=cnt; /* 当前期次 */		
	p_ln_pay_pln->wrk_date=g_pub_tx.tx_date; /* 登记日期 */	
	strcpy ( p_ln_pay_pln->pay_ind , "0" );	/* 还款状态0-未还 */
	p_ln_pay_pln->de_ac_id = p_ln_mst.repay_ac_id ;	/* 还款账户ID */
	/* 根据币种生成活期户的账号ID */ 
	/**Mod by zgf 2015/4/16 32位-64位迁移,long指针赋值给int型指针错误 start*/
	/*ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &p_ln_pay_pln->de_ac_seqn );*/
	ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &iAc_seqn );
	if ( ret )
	{
		sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
		WRITEMSG
		return 1;	
	}			
	p_ln_pay_pln->de_ac_seqn = iAc_seqn;/**Mod by zgf 2015/4/16***/

	sprintf(acErrMsg,"orange!!!!!!!!!第[%d]期",p_ln_pay_pln->curr_cnt);
	WRITEMSG	
	sprintf(acErrMsg,"当前期次[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG				
	sprintf(acErrMsg,"原始本金[%lf]",p_ln_pay_pln->ttl_amt);
	WRITEMSG
	sprintf(acErrMsg,"账号ID[%ld]",p_ln_pay_pln->ac_id);
	WRITEMSG
	sprintf(acErrMsg,"账号序号[%d]",p_ln_pay_pln->ac_seqn);
	WRITEMSG
	sprintf(acErrMsg,"当前期次[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG
	sprintf(acErrMsg,"登记日期[%ld]",p_ln_pay_pln->wrk_date);
	WRITEMSG
	sprintf(acErrMsg,"还款状态0[%s]",p_ln_pay_pln->pay_ind);
	WRITEMSG
	
	
	sprintf(acErrMsg,"总期数[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG
	sprintf(acErrMsg,"当前期次应还本金[%lf]",p_ln_pay_pln->curr_amt);
	WRITEMSG
	sprintf(acErrMsg,"当前期次应还利息[%lf]",p_ln_pay_pln->curr_intst);
	WRITEMSG
	sprintf(acErrMsg,"开始归还日期[%ld]",p_ln_pay_pln->beg_date);
	WRITEMSG
	sprintf(acErrMsg,"到期归还日期[%ld]",p_ln_pay_pln->end_date);
	WRITEMSG
				
	return 0;	
}
