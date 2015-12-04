/*************************************************************
* �� �� ��: rpt053.c
* ����������֧�а����ǿ�ҵ����ͳ���±�
*
* ��    ��: lance
* �������: 2003��10��08��
*
* ע����ʽ�ļ�Ϊjjkywltjyb.tbl
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

static struct com_sys_parm_c sComSysParm;
static struct com_branch_c sComBranch;
static struct com_branch_c sComBranch1;
static int cnt1,cnt2,cnt3,cnt4,cnt5,cnt6,cnt7;
static double amt1,amt2,amt3,amt4,amt5,amt6,amt7;
static long date_min,date_max;    /*����*/
static char br_no_tmp[6];

int get_ratio_jjkywltjyb(int bli,int bll,char str[100]);
rpt053()
{
	int line, opncnt;
	char vpfm[21];
	char fname[10];
	char pre_br_type[2];
	char pre_br_no[6];
	char pre_up_br_no[6];
	FILE *fp; 
	char zh_br_no[6];    /*���л�����*/
	int br_cnt;
	
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
    
	strcpy( fname, "BMjjkywltjyb" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"ɾ��֧�а����ǿ�ҵ����ͳ���±�����!!");
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
	date_max = date_min + 32;
	
	strcpy(vpfm, "jjkywltjyb");

	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_type='6'" );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"ȡ���л����Ŵ�[%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	strcpy( zh_br_no, sComBranch.br_no );

	g_reply_int = pub_rpt_openfile( &fp, sComBranch.br_no , fname );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_jjkywltjyb );
	if( g_reply_int )
	{
		sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "br_type='7' ORDER BY br_no" );
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
			/*ȫ�кϼ�ֵ*/
			g_reply_int = get_br_value( zh_br_no,0 );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"ȡȫ�и���ͳ��ֵ����" );
				WRITEMSG
				goto ErrExit;
			}
	
			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "ZZ", &opncnt, \ 
				get_ratio_jjkywltjyb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM ZZ error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			break;
		}

		br_cnt = 0;
		g_reply_int = Com_branch_Dec_Sel1( g_pub_tx.reply, "up_br_no='%s' ORDER BY br_type,br_no", \ 
			sComBranch.br_no );
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
			{
				if( br_cnt < 2 ) break;    /*�������������С����������ӡ�ϼ�ֵ*/

				/*֧�кϼ�*/
				g_reply_int = get_br_value( sComBranch.br_no , 1 );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"ȡ[%s]��������ͳ��ֵ����", sComBranch.br_no );
					WRITEMSG
					goto ErrExit;
				}
	
				g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "AA", &opncnt, \ 
					get_ratio_jjkywltjyb );
				if( g_reply_int )
				{
					sprintf( acErrMsg,"RPT READ PFM AA error![%d]", g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				break;
			}

			g_reply_int = get_br_value( sComBranch1.br_no,2 );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"ȡ[%s]��������ͳ��ֵ����", sComBranch1.br_no );
				WRITEMSG
				goto ErrExit;
			}

			g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "AA", &opncnt, \ 
				get_ratio_jjkywltjyb );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM AA error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			br_cnt ++;
		}
		g_reply_int = pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt, \ 
				get_ratio_jjkywltjyb );
		Com_branch_Clo_Sel1();
	}
	
	Com_branch_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����֧�а����ǿ�ҵ����ͳ���±��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"����֧�а����ǿ�ҵ����ͳ���±�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_jjkywltjyb( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	
	switch( bli ) 
	{ 
		case 'V': 
			sprintf(vstr, "%s--%s", sComBranch.br_no, sComBranch.br_name );
			sprintf( fmt, "%%-%ds", bll );
			pub_base_strpack( vstr );
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
		case 'v': 
			sprintf(vstr, "%s", br_no_tmp );
			sprintf( fmt, "%%%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'a': 
			sprintf(vstr, "%ld", cnt1 );
			sprintf( fmt, "%%%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'b': 
			sprintf(vstr, "%5.2lf", amt1 / 10000.00 );
			sprintf( fmt, "%%%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'c': 
			sprintf(vstr, "%ld", cnt2 );
			sprintf( fmt, "%%%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'd': 
			sprintf(vstr, "%5.2lf", amt2 / 10000.00 );
			sprintf( fmt, "%%%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'e': 
			sprintf(vstr, "%ld", cnt3 );
			sprintf( fmt, "%%%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'f': 
			sprintf(vstr, "%5.2lf", amt3 / 10000.00 );
			sprintf( fmt, "%%%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'g': 
			sprintf(vstr, "%ld", cnt4 );
			sprintf( fmt, "%%%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		case 'h': 
			sprintf(vstr, "%5.2lf", amt4 / 10000.00 );
			sprintf( fmt, "%%%ds", bll );
			sprintf( str, fmt, vstr );
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
	return 0; 
}

/*�ѻ����������ʹ��ϼ�ȡ����Ӧ������*/
/*flag:0-ȫ��,1-֧��,2-����*/
int get_br_value( br_no_v, flag )
char *br_no_v;
int flag;
{
	int cnt_tmp,cnt_tmp1;
	double amt_tmp,amt_tmp1;
	long date_tmp;

	struct com_branch_c sComBranch_v;

	memset( &sComBranch_v, 0x00, sizeof( struct com_branch_c ) );
	
	memset( br_no_tmp, 0x00, sizeof( br_no_tmp ) );
	strcpy( br_no_tmp, br_no_v );

	cnt1=cnt2=cnt3=cnt4=cnt5=cnt6=cnt7=0;
	amt1=amt2=amt3=amt4=amt5=amt6=amt7=0.00;

	/*���������������ȡȫ��ͳ��ֵ������ǹ���֧��ȡȫ֧��ͳ��ֵ������ǻ���ȡ����ֵ*/
	/*ͳ��ȫ��*/
	if( flag == 0 )
	{
		/*���ؿ����*/
		cnt_tmp = sql_count( "trace_log_rz", " ( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='1'  and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001' ", date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt1 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz", "amt", &amt_tmp, "( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='1' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001'", date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt1 += amt_tmp;
		
		/*���ؿ�ȡ��*/
		cnt_tmp = sql_count( "trace_log_rz", " ( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='0' and tx_date between %ld and %ld and sts!='1'  and sub_tx_code!='M002'", date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt2 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz", "amt", &amt_tmp, "( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='0' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M002'", date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt2 += amt_tmp;
	
	
		/*��ؿ����*/
		cnt_tmp = sql_count( "trace_log_rz",  " tx_code='7701' and amt>0.00 and add_ind='0' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001'", date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt3 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz", "amt", &amt_tmp, " tx_code='7701' and amt>0.00  and add_ind='0' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001'", date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt3 += amt_tmp;
	
		/*��ؿ�ȡ��*/
		cnt_tmp = sql_count( "trace_log_rz", " tx_code in( '7702','6203' ) and ( ac_no[1,11]!='94007173308' and  ac_no[1,4]!='4302' )  and amt>0.00  and add_ind='1' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M002'", date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt4 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz", "amt", &amt_tmp, " tx_code in( '7702','6203' ) and ( ac_no[1,11]!='94007173308' and  ac_no[1,4]!='4302' ) and amt>0.00  and add_ind='1' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M002'", date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt4 += amt_tmp;
	}
	else if( flag == 1 )
	{
		/*���ؿ����*/
		cnt_tmp = sql_count( "trace_log_rz,com_branch", " ( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='1' and tx_br_no=com_branch.br_no and com_branch.up_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001' ", br_no_v, date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt1 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz,com_branch", "amt", &amt_tmp, "( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='1' and tx_br_no=com_branch.br_no and com_branch.up_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001'", br_no_v, date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt1 += amt_tmp;
		
		/*���ؿ�ȡ��*/
		cnt_tmp = sql_count( "trace_log_rz,com_branch", " ( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='0' and tx_br_no=com_branch.br_no and com_branch.up_br_no='%s' and tx_date between %ld and %ld and sts!='1'  and sub_tx_code!='M002'", br_no_v, date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt2 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz,com_branch", "amt", &amt_tmp, "( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='0' and tx_br_no=com_branch.br_no and com_branch.up_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M002'", br_no_v, date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt2 += amt_tmp;
	
	
		/*��ؿ����*/
		cnt_tmp = sql_count( "trace_log_rz,com_branch",  " tx_code='7701' and amt>0.00  and tx_br_no=com_branch.br_no and com_branch.up_br_no='%s' and add_ind='0' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001'", br_no_v, date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt3 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz,com_branch", "amt", &amt_tmp, " tx_code='7701' and amt>0.00  and tx_br_no=com_branch.br_no and com_branch.up_br_no='%s' and add_ind='0' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001'", br_no_v, date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt3 += amt_tmp;
	
		/*��ؿ�ȡ��*/
		cnt_tmp = sql_count( "trace_log_rz,com_branch", " tx_code in( '7702','6203' ) and ( ac_no[1,11]!='94007173308' and  ac_no[1,4]!='4302' )  and amt>0.00  and add_ind='1' and tx_br_no=com_branch.br_no and com_branch.up_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M002'", br_no_v, date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt4 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz,com_branch", "amt", &amt_tmp, " tx_code in( '7702','6203' ) and ( ac_no[1,11]!='94007173308' and  ac_no[1,4]!='4302' ) and amt>0.00  and add_ind='1' and tx_br_no=com_branch.br_no and com_branch.up_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M002'", br_no_v, date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt4 += amt_tmp;
	}
	else if( flag == 2 )
	{
		/*���ؿ����*/
		cnt_tmp = sql_count( "trace_log_rz", " ( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='1' and tx_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001' ", br_no_v, date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt1 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz", "amt", &amt_tmp, "( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='1' and tx_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001'", br_no_v, date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt1 += amt_tmp;
		
		/*���ؿ�ȡ��*/
		cnt_tmp = sql_count( "trace_log_rz", " ( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='0' and tx_br_no='%s' and tx_date between %ld and %ld and sts!='1'  and sub_tx_code!='M002'", br_no_v, date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt2 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz", "amt", &amt_tmp, "( ac_no[1,11]='94007173308' or ac_no[1,4]='4302' ) and amt>0.00 and add_ind='0' and tx_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M002'", br_no_v, date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt2 += amt_tmp;
	
	
		/*��ؿ����*/
		cnt_tmp = sql_count( "trace_log_rz",  " tx_code='7701' and amt>0.00  and tx_br_no='%s' and add_ind='0' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001'", br_no_v, date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt3 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz", "amt", &amt_tmp, " tx_code='7701' and amt>0.00  and tx_br_no='%s' and add_ind='0' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M001'", br_no_v, date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt3 += amt_tmp;
	
		/*��ؿ�ȡ��*/
		cnt_tmp = sql_count( "trace_log_rz", " tx_code in( '7702','6203' ) and ( ac_no[1,11]!='94007173308' and  ac_no[1,4]!='4302' )  and amt>0.00  and add_ind='1' and tx_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M002'", br_no_v, date_min, date_max );
		if( cnt_tmp < 0 )
		{
			sprintf( acErrMsg,"count trace_log_rz error![%d]", cnt_tmp );
			WRITEMSG
			goto ErrExit;
		}
		cnt4 += cnt_tmp;
		g_reply_int = sql_sum_double( "trace_log_rz", "amt", &amt_tmp, " tx_code in( '7702','6203' ) and ( ac_no[1,11]!='94007173308' and  ac_no[1,4]!='4302' ) and amt>0.00  and add_ind='1' and tx_br_no='%s' and tx_date between %ld and %ld and sts!='1' and sub_tx_code!='M002'", br_no_v, date_min, date_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"SUM trace_log_rz's amt error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		amt4 += amt_tmp;
	}
				
	return 0;

ErrExit:
	return -1;
}
