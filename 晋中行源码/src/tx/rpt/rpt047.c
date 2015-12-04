/*************************************************************
* 文 件 名: rpt047.c
* 功能描述：按揭贷款欠款户清单
*
* 作    者: lance
* 完成日期: 2003年09月21日
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
#include "ln_lo_c.h"
#include "ln_pay_pln_tmp_c.h"
#include "ln_parm_c.h"                  /*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/
#include "dc_acc_rel_c.h"								/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/
#include "com_item_c.h"									/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/

struct com_sys_parm_c	s_com_sys_parm;
struct ln_mst_c	s_ln_mst;
struct ln_lo_c	s_ln_lo;
struct ln_pay_pln_tmp_c	s_ln_pay_pln_tmp;
struct ln_parm_c s_ln_parm;							/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/
struct dc_acc_rel_c s_dc_acc_rel;				/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/
struct com_item_c s_com_item;						/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
double lo_bj=0.00;
double lo_lx=0.00;
double tol_bj=0.00;
double tol_lx=0.00;
double pay_bj=0.00;
double pay_lx=0.00;

double s_bj=0.00;
double s_lx=0.00;
double tol_s_bj=0.00;
double tol_s_lx=0.00;

long   tol_jx_cnt=0;
double tol_out_amt=0.00;
double tol_cmpd_amt=0.00;

int get_ratio_lnajloqd(int bli,int bll,char str[100]);

rpt047()
{
	int flag=0;
	int ttlnum;
	long tmp_date;
	long old_ac_id=0;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	
	tol_jx_cnt=0;	
	tol_out_amt=0.00;	
	tol_cmpd_amt=0.00;
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &s_ln_mst , 0x00 , sizeof(struct ln_mst_c));
	memset( &s_ln_lo , 0x00 , sizeof(struct ln_lo_c));
				
	strcpy( old_br_no , "*****" );
		
   
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	
	strcpy( filename , "RPT047" );	
	strcpy(vpfm, "RPT047" );
		
	/**组成查询**/	
	strcpy( wherelist," repay_type in ('3','4') ORDER BY opn_br_no,prdt_no,opn_date,ac_id,ac_seqn");
	
	ret= Ln_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
	
	ttlnum=0;	
	while(1)
	{
		ret= Ln_mst_Fet_Sel( &s_ln_mst,g_pub_tx.reply);
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
			sprintf(acErrMsg,"读取贷款主文件信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		ret= Ln_lo_Sel( g_pub_tx.reply,&s_ln_lo,"ac_id=%ld and ac_seqn=%d and pay_type='0' ",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if( ret&&ret!=100 )
		{
			sprintf(acErrMsg,"读取按揭欠款表信息异常出错!");
			WRITEMSG			
			goto ErrExit;
		}else if( ret==100 )
		{
			continue;
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
		/* 统计信息 */	
		ret= sql_sum_double( "ln_lo","lo_amt",&lo_bj,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if(ret)	return 1; 
		ret= sql_sum_double( "ln_lo","lo_intst",&lo_lx,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if(ret)	return 1;
		ret= sql_sum_double( "ln_lo","pay_lo_amt",&pay_bj,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if(ret)	return 1;
		ret= sql_sum_double( "ln_lo","pay_lo_intst",&pay_lx,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if(ret)	return 1;
		
		
		pay_bj=	lo_bj-pay_bj;
		pay_lx= lo_lx-pay_lx;
		
    		/* 数据统计 */
    		tol_jx_cnt++;
    		tol_out_amt+=pay_bj;
    		tol_cmpd_amt+=pay_lx;
    																	
    		ret= Ln_pay_pln_tmp_Sel( g_pub_tx.reply,&s_ln_pay_pln_tmp,"ac_id=%ld and ac_seqn=%d ",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if( ret&&ret!=100 )
		{
			sprintf(acErrMsg,"读取按揭欠款表信息异常出错!");
			WRITEMSG			
			goto ErrExit;
		}else if( ret==100 )
		{
			s_bj=0.00;
			s_lx=0.00;
		}
    		s_bj=s_ln_pay_pln_tmp.curr_amt;
    		s_lx=s_ln_pay_pln_tmp.curr_intst;
    		tol_s_bj+=s_bj;
    		tol_s_lx+=s_lx;
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
	sprintf(acErrMsg,"生成按揭贷款欠款户清单成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成按揭贷款欠款户清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnajloqd);
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
int get_ratio_lnajloqd( bli,bll,str )
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
			sprintf( t_str , "%4d年%2d月%2d日" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
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
		case 'H': /* 欠本金金额 */			
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , pay_bj ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* 欠利息金额 */			
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , pay_lx ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;		
		case 'G': /* 本期本金 */			
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_bj ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K': /* 本期利息 */			
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_lx ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;	
		case 'Q': /* 总贷款金额 */
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
		/**产品和产品名称注释掉增加科目号。zhangpeng 20131125 start**
		case 'J': //产品 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_mst.prdt_no);
			break;
		case 'M': //产品名称			
			ret= Ln_parm_Sel( g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",s_ln_mst.prdt_no);
			if(ret)	return 1;
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, g_ln_parm.title ); 
			break;
		*产品和产品名称注释掉增加科目号。zhangpeng 20131125 end***/
		
		/**增加科目号。zhangpeng 20131125 start***/
		case 'J': /*科目号*/			
			ret=Ln_parm_Sel( g_pub_tx.reply ,&s_ln_parm,"prdt_no='%s'",s_ln_mst.prdt_no);
			if( ret )
			{
				return 1;
			}
			ret=Dc_acc_rel_Sel( g_pub_tx.reply ,&s_dc_acc_rel,"dc_code='%s'",s_ln_parm.dc_code);
			if( ret )
			{
				return 1;
			}
			ret=Com_item_Sel( g_pub_tx.reply ,&s_com_item,"acc_hrt='%s'",s_dc_acc_rel.acc_hrt);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, s_com_item.acc_no ); 
			memset( &s_ln_parm, 0x00 , sizeof( struct ln_parm_c ));
			memset( &s_dc_acc_rel, 0x00 , sizeof( struct dc_acc_rel_c ));
			memset( &s_com_item, 0x00 , sizeof( struct com_item_c ));
			break;
		/**增加科目号。zhangpeng 20131125 end***/
		
		case 'X': /* 开户日期 */
			sprintf( t_str , "%4d-%02d-%02d" , s_ln_mst.opn_date/10000 , s_ln_mst.opn_date%10000/100 , s_ln_mst.opn_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'Y': /* 到期日期 */
			sprintf( t_str , "%4d-%02d-%02d" , s_ln_mst.mtr_date/10000 , s_ln_mst.mtr_date%10000/100 , s_ln_mst.mtr_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'R': /* 户数总计 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, tol_jx_cnt); 
			break; 
		case 'U': /* 欠本总计 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , tol_out_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'V': /* 欠息金额总计 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , tol_cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'W': /* 本期本金总计 */			
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , tol_s_bj ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Z': /* 本期利息总计 */			
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , tol_s_lx ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
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
