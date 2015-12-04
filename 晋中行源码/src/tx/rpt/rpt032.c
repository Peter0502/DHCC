/*************************************************************
* 文 件 名: rpt032.c
* 功能描述：特殊业务流水清单(汇票签发、注销客户凭证、注销柜员凭证、凭证作废、转存单交易)
*
* 作    者: lance
* 完成日期: 2003年09月15日
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
#include "menu_c.h"
#include "note_parm_c.h"
#include "note_mst_hst_c.h"
#include "note_cheq_hst_c.h"

static	struct trace_log_bk_c s_trace_log_bk;
static	struct menu_c	s_menu;
static	struct note_parm_c	s_note_parm;
static	struct note_mst_hst_c	s_note_mst_hst;
static	struct note_cheq_hst_c	s_note_cheq_hst;

static	char end_note_no[17];
static	long note_cnt=0;
static	int page=0;
static	int line=0;
static	int line_file=0;
static	char vpfm[21];
static	char t_str[41];
static	int opncnt;
static	FILE    *fp; 
static	int ret=0;

static	int get_ratio_tsjylsqd(int bli,int bll,char str[100]);

rpt032()
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
	
	char o_tx_code[5];/*原交易号 2009/12/17 16:23:51 add by martin */
	char o_note_type[4];/*原凭证号 2009/12/17 16:23:51 add by martin */
	
	
	memset( wherelist,0,sizeof(wherelist) );
	memset(&o_tx_code, 0, sizeof(o_tx_code) );
	memset(&o_note_type, 0, sizeof(o_note_type) );
	memset( &s_trace_log_bk , 0x00 , sizeof(struct trace_log_bk_c));
	
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
	strcpy( filename , "BRtsjylsqd" );	
	strcpy(vpfm, "tsjylsqd" );
*/	
	strcpy( filename , "RPT032");
	strcpy(vpfm, "RPT032");
	/* 条件:交易代码为2401,5106,5110,5111,4142 */
	
	sprintf( tmpstr," tx_code in('2401','5106','5110','5111','4142') and tx_date=%ld and trace_cnt=1 and " , qry_date );
	strcat( wherelist,tmpstr );	
	/**组成查询**/

	strcat(wherelist," 1=1 ORDER BY tx_br_no, tx_code, trace_no,tel ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Trace_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Trace_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
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
			sprintf(acErrMsg,"读取流水表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}	
					
		/* 机构变换时 */
		pub_base_strpack( s_trace_log_bk.tx_br_no );
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
			ret = pub_rpt_rmfile( s_trace_log_bk.tx_br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_trace_log_bk.tx_br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		if( strcmp( s_trace_log_bk.tx_code,"4142")==0 )
		{
			TRACE
			if( s_trace_log_bk.add_ind[0]=='0' )
			{
				continue;
			}
		}
		
		{/*modify by martin 2009/12/18 20:19:31 换个地方*/
			
			memset(&s_menu, 0x0, sizeof(struct menu_c));
			memset(&s_note_parm, 0x0, sizeof(struct note_parm_c));
			memset(&s_note_mst_hst, 0x0, sizeof(struct note_mst_hst_c));
			memset(&s_note_cheq_hst, 0x0, sizeof(struct note_cheq_hst_c));
					/*取交易名称*/
			if( strcmp( o_tx_code,s_trace_log_bk.tx_code) )
			{
				ret = Menu_Sel( g_pub_tx.reply , &s_menu,"code='%s' ",s_trace_log_bk.tx_code ); 
				if( ret ) 
				{
					sprintf(acErrMsg,"读取交易名称信息error");
					WRITEMSG
					return ret;
				}
				strcpy(o_tx_code,s_trace_log_bk.tx_code);
			}
			/*取介质名称*/
			if( strcmp( o_note_type,s_trace_log_bk.note_type) )
			{
				ret = Note_parm_Sel( g_pub_tx.reply , &s_note_parm,"note_type='%s' ",s_trace_log_bk.note_type ); 
				if( ret ) 
				{
					sprintf(acErrMsg,"读取介质名称信息error");
					WRITEMSG
					return ret;
				}
				strcpy(o_note_type,s_trace_log_bk.note_type);
			}
			if( strcmp( s_trace_log_bk.tx_code,"5111") ) 
			{
				ret= Note_mst_hst_Sel( g_pub_tx.reply,&s_note_mst_hst,"trace_no=%ld and tx_date=%ld",s_trace_log_bk.trace_no,s_trace_log_bk.tx_date);
				if(ret)	
				{
					sprintf(acErrMsg,"读凭证明细表出错ret=[%ld]!",ret);
					WRITEMSG					
					sprintf(acErrMsg,"trace_no=[%ld],tx_date=[%ld],note_no=[%s]!",s_trace_log_bk.trace_no,s_trace_log_bk.tx_date,s_trace_log_bk.note_no);
					WRITEMSG
					return ret;
				}
				strcpy( end_note_no ,s_note_mst_hst.end_note_no);
				note_cnt=s_note_mst_hst.note_cnt;
			}else{
				ret= Note_cheq_hst_Sel( g_pub_tx.reply,&s_note_cheq_hst,"trace_no=%ld and tx_date=%ld",s_trace_log_bk.trace_no,s_trace_log_bk.tx_date);
				if(ret)	
				{
					sprintf(acErrMsg,"读客户凭证明细表出错ret=[%ld]!",ret);
					WRITEMSG					
					sprintf(acErrMsg,"trace_no=[%ld],tx_date=[%ld],note_no=[%s]!",s_trace_log_bk.trace_no,s_trace_log_bk.tx_date,s_trace_log_bk.note_no);
					WRITEMSG
					return ret;
				}
				strcpy( end_note_no ,s_note_cheq_hst.end_note_no);
				note_cnt=s_note_cheq_hst.cnt;
			}	
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
	sprintf(acErrMsg,"生成特殊业务流水清单成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成特殊业务流水清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_tsjylsqd);
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
int get_ratio_tsjylsqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char amt_tmp[21]; 
	char fmt[10]; 
	double tx_amt=0.00;
	
	int i=0; 
	struct com_branch_c sCom_branch;
	
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
			
	
	/* 交易金额 */
	if( strcmp( s_trace_log_bk.tx_code,"4142") ) 
	{
		tx_amt= 0.00;
	}else{
		tx_amt= s_trace_log_bk.amt;
	}
							
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
			sprintf(str, fmt, s_trace_log_bk.trace_no); 
			break; 
		case 'F': /* 交易代码 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.tx_code );
			break;
		case 'G': /* 交易名称 */						
			sprintf(fmt, "%%-%ds", bll);		
			sprintf( str, fmt, s_menu.title ); 
			break;
		case 'H': /* 凭证名称 */
			sprintf(fmt, "%%-%ds", bll);		
			sprintf( str, fmt, s_note_parm.name ); 
			break;
		case 'I': /* 凭证编码 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.note_type);
			break;
		case 'J': /* 起始号码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_trace_log_bk.note_no);
			break;
		case 'K': /* 终止号码 */		
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, end_note_no );		
			break;
		case 'M': /* 数量 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, note_cnt); 			
			break;	
		case 'N': /* 交易金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , tx_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 			
			break;	
		case 'L': /* 柜员 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_trace_log_bk.tel);
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
