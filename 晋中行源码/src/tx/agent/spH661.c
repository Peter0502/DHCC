/*************************************************
* �� �� ��:  spH661.c
* ���������� ���ŷ���ͷ���Ʒ�ֽ���
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

int spH661()
{
	int ret = 0;

	char cOpt[4];
	char cMessage[141];

	char cMobile[16];

	struct mob_sendmail_c sMob_sendmail;

	memset( cOpt, 0x0, sizeof(cOpt) );
	memset( cMessage, 0x0, sizeof(cMessage) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	get_zd_data( "0760", cMessage );
	get_zd_data( "0610", cMobile );

	ret = sql_count( "mob_acct_type", "mobile = '%s'", cMobile ); 
	if ( ret <= 0 )
	{
		strcpy( cMessage,"���ֻ�û�п�ͨ��������ҵ��!" );
		sprintf( acErrMsg, "���ֻ�û�п�ͨ��������ҵ��!" );
		WRITEMSG
		goto ErrExit;
	}

	if ( strncmp(cMessage, "0001", 4) == 0 )
	{
		memset( cMessage, 0x0, sizeof(cMessage) );

		strcpy( cMessage,"��ɳ���ж���Ϊ������.�뵽���й�̨�������.�ƶ��û���0002��13874183399,��ͨ�û���0002��13054110000Ʒ�ֽ���");
	}
	if ( strncmp(cMessage, "0002", 4) == 0 )
	{
		memset( cMessage, 0x0, sizeof(cMessage) );

		strcpy( cMessage,"0001����(dxjs),0003��ѯ���(cxye),ktgs���ۿ�ͷ��ʧ,0005�鵱���ۼ�(cxlj),zxfwע������");
	}

	strcpy( sMob_sendmail.tx_code, g_pub_tx.tx_code );
	strcpy( sMob_sendmail.ac_no, "�˺�" );
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
	sprintf(acErrMsg,"���ŷ�����ܳɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data( "0760", cMessage );
	sprintf(acErrMsg,"���ŷ������ʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
