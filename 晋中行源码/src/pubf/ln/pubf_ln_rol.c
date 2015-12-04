/*************************************************
* 文 件 名:    pubf_ln_rol.c
* 功能描述：   
*              get_ln_mst_intst    	 根据流水号，查询贷款明细文件，取出利息类型，积数
*	      	   pub_ln_Udp_LnReg_Back	 归还欠息后登记簿处理(撤销)
*	           pub_ln_Del_LnReg		 根据流水，查询贷款明细，删除相关记录
*              pub_ln_mst_acm_Back	 贷款主文件中积数处理(撤销交易时)
*              pub_ln_sect_acm_Back	 分段积数表中积数处理(撤销贷款交易时)
*              Ins_ln_lo_rol		 登记按揭欠款撤销登记簿(表中记录当时做按揭还款交易时, 贷款欠款表中数据)
*
* 作    者:    rob
* 完成日期：   2003年07月06日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "ln_mst_hst_c.h"
#include "ln_reg_c.h"
#include "trace_log_c.h"
#include "ln_acm_reg_c.h"
#include "com_sect_acm_c.h"
#include "ln_lo_c.h"
#include "ln_lo_rol_c.h"
#include "ln_pay_pln_c.h"
#include "ln_pay_pln_rol_c.h"

/**********************************************************************
* 函 数 名：  get_ln_mst_intst
* 函数功能：  根据流水号，查询贷款明细文件，取出利息类型，积数
* 作    者：  rob
* 完成时间：  2003年06月27日
*
* 参    数：
*     输  入：   ptrace_log		 流水结构
*                acm_type  char[2]       利息类型    
*	         acm	  double	 积数
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int get_ln_mst_intst( ptrace_log , acm_type , acm )
  struct trace_log_c *ptrace_log;
  char *acm_type;
  double *acm;
 {
	int ret;
	struct ln_mst_hst_c	t_ln_mst_hst;
	memset( &t_ln_mst_hst , 0x00 , sizeof( struct ln_mst_hst_c ) );
	
	TRACE
	
	sprintf( acErrMsg , "流水trace_no=%ld笔数trace_cnt=%d" , ptrace_log->trace_no , ptrace_log->trace_cnt);
	WRITEMSG
	
	TRACE
	
	ret = Ln_mst_hst_Sel( g_pub_tx.reply , &t_ln_mst_hst , "trace_no=%ld and trace_cnt=%d and tx_date=%ld " , ptrace_log->trace_no , ptrace_log->trace_cnt ,ptrace_log->tx_date);
	if( ret )	return 1;
	TRACE
	if( t_ln_mst_hst.ln_tx_type[0]=='0' )	/* 本金 */
	{
		strcpy( acm_type , "1" );
	}else if( t_ln_mst_hst.ln_tx_type[0]=='1' ){	/* 表内 */
		strcpy( acm_type , "2" );
	}else if( t_ln_mst_hst.ln_tx_type[0]=='2' ){	/* 表外 */
		strcpy( acm_type , "3" );
	}else if( t_ln_mst_hst.ln_tx_type[0]=='3' || t_ln_mst_hst.ln_tx_type[0]=='5' || t_ln_mst_hst.ln_tx_type[0]=='6' || t_ln_mst_hst.ln_tx_type[0]=='7' ){	/* 复利 */
		strcpy( acm_type , "4" );
	}		
	
	*acm = t_ln_mst_hst.intst_acm ;
	return 0;
 } 
	
	
