/*************************************************************
* 文 件 名: rpt059.c
* 功能描述：大额滞留往账业务
*
* 作    者: jane
* 完成日期: 2003年10月31日
*
* 注：格式文件为mbfesh.tbl( sd 为 send hang 缩写 )
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
#include "mbfeghvsendlist_c.h"
#include "mbfebrbkno_c.h"

static int page,total;
static struct mbfeghvsendlist_c ghvsendlist_v;
static struct com_sys_parm_c sComSysParm;
static struct mbfebrbkno_c sMbfebrbkno;

int get_ratio_mbfesh(int bli,int bll,char str[100]);
rpt059()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	
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
    
	strcpy( fname, "BRmbfesh" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除大额滞留往账业务清单错误!!");
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

	strcpy(vpfm, "mbfesh");

	g_reply_int = Mbfeghvsendlist_Dec_Sel(g_pub_tx.reply, \
		"procstate[1]='3' and consigndate>20030827 ORDER BY cnapsodfi,consigndate" );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE mbfeghvsendlist CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	while(1) 
	{ 
		g_reply_int = Mbfeghvsendlist_Fet_Sel( &ghvsendlist_v, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH mbfeghvsendlist CUR error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100 )
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt, get_ratio_mbfesh );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			fclose( fp );
			break;
		}

		g_reply_int = Mbfebrbkno_Sel( g_pub_tx.reply, &sMbfebrbkno, "bkno='%s'", &ghvsendlist_v.cnapsodfi );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"取发起机构号错[%s][%d]", &ghvsendlist_v.cnapsodfi, g_reply_int );
			WRITEMSG
			goto ErrExit;
		}

		/*机构号变,下一个文件*/
		if( strcmp( sMbfebrbkno.brno, pre_br_no ) )
		{
			/*不是第一个机构,打上个机构的结尾*/
			if( strlen( pre_br_no ) )
			{
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesh);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				fclose( fp );
			}

			g_reply_int = pub_rpt_openfile( &fp, sMbfebrbkno.brno, fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			g_reply_int = sql_count( "mbfeghvsendlist", \
				"cnapsodfi='%s' and procstate='3'", sMbfebrbkno.bkno );

			total = g_reply_int / 25 + 1;
			hy_line = 0;
			page = 1;

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_mbfesh );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_no, sMbfebrbkno.brno );
		}

		g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_mbfesh );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		hy_line++;
			
		if( hy_line % 25 == 0 )
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesh);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			hy_line = 0;
			page++;

			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesh);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		}
	} 
	
	Mbfeghvsendlist_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成大额滞留业务清单成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成大额滞留业务清单失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_mbfesh( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	struct com_branch_c sComBranch;
	
	switch( bli ) 
	{ 
		case 'A': 
			sprintf( vstr, "%08ld", ghvsendlist_v.consigndate ); 
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'E': 
			strcpy( str, ghvsendlist_v.txssno ); 
			break; 
		case 'B': 
			strcpy( str, ghvsendlist_v.payeracc ); 
			break; 
		case 'C': 
			strcpy( str, ghvsendlist_v.payername ); 
			break; 
		case 'D': 
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, ghvsendlist_v.amount );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'T': 
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", sMbfebrbkno.brno );
			if( g_reply_int ) 
			{ 
				sprintf(acErrMsg, "SELECT com_branch error![%d]", g_reply_int ); 
				WRITEMSG 
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s（%s）", sComBranch.br_name, sComBranch.br_no );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'X': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'W': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'V': 
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'U': 
			sprintf( str, "%02ld", page ); 
			break; 
		case 'Z': 
			sprintf( str, "%02ld", total ); 
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}
