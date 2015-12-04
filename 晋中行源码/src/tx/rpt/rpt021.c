/*************************************************************
* 文 件 名: rpt021.c
* 功能描述：大额来账挂帐未处理业务清单
*
* 作    者: jane
* 完成日期: 2003年8月17日
*
* 注：格式文件为mbferh.tbl( rh 为 recv hang 缩写 )
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
#include "mbfeghvrecvlist_c.h"
#include "mbfebrbkno_c.h"

static int page,total;
static struct mbfeghvrecvlist_c ghvrecvlist_v;
static struct mbfebrbkno_c brbkno_v;
static struct com_sys_parm_c sComSysParm;
static struct dic_data_c sDicData;
static char proc_state[30];    /*挂账原因*/

int get_ratio_mbferh(int bli,int bll,char str[100]);
rpt021()
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
    
	strcpy( fname, "BRmbferh" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除大额未处理来帐挂账业务清单错误!!");
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

	strcpy(vpfm, "mbferh");

	g_reply_int = Mbfeghvrecvlist_Dec_Sel(g_pub_tx.reply, "procstate[1]='5' ORDER BY cnapsrdfi,rectime" );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE mbfeghvrecvlist CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	while(1) 
	{ 
		g_reply_int = Mbfeghvrecvlist_Fet_Sel( &ghvrecvlist_v, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH mbfeghvrecvlist CUR error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100 )
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt, get_ratio_mbferh );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			fclose( fp );
			break;
		}

		/*取行号对应机构号*/
		memset( &brbkno_v, 0x00, sizeof( struct mbfebrbkno_c ) );
		g_reply_int = Mbfebrbkno_Sel( g_pub_tx.reply, &brbkno_v, "bkno='%s'", ghvrecvlist_v.cnapsrdfi );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"取来帐接收行号对应机构号错![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}

		/*机构号变,下一个文件*/
		if( strcmp( brbkno_v.brno, pre_br_no ) )
		{
			/*不是第一个机构,打上个机构的结尾*/
			if( strlen( pre_br_no ) )
			{
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbferh);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				fclose( fp );
			}

			g_reply_int = pub_rpt_openfile( &fp, brbkno_v.brno , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			g_reply_int = sql_count( "mbfeghvrecvlist", \
				"cnapsrdfi='%s' and procstate[1]='5'", ghvrecvlist_v.cnapsrdfi );
			total = g_reply_int / 25 + 1;    /*计算总页数*/

			hy_line = 0;
			page = 1;

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_mbferh );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_no, brbkno_v.brno );
		}

		/*挂账原因*/
		memset( &sDicData, 0x00, sizeof( struct dic_data_c ) );
		g_reply_int = Dic_data_Sel( g_pub_tx.reply, &sDicData, \
			"pr_code='s_procstate' and pr_seq='%s'", ghvrecvlist_v.procstate );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SELECT tx_def error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		memset( proc_state, 0x00, sizeof( proc_state ) );
		strcpy( proc_state, sDicData.content + 5 );
		pub_base_strpack( proc_state );
		proc_state[ strlen( proc_state ) - 1 ] = '\0';    /*去掉左右括号*/
		
		g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_mbferh );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		hy_line++;
			
		if( hy_line % 25 == 0 )
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbferh);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			hy_line = 0;
			page++;

			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbferh);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		}
	} 
	
	Mbfeghvrecvlist_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成大额未汇出往账清单成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成大额未汇出往账清单失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_mbferh( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	struct com_branch_c sComBranch;
	
	switch( bli ) 
	{ 
		case 'A': 
			sprintf( str, "%08ld", ghvrecvlist_v.consigndate ); 
			break; 
		case 'B': 
			sprintf( str, "%06ld", ghvrecvlist_v.flowno ); 
			break; 
		case 'C': 
			sprintf( fmt, "%%%ds", bll ); 
			sprintf( vstr, fmt, proc_state );
			strcpy( str, vstr ); 
			break; 
		case 'D': 
			strcpy( str, ghvrecvlist_v.recipientacc ); 
			break; 
		case 'E': 
			strcpy( str, ghvrecvlist_v.recipientname ); 
			break; 
		case 'F':
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, ghvrecvlist_v.amount );/*用带逗号的金额*/
			strcpy( str, vstr ); 
			break; 
		case 'T': 
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", brbkno_v.brno );
			if( g_reply_int ) 
			{ 
				sprintf(acErrMsg, "SELECT com_branch error![%d]", g_reply_int ); 
				WRITEMSG 
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s（%s）", sComBranch.br_name, brbkno_v.bkno );
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
