/*************************************************************
* 文 件 名: rpt101.c
* 功能描述：无本欠息贷款户清单
*
* 作    者: lance
* 完成日期: 2003年10月26日
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
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "mo_opn_cls_c.h"

struct com_sys_parm_c	s_com_sys_parm;
struct ln_mst_c	s_ln_mst;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
static long qry_date=0; /*add by martin 2009/4/17 9:27:43 */

long   tol_jx_cnt=0;
double tol_bal_amt=0.00;
double tol_in_amt=0.00;
double tol_out_amt=0.00;
double tol_cmpd_amt=0.00;

int get_ratio_lnwbqxqd(int bli,int bll,char str[100]);

rpt101()
{
	int flag=0;
	int ttlnum;
	long tmp_date;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	
	tol_jx_cnt=0;
	tol_bal_amt=0.00;
	tol_in_amt=0.00;	
	tol_out_amt=0.00;	
	tol_cmpd_amt=0.00;
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &s_ln_mst , 0x00 , sizeof(struct ln_mst_c));
			
	strcpy( old_br_no , "*****" );
	
	/*计算交易日期 add by martin 2009/4/17 9:28:58*/
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
	
	strcpy( filename , "RPT101" );	
	strcpy(vpfm, "RPT101" );
		
	/**组成查询**/	
	strcpy(wherelist," (in_lo_intst>0.00 or out_lo_intst>0.00 or cmpd_lo_intst>0.00) and repay_type not in ('3','4') and prdt_no not in ('3Z1','3Z2','3Z3' ) and bal=0.00 ORDER BY opn_br_no,prdt_no,ac_id");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Ln_mst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Ln_mst_Fet_Sel( &s_ln_mst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL				
				fclose(fp);
			}
			break;
		}else if( ret ){
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* 机构变换时 */
		pub_base_strpack(s_ln_mst.opn_br_no );
		if( strcmp( s_ln_mst.opn_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_mst.opn_br_no );
			
			page=1;
			line_file=0;
			tol_jx_cnt=0;
			tol_bal_amt=0.00;
			tol_in_amt=0.00;	
			tol_out_amt=0.00;	
			tol_cmpd_amt=0.00;
						
			/* 删除原文件 */
			ret = pub_rpt_rmfile(s_ln_mst.opn_br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_ln_mst.opn_br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
    		/* 数据统计 */
    		tol_jx_cnt++;
    		tol_bal_amt+=s_ln_mst.bal;
    		tol_in_amt+=s_ln_mst.in_lo_intst;
    		tol_out_amt+=s_ln_mst.out_lo_intst;
    		tol_cmpd_amt+=s_ln_mst.cmpd_lo_intst;
    		  
    		/* 打印信息 */
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>=50 )	/* 大于50行换页 */
		{
			line_file=0;
			page++;
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL
			/* 打换页 */
			ret = print_NY();
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Ln_mst_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成普通贷款欠款户清单成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成普通贷款欠款户清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnwbqxqd);
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
		fprintf ( fp ,"\f" );
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


/* 打印换页 */
int print_NY( )
{
	char vrec[3];

	strcpy( vrec,"EE" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* 打印总计 */
int print_zj( )
{
	char vrec[3];

	strcpy( vrec,"DD" );
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
int get_ratio_lnwbqxqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			/*sprintf( t_str , "%4d年%2d月%2d日" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );*/
			sprintf( t_str , "%4d年%2d月%2d日" , qry_date/10000 , qry_date%10000/100 , qry_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_ln_mst.opn_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_ln_mst.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_ln_mst.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 借据号 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, s_ln_mst.pact_no); 
			break; 
		case 'F': /* 贷款账号 */
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'H': /* 欠表内金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.in_lo_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* 欠表外金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.out_lo_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /* 欠复利金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.cmpd_lo_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;			
		case 'Q': /* 尚欠本金 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.bal ); 
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
		case 'M': /* 产品名称 */			
			ret= Ln_parm_Sel( g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",s_ln_mst.prdt_no);
			if(ret)	return 1;
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, g_ln_parm.title ); 
			break;
		case 'R': /* 户数总计 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, tol_jx_cnt); 
			break; 	
		case 'X': /* 表内息金额总计 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , tol_in_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Y': /* 表外息金额总计 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , tol_out_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Z': /* 复利息金额总计 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , tol_cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'G': /* 开户日期 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_mst.opn_date); 
			break; 
		case 'S': /* 开户日期 */
			sprintf(fmt,"%%%dld",bll); 
			if( s_ln_mst.exp_ind[0]!='0' )
			{
				sprintf(str, fmt, s_ln_mst.exp_mtr_date); 
			}else{
				sprintf(str, fmt, s_ln_mst.mtr_date); 
			}				
			break; 			
		case 'W': /* 展期标志 */
			sprintf(fmt,"%%-%ds",bll); 
			if( s_ln_mst.exp_ind[0]!='0' )
			{
				strcpy(str,"展"); 
			}else{
				strcpy(str,"  ");  
			}
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
