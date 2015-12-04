/*************************************************************
* 文 件 名: rpt996.c
* 功能描述：对公存款计息清单打印
*
* 作    者: rob
* 完成日期: 2003年08月15日
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
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "rpt_note_inf_c.h"
#include "com_item_c.h"
#include "com_sys_parm_c.h"
#include "intst_hst_c.h"

#define EXTERN

static	char    g_brno[10];         	 

struct intst_hst_c	s_intst_hst;
int page=0;
int line=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_dglsqd(int bli,int bll,char str[100]);

rpt996()
{
	int    ret;
	int    ttlnum;
	char   flag ;
	char   old_br_no[8];
	char   filename[100];

	struct com_sys_parm_c s_com_sys_parm;

	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
         
	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	strcpy( old_br_no , "*****" );

	/* 初始化全局变量 */
	pub_base_sysinit();
    
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;

	ret = Trace_log_Dec_Sel( g_pub_tx.reply, "1=1 ORDER BY tx_br_no,opn_br_no,sts,trace_no,trace_cnt" );
	ERR_DEAL

	while(1)
	{
		ret = Trace_log_Fet_Sel( &s_intst_hst,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
			}
			else 
			{
				sprintf( acErrMsg, "本日没有要打印的流水!" );
				WRITEMSG
			}
			break;
		}
		else 
			ERR_DEAL

		/* 机构变换时 */
		pub_base_strpack(s_intst_hst.tx_br_no );
		if( strcmp( s_intst_hst.tx_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
			}
			
			strcpy( old_br_no , s_intst_hst.tx_br_no );
			
			page=1;
			line=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile(s_intst_hst.tx_br_no , filename , 0 );
			ERR_DEAL

			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_intst_hst.tx_br_no , filename );
			ERR_DEAL

			/* 打印页首 */
			ret = print_head( );
			ERR_DEAL
		}

    	/* 打印信息 */
		ret = print_body();
		ERR_DEAL

		line++;

		if( line>50 )	/* 大于50行换页 */
		{
			line=0;
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

	Trace_log_Clo_Sel( );

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终流水清单处理程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终流水清单处理程序失败!!!!![%s]",g_pub_tx.reply);
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
	fclose(fp);
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
/* 说明：bli 代表变量字符名称 ... bll代表变量占的长度 ... str代表变量值 */
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
	char br_name[71];
	int i=0; 

	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , s_intst_hst.tx_date/10000 , s_intst_hst.tx_date%10000/100 , s_intst_hst.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_intst_hst.tx_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = pub_base_getbrname( s_intst_hst.tx_br_no, br_name );
			if( ret )
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_intst_hst.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 流水号 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_intst_hst.trace_no); 
			break; 
		case 'F': /* 记账员 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_intst_hst.tel);
			break;
		case 'G': /* 复核员,授权人 */
			if( strlen(s_intst_hst.chk) )
			{
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt , s_intst_hst.chk );
			}
			else if( strlen(s_intst_hst.aut) )
			{
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt , s_intst_hst.aut );
			}
			else 
				strcpy( str,"" );
			break;
		case 'H': /* 交易码 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_intst_hst.tx_code);
			break;
		case 'I': /* 账      号 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_intst_hst.ac_no);
			break;
		case 'J': /* 账户序号 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_intst_hst.ac_seqn);
			break;
		case 'K': /* 交  易  额 */
			/* 加负号 */
			if( s_intst_hst.add_ind[0]=='0' )
			{
				/* 加逗号 */
				strcpy( tmp_inf , "-" );			
				sprintf( amt_tmp, "%.2lf" , s_intst_hst.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				/* 加负号 */
				strcat( tmp_inf , amt_tmp); 							
				sprintf(fmt,"%%%ds",bll); 		
				sprintf( str, fmt, tmp_inf ); 
			}
			else
			{							
				/* 加逗号 */
				sprintf( amt_tmp, "%.2lf" , s_intst_hst.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				
				sprintf(fmt,"%%%ds",bll); 		
				sprintf( str, fmt, amt_tmp ); 
			}
			break;
		case 'L': /* 产品 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_intst_hst.prdt_no);
			break;
		case 'M': /* 现转 */
			pub_base_dic_show_str(tmp_inf,"rpt_ct_ind",s_intst_hst.ct_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
		case 'N': /* 代理行 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_intst_hst.opn_br_no);
			break;
		case 'O': /* 状态 */
			pub_base_dic_show_str(tmp_inf , "rpt_log_sts" , s_intst_hst.sts);
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt , tmp_inf);
			break;
		case 'P': /* 交易日期 */ 
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_intst_hst.tx_date);
			break;
		case 'Q': /* 交易时间 */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_intst_hst.tx_time);
			break;
		case 'R': /* 流水笔次 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_intst_hst.trace_cnt);
			break;		
		case 'S': /* 摘要 */
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt, s_intst_hst.brf);
			break;
		case 'T': /* 借贷 */
			pub_base_dic_show_str(tmp_inf,"accdc_ind",s_intst_hst.add_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
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
