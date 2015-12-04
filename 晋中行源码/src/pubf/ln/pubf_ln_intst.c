/*************************************************
* 文 件 名: pubf_ln_intst.c
* 功能描述：   
*              pub_ln_cal_intst       贷款还息金额
*              pub_ln_cal_AmtIntst    计算贷款本金利息
*              pub_ln_cal_ComIntst    贷款利息利息计算（计算复利函数）
*              pub_ln_GetRate	      取得贷款利率
*	       	   pub_ln_GetIntstTyp     贷款本金利息类型判断
*              pub_ln_CalIntst        贷款算息
*              pub_ln_Udp_LnReg       归还欠息后登记簿处理
*	      	   pub_ln_CleanAcm	      贷款积数清零
*	           pub_ln_CalPunIntst     贷款算罚息
*	           pub_ln_Ins_ln_reg      登记贷款登记簿
*	           pub_ln_Ins_ln_lo	      登记贷款欠款登记簿
*              pub_ln_ln_reg	  	  登记/修改贷款登记簿
*              pub_ln_Ins_sect_rate	  登记分段积数表
*		Ln_eod_pri_InsReg		添加贷款日终打印登记簿
* 作    者:    rob
* 完成日期：   2004年12月28日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "com_sect_acm_c.h"
#include "com_parm_c.h"
#include "ln_reg_c.h"
#include "ln_pay_pln_c.h"
#include "ln_lo_c.h"
#include "ln_acm_reg_c.h"
#include "mo_pun_acc_c.h"
#include "ln_eod_pri_c.h"

/**********************************************************************
* 函 数 名：  pub_ln_Ins_sect_rate
* 函数功能：  登记分段积数表
* 作    者：  rob
* 完成时间：  2003年08月19日
*
* 参    数：
*     输  入：   p_ln_mst           	贷款主文件结构
*                p_ln_parm          	贷款参数结构    
*                rate		double	利率值
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_Ins_sect_rate ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , double rate )
{	
	char acm_type[2];
	
	/* 统计贷款主文件中积数 */
	strcpy( acm_type,"2");	/*积数计算类型（1每日累计2变动累计3变动累计倒算）*/
	g_reply_int = pub_base_CalAcm_Ln( p_ln_mst.lst_date,g_pub_tx.tx_date,
						p_ln_parm.intst_days_type,acm_type,p_ln_mst.bal,0.00,
						&p_ln_mst.intst_acm,"0",0,0 );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用函数计算积数错误!");
		WRITEMSG
		return 1;	
	}				
	g_reply_int = pub_base_InsertSectAcm(p_ln_mst.ac_id, p_ln_mst.ac_seqn, '2',
		  p_ln_mst.ic_date, g_pub_tx.tx_date, p_ln_mst.intst_acm,
		  rate, g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"调用函数登记分段积数表失败");
		WRITEMSG
		return 1;	
	}
	/* 若计算复利 */  
	if ( p_ln_mst.cmpd_intst_ind[0]=='Y' )
	{
		TRACE
		
		/* 表内欠息积数 */
		g_reply_int = pub_base_CalAcm_Ln( p_ln_mst.lst_date,g_pub_tx.tx_date,
				p_ln_parm.intst_days_type,acm_type,p_ln_mst.in_lo_intst,0.00,
					&p_ln_mst.in_lo_acm,"0",0,0 );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return 1;;	
		}
		TRACE	
		g_reply_int = pub_base_InsertSectAcm(p_ln_mst.ac_id, p_ln_mst.ac_seqn, '3',
		 	 p_ln_mst.ic_date, g_pub_tx.tx_date, p_ln_mst.in_lo_acm,
			  rate, g_pub_tx.reply);
		if (g_reply_int)
		{
			sprintf( acErrMsg,"调用函数登记分段积数表失败");
			WRITEMSG
			return 1;	
		}
		/* 表外欠息积数 */
		g_reply_int = pub_base_CalAcm_Ln( p_ln_mst.lst_date,g_pub_tx.tx_date,
				p_ln_parm.intst_days_type,acm_type,p_ln_mst.out_lo_intst,0.00,
					&p_ln_mst.out_lo_acm,"0",0,0 );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return 1;	
		}				
		g_reply_int = pub_base_InsertSectAcm(p_ln_mst.ac_id, p_ln_mst.ac_seqn, '4',
		 	 p_ln_mst.ic_date, g_pub_tx.tx_date, p_ln_mst.out_lo_acm,
			  rate, g_pub_tx.reply);
		if (g_reply_int)
		{
			sprintf( acErrMsg,"调用函数登记分段积数表失败");
			WRITEMSG
			return 1;	
		}	
		/* 复利积数 */
		g_reply_int = pub_base_CalAcm_Ln( p_ln_mst.lst_date,g_pub_tx.tx_date,
				p_ln_parm.intst_days_type,acm_type,p_ln_mst.cmpd_lo_intst,0.00,
					&p_ln_mst.cmpd_lo_acm,"0",0,0 );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return 1;	
		}		
		g_reply_int = pub_base_InsertSectAcm(p_ln_mst.ac_id, p_ln_mst.ac_seqn, '5',
		 	 p_ln_mst.ic_date, g_pub_tx.tx_date, p_ln_mst.cmpd_lo_acm,
			  rate, g_pub_tx.reply);
		if (g_reply_int)
		{
			sprintf( acErrMsg,"调用函数登记分段积数表失败");
			WRITEMSG
			return 1;	
		}		
	}
	
	return 0;	
	
}
/**********************************************************************
* 函 数 名：  pub_ln_GetRate
* 函数功能：  取得贷款利率
* 作    者：  rob
* 完成时间：  2003年01月18日
*
* 参    数：
*     输  入：   p_ln_mst           	贷款主文件结构
*                p_ln_parm          	贷款参数结构    
*      
*     输  出:    
*                rate		double	利率值
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_GetRate ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , double *rate )
{
  	int ret=0;
  	struct 	mo_pun_acc_c	mo_pun_acc;
  	memset( &mo_pun_acc, 0x00, sizeof(struct mo_pun_acc_c));
	
	/* 贷款状态检查 */
	/* 贷款状态为开户撤销或撤销时 */
	if ( (p_ln_mst.ac_sts[0]=='9') || (p_ln_mst.ac_sts[0]=='*') ) 
	{
		sprintf(acErrMsg,"贷款状态错误,贷款状态为开户撤销或撤销状态!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L032");
		return 1;	
	}
	
	/* 若贷款违约，取罚息利率 */
	ret = Mo_pun_acc_Sel( g_pub_tx.reply, &mo_pun_acc, \
		"ac_id=%ld and ac_seqn=%d and sts=0 ", p_ln_mst.ac_id, p_ln_mst.ac_seqn );
	if( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "查询罚息利率登记簿出错![%d]", ret );
		WRITEMSG
		return 1;			
	}else if(!ret){
		/*罚息登记簿中有记录*/
		*rate= p_ln_mst.fine_rate;
					sprintf(acErrMsg,"1111111111111111111111111111111![%lf]\n",p_ln_mst.rate);
			WRITEMSG
		return 0;
	}	
			
	if ( atoi(p_ln_mst.ac_sts)==1 )	/* 贷款状态正常时 */
	{
		/* 如果贷款账户展期控制标志不为0,即已经展期,且当前日期大于到期日期 */
		if ( atoi(p_ln_mst.exp_ind)!=0 && g_pub_tx.tx_date > p_ln_mst.mtr_date )
		{
			*rate = p_ln_mst.exp_rate ;
					sprintf(acErrMsg,"222222222222222222222222222222![%lf]\n",p_ln_mst.exp_rate);
			WRITEMSG
			return 0;
		}
		
		if ( p_ln_parm.get_rate_type[0]=='1' ) /* 利率取得方式为手工 */
		{
			*rate = p_ln_mst.rate ;
			sprintf(acErrMsg,"调用函数根据利率编号币种日期,取利率值错误![%lf]\n",p_ln_mst.rate);
			WRITEMSG
			return 0;
		}else{
			ret = pub_base_getllz(p_ln_parm.rate_no, p_ln_parm.cur_no, 
									g_pub_tx.tx_date, rate);
			if ( ret )
			{
				sprintf(acErrMsg,"调用函数根据利率编号币种日期,取利率值错误!");
				WRITEMSG
				return 1;	
			}
			*rate = *rate/1.2;/* 转换成月利率 */
			sprintf(acErrMsg,"根据利率编号取利率值 PASS!");
			WRITEMSG
		     }
	}else{
		/* 贷款状态为{2-逾期}{3-呆滞}{4-呆账} 5-90天外 即取逾期利率 */ 
		if ( p_ln_parm.get_rate_type[0]=='1' ) /* 利率取得方式为手工 */
		{
			*rate = p_ln_mst.over_rate ;
	   sprintf(acErrMsg,"333333333333333333333333333333333![%lf]\n",p_ln_mst.rate);
			WRITEMSG

			return 0;
		}else{
			ret = pub_base_getllz(p_ln_parm.over_rate_no, p_ln_parm.cur_no, 
									g_pub_tx.tx_date, rate);
			if ( ret )
			{
				sprintf(acErrMsg,"调函数根据逾期利率编号币种日期,取利率错误!");
				WRITEMSG
				return 1;	
			}
			*rate = *rate/1.2;/* 转换成月利率 */			
			sprintf(acErrMsg,"根据逾期利率编号取利率值 PASS!");
			WRITEMSG
		     }
	}

  return 0 ;
}


