/*************************************************
* 文 件 名: spL401.c
* 功能描述：   
*              管理交易－贷款利率调整
*
* 作    者:    lance
* 完成日期：   2003年04月01日
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
#include "com_rate_c.h"
	
char 	rate_no[4],cur_no[3];
double 	rate,rate_parm;
static  struct ln_expand_c 	p_ln_expand;	
static  struct com_rate_c 	t_com_rate;
		
spL401()  
{ 		
	int ret=0 ,min_ret=0 ,max_ret=0;
	int rate_flag;
	char acm_type[2],ln_flag[2];
			
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
	/* 更新公共利率表 */
	ret = Com_rate_Dec_Upd( g_pub_tx.reply , "cur_no='%s' and\
						rate_code='%s' and end_date=99999999",cur_no,rate_no);
    if ( ret==100 )
	{
		sprintf( acErrMsg, "利率表中无符合条件的记录ret=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply , "L088" );
		goto ErrExit;
	}else if ( ret ){
	TRACE
		sprintf( acErrMsg, "执行Com_rate_Dec_Upd错!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
	ret = Com_rate_Fet_Upd( &t_com_rate , g_pub_tx.reply );
	TRACE
	if( ret )
	{
		sprintf( acErrMsg, "执行Com_rate_Fet_Upd错!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
	t_com_rate.end_date = g_pub_tx.tx_date;
	ret = Com_rate_Upd_Upd( t_com_rate , g_pub_tx.reply );
	if( ret )
	{
	TRACE
		sprintf( acErrMsg, "执行Com_rate_Upd_Upd错!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}		
	ret = Com_rate_Clo_Upd( );
	if( ret )
	{
	TRACE
		sprintf( acErrMsg, "执行Com_rate_Fet_Upd错!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
	/* 添加修改的记录 */
	strcpy( t_com_rate.cur_no , cur_no );
	t_com_rate.beg_date = g_pub_tx.tx_date;
	t_com_rate.end_date = 99999999;
	t_com_rate.rate_val = rate;
	ret = Com_rate_Ins( t_com_rate , g_pub_tx.reply );
	if( ret )
	{
	TRACE
		sprintf( acErrMsg, "公共利率表添加记录失败!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
	
	/* 查询贷款参数表中满足条件的记录(币种,利率编号,挂牌利率标志 */
	ret = Ln_parm_Dec_Sel( g_pub_tx.reply , "get_rate_type='1' and cur_no='%s' and ( rate_no='%s' or over_rate_no='%s' )" ,cur_no , rate_no , rate_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_parm_Dec_Upd错!ret=[%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{		
		ret = Ln_parm_Fet_Sel( &g_ln_parm,g_pub_tx.reply );   
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "贷款参数表中无记录" );
			WRITEMSG
			break;
		}else if ( ret ){
			sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}
		if( g_pub_tx.tx_date < g_ln_parm.beg_date||g_pub_tx.tx_date > g_ln_parm.end_date )
		
		if( !strcmp( rate_no , g_ln_parm.rate_no ) )
		{
			rate_flag=1;/* 正常利率 */
		}else{
			rate_flag=2;/* 逾期利率 */
		}	
		/* 按产品参数取贷款主文件中数据 */
		ret = Ln_mst_Dec_Sel( g_pub_tx.reply , "get_rate_type='1' and cur_no='%s' and ( rate_no='%s' or over_rate_no='%s' )" ,cur_no , rate_no , rate_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Ln_parm_Dec_Upd错!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			ret = Ln_mst_Fet_Upd( &g_ln_mst , g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg, "执行Ln_mst_Fet_Upd错!ret=[%d]",ret );
				WRITEMSG
				goto ErrExit;
			}
			/* 取贷款状态 */
			ret = pub_ln_check_acsts(ln_flag);
			/* 按揭贷款 */	
			if ( g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4' )
			{
				/* 修改逾期利率 */
				if( rate_flag==2 )
				{
					g_ln_mst.over_rate = rate ;
				}
				/* 修改正常利率 */
				if( rate_flag==1 )
				{
					g_ln_mst.rate = rate ;
					/* 重新生成还款计划 */
					ret = pub_ln_Chg_PayPlan( g_ln_mst , g_ln_parm , g_ln_mst.rate , g_ln_mst.bal , g_pub_tx.tx_date , g_ln_mst.mtr_date );
					if( ret==1) break;
					if( ret==-1 ) goto ErrExit;										
				}
			}else{		
			/* 普通贷款 */
				/* 修改正常利率 */
				if( rate_flag==1)
				{
					if(ln_flag[0]=='1')/* 状态为正常 */
					{
						g_ln_mst.rate = rate;						
						/* 登记分段积数表 */
						strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
						ret = Ins_Acm( acm_type , g_ln_mst.rate );
						if (ret) goto ErrExit;
					}
					if(ln_flag[0]=='Z')/* 状态为展期 */
					{						
						g_ln_mst.rate = rate;						
					}
					if(ln_flag[0]=='2'||ln_flag[0]=='3'||ln_flag[0]=='4')/* 状态为逾期 */
					{						
						g_ln_mst.rate = rate;							
					}
					if(ln_flag[0]=='*'||ln_flag[0]=='9')/* 状态为其他 */
					{						
						break;						
					}				
				}
				/* 修改逾期利率 */					
				if( rate_flag==2)
				{
					if(ln_flag[0]=='1')/* 状态为正常 */
					{
						g_ln_mst.over_rate = rate;						
					}
					if(ln_flag[0]=='Z')/* 状态为展期 */
					{						
						g_ln_mst.over_rate = rate;						
					}
					if(ln_flag[0]=='2'||ln_flag[0]=='3'||ln_flag[0]=='4')/* 状态为逾期 */
					{						
						g_ln_mst.over_rate = rate;						
						/* 登记分段积数表 */
						strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
						ret = Ins_Acm( acm_type , g_ln_mst.over_rate );
						if (ret) goto ErrExit;												
					}
					if(ln_flag[0]=='*'||ln_flag[0]=='9')/* 状态为其他 */
					{						
						break;						
					}		
				}
			}
			/* 更新贷款主文件记录 */								
			ret = Ln_mst_Upd_Upd( g_ln_mst , g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg, "执行Ln_mst_Upd_Upd错!ret=[%d]",ret );
				WRITEMSG
				goto ErrExit;
			}
			
			/* Pub_tx结构赋值 */
			strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
			strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
			/* 是否打印存折(有折无折标志):1是0否 */
			strcpy ( g_pub_tx.prt_ind , "0" );	
			/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
			strcpy ( g_pub_tx.hst_ind , "1" );
			g_pub_tx.svc_ind = 3060;			
			strcpy ( g_pub_tx.brf,"贷款利率调整" );	
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
		ret = Ln_mst_Clo_Upd( );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Ln_mst_Clo_Sel错!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}	
	}
	ret = Ln_parm_Clo_Sel( );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_parm_Clo_Sel错!ret=[%d]",ret );
		WRITEMSG
		goto ErrExit;
	}						
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
	memset ( &t_com_rate ,0x00 ,sizeof(struct com_rate_c) );
	return 0;	
}

int data_set_struct()
{    				
	get_zd_data("0210",cur_no);			/* 币种 */
	sprintf(acErrMsg,"币种[%s]",cur_no);
	WRITEMSG
		
	get_zd_data("1161",rate_no);			/* 利率编号 */
	sprintf(acErrMsg,"利率编号[%s]",rate_no);
	WRITEMSG
	
	get_zd_double("0840",&rate);			/* 利率 */
	sprintf(acErrMsg,"利率[%lf]",rate);
	WRITEMSG	
						
	return 0;
}
/**********************************************************************
* 函 数 名：  Ins_Acm
* 函数功能：  计算贷款主文件中各个积数
*	      添加分段积数表
*	      将主文件中各个积数置0
*             修改起息日期
* 作    者：  lance
* 完成时间：  2003年04月02日
*
* 参    数： 
*     输  入：acm_type   char(2)   积数计算类型（1每日累计2变动累计3变动累计倒算）   
*             rate       double    利率(添加分段积数表)
*     输  出:           char(4)   响应码
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int Ins_Acm( char *acm_type , double rate )
{
	int ret=0;
	
	/* 统计贷款主文件中积数 */
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,g_ln_parm.intst_days_type,acm_type,g_ln_mst.bal,0,
						&g_ln_mst.intst_acm,"0",0,0 );
	if( ret )
	{
		sprintf(acErrMsg,"调用函数计算积数错误!");
		WRITEMSG
		return -1;	
	}				
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '2',
		  g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.intst_acm,
		  rate, g_pub_tx.reply);
	if (ret)
	{
		sprintf( acErrMsg,"调用函数登记分段积数表失败");
		WRITEMSG
		return -1;	
	}
	/* 若计算复利 */
	if ( g_ln_mst.cmpd_intst_ind[0]=='Y' )
	{
		/* 表内欠息积数 */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,g_ln_parm.intst_days_type,acm_type,g_ln_mst.in_lo_intst,0,
					&g_ln_mst.in_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return -1;	
		}	
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '3',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.in_lo_acm,
			  rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"调用函数登记分段积数表失败");
			WRITEMSG
			return -1;	
		}
		/* 表外欠息积数 */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,g_ln_parm.intst_days_type,acm_type,g_ln_mst.out_lo_intst,0,
					&g_ln_mst.out_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return -1;	
		}				
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '4',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.out_lo_acm,
			  rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"调用函数登记分段积数表失败");
			WRITEMSG
			return -1;	
		}	
		/* 复利积数 */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,g_ln_parm.intst_days_type,acm_type,g_ln_mst.cmpd_lo_intst,0,
					&g_ln_mst.cmpd_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数计算积数错误!");
			WRITEMSG
			return -1;	
		}		
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '5',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.cmpd_lo_acm,
			  rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"调用函数登记分段积数表失败");
			WRITEMSG
			return -1;	
		}		
	}
	
	/* 主文件中积数清0 */	
	g_ln_mst.intst_acm = 0;			/* 利息积数 */		
	g_ln_mst.in_lo_acm = 0;			/* 表内欠息积数 */
	g_ln_mst.out_lo_acm= 0;			/* 表外欠息积数 */
	g_ln_mst.cmpd_lo_acm = 0;		/* 复利积数 */				
	g_ln_mst.lst_date = g_pub_tx.tx_date;	/* 上笔发生日期 */
	g_ln_mst.ic_date = g_pub_tx.tx_date;	/* 起息日期 */	
	
	return 0;
}	
