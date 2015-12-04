/*************************************************
* �� �� ��:  spH653.c
* ���������� �ͻ����ŷ���Ʒ�ֲ�ѯ
*
* ��    ��:  mike
* ������ڣ� 2004��04��24��
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
#include "mob_acct_type_c.h"

int spH653()
{
	char cMobile[14];
	char cSfbz[7];

	char cFileName[50];			/* ��ʱ�ļ��� */
	int	 ret = 0;
	FILE *fp;

	int num = 0;

	struct mob_acct_type_c sMob_acct_type;

	memset( &sMob_acct_type, 0x00, sizeof(sMob_acct_type) );

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	get_zd_data( "0610", cMobile );

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

	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply , "mobile = '%s' \
				order by ac_no,server_type", cMobile );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mob_acct_type_Dec_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	fprintf( fp, "~%15s|%20s|%8s|%8s|%8s|%8s|\n", "�ֻ�����", \
			"��   ��", "��������", "��������", "������Ա", "�շѱ�־" );
	
	while(1)
	{
		memset( &sMob_acct_type, 0x00, sizeof(sMob_acct_type) );
		
		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mob_acct_type_Fet_Sel��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( cSfbz, 0x0, sizeof(cSfbz) );
		if ( sMob_acct_type.sfbz[0] == '0' )
		{
			strcpy( cSfbz, "���շ�" );
		}
		if ( sMob_acct_type.sfbz[0] == '1' )
		{
			strcpy( cSfbz, "�շ�" );
		}

		fprintf(fp,"%s|%s|%s|%ld|%s|%s|\n", sMob_acct_type.mobile, sMob_acct_type.ac_no, sMob_acct_type.server_type, sMob_acct_type.tx_date, sMob_acct_type.tel, cSfbz );

		++num;

	}
	fclose(fp);

	Mob_acct_type_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */

	if (num == 0)
	{
		sprintf(acErrMsg,"δ��ѯ�����ֻ���Ӧ�ķ���Ʒ�ּ�¼!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "0001" );
	}
	else
	{
		strcpy( g_pub_tx.reply, "0000" );
	}

OkExit:
	sprintf(acErrMsg,"�����ֻ���Ӧ�ķ���Ʒ���ļ��ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	Mob_acct_type_Clo_Sel( );
	sprintf(acErrMsg,"�����ֻ���Ӧ�ķ���Ʒ���ļ�ʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
