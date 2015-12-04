/*************************************************************
* 文 件 名: rpt037.c
* 功能描述：存款日平均余额月报表（支行）
*
* 作    者: jane
* 完成日期: 2003年9月16日
*
* 注：格式文件为ckyeybb.tbl
*	  从gl_prdt_dyn_hst统计
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
static double bal_201;
static double bal_205;
static double bal_20201;
static double bal_211_dd,bal_211_td;
static double bal_215;
static double bal_20202;
static double bal_235;
static double bal_243;
static double bal_25101,bal_25102;
static double bal_234;
static double de_bal_sum;    /*存款合计*/
static int date_cnt;    /*天数*/
static long date_min,date_max;    /*天数*/

int get_ratio_ckyerbb(int bli,int bll,char str[100]);
rpt037()
{
	int line, opncnt;
	char vpfm[21];
	char fname[10];
	char pre_br_type[2];
	char pre_br_no[6];
	char pre_up_br_no[6];
	FILE *fp; 
	
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );
	memset( pre_up_br_no, 0x00, sizeof( pre_up_br_no ) );

    g_reply_int = sql_begin(); /*打开事务*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
	strcpy( fname, "BMCKYEYBB" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除存款余额日报表错误!!");
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

	date_min = sComSysParm.lst_date / 100;
	date_min = date_min * 100;
	date_max = date_min + 99;

	date_cnt = count_distinct( "gl_prdt_dyn_hst", "date", "\"date\">%ld and \"date\"<%ld", date_min, date_max );
	vtcp_log( "date_cnt[%ld]", date_cnt );

	strcpy(vpfm, "ckyeybb");

	/*清算中心和管理支行营业部*/
	/*g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "br_type in ('1','4') ORDER BY br_no" );lwb 20150525  加入分行   */
	g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "br_type in ('1','4','3') ORDER BY br_no" );
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
			if( pre_br_type[0] == '1' )
				g_reply_int = get_br_cr_bal( pre_up_br_no );
			if( pre_br_type[0] == '4' )
				g_reply_int = get_up_br_cr_bal( pre_up_br_no );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"取[%s]机构各项存款余额出错", sComBranch.up_br_no );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( sComBranch1.br_name, "        合计：  " );

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "AA", &opncnt, get_ratio_ckyerbb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM AA error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "ZZ", &opncnt, get_ratio_ckyerbb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM ZZ error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			fclose( fp );
			break;
		}

		/*清算中心*/
		if( sComBranch.br_type[0] == '1' )
			strcpy( sComBranch.up_br_no, sComBranch.br_no );

		/*机构号变,下一个文件*/
		if( strcmp( sComBranch.br_no, pre_br_no ) )
		{
			/*不是第一个机构,打上个机构的结尾*/
			if( strlen( pre_br_no ) )
			{
				if( pre_br_type[0] == '1' )
					g_reply_int = get_br_cr_bal( pre_up_br_no );
				if( pre_br_type[0] == '4' )
					g_reply_int = get_up_br_cr_bal( pre_up_br_no );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"取[%s]机构各项存款余额出错", pre_up_br_no );
					WRITEMSG
					goto ErrExit;
				}

				strcpy( sComBranch1.br_name, "        合计：  " );

				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","AA",&opncnt,get_ratio_ckyerbb);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM AA error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}

				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","ZZ",&opncnt,get_ratio_ckyerbb);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM ZZ error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
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

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_ckyerbb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_type, sComBranch.br_type );
			strcpy( pre_br_no, sComBranch.br_no );
			strcpy( pre_up_br_no, sComBranch.up_br_no );
		}

		/*清算中心*/
		if( sComBranch.br_type[0] == '1' )
		{
			g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "br_no='%s'", sComBranch.br_no );
			vtcp_log("清算中心[%s]",sComBranch.br_no);
		}
		else if( sComBranch.br_type[0] == '4' )
		{
			if( !strcmp( sComBranch.up_br_no, "61000" ) )
				g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "br_no='%s' ORDER BY br_no", \
					sComBranch.br_no );
			else
				g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "up_br_no='%s' ORDER BY br_no", \
					sComBranch.up_br_no, sComBranch.up_br_no );
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

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "AA", &opncnt, get_ratio_ckyerbb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM AA error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_ckyerbb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		}
		Com_branch_Clo_Sel1();
	}
	
	Com_branch_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成存款余额日报表成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成存款余额日报表失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_ckyerbb( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	
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
		case 'v': 
			pub_base_strpack( sComBranch1.br_name );
			sprintf(vstr, "%s", sComBranch1.br_name );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'a': 
			sprintf(vstr, "%8.0lf", bal_201 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'b': 
			sprintf(vstr, "%8.0lf", bal_205 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'c': 
			sprintf(vstr, "%8.0lf", bal_20201 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'd': 
			sprintf(vstr, "%8.0lf", ( bal_211_dd + bal_211_td ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'e': 
			sprintf(vstr, "%8.0lf", bal_215 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'f': 
			sprintf(vstr, "%8.0lf", bal_20202 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'g': 
			sprintf(vstr, "%8.0lf", bal_235 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'h': 
			sprintf(vstr, "%8.0lf", bal_243 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'i': 
			sprintf(vstr, "%8.0lf", ( bal_25101 + bal_25102 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'j': 
			sprintf(vstr, "%8.0lf", de_bal_sum / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'k': 
			sprintf(vstr, "%8.0lf", bal_234 / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}

/*把机构各项余额和存款合计取到对应变量里*/
int get_br_cr_bal( char *br_no_v )
{
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", &bal_201, \ 
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='201'", \ 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_201 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", &bal_205, \ 
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='205'", \ 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_205 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", &bal_20201, 
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt='20201'", \ 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_20201 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", &bal_211_dd,
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='211'", 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_211_dd /= date_cnt;
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", &bal_211_td,
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='211'", 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_211_td /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", &bal_215, \ 
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='215'", 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_215 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", &bal_20202,
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt='20202'", 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_20202 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", &bal_235, \ 
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='235'", 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_235 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", &bal_243, \ 
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='243'", 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_243 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", &bal_25101, 
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt='25101'", 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_25101 /= date_cnt;
	g_reply_int = sql_sum_double( "mo_jl_bal,dd_mst,dd_parm,dc_acc_rel", "mo_jl_bal.bal", &bal_25102, "mo_jl_bal.beg_date between %ld and %ld and mo_jl_bal.ac_id=dd_mst.ac_id and mo_jl_bal.ac_seqn=dd_mst.ac_seqn and dd_mst.cal_code='%s' and dd_mst.prdt_no=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt='25102'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM dd_mst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_25102 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", &bal_234, \ 
		"gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='234'", \ 
		date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_234 /= date_cnt;

	de_bal_sum = bal_201 + bal_205 + \ 
		bal_20201 + bal_211_dd + bal_211_td + bal_215 + \ 
		bal_20202 + bal_235 + bal_243 + bal_25101 + bal_25102;

	return 0;

ErrExit:
	return -1;
}

/*把机构合计各项余额取到对应变量里*/
int get_up_br_cr_bal( char *br_no_v )
{
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_201, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='201'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_201 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_205, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='205'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_205 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_20201, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt='20201'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_20201 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_211_dd, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='211'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_211_dd /= date_cnt;
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_211_td, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='211'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_211_td /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_215, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='215'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_215 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,td_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_20202, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no and td_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt='20202'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_20202 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_235, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='235'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_235 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_243, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='243'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_243 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", \ 
	&bal_25101, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt='25101'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_25101 /= date_cnt;
	g_reply_int = sql_sum_double( "mo_jl_bal,dd_mst,dd_parm,dc_acc_rel,com_branch", "mo_jl_bal.bal", &bal_25102, "mo_jl_bal.beg_date between %ld and %ld and mo_jl_bal.ac_id=dd_mst.ac_id and mo_jl_bal.ac_seqn=dd_mst.ac_seqn and dd_mst.cal_code=com_branch.br_no and com_branch.up_br_no='%s' and dd_mst.prdt_no=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt='25102'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM dd_mst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_25102 /= date_cnt;

	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,com_branch,dc_acc_rel,dd_parm", "gl_prdt_dyn_hst.cr_bal", 
	&bal_234, "gl_prdt_dyn_hst.date between %ld and %ld and gl_prdt_dyn_hst.br_no=com_branch.br_no and com_branch.up_br_no='%s' and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no and dd_parm.dc_code=dc_acc_rel.dc_code and dc_acc_rel.acc_hrt[1,3]='234'", date_min, date_max, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SUM gl_prdt_dyn_hst error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	bal_234 /= date_cnt;

	de_bal_sum = bal_201 + bal_205 + \ 
		bal_20201 + bal_211_dd + bal_211_td + bal_215 + \ 
		bal_20202 + bal_235 + bal_243 + bal_25101 + bal_25102;

	return 0;

ErrExit:
	return -1;
}
