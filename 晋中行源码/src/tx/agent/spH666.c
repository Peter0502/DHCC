/*************************************************
* �� �� ��:  spH666.c
* ���������� ���ſ�ͷ��ʧ
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

int spH666()
{
	int ret = 0;
	int i = 0;

	double dAcbl = 0.00;

	char cOpt[4];
	char cMessage[141];
	char cMobile[16];

	char fldstr[5][128];

	struct mob_sendmail_c sMob_sendmail;

	memset( cOpt, 0x0, sizeof(cOpt) );
	memset( cMessage, 0x0, sizeof(cMessage) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( fldstr, 0x0, sizeof(fldstr) );
	memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );
	memset( &g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c) );
	
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

		i= i + 1;
	}

	ret = sql_count( "mob_acct_type", "mobile = '%s' and \
			ac_no = '%s'", cMobile, fldstr[i] ); 
	if ( ret <= 0 )
	{
		strcpy( cMessage,"���˺Ŷ�Ӧ���ֻ�û�п�ͨ��������ҵ��!" );
		sprintf( acErrMsg, "���˺Ŷ�Ӧ���ֻ�û�п�ͨ��������ҵ��!" );
		WRITEMSG
		goto ErrExit;
	}

	/*--- fldstr[1]�˺� fldstr[2]�˺���� fldstr[3]���� ---*/
	strcpy ( g_pub_tx.ac_no , fldstr[1] );
	strcpy ( g_pub_tx.name , "���ſ�ͷ��ʧ" );
	strcpy ( g_pub_tx.draw_pwd, fldstr[3] );

	ret = pub_acct_loss( 11, 0 );	/*-- ֻ�����ͷ��ʧ --*/
	if( ret )
	{
		sprintf( cMessage, "�𾴵Ŀͻ�:������(��)��%s��ͷ��ʧʧ��,��˶���(��)�ź�������ٹ�ʧ����ɳ����ҵ����", fldstr[1] );
		sprintf( acErrMsg,"���ж��ſ�ͷ��ʧ����!!!" );
		WRITEMSG
		goto ErrExit;
	}

	memset( cMessage, 0x0, sizeof(cMessage) );
	sprintf( cMessage, "�𾴵Ŀͻ�: �����ʺ�%s��ͷ��ʧ�ɹ�,����7���ڵ����й�̨������ʽ��ʧ����ɳ����ҵ����", fldstr[1] );

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
	sprintf(acErrMsg,"���ſ�ͷ��ʧ�ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data( "0760", cMessage );
	sprintf(acErrMsg,"���ſ�ͷ��ʧʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
