/***************************************************************
* �� �� ��: sp9750.c
* �����������շѼ���ӽ���
* ��    ��: rob
* ������ڣ�2003��5��10��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "com_def_chrg_c.h"
#include "mo_def_rate_c.h"
#include "com_chrg_rate_c.h"

sp9750()
{
	int 	ret=0;
	char	dc_code[5];
	char	chrg_rate_code[4];
	char	tmp_ind[2];
	double	amt,chrg_amt;
	double	xj_amt,zz_amt,kx_amt;

	struct	com_def_chrg_c	def_chrg;
	struct	mo_def_rate_c	mo_def_rate;
	struct	com_chrg_rate_c	com_chrg_rate;
	char filename[100];

	FILE *fp;

	xj_amt=zz_amt=kx_amt=0.00;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	pub_base_AllDwnFilName( filename );
	fp=fopen(filename,"w");
	if (fp==NULL)
	{
		sprintf(acErrMsg,"�����ļ�����![%s]",tmpfile);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P157");
		goto ErrExit;
	}

	vtcp_log( "TJ:tx_code='%s' order by seqn",g_pub_tx.tx_code );

/*
	ret=Com_def_chrg_Dec_Sel( g_pub_tx.reply,"tx_code='%s' order by seqn",g_pub_tx.tx_code );
*/
	ret=Com_def_chrg_Dec_Sel( g_pub_tx.reply,"tx_code='%s' order by seqn","2101" );
	if( ret )
	{
		goto ErrExit;
	}

	while( 1 )
	{
		ret=Com_def_chrg_Fet_Sel( &def_chrg,g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack( def_chrg.amt_no );
		pub_base_strpack( def_chrg.mo );

		ret=pubf_def_chrg_proc( def_chrg.chrg_rate_code,
			mo_def_rate.chrg_rate_no,def_chrg.amt_no,&amt );
		if( ret ) goto ErrExit;

		ret=pubf_acct_chrg_proc( mo_def_rate.chrg_rate_no,
			com_chrg_rate.dc_code,&chrg_amt,amt );
		if( ret ) goto ErrExit;

		set_zd_data(DC_FILE_SND,"1");

		fprintf( fp,"%s|%s|%s|%.2lf|\n",def_chrg.chrg_mode,
			def_chrg.chrg_rate_code,def_chrg.mo,amt );
		
		if( def_chrg.chrg_mode[0]=='0' ) kx_amt+=amt;
		if( def_chrg.chrg_mode[0]=='1' ) xj_amt+=amt;
		if( def_chrg.chrg_mode[0]=='2' ) zz_amt+=amt;

	}
	Com_def_chrg_Clo_Sel();

	fclose( fp );
OkExit:
	strcpy( g_pub_tx.reply,"0000" );
	sprintf(acErrMsg,"�շѼ���ӽ��׳ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�շѼ���ӽ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
