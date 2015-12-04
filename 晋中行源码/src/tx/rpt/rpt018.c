/*************************************************************
* 文 件 名: rpt018.c
* 功能描述：储蓄产品账务清单
*
* 作    者: lance
* 完成日期: 2003年07月15日
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
#include "gl_prdt_dyn_hst_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"

struct gl_prdt_dyn_hst_c	s_gl_prdt_dyn_hst;
struct com_sys_parm_c	s_com_sys_parm;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
char prdt_name[31];
int opncnt;
FILE  *fp; 
int ret = 0;

long	l_ac_cnt_tol=0;		/* 上日户数总计 */
double	lcd_bal_tol=0;		/* 上日余额总计 */
long	rcd_cnt_tol=0;		/* 增加笔数总计 */
double	rcd_amt_tol=0;		/* 增加金额总计 */
long	rdd_cnt_tol=0;		/* 减少笔数总计 */
double	rdd_amt_tol=0;		/* 减少金额总计 */
long	d_opn_cnt_tol=0;	/* 开户笔数总计 */
double	cr_bal_tol=0;		/* 本日余额总计 */
long	d_cls_cnt_tol=0;	/* 销户笔数总计 */
long	ac_cnt_tol=0;		/* 本日户数总计 */

int get_ratio_cxcpqd(int bli,int bll,char str[100]);

