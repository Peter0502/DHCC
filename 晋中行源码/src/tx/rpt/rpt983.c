/*************************************************************
* 文 件 名: rpt983.c
* 功能描述：金融机构现金收支月报表
*
* 作    者: 
* 完成日期: 2003年08月15日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define UNIT_AMT 1000000
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "mo_xjszybb_c.h"
#include "correl_brf_c.h"
#define PAGE_CNT 43

struct com_sys_parm_c 		s_com_sys_parm;
struct mo_xjszybb_c	s_mo_xjszybb;
struct correl_brf_c			s_correl_brf;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
int cnt = 1;
int target_cnt=1;
double lst_amt=0.00;
double now_amt=0.00;
long lst_date;

int get_ratio_lsqd(int bli,int bll,char str[100]);

char CAL_BR_NO[6];

rpt983( )
{
	int ttlnum;
	char   filename[100];
	long   tmp_date;

	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/* 初始化全局变量 */
	pub_base_sysinit();
    
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	vtcp_log("AAAAAAAAAAAAAAAAAAAAAAA");
	g_pub_tx.tx_date = s_com_sys_parm.lst_date;
	tmp_date = g_pub_tx.tx_date/100*100 + 1 ; 
	ret = pub_base_deadlineD( tmp_date, -1, &lst_date );
	ERR_DEAL
	lst_date = lst_date/100 ;

	tmp_date = g_pub_tx.tx_date/100;

	vtcp_log("CCCCCCCCCCCCCCCCCCCCCCC------------->INSPECT-->[%ld]",lst_date);
	vtcp_log("CCCCCCCCCCCCCCCCCCCCCCC------------->INSPECT-->[%ld]",tmp_date);
	strcpy( filename , "BMjrjjxjszybb" );	
	strcpy( vpfm, "jrjjxjszybb" );
	strcpy( CAL_BR_NO,"61000" );

	ttlnum=0;

	ret = Correl_brf_Dec_Sel( g_pub_tx.reply, " 1=1 " );
	ERR_DEAL

	while(1)
	{
		ret = Correl_brf_Fet_Sel( &s_correl_brf,g_pub_tx.reply );
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
				sprintf( acErrMsg, "本日没有要打印的记录!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}

		/*** 取上月金额数 ***/
		ret = sql_sum_double ( "mo_xjszybb", "amt", &lst_amt, 
			"br_no='%s' and target_no='%s' and tx_date=%ld", 
			CAL_BR_NO,s_correl_brf.target_no, lst_date );
		ERR_DEAL

		/*** 取本月金额数 ***/
		ret = sql_sum_double ( "mo_xjszybb", "amt", &now_amt, 
			"br_no='%s' and target_no='%s' and tx_date=%ld", 
			CAL_BR_NO,s_correl_brf.target_no, tmp_date );
		ERR_DEAL

		if( !ttlnum )
		{
			page=1;
			line_file=1;

			/* 删除原文件 */
			ret = pub_rpt_rmfile( CAL_BR_NO , filename , 0 );
			ERR_DEAL

			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, CAL_BR_NO , filename );
			ERR_DEAL

			/* 打印页首 */
			ret = print_head( );
			ERR_DEAL
		}

		/* 打印信息 */
		ret = print_body( );
		ERR_DEAL

		line_file++;
		if( line_file>PAGE_CNT )
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
	Correl_brf_Clo_Sel( );
	fclose(fp);

	vtcp_log("CCCCCCCCCCCCCCCCCCCCCCC------------->INSPECT-->[%ld]",tmp_date);
	strcpy( filename , "BMjrjjxjszybb" );	
	strcpy( vpfm, "jrjjxjszybb" );
	strcpy( CAL_BR_NO,"61027" );

	ttlnum=0;

	ret = Correl_brf_Dec_Sel( g_pub_tx.reply, " 1=1 " );
	ERR_DEAL

	while(1)
	{
		ret = Correl_brf_Fet_Sel( &s_correl_brf,g_pub_tx.reply );
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
				sprintf( acErrMsg, "本日没有要打印的记录!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}

		/*** 取上月金额数 ***/
		ret = sql_sum_double ( "mo_xjszybb", "amt", &lst_amt, 
			"br_no='%s' and target_no='%s' and tx_date=%ld", 
			CAL_BR_NO,s_correl_brf.target_no, lst_date );
		ERR_DEAL

		/*** 取本月金额数 ***/
		ret = sql_sum_double ( "mo_xjszybb", "amt", &now_amt, 
			"br_no='%s' and target_no='%s' and tx_date=%ld", 
			CAL_BR_NO,s_correl_brf.target_no, tmp_date );
		ERR_DEAL

		if( !ttlnum )
		{
			page=1;
			line_file=1;

			/* 删除原文件 */
			ret = pub_rpt_rmfile( CAL_BR_NO , filename , 0 );
			ERR_DEAL

			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, CAL_BR_NO , filename );
			ERR_DEAL

			/* 打印页首 */
			ret = print_head( );
			ERR_DEAL
		}

		/* 打印信息 */
		ret = print_body( );
		ERR_DEAL

		line_file++;
		if( line_file>PAGE_CNT )
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
	Correl_brf_Clo_Sel( );
	fclose(fp);
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"金融机构现金收支月报表处理成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"金融机构现金收支月报表处理失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
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
/* 说明：bli 代表变量字符名称 ... bll代表变量占的长度 ... str代表变量值 */
int get_ratio_lsqd( bli,bll,str )
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
	long long_amt;

	struct correl_brf_c x_correl_brf;

	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , s_com_sys_parm.sys_date/10000 , s_com_sys_parm.sys_date%10000/100 , s_com_sys_parm.sys_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'C': /* 机构名称 */
			ret = pub_base_getbrname( CAL_BR_NO, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" ,br_name,CAL_BR_NO ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'F': /* 位置 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, cnt++); 
			break; 
		case 'G': /* 频度 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , "月");
			break;
		case 'H': /* 上月数 */
			vtcp_log("lst_amt---------------[%lf]",lst_amt);
			long_amt = pub_base_PubDround_1( lst_amt / UNIT_AMT ) * 100;
			sprintf( amt_tmp ,"%ld", long_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'I': /* 本月数 */
			vtcp_log("now_amt---------------[%lf]",now_amt);
			long_amt = pub_base_PubDround_1( now_amt / UNIT_AMT ) * 100;
			sprintf( amt_tmp ,"%ld", long_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L': /* 指标编码 */
			sprintf( amt_tmp, " %04d ",target_cnt++ );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'S': /* 业务类 */
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt, "现金");
			break;
		case 'U': /* 指标名称 */
			pub_base_strpack(s_correl_brf.target_no);

			ret = Correl_brf_Sel( g_pub_tx.reply, &x_correl_brf, \
						"target_no='%s'",  s_correl_brf.target_no );

			vtcp_log("RET------------.....?[%d]",ret);
			vtcp_log("INSPECT------>target_no=[%s],target_name=[%s]",
						s_correl_brf.target_no,x_correl_brf.brf );

			ERR_DEAL
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt, x_correl_brf.brf);
			break;
		case 'X': /* 指标代码 */
			pub_base_strpack(s_correl_brf.target_no);
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt,s_correl_brf.target_no);
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
