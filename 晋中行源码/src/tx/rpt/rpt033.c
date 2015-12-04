/*************************************************************
* 文 件 名: rpt033.c
* 功能描述：主要业务状况日报表（支行）
*
* 作    者: jane
* 完成日期: 2003年9月15日
*
* 注：格式文件为zyywzkrbb.tbl
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
#include "gl_oprt_dyn_c.h"

static struct com_sys_parm_c sComSysParm;
static struct com_branch_c sComBranch;
static struct com_branch_c sComBranch1;
static struct gl_oprt_dyn_c sGlOprtDyn;
static struct gl_oprt_dyn_c sGlOprtDyn_sum;

int get_ratio_zyywzkrbb(int bli,int bll,char str[100]);
rpt033()
{
	int line, opncnt;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	short iii=0;
	
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
    
	strcpy( fname, "BRZYYWZKRBB" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除主要业务状况日报表错误!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	g_reply_int = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "sys_date=%ld" , g_pub_tx.tx_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}

	strcpy(vpfm, "zyywzkrbb");

	/*g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "br_type in ('1','4') ORDER BY up_br_no" );20150525 lwb  增加分行*/
	g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "br_type in ('1','4','3') ORDER BY up_br_no" );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE com_branch CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}

	iii=1;
	while(1) 
	{ 
		memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
		vtcp_log("第%d次",iii);
		g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH com_branch CUR error![%d][%s][%d]", g_reply_int,__FILE__,__LINE__ );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100 )
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "ZZ", &opncnt, get_ratio_zyywzkrbb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM ZZ error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			fclose( fp );
			break;
		}

		/*机构号变,下一个文件*/
		if( strcmp( sComBranch.br_no, pre_br_no ) )
		{
			/*不是第一个机构,打上个机构的结尾*/
			if( strlen( pre_br_no ) )
			{
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","ZZ",&opncnt,get_ratio_zyywzkrbb);
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

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_zyywzkrbb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_no, sComBranch.br_no );
			memset( &sGlOprtDyn_sum, 0x00, sizeof( struct gl_oprt_dyn_c ) );    /*合计值*/
		}
		/*存款部分*/
	if( !strcmp( sComBranch.up_br_no, TOT_BR_NO ) )
			g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "br_no='%s' or up_br_no='%s' ORDER BY br_no", \
				sComBranch.br_no, sComBranch.br_no );
		else
			g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "br_no='%s' or up_br_no='%s' ORDER BY br_no", \
				sComBranch.up_br_no, sComBranch.up_br_no );
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
				sprintf( acErrMsg,"FETCH com_branch CUR1 error![%d][%s][%d]", g_reply_int,__FILE__,__LINE__ );
				WRITEMSG
				goto ErrExit;
			}
			else if( g_reply_int == 100 )
			{
				g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", \
					&opncnt, get_ratio_zyywzkrbb );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				break;
			}

			memset( &sGlOprtDyn, 0x00, sizeof( struct gl_oprt_dyn_c ) );
			g_reply_int = Gl_oprt_dyn_Sel( g_pub_tx.reply, &sGlOprtDyn, \ 
				"\"date\"='%ld' and br_no='%s'", sComSysParm.lst_date, sComBranch1.br_no );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"SELECT gl_oprt_dyn error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			sGlOprtDyn_sum.cr_de_bal += sGlOprtDyn.cr_de_bal;
			sGlOprtDyn_sum.cr_de_bal1 += sGlOprtDyn.cr_de_bal1;
			sGlOprtDyn_sum.cr_de_bal2 += sGlOprtDyn.cr_de_bal2;
			sGlOprtDyn_sum.cr_de_bal3 += sGlOprtDyn.cr_de_bal3;
			sGlOprtDyn_sum.cr_de_bal4 += sGlOprtDyn.cr_de_bal4;
			sGlOprtDyn_sum.cr_de_bal5 += sGlOprtDyn.cr_de_bal5;
			sGlOprtDyn_sum.cr_de_bal6 += sGlOprtDyn.cr_de_bal6;
			sGlOprtDyn_sum.ys_de_bal += sGlOprtDyn.ys_de_bal;
			sGlOprtDyn_sum.ys_de_bal1 += sGlOprtDyn.ys_de_bal1;
			sGlOprtDyn_sum.ys_de_bal2 += sGlOprtDyn.ys_de_bal2;
			sGlOprtDyn_sum.ys_de_bal3 += sGlOprtDyn.ys_de_bal3;
			sGlOprtDyn_sum.ys_de_bal4 += sGlOprtDyn.ys_de_bal4;
			sGlOprtDyn_sum.ys_de_bal5 += sGlOprtDyn.ys_de_bal5;
			sGlOprtDyn_sum.ys_de_bal6 += sGlOprtDyn.ys_de_bal6;
			sGlOprtDyn_sum.yb_de_bal += sGlOprtDyn.yb_de_bal;
			sGlOprtDyn_sum.yb_de_bal1 += sGlOprtDyn.yb_de_bal1;
			sGlOprtDyn_sum.yb_de_bal2 += sGlOprtDyn.yb_de_bal2;
			sGlOprtDyn_sum.yb_de_bal3 += sGlOprtDyn.yb_de_bal3;
			sGlOprtDyn_sum.yb_de_bal4 += sGlOprtDyn.yb_de_bal4;
			sGlOprtDyn_sum.yb_de_bal5 += sGlOprtDyn.yb_de_bal5;
			sGlOprtDyn_sum.yb_de_bal6 += sGlOprtDyn.yb_de_bal6;
			if( !strcmp( sComBranch.up_br_no, sComBranch1.br_no ) )
				continue;
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "AA", &opncnt, get_ratio_zyywzkrbb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM AA error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		}
		Com_branch_Clo_Sel1();
		/*贷款部分*/
		if( !strcmp( sComBranch.up_br_no, TOT_BR_NO ) )
			g_reply_int = Com_branch_Dec_Sel2( g_pub_tx.reply, "br_no='%s' or up_br_no='%s' ORDER BY br_no", \
				sComBranch.br_no, sComBranch.br_no );
		else
			g_reply_int = Com_branch_Dec_Sel2( g_pub_tx.reply, "br_no='%s' or up_br_no='%s' ORDER BY br_no", sComBranch.up_br_no, sComBranch.up_br_no );
		if( g_reply_int ) 
		{ 
			sprintf(acErrMsg, "DECLARE com_branch2 CUR error![%d]", g_reply_int ); 
			WRITEMSG 
			goto ErrExit; 
		} 
		while(1)
		{
			memset( &sComBranch1, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel2( &sComBranch1, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			else if( g_reply_int )
			{
				sprintf( acErrMsg,"FETCH com_branch CUR2 error![%d][%s][%d]", g_reply_int,__FILE__,__LINE__ );
				WRITEMSG
				goto ErrExit;
			}

			memset( &sGlOprtDyn, 0x00, sizeof( struct gl_oprt_dyn_c ) );
			g_reply_int = Gl_oprt_dyn_Sel( g_pub_tx.reply, &sGlOprtDyn, \ 
				"\"date\"='%ld' and br_no='%s'", sComSysParm.lst_date, sComBranch1.br_no );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"SELECT gl_oprt_dyn error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			/*所有贷款余额为零就不出该机构的*/
			if( !pub_base_CompDblVal( sGlOprtDyn.cr_ln_bal, 0.00 ) &&
				!pub_base_CompDblVal( sGlOprtDyn.ys_ln_bal, 0.00 ) &&
				!pub_base_CompDblVal( sGlOprtDyn.yb_ln_bal, 0.00 ) &&
				!pub_base_CompDblVal( sGlOprtDyn.cr_ln_bal5, 0.00 ) &&
				!pub_base_CompDblVal( sGlOprtDyn.ys_ln_bal5, 0.00 ) &&
				!pub_base_CompDblVal( sGlOprtDyn.yb_ln_bal5, 0.00 ) )
				continue;

			sGlOprtDyn_sum.cr_ln_bal += sGlOprtDyn.cr_ln_bal;
			sGlOprtDyn_sum.cr_ln_bal1 += sGlOprtDyn.cr_ln_bal1;
			sGlOprtDyn_sum.cr_ln_bal2 += sGlOprtDyn.cr_ln_bal2;
			sGlOprtDyn_sum.cr_ln_bal3 += sGlOprtDyn.cr_ln_bal3;
			sGlOprtDyn_sum.cr_ln_bal4 += sGlOprtDyn.cr_ln_bal4;
			sGlOprtDyn_sum.cr_ln_bal5 += sGlOprtDyn.cr_ln_bal5;
			sGlOprtDyn_sum.ys_ln_bal += sGlOprtDyn.ys_ln_bal;
			sGlOprtDyn_sum.ys_ln_bal1 += sGlOprtDyn.ys_ln_bal1;
			sGlOprtDyn_sum.ys_ln_bal2 += sGlOprtDyn.ys_ln_bal2;
			sGlOprtDyn_sum.ys_ln_bal3 += sGlOprtDyn.ys_ln_bal3;
			sGlOprtDyn_sum.ys_ln_bal4 += sGlOprtDyn.ys_ln_bal4;
			sGlOprtDyn_sum.ys_ln_bal5 += sGlOprtDyn.ys_ln_bal5;
			sGlOprtDyn_sum.yb_ln_bal += sGlOprtDyn.yb_ln_bal;
			sGlOprtDyn_sum.yb_ln_bal1 += sGlOprtDyn.yb_ln_bal1;
			sGlOprtDyn_sum.yb_ln_bal2 += sGlOprtDyn.yb_ln_bal2;
			sGlOprtDyn_sum.yb_ln_bal3 += sGlOprtDyn.yb_ln_bal3;
			sGlOprtDyn_sum.yb_ln_bal4 += sGlOprtDyn.yb_ln_bal4;
			sGlOprtDyn_sum.yb_ln_bal5 += sGlOprtDyn.yb_ln_bal5;
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt, get_ratio_zyywzkrbb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		}
		Com_branch_Clo_Sel2();
		iii++;
	} 
	
	Com_branch_Clo_Sel();

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
		case 'V': 
			pub_base_strpack( sComBranch1.br_name );
			sprintf(vstr, "%s", sComBranch1.br_name );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'a': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_de_bal / 10000, \
				( sGlOprtDyn.cr_de_bal - sGlOprtDyn.ys_de_bal ) / 10000, \
				( sGlOprtDyn.cr_de_bal - sGlOprtDyn.yb_de_bal ) / 10000 );
