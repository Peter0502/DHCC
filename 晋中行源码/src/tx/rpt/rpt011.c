/*************************************************************
* 文 件 名: rpt011.c
* 功能描述：对公交易流水清单
*
* 作    者: lance
* 完成日期: 2003年07月13日
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
#include "trace_log_bk_c.h"
#include "com_branch_c.h"

struct trace_log_bk_c	s_trace_log_bk;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_dglsqd(int bli,int bll,char str[100]);

rpt011()
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
	memset( &s_trace_log_bk , 0x00 , sizeof(struct trace_log_bk_c));
	
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
	
	strcpy( filename , "BRdglsqd" );	
	strcpy(vpfm, "dglsqd" );
	
	/* 条件: 交易机构=开户机构 */
	strcpy( tmpstr," tx_br_no=opn_br_no and ");
	strcat( wherelist,tmpstr );
		
	/* 条件: 对公账户*/
	strcpy( tmpstr," ac_no[1]='5' and ");
	strcat( wherelist,tmpstr );


	/**组成查询**/	
	strcat(wherelist," no_show!='1' and 1=1 ORDER BY tx_br_no,opn_br_no,sts,trace_no,trace_cnt");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Trace_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Trace_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
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
		
		/* 机构变换时 */
		pub_base_strpack(s_trace_log_bk.tx_br_no );
		if( strcmp( s_trace_log_bk.tx_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_trace_log_bk.tx_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile(s_trace_log_bk.tx_br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_trace_log_bk.tx_br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
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
			TRACE
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
			TRACE
		}
		ttlnum++;
	}		
	Trace_log_bk_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成对公流水交易清单成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成对公交易流水清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_dglsqd);
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
	TRACE
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	TRACE
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
int get_ratio_dglsqd( bli,bll,str )
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
			sprintf( t_str , "%4d年%2d月%2d日" , s_trace_log_bk.tx_date/10000 , s_trace_log_bk.tx_date%10000/100 , s_trace_log_bk.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_trace_log_bk.tx_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_trace_log_bk.tx_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_trace_log_bk.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 流水号 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_trace_log_bk.trace_no); 
			break; 
		case 'F': /* 记账员 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.tel);
			break;
		case 'G': /* 复核员 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.chk);
			break;
		case 'H': /* 交易码 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.tx_code);
			break;
		case 'I': /* 账      号 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.ac_no);
			break;
		case 'J': /* 账户序号 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_trace_log_bk.ac_seqn);
			break;
		case 'K': /* 交  易  额 */
			/* 加负号 */
			if( s_trace_log_bk.add_ind[0]=='0' )
			{
				/* 加逗号 */
				strcpy( tmp_inf , "-" );			
				sprintf( amt_tmp, "%.2lf" , s_trace_log_bk.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				/* 加负号 */
				strcat( tmp_inf , amt_tmp); 							
				sprintf(fmt,"%%%ds",bll); 		
				sprintf( str, fmt, tmp_inf ); 

			}else{							
				/* 加逗号 */
				sprintf( amt_tmp, "%.2lf" , s_trace_log_bk.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				
				sprintf(fmt,"%%%ds",bll); 		
				sprintf( str, fmt, amt_tmp ); 
			}
			break;

		case 'L': /* 产品 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_trace_log_bk.prdt_no);
			break;
		case 'M': /* 现转 */
			pub_base_dic_show_str(tmp_inf,"rpt_ct_ind",s_trace_log_bk.ct_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
		case 'N': /* 代理行 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_trace_log_bk.opn_br_no);
			break;
		case 'O': /* 状态 */
			pub_base_dic_show_str(tmp_inf , "rpt_log_sts" , s_trace_log_bk.sts);
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt , tmp_inf);
			break;
		case 'P': /* 交易日期 */ 
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_trace_log_bk.tx_date);
			break;
		case 'Q': /* 交易时间 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_trace_log_bk.tx_time);
			break;
		case 'R': /* 流水笔次 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_trace_log_bk.trace_cnt);
			break;		
		case 'S': /* 摘要 */
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt, s_trace_log_bk.brf);
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
