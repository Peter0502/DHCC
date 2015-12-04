/*************************************************
* �� �� ��: spH676.c
* ��������������ҵ�񴫵�����(������)���� 
*
* ��    ��: mike
* ������ڣ�2004��09��12��
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
#include "ag_peritemmap_c.h"

int spH676()
{
	int ret = 0;
	char tmpstr[32];

	char cUnit_no[5];
	
	char filename[128];
	FILE *fp;

	struct ag_peritemmap_c sAg_peritemmap;

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;
	}

	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	get_zd_data( "0920", cUnit_no );
	
	memset( filename, 0x0, sizeof(filename) );
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;
	}
	
	ret = Ag_peritemmap_Dec_Sel( g_pub_tx.reply , "unit_no = '%s'" , cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_peritemmap_Dec_Sel����!!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Fet_Sel( &sAg_peritemmap, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "ִ��Ag_peritemmap_Fet_Sel����!!!" );
			WRITEMSG
			Ag_peritemmap_Clo_Sel();
			goto ErrExit;
		}
		
		pub_base_strpack( sAg_peritemmap.item_no );
		pub_base_strpack( sAg_peritemmap.ac_no );
		fprintf( fp, "%s|%s|%s|\n", cUnit_no, sAg_peritemmap.item_no, sAg_peritemmap.ac_no );
	}
	
	fclose( fp );
	
	Ag_peritemmap_Clo_Sel();	

	set_zd_data(DC_FILE_SND,"1");

OkExit:
	sprintf( acErrMsg,"�������ϴ��ݴ���ɹ�!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�������ϴ��ݴ���ʧ��[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