/**********************************************************************
* 函 数 名：  pub_ln_Udp_LnReg_Back
* 函数功能：  归还欠息后登记簿处理(撤销)
* 作    者：  rob
* 完成时间：  2003年06月27日
*
* 参    数：
*     输  入：   ac_id    integer        账户ID
*                ac_seqn  smallint       帐户序号    
*                type     char[2]        归还类型：1本金、2表内欠息、3表外欠息、4复利欠息
*	         pamt	  double	 归还金额
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int pub_ln_Udp_LnReg_Back ( long ac_id , int ac_seqn , char *type , double pamt )
{
  	int ret;
  	double amt_tmp,amt;
	char sts[2];
	long tx_date=0;
	int flag=0;

  	struct ln_reg_c	ln_reg;	
	memset( &ln_reg,0x00,sizeof(struct ln_reg_c) );	
	amt=pamt;

	sprintf(acErrMsg,"QQac_id[%ld]ac_seqn[%d]type[%s]",ac_id,ac_seqn,type);
	WRITEMSG	
	sprintf(acErrMsg,"金额![%lf]",amt);
	WRITEMSG
	
	
	if( pub_base_CompDblVal( amt , 0.00 ) )
	{	
		ret = Ln_reg_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and type='%s' and sts='0'  order by reg_date_beg desc", ac_id,ac_seqn,type );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Ln_reg_Dec_Sel错!ret=[%d]", ret);
			WRITEMSG		
			return(1);	
		}			
		
		flag=0;
	
		while(1)
		{
			ret = Ln_reg_Fet_Upd( &ln_reg, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		sprintf( acErrMsg,"fetch Ln_reg_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		return 1;
	 		}
			else if( ret==100 ){
				break;
			}
	
			flag++;
			
			/* 反还款处理 */
			/* 检查未还清记录 */
			
			/* 若补款金额大于已还息金额 */
			amt_tmp = amt - ln_reg.pay_bal;
			sprintf(acErrMsg,"amt![%lf]amt_tmp![%lf]",amt,amt_tmp);
			WRITEMSG
						
			if ( pub_base_CompDblVal( amt_tmp , 0.00 ) ==1 )
			{
				TRACE
				amt-=ln_reg.pay_bal;   /* 剩余金额 */
				sprintf(acErrMsg,"金额![%lf]",amt);
				WRITEMSG
				ln_reg.pay_bal = 0.00 ;/* 已归还金额 */
				ret = Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
				if( ret )
				{					
					sprintf(acErrMsg,"Ln_reg_Upd_Upd异常出错!");
					WRITEMSG
					return 1;			
				}							
				
			}else if ( pub_base_CompDblVal( amt_tmp , 0.00 ) != 1 )	/* 若补款金额小于或等于已还息金额 */
			{
				TRACE
				ln_reg.pay_bal = ln_reg.pay_bal - amt ;	/* 已归还金额 */
				amt = 0.00;
				ret = Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
				if( ret )
				{					
					sprintf(acErrMsg,"Ln_reg_Upd_Upd异常出错!");
					WRITEMSG
					return 1;			
				}

				break;
			}
					
		}
		Ln_reg_Clo_Upd ();
	}

	if( pub_base_CompDblVal( amt , 0.00 ) )
	{
		sprintf(acErrMsg,"金额![%lf]",amt);
		WRITEMSG
		memset( &ln_reg,0x00,sizeof(struct ln_reg_c) );	
		/***		
		ret = Ln_reg_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and type='%s' and sts='1' \
										ORDER BY reg_date DESC ", ac_id,ac_seqn,type );
		***/
		ret = Ln_reg_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and type='%s' and sts='1'  order by reg_date_beg desc", ac_id,ac_seqn,type );

		if ( ret )
		{
			sprintf( acErrMsg, "执行Ln_reg_Dec_Sel错!ret=[%d]", ret);
			WRITEMSG		
			return(1);	
		}			
		
		flag=0;
	
		while(1)
		{
			ret = Ln_reg_Fet_Upd( &ln_reg, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		sprintf( acErrMsg,"fetch Ln_reg_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		return 1;
	 		}
			else if( ret==100 )
				break;
	
			flag++;
			
			/* 补款处理 */
			/* 若补款金额大于已还息金额 */
			amt_tmp = amt - ln_reg.pay_bal;
			
			if ( pub_base_CompDblVal( amt_tmp , 0.00 ) == 1 )
			{
				amt-=ln_reg.pay_bal;
	
				ln_reg.pay_bal = 0.00 ;/* 已归还金额 */
				strcpy( ln_reg.sts,"0" );
				ln_reg.pay_date=0;
				
		 		/* 更新 已归还金额,欠款状态,结清日期 *
				sprintf( acErrMsg,"UPDATE ln_reg SET pay_bal=0.00,pay_date=0,sts='0'\
								WHERE ac_id=%ld and ac_seqn=%d and type='%s' and reg_date=%ld" , 
								ln_reg.ac_id,ln_reg.ac_seqn,ln_reg.type,ln_reg.reg_date );
		 		WRITEMSG
				ret = sql_execute( "UPDATE ln_reg SET pay_bal=0.00,pay_date=0,sts='0'\
								WHERE ac_id=%ld and ac_seqn=%d and type='%s' and reg_date=%ld" , 
								ln_reg.ac_id,ln_reg.ac_seqn,ln_reg.type,ln_reg.reg_date );
				if( ret ) return ret;
				*/
				ret = Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
				if( ret )
				{					
					sprintf(acErrMsg,"Ln_reg_Upd_Upd异常出错!");
					WRITEMSG
					return 1;			
				}
			}		
			else if ( pub_base_CompDblVal( amt_tmp , 0.00 ) != 1 )
			{
				ln_reg.pay_bal = ln_reg.pay_bal - amt ;	/* 已归还金额 */
				amt =0.00;
				strcpy( ln_reg.sts,"0" );
				ln_reg.pay_date=0;
		 		
		 		/* 更新 已归还金额,欠款状态,结清日期 *
				sprintf( acErrMsg,"UPDATE ln_reg SET pay_bal=%lf,pay_date=0,sts='0' \
								WHERE ac_id=%ld and ac_seqn=%d and type='%s' and reg_date=%ld" , 
								ln_reg.pay_bal,ln_reg.ac_id,ln_reg.ac_seqn,ln_reg.type,ln_reg.reg_date );
		 		WRITEMSG
				ret = sql_execute( "UPDATE ln_reg SET pay_bal=%lf,pay_date=0,sts='0' \
								WHERE ac_id=%ld and ac_seqn=%d and type='%s' and reg_date=%ld" , 
								ln_reg.pay_bal,ln_reg.ac_id,ln_reg.ac_seqn,ln_reg.type,ln_reg.reg_date );
				if( ret ) return ret;
				*/
				ret = Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
				if( ret )
				{					
					sprintf(acErrMsg,"Ln_reg_Upd_Upd异常出错!");
					WRITEMSG
					return 1;			
				}
				break;
			}
		}
		Ln_reg_Clo_Upd ();		

	}
	
	return 0;
}	 	


