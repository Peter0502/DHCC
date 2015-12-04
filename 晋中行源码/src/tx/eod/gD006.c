/*************************************************************
* �� �� ��: gD006.c
* ������������������
* �Ѳ��������ļ��еļ�¼�Ǽ��ڵǼǲ��У������ǽ跽�ʹ���
**************************************************************/
#define EXTERN
#include <stdio.h>
#include "public.h"
#include "fin_acct_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"

struct fin_acct_c sFinAcct;
struct com_sys_parm_c sComSysParm;
struct com_branch_c sComBranch;

gD006()
{
	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );

    g_reply_int = sql_begin(); /*������*/
	if( g_reply_int )
	{
		sprintf( acErrMsg, "���ݿ���� ������ʧ��!" );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = pub_base_GetSysparm( &sComSysParm );
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "ȡ����ϵͳ�������![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}
	g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_type='1'" );
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "ȡ�������Ļ����Ŵ�![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}

    pub_base_sysinit();

	g_pub_tx.tx_date = sComSysParm.sys_date;
	g_pub_tx.tx_date = 20051103;
	
	fin_report( "---------- %ld - �������˿�ʼ ----------", g_pub_tx.tx_date );
	vtcp_log( "---------- %ld - �������˵Ǽǿ�ʼ ----------", g_pub_tx.tx_date );

	g_reply_int = fin_reg();    /*�Ǽǲ������˵Ǽǲ�*/
	if( g_reply_int ) goto ErrExit;

	/*���ƽ��*/
	g_reply_int = check_balance();
	if( g_reply_int )
	{
		fin_report( "---------- %ld - ������ܶƽ�� ����ʧ��----------", g_pub_tx.tx_date );
		vtcp_log( "---------- %ld - ������ܶƽ�� ����ʧ��----------", g_pub_tx.tx_date );
		goto ErrExit;
	}

	vtcp_log( "---------- %ld - �������˿�ʼ ----------", g_pub_tx.tx_date );
	g_reply_int = fin_acct();    /*���� ÿ��һ������*/
	if( g_reply_int ) goto ErrExit;

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg, "�������˳ɹ�![%s]", g_pub_tx.reply );
	WRITEMSG
	fin_report( "---------- %ld - �������˳ɹ����� ----------", g_pub_tx.tx_date );
	set_zd_data( "0120", g_pub_tx.reply );
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf( acErrMsg, "��������ʧ��![%s]", g_pub_tx.reply );
	WRITEMSG
	fin_report( "---------- %ld - ��������ʧ�ܽ��� ----------", g_pub_tx.tx_date );
	set_zd_data( "0120", g_pub_tx.reply );
	return 1;
}

int fin_reg()
{
	FILE *fp;
	char fin_filename[100];
	char fin_info[1024];

	g_reply_int = sql_count( "fin_acct", "tx_date='%ld'", g_pub_tx.tx_date );
	if( g_reply_int < 0 )
	{
		sprintf( acErrMsg, "�����������ļ��Ƿ��Ѿ��Ǽ�ʧ��![%ld]", g_reply_int );
		WRITEMSG
		return -1;
	}
	else if( g_reply_int > 0 )
	{
		sprintf( acErrMsg, "���������ļ��ѵǼǳɹ�,���ǵ�һ��ִ��" );
		WRITEMSG
		return 0;
	}

	sprintf( acErrMsg, "��ʼ�Ǽǲ��������ļ�" );
	WRITEMSG


	sprintf( fin_filename, "%s/MFAP%ld.txt", getenv( "AUTOACCTDIR" ), g_pub_tx.tx_date );
	fp = fopen( fin_filename, "r" );
	if( fp == NULL )
	{
		sprintf( acErrMsg, "�򿪲��������ļ�[%s]ʧ��", fin_filename );
		WRITEMSG
		return -1;
	}

	memset( fin_info, 0x00, sizeof( fin_info ) );
	while( fgets( fin_info, sizeof( fin_info ), fp ) != NULL )
	{
		pub_base_strpack_all( fin_info );
		if( !strncmp( fin_info, "END", 3 ) )
		{
			vtcp_log( "�������������ļ��������" );
			break;
		}

		if( strlen( fin_info ) )
		{
			if( split_fin_info( fin_info ) )
				return -1;
		}

		memset( fin_info, 0x00, sizeof( fin_info ) );
	}
	if( strncmp( fin_info, "END", 3 ) )
	{
		fin_report( "���������ļ�û�ж����������,����!" );
		sprintf( acErrMsg, "���������ļ�û�ж���'END'�������!" );
		WRITEMSG
	}

	return 0;
}

