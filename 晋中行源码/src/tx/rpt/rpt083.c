/*************************************************************
* 文 件 名: rpt083.c
* 功能描述：承兑汇票多余保证金清单
*
* 作    者: lance
* 完成日期: 2003年12月09日
*
* 注：格式文件为cdhpdybzjqd.tbl
*     只清算中心出
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
#include "tx_def_c.h"
#include "com_branch_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"

static int page,total;
static struct mo_bank_acc_po_c sMoBankAccPo;
static struct com_sys_parm_c sComSysParm;

double  tol_amt=0.00;
double  tol_bz_amt=0.00;
double	bail_amt_ratio=0.00;
double	po_amt=0.00;
double	bail_amt=0.00;

int get_ratio_cdhpdybzjqd(int bli,int bll,char str[100]);
rpt083()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int cntv=0;
	
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
    
	strcpy( fname, "BRcdhpdybzjqd" );
	strcpy( fname, "RPT083");
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除承兑汇票多余保证金清单错误!!");
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

	strcpy(vpfm, "cdhpdybzjqd");
	strcpy(vpfm, "RPT083");
	/****若表中无数据，则不打印此报表****/
	g_reply_int = sql_count( "mo_bank_acc_po", "acc_po_sts  not in ('3')  and acc_type in ('1','4','3')");
	if(g_reply_int == 0)
	{
		sprintf( acErrMsg,"数据库中无记录,不用打印此报表![%s]error![%d]", fname, g_reply_int );
		WRITEMSG
		goto OkExit;
	}

	g_reply_int = pub_rpt_openfile( &fp, QS_BR_NO, fname );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	hy_line = 0;
	page = 1;

	g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_cdhpdybzjqd );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "acc_po_sts  not in ('3')  and acc_type in ('1','4','3') ORDER BY pact_no" );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"mo_bank_acc_po Declare Cursor error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		g_reply_int = Mo_bank_acc_po_Fet_Sel( &sMoBankAccPo, g_pub_tx.reply );
		if( g_reply_int == 100 )
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt, get_ratio_cdhpdybzjqd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "FF", &opncnt, get_ratio_cdhpdybzjqd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "EE", &opncnt, get_ratio_cdhpdybzjqd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}						
			fclose( fp );
			break;
		}
		else if( g_reply_int )
		{
			sprintf( acErrMsg,"mo_bank_acc_po Declare Cursor error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		
		bail_amt_ratio=sMoBankAccPo.bail_amt_ratio/100.00;
		po_amt=sMoBankAccPo.sign_amt;
		bail_amt=sMoBankAccPo.bail_amt;
		
		if( pub_base_CompDblVal( bail_amt_ratio*po_amt , bail_amt )==0 )
		{
			bail_amt_ratio=0.00;
			po_amt=0.00;
			bail_amt=0.00;
			continue;
		}else{
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_cdhpdybzjqd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			
			tol_amt+=bail_amt;
			tol_bz_amt+=bail_amt-bail_amt_ratio*po_amt;
			
			hy_line++;
	
			cntv++;
			vtcp_log("%d",cntv);
	
			if( hy_line % 25 == 0 )
			{
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_cdhpdybzjqd);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","EE",&opncnt,get_ratio_cdhpdybzjqd);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				
				hy_line = 0;
				page++;
	
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_cdhpdybzjqd);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
			}
		}
	}

	Mo_bank_acc_po_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成承兑汇票多余保证金清单成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成承兑汇票多余保证金清单失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_cdhpdybzjqd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	struct com_branch_c sComBranch;
	
	switch( bli ) 
	{ 
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
		case 'A': 
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", sMoBankAccPo.tx_br_no );
			if( g_reply_int )
			{
				sprintf( acErrMsg, "Err[%ld]", g_reply_int );
				WRITEMSG
				return -1;
			}
			pub_base_strpack( sComBranch.up_br_no );
			if( !strcmp( sComBranch.up_br_no, TOT_BR_NO ) )
				strcpy( sComBranch.up_br_no, sComBranch.br_no );
			sprintf(vstr, "%s", sComBranch.up_br_no );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'B': 
			pub_base_strpack( sMoBankAccPo.payer_name );
			sprintf(vstr, "%s", sMoBankAccPo.payer_name );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'C': 
			pub_base_strpack( sMoBankAccPo.pact_no );
			sprintf(vstr, "%s", sMoBankAccPo.pact_no );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'D': 
			sprintf( fmt, "%%%d.0lf", bll ); 
			sprintf( vstr, fmt, sMoBankAccPo.sign_amt );
			strcpy( str, vstr ); 
			break; 
		case 'F': 
			sprintf(vstr, "%8.0ld", sMoBankAccPo.matr_date );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'G': 
			sprintf(vstr, "%8.2lf", sMoBankAccPo.bail_amt_ratio );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'H': 
			sprintf(vstr, "%8.0lf", bail_amt );
			if( sMoBankAccPo.bail_amt_ratio==0.00 )
				sprintf(vstr, "%8.0lf", 0 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'I': 
			sprintf(vstr, "%8.0lf", (bail_amt-bail_amt_ratio*po_amt) );
			if( sMoBankAccPo.bail_amt_ratio==0.00 )
				sprintf(vstr, "%8.0lf", 0 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'J': 
			switch( sMoBankAccPo.acc_type[0] )
			{
				case '1': strcpy( vstr, "差额承兑" ); break;
				case '2': strcpy( vstr, "百分百质押存单" ); break;
				case '3': strcpy( vstr, "百分百保证金" ); break;
				case '4': strcpy( vstr, "保兑仓" ); break;
				case '5': strcpy( vstr, "承兑拆分" ); break;
				default : break;
			}
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'K':
			switch( sMoBankAccPo.acc_po_sts[0] )
			{
				case 'A': strcpy( vstr, "存保证金" ); break;
				case 'B': strcpy( vstr, "承兑拆分录入" ); break;
				case '0': strcpy( vstr, "录入" ); break;
				case '1': strcpy( vstr, "审核" ); break;
				case '2': strcpy( vstr, "正常" ); break;
				case '3': strcpy( vstr, "退票修改" ); break;
				default : break;
			}
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'L': 
			sprintf( fmt, "%%%d.0lf", bll ); 
			sprintf( vstr, fmt, tol_amt );
			strcpy( str, vstr );
			break;
		case 'M': 
			sprintf( fmt, "%%%d.0lf", bll ); 
			sprintf( vstr, fmt, tol_bz_amt );
			strcpy( str, vstr );
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}