/**********************************************************************
* 函 数 名：  pub_ln_Del_LnReg
* 函数功能：  根据流水，查询贷款明细，删除相关记录
*             此类信息只可能有几种情况产生,1-贷款放款 2-普通贷款还息时积数新产生的利息
* 作    者：  rob
* 完成时间：  2003年06月27日
*
* 参    数：
*     输  入：   ptrace_log		 流水结构
*      		 amt_type		 金额类型 1-贷款放款 2-普通贷款还息时积数新产生的利息
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int pub_ln_Del_LnReg( ptrace_log , amt_type )
  struct trace_log_c *ptrace_log ;
  char amt_type[2];
{
	int ret=0;
	int del_flag=0;
	char tmpstr[500];
	char con_list[51];
	struct ln_mst_hst_c	t_ln_mst_hst;
	struct ln_reg_c		t_ln_reg;
	
	memset( &t_ln_mst_hst , 0x00 , sizeof( struct ln_mst_hst_c ) );
	memset( &t_ln_reg , 0x00 , sizeof( struct ln_reg_c ) );

	memset( &tmpstr , 0x00 , sizeof( tmpstr ) );
	memset( &con_list , 0x00 , sizeof( con_list ) );	
		
	/* 取贷款明细 */
	ret = Ln_mst_hst_Sel( g_pub_tx.reply , &t_ln_mst_hst , "trace_no=%ld and trace_cnt=%d and tx_date=%ld " , ptrace_log->trace_no , ptrace_log->trace_cnt, g_pub_tx.tx_date );
	if( ret )	return 1;	
	
	sprintf(acErrMsg,"trace_no[%ld]trace_cnt[%d]",ptrace_log->trace_no , ptrace_log->trace_cnt);
	WRITEMSG	
	sprintf(acErrMsg,"金额![%lf]",ptrace_log->amt);
	WRITEMSG
	sprintf(acErrMsg,"类型![%s]",t_ln_mst_hst.ln_tx_type);
	WRITEMSG
		
	/* 拼接条件语句 */
	if( amt_type[0]=='1' )
	{
		strcpy( tmpstr,"ac_id=%ld and ac_seqn=%d and reg_date_beg=%ld  and lo_bal=%lf and ind='0' " );
	}else if( amt_type[0]=='2' ){
		strcpy( tmpstr,"ac_id=%ld and ac_seqn=%d and reg_date_end=%ld  and lo_bal=%lf and ind='0' " );
	}else{
		sprintf(acErrMsg,"金额类型错误![%s]",amt_type);
		WRITEMSG
		return 1;		
	}
	/* 复利 */
	/***
	if( t_ln_mst_hst.ln_tx_type[0]=='5' || t_ln_mst_hst.ln_tx_type[0]=='6' || t_ln_mst_hst.ln_tx_type[0]=='7' )
	{
	*******/
	if( t_ln_mst_hst.ln_tx_type[0]=='3' )
	{	
		strcpy( con_list , "and type='4' and sts='1' " );
	}
	
	/* 本金积数 *
	if( t_ln_mst_hst.ln_tx_type[0]=='4' )
	{
		strcpy( con_list , "and type in ('2','3') and sts='1'" );
	}		
	*******/
	
	/* 本金积数入表内 */
	if( t_ln_mst_hst.ln_tx_type[0]=='1' )	
	{
		TRACE
		strcpy( con_list , "and type='2' and sts='1' " );
	}	
	/* 本金积数入表外 */				
	if( t_ln_mst_hst.ln_tx_type[0]=='2' )
	{
		TRACE	
		strcpy( con_list , "and type='3' and sts='1' " );
	}			
	
	/* 贷款本金 */
	if( t_ln_mst_hst.ln_tx_type[0]=='0' )
	{	
		TRACE
		strcpy( con_list , "and type='1' " );
	}	
	sprintf(acErrMsg,"条件1tmpstr![%s]",tmpstr);
	WRITEMSG
	sprintf(acErrMsg,"con_list![%s]",con_list);
	WRITEMSG
	/***
	strcat(tmpstr,con_list);
	*******/
	
	sprintf(acErrMsg,"条件3tmpstr![%s]",tmpstr);
	WRITEMSG	
	sprintf(acErrMsg,"[%ld][%d][%ld][%lf]",t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->amt);
	WRITEMSG	
	/* 删除操作 */
	del_flag=0;	/* 记录删除数据数 */
	ret = Ln_reg_Dec_Upd( g_pub_tx.reply , tmpstr , t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->amt );
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Ln_reg_Dec_Upd 游标出错[%d]",ret);
		WRITEMSG
		return 1;
	}
	ret = Ln_reg_Fet_Upd( &t_ln_reg , g_pub_tx.reply );
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款已归还过,请先做还款撤销![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply , "L179");
		return 1;		
	}else if( ret ){
		sprintf( acErrMsg,"查询贷款登记簿信息异常出错![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply , "L180");
		return 1;
	}
	if( del_flag )
	{
		Ln_reg_Clo_Upd( );
		return 0;
	}else{		
		del_flag++;
	}
	
	ret = Ln_reg_Del_Upd(t_ln_reg, g_pub_tx.reply );
	if( ret )
	{
		sprintf( acErrMsg,"Ln_reg_Del_Upd异常出错![%d]",ret);
		WRITEMSG
		return 1;
	}
	Ln_reg_Clo_Upd( );		
	
	
	/* 根据贷款详细信息取贷款登记簿中记录----复利 *
	if( t_ln_mst_hst.ln_tx_type[0]=='5' || t_ln_mst_hst.ln_tx_type[0]=='6' || t_ln_mst_hst.ln_tx_type[0]=='7' )
	{
		ret=sql_execute("delete from ln_reg where ac_id= %ld and ac_seqn=%d and reg_date=%ld and pay_date=%ld and type='4' and ind='0' and sts='1'",
				t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->tx_date );
		if( ret )	return 1;		
	}
	* 根据贷款详细信息取贷款登记簿中记录----本金积数 *
	if( t_ln_mst_hst.ln_tx_type[0]=='4' )
	{
		ret=sql_execute("delete from ln_reg where ac_id= %ld and ac_seqn=%d and reg_date=%ld and pay_date=%ld  and ind='0' and sts='1' and type in ('2' ,'3') ",
				t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->tx_date );
		if( ret )	return 1;		
	}	
	* 根据贷款详细信息取贷款登记簿中记录-----本金 *
	if( t_ln_mst_hst.ln_tx_type[0]=='0' )
	{
		ret=sql_execute("delete from ln_reg where ac_id= %ld and ac_seqn=%d and reg_date=%ld and pay_date=%ld  and ind='0' and sts='1' and type ='1'",
				t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->tx_date );
		if( ret )	return 1;		
	}
	*************/		
	return 0;
}

