/*************************************************************
* 文 件 名: rpt901.c
* 功能描述：科目流水检索查询打印
*
* 作    者: martin
* 完成日期: 2007年07月
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret ) \
	  {\
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
#include "tx_def_c.h"
#include "com_sys_parm_c.h"
#include "dc_log_rz_c.h"

#define PAGE_CNT 42

int page=0;
int line=0;
int line_file=0;

FILE    *fp; 
int     ret = 0;
int     opncnt;
char    vpfm[21];
char    t_str[41];

struct dc_log_rz_c	s_dc_log_rz;
struct tx_def_c s_tx_def;


int get_ratio_kmls(int bli,int bll,char str[100]);

rpt901()
{
	int    ttlnum;
	char   filename[100];
	char   old_br_no[6];
	
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

	strcpy( filename , "RPT901" );
	strcpy( vpfm, "RPT901");
	
	ret = Dc_log_rz_Dec_Sel( g_pub_tx.reply, "tx_date='%08d' and 1=1 ORDER BY tx_tx_br_no,acc_hrt,amt,trace_no",g_pub_tx.tx_date );
	ERR_DEAL
	
	ttlnum=0;
	
	while(1)
	{
		ret = Dc_log_rz_Fet_Sel( &s_dc_log_rz,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL

				fclose(fp);
			}
			else 
			{
				sprintf( acErrMsg, "本日没有要打印的流水!" );
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
		
		/* 机构变换时 */
		pub_base_strpack( s_dc_log_rz.tx_tx_br_no );
		if( strcmp( s_dc_log_rz.tx_tx_br_no , old_br_no ) )
		{
			printf("[%s][%d]这是[%s]机构!\n",__FILE__,__LINE__,s_dc_log_rz.tx_tx_br_no);
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL

				fclose(fp);
			}
			
			strcpy( old_br_no , s_dc_log_rz.tx_tx_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_dc_log_rz.tx_tx_br_no , filename , 0 );
			ERR_DEAL

			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_dc_log_rz.tx_tx_br_no , filename );
			ERR_DEAL

			/* 打印页首 */
			ret = print_head( );
			ERR_DEAL
		}
		
			/* 打印信息 */
		ret = print_body( );
		ERR_DEAL

		line_file++;
		sprintf( acErrMsg, "科目流水=第[%d]行!",line );
		WRITEMSG
		if( line_file>PAGE_CNT )
		{
			line_file=0;
			page++;
			sprintf( acErrMsg, "科目流水=第[%d]页!",page );
			WRITEMSG			
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL
			
			
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}

	Dc_log_rz_Clo_Sel( );
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"科目流水清单处理程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"科目流水清单处理程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_kmls);
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
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
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
int get_ratio_kmls( bli,bll,str )
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
	memset(&s_tx_def,0x00,sizeof(struct tx_def_c));

	vtcp_log("DC_LOG_RZ.TX_TX_BR_NO==============[%s]",s_dc_log_rz.tx_tx_br_no);

	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , s_dc_log_rz.tx_date/10000 , s_dc_log_rz.tx_date%10000/100 , s_dc_log_rz.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_dc_log_rz.tx_tx_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = pub_base_getbrname( s_dc_log_rz.tx_br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" ,br_name,s_dc_log_rz.tx_tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break;
		case 'E': /* 流水号 add by martin 取消流水前加（*） */
		  if( strcmp( s_dc_log_rz.sts , "1" ) )
		  {
				sprintf(fmt,"%%%dld",bll); 
				sprintf(str, fmt , s_dc_log_rz.trace_no); 
				break; 
			}else
			{
				sprintf(fmt,"*%%%dld",bll); 
				sprintf(str, fmt , s_dc_log_rz.trace_no); 
				break;
		  } 
		case 'J': /* 科目号 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt , s_dc_log_rz.acc_hrt);
			break;
		case 'K': /* 交  易  额 */
			sprintf( amt_tmp, "%.2lf" , s_dc_log_rz.amt ); 
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