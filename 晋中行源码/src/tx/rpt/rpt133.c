/*************************************************************
* 文 件 名: rpt133.c
* 功能描述：主要业务状况日报表（总行）新
*  BRZYYWZKRBB2  61000
* 注：格式文件为zyywzkrbb2.tbl
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

static struct com_sys_parm_c sComSysParm;
static struct com_branch_c pComBranch;
static struct com_branch_c sComBranch1;
static struct gl_oprt_dyn_c sGlOprtDyn;
static struct gl_oprt_dyn_c sGlOprtDyn_sum;
char _flag='0';

int get_ratio_zyywzkrbb(int bli,int bll,char str[100]);
char *sub_rept_cal_zyywzkrbb( char *up_br_no, char *item );
long  g_rept_date;

rpt133()
{
	int line, opncnt;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );

    g_reply_int = sql_begin(); /*打开事务*/
	MYRETERR

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
	strcpy( fname, "BRZYYWZKRBB2" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	MYRETERR

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	g_reply_int=Com_sys_parm_Sel(g_pub_tx.reply,&sComSysParm,"1=1");
	MYRETERR

	strcpy(vpfm, "zyywzkrbb2");
	g_rept_date=sComSysParm.lst_date;

	g_reply_int = pub_rpt_openfile( &fp, TOT_BR_NO, fname );
	MYRETERR

	g_reply_int = pub_rpt_read_pfm_qd( fp, &line, 
		vpfm, "0001", "AA", &opncnt, get_ratio_zyywzkrbb );
	MYRETERR

	g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, 
		"up_br_no='61000' and br_no!='61000' and br_type!='0' and wrk_sts!='*' and br_no not in(select val from com_parm where parm_code='YWZKB') ORDER BY br_no" ); 
	/* parm_code='YWZKB' 表示不用出业务状况表的机构 */
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

	_flag='D';
	g_reply_int = pub_rpt_read_pfm_qd( fp, &line, 
		vpfm, "0001", "DD", &opncnt, get_ratio_zyywzkrbb );
	MYRETERR

	g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, 
		"up_br_no='61000' and br_no!='61000' and br_type!='0' and wrk_sts!='*' and br_no not in(select val from com_parm where parm_code='YWZKB') ORDER BY br_no" );
	MYRETERR
	
	while(1) 
	{ 
		memset( &pComBranch, 0x00, sizeof( struct com_branch_c ) );
		g_reply_int = Com_branch_Fet_Sel1( &pComBranch, g_pub_tx.reply );
		if( g_reply_int==100 ) break;
		MYRETERR

		_flag='E';
		g_reply_int = pub_rpt_read_pfm_qd( fp, &line, 
			vpfm, "0001", "EE", &opncnt, get_ratio_zyywzkrbb );
		MYRETERR
	} 
	Com_branch_Clo_Sel1();

	_flag='F';
	g_reply_int = pub_rpt_read_pfm_qd( fp, &line, 
		vpfm, "0001", "FF", &opncnt, get_ratio_zyywzkrbb );
	MYRETERR

	_flag='G';
	g_reply_int = pub_rpt_read_pfm_qd( fp, &line, 
		vpfm, "0001", "GG", &opncnt, get_ratio_zyywzkrbb );
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
			item[0]=bli+'A'-'a';
			strcpy( vstr,sub_rept_cal_zyywzkrbb( pComBranch.br_no , item ) );
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,atof(vstr)/10000 );
		}
		else if( _flag=='C' )
		{
			item[0]=bli+'A'-'a';
			strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , item ) );
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,atof(vstr)/10000 );
		}
		else if( _flag=='E' )
		{
			item[0]=bli;
			strcpy( vstr,sub_rept_cal_zyywzkrbb( pComBranch.br_no , item ) );
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,atof(vstr)/10000 );
		}
		else if( _flag=='F' )
		{
			item[0]=bli;
			strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , item ) );
			sprintf(fmt, "%%%d.0lf", bll);
			sprintf( str,fmt,atof(vstr)/10000 );
		}
		else if( _flag=='G' )
		{
			db4=0;
			if( bli=='a' )
			{
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "a" ) );
				db1=atof(vstr);
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "A" ) );
				db2=atof(vstr);
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "M" ) );
				db3=atof(vstr);
				if( db2-db3==0.00 ) db4=0.00;
				else db4=(db1/(db2-db3))*100;
			}
			else if( bli=='b' )
			{
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "d" ) );
				db1=atof(vstr);
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "D" ) );
				db2=atof(vstr);
				db3=0.00;
				if( db2-db3==0.00 ) db4=0.00;
				else db4=(db1/(db2-db3))*100;
			}
			else if( bli=='c' )
			{
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "g" ) );
				db1=atof(vstr);
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "G" ) );
				db2=atof(vstr);
				db3=0.00;
				if( db2-db3==0.00 ) db4=0.00;
				else db4=(db1/(db2-db3))*100;
			}
			else if( bli=='d' )
			{
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "j" ) );
				db1=atof(vstr);
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "J" ) );
				db2=atof(vstr);
				db3=0.00;
				if( db2-db3==0.00 ) db4=0.00;
				else db4=(db1/(db2-db3))*100;
			}
			else if( bli=='e' )
			{
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "a" ) );
				db1=atof(vstr);
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "m" ) );
				db2=atof(vstr);
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "A" ) );
				db3=atof(vstr);
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "M" ) );
				db5=atof(vstr);

				if( db3-db5==0.00 ) db4=0.00;
				else db4=((db1-db2)/(db3-db5))*100;
			}
			else if( bli=='f' )
			{
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "p" ) );
				db1=atof(vstr);
				strcpy( vstr,sub_rept_cal_zyywzkrbb( "ALL" , "Z" ) );
				db2=atof(vstr);
				db3=0.00;
				if( db2-db3==0.00 ) db4=0.00;
				else db4=(db1/(db2-db3))*100;
			}
			sprintf(fmt, "%%%d.2lf", bll);
			sprintf( str,fmt,db4 );
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
	sprintf( tmpstr, "\"date\"='%ld'", g_rept_date );
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