/**********************************************************************
* 函 数 名：  pub_ln_mst_acm_Back
* 函数功能：  贷款主文件中积数处理(撤销交易时)
* 作    者：  rob
* 完成时间：  2003年07月08日
*
* 参    数：
*     输  入：   trace_log	流水结构
*      
*     输  出:    ln_mst		贷款主文件结构
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int pub_ln_mst_acm_Back ( struct trace_log_c	*s_trace_log , struct ln_mst_c *ln_mst_tmp) 
{
	int ret=0;
	int flag=0;
	
	struct ln_acm_reg_c	ln_acm_reg_tmp;
	
	memset( ln_mst_tmp , 0x00 , sizeof( struct ln_mst_c ) );
	memset( &ln_acm_reg_tmp , 0x00 , sizeof( struct ln_acm_reg_c ) );
		
	ret = Ln_acm_reg_Dec_Sel( g_pub_tx.reply , "trace_no=%ld and ac_id=%ld and ac_seqn=%d  and type='1' ", s_trace_log->trace_no,s_trace_log->ac_id,s_trace_log->ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_acm_reg_Dec_Sel错!ret=[%d]", ret);
		WRITEMSG		
		return 1;	
	}			
	flag=0;
	
	while(1)
	{
		ret = Ln_acm_reg_Fet_Sel( &ln_acm_reg_tmp, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_acm_reg_Fet_Sel error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}else if( ret==100 ){
			break;
		}

		flag++;
		ln_mst_tmp->lst_date = ln_acm_reg_tmp.beg_date ; 
		ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date ;
		
		if( ln_acm_reg_tmp.intst_type[0]=='1' )	/*本金积数*/
		{
			ln_mst_tmp->intst_acm = ln_acm_reg_tmp.intst_acm;
			ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date;
		}else if( ln_acm_reg_tmp.intst_type[0]=='2' ){	/*表内欠息积数*/
			ln_mst_tmp->in_lo_acm = ln_acm_reg_tmp.intst_acm;
			ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date;
		}else if( ln_acm_reg_tmp.intst_type[0]=='3' ){	/*表外欠息积数*/
			ln_mst_tmp->out_lo_acm = ln_acm_reg_tmp.intst_acm;
			ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date;
		}else if( ln_acm_reg_tmp.intst_type[0]=='4' ){	/*复利欠息积数*/
			ln_mst_tmp->cmpd_lo_acm = ln_acm_reg_tmp.intst_acm;
			ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date;
		}else{
			break;
		}
	}
	Ln_acm_reg_Clo_Sel();
	
	sprintf( acErrMsg, "执行次数!flag=[%d]", flag);
	WRITEMSG
	
	return 0;
		
}

