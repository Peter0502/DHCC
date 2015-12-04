/*************************************************************
* �� �� ��: rpt036.c
* �����������������ձ���֧�У�
*
* ��    ��: jane
* �������: 2003��9��16��
*
* ע����ʽ�ļ�Ϊckyerbb.tbl
*	  ��gl_subͳ��
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
#include "com_branch_c.h"
#include "gl_sub_c.h"

static struct com_sys_parm_c sComSysParm;
static struct com_branch_c sComBranch;
static struct com_branch_c sComBranch1;
static struct gl_sub_c sGlSub201;
static struct gl_sub_c sGlSub205;
static struct gl_sub_c sGlSub20201;
static struct gl_sub_c sGlSub211;
static struct gl_sub_c sGlSub215;
static struct gl_sub_c sGlSub20202;
static struct gl_sub_c sGlSub235;
static struct gl_sub_c sGlSub243;
static struct gl_sub_c sGlSub251;
static struct gl_sub_c sGlSub234;
static struct gl_sub_c sGlSub101;
static struct gl_sub_c sGlSub_sum;
static double de_bal_sum;    /*���ϼ�*/
static double ys_de_bal_sum;    /*���մ��ϼ�*/
static double yb_de_bal_sum;    /*������ϼ�*/

int get_ratio_ckyerbb(int bli,int bll,char str[100]);
rpt036()
{
	int line, opncnt;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	char pre_up_br_no[6];
	FILE *fp; 
	
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );
	memset( pre_up_br_no, 0x00, sizeof( pre_up_br_no ) );

    g_reply_int = sql_begin(); /*������*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
    
	strcpy( fname, "BRCKYERBB" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"ɾ���������ձ������!!");
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

	strcpy(vpfm, "ckyerbb");

	/*�������ĺ͹���֧��Ӫҵ��*/

	/*g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "br_type in ('1','4') ORDER BY br_no" );lwb  20150525 �������   */
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
			g_reply_int = get_br_cr_bal( pre_up_br_no );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"ȡ[%s]����������������", sComBranch.up_br_no );
				WRITEMSG
				goto ErrExit;
			}

			strcpy( sComBranch1.br_name, "        �ϼƣ�  " );

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

		/*��������*/
		if( !strcmp( sComBranch.up_br_no, "61000" ) )
			strcpy( sComBranch.up_br_no, sComBranch.br_no );

		/*�����ű�,��һ���ļ�*/
		if( strcmp( sComBranch.br_no, pre_br_no ) )
		{
			/*���ǵ�һ������,���ϸ������Ľ�β*/
			if( strlen( pre_br_no ) )
			{
				g_reply_int = get_br_cr_bal( pre_up_br_no );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"ȡ[%s]����������������", pre_up_br_no );
					WRITEMSG
					goto ErrExit;
				}

				strcpy( sComBranch1.br_name, "        �ϼƣ�  " );

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

			/*��ӡ��һ��������ʼ*/
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

			strcpy( pre_br_no, sComBranch.br_no );
			strcpy( pre_up_br_no, sComBranch.up_br_no );
			memset( &sGlSub_sum, 0x00, sizeof( struct gl_sub_c ) );    /*�ϼ�ֵ*/
		}

		/*��������*/
		/*if( sComBranch.br_type[0] == '1' )lwb 20150525  �������    */
		if( sComBranch.br_type[0] == '1' || sComBranch.br_type[0] == '3')
		{
			g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "br_no='%s'", sComBranch.br_no );
			vtcp_log("��������[%s]",sComBranch.br_no);
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
				sprintf( acErrMsg,"ȡ[%s]����������������", sComBranch1.br_no );
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
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ������ձ���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"���ɴ������ձ���ʧ��![%s]",g_pub_tx.reply);
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
			sprintf(vstr, "%8.0lf", sGlSub201.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'b': 
			sprintf(vstr, "%8.0lf", sGlSub205.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'c': 
			sprintf(vstr, "%8.0lf", sGlSub20201.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'd': 
			sprintf(vstr, "%8.0lf", sGlSub211.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'e': 
			sprintf(vstr, "%8.0lf", sGlSub215.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'f': 
			sprintf(vstr, "%8.0lf", sGlSub20202.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'g': 
			sprintf(vstr, "%8.0lf", sGlSub235.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'h': 
			sprintf(vstr, "%8.0lf", sGlSub243.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'i': 
			sprintf(vstr, "%8.0lf", sGlSub251.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'j': 
			sprintf(vstr, "%8.0lf", de_bal_sum / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'k': 
			sprintf(vstr, "%8.0lf", ( de_bal_sum - ys_de_bal_sum ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'l': 
			sprintf(vstr, "%8.0lf", ( de_bal_sum - yb_de_bal_sum ) / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'm': 
			sprintf(vstr, "%8.0lf", sGlSub234.cr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'n': 
			sprintf(vstr, "%8.0lf", sGlSub101.dr_bal / 10000 );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}

/*�ѻ����������ʹ��ϼ�ȡ����Ӧ�ṹ��������*/
int get_br_cr_bal( char *br_no_v )
{
	memset( &sGlSub201, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub201, \ 
		"date='%ld' and br_no='%s' and acc_hrt='20100'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub205, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub205, \ 
		"date='%ld' and br_no='%s' and acc_hrt='20500'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub20201, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub20201, \ 
		"date='%ld' and br_no='%s' and acc_hrt='20201'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub211, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub211, \ 
		"date='%ld' and br_no='%s' and acc_hrt='21100'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub215, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub215, \ 
		"date='%ld' and br_no='%s' and acc_hrt='21500'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub20202, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub20202, \ 
		"date='%ld' and br_no='%s' and acc_hrt='20202'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub235, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub235, \ 
		"date='%ld' and br_no='%s' and acc_hrt='23500'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub243, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub243, \ 
		"date='%ld' and br_no='%s' and acc_hrt='24300'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub251, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub251, \ 
		"date='%ld' and br_no='%s' and acc_hrt='25100'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub234, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub234, \ 
		"date='%ld' and br_no='%s' and acc_hrt='23400'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	memset( &sGlSub101, 0x00, sizeof( struct gl_sub_c ) );
	g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub101, \ 
		"date='%ld' and br_no='%s' and acc_hrt='10100'", sComSysParm.lst_date, br_no_v );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"SELECT gl_sub error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	de_bal_sum = sGlSub201.cr_bal + sGlSub205.cr_bal + \ 
		sGlSub20201.cr_bal + sGlSub211.cr_bal + sGlSub215.cr_bal + \ 
		sGlSub20202.cr_bal + sGlSub235.cr_bal + sGlSub243.cr_bal + sGlSub251.cr_bal;
	ys_de_bal_sum = sGlSub201.lcd_bal + sGlSub205.lcd_bal + \ 
		sGlSub20201.lcd_bal + sGlSub211.lcd_bal + sGlSub215.lcd_bal + \ 
		sGlSub20202.lcd_bal + sGlSub235.lcd_bal + sGlSub243.lcd_bal + sGlSub251.lcd_bal;
	yb_de_bal_sum = sGlSub201.ycr_bal + sGlSub205.ycr_bal + \ 
		sGlSub20201.ycr_bal + sGlSub211.ycr_bal + sGlSub215.ycr_bal + \ 
		sGlSub20202.ycr_bal + sGlSub235.ycr_bal + sGlSub243.ycr_bal + sGlSub251.ycr_bal;

	return 0;

ErrExit:
	return -1;
}
