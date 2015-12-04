/*************************************************************
* 文 件 名: rpt048.c
* 功能描述：定期储蓄存款分析月报表（支行、总行）
*
* 作    者: lance
* 完成日期: 2003年9月27日
*
* 注：格式文件为dqcxckfxybb.tbl
*	  从gl_prdt_dyn统计
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"

static struct com_sys_parm_c sComSysParm;
static struct com_branch_c sComBranch;
static struct com_branch_c sComBranch1;
static double sum_a,m_3,m_6,y_1,y_2,y_3,y_5,y_8;
static double sum_a_sy,m_3_sy,m_6_sy,y_1_sy,y_2_sy,y_3_sy,y_5_sy,y_8_sy;    /*上月值*/
static double sum_a_nc,m_3_nc,m_6_nc,y_1_nc,y_2_nc,y_3_nc,y_5_nc,y_8_nc;    /*年初值*/
static double sum_sum_a,sum_m_3,sum_m_6,sum_y_1,sum_y_2,sum_y_3,sum_y_5,sum_y_8;    /*合计值*/
static double sum_sum_a_sy,sum_m_3_sy,sum_m_6_sy,sum_y_1_sy,sum_y_2_sy,sum_y_3_sy,sum_y_5_sy,sum_y_8_sy;    /*上月合计值*/
static double sum_sum_a_nc,sum_m_3_nc,sum_m_6_nc,sum_y_1_nc,sum_y_2_nc,sum_y_3_nc,sum_y_5_nc,sum_y_8_nc;    /*年初合计值*/
static double cdb_de_1,cdb_de_2,cdb_ln,cdb;    /*储蓄长期存款,中长期贷款,存贷比*/

int get_ratio_dqcxckfxybb(int bli,int bll,char str[100]);
int get_ratio_dqcxckfxybb_hj(int bli,int bll,char str[100]);
rpt048()
{
	int line, opncnt;
	char vpfm[21];
	char fname[20];
	char pre_br_type[2];
	char pre_br_no[6];
	FILE *fp; 
	long date_next_year;
	
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );

    g_reply_int = sql_begin(); /*打开事务*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
	strcpy( fname, "RPT048");
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除定期储蓄存款分析月报表错误!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	g_reply_int = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "sys_date=%ld" , g_pub_tx.tx_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "SUM com_sys_parm error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}

	/*strcpy(vpfm, "dqcxckfxybb");*/
	strcpy(vpfm, "RPT048");

	/*总行、清算中心和管理支行营业部*/
	g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "br_type in ('1','4','6') ORDER BY br_no" );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE com_branch CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}
	while(1) 
	{ 
		memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
		g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH com_branch CUR error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100 )
		{
			/*打印合计值*/
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, \ 
				get_ratio_dqcxckfxybb_hj );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			if( !strcmp( pre_br_type, "6" ) )    /*总行*/
			{
				/*取中长期存款总额与贷款总额及存贷比值*/
				pub_base_deadlineM( sComSysParm.lst_date, 12, &date_next_year );
				g_reply_int = sql_sum_double( "td_mst", "bal", &cdb_de_1, \ 
					"mtr_date>=%ld and cal_code[1]=1", date_next_year );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"SUM td_mst bal error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				g_reply_int = sql_sum_double( "td_mst", "bal", &cdb_de_2, \ 
					"mtr_date>=%ld and substr(cal_code,1,1)!=1", date_next_year );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"SUM td_mst bal error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				g_reply_int = sql_sum_double( "ln_mst", "bal", &cdb_ln, "mtr_date>=%ld", date_next_year );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"SUM td_mst bal error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				cdb = cdb_ln / ( cdb_de_1 + cdb_de_2 );

				g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt, \ 
					get_ratio_dqcxckfxybb );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM ZZ error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
			}

			fclose( fp );
			break;
		}