/**********************************************************************
* 函 数 名：  pub_ln_sect_acm_Back
* 函数功能：  分段积数表中积数处理(撤销贷款交易时)
* 作    者：  rob
* 完成时间：  2003年07月08日
*
* 参    数：
*     输  入：   trace_log	流水结构
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int pub_ln_sect_acm_Back ( struct trace_log_c	*s_trace_log ) 
{
	int ret=0;
	int flag_ln=0;
	int flag_acm=0;
	
	struct com_sect_acm_c	com_sect_acm_tmp;
	struct ln_acm_reg_c	ln_acm_reg_tmp;
	
	memset( &com_sect_acm_tmp , 0x00 , sizeof( struct com_sect_acm_c ) );
	memset( &ln_acm_reg_tmp , 0x00 , sizeof( struct ln_acm_reg_c ) );
		
	ret = Ln_acm_reg_Dec_Sel( g_pub_tx.reply , "trace_no=%ld  and ac_id=%ld and ac_seqn=%d  and type='0' ", s_trace_log->trace_no,s_trace_log->ac_id,s_trace_log->ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_acm_reg_Dec_Sel错!ret=[%d]", ret);
		WRITEMSG		
		return 1;	
	}			
	flag_ln=0;
	flag_acm=0;	
	
	while(1)
	{
		ret = Ln_acm_reg_Fet_Sel( &ln_acm_reg_tmp, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_acm_reg_Fet_Sel error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}else if( ret==100 ){
			TRACE
			break;
		}

		flag_ln++;
		sprintf( acErrMsg, "flag_ln=[%d]", flag_ln);
		WRITEMSG		
		
		TRACE
		/* 根据ln_reg_acm中信息修改分段积数表中数据 */
		ret = Com_sect_acm_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and beg_date=%ld and end_date=%ld and intst_acm=%lf and intst_type='%s' and sts='1' ", ln_acm_reg_tmp.ac_id,ln_acm_reg_tmp.ac_seqn,ln_acm_reg_tmp.beg_date,ln_acm_reg_tmp.end_date,ln_acm_reg_tmp.intst_acm,ln_acm_reg_tmp.intst_type );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Com_sect_acm_Dec_Upd错!ret=[%d]", ret);
			WRITEMSG	
			sprintf( acErrMsg, "ac_id=%ld and ac_seqn=%d and beg_date=%ld and end_date=%ld and intst_acm=%lf and intst_type='%s' and sts='1' ", ln_acm_reg_tmp.ac_id,ln_acm_reg_tmp.ac_seqn,ln_acm_reg_tmp.beg_date,ln_acm_reg_tmp.end_date,ln_acm_reg_tmp.intst_acm,ln_acm_reg_tmp.intst_type);
			WRITEMSG		
			return(1);	
		}			
		sprintf( acErrMsg, "trace_no=%ld  and ac_id=%ld and ac_seqn=%d  and type='0' ", s_trace_log->trace_no,s_trace_log->ac_id,s_trace_log->ac_seqn );
		WRITEMSG
		sprintf( acErrMsg, "ac_id=%ld and ac_seqn=%d and beg_date=%ld and end_date=%ld and intst_acm=%lf and intst_type='%s' and sts='1' ", ln_acm_reg_tmp.ac_id,ln_acm_reg_tmp.ac_seqn,ln_acm_reg_tmp.beg_date,ln_acm_reg_tmp.end_date,ln_acm_reg_tmp.intst_acm,ln_acm_reg_tmp.intst_type);
		WRITEMSG			
		
		while(1)
		{
			ret = Com_sect_acm_Fet_Upd( &com_sect_acm_tmp, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		TRACE
		 		sprintf( acErrMsg,"fetch Ln_reg_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		return 1;
	 		}else if( ret==100 ){
				sprintf( acErrMsg,"无法在分段积数表中查到相关记录,请与中心机房联系!",ret);
		 		WRITEMSG
		 		strcpy( g_pub_tx.reply , "L186");
		 		break;
		 	}		 			 	
		 		
		 	/****
	 		}else if( ret==100 && flag_acm==0 ){
				sprintf( acErrMsg,"无法在分段积数表中查到相关记录,请与中心机房联系!",ret);
		 		WRITEMSG
		 		strcpy( g_pub_tx.reply , "L186");
		 		return 1;
			}else if( ret==100 && flag_acm!=0){
				TRACE
				break;
			}
			********/
			flag_acm++;
			sprintf( acErrMsg, "flag_acm!=[%d]", flag_acm);
			WRITEMSG			
			/* 修改状态 */
			strcpy( com_sect_acm_tmp.sts , "0" );	/* 修改状态为"未计息" */
			ret= Com_sect_acm_Upd_Upd (com_sect_acm_tmp, g_pub_tx.reply);		
		        if (ret)
		        {
		               sprintf(acErrMsg,"更新分段积数表错误!!");
		               WRITEMSG
		               return 1;
		        }    					
		}
		Com_sect_acm_Clo_Upd ();		
					
	}
	Ln_acm_reg_Clo_Sel();
	
	sprintf( acErrMsg, "执行次数!flag_acm=[%d]++flag_ln=[%d]", flag_acm,flag_ln);
	WRITEMSG
	
	return 0;
		
}