/**********************************************************************
* 函 数 名：  pub_ln_GetIntstTyp
* 函数功能：  贷款本金利息类型判断
* 作    者：  rob
* 完成时间：  2003年01月18日
*
* 参    数：
*     输  入：   p_ln_mst           	贷款主文件结构
*                p_ln_parm          	贷款参数结构    
*      
*     输  出:    
*
*     返回值： 	 利息类型  -1失败  1表内  2表外
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_GetIntstTyp ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm )
{
  	int ret=0;
  	long beg_day=0;	
  	long days=0;
  	char parm_code[6];
  	int  parm_seqn=0;
  	long get_days=0;
  	long over_end_date=0;
  	long over_days=0;
	int type_lx=0;
  	
	struct com_parm_c	com_parm;
	
	memset( &com_parm,0x00,sizeof(struct com_parm_c) );	
	
	/* 贷款状态检查 */
	/* 贷款状态为开户撤销或撤销时 */
	if ( (p_ln_mst.ac_sts[0]=='9') || (p_ln_mst.ac_sts[0]=='*') )
	{
		sprintf(acErrMsg,"贷款状态错误,贷款状态为开户撤销或撤销状态!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L032");
		return -1;	
	}
			
	/* 贷款状态:呆滞 呆账 逾期90天外*/
	if ( (p_ln_mst.ac_sts[0]=='3') || (p_ln_mst.ac_sts[0]=='4') || (p_ln_mst.ac_sts[0]=='5') )
	{	
		return 2; /* 表外 */
		sprintf(acErrMsg,"贷款状态:呆滞 呆账 !");
		WRITEMSG
	}

	
	/* 逾期90天入表外 */
	/* 取到期日期 */
	if ( p_ln_mst.exp_ind[0]=='0' )
	{
		over_end_date = p_ln_mst.mtr_date ;
	}else{
		over_end_date = p_ln_mst.exp_mtr_date;
	}
	/* 若已到期 */
	if( g_pub_tx.tx_date > over_end_date )
	{
	TRACE
		over_days = pub_base_CalTrueDays( over_end_date , g_pub_tx.tx_date );

		if( over_days >=90 )
		{
			sprintf(acErrMsg,"表外!!!!!!!!!!!");
			WRITEMSG
			return 2;	/* 表外 */  
		}else{
			sprintf(acErrMsg,"表内!!!!!!!!!!!");
			WRITEMSG
			type_lx=1;	/* 表内 */
		}
			
	}else{
		sprintf(acErrMsg,"表内!!!!!!!!!!!");
		WRITEMSG
		type_lx=1;	/* 表内 */
	}
	
	/* 在com_parm中取得欠息入表外天数 */
	strcpy( parm_code , "L0001" );
	parm_seqn = 1 ;
	
	ret = pub_base_GetParm_long( parm_code , parm_seqn , &get_days );
	if( ret )
	{
			sprintf(acErrMsg,"调用函数取欠息入表外天数错误!");
			WRITEMSG
			return -1;	
	}	
	sprintf(acErrMsg,"调用函数取欠息入表外天数 PASS![%ld]",get_days);
	WRITEMSG
		
	/* 欠息天数检查 */
	ret=sql_min_long("ln_reg","reg_date_end",&beg_day," ac_id=%ld and ac_seqn=%d and type in('2','3') and sts='0' " , p_ln_mst.ac_id , p_ln_mst.ac_seqn );
	if( ret )
	{
		sprintf(acErrMsg,"调用函数查询最早日期错误!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"调用函数查询最早日期 PASS[%ld]!",beg_day);
	WRITEMSG
	
	if ( beg_day )	/* 存在欠息天数 */
	{
		/* 调用函数计算天数 */
		days = pub_base_CalTrueDays( beg_day , g_pub_tx.tx_date );
		sprintf(acErrMsg,"调用函数计算天数 PASS[%ld]!",days);
		WRITEMSG
	
		/* 两个天数比较 */
		if ( days > get_days ) /* 最早欠息天数与参数表中的天数进行比较 */
		{
			sprintf(acErrMsg,"表外!!!!!!!!!!!");
			WRITEMSG
			return 2;	/* 表外 */  
		}else{
			sprintf(acErrMsg,"表内!!!!!!!!!!!");
			WRITEMSG
			type_lx=1;	/* 表内 */
		}
		sprintf(acErrMsg,"存在欠息天数!!! PASS[%ld]!",beg_day);
		WRITEMSG
	}
	return type_lx;
}

/**********************************************************************
* 函 数 名：  pub_ln_CalIntst
* 函数功能：  贷款算息
* 作    者：  rob
* 完成时间：  2003年01月18日
*
* 参    数：
*     输  入：   p_ln_mst           	贷款主文件结构
*                p_ln_parm          	贷款参数结构    
*		 intst_typ	char[2]		利息类型( 0本金积数计算利息
*												  1表内欠息积数计算复利
*						  						  2表外欠息积数计算复利
*					  	  					  	  3复利欠息积数计算复利 )
*                mode                   处理模式：0修改数据库1不修改数据库
*     
*     输  出:    intst		double	本金利息总数
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_CalIntst ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , 
			char *intst_typ ,double *intst, int mode )
{
  	int ret;
  	char type[2];	
  	char acm_type[2];	/* 积数计算类型 */	
  	double sect_intst;	/* 分段利息 */
  	double mst_intst;	/* 主表中利息 */
  	double tmp_amt;
  	double acm;
  	double rate;
  	int	jx_date=0;
	int	month_tmp=0;
	int	day_tmp=0; 
	int	jx_ind=0; 	
	
	struct ln_acm_reg_c	ln_acm_reg;
	memset( &ln_acm_reg, 0x00, sizeof(struct ln_acm_reg_c) );
		
	/* 贷款文件中计息类型为0不计息 */
	if ( p_ln_mst.intst_type[0]=='0' )
	{
		*intst = 0;
		sprintf(acErrMsg,"贷款文件中计息类型为不计息 [%lf]",*intst );
		WRITEMSG
		return 0;	
	}
	TRACE
	
	/*****-------------------- add by rob 20030922 ------------------*********/
	/* 取结息日参数 */
	ret= pub_base_GetParm_int( "L0003" , 1 , &jx_date );
	if(ret)	return 1;
	
	month_tmp=g_pub_tx.tx_date%10000/100;
	day_tmp=g_pub_tx.tx_date%100;
		
	sprintf(acErrMsg,"日期 [%ld]",day_tmp );
	WRITEMSG	
	sprintf(acErrMsg,"日期 [%ld][%d]",jx_date,jx_ind );
	WRITEMSG
	if( day_tmp!=jx_date )
	{
		jx_ind++;	/* 计复利标志 */
	}
	vtcp_log("[%s][%d]heheheehehehe",__FILE__,__LINE__);
	if( day_tmp==jx_date )
	{
		if( p_ln_mst.intst_type[0]=='3' ) /*月结*/
		{
			jx_ind++;
		}else if( p_ln_mst.intst_type[0]=='4' ){/*季结*/
			if( month_tmp==3|| month_tmp==6|| month_tmp==9|| month_tmp==12 )
			{
				jx_ind++;
			}
		}else if( p_ln_mst.intst_type[0]=='5' ){/*年结*/
			if( month_tmp==12 )
			{
				jx_ind++;
			}
		}	
	}
	/* 暂时屏蔽 */
	jx_ind=0;
	vtcp_log("[%s][%d]heheheehehehe",__FILE__,__LINE__);
	
	/* 利息类型为 0本金积数计算利息 */	 	
	if ( intst_typ[0]=='0' )
	{
		/* 计算分段积数利息 */
		strcpy( type , "1" ); /* 1-贷款本金 */
		ret = pub_base_Get_SectIntst( p_ln_mst.ac_id , 
			p_ln_mst.ac_seqn , type , &sect_intst , mode );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算分段积数利息错误!");
			WRITEMSG
			return -1;	
		}	
	TRACE
		sect_intst = pub_base_PubDround_1( sect_intst );
		sprintf(acErrMsg,"调用函数计算分段积数利息PASS[%lf]!",sect_intst);
		WRITEMSG		
		
		/* 计算贷款主表中积数的利息 */
		/* 计算贷款主表中积数 */
		strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
		acm = p_ln_mst.intst_acm ;/* 利息积数 */
		
		ret = pub_base_CalAcm_Ln( p_ln_mst.lst_date , g_pub_tx.tx_date , 
			p_ln_parm.intst_days_type , acm_type , 
			p_ln_mst.bal ,g_pub_tx.tx_amt1 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return -1;	
		}
		sprintf(acErrMsg,"调用函数计算积数PASS为 [%lf]",acm );
		WRITEMSG

		/* 计算贷款主表中积数利息 */
		/* 取利率值(月利率) */
		ret = pub_ln_GetRate ( p_ln_mst , p_ln_parm , &rate );	
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算贷款利率错误!");
			WRITEMSG
			return -1;	
		}		
		sprintf(acErrMsg,"调用函数计算贷款利率PASS为 [%lf]",rate );
		WRITEMSG

		mst_intst = acm*rate/30/1000;
		sprintf(acErrMsg,"前计算贷款主表中积数的利息PASS为 [%lf]",mst_intst );
		WRITEMSG
		tmp_amt = pub_base_PubDround_1(mst_intst);
		sprintf(acErrMsg,"tmp_amt计算贷款主表中积数的利息PASS为 [%lf]",tmp_amt );
		WRITEMSG
		mst_intst = pub_base_PubDround_1(mst_intst);		
		sprintf(acErrMsg,"计算贷款主表中积数的利息PASS为 [%lf]",mst_intst );
		WRITEMSG

		*intst = sect_intst + mst_intst ; 
		*intst = pub_base_PubDround_1( *intst );		
		sprintf(acErrMsg,"利息总数为 [%lf]",*intst );
		WRITEMSG
		
		/* 利息金额不为0时，判断是否达到最低限额 */
		ret = pub_base_CompDblVal( *intst, 0.00 );
		if ( ret>0 )
		{					
			ret = pub_base_CompDblVal( *intst, p_ln_parm.min_intst );
			if ( ret<0 )
			{			
				*intst = p_ln_parm.min_intst;
				*intst = pub_base_PubDround_1( *intst );
			}
		}
		sprintf(acErrMsg,"本金利息总数为 [%lf]",*intst );
		WRITEMSG
		
		/* 利息金额为负数时，不处理*/
		ret = pub_base_CompDblVal( *intst, 0.00 );	
		if ( ret<0 )
		{	
			mode=1;
			*intst=0.00;
		}
		/* 添加贷款结息积数登记薄 */
		if (!mode)
		{	
			if( pub_base_CompDblVal( acm , 0.00 ) )	
			{
				
				ln_acm_reg.ac_id = p_ln_mst.ac_id;
				ln_acm_reg.ac_seqn = p_ln_mst.ac_seqn;
				strcpy( ln_acm_reg.br_no, p_ln_mst.opn_br_no );
				ln_acm_reg.beg_date = p_ln_mst.ic_date;
				ln_acm_reg.end_date = g_pub_tx.tx_date;
				ln_acm_reg.cal_date = g_pub_tx.tx_date;
				ln_acm_reg.rate = rate;
				ln_acm_reg.intst_acm = acm;
				ln_acm_reg.part_intst_acm = p_ln_mst.intst_acm;
				ln_acm_reg.intst = mst_intst;	
				sprintf( ln_acm_reg.intst_type , "%d" , atoi(intst_typ)+1 );														
				strcpy( ln_acm_reg.type , "1" );
				ln_acm_reg.trace_no = g_pub_tx.trace_no;
				TRACE
				sprintf( acErrMsg,"!!rob!ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
				WRITEMSG	
				TRACE									
				ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
				if( ret )	return 1;
				TRACE	
			}
		}		
		/* 贷款主表中积数清零,更改起息日期 */
		if (!mode)
		{
			ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
			if( ret )
			{
				sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd 游标出错[%d]",ret);
				WRITEMSG
				return 1;
			}	
			ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
			if( ret==100 )
			{
				sprintf( acErrMsg,"无记录[%d]",ret);
				WRITEMSG
				return 1;
			}
   			if( ret )
 			{
			 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
			 	WRITEMSG
			 	strcpy( g_pub_tx.reply ,"D103" );
			 	return 1;
 			}
			
			p_ln_mst.intst_acm = 0;	
	
			ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Ln_mst_Upd_Upd 出错[%d]",ret);
				WRITEMSG
				return 1;
			}		
		
			Ln_mst_Clo_Upd( );
			sprintf(acErrMsg,"贷款主文件中积数清零 PASS");
			WRITEMSG
		}
					

		return 0;
	}


	/* 利息类型为 1表内欠息积数计算复利 */	 	
	if ( intst_typ[0]=='1' )
	{	
vtcp_log("cal bnlx" );
		sprintf(acErrMsg,"是否计算复利cmpd_intst_ind=[%s]",p_ln_mst.cmpd_intst_ind);
		WRITEMSG
		/* 检查是否计算复利 */
		if ( p_ln_mst.cmpd_intst_ind[0]=='N' )
		{
			TRACE
			*intst = 0;
			sprintf(acErrMsg,"此种贷款不计算复利 [%lf]",*intst );
			WRITEMSG
			return 0;	
		}
		TRACE		
		/* 检查复利计算方法 */
		if ( p_ln_parm.cmpd_intst_type[0]!='0' )
		{
			*intst = 0;
			sprintf(acErrMsg,"此种贷款非按照积数计算 [%lf]",*intst );
			WRITEMSG
			return 0;	
		}
		/* 复利计算方法是按照积数计算 */
		else if ( p_ln_parm.cmpd_intst_type[0]=='0' )
		{				
			/* 计算分段积数表中表内欠息利息 */
			strcpy( type , "2" ); /* 2-贷款表内欠息 */
			ret = pub_base_Get_SectIntst( p_ln_mst.ac_id , 
				p_ln_mst.ac_seqn , type , &sect_intst , mode );
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算分段积数利息错误!");
				WRITEMSG
				return -1;	
			}	
			sprintf(acErrMsg,"调用函数计算分段积数利息PASS[%lf]!",sect_intst);
			WRITEMSG		
				
			/* 计算贷款主表中表内欠息积数的利息 */
			/* 计算贷款主表中表内欠息积数 */
			strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
			acm = p_ln_mst.in_lo_acm ;/* 表内欠息积数 */
			
			ret = pub_base_CalAcm_Ln( p_ln_mst.lst_date , g_pub_tx.tx_date , 
				p_ln_parm.intst_days_type , acm_type , 
				p_ln_mst.in_lo_intst ,g_pub_tx.tx_amt2, &acm,"0",0,0 );
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算积数错误!");
				WRITEMSG
				return -1;	
			}
			sprintf(acErrMsg,"调用函数计算积数PASS为 [%lf]",acm );
			WRITEMSG
		
			/* 计算贷款主表中表内欠息积数 */
			ret = pub_ln_GetRate ( p_ln_mst , p_ln_parm , &rate );	
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算贷款利率错误!");
				WRITEMSG
				return -1;	
			}		
			sprintf(acErrMsg,"调用函数计算贷款利率PASS为 [%lf]",rate );
			WRITEMSG
		
			mst_intst = acm*rate/30/1000;
			sprintf(acErrMsg,"计算表内欠息积数的利息PASS为 [%lf]",mst_intst );
			WRITEMSG
			mst_intst = pub_base_PubDround_1(mst_intst);		
			sprintf(acErrMsg,"计算表内欠息积数的利息PASS为 [%lf]",mst_intst );
			WRITEMSG
			
			*intst = sect_intst + mst_intst ; 
			*intst = pub_base_PubDround_1( *intst );
			sprintf(acErrMsg,"表内利息总数为 [%lf]",*intst );
			WRITEMSG	
				
			/* 利息金额为负数时，不处理*/
			ret = pub_base_CompDblVal( *intst, 0.00 );	
			if ( ret<0 )
			{	
				mode=1;
				*intst=0.00;
			}
			
		/* 添加贷款结息积数登记薄 */
		if (!mode)
		{		
	 		if( pub_base_CompDblVal( acm , 0.00 ) )
			{
				ln_acm_reg.ac_id = p_ln_mst.ac_id;
				ln_acm_reg.ac_seqn = p_ln_mst.ac_seqn;
				strcpy( ln_acm_reg.br_no , p_ln_mst.opn_br_no );
				ln_acm_reg.beg_date = p_ln_mst.ic_date;
				ln_acm_reg.end_date = g_pub_tx.tx_date;
				ln_acm_reg.cal_date = g_pub_tx.tx_date;
				ln_acm_reg.rate = rate;
				ln_acm_reg.intst_acm = acm;
				ln_acm_reg.part_intst_acm = p_ln_mst.in_lo_acm;
				if( jx_ind )
				{
					mst_intst=0.00;
				}
				ln_acm_reg.intst = mst_intst;	
				sprintf( ln_acm_reg.intst_type , "%d" , atoi(intst_typ)+1 );															
				strcpy( ln_acm_reg.type , "1" );
				ln_acm_reg.trace_no = g_pub_tx.trace_no;
				TRACE
				sprintf( acErrMsg,"!!rob!ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
				WRITEMSG	
				TRACE									
				ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
				if( ret )	return 1;
				TRACE	
			}
		}
		if( jx_ind )
		{
			*intst = 0;
			sprintf(acErrMsg,"未到计息日不计算复利 [%lf]",*intst );
			WRITEMSG
			sprintf(acErrMsg,"原表内积数为 [%lf]",p_ln_mst.in_lo_acm );
			WRITEMSG			
			return 0;	
		}
		TRACE
		/**** del by ly   在记账主控里清理 *********/	
		/* 贷款主表中积数清零 *
		if (!mode)
		{
			ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
			if( ret )
			{
				sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd 游标出错[%d]",ret);
				WRITEMSG
				return 1;
			}	
			ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
			if( ret==100 )
			{
				sprintf( acErrMsg,"无记录[%d]",ret);
				WRITEMSG
				return 1;
			}
   			if( ret )
 			{
			 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
			 	WRITEMSG
			 	strcpy( g_pub_tx.reply ,"D103" );
			 	return 1;
 			}
			
			p_ln_mst.in_lo_acm = 0;			
				
			ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Ln_mst_Upd_Upd 出错[%d]",ret);
				WRITEMSG
				return 1;
			}		
		
			Ln_mst_Clo_Upd( );
			sprintf(acErrMsg,"贷款主文件中积数清零 PASS");
			WRITEMSG
		}
		**********/
				

		}
		
		return 0;
	}	


	/* 利息类型为 2表外欠息积数计算复利 */	 	
	if ( intst_typ[0]=='2' )
	{	
		sprintf(acErrMsg,"是否计算复利cmpd_intst_ind=[%s]",p_ln_mst.cmpd_intst_ind);
		WRITEMSG
		/* 检查是否计算复利 */
		if ( p_ln_mst.cmpd_intst_ind[0]=='N' )
		{
			*intst = 0;
			sprintf(acErrMsg,"此种贷款不计算复利 [%lf]",*intst );
			WRITEMSG
			return 0;	
		}				
		/* 检查复利计算方法 */
		if ( p_ln_parm.cmpd_intst_type[0]!='0' )
		{
			*intst = 0;
			sprintf(acErrMsg,"此种贷款非按照积数计算 [%lf]",*intst );
			WRITEMSG
			return 0;	
		}
		/* 复利计算方法是按照积数计算 */
		else if ( p_ln_parm.cmpd_intst_type[0]=='0' )
		{
			/* 计算分段积数表中表外欠息利息 */
			strcpy( type , "3" ); /* 3-贷款表外欠息 */			
			ret = pub_base_Get_SectIntst( p_ln_mst.ac_id , 
				p_ln_mst.ac_seqn , type , &sect_intst , mode );
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算分段积数利息错误!");
				WRITEMSG
				return -1;	
			}	
			sprintf(acErrMsg,"调用函数计算分段积数利息PASS[%lf]!",sect_intst);
			WRITEMSG		
				
			/* 计算贷款主表中表外欠息积数的利息 */
			/* 计算贷款主表中表外欠息积数 */
			strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
			acm = p_ln_mst.out_lo_acm ;/* 表外欠息积数 */
			
			ret = pub_base_CalAcm_Ln( p_ln_mst.lst_date , g_pub_tx.tx_date , 
				p_ln_parm.intst_days_type , acm_type , 
				p_ln_mst.out_lo_intst ,g_pub_tx.tx_amt3, &acm,"0",0,0 );
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算积数错误!");
				WRITEMSG
				return -1;	
			}
			sprintf(acErrMsg,"调用函数计算积数PASS为 [%lf]",acm );
			WRITEMSG
		
			/* 计算贷款主表中表外欠息积数 */
			ret = pub_ln_GetRate ( p_ln_mst , p_ln_parm , &rate );	
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算贷款利率错误!");
				WRITEMSG
				return -1;	
			}		
			sprintf(acErrMsg,"调用函数计算贷款利率PASS为 [%lf]",rate );
			WRITEMSG
		
			mst_intst = acm*rate/30/1000;
			sprintf(acErrMsg,"计算贷款主表中积数的利息PASS为 [%lf]",mst_intst );
			WRITEMSG

			mst_intst = pub_base_PubDround_1(mst_intst);		
			sprintf(acErrMsg,"计算表外欠息积数的利息PASS为 [%lf]",mst_intst );
			WRITEMSG

			*intst = sect_intst + mst_intst ;
			*intst = pub_base_PubDround_1( *intst );			 
			sprintf(acErrMsg,"表外利息总数为 [%lf]",*intst );
			WRITEMSG
			
			/* 利息金额为负数时，不处理*/
			ret = pub_base_CompDblVal( *intst, 0.00 );	
			if ( ret<0 )
			{	
				mode=1;
				*intst=0.00;
			}
			
		/* 添加贷款结息积数登记薄 */
		if (!mode)
		{		
			if( pub_base_CompDblVal( acm , 0.00 ) )
			{
				ln_acm_reg.ac_id = p_ln_mst.ac_id;
				ln_acm_reg.ac_seqn = p_ln_mst.ac_seqn;
				strcpy( ln_acm_reg.br_no , p_ln_mst.opn_br_no );
				ln_acm_reg.beg_date = p_ln_mst.ic_date;
				ln_acm_reg.end_date = g_pub_tx.tx_date;
				ln_acm_reg.cal_date = g_pub_tx.tx_date;
				ln_acm_reg.rate = rate;
				ln_acm_reg.intst_acm = acm;
				ln_acm_reg.part_intst_acm = p_ln_mst.out_lo_acm;
				if( jx_ind )
				{
					mst_intst=0.00;
				}
				ln_acm_reg.intst = mst_intst;	
				sprintf( ln_acm_reg.intst_type , "%d" , atoi(intst_typ)+1 );														
				strcpy( ln_acm_reg.type , "1" );
				ln_acm_reg.trace_no = g_pub_tx.trace_no;
				TRACE
				sprintf( acErrMsg,"!!rob!ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
				WRITEMSG	
				TRACE									
				ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
				if( ret )	return 1;
				TRACE	
			}
		}
		if( jx_ind )
		{
			*intst = 0;
			sprintf(acErrMsg,"未到计息日不计算复利 [%lf]",*intst );
			WRITEMSG
			sprintf(acErrMsg,"原表外积数为 [%lf]",p_ln_mst.out_lo_acm );
			WRITEMSG
			return 0;	
		}
		/**** del by ly   在记账主控里清理 *********/			
		/* 贷款主表中积数清零 *
		if (!mode)
		{
			ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
			if( ret )
			{
				sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd 游标出错[%d]",ret);
				WRITEMSG
				return 1;
			}	
			ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
			if( ret==100 )
			{
				sprintf( acErrMsg,"无记录[%d]",ret);
				WRITEMSG
				return 1;
			}
   			if( ret )
 			{
			 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
			 	WRITEMSG
			 	strcpy( g_pub_tx.reply ,"D103" );
			 	return 1;
 			}
			
			p_ln_mst.out_lo_acm = 0;		
	
			ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Ln_mst_Upd_Upd 出错[%d]",ret);
				WRITEMSG
				return 1;
			}		
		
			Ln_mst_Clo_Upd( );
			sprintf(acErrMsg,"贷款主文件中积数清零 PASS");
			WRITEMSG
		}
		**************/


		}
		
		return 0;
		
	}	

	/* 利息类型为 3复利欠息积数计算复利 */	 	
	if ( intst_typ[0]=='3' )
	{	
		sprintf(acErrMsg,"是否计算复利cmpd_intst_ind=[%s]",p_ln_mst.cmpd_intst_ind);
		WRITEMSG
		/* 检查是否计算复利 */
		if ( p_ln_mst.cmpd_intst_ind[0]=='N' )
		{
			*intst = 0;
			sprintf(acErrMsg,"此种贷款不计算复利 [%lf]",*intst );
			WRITEMSG
			return 0;	
		}		
		/* 检查复利计算方法 */
		if ( p_ln_parm.cmpd_intst_type[0]!='0' )
		{
			*intst = 0;
			sprintf(acErrMsg,"此种贷款非按照积数计算 [%lf]",*intst );
			WRITEMSG
			return 0;	
		}
		/* 复利计算方法是按照积数计算 */
		else if ( p_ln_parm.cmpd_intst_type[0]=='0' )
		{
			/* 计算分段积数表中复利欠息利息 */
			strcpy( type , "4" ); /* 4-贷款复利 */			
			ret = pub_base_Get_SectIntst( p_ln_mst.ac_id , 
				p_ln_mst.ac_seqn , type , &sect_intst , mode );
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算分段积数利息错误!");
				WRITEMSG
				return -1;	
			}	
			sprintf(acErrMsg,"调用函数计算分段积数利息PASS[%lf]!",sect_intst);
			WRITEMSG		
				
			/* 计算贷款主表中复利欠息积数的利息 */
			/* 计算贷款主表中复利欠息积数 */
			strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
			acm = p_ln_mst.cmpd_lo_acm ;/* 复利欠息积数 */
			
			ret = pub_base_CalAcm_Ln( p_ln_mst.lst_date , g_pub_tx.tx_date , 
				p_ln_parm.intst_days_type , acm_type , 
				p_ln_mst.cmpd_lo_intst ,g_pub_tx.tx_amt4, &acm,"0",0,0 );
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算积数错误!");
				WRITEMSG
				return -1;	
			}
			sprintf(acErrMsg,"调用函数计算积数PASS为 [%lf]",acm );
			WRITEMSG
		
			/* 计算贷款主表中复利欠息积数 */
			ret = pub_ln_GetRate ( p_ln_mst , p_ln_parm , &rate );	
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算贷款利率错误!");
				WRITEMSG
				return -1;	
			}		
			sprintf(acErrMsg,"调用函数计算贷款利率PASS为 [%lf]",rate );
			WRITEMSG
		
			mst_intst = acm*rate/30/1000;
			sprintf(acErrMsg,"计算贷款主表中积数的利息PASS为 [%lf]",mst_intst );
			WRITEMSG
			mst_intst = pub_base_PubDround_1(mst_intst);		
			sprintf(acErrMsg,"计算复利欠息积数的利息PASS为 [%lf]",mst_intst );
			WRITEMSG
			
			*intst = sect_intst + mst_intst ;
			*intst = pub_base_PubDround_1( *intst );			 
			sprintf(acErrMsg,"复利利息总数为 [%lf]",*intst );
			WRITEMSG			
			
			/* 利息金额为负数时，不处理*/
			ret = pub_base_CompDblVal( *intst, 0.00 );	
			if ( ret<0 )
			{	
				mode=1;
				*intst=0.00;
			}

		/* 添加贷款结息积数登记薄 */
		if (!mode)
		{		
			if( pub_base_CompDblVal( acm , 0.00 ) )
			{
				ln_acm_reg.ac_id = p_ln_mst.ac_id;
				ln_acm_reg.ac_seqn = p_ln_mst.ac_seqn;
				strcpy( ln_acm_reg.br_no , p_ln_mst.opn_br_no );
				ln_acm_reg.beg_date = p_ln_mst.ic_date;
				ln_acm_reg.end_date = g_pub_tx.tx_date;
				ln_acm_reg.cal_date = g_pub_tx.tx_date;
				ln_acm_reg.rate = rate;
				ln_acm_reg.intst_acm = acm;
				ln_acm_reg.part_intst_acm = p_ln_mst.cmpd_lo_acm;
				if( jx_ind )
				{
					mst_intst=0.00;
				}
				ln_acm_reg.intst = mst_intst;	
				sprintf( ln_acm_reg.intst_type , "%d" , atoi(intst_typ)+1 );															
				strcpy( ln_acm_reg.type , "1" );
				ln_acm_reg.trace_no = g_pub_tx.trace_no;
				TRACE
				sprintf( acErrMsg,"!!rob!ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
				WRITEMSG	
				TRACE									
				ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
				if( ret )	return 1;
				TRACE	
			}
		}
		if( jx_ind )
		{
			*intst = 0;
			sprintf(acErrMsg,"未到计息日不计算复利 [%lf]",*intst );
			WRITEMSG
			sprintf(acErrMsg,"原复利积数为 [%lf]",p_ln_mst.cmpd_lo_acm );
			WRITEMSG			
			return 0;	
		}
		/**** del by ly   在记账主控里清理 *********/		
		/* 贷款主表中积数清零 *
		if (!mode)
		{
			ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
			if( ret )
			{
				sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd 游标出错[%d]",ret);
				WRITEMSG
				return 1;
			}	
			ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
			if( ret==100 )
			{
				sprintf( acErrMsg,"无记录[%d]",ret);
				WRITEMSG
				return 1;
			}
   			if( ret )
 			{
			 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
			 	WRITEMSG
			 	strcpy( g_pub_tx.reply ,"D103" );
			 	return 1;
 			}
			
			p_ln_mst.cmpd_lo_acm = 0;		
	
			ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Ln_mst_Upd_Upd 出错[%d]",ret);
				WRITEMSG
				return 1;
			}		
		
			Ln_mst_Clo_Upd( );
			sprintf(acErrMsg,"贷款主文件中积数清零 PASS");
			WRITEMSG
		}
		***************/

		}
		
		return 0;
	}
	/***写下传文件***/	
  return -1 ;
}
/**********************************************************************
* 函 数 名：  pub_ln_Udp_LnReg
* 函数功能：  归还欠息后登记簿处理
* 作    者：  rob
* 完成时间：  2003年01月18日
*
* 参    数：
*     输  入：   ac_id    integer        账户ID
*                ac_seqn  smallint       帐户序号    
*                type     char[2]        归还类型：1本金、2表内欠息、3表外欠息、4复利欠息
*	         amt	  double	 归还金额
*      
*     输  出:    amt	  double	 归还金额
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int pub_ln_Udp_LnReg ( long ac_id , int ac_seqn , char *type , double pamt )
{
  	int ret;
  	double amt_tmp,amt;
	char sts[2];
	long tx_date=0;
	int flag=0;
	int while_sts=1;

  	struct ln_reg_c	ln_reg;	
	memset( &ln_reg,0x00,sizeof(struct ln_reg_c) );	
	amt=pamt;
	
	sprintf(acErrMsg,"金额![%lf]",amt);
	WRITEMSG
	
	
	ret = Ln_reg_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and type=%s and sts='0' order by reg_date_beg ", ac_id,ac_seqn,type );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_reg_Dec_Upd错!ret=[%d]", ret);
		WRITEMSG	
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
		return(1);	
	}			

	flag=0;

		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
	while( while_sts )
	{
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
		ret = Ln_reg_Fet_Upd( &ln_reg, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_reg_Fet_Upd error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}
		else if( ret==100 )
		break;
		
		amt_tmp = ln_reg.pay_bal + amt;

		flag++;

		/* 还款处理 */
		/* 若欠款金额小于已归还金额+还款金额) */
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	

		if ( pub_base_CompDblVal( ln_reg.lo_bal , amt_tmp ) == -1 )
		{
			amt-=(ln_reg.lo_bal-ln_reg.pay_bal);

			ln_reg.pay_bal = ln_reg.lo_bal ;/* 已归还金额 */
			strcpy( sts,"1" );
			tx_date=g_pub_tx.tx_date;

			sprintf( acErrMsg,"欠款金额小于(已归还金额+还款金额)[%lf]",amt);
	 		WRITEMSG
	 		ln_reg.pay_date = g_pub_tx.tx_date;
	 		strcpy( ln_reg.sts , "1" );

			ret=Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
			if(ret)	return ret;	
			while_sts=1;
			if( flag==1 )
			{
				g_pub_tx.acm_beg_date = ln_reg.reg_date_beg ;
			}	
TRACE
		}
		else if ( pub_base_CompDblVal( ln_reg.lo_bal , amt_tmp ) == 1 )
		{
			amt = 0;
			ln_reg.pay_bal = amt_tmp ;	/* 已归还金额 */
			strcpy( sts,"0" );

			sprintf( acErrMsg,"欠款金额大于(已归还金额+还款金额)[%lf]",ln_reg.pay_bal);
	 		WRITEMSG
	 		
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
	 		/* 更新 已归还金额,欠款状态,结清日期 */
	 		ln_reg.pay_date = 0;
	 		strcpy( ln_reg.sts , "0" );

			Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
			if(ret)	return ret;
			while_sts=0;
			if( flag==1 )
			{
				g_pub_tx.acm_beg_date = ln_reg.reg_date_beg ;
			}
		}else if ( pub_base_CompDblVal( ln_reg.lo_bal , amt_tmp ) == 0 )/* 若欠款金额等于(已归还金额+还款金额) */
		{
			amt = 0;
			ln_reg.pay_bal = amt_tmp ;	/* 已归还金额 */
			strcpy( sts,"1" );
			tx_date=g_pub_tx.tx_date;
			
			sprintf( acErrMsg,"欠款金额等于(已归还金额+还款金额)[%lf]",amt);
	 		WRITEMSG
	 		
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
	 		/* 更新 已归还金额,欠款状态,结清日期 */
	 		ln_reg.pay_date = g_pub_tx.tx_date;
	 		strcpy( ln_reg.sts , "1" );

			Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
			if(ret)	return ret;
			while_sts=0;
			if( flag==1 )
			{
				g_pub_tx.acm_beg_date = ln_reg.reg_date_beg ;
			}
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
		}
