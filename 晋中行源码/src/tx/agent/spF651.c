/*************************************************
* �� �� ��:  spF651.c
* ���������� ��ѯ��������δ������Ϣ(��������)
*
* ��    ��:  jack
* ������ڣ� 2004��03��25��
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mob_sendmail_c.h"

int spF651()
{
	char cFileName[50];			/* ��ʱ�ļ��� */
	int	 ret = 0;
	FILE *fp;

	int num = 0;

	struct mob_sendmail_c sMob_sendmail;

	memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );

	memset( cFileName, 0x00, sizeof( cFileName ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ���ݹ�Ա�š��ն˺�������ʱ�ļ� */
	ret = pub_base_AllDwnFilName( cFileName ); 
	if( ret )
	{
		sprintf( acErrMsg, "�����´�ȫ·���ļ���(����)��" );
		WRITEMSG
		goto ErrExit;
	}

	fp = fopen( cFileName, "w" );
	if( fp==NULL )
	{
		sprintf( acErrMsg, "��ʱ�ļ�����!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}

	ret = Mob_sendmail_Dec_Upd( g_pub_tx.reply , "proc_flag = '0'" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mob_sendmail_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );
		
		ret = Mob_sendmail_Fet_Upd( &sMob_sendmail, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mob_sendmail_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%s|%s|%ld|%ld|%s|%s|\n", sMob_sendmail.opt_no, sMob_sendmail.mobile, sMob_sendmail.message, "0", sMob_sendmail.tx_date, sMob_sendmail.tx_time, "0", "0");

		++num;

		strcpy( sMob_sendmail.proc_flag, "1" );

		ret = Mob_sendmail_Upd_Upd( sMob_sendmail, g_pub_tx.reply  );
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Mob_sendmail_Upd_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	fclose(fp);

	Mob_sendmail_Clo_Upd( );

	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */

	if (num == 0)
	{
		sprintf(acErrMsg,"δ��ѯ����Ҫ���͵Ķ��ż�¼!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "0001" );
	}
	else
	{
		strcpy( g_pub_tx.reply, "0000" );
	}

OkExit:
	sprintf(acErrMsg,"���ɶ���δ�����ļ��ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	Mob_sendmail_Clo_Upd( );
	sprintf(acErrMsg,"���ɶ���δ�����ļ�ʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