/**********************************************************************
* 函 数 名：  Ins_ln_lo_rol
* 函数功能：  登记按揭欠款撤销登记簿(表中记录为  做按揭还款交易时贷款欠款表中数据)
* 作    者：  rob
* 完成时间：  2003年08月10日
*
* 参    数：
*     输  入：   ln_lo		 按揭贷款欠息表结构
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int Ins_ln_lo_rol( struct ln_lo_c  ln_lo_tmp )
{
	int ret=0;
	struct ln_lo_rol_c	ln_lo_rol;
	memset( &ln_lo_rol , 0x00 , sizeof( struct ln_lo_rol_c ) );
	
	TRACE
	
	sprintf( acErrMsg , "流水trace_no=[%ld]" , g_pub_tx.trace_no );
	WRITEMSG
	
	TRACE
	
	ln_lo_rol.tx_date = g_pub_tx.tx_date;
	ln_lo_rol.trace_no = g_pub_tx.trace_no;
	strcpy ( ln_lo_rol.rol_ind , "0" ); 	/* 0-正常 1-撤销 */
	ln_lo_rol.ac_id = ln_lo_tmp.ac_id;
	ln_lo_rol.ac_seqn = ln_lo_tmp.ac_seqn; 
	ln_lo_rol.lo_term = ln_lo_tmp.lo_term;
	ln_lo_rol.lo_amt = ln_lo_tmp.lo_amt;
	ln_lo_rol.lo_intst = ln_lo_tmp.lo_intst;
	ln_lo_rol.pun_intst = ln_lo_tmp.pun_intst;
	strcpy(	ln_lo_rol.pay_type , ln_lo_tmp.pay_type );
	ln_lo_rol.over_date = ln_lo_tmp.over_date;
	ln_lo_rol.shld_pay_date = ln_lo_tmp.shld_pay_date;
	ln_lo_rol.pay_lo_amt = ln_lo_tmp.pay_lo_amt;
	ln_lo_rol.pay_lo_intst = ln_lo_tmp.pay_lo_intst;
	ln_lo_rol.pay_pun_intst = ln_lo_tmp.pay_pun_intst;
	strcpy(	ln_lo_rol.mac , ln_lo_tmp.mac);
	strcpy(	ln_lo_rol.old_ac_ind , ln_lo_tmp.old_ac_ind );

	ret = Ln_lo_rol_Ins( ln_lo_rol , g_pub_tx.reply );
	if( ret )	
	{
		sprintf( acErrMsg , "登记按揭欠款撤销登记簿异常出错![%d]" , ret );
		WRITEMSG		
		return 1;
	}
	TRACE
	return 0;
 } 

/**********************************************************************
* 函 数 名：  Ins_ln_pay_pln_rol
* 函数功能：  登记按揭还款计划表撤销登记簿(表中记录为  做按揭还款交易时, 贷款还款计划表中数据)
* 作    者：  rob
* 完成时间：  2003年08月10日
*
* 参    数：
*     输  入：   ln_lo		 按揭贷款欠息表结构
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int Ins_ln_pay_pln_rol( struct ln_pay_pln_c  ln_pay_pln_tmp )
{
	int ret=0;
	struct ln_pay_pln_rol_c	ln_pay_pln_rol;
	memset( &ln_pay_pln_rol , 0x00 , sizeof( struct ln_pay_pln_rol_c ) );
	
	TRACE
	
	sprintf( acErrMsg , "流水trace_no=[%ld]" , g_pub_tx.trace_no );
	WRITEMSG
	
	TRACE
	
	ln_pay_pln_rol.tx_date = g_pub_tx.tx_date;
	ln_pay_pln_rol.trace_no = g_pub_tx.trace_no;
	strcpy ( ln_pay_pln_rol.rol_ind , "0" ); 	/* 0-正常 1-撤销 */

	ln_pay_pln_rol.ac_id = ln_pay_pln_tmp.ac_id;
	ln_pay_pln_rol.ac_seqn = ln_pay_pln_tmp.ac_seqn;
	ln_pay_pln_rol.de_ac_id = ln_pay_pln_tmp.de_ac_id;
	ln_pay_pln_rol.de_ac_seqn = ln_pay_pln_tmp.de_ac_seqn;
	ln_pay_pln_rol.ttl_amt = ln_pay_pln_tmp.ttl_amt;
	ln_pay_pln_rol.ttl_cnt = ln_pay_pln_tmp.ttl_cnt;
	ln_pay_pln_rol.curr_cnt = ln_pay_pln_tmp.curr_cnt;
	ln_pay_pln_rol.curr_amt = ln_pay_pln_tmp.curr_amt;
	ln_pay_pln_rol.curr_intst = ln_pay_pln_tmp.curr_intst;
	ln_pay_pln_rol.wrk_date = ln_pay_pln_tmp.wrk_date;
	ln_pay_pln_rol.beg_date = ln_pay_pln_tmp.beg_date;
	ln_pay_pln_rol.end_date = ln_pay_pln_tmp.end_date;
	strcpy ( ln_pay_pln_rol.pay_ind , ln_pay_pln_tmp.pay_ind);
	
	ln_pay_pln_rol.ic_date_aj = g_ln_mst.ic_date;
	
	ret = Ln_pay_pln_rol_Ins( ln_pay_pln_rol , g_pub_tx.reply );
	if( ret )	
	{
		sprintf( acErrMsg , "登记按揭还款计划撤销登记簿异常出错![%d]" , ret );
		WRITEMSG		
		return 1;
	}
	TRACE
	return 0;
 } 