int split_fin_info( char *tmp_buf )
{
	char tmp_str[100];
	double d_amt, c_amt;

	memset( &sFinAcct, 0x00, sizeof( struct fin_acct_c ) );

	memset( tmp_str, 0x00, sizeof( tmp_str ) );
	pub_base_readfield( tmp_str, tmp_buf, '|' );
	sFinAcct.fin_date = atol( tmp_str );/*����*/

	pub_base_readfield( sFinAcct.ac_no, tmp_buf, '|' );/*�˺�*/

	memset( tmp_str, 0x00, sizeof( tmp_str ) );
	pub_base_readfield( tmp_str, tmp_buf, '|' );
	d_amt = atof( tmp_str );/*�跽������*/

	memset( tmp_str, 0x00, sizeof( tmp_str ) );
	pub_base_readfield( tmp_str, tmp_buf, '|' );
	c_amt = atof( tmp_str );/*����������*/

	memset( tmp_str, 0x00, sizeof( tmp_str ) );
	pub_base_readfield( tmp_str, tmp_buf, '|' );
	sFinAcct.bal = atof( tmp_str );/*���*/

	sprintf( acErrMsg, "����[%ld] ��Ŀ��[%s] ��[%.2lf] ��[%.2lf] ���[%.2lf]",
		sFinAcct.fin_date, sFinAcct.ac_no, d_amt, c_amt, sFinAcct.bal );
	WRITEMSG

	sFinAcct.tx_date = g_pub_tx.tx_date;
	strcpy( sFinAcct.acct_flag, "0" );

	if( d_amt > 0.005 || d_amt < -0.005 )
	{
		sFinAcct.amt = d_amt;
		strcpy( sFinAcct.dc_flag, "0" );

		g_reply_int = Fin_acct_Ins( sFinAcct, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf(acErrMsg, "�跽���˵Ǽ�ʧ��![%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
	}

	if( c_amt > 0.005 || c_amt < -0.005 )
	{
		sFinAcct.amt = c_amt;
		strcpy( sFinAcct.dc_flag, "1" );

		g_reply_int = Fin_acct_Ins( sFinAcct, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf(acErrMsg, "�������˵Ǽ�ʧ��![%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
	}

	return 0;
}

/*���ƽ��*/
int check_balance()
{
	double amt_j=0.0;
	double amt_d=0.0;

	g_reply_int = sql_sum_double( "fin_acct", "amt", &amt_j, "tx_date=%ld and dc_flag='1'" );
	if( g_reply_int )
	{
		return 1;
	}
	g_reply_int = sql_sum_double( "fin_acct", "amt", &amt_d, "tx_date=%ld and dc_flag='2'" );
	if( g_reply_int )
	{
		return 1;
	}

	if( amt_j < amt_d || amt_j > amt_d )
	{
		return 1;
	}

	return 0;
}

/*�����Ӻ���
	Ϊ��ÿ�ʼ���һ������,��ѭ������ȡ��¼��,��ͨ����־���Ǵ�����ļ�¼*/
/*20051111�޸�:ÿ��һ��������ܻ��������ƽ,ȡ��ÿ��һ�����������
��û��ȡ���Ƿ��Ѽ��ʵı�־λ,�ñ�־λ���������ж��Ƿ���ɼ��ʵ�����*/
int fin_acct()
{
	int i=0, j=0;
	char tmpstr[10];

	g_reply_int = Fin_acct_Dec_Upd( g_pub_tx.reply, "tx_date='%ld' and acct_flag='0'", g_pub_tx.tx_date );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "ȡ���˼�¼�����α��[%ld]", g_reply_int );
		WRITEMSG
		return -1;
	}
	while( 1 )
	{
		memset( &sFinAcct, 0x00, sizeof( struct fin_acct_c ) );

		g_reply_int = Fin_acct_Fet_Upd( &sFinAcct, g_pub_tx.reply );
		if( g_reply_int == 100 )
		{
			fin_report( "���˽���,�� %d ��,�ɹ����� %d ��", i, j );
			vtcp_log( "���˽���,�� %d ��,�ɹ����� %d ��", i, j );
			break;
		}
		else if( g_reply_int )
		{
			sprintf( acErrMsg, "ȡ���˼�¼�����α��[%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
		i++;
		strcpy( sFinAcct.acct_flag, "1" );
		g_reply_int = Fin_acct_Upd_Upd( sFinAcct, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���¼��˼�¼��[%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}

		/*����*/

		if( sFinAcct.dc_flag[0] == '0' )
			strcpy( tmpstr, "��" );
		else if( sFinAcct.dc_flag[0] == '1' )
			strcpy( tmpstr, "��" );

		g_reply_int =  fin_acct_sub();
		if( g_reply_int )
		{
			fin_report( " %3d ����Ŀ�š�%7s %s��: %.2lf    ʧ��!���󷵻��� %s", 
				i, sFinAcct.ac_no, tmpstr, sFinAcct.amt, g_pub_tx.reply );
			vtcp_log( " %3d ����Ŀ�š�%7s %s��: %.2lf    ʧ��!���󷵻��� %s", 
				i, sFinAcct.ac_no, tmpstr, sFinAcct.amt, g_pub_tx.reply );
			/*sql_rollback();*/
			goto ErrExit;
		}
		else
		{
			fin_report( " %3d ����Ŀ�š�%7s %s��: %.2lf", i, sFinAcct.ac_no, tmpstr, sFinAcct.amt );
			vtcp_log( " %3d ����Ŀ�š�%7s %s��: %.2lf", i, sFinAcct.ac_no, tmpstr, sFinAcct.amt );
			j++;
			/*sql_commit();*/
		}
	}
	Fin_acct_Clo_Upd();

OkExit:
	return 0;
ErrExit:
	return -1;
}

/*ÿ������һ������ı���begin
int fin_acct()
{
	int i=0, j=0;
	char tmpstr[10];

	while( 1 )
	{
		memset( &sFinAcct, 0x00, sizeof( struct fin_acct_c ) );

   		g_reply_int = sql_begin(); *��ȡ��¼����*
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���ݿ���� ������ʧ��!" );
			WRITEMSG
			return -1;
		}

		*ȡ��¼*
		g_reply_int = Fin_acct_Dec_Upd( g_pub_tx.reply, "tx_date='%ld' and acct_flag='0'", g_pub_tx.tx_date );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "ȡ���˼�¼�����α��[%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
		g_reply_int = Fin_acct_Fet_Upd( &sFinAcct, g_pub_tx.reply );
		if( g_reply_int == 100 )
		{
			fin_report( "���˽���,�� %d ��,�ɹ����� %d ��", i, j );
			vtcp_log( "���˽���,�� %d ��,�ɹ����� %d ��", i, j );
			sql_commit();	*--�ύȡ��¼����--*
			break;
		}
		else if( g_reply_int )
		{
			sprintf( acErrMsg, "ȡ���˼�¼�����α��[%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
		i++;
		strcpy( sFinAcct.acct_flag, "1" );
		g_reply_int = Fin_acct_Upd_Upd( sFinAcct, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���¼��˼�¼��[%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
		Fin_acct_Clo_Upd();
		sql_commit();	*--�ύȡ��¼����--*

		*����*
		vtcp_log("�򿪼�������");
   		g_reply_int=sql_begin(); *�򿪼�������*
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���ݿ���� ������ʧ��!" );
			WRITEMSG
			return -1;
		}

		if( sFinAcct.dc_flag[0] == '0' )
			strcpy( tmpstr, "��" );
		else if( sFinAcct.dc_flag[0] == '1' )
			strcpy( tmpstr, "��" );

		g_reply_int =  fin_acct_sub();
		if( g_reply_int )
		{
			fin_report( " %3d ����Ŀ�š�%7s %s��: %.2lf    ʧ��!���󷵻��� %s", 
				i, sFinAcct.ac_no, tmpstr, sFinAcct.amt, g_pub_tx.reply );
			vtcp_log( " %3d ����Ŀ�š�%7s %s��: %.2lf    ʧ��!���󷵻��� %s", 
				i, sFinAcct.ac_no, tmpstr, sFinAcct.amt, g_pub_tx.reply );
			*sql_rollback();*
			goto ErrExit;
		}
		else
		{
			fin_report( " %3d ����Ŀ�š�%7s %s��: %.2lf", i, sFinAcct.ac_no, tmpstr, sFinAcct.amt );
			vtcp_log( " %3d ����Ŀ�š�%7s %s��: %.2lf", i, sFinAcct.ac_no, tmpstr, sFinAcct.amt );
			j++;
			*sql_commit();*
		}
	}

OkExit:
	return 0;
ErrExit:
	return -1;
}
ÿ������һ������ı���end*/


int fin_acct_sub()
{
	/*����*/
	memset( &g_pub_tx, 0x00, sizeof( struct pub_tx ) );
	pub_base_sysinit();
	strcpy( g_pub_tx.opn_br_no, sComBranch.br_no );
	strcpy( g_pub_tx.tx_br_no, sComBranch.br_no );
	g_pub_tx.tx_date = sComSysParm.sys_date;
	memset( &g_mdm_ac_rel, 0x00, sizeof( struct mdm_ac_rel_c ) ); 
	memset( &g_dd_mst, 0x00, sizeof( struct dd_mst_c ) ); 
	memset( &g_dd_parm, 0x00, sizeof( struct dd_parm_c ) ); 
	memset( &g_td_mst, 0x00, sizeof( struct td_mst_c ) ); 
	memset( &g_td_parm, 0x00, sizeof( struct td_parm_c ) ); 
	memset( &g_ln_mst, 0x00, sizeof( struct ln_mst_c ) ); 
	memset( &g_ln_parm, 0x00, sizeof( struct ln_parm_c ) ); 
	memset( &g_in_mst, 0x00, sizeof( struct in_mst_c ) );
	memset( &g_in_parm, 0x00, sizeof( struct in_parm_c ) );

	strcpy( g_pub_tx.tx_code, "MIS1" );
	strcpy( g_pub_tx.ac_no, sFinAcct.ac_no );
	strcpy( g_pub_tx.cur_no, "01" );
	strcpy( g_pub_tx.ct_ind, "2" );
	g_pub_tx.tx_amt1 = sFinAcct.amt;
	strcpy( g_pub_tx.brf, "��������" );
	strcpy( g_pub_tx.prt_ind, "0");
	strcpy( g_pub_tx.add_ind, sFinAcct.dc_flag );
	strcpy( g_pub_tx.ac_wrk_ind, "00000" );
	strcpy( g_pub_tx.hst_ind, "1" );
	g_pub_tx.svc_ind = 1001;
	
	g_reply_int = pub_acct_trance();
	if( g_reply_int )
	{
		sprintf( acErrMsg, "���ü���ˮ�ʺ�������![%d]", g_reply_int );
		WRITEMSG
		return -1;
	}
	
	/*�ǻ����*/
	if( sFinAcct.dc_flag[0] == '0' )
	{
		set_zd_double("1208", sFinAcct.amt);
		set_zd_data("1204", "01");
		set_zd_data("1201", sFinAcct.ac_no);
		set_zd_data("1205","2");
		g_reply_int = pubf_acct_proc("A016");
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���üǻ���ʺ�������![%d]", g_reply_int );
			WRITEMSG
			return -1;
		}
	}
	else if( sFinAcct.dc_flag[0] == '1' )
	{
		set_zd_double("1218", sFinAcct.amt);
		set_zd_data("1214", "01");
		set_zd_data("1211", sFinAcct.ac_no);
		set_zd_data("1215","2");
		g_reply_int = pubf_acct_proc("A017");
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���üǻ���ʺ�������![%d]", g_reply_int );
			WRITEMSG
			return -1;
		}
	}

	return 0;
}
