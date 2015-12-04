/*************************************************************
* 文 件 名: rpt027.c
* 功能描述：普通贷款日终扣款流水清单
*
* 作    者: lance
* 完成日期: 2003年09月07日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "ln_mst_hst_c.h"
#include "com_branch_c.h"

struct ln_mst_hst_c s_ln_mst_hst;
struct ln_mst_c s_ln_mst;
struct ln_parm_c s_ln_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
double bal_amt=0.00;
double in_amt=0.00;
double out_amt=0.00;
double cmpd_amt=0.00;
char pay_c_sts[7];
char clear_flg[2];

int get_ratio_lnkkqd(int bli,int bll,char str[100]);

rpt027()
{
	int flag=0;
	int ttlnum;
	long qry_date=0;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	long old_trace_no=0;
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_ln_mst_hst , 0x00 , sizeof(struct ln_mst_hst_c));
	
	strcpy( old_br_no , "*****" );
	/*计算交易日期*/
	pub_base_deadlineD( g_pub_tx.tx_date, -1 ,&qry_date );	
   
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
/*	
	strcpy( filename , "BRlnkkqd" );	
	strcpy(vpfm, "lnkkqd" );
*/	
	strcpy( filename , "RPT027");
	strcpy(vpfm, "RPT027");
	/* 条件:交易代码为"G088" */
	sprintf( tmpstr," tx_code='G088'and tx_date=%ld and add_ind='0' and " , qry_date );
	strcat( wherelist,tmpstr );	
	/**组成查询**/

	strcat(wherelist," 1=1 ORDER BY opn_br_no, trace_no, trace_cnt,ac_id ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_mst_hst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Ln_mst_hst_Fet_Sel( &s_ln_mst_hst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取贷款明细表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}	
		/*取贷款户信息*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_mst_hst.ac_id,s_ln_mst_hst.ac_seqn ); /*读取贷款户信息*/
		if( ret ) 
		{
			sprintf(acErrMsg,"读取贷款户信息error");
			WRITEMSG
			return ret;
		}		
		ret = Ln_parm_Sel( g_pub_tx.reply , &s_ln_parm,"prdt_no='%s' ",s_ln_mst.prdt_no ); /*读取贷款户信息*/
		if( ret ) 
		{
			sprintf(acErrMsg,"读取贷款户信息error");
			WRITEMSG
			return ret;
		}
		if( s_ln_mst_hst.trace_no==old_trace_no )	/*流水号未改变*/
		{
			bal_amt=0.00;
			in_amt=0.00;
			out_amt=0.00;
			cmpd_amt=0.00;
			memset( &s_ln_mst , 0x00 , sizeof(struct ln_mst_c));
			memset( &s_ln_parm , 0x00 , sizeof(struct ln_parm_c));
			continue;
		}
		
		old_trace_no= s_ln_mst_hst.trace_no;
					
		/* 机构变换时 */
		pub_base_strpack( s_ln_mst_hst.tx_br_no );
		if( strcmp( s_ln_mst_hst.tx_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_mst_hst.tx_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_ln_mst_hst.tx_br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_ln_mst_hst.tx_br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
    		/* 统计信息 */
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &bal_amt , "ac_id=%ld and ac_seqn=%d and tx_date=%ld and tx_code='G088' and ln_tx_type='0' and add_ind='0' " , 
					s_ln_mst_hst.ac_id, s_ln_mst_hst.ac_seqn, qry_date );
		ERR_DEAL 
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &in_amt , "ac_id=%ld and ac_seqn=%d and tx_date=%ld and tx_code='G088' and ln_tx_type='1' and add_ind='0' " , 
					s_ln_mst_hst.ac_id, s_ln_mst_hst.ac_seqn, qry_date );
		ERR_DEAL
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &out_amt , "ac_id=%ld and ac_seqn=%d and tx_date=%ld and tx_code='G088' and ln_tx_type='2' and add_ind='0' " , 
					s_ln_mst_hst.ac_id, s_ln_mst_hst.ac_seqn, qry_date );
		ERR_DEAL 
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &cmpd_amt , "ac_id=%ld and ac_seqn=%d and tx_date=%ld and tx_code='G088' and ln_tx_type='3' and add_ind='0' " , 
					s_ln_mst_hst.ac_id, s_ln_mst_hst.ac_seqn, qry_date );
		ERR_DEAL
		 
		ret= pub_ln_chk_clear(s_ln_mst, s_ln_parm,  clear_flg );
		ERR_DEAL
		if( clear_flg[0]=='0' )
		{
			strcpy( pay_c_sts,"还  清");
		}else{
			strcpy( pay_c_sts,"未还清");
		}					  
    		/* 打印信息 */		
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>50 )	/* 大于50行换页 */
		{
			line_file=0;
			page++;
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Trace_log_bk_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成贷款到期扣款清单成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成贷款到期扣款清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnkkqd);
	vtcp_log("[%s][%d] ret =[%d]",__FILE__,__LINE__,ret);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表首 */
int print_head( )
{
	char vrec[3];
	
	if( page != 1 )
		{
			fprintf ( fp , "\f" );
		}
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表尾 */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表体 */
int print_body( )
{
	char vrec[3];

	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 赋值函数 */
int get_ratio_lnkkqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char mtr_date[13];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , s_ln_mst_hst.tx_date/10000 , s_ln_mst_hst.tx_date%10000/100 , s_ln_mst_hst.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_ln_mst_hst.tx_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_ln_mst_hst.tx_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_ln_mst_hst.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 流水号 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_mst_hst.trace_no); 
			break; 
		case 'F': /* 贷款账号 */
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d",s_ln_mst_hst.ac_id,s_ln_mst_hst.ac_seqn);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'G': /* 偿还本金 */						
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , bal_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* 偿还表内 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , in_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'I': /* 贷款户名 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_ln_mst.name);

			break;
		case 'J': /* 产品 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_mst.prdt_no);
			break;
		case 'K': /* 偿还表外 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , out_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L': /* 偿还复利 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'M': /* 总计 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , bal_amt+in_amt+out_amt+cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;	
		case 'O': /* 状态 */
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt, clear_flg);
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