/**********************************************************************
* 函 数 名：  Reback_ln_pay_pln
* 函数功能：  回滚按揭还款计划表(根据 按揭还款计划表撤销登记簿回滚)
* 作    者：  rob
* 完成时间：  2003年08月11日
*
* 参    数：
*     输  入：   ln_pay_pln_rol		 按揭贷款欠息撤销表结构
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/	
int Reback_ln_pay_pln( long trace_no )
{
	int ret=0;
	
	struct ln_pay_pln_rol_c	ln_pay_pln_rol_t;
	struct ln_pay_pln_c	ln_pay_pln;
	
	memset( &ln_pay_pln_rol_t , 0x00 , sizeof( struct ln_pay_pln_rol_c ) );
	memset( &ln_pay_pln , 0x00 , sizeof( struct ln_pay_pln_c ) );	

	TRACE
	
	sprintf( acErrMsg , "流水trace_no=%ld" , trace_no );
	WRITEMSG
	sprintf( acErrMsg , "tx_date=%ld,trace_no=%ld" , g_pub_tx.tx_date , trace_no );
	WRITEMSG	
	TRACE	
	
	/* 取出 按揭还款计划表撤销表中数据 */
	ret = Ln_pay_pln_rol_Sel( g_pub_tx.reply , &ln_pay_pln_rol_t , "tx_date=%ld and trace_no=%ld " , g_pub_tx.tx_date , trace_no );
	if( ret&ret!=100 )	
	{
		return 1;
	}else if( ret==100 ){
		return 0;
	}
	
	TRACE
	if( ln_pay_pln_rol_t.rol_ind[0]=='1' )
	{
		/* 只需对登记簿修改一次,若已经修改过了,则无需再修改 */
		return 0;
	}
	
	/* 修改放/还款计划表中数据 */	
	ret = Ln_pay_pln_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d and pay_ind in ( '0','1' ) " , ln_pay_pln_rol_t.ac_id,ln_pay_pln_rol_t.ac_seqn );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		return 1;
	}
	ret = Ln_pay_pln_Fet_Upd( &ln_pay_pln , g_pub_tx.reply );
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
		WRITEMSG
		return 1;
	}else if (ret == 100){
		return 0;
	}

	ln_pay_pln.ac_id = ln_pay_pln_rol_t.ac_id;
	ln_pay_pln.ac_seqn = ln_pay_pln_rol_t.ac_seqn;
	ln_pay_pln.de_ac_id = ln_pay_pln_rol_t.de_ac_id;
	ln_pay_pln.de_ac_seqn = ln_pay_pln_rol_t.de_ac_seqn;
	ln_pay_pln.ttl_amt = ln_pay_pln_rol_t.ttl_amt;
	ln_pay_pln.ttl_cnt = ln_pay_pln_rol_t.ttl_cnt;
	ln_pay_pln.curr_cnt = ln_pay_pln_rol_t.curr_cnt;
	ln_pay_pln.curr_amt = ln_pay_pln_rol_t.curr_amt;
	ln_pay_pln.curr_intst = ln_pay_pln_rol_t.curr_intst;
	ln_pay_pln.wrk_date = ln_pay_pln_rol_t.wrk_date;
	ln_pay_pln.beg_date = ln_pay_pln_rol_t.beg_date;
	ln_pay_pln.end_date = ln_pay_pln_rol_t.end_date;
	strcpy( ln_pay_pln.pay_ind , ln_pay_pln_rol_t.pay_ind);
	
	ret = Ln_pay_pln_Upd_Upd( ln_pay_pln , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"更新按揭还款计划表错误!!");
               WRITEMSG
               return 1;
        }          
	Ln_pay_pln_Clo_Upd();
	
	/* 修改 按揭还款计划表撤销表中数据 */
	ret=sql_execute("update ln_pay_pln_rol set rol_ind='1'  \
		where tx_date=%ld and trace_no=%ld ", 
		g_pub_tx.tx_date , trace_no );
	if( ret ) return 1;		
	return 0;
}



/**********************************************************************
* 函 数 名：  Reback_ln_lo
* 函数功能：  回滚按揭欠款表(根据 按揭欠款撤销登记簿回滚)
* 作    者：  rob
* 完成时间：  2003年08月11日
*
* 参    数：
*     输  入：   ln_lo_rol		 按揭贷款欠息撤销表结构
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/		
int Reback_ln_lo( long trace_no )
{
	int ret=0;
	
	struct ln_lo_rol_c	ln_lo_rol_t;
	struct ln_lo_c	ln_lo;
	
	memset( &ln_lo_rol_t , 0x00 , sizeof( struct ln_lo_rol_c ) );
	memset( &ln_lo , 0x00 , sizeof( struct ln_lo_c ) );	

	TRACE
	
	sprintf( acErrMsg , "流水trace_no=%ld" , trace_no );
	WRITEMSG
	
	TRACE	
	
	/* 取出 按揭欠款表撤销表中数据 */
	ret = Ln_lo_rol_Sel( g_pub_tx.reply , &ln_lo_rol_t , "tx_date=%ld and trace_no=%ld and rol_ind='0'" , g_pub_tx.tx_date , trace_no );
	if( ret&&ret!=100 )
	{
			return 1;
	}else if(ret ==100 ){
		  return 0;
	}
			
	if( ln_lo_rol_t.rol_ind[0]=='1' )
	{
		/* 只需对登记簿修改一次,若已经修改过了,则无需再修改 */
		return 0;
	}

	
	/* 修改放/还款计划表中数据 */	
	ret = Ln_lo_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d and lo_term=%d " , ln_lo_rol_t.ac_id,ln_lo_rol_t.ac_seqn,ln_lo_rol_t.lo_term );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		return 1;
	}
	ret = Ln_lo_Fet_Upd( &ln_lo , g_pub_tx.reply );
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
		WRITEMSG
		return 1;
	}else if (ret == 100){
		return 0;
	}
	
	ln_lo.ac_id = ln_lo_rol_t.ac_id;
	ln_lo.ac_seqn = ln_lo_rol_t.ac_seqn;
	ln_lo.lo_term = ln_lo_rol_t.lo_term;
	ln_lo.lo_amt = ln_lo_rol_t.lo_amt;
	ln_lo.lo_intst = ln_lo_rol_t.lo_intst;
	ln_lo.pun_intst = ln_lo_rol_t.pun_intst;
	strcpy( ln_lo.pay_type , ln_lo_rol_t.pay_type );
	ln_lo.over_date = ln_lo_rol_t.over_date;
	ln_lo.shld_pay_date = ln_lo_rol_t.shld_pay_date;
	ln_lo.pay_lo_amt = ln_lo_rol_t.pay_lo_amt;
	ln_lo.pay_lo_intst = ln_lo_rol_t.pay_lo_intst;
	ln_lo.pay_pun_intst = ln_lo_rol_t.pay_pun_intst;
	strcpy( ln_lo.mac , ln_lo_rol_t.mac );
	strcpy( ln_lo.old_ac_ind , ln_lo_rol_t.old_ac_ind );

	ret = Ln_lo_Upd_Upd( ln_lo , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"更新按揭欠款表错误!!");
               WRITEMSG
               return 1;
        }          
	Ln_lo_Clo_Upd();
	
	/* 修改 按揭欠款表撤销表中数据状态 */
	ret=sql_execute("update ln_lo_rol set rol_ind='1' \
		where tx_date=%ld and trace_no=%ld and lo_term=%d", 
		g_pub_tx.tx_date , trace_no , ln_lo_rol_t.lo_term );
	if( ret ) return 1;	
	return 0;
}