TRACE;
		/*机构号变,下一个文件*/
		if( strcmp( sComBranch.br_no, pre_br_no ) )
		{
			if( strlen( pre_br_no ) ) /*不是第一个机构,打上个机构的结尾*/
			{
				/*打印合计值*/
				g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, \ 
					get_ratio_dqcxckfxybb_hj );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}

				if( !strcmp( pre_br_type, "6" ) )    /*总行打印存贷比*/
				{
					/*取中长期存款总额与贷款总额及存贷比值*/
					pub_base_deadlineM( sComSysParm.lst_date, 12, &date_next_year );
					g_reply_int = sql_sum_double( "td_mst", "bal", &cdb_de_1, \ 
						"mtr_date>=%ld and substr(cal_code,1,1)=1", date_next_year );
					if( g_reply_int )
					{
						sprintf( acErrMsg,"SUM td_mst bal error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
					g_reply_int = sql_sum_double( "td_mst", "bal", &cdb_de_2, \ 
						"mtr_date>=%ld and substr(cal_code,1,1)!=1", date_next_year );
					if( g_reply_int )
					{
						sprintf( acErrMsg,"SUM td_mst bal error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
					g_reply_int = sql_sum_double( "ln_mst", "bal", &cdb_ln, "mtr_date>=%ld", date_next_year );
					if( g_reply_int )
					{
						sprintf( acErrMsg,"SUM td_mst bal error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
					cdb = cdb_ln / ( cdb_de_1 + cdb_de_2 );
	
					g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt, \ 
						get_ratio_dqcxckfxybb );
					if( g_reply_int )
					{
						sprintf( acErrMsg,"RPT READ PFM ZZ error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
				}
				fclose( fp );
			}

			/*打印下一个机构开始*/
			g_reply_int = pub_rpt_openfile( &fp, sComBranch.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_dqcxckfxybb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_type, sComBranch.br_type );
			strcpy( pre_br_no, sComBranch.br_no );

			sum_a_nc=m_3_nc=m_6_nc=y_1_nc=y_2_nc=y_3_nc=y_5_nc=y_8_nc=0;    /*年初值*/
			sum_sum_a=sum_m_3=sum_m_6=sum_y_1=sum_y_2=sum_y_3=sum_y_5=sum_y_8=0;    /*合计值*/
			sum_sum_a_sy=sum_m_3_sy=sum_m_6_sy=sum_y_1_sy=sum_y_2_sy=sum_y_3_sy=sum_y_5_sy=sum_y_8_sy=0;    /*上月合计值*/
		}

		if( sComBranch.br_type[0] == '6' )    /*总行*/
			g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "br_type in ('1','4') ORDER BY br_no" );
		else if( sComBranch.br_type[0] == '1' )    /*清算中心*/
			g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "br_no='%s' ORDER BY br_no", sComBranch.br_no );
		else if( sComBranch.br_type[0] == '4' )    /*管理支行营业部*/
		{
			if( !strcmp( sComBranch.up_br_no, "61000" ) )
				g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "br_no='%s' ORDER BY br_no", sComBranch.br_no );
			else
				g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "up_br_no='%s' ORDER BY br_no", sComBranch.up_br_no );
		}
		if( g_reply_int ) 
		{ 
			sprintf(acErrMsg, "DECLARE com_branch1 CUR error![%d]", g_reply_int ); 
			WRITEMSG 
			goto ErrExit; 
		} 
		while(1)
		{
			memset( &sComBranch1, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel1( &sComBranch1, g_pub_tx.reply );
			if( g_reply_int && g_reply_int != 100 )
			{
				sprintf( acErrMsg,"FETCH com_branch CUR1 error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			else if( g_reply_int == 100 )
				break;

			g_reply_int = get_br_cr_bal( sComBranch1.br_no );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"取[%s]机构各项存款余额出错", sComBranch1.br_no );
				WRITEMSG
				goto ErrExit;
			}

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "AA", &opncnt, get_ratio_dqcxckfxybb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM AA error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			sum_sum_a += sum_a;
			sum_m_3 += m_3;
			sum_m_6 += m_6;
			sum_y_1 += y_1;
			sum_y_2 += y_2;
			sum_y_3 += y_3;
			sum_y_5 += y_5;
			sum_y_8 += y_8;

			sum_sum_a_sy += sum_a_sy;
			sum_m_3_sy += m_3_sy;
			sum_m_6_sy += m_6_sy;
			sum_y_1_sy += y_1_sy;
			sum_y_2_sy += y_2_sy;
			sum_y_3_sy += y_3_sy;
			sum_y_5_sy += y_5_sy;
			sum_y_8_sy += y_8_sy;

			sum_sum_a_nc += sum_a_nc;
			sum_m_3_nc += m_3_nc;
			sum_m_6_nc += m_6_nc;
			sum_y_1_nc += y_1_nc;
			sum_y_2_nc += y_2_nc;
			sum_y_3_nc += y_3_nc;
			sum_y_5_nc += y_5_nc;
			sum_y_8_nc += y_8_nc;
			/*合总计值*/
		}
		Com_branch_Clo_Sel1();
	}
	
TRACE;
	Com_branch_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成定期储蓄存款分析月报表成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成定期储蓄存款分析月报表失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_dqcxckfxybb( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	double dqdqck,dqyfzk;    /*一个月内到期的定期存款和应付帐款*/
	
	switch( bli ) 
	{ 
		case 'W': 
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s", sComBranch.br_name );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'X': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'Y': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'Z': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'V': 
			pub_base_strpack( sComBranch1.br_name );
			sprintf(vstr, "%s", sComBranch1.br_name );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'A': 
			sprintf(vstr, "%13.2lf", cdb_ln );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'B': 
			sprintf(vstr, "%13.2lf", cdb_de_1 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'C': 
			sprintf(vstr, "%13.2lf", cdb_de_2 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'D': 
			sprintf(vstr, "%7.2lf", pub_base_PubDround_1( cdb * 100 ) );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'E':
			dqdqck = 0.00;
			g_reply_int = sql_sum_double( "gl_rpt", "amt", &dqdqck, 
				"\"date\"='%ld' and code='M003_be'", sComSysParm.lst_date );
			if( g_reply_int ) 
			{ 
				sprintf(acErrMsg, "SUM gl_rpt error![%d]", g_reply_int ); 
				WRITEMSG 
			}
			sprintf(vstr, "%13.2lf", dqdqck );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'F':
			dqyfzk = 0.00;
			g_reply_int = sql_sum_double( "gl_rpt", "amt", &dqyfzk, 
				"\"date\"='%ld' and code='M003_bg'", sComSysParm.lst_date );
			if( g_reply_int ) 
			{ 
				sprintf(acErrMsg, "SUM gl_rpt error![%d]", g_reply_int ); 
				WRITEMSG 
			}
			sprintf(vstr, "%13.2lf", dqyfzk );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'a': 
			sprintf(vstr, "%6.0lf", sum_a / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'b': 
			sprintf(vstr, "%6.0lf", ( sum_a - sum_a_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'c': 
			sprintf(vstr, "%6.0lf", ( sum_a - sum_a_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'd': 
			sprintf(vstr, "%6.0lf", m_3 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'e': 
			sprintf(vstr, "%6.0lf", ( m_3 - m_3_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'f': 
			sprintf(vstr, "%6.0lf", ( m_3 - m_3_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'g': 
			sprintf(vstr, "%6.0lf", m_6 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'h': 
			sprintf(vstr, "%6.0lf", ( m_6 - m_6_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'i': 
			sprintf(vstr, "%6.0lf", ( m_6 - m_6_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'j': 
			sprintf(vstr, "%6.0lf", y_1 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'k': 
			sprintf(vstr, "%6.0lf", ( y_1 - y_1_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'l': 
			sprintf(vstr, "%6.0lf", ( y_1 - y_1_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'm': 
			sprintf(vstr, "%6.0lf", y_2 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'n': 
			sprintf(vstr, "%6.0lf", ( y_2 - y_2_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'o': 
			sprintf(vstr, "%6.0lf", ( y_2 - y_2_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'p': 
			sprintf(vstr, "%6.0lf", y_3 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'q': 
			sprintf(vstr, "%6.0lf", ( y_3 - y_3_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'r': 
			sprintf(vstr, "%6.0lf", ( y_3 - y_3_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 's': 
			sprintf(vstr, "%6.0lf", y_5 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 't': 
			sprintf(vstr, "%6.0lf", ( y_5 - y_5_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'u': 
			sprintf(vstr, "%6.0lf", ( y_5 - y_5_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'v': 
			sprintf(vstr, "%6.0lf", y_8 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'w': 
			sprintf(vstr, "%6.0lf", ( y_8 - y_8_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'x': 
			sprintf(vstr, "%6.0lf", ( y_8 - y_8_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}

int get_ratio_dqcxckfxybb_hj( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	
	switch( bli ) 
	{
		case 'a': 
			sprintf(vstr, "%6.0lf", sum_sum_a / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'b': 
			sprintf(vstr, "%6.0lf", ( sum_sum_a - sum_sum_a_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'c': 
			sprintf(vstr, "%6.0lf", ( sum_sum_a - sum_sum_a_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'd': 
			sprintf(vstr, "%6.0lf", sum_m_3 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'e': 
			sprintf(vstr, "%6.0lf", ( sum_m_3 - sum_m_3_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'f': 
			sprintf(vstr, "%6.0lf", ( sum_m_3 - sum_m_3_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'g': 
			sprintf(vstr, "%6.0lf", sum_m_6 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'h': 
			sprintf(vstr, "%6.0lf", ( sum_m_6 - sum_m_6_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'i': 
			sprintf(vstr, "%6.0lf", ( sum_m_6 - sum_m_6_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'j': 
			sprintf(vstr, "%6.0lf", sum_y_1 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'k': 
			sprintf(vstr, "%6.0lf", ( sum_y_1 - sum_y_1_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'l': 
			sprintf(vstr, "%6.0lf", ( sum_y_1 - sum_y_1_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'm': 
			sprintf(vstr, "%6.0lf", sum_y_2 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'n': 
			sprintf(vstr, "%6.0lf", ( sum_y_2 - sum_y_2_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'o': 
			sprintf(vstr, "%6.0lf", ( sum_y_2 - sum_y_2_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'p': 
			sprintf(vstr, "%6.0lf", sum_y_3 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'q': 
			sprintf(vstr, "%6.0lf", ( sum_y_3 - sum_y_3_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'r': 
			sprintf(vstr, "%6.0lf", ( sum_y_3 - sum_y_3_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 's': 
			sprintf(vstr, "%6.0lf", sum_y_5 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 't': 
			sprintf(vstr, "%6.0lf", ( sum_y_5 - sum_y_5_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'u': 
			sprintf(vstr, "%6.0lf", ( sum_y_5 - sum_y_5_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'v': 
			sprintf(vstr, "%6.0lf", sum_y_8 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'w': 
			sprintf(vstr, "%6.0lf", ( sum_y_8 - sum_y_8_sy ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'x': 
			sprintf(vstr, "%6.0lf", ( sum_y_8 - sum_y_8_nc ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}

int get_br_cr_bal( char *br_no_v )
{
	char wherelist[500],wherelist_1[500];

	memset( wherelist_1, 0x00, sizeof( wherelist_1 ) );
	
	vtcp_log("brbrbr_no[%s]",br_no_v);
	if( !strcmp( sComBranch.br_type, "1" ) || !strcmp( sComBranch.br_type, "4" ) )
		sprintf( wherelist_1, "gl_prdt_dyn.prdt_no=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and substr(dc_acc_rel.acc_hrt,1,3)='215' and gl_prdt_dyn.br_no=com_branch.br_no and com_branch.br_no='%s'", sComBranch1.br_no );
	if( !strcmp( sComBranch.br_type, "6" ) )
	{
		if( !strcmp( sComBranch1.up_br_no, TOT_BR_NO ) )    /*如果上级机构是总行只统计自己的*/
			sprintf( wherelist_1, "gl_prdt_dyn.prdt_no=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and substr(dc_acc_rel.acc_hrt,1,3)='215' and gl_prdt_dyn.br_no=com_branch.br_no and com_branch.br_no='%s'", sComBranch1.br_no );
		else
			sprintf( wherelist_1, "gl_prdt_dyn.prdt_no=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and substr(dc_acc_rel.acc_hrt,1,3)='215' and gl_prdt_dyn.br_no=com_branch.br_no and com_branch.up_br_no='%s'", sComBranch1.up_br_no );
	}

	/*取月末余额*/
	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='M' and td_parm.term between 0 and 5", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn.cr_bal", \ 
		&m_3, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='M' and td_parm.term between 6 and 11", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn.cr_bal", \ 
		&m_6, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term=1", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn.cr_bal", \ 
		&y_1, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term=2", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn.cr_bal", \ 
		&y_2, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term between 3 and 4", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn.cr_bal", \ 
		&y_3, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term between 5 and 7", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn.cr_bal", \ 
		&y_5, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term>=8", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn.cr_bal", \ 
		&y_8, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = sql_sum_double( "gl_prdt_dyn,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn.cr_bal", \ 
		&sum_a, wherelist_1 );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	/*取上月值*/
	memset( wherelist_1, 0x00, sizeof( wherelist_1 ) );
	
	vtcp_log("brbrbr_no[%s]",br_no_v);
	if( !strcmp( sComBranch.br_type, "1" ) || !strcmp( sComBranch.br_type, "4" ) )
		sprintf( wherelist_1, "gl_prdt_dyn_hst.\"date\"=20030831 and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and substr(dc_acc_rel.acc_hrt,1,3)='215' and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.br_no='%s'", sComBranch1.br_no );
	if( !strcmp( sComBranch.br_type, "6" ) )
	{
		if( !strcmp( sComBranch1.up_br_no, TOT_BR_NO ) )    /*如果上级机构是总行只统计自己的*/
			sprintf( wherelist_1, "gl_prdt_dyn_hst.date=20030831 and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='215' and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.br_no='%s'", sComBranch1.br_no );
		else
			sprintf( wherelist_1, "gl_prdt_dyn_hst.date=20030831 and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='215' and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s'", sComBranch1.up_br_no );
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='M' and td_parm.term between 0 and 5", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn_hst.cr_bal", \ 
		&m_3_sy, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='M' and td_parm.term between 6 and 11", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn_hst.cr_bal", \ 
		&m_6_sy, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term=1", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn_hst.cr_bal", \ 
		&y_1_sy, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term=2", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn_hst.cr_bal", \ 
		&y_2_sy, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term between 3 and 4", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn_hst.cr_bal", \ 
		&y_3_sy, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term between 5 and 7", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn_hst.cr_bal", \ 
		&y_5_sy, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( wherelist, 0x00, sizeof( wherelist ) );
	sprintf( wherelist, "%s and td_parm.term_type='Y' and td_parm.term>=8", wherelist_1 );
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn_hst.cr_bal", \ 
		&y_8_sy, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm,com_branch", "gl_prdt_dyn_hst.cr_bal", \ 
		&sum_a_sy, wherelist_1 );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	return 0;

ErrExit:
	return -1;
}
