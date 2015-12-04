/*************************************************
* �� �� ��:  spF617.c
* ���������� ������ѯ��������
*
* ��    ��:  jack
* ������ڣ� 2004��08��29��
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
#include "ag_peritemmap_c.h"

int spF617()
{
	char cUnit_no[5];
	char cName[51];

	char cFileName[50];			/* ��ʱ�ļ��� */
	int	 ret = 0;
	FILE *fp;

	int num = 0;

	struct ag_peritemmap_c sAg_peritemmap;

	memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	memset( cName, 0x0, sizeof(cName) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0260", cName );

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


	if ( strlen(cName) == 0 )
	{
		ret = Ag_peritemmap_Dec_Sel( g_pub_tx.reply , \
			"unit_no = '%s'", cUnit_no );
	}
	else
	{
		ret = Ag_peritemmap_Dec_Sel( g_pub_tx.reply , \
			"unit_no = '%s' and name = '%s'", cUnit_no, cName );
	}
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_peritemmap_Dec_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	fprintf( fp, "~%s|%s|%s|\n", "���˱��","�˺�","����" );
	
	while(1)
	{
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		
		ret = Ag_peritemmap_Fet_Sel( &sAg_peritemmap, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Ag_peritemmap_Fet_Sel��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%s|\n", sAg_peritemmap.item_no, sAg_peritemmap.ac_no, sAg_peritemmap.name );

		++num;

	}
	fclose(fp);

	Ag_peritemmap_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */

	if (num == 0)
	{
		sprintf( acErrMsg,"����λ�޸��˴�������!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D104" );
	}
	else
	{
		strcpy( g_pub_tx.reply, "0000" );
	}

OkExit:
	sprintf(acErrMsg,"�����������������ļ��ļ��ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	Ag_peritemmap_Clo_Sel( );
	sprintf(acErrMsg,"�����������������ļ��ļ�ʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