/**********************************************************************
* 函 数 名：  check_ln_mst_no
* 函数功能：  根据流水号，查询贷款明细中,是否有此交易后交易(即检查是否是最后一期)
* 作    者：  rob
* 完成时间：  2003年08月23日
*
* 参    数：
*     输  入：   ptrace_log		 流水结构
*      
*     输  出:    
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：	
*                   
********************************************************************/
int check_ln_mst_no( ptrace_log )
  struct trace_log_c *ptrace_log;
 {
	int ret=0;
	int flag=0;
	long max_trace_no=0;
	struct ln_mst_hst_c	S_ln_mst_hst;
	struct trace_log_c	LS_trace_log;
	memset( &S_ln_mst_hst, 0x00 , sizeof( struct ln_mst_hst_c ));
	
	TRACE
	
	sprintf( acErrMsg , "流水trace_no=%ld笔数trace_cnt=%d" , ptrace_log->trace_no , ptrace_log->trace_cnt);
	WRITEMSG
	
	TRACE
    sprintf(acErrMsg,"max_trace_no=[%ld]ptrace_log->trace_no=[%ld]!!",max_trace_no , ptrace_log->trace_no);
    WRITEMSG
    sprintf(acErrMsg,"ac_id=%ld and ac_seqn=%d and tx_date=%ld", 
							ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date);
    WRITEMSG	
	/* 取当然明细中属于该账户的最后一笔流水号 *
	ret = sql_max_long( "ln_mst_hst", "trace_no", &max_trace_no, 
				"ac_id=%ld and ac_seqn=%d and tx_date=%ld and tx_code!='5701'",
				ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date);
	if( ret )	return 1;
	TRACE

	if( max_trace_no > ptrace_log->trace_no )
	{
               sprintf(acErrMsg,"尚有未撤销交易!!");
               WRITEMSG
               sprintf(acErrMsg,"max_trace_no=[%ld]ptrace_log->trace_no=[%ld]!!"
											,max_trace_no , ptrace_log->trace_no);
               WRITEMSG
               sprintf(acErrMsg,"ac_id=%ld and ac_seqn=%d and tx_date=%ld",
										ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date);
               WRITEMSG
               strcpy(  g_pub_tx.reply ,"L231" );
               return 1;
	}
	******/
	
	ret = Ln_mst_hst_Dec_Sel( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and tx_date=%ld and trace_no>%ld and tx_code!='5701'",
				ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date,ptrace_log->trace_no);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_mst_hst_Dec_Sel错!ret=[%d]", ret);
		WRITEMSG		
		return 1;	
	}			
	
	while(1)
	{
		ret = Ln_mst_hst_Fet_Sel( &S_ln_mst_hst, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_mst_hst_Fet_Sel error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}else if( ret==100 ){
			break;
		}else if( ret==0 ){
			flag++;
			ret = Trace_log_Sel_rol( g_pub_tx.reply, &LS_trace_log, "trace_no=%ld and tx_date=%ld", S_ln_mst_hst.trace_no,g_pub_tx.tx_date );
			if(ret) return 1;
			if( LS_trace_log.sts[0]=='1' )
			{
				continue;
			}else{
				sprintf(acErrMsg,"尚有未撤销交易!!");
				WRITEMSG
				sprintf(acErrMsg,"NOW_trace_no=[%ld]ptrace_log->trace_no=[%ld]!!"
									,S_ln_mst_hst.trace_no , ptrace_log->trace_no);
				WRITEMSG
				sprintf(acErrMsg,"ac_id=%ld and ac_seqn=%d and tx_date=%ld",
								ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date);
				WRITEMSG
				strcpy(  g_pub_tx.reply ,"L231" );
				return 1;
			}
		}	
	}
	Ln_mst_hst_Clo_Sel();
	sprintf( acErrMsg, "进入次数【%d】", flag);
	WRITEMSG	
	return 0;
 } 
	
