/*************************************************************
* 文 件 名: rpt134.c
* 功能描述：主要业务状况日报表（总行）旧
*  BRZHZYYWZKRBB1  61000
* 注：格式文件为zyywzkrbb1.tbl
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define MYRETERR if(g_reply_int) \
	{ \
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", g_reply_int ); \
		WRITEMSG \
		goto ErrExit; \ 
	}
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "gl_oprt_dyn_c.h"
#include "gl_mst_c.h"
#include "gl_sub_c.h"
#include "gl_rpt_c.h"
#include "dda_zyywzk_c.h"
#include "trace_log_bk_c.h"
#include "cash_lmt_def_c.h"

static struct com_sys_parm_c sComSysParm;
static struct com_branch_c pComBranch;
static struct com_branch_c sComBranch1;
static struct gl_oprt_dyn_c sGlOprtDyn;
static struct gl_oprt_dyn_c sGlOprtDyn_sum;
char _flag='0';
double hjb=0.00;
double hjc=0.00;
double hje=0.00;
double hjf=0.00;
double hjh=0.00;
double hji=0.00;

char *pub_rept_cal_zhkmye1( char *up_br_no, char *main_acc );
char *pub_rept_cal_bsrzhkmye1( char *up_br_no, char *main_acc );
char *pub_rept_cal_bnczhkmye1( char *up_br_no, char *main_acc );
char *pub_rept_cal_zyywzkrbb( char *up_br_no, char *item );

int get_ratio_zyywzkrbb(int bli,int bll,char str[100]);
long  g_rept_date;

rpt134()
{
	int line, opncnt;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );
 hjb=0.00;
 hjc=0.00;
 hje=0.00;
 hjf=0.00;
 hjh=0.00;
 hji=0.00;

    g_reply_int = sql_begin(); /*打开事务*/
	MYRETERR

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
	strcpy( fname, "BRZHZYYWZKRBB1" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	MYRETERR

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	g_reply_int=Com_sys_parm_Sel(g_pub_tx.reply,&sComSysParm,"1=1");
	MYRETERR

	strcpy(vpfm, "zyywzkrbb1");
	g_rept_date=sComSysParm.lst_date;

	g_reply_int = pub_rpt_openfile( &fp, "61000", fname );
	MYRETERR

	g_reply_int = pub_rpt_read_pfm_qd( fp, &line, 
		vpfm, "0001", "AA", &opncnt, get_ratio_zyywzkrbb );
	MYRETERR

	g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, 
		"up_br_no='61000' and br_no!='61000' and br_type!='0' and wrk_sts!='*' and br_no[1,2]!='62' and br_no not in(select val from com_parm where parm_code='YWZKB') ORDER BY br_no" );
	MYRETERR
	
	while(1) 
	{ 
		memset( &pComBranch, 0x00, sizeof( struct com_branch_c ) );
		g_reply_int = Com_branch_Fet_Sel1( &pComBranch, g_pub_tx.reply );
		if( g_reply_int==100 ) break;
		MYRETERR

		_flag='B';
		g_reply_int = pub_rpt_read_pfm_qd( fp, &line, 
			vpfm, "0001", "BB", &opncnt, get_ratio_zyywzkrbb );
		MYRETERR
	} 
	Com_branch_Clo_Sel1();

	_flag='C';
	g_reply_int = pub_rpt_read_pfm_qd( fp, &line, 
		vpfm, "0001", "CC", &opncnt, get_ratio_zyywzkrbb );
	MYRETERR

	fclose(fp);

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成主要业务状况日报表成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成主要业务状况日报表失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_zyywzkrbb( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	char item[11];
	char oitem[11];
	double db1,db2,db3,db4,db5;
	
	memset( item,0,sizeof(item) );
	memset( oitem,0,sizeof(oitem) );
	if( bli>='a' && bli<='z' )
	{
		if( _flag=='B' )
		{
	switch( bli ) 
	{ 
		case 'A': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'B': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'C': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'D': 
			sprintf(fmt, "%%%ds", bll);
			sprintf( str, fmt, pComBranch.br_name ); 
			break; 
		
		case 'a':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "202" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "201" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "205" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "211" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "215" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "235" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "243" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "P" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'b':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "202" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "201" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "205" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "211" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "215" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "235" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "243" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "Q" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			hjb+=db3;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'c':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "202" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "201" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "205" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "211" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "215" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "235" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "243" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "R" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			hjc+=db3;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'd':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "211" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "215" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'e':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "211" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "215" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			hje+=db3;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'f':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "211" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "215" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			hjf+=db3;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'g':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "202" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "201" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "205" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "235" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "243" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "P" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'h':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "202" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "201" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "205" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "235" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bsrzhkmye1( pComBranch.br_no , "243" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "Q" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			hjh+=db3;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'i':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "202" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "201" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "205" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "235" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_bnczhkmye1( pComBranch.br_no , "243" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "R" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			hji+=db3;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'j':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "a" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'k':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "b" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'l':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "c" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'm':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "101" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'n':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "234" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'o':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( pComBranch.br_no , "235" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'p':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( pComBranch.br_no , "p" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	}
		}
		else if( _flag=='C' )
		{
	switch( bli ) 
	{ 
		case 'A': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'B': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'C': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'D': 
			sprintf(fmt, "%%%ds", bll);
			sprintf( str, fmt, pComBranch.br_name ); 
			break; 
		
		case 'a':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "202" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "201" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "205" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "211" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "215" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "235" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "243" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( "ALL" , "P" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'b':
			db3=hjb;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'c':
			db3=hjc;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'd':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "211" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "215" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'e':
			db3=hje;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'f':
			db3=hjf;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'g':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "202" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "201" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "205" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "235" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "243" ) );
			db1=atof(vstr);
			db4+=db1;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( "ALL" , "P" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'h':
			db3=hjh;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'i':
			db3=hji;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'j':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( "ALL" , "a" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'k':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( "ALL" , "b" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'l':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( "ALL" , "c" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'm':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "101" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'n':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "234" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'o':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zhkmye1( "ALL" , "235" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		case 'p':
			db4=0.00;
			strcpy( vstr,pub_rept_cal_zyywzkrbb( "ALL" , "p" ) );
			db1=atof(vstr);
			db4+=db1;

			db3=db4/10000;
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,db3 );
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	}
		}
	}
	else
	{

	switch( bli ) 
	{ 
		case 'A': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'B': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'C': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'D': 
			sprintf(fmt, "%%%ds", bll);
			sprintf( str, fmt, pComBranch.br_name ); 
			break; 
		
		default : 
			memset( str,' ',bll ); 
			break; 
	}

	}
	
	return 0; 
}
char *sub_rept_cal_zyywzkrbb( char *up_br_no, char *item )
{
	double _rtn_bal_f;
	char _rtn_bal_s[256];
	char br_no_lst[256];
	char sum_field[100];
	char tmpstr[1024];
	char wherelist[1024];
	struct com_branch_c sComBranch;

	switch( item[0] )
	{
/*存款部分*/
		case 'A': strcpy( sum_field, "cr_de_bal" ); break;    /*存款总额*/
		case 'B': strcpy( sum_field, "cr_de_bal-ys_de_bal" ); break;
		case 'C': strcpy( sum_field, "cr_de_bal-yb_de_bal" ); break;
		case 'D': strcpy( sum_field, "cr_de_bal1" ); break;
		case 'E': strcpy( sum_field, "cr_de_bal1-ys_de_bal1" ); break;
		case 'F': strcpy( sum_field, "cr_de_bal1-yb_de_bal1" ); break;
		case 'G': strcpy( sum_field, "cr_de_bal2" ); break;
		case 'H': strcpy( sum_field, "cr_de_bal2-ys_de_bal2" ); break;
		case 'I': strcpy( sum_field, "cr_de_bal2-yb_de_bal2" ); break;
		case 'J': strcpy( sum_field, "cr_de_bal3" ); break;
		case 'K': strcpy( sum_field, "cr_de_bal3-ys_de_bal3" ); break;
		case 'L': strcpy( sum_field, "cr_de_bal3-yb_de_bal3" ); break;
		case 'M': strcpy( sum_field, "cr_de_bal4" ); break;
		case 'N': strcpy( sum_field, "cr_de_bal4-ys_de_bal4" ); break;
		case 'O': strcpy( sum_field, "cr_de_bal4-yb_de_bal4" ); break;
		case 'P': strcpy( sum_field, "cr_de_bal5" ); break;
		case 'Q': strcpy( sum_field, "cr_de_bal5-ys_de_bal5" ); break;
		case 'R': strcpy( sum_field, "cr_de_bal5-yb_de_bal5" ); break;
		case 'S': strcpy( sum_field, "cr_de_bal6" ); break;
		case 'T': strcpy( sum_field, "cr_de_bal6-ys_de_bal6" ); break;
		case 'U': strcpy( sum_field, "cr_de_bal6-yb_de_bal6" ); break;

		case 'Z': strcpy( sum_field, "yb_de_bal" ); break;
/*贷款部分*/
		case 'a': strcpy( sum_field, "cr_ln_bal" ); break;
		case 'b': strcpy( sum_field, "cr_ln_bal-ys_ln_bal" ); break;
		case 'c': strcpy( sum_field, "cr_ln_bal-yb_ln_bal" ); break;
		case 'd': strcpy( sum_field, "cr_ln_bal1" ); break;
		case 'e': strcpy( sum_field, "cr_ln_bal1-ys_ln_bal1" ); break;
		case 'f': strcpy( sum_field, "cr_ln_bal1-yb_ln_bal1" ); break;
		case 'g': strcpy( sum_field, "cr_ln_bal2" ); break;
		case 'h': strcpy( sum_field, "cr_ln_bal2-ys_ln_bal2" ); break;
		case 'i': strcpy( sum_field, "cr_ln_bal2-yb_ln_bal2" ); break;
		case 'j': strcpy( sum_field, "cr_ln_bal3" ); break;
		case 'k': strcpy( sum_field, "cr_ln_bal3-ys_ln_bal3" ); break;
		case 'l': strcpy( sum_field, "cr_ln_bal3-yb_ln_bal3" ); break;
		case 'm': strcpy( sum_field, "cr_ln_bal4" ); break;
		case 'n': strcpy( sum_field, "cr_ln_bal4-ys_ln_bal4" ); break;
		case 'o': strcpy( sum_field, "cr_ln_bal4-yb_ln_bal4" ); break;
		case 'p': strcpy( sum_field, "cr_ln_bal5" ); break;
		case 'q': strcpy( sum_field, "cr_ln_bal5-ys_ln_bal5" ); break;
		case 'r': strcpy( sum_field, "cr_ln_bal5-yb_ln_bal5" ); break;
		default: goto ErrExit;
	}
	vtcp_log( "统计项目：[%s]", sum_field );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s' order by br_no", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();
		if( !strcmp( up_br_no, "61099" ) )
			sprintf( tmpstr, "br_no in ('%s') and ", up_br_no );
		else
			sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	sprintf( tmpstr, "date='%ld'", g_rept_date );
	strcat( wherelist, tmpstr );

	vtcp_log( "SUM gl_oprt_dyn wherelist\n%s\n", wherelist );
	g_reply_int = sql_sum_double( "gl_oprt_dyn", sum_field, &_rtn_bal_f, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "br_no_lst[%s]", br_no_lst );
		sprintf( acErrMsg, "主要业务状况日报表[%s]出错![%ld]", item, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]",_rtn_bal_f, _rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}
char *pub_rept_cal_zyywzkrbb( char *up_br_no, char *item )
{
	double _rtn_bal_f;
	char _rtn_bal_s[256];
	char br_no_lst[256];
	char sum_field[100];
	char tmpstr[1024];
	char wherelist[1024];
	struct com_branch_c sComBranch;

	switch( item[0] )
	{
/*存款部分*/
		case 'A': strcpy( sum_field, "cr_de_bal" ); break;    /*存款总额*/
		case 'B': strcpy( sum_field, "cr_de_bal-ys_de_bal" ); break;
		case 'C': strcpy( sum_field, "cr_de_bal-yb_de_bal" ); break;
		case 'D': strcpy( sum_field, "cr_de_bal1" ); break;
		case 'E': strcpy( sum_field, "cr_de_bal1-ys_de_bal1" ); break;
		case 'F': strcpy( sum_field, "cr_de_bal1-yb_de_bal1" ); break;
		case 'G': strcpy( sum_field, "cr_de_bal2" ); break;
		case 'H': strcpy( sum_field, "cr_de_bal2-ys_de_bal2" ); break;
		case 'I': strcpy( sum_field, "cr_de_bal2-yb_de_bal2" ); break;
		case 'J': strcpy( sum_field, "cr_de_bal3" ); break;
		case 'K': strcpy( sum_field, "cr_de_bal3-ys_de_bal3" ); break;
		case 'L': strcpy( sum_field, "cr_de_bal3-yb_de_bal3" ); break;
		case 'M': strcpy( sum_field, "cr_de_bal4" ); break;
		case 'N': strcpy( sum_field, "cr_de_bal4-ys_de_bal4" ); break;
		case 'O': strcpy( sum_field, "cr_de_bal4-yb_de_bal4" ); break;
		case 'P': strcpy( sum_field, "cr_de_bal5" ); break;
		case 'Q': strcpy( sum_field, "cr_de_bal5-ys_de_bal5" ); break;
		case 'R': strcpy( sum_field, "cr_de_bal5-yb_de_bal5" ); break;
		case 'S': strcpy( sum_field, "cr_de_bal6" ); break;
		case 'T': strcpy( sum_field, "cr_de_bal6-ys_de_bal6" ); break;
		case 'U': strcpy( sum_field, "cr_de_bal6-yb_de_bal6" ); break;

		case 'Z': strcpy( sum_field, "yb_de_bal" ); break;
/*贷款部分*/
		case 'a': strcpy( sum_field, "cr_ln_bal" ); break;
		case 'b': strcpy( sum_field, "cr_ln_bal-ys_ln_bal" ); break;
		case 'c': strcpy( sum_field, "cr_ln_bal-yb_ln_bal" ); break;
		case 'd': strcpy( sum_field, "cr_ln_bal1" ); break;
		case 'e': strcpy( sum_field, "cr_ln_bal1-ys_ln_bal1" ); break;
		case 'f': strcpy( sum_field, "cr_ln_bal1-yb_ln_bal1" ); break;
		case 'g': strcpy( sum_field, "cr_ln_bal2" ); break;
		case 'h': strcpy( sum_field, "cr_ln_bal2-ys_ln_bal2" ); break;
		case 'i': strcpy( sum_field, "cr_ln_bal2-yb_ln_bal2" ); break;
		case 'j': strcpy( sum_field, "cr_ln_bal3" ); break;
		case 'k': strcpy( sum_field, "cr_ln_bal3-ys_ln_bal3" ); break;
		case 'l': strcpy( sum_field, "cr_ln_bal3-yb_ln_bal3" ); break;
		case 'm': strcpy( sum_field, "cr_ln_bal4" ); break;
		case 'n': strcpy( sum_field, "cr_ln_bal4-ys_ln_bal4" ); break;
		case 'o': strcpy( sum_field, "cr_ln_bal4-yb_ln_bal4" ); break;
		case 'p': strcpy( sum_field, "cr_ln_bal5" ); break;
		case 'q': strcpy( sum_field, "cr_ln_bal5-ys_ln_bal5" ); break;
		case 'r': strcpy( sum_field, "cr_ln_bal5-yb_ln_bal5" ); break;
		default: goto ErrExit;
	}
	vtcp_log( "统计项目：[%s]", sum_field );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s' order by br_no", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();
		if( !strcmp( up_br_no, "61099" ) )
			sprintf( tmpstr, "br_no in ('%s') and ", up_br_no );
		else
			sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	sprintf( tmpstr, "date='%ld'", g_rept_date );
	strcat( wherelist, tmpstr );

	vtcp_log( "SUM gl_oprt_dyn wherelist\n%s\n", wherelist );
	g_reply_int = sql_sum_double( "gl_oprt_dyn", sum_field, &_rtn_bal_f, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "br_no_lst[%s]", br_no_lst );
		sprintf( acErrMsg, "主要业务状况日报表[%s]出错![%ld]", item, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]",_rtn_bal_f, _rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}
/*************************************
函数功能：计算支行及下属机构的存款科目余额
		  主要应用报表：存款余额日报表
参数：
    入口：上级机构号 char *up_br_no
		  科目号     char *main_acc
返回：
注：统计机构包括up_br_no及up_br_no的下级机构,如果机构号为ALL则统计全行
	统计科目包括所有前三位为main_acc的科目控制字
*************************************/
char *pub_rept_cal_zhkmye1( char *up_br_no, char *main_acc )
{
	double _rtn_bal_f;
	double _ret_bal_f_dd;
	double _ret_bal_f_td;
	char _rtn_bal_s[256];
	char br_no_lst[1024];
	char prdt_no_lst[1024];
	struct com_branch_c sComBranch;
	char tmpstr[1024];
	char wherelist[1024];
	char wherelist_tmp[1024];

vtcp_log( "计算   [%s]   支行   [%s]   科目余额", up_br_no, main_acc );

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );
	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		if( !strcmp( main_acc, "101" ) )
			sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		else
			sprintf( tmpstr, "gl_prdt_dyn.br_no in (%s) and ", br_no_lst );

		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*库存现金统计*/
	if( !strcmp( main_acc, "101" ) )
	{
		strcat( wherelist, "cur_no='01'" );
		g_reply_int = sql_sum_double( "cash_mst", "bal", &_ret_bal_f_dd, wherelist );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "统计[%s]机构现金台账表出错![%ld]", up_br_no, g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		goto OkExit;
	}

	/*存款科目统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	/*sprintf( tmpstr, "dd_parm.dc_code[1,3]='%s' and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );*/
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and dd_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	/*g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );*/
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计活期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	/*sprintf( tmpstr, "td_parm.dc_code[1,3]='%s' and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );*/
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and td_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );
	strcpy( wherelist, wherelist_tmp );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	/*g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );*/
	g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计定期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _ret_bal_f_dd+_ret_bal_f_td );
	vtcp_log("[%lf][%lf][%s]",_ret_bal_f_dd,_ret_bal_f_td,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：计算支行及下属机构的科目余额
		  主要应用报表：存款余额日报表
参数：
    入口：上级机构号 char *up_br_no
		  科目号     char *main_acc
返回：该机构本日存款余额
注：统计机构包括up_br_no及up_br_no的下级机构,如果机构号为ALL则统计全行
	统计科目包括所有前三位为main_acc的科目控制字
*************************************/
char *pub_rept_cal_bsrzhkmye1( char *up_br_no, char *main_acc )
{
	double _rtn_bal_f;
	double _ret_bal_f_dd;
	double _ret_bal_f_td;
	char _rtn_bal_s[256];
	char br_no_lst[1024];
	char prdt_no_lst[1024];
	struct com_branch_c sComBranch;
	char tmpstr[1024];
	char wherelist[1024];
	char wherelist_tmp[1024];
	double cr_bal_v, lcd_bal_v;
	long lst_date;

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

vtcp_log( "计算   [%s]   支行   [%s]   本日科目余额", up_br_no, main_acc );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		if( !strcmp( main_acc, "101" ) )
			sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		else
			sprintf( tmpstr, "gl_prdt_dyn.br_no in (%s) and ", br_no_lst );

		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*存款科目统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	/*sprintf( tmpstr, "dd_parm.dc_code[1,3]='%s' and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );*/
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and dd_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	/*g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );*/
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计活期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	/*sprintf( tmpstr, "td_parm.dc_code[1,3]='%s' and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );*/
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and td_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );
	strcpy( wherelist, wherelist_tmp );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	/*g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );*/
	g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计定期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	cr_bal_v = _ret_bal_f_dd + _ret_bal_f_td;

vtcp_log( "计算   [%s]   支行   [%s]   昨日科目余额", up_br_no, main_acc );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		lst_date = pub_base_daynumSUB( g_rept_date, 1 );
		sprintf( tmpstr, "gl_prdt_dyn_hst.date='%ld' and gl_prdt_dyn_hst.br_no in (%s) and ", 
			lst_date, br_no_lst );

		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*存款科目统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and dd_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn_hst:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dd_parm,dc_acc_rel", "gl_prdt_dyn_hst.cr_bal", &_ret_bal_f_dd, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计活期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and td_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no", main_acc );
	strcpy( wherelist, wherelist_tmp );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn_hst:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,td_parm,dc_acc_rel", "gl_prdt_dyn_hst.cr_bal", &_ret_bal_f_td, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计定期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	lcd_bal_v = _ret_bal_f_dd + _ret_bal_f_td;    /*昨日科目余额*/

	sprintf( _rtn_bal_s, "%.2lf", cr_bal_v - lcd_bal_v );
	vtcp_log("[%lf]-[%lf]=[%s]", cr_bal_v, lcd_bal_v, _rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：计算支行及下属机构的科目余额
		  主要应用报表：存款余额日报表
参数：
    入口：上级机构号 char *up_br_no
		  科目号     char *main_acc
返回：该机构本日存款余额
注：统计机构包括up_br_no及up_br_no的下级机构,如果机构号为ALL则统计全行
	统计科目包括所有前三位为main_acc的科目控制字
*************************************/
char *pub_rept_cal_bnczhkmye1( char *up_br_no, char *main_acc )
{
	double _rtn_bal_f;
	double _ret_bal_f_dd;
	double _ret_bal_f_td;
	char _rtn_bal_s[256];
	char br_no_lst[1024];
	char prdt_no_lst[1024];
	struct com_branch_c sComBranch;
	struct dda_zyywzk_c sDdaZyywzk;
	char tmpstr[1024];
	char wherelist[1024];
	char wherelist_tmp[1024];
	double cr_bal_v, y_cr_bal_v;
	long lst_date;

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

vtcp_log( "计算   [%s]   支行   [%s]   本日科目余额", up_br_no, main_acc );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		if( !strcmp( main_acc, "101" ) )
			sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		else
			sprintf( tmpstr, "gl_prdt_dyn.br_no in (%s) and ", br_no_lst );

		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*存款科目统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and dd_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计活期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and td_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );
	strcpy( wherelist, wherelist_tmp );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计定期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	cr_bal_v = _ret_bal_f_dd + _ret_bal_f_td;

vtcp_log( "计算   [%s]   支行   [%s]   年初科目余额", up_br_no, main_acc );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		sprintf( tmpstr, "dda_zyywzk.inst_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*年初科目余额统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dda_zyywzk.item_no like '%s%%%%'", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum dda_zyywzk:lst_ybal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "dda_zyywzk", "lst_ybal", &y_cr_bal_v, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计年初余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	sprintf( _rtn_bal_s, "%.2lf", cr_bal_v - y_cr_bal_v );
	vtcp_log("[%lf]-[%lf]=[%s]", cr_bal_v, y_cr_bal_v, _rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}