vtcp_log( "zyywzk_aaaaaa[%s]", vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
vtcp_log( "aaaaaa_str[%s]", str );
			break; 
		case 'b': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_de_bal1 / 10000, \
				( sGlOprtDyn.cr_de_bal1 - sGlOprtDyn.ys_de_bal1 ) / 10000, \
				( sGlOprtDyn.cr_de_bal1 - sGlOprtDyn.yb_de_bal1 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'c': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_de_bal2 / 10000, \
				( sGlOprtDyn.cr_de_bal2 - sGlOprtDyn.ys_de_bal2 ) / 10000, \
				( sGlOprtDyn.cr_de_bal2 - sGlOprtDyn.yb_de_bal2 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'd': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_de_bal3 / 10000, \
				( sGlOprtDyn.cr_de_bal3 - sGlOprtDyn.ys_de_bal3 ) / 10000, \
				( sGlOprtDyn.cr_de_bal3 - sGlOprtDyn.yb_de_bal3 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'e': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_de_bal4 / 10000, \
				( sGlOprtDyn.cr_de_bal4 - sGlOprtDyn.ys_de_bal4 ) / 10000, \
				( sGlOprtDyn.cr_de_bal4 - sGlOprtDyn.yb_de_bal4 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'f': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_de_bal5 / 10000, \
				( sGlOprtDyn.cr_de_bal5 - sGlOprtDyn.ys_de_bal5 ) / 10000, \
				( sGlOprtDyn.cr_de_bal5 - sGlOprtDyn.yb_de_bal5 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'g': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_de_bal6 / 10000, \
				( sGlOprtDyn.cr_de_bal6 - sGlOprtDyn.ys_de_bal6 ) / 10000, \
				( sGlOprtDyn.cr_de_bal6 - sGlOprtDyn.yb_de_bal6 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'h': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_de_bal / 10000, \
				( sGlOprtDyn_sum.cr_de_bal - sGlOprtDyn_sum.ys_de_bal ) / 10000, \
				( sGlOprtDyn_sum.cr_de_bal - sGlOprtDyn_sum.yb_de_bal ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'i': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_de_bal1 / 10000, \
				( sGlOprtDyn_sum.cr_de_bal1 - sGlOprtDyn_sum.ys_de_bal1 ) / 10000, \
				( sGlOprtDyn_sum.cr_de_bal1 - sGlOprtDyn_sum.yb_de_bal1 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'j': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_de_bal2 / 10000, \
				( sGlOprtDyn_sum.cr_de_bal2 - sGlOprtDyn_sum.ys_de_bal2 ) / 10000, \
				( sGlOprtDyn_sum.cr_de_bal2 - sGlOprtDyn_sum.yb_de_bal2 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'k': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_de_bal3 / 10000, \
				( sGlOprtDyn_sum.cr_de_bal3 - sGlOprtDyn_sum.ys_de_bal3 ) / 10000, \
				( sGlOprtDyn_sum.cr_de_bal3 - sGlOprtDyn_sum.yb_de_bal3 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'l': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_de_bal4 / 10000, \
				( sGlOprtDyn_sum.cr_de_bal4 - sGlOprtDyn_sum.ys_de_bal4 ) / 10000, \
				( sGlOprtDyn_sum.cr_de_bal4 - sGlOprtDyn_sum.yb_de_bal4 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'm': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_de_bal5 / 10000, \
				( sGlOprtDyn_sum.cr_de_bal5 - sGlOprtDyn_sum.ys_de_bal5 ) / 10000, \
				( sGlOprtDyn_sum.cr_de_bal5 - sGlOprtDyn_sum.yb_de_bal5 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'n': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_de_bal6 / 10000, \
				( sGlOprtDyn_sum.cr_de_bal6 - sGlOprtDyn_sum.ys_de_bal6 ) / 10000, \
				( sGlOprtDyn_sum.cr_de_bal6 - sGlOprtDyn_sum.yb_de_bal6 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'A': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_ln_bal / 10000, \
				( sGlOprtDyn.cr_ln_bal - sGlOprtDyn.ys_ln_bal ) / 10000, \
				( sGlOprtDyn.cr_ln_bal - sGlOprtDyn.yb_ln_bal ) / 10000 );
vtcp_log( "zyywzk_aaaaaa[%s]", vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
vtcp_log( "aaaaaa_str[%s]", str );
			break; 
		case 'B': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_ln_bal1 / 10000, \
				( sGlOprtDyn.cr_ln_bal1 - sGlOprtDyn.ys_ln_bal1 ) / 10000, \
				( sGlOprtDyn.cr_ln_bal1 - sGlOprtDyn.yb_ln_bal1 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'C': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_ln_bal2 / 10000, \
				( sGlOprtDyn.cr_ln_bal2 - sGlOprtDyn.ys_ln_bal2 ) / 10000, \
				( sGlOprtDyn.cr_ln_bal2 - sGlOprtDyn.yb_ln_bal2 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'D': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_ln_bal3 / 10000, \
				( sGlOprtDyn.cr_ln_bal3 - sGlOprtDyn.ys_ln_bal3 ) / 10000, \
				( sGlOprtDyn.cr_ln_bal3 - sGlOprtDyn.yb_ln_bal3 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'E': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_ln_bal4 / 10000, \
				( sGlOprtDyn.cr_ln_bal4 - sGlOprtDyn.ys_ln_bal4 ) / 10000, \
				( sGlOprtDyn.cr_ln_bal4 - sGlOprtDyn.yb_ln_bal4 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'F': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn.cr_ln_bal5 / 10000, \
				( sGlOprtDyn.cr_ln_bal5 - sGlOprtDyn.ys_ln_bal5 ) / 10000, \
				( sGlOprtDyn.cr_ln_bal5 - sGlOprtDyn.yb_ln_bal5 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'H': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_ln_bal / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal - sGlOprtDyn_sum.ys_ln_bal ) / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal - sGlOprtDyn_sum.yb_ln_bal ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'I': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_ln_bal1 / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal1 - sGlOprtDyn_sum.ys_ln_bal1 ) / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal1 - sGlOprtDyn_sum.yb_ln_bal1 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'J': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_ln_bal2 / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal2 - sGlOprtDyn_sum.ys_ln_bal2 ) / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal2 - sGlOprtDyn_sum.yb_ln_bal2 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'K': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_ln_bal3 / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal3 - sGlOprtDyn_sum.ys_ln_bal3 ) / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal3 - sGlOprtDyn_sum.yb_ln_bal3 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'L': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_ln_bal4 / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal4 - sGlOprtDyn_sum.ys_ln_bal4 ) / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal4 - sGlOprtDyn_sum.yb_ln_bal4 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'M': 
			sprintf(vstr, "%6.0lf|%6.0lf|%6.0lf", sGlOprtDyn_sum.cr_ln_bal5 / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal5 - sGlOprtDyn_sum.ys_ln_bal5 ) / 10000, \
				( sGlOprtDyn_sum.cr_ln_bal5 - sGlOprtDyn_sum.yb_ln_bal5 ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}
