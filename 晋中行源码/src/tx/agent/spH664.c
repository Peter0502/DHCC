/*************************************************
* �� �� ��:  spH664.c
* ���������� �˻�����ѯ(ȱʡ������ѯ����)
*
* ��    ��:  mike
* ������ڣ� 2004��04��25��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mob_sendmail_c.h"
#include "mob_acct_type_c.h"

int spH664()
{
	int ret = 0;
	int i = 0;
	int iAc_seq = 0;
	char cAc_type[2];

	double dAcbl = 0.00;

	char cOpt[4];
	char cMessage[141];
	char cMobile[16];

	char fldstr[5][128];

	struct mob_sendmail_c sMob_sendmail;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mob_acct_type_c sMob_acct_type;

	memset( cAc_type, 0x0, sizeof(cAc_type) );

	memset( cOpt, 0x0, sizeof(cOpt) );
	memset( cMessage, 0x0, sizeof(cMessage) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( fldstr, 0x0, sizeof(fldstr) );
	memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	get_zd_data( "0760", cMessage );
	get_zd_data( "0610", cMobile );

	for ( ; ; )
	{
		ret = pub_base_cut_str( cMessage, fldstr[i], "#", i+1 );
		if ( ret == 100 )
		{
			break;
		}

		pub_base_strpack( fldstr[i] );
		sprintf( acErrMsg, "[%s]", fldstr[i] );
		WRITEMSG

		i= i + 1;
	}
	
	if ( strlen(fldstr[1]) == 0 )
	{
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Sel( g_pub_tx.reply, &sMob_acct_type, \
			"mobile = '%s'", cMobile );
		
		strcpy( fldstr[1], sMob_acct_type.ac_no );
	}

	ret = sql_count( "mob_acct_type", "mobile = '%s' and \
			ac_no = '%s'", cMobile, fldstr[1] ); 
	if ( ret <= 0 )
	{
		strcpy( cMessage,"���˺Ŷ�Ӧ���ֻ�û�п�ͨ��������ҵ��!" );
		sprintf( acErrMsg, "���˺Ŷ�Ӧ���ֻ�û�п�ͨ��������ҵ��!" );
		WRITEMSG
		goto ErrExit;
	}

	/*--- fldstr[1]�˺� fldstr[2]�˺���� fldstr[3]���� ---*/

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel, \
		"ac_no = '%s'", fldstr[1] );
	if ( ret != 0 )
	{
		sprintf( acErrMsg, "��ѯ�����˺Ŷ��ձ�ʧ��!!!acno[%s][%d]", fldstr[1], ret );
		WRITEMSG
		goto ErrExit;
	}

	/*----------------------------------------------
	if ( strcmp( sMdm_ac_rel.draw_pwd, fldstr[3] ) != 0 )
	{
		strcpy ( g_pub_tx.reply, "F004" );
		sprintf( acErrMsg, "����֧ȡ���������!!!" );
		WRITEMSG
		goto ErrExit;
	}
	-----------------------------------------------*/

	if ( strlen( fldstr[2] ) == 0 )
	{
		iAc_seq = 1;	/* û�������,������һ��� */
	}
	else
	{
		iAc_seq = atoi( fldstr[2] );
	}

	ret = pub_base_disac_file ( fldstr[1], iAc_seq, cAc_type );
	if ( ret )
	{
		strcpy( cMessage, "�𾴵Ŀͻ�: ���������(��)��������,��˶Ժ��ٲ���!" );
		sprintf( acErrMsg, "��ѯ���ļ�����!!!ret[%d]" ,ret );
		WRITEMSG
		goto ErrExit;
	}

	if ( cAc_type[0] == '1' )		/*-- ���� --*/
	{
		dAcbl = g_dd_mst.bal;

		fldstr[1][strlen(fldstr[1])-2] = '*';
		pub_base_strpack( fldstr[1] );

		memset( cMessage, 0x0, sizeof(cMessage) );
		sprintf( cMessage, "�𾴵Ŀͻ�:������(��)%s��ǰ�������Ϊ%.2fԪ.��ɳ����ҵ����",  fldstr[1], dAcbl );
	}
	if ( cAc_type[0] == '2' )		/*-- ���� --*/
	{
		dAcbl = g_td_mst.bal;

		fldstr[1][strlen(fldstr[1])-2] = '*';
		pub_base_strpack( fldstr[1] );

		memset( cMessage, 0x0, sizeof(cMessage) );
		sprintf( cMessage, "�𾴵Ŀͻ�:������(��)%s��ǰ�������Ϊ%.2fԪ.��ɳ����ҵ����",  fldstr[1], dAcbl );
	}
	if ( cAc_type[0] == '3' )		/*-- ���� --*/
	{
		dAcbl = g_ln_mst.bal;

		fldstr[1][strlen(fldstr[1])-2] = '*';
		pub_base_strpack( fldstr[1] );

		memset( cMessage, 0x0, sizeof(cMessage) );
		sprintf( cMessage, "�𾴵Ŀͻ�:������(��)%s��ǰ�������Ϊ%.2fԪ.��ɳ����ҵ����",  fldstr[1], dAcbl );
	}

	strcpy( sMob_sendmail.tx_code, g_pub_tx.tx_code );
	strcpy( sMob_sendmail.ac_no, fldstr[1] );
	strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
	sMob_sendmail.trace_no = g_pub_tx.trace_no;
	sMob_sendmail.tx_date = g_pub_tx.tx_date;
	sMob_sendmail.tx_time = g_pub_tx.tx_time;
	get_zd_data( "0240", sMob_sendmail.opt_no );
	get_zd_data( "0610", sMob_sendmail.mobile );
	strcpy( sMob_sendmail.message, cMessage );
	strcpy(	sMob_sendmail.timeflag, "0" );
	sMob_sendmail.t_date = 0;
	sMob_sendmail.t_time = 0;
	strcpy(	sMob_sendmail.send_level, "1" );
	sMob_sendmail.sene_times = 1;
	sMob_sendmail.send_date = 0;
	sMob_sendmail.send_time = 0;
	sMob_sendmail.sxf_amt = 0.00;
	strcpy(	sMob_sendmail.proc_flag, "1" );

	ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
	if ( ret != 0 ) 
	{
		sprintf( acErrMsg, "дmob_sendmail��ʧ��!!![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0760", cMessage );

OkExit:
	sprintf(acErrMsg,"�˻�����ѯ�ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data( "0760", cMessage );
	sprintf(acErrMsg,"�˻�����ѯʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
