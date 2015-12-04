/*************************************************
* �� �� ��:  spH654.c
* ���������� ��ѯ���ж��ŷ���Ʒ��
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
#include "mob_server_type_c.h"

int spH654()
{
	char cMobile[14];

	char cFileName[50];			/* ��ʱ�ļ��� */
	int	 ret = 0;
	FILE *fp;

	int num = 0;

	struct mob_server_type_c sMob_server_type;

	memset( &sMob_server_type, 0x00, sizeof(sMob_server_type) );

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

	ret = Mob_server_type_Dec_Sel( g_pub_tx.reply , \
			"1=1 order by server_type" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mob_server_type_Dec_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	fprintf( fp, "~%s|%s|%s|%s|%s|\n", "�����������", \
			"������������", "�м�¼�շѱ�׼(����)", \
			"�޼�¼�շѱ�׼(����)", "����ѱ�׼" );
	
	while(1)
	{
		memset( &sMob_server_type, 0x00, sizeof(sMob_server_type) );
		
		ret = Mob_server_type_Fet_Sel( &sMob_server_type, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mob_server_type_Fet_Sel��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%.2f|%.2f|%.2f|\n", sMob_server_type.server_type, sMob_server_type.servername, sMob_server_type.sfbz_yes, sMob_server_type.sfbz_no, sMob_server_type.sfbz_mon );

		++num;

	}
	fclose(fp);

	Mob_server_type_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */

	if (num == 0)
	{
		sprintf(acErrMsg,"�����޶��ŷ���Ʒ�ּ�¼!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "0001" );
	}
	else
	{
		strcpy( g_pub_tx.reply, "0000" );
	}

OkExit:
	sprintf(acErrMsg,"���ɱ��з���Ʒ���ļ��ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	Mob_server_type_Clo_Sel( );
	sprintf(acErrMsg,"���ɱ��з���Ʒ���ļ�ʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
