/*************************************************************
* �� �� ��: rpt020.c
* �������������δ�������ҵ��
*
* ��    ��: jane
* �������: 2003��8��17��
*
* ע����ʽ�ļ�Ϊmbfesd.tbl( sd Ϊ send delay ��д )
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
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
#include "mbfeghvsendjrn_c.h"

static int page,total;
static struct mbfeghvsendjrn_c ghvsendjrn_v;
static struct com_sys_parm_c sComSysParm;
static struct tx_def_c sTxDef;

int get_ratio_mbfesd(int bli,int bll,char str[100]);
rpt020()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );

    g_reply_int = sql_begin(); /*������*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
    
	strcpy( fname, "BRmbfesd" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"ɾ�����δ��������ҵ���嵥����!!");
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

	strcpy(vpfm, "mbfesd");

	g_reply_int = Mbfeghvsendjrn_Dec_Sel(g_pub_tx.reply, \
		"procstate='0' ORDER BY tx_br_no,consigndate,tx_code" );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE mbfeghvsendjrn CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	while(1) 
	{ 
		g_reply_int = Mbfeghvsendjrn_Fet_Sel( &ghvsendjrn_v, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH mbfeghvsendjrn CUR error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100 )
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			fclose( fp );
			break;
		}

		/*�����ű�,��һ���ļ�*/
		if( strcmp( ghvsendjrn_v.tx_br_no, pre_br_no ) )
		{
			/*���ǵ�һ������,���ϸ������Ľ�β*/
			if( strlen( pre_br_no ) )
			{
				g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesd);
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				fclose( fp );
			}

			g_reply_int = pub_rpt_openfile( &fp, ghvsendjrn_v.tx_br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			g_reply_int = sql_count( "mbfeghvsendjrn", \
				"tx_br_no='%s' and procstate='0'", ghvsendjrn_v.tx_br_no );

			total = g_reply_int / 25 + 1;
			hy_line = 0;
			page = 1;

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( pre_br_no, ghvsendjrn_v.tx_br_no );
		}

		memset( &sTxDef, 0x00, sizeof( struct tx_def_c ) );
		g_reply_int = Tx_def_Sel( g_pub_tx.reply, &sTxDef, "tx_code='%s'", ghvsendjrn_v.tx_code );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SELECT tx_def error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		
		g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_mbfesd );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		hy_line++;
			
		if( hy_line % 25 == 0 )
		{
			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","CC",&opncnt,get_ratio_mbfesd);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			hy_line = 0;
			page++;

			g_reply_int = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		}
	} 
	
	Mbfeghvsendjrn_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ��δ��������嵥�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"���ɴ��δ��������嵥ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_mbfesd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	struct com_branch_c sComBranch;
	
	switch( bli ) 
	{ 
		case 'A': 
			strcpy( str, sTxDef.tx_name ); 
			break; 
		case 'B': 
			strcpy( str, ghvsendjrn_v.payeracc ); 
			break; 
		case 'C': 
			strcpy( str, ghvsendjrn_v.payername ); 
			break; 
		case 'D': 
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, ghvsendjrn_v.amount );/*�ô����ŵĽ��*/
			strcpy( str, vstr ); 
			break; 
		case 'T': 
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", ghvsendjrn_v.tx_br_no );
			if( g_reply_int ) 
			{ 
				sprintf(acErrMsg, "SELECT com_branch error![%d]", g_reply_int ); 
				WRITEMSG 
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s��%s��", sComBranch.br_name, sComBranch.br_no );
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