rpt018()
{
	int flag=0;
	int ttlnum;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_gl_prdt_dyn_hst , 0x00 , sizeof(struct gl_prdt_dyn_hst_c));
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
		
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
	
	strcpy( filename , "BRcxcpqd" );	
	strcpy(vpfm, "cxcpqd" );
	
	/* 条件: 交易日期为当前日期前一天 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "sys_date=%ld" , g_pub_tx.tx_date );
	ERR_DEAL
	sprintf( tmpstr," date=%ld and " , s_com_sys_parm.lst_date );
	strcat( wherelist,tmpstr );	

	/* 条件: 存款产品 */
	strcpy( tmpstr," prdt_cod[1]='1' or prdt_cod[1]='2' and ");
	strcat( wherelist,tmpstr );

	/**组成查询**/	
	strcat(wherelist," 1=1 ORDER BY br_no,prdt_cod");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Gl_prdt_dyn_hst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Gl_prdt_dyn_hst_Fet_Sel( &s_gl_prdt_dyn_hst, g_pub_tx.reply );
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
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* 判断是否是对私客户*/
		if( s_gl_prdt_dyn_hst.prdt_cod[0]=='1' )	/* 活期 */
		{
			ret = Dd_parm_Sel( g_pub_tx.reply , &g_dd_parm , "prdt_no='%s'" , s_gl_prdt_dyn_hst.prdt_cod );
			ERR_DEAL
			/* 若非对私或公用或内部员工 */
			if( g_dd_parm.cif_type[0]!='1' && g_dd_parm.cif_type[0]!='0' && g_dd_parm.cif_type[0]!='5' )
			{
				continue;
			}
			strcpy( prdt_name , g_dd_parm.title );
		}
				
		/* 机构变换时 */
		pub_base_strpack(s_gl_prdt_dyn_hst.br_no );
		if( strcmp( s_gl_prdt_dyn_hst.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_gl_prdt_dyn_hst.br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile(s_gl_prdt_dyn_hst.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_gl_prdt_dyn_hst.br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
			/* 总计数据清零 */
			l_ac_cnt_tol=0;		/* 上日户数总计 */
			lcd_bal_tol=0;		/* 上日余额总计 */
			rcd_cnt_tol=0;		/* 增加笔数总计 */
			rcd_amt_tol=0;		/* 增加金额总计 */
			rdd_cnt_tol=0;		/* 减少笔数总计 */
			rdd_amt_tol=0;		/* 减少金额总计 */
			d_opn_cnt_tol=0;	/* 开户笔数总计 */
			cr_bal_tol=0;		/* 本日余额总计 */
			d_cls_cnt_tol=0;	/* 销户笔数总计 */
			ac_cnt_tol=0;		/* 本日户数总计 */						
		}
		
    		/* 打印信息 */
		/* 统计信息 */
		l_ac_cnt_tol+=s_gl_prdt_dyn_hst.ac_cnt;		/* 上日户数总计 */
		lcd_bal_tol+=s_gl_prdt_dyn_hst.cr_bal;		/* 上日余额总计 */
		rcd_cnt_tol+=s_gl_prdt_dyn_hst.rcd_cnt;		/* 增加笔数总计 */
		rcd_amt_tol+=s_gl_prdt_dyn_hst.rcd_amt;		/* 增加金额总计 */
		rdd_cnt_tol+=s_gl_prdt_dyn_hst.rdd_cnt;		/* 减少笔数总计 */
		rdd_amt_tol+=s_gl_prdt_dyn_hst.rdd_amt;		/* 减少金额总计 */
		d_opn_cnt_tol+=s_gl_prdt_dyn_hst.d_opn_cnt;	/* 开户笔数总计 */
		cr_bal_tol+=s_gl_prdt_dyn_hst.cr_bal;		/* 本日余额总计 */
		d_cls_cnt_tol+=s_gl_prdt_dyn_hst.d_cls_cnt;	/* 销户笔数总计 */
		ac_cnt_tol+=s_gl_prdt_dyn_hst.ac_cnt;		/* 本日户数总计 */
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
	Gl_prdt_dyn_hst_Clo_Sel();
	
OkExit:
	
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成储蓄产品账务清单成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成储蓄产品账务清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_cxcpqd);
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
int get_ratio_cxcpqd( bli,bll,str )
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
			sprintf( t_str , "%4d年%2d月%2d日" , s_gl_prdt_dyn_hst.date/10000 , s_gl_prdt_dyn_hst.date%10000/100 , s_gl_prdt_dyn_hst.date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_gl_prdt_dyn_hst.br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_gl_prdt_dyn_hst.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				strcpy( sCom_branch.br_name," " );
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_gl_prdt_dyn_hst.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 		
		case 'E': /* 产品 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_gl_prdt_dyn_hst.prdt_cod); 
			break; 
		case 'F': /* 产品名称 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , prdt_name);
			break;
		case 'G': /* 上日户数 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.ac_cnt);
			break;		
		case 'H': /* 上日余额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn_hst.cr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;			
		case 'I': /* 增加笔数 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.rcd_cnt);
			break;
		case 'J': /* 增加金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn_hst.rcd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;					
		case 'K': /* 减少笔数 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.rdd_cnt);
			break;
		case 'L': /* 减少金额 */
			/* 加逗号 */
			strcpy( tmp_inf , "-" );			
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn_hst.rdd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			/* 加负号 */
			strcat( tmp_inf , amt_tmp); 							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, tmp_inf ); 
			break;
		case 'M': /* 开户笔数 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.d_opn_cnt);
			break;
		case 'N': /* 本日余额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn_hst.cr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;	
		case 'O': /* 销户笔数 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.d_cls_cnt);
			break;
		case 'P': /* 本日户数 */ 
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_gl_prdt_dyn_hst.ac_cnt);
			break;									
		case 'Q': /* 上日户数总计 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , l_ac_cnt_tol);
			break;
		case 'R': /* 上日余额总计 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , lcd_bal_tol ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;		
		case 'S': /* 增加笔数总计 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , rcd_cnt_tol);
			break;
		case 'T': /* 增加金额总计 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , rcd_amt_tol ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'U': /* 减少笔数总计 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , rdd_cnt_tol);
			break;
		case 'V': /* 减少金额总计 */
			/* 加逗号 */
			strcpy( tmp_inf , "-" );			
			sprintf( amt_tmp, "%.2lf" , rdd_amt_tol ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			/* 加负号 */
			strcat( tmp_inf , amt_tmp); 							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, tmp_inf ); 
			break;
		case 'W': /* 开户笔数总计 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , d_opn_cnt_tol);
			break;
		case 'X': /* 本日余额总计 */ 			
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , cr_bal_tol ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Y': /* 销户笔数总计 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , d_cls_cnt_tol);
			break;
		case 'Z': /* 本日户数总计 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , ac_cnt_tol);
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