TRACE
	}
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
		Ln_reg_Clo_Upd ();
	if( pub_base_CompDblVal( amt , 0.00 ) )	/* 一般是贷款登记簿中数据问题 */
	{
		sprintf( acErrMsg, "还款金额过大,有结余![%lf]",amt );
		WRITEMSG
		strcpy( g_pub_tx.reply , "L156" );
		return(-1);		
	}	
	if( flag==0 )
	{
		/*没有匹配的记录*/
	}
	sprintf(acErrMsg,"归还欠息后登记簿处理 PASS");
	WRITEMSG
	
	return 0;
}

/*****************************************************
* 函 数 名：  pub_ln_CleanAcm
* 函数功能：  贷款积数清零
* 作    者：  rob
* 完成时间：  2003年01月18日
*
* 参    数：
*     输  入：   ac_id    integer        账户ID
*                ac_seqn  smallint       帐户序号    
*                type     char[2]        类型：0本金 1利息
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int pub_ln_CleanAcm ( long ac_id , int ac_seqn , char *type )
{
  	int ret;

  	struct com_sect_acm_c	com_sect_acm;
	
	memset( &com_sect_acm,0x00,sizeof(struct com_sect_acm_c) );	
		
	/* 修改分段积数表,将状态置为2清理 */
	if ( type[0]=='0' ) /* 类型为本金 */	
	{
		ret = Com_sect_acm_Dec_Upd(g_pub_tx.reply, 
			"ac_id=%ld and ac_seqn=%d and intst_type='1' " ,ac_id ,ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Com_sect_acm_Dec_Upd错!ret=[%d]",ret );
			WRITEMSG
			return(-1);
		}
	}
	else if ( type[0]=='1' ) /* 类型为利息 */
	{
		ret = Com_sect_acm_Dec_Upd(g_pub_tx.reply, 
			"ac_id=%ld and ac_seqn=%d and intst_type in('2','3','4') " ,
			ac_id ,ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Com_sect_acm_Dec_Upd错!ret=[%d]",ret );
			WRITEMSG
			return(-1);
		}
	}	
	else 
	{
		sprintf( acErrMsg, "输入的类型错误!不是0 ,1");
		WRITEMSG
		return(-1);	
	}

	while(1)
	{
		ret = Com_sect_acm_Fet_Upd( &com_sect_acm, g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret )
 		{
	 		sprintf( acErrMsg,"fetch com_sect_acm error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}
			
		strcpy( com_sect_acm.sts , "2" );	/* 修改状态:清理 */
			
		ret = Com_sect_acm_Upd_Upd( com_sect_acm, g_pub_tx.reply  );
		if ( ret )
		{
			sprintf( acErrMsg,"Com_sect_acm_Upd_Upd 出错[%d]",ret);
			WRITEMSG
			return 1;
		}		
	}
	Com_sect_acm_Clo_Upd( );
	sprintf(acErrMsg,"更新分段积数表中状态 PASS");
	WRITEMSG

	/* 修改主文件中记录 */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply, 
		"ac_id=%ld and ac_seqn=%d ",ac_id , ac_seqn );	
	if( ret )
	{
		sprintf( acErrMsg,"Ln_mst_Dec_Upd 游标出错[%d]",ret);
		WRITEMSG
		return 1;
	}	
	
	ret = Ln_mst_Fet_Upd( &g_ln_mst, g_pub_tx.reply );
	if( ret && ret!=100 )
 	{
		sprintf( acErrMsg,"fetch ln_mst error code = [%d]",ret);
	 	WRITEMSG
	 	return 1;
 	}
   	if( ret && ret!=100 )
 	{
		sprintf( acErrMsg,"fetch ln_mst error code = [%d]",ret);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"D103" );
	 	return 1;
 	}
	
	/* 类型为本金 */	
	if ( type[0]=='0' )
	{
		g_ln_mst.intst_acm = 0;			/* 利息积数 */
		g_ln_mst.lst_date = g_pub_tx.tx_date;	/* 上笔发生日期 */		
	}
	/* 类型为利息 */
	if ( type[0]=='1' )
	{
		g_ln_mst.in_lo_acm = 0;			/* 表内欠息积数 */
		g_ln_mst.out_lo_acm= 0;			/* 表外欠息积数 */
		g_ln_mst.cmpd_lo_acm = 0;		/* 复利积数 */				
		g_ln_mst.lst_date = g_pub_tx.tx_date;	/* 上笔发生日期 */
	}
					
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg,"Com_sect_acm_Upd_Upd 出错[%d]",ret);
		WRITEMSG
		return 1;
	}		
	
	Ln_mst_Clo_Upd( );
	sprintf(acErrMsg,"更新贷款主文件 PASS");
	WRITEMSG		
		
	return 0 ;
}
/**********************************************************************
* 函 数 名：  pub_ln_ln_reg
* 函数功能：  登记贷款登记簿
* 作    者：  rob
* 完成时间：  2003年03月31日
*
* 参    数：
*     输  入：   p_ln_mst           	贷款主文件结构   
*                beg_date		起始日期  
*		 tx_date		交易日期(终止日期)
*                type     char[2]       类型：1本金、2表内欠息、3表外欠息、4复利欠息
*	         amt	  double	金额
*		 acm	  double        积数
*                ind      char[2]       结息标志0：非结息日；1：结息日数据
*                mode     int	        登记模式 0登记欠款 1回收已登记欠款 2登记并回收
*          
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_ln_reg( struct ln_mst_c ln_mst,long beg_date , long tx_date,char *type,
	double amt, double acm , char *ind,int mode )
{
	struct ln_reg_c ln_reg;
	int ret=0;
	
	sprintf(acErrMsg,"金额![%lf]",amt);
	WRITEMSG
	
	memset( &ln_reg,0,sizeof(ln_reg) );

	
	if( amt<0.005 ) return 0;
	
	
	ln_reg.ac_id = ln_mst.ac_id;		/* 账户ID */
	ln_reg.ac_seqn = ln_mst.ac_seqn;	/* 账户序号 */	
	if( mode==0 )
	{
		ln_reg.reg_date_beg = beg_date;	/* 起始日期 */
		ln_reg.reg_date_end = tx_date;	/* 终止日期 */
		ln_reg.pay_date = 0;	/* 结清日期 */
		strcpy ( ln_reg.type,type );	/* 类型 */
		ln_reg.lo_bal = amt;		/* 欠款金额 */
		ln_reg.acm = acm;		/* 积数 */
		ln_reg.pay_bal = 0.00;		/* 已归还金额 */
		ln_reg.sts[0]='0';			/* 欠款状态：0未还清1已还清 */
		strcpy( ln_reg.ind , ind );	/* 是否结息日 */
		ret = Ln_reg_Ins( ln_reg , g_pub_tx.reply );
		if( ret )
		{
			sprintf(acErrMsg,"登记贷款登记簿失败![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L041");
			return ret;
		}
		g_pub_tx.acm_beg_date = ln_reg.reg_date_beg;
		sprintf(acErrMsg,"mode 0!");
		WRITEMSG
	}
	else if( mode==2 )
	{
		ln_reg.reg_date_beg = beg_date;	/* 起始日期 */
		ln_reg.reg_date_end = tx_date;	/* 终止日期 */
		ln_reg.pay_date = tx_date;	/* 结清日期 */
		strcpy ( ln_reg.type,type );	/* 类型 */
		ln_reg.lo_bal = amt;		/* 欠款金额 */
		ln_reg.acm = acm;		/* 积数 */		
		ln_reg.pay_bal = amt;		/* 已归还金额 */
		ln_reg.sts[0]='1';			/* 欠款状态：0未还清1已还清 */
		strcpy( ln_reg.ind , ind );	/* 是否结息日 */
		ret = Ln_reg_Ins( ln_reg , g_pub_tx.reply );
		if( ret )
		{
			sprintf(acErrMsg,"登记贷款登记簿失败![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L041");
			return ret;
		}
		g_pub_tx.acm_beg_date = ln_reg.reg_date_beg;
		sprintf(acErrMsg,"mode 2!");
		WRITEMSG
	}
	else
	{
		sprintf(acErrMsg,"mode 3进入!");
		WRITEMSG
		sprintf(acErrMsg,"金额![%lf]",amt);
		WRITEMSG
		ret=pub_ln_Udp_LnReg(ln_mst.ac_id,ln_mst.ac_seqn,type,amt);
		if( ret ) return ret;
		sprintf(acErrMsg,"mode 3!PASS");
		WRITEMSG
	}

	return 0;
}
/**********************************************************************
* 函 数 名：  pub_ln_CalPunIntst
* 函数功能：  贷款算罚息
* 作    者：  rob
* 完成时间：  2003年03月30日
*
* 参    数：
*     输  入：   p_ln_mst           	贷款主文件结构
*                p_ln_parm          	贷款参数结构
*                amt                    金额
*                beg_date		起始日期
*		 end_date		终止日期    
*     
*     输  出:    pun_intst	double	罚息总数(本金×罚息利率)
*		 cmpd_intst	double	复利总数(利息×合同利率)
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_CalPunIntst ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , double amt , double intst , long beg_date , long end_date , double *pun_intst , double *cmpd_intst )
{
  	int ret;		
  	double fine_rate;
  	double cmpd_rate;
  	long days;
	sprintf(acErrMsg,"beg_date=[%d] end_date=[%d]",beg_date,end_date);
	WRITEMSG
	TRACE
	/* 取罚息利率 */
	sprintf(acErrMsg,"fine_rate_no[%lf]cur_no[%s]date[%ld]",p_ln_mst.fine_rate, p_ln_parm.cur_no, g_pub_tx.tx_date);
	WRITEMSG
	/*****	
	ret = pub_base_getllz(p_ln_parm.fine_rate_no, p_ln_parm.cur_no, g_pub_tx.tx_date, &rate);
	if ( ret )
	{
		sprintf(acErrMsg,"调函数根据罚息利率编号币种日期,取利率错误!");
		WRITEMSG
		return 1;	
	}
	************/
	TRACE
	fine_rate = p_ln_mst.fine_rate;
	cmpd_rate = p_ln_mst.over_rate;
	sprintf(acErrMsg,"罚息利率[%lf]!",fine_rate);
	WRITEMSG	
	sprintf(acErrMsg,"利息复息利率[%lf]!",cmpd_rate);
	WRITEMSG
	fine_rate= fine_rate/30/1000;/* 转换成日利率 */
	cmpd_rate= cmpd_rate/30/1000;/* 转换成日利率 */
	sprintf(acErrMsg,"根据罚息利率编号取fine_rate利率值[%lf]cmpd_rate利率值[%lf]",fine_rate,cmpd_rate);
	WRITEMSG
	sprintf(acErrMsg,"beg_date=[%d]end_date=[%d] [%s]",beg_date,end_date,p_ln_parm.intst_days_type);
	WRITEMSG
	/* 计算天数 */
	ret = pub_base_CalDays( beg_date , end_date , p_ln_parm.intst_days_type , &days );
	if ( ret )
	{
		sprintf(acErrMsg,"调用计算天数函数错误!");
		WRITEMSG
		return 1;	
	}
	sprintf(acErrMsg,"调用计算天数函数 PASS!天数为[%ld]",days);
	WRITEMSG
	
	/* 罚息为本金*罚息利率 */
	*pun_intst= amt*days*fine_rate;
	/* 复息为利息*合同利率 */
	*cmpd_intst= intst*days*cmpd_rate;
	TRACE
	
	*pun_intst = pub_base_PubDround_1( *pun_intst );
	*cmpd_intst = pub_base_PubDround_1( *cmpd_intst );
			
	sprintf(acErrMsg,"罚息pun_intst=[%lf]",*pun_intst);
	WRITEMSG
	sprintf(acErrMsg,"复息cmpd_intst=[%lf]",*cmpd_intst);
	WRITEMSG
			
	return 0;
}
/**********************************************************************
* 函 数 名：  pub_ln_Ins_ln_reg
* 函数功能：  添加贷款登记簿
* 作    者：  rob
* 完成时间：  2003年03月31日
*
* 参    数：
*     输  入：   p_ln_mst           	贷款主文件结构   
*                p_ln_parm		贷款参数结构  
*                type     char[2]       类型：1本金、2表内欠息、3表外欠息、4复利欠息
*	         pamt	  double	金额
*                ind      char[2]       结息标志0：非结息日；1：结息日数据
*          
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_Ins_ln_reg( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *type , double pamt , double acm , char *ind)
{
  	int ret;
  	struct ln_reg_c    ln_reg_tmp;
  	
  	memset( &ln_reg_tmp , 0x00 , sizeof(struct ln_reg_c) );
  	sprintf(acErrMsg,"pub_ln_Ins_ln_reg金额![%lf]",pamt);
	WRITEMSG
		
  	ln_reg_tmp.ac_id = p_ln_mst.ac_id;
  	ln_reg_tmp.ac_seqn = p_ln_mst.ac_seqn;
   	ln_reg_tmp.reg_date_beg = p_ln_mst.lst_date; 	
  	ln_reg_tmp.reg_date_end = g_pub_tx.tx_date;
vtcp_log("A [%d][%d]",ln_reg_tmp.reg_date_beg,ln_reg_tmp.reg_date_beg);
  	strcpy ( ln_reg_tmp.type , type );
  	ln_reg_tmp.lo_bal = pamt;
  	ln_reg_tmp.acm = acm;
vtcp_log("B [%lf][%lf]",ln_reg_tmp.lo_bal,ln_reg_tmp.acm );
  	strcpy( ln_reg_tmp.ind , ind );
  	strcpy( ln_reg_tmp.sts , "0" );
  	
  	ret = Ln_reg_Ins( ln_reg_tmp , g_pub_tx.reply );
        if(ret)
        {
            sprintf(acErrMsg,"登记贷款登记薄失败");
            WRITEMSG
            strcpy(g_pub_tx.reply,"L082");
            return 1;
        }

  return 0 ;
}
/**********************************************************************
* 函 数 名：  pub_ln_Ins_ln_lo
* 函数功能：  登记贷款欠款表
* 作    者：  rob
* 完成时间：  2003年03月31日
*
* 参    数：
*     输  入：   p_ln_mst           	贷款主文件结构   
*                p_ln_pay_pln		    贷款参数结构  
*                type     char[2]       类型：1已还清 0 未还清
*	             amt	  double	    本金金额
*                intst    double        利息金额
*          
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_Ins_ln_lo( struct ln_mst_c p_ln_mst , 
		struct ln_pay_pln_c p_ln_pay_pln , char *type , double amt , 
		double intst , double pay_amt , double pay_intst )
{
  	int ret=0;
  	struct ln_lo_c    ln_lo_tmp;
  	
  	memset( &ln_lo_tmp , 0x00 , sizeof(struct ln_lo_c) );
  	
  	ln_lo_tmp.ac_id = p_ln_mst.ac_id;
  	ln_lo_tmp.ac_seqn = p_ln_mst.ac_seqn;
  	ln_lo_tmp.lo_term = p_ln_pay_pln.curr_cnt;
  	ln_lo_tmp.lo_amt = amt;
   	ln_lo_tmp.lo_intst = intst; 	
	ln_lo_tmp.pun_intst = 0.00;
	strcpy ( ln_lo_tmp.pay_type , type );
	ln_lo_tmp.over_date = 0;
	ln_lo_tmp.shld_pay_date = p_ln_pay_pln.end_date;
	ln_lo_tmp.pay_lo_amt = pay_amt;
	ln_lo_tmp.pay_lo_intst = pay_intst;
	ln_lo_tmp.pay_pun_intst = 0.00;
	strcpy( ln_lo_tmp.old_ac_ind , "0" );	/* 新系统数据 */   	
  	ret = Ln_lo_Ins( ln_lo_tmp , g_pub_tx.reply );
    if(ret)
    {
    	sprintf(acErrMsg,"登记贷款欠款表失败");
        WRITEMSG
       	strcpy(g_pub_tx.reply,"L083");
        return 1;
    }

  return 0 ;
}
/**********************************************************************
* 函 数 名：  Ln_eod_pri_InsReg
* 函数功能：  添加贷款日终打印登记簿
* 作    者：  rob
* 完成时间：  2003年08月28日
*
* 参    数： 
*     输  入：   
*		ln_ac_no	Char(20)	贷款账号
*		p_ln_mst           		贷款主文件结构
*		pay_ac_no	Char(20)	还款账号
*		pay_ac_seqn	smallint	还款账户序号
*		pay_ac_name	Char(50)	还款户名
*		amt		decimal(16,2)	交易金额
*		acm		decimal(19,2)	积数
*		tx_code		char(4)		交易类型 	
*		amt_type	Char(1)		金额类型 1：贷款本金； 2：表内利息；3：表外利息；4：复利利息；	
*		acm_type	Char(1)		积数类型 0：存款；1：贷款本金积数； 2：贷款表内欠息积数；3：贷款表外欠息积数；4：贷款复利积数；5：透支
*		beg_date	Integer		起始日期	
*		end_date	Integer		终止日期	
*		trace_no	Integer		流水号	
*		tx_date		Integer		交易日期
*	      
*     输  出: 
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int Ln_eod_pri_InsReg( char *ln_ac_no, struct ln_mst_c p_ln_mst, char *pay_ac_no, int pay_ac_seqn, char *pay_ac_name, 
			double amt, double acm, char *tx_code, char *amt_type, char *acm_type, long beg_date, long end_date, 
			long trace_no , long tx_date )
{
	int ret=0;
	struct ln_eod_pri_c	S_ln_eod_pri;
	memset( &S_ln_eod_pri, 0x00, sizeof( struct ln_eod_pri_c));
	
	strcpy( S_ln_eod_pri.ln_ac_no , ln_ac_no);
	S_ln_eod_pri.ln_ac_seqn = p_ln_mst.ac_seqn;
	strcpy( S_ln_eod_pri.ln_ac_name , p_ln_mst.name);
	strcpy( S_ln_eod_pri.ln_pact_no , p_ln_mst.pact_no);
	strcpy( S_ln_eod_pri.ln_ac_prdt , p_ln_mst.prdt_no);
	strcpy( S_ln_eod_pri.pay_ac_no , pay_ac_no );
	S_ln_eod_pri.pay_ac_seqn = pay_ac_seqn;
	strcpy(  S_ln_eod_pri.pay_ac_name , pay_ac_name );
	S_ln_eod_pri.amt = amt;
	S_ln_eod_pri.acm = acm;
	S_ln_eod_pri.rate = p_ln_mst.rate;
	S_ln_eod_pri.over_rate = p_ln_mst.over_rate;
	strcpy( S_ln_eod_pri.tx_code , tx_code );
	strcpy( S_ln_eod_pri.amt_type , amt_type );
	strcpy( S_ln_eod_pri.acm_type , acm_type );
	S_ln_eod_pri.beg_date = beg_date;
	S_ln_eod_pri.end_date = end_date;
	S_ln_eod_pri.trace_no = trace_no;
	S_ln_eod_pri.tx_date = tx_date;	
	
	TRACE
	sprintf( acErrMsg,"S_ln_eod_pri.amt=[%lf]acm=[%lf]",S_ln_eod_pri.amt,S_ln_eod_pri.acm);
	WRITEMSG
	ret = Ln_eod_pri_Ins( S_ln_eod_pri , g_pub_tx.reply );
	if( ret )	return 1;
	
	return 0;
}
