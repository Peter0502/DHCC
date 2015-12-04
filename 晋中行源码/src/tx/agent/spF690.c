/*************************************************
* �� �� ��: spF690.c
* ���������������������ʵ���ӡ
*
* ��    ��: jack
* �������: 2004��06��28��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <sys/types.h>
#include <errno.h>
#include "ag_itemlist_c.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"

spF690()  
{ 	
	
	int ret=0;

	char cUnit_no[5];
	char cItem_no[21];
	long lDate = 0;
	char cDate[7];

	char filename[100];
	FILE *fp;
	int i=0;

	struct ag_itemlist_c sAg_itemlist;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;

	memset( cItem_no, 0x0, sizeof(cItem_no) );
	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	memset( cDate, 0x0, sizeof(cDate) );

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );
	get_zd_long( "0460", &lDate );

	sprintf( cDate, "%ld", lDate );

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sAg_unitinfo, \
			"unit_no = '%s' and unit_stat = '0'", cUnit_no );
	if ( ret )
	{
    	sprintf( acErrMsg, "---------->��ѯ��λ��Ϣ����[%d]!!!", ret );
    	WRITEMSG
		goto ErrExit;
	}

	memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
	ret = Ag_peritemmap_Sel( g_pub_tx.reply, &sAg_peritemmap, \
			"unit_no = '%s' and item_no = '%s' and stat = '0'", \
			cUnit_no, cItem_no );
	if ( ret )
	{
    	sprintf( acErrMsg, "---------->��ѯ������Ϣ����[%d]!!!", ret );
    	WRITEMSG
		goto ErrExit;
	}

	memset( &sAg_itemlist, 0x0, sizeof(sAg_itemlist) );
	ret = Ag_itemlist_Sel( g_pub_tx.reply, &sAg_itemlist, \
			"unit_no = '%s' and item_no = '%s' and owe_month = '%s'", \
			cUnit_no, cItem_no, cDate );
	if ( ret )
	{
    	sprintf( acErrMsg, "--->��ѯ����������ϸ��¼����[%d]!!!", ret );
    	WRITEMSG
		goto ErrExit;
	}

	memset(filename,0x0,sizeof(filename));
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
    	sprintf( acErrMsg, "--->��д�������ļ�����!!!" );
    	WRITEMSG
		goto ErrExit;
	}

	fprintf( fp, "\n\n					��	��	��\n\n" );

	pub_base_strpack( sAg_peritemmap.name );
	pub_base_strpack( sAg_peritemmap.ac_no );
	fprintf( fp, "����:%s		����:%s		�˺�:%s\n",  \
		sAg_peritemmap.name, sAg_itemlist.trn_date, \
		sAg_peritemmap.ac_no );

    sprintf( acErrMsg, "---------->��ʼ��ѯ���ʵ���ӡ��¼ ! " );
    WRITEMSG


	fprintf( fp, "���ڵ�λ���:%s				���ڵ�λ����:%s\n", \
		cUnit_no, sAg_unitinfo.unit_name );
	fprintf( fp, "�����������������Щ��������������Щ��������������Щ��������������Щ��������������Щ���������������\n" );
	fprintf( fp, "��      �������ʩ�%14.2f��      ְ���ʩ�%14.2f��      �����ʩ�%14.2f��\n", \
				sAg_itemlist.am00, sAg_itemlist.am01, sAg_itemlist.am03 );
	fprintf( fp, "�����������������੤�������������੤�������������੤�������������੤�������������੤��������������\n" );
	fprintf( fp, "��      ���乤�ʩ�%14.2f��δ���������%14.2f��      �ʵ�������%14.2f��\n", \
				sAg_itemlist.am04, sAg_itemlist.am08, sAg_itemlist.am10 );
	fprintf( fp, "�����������������੤�������������੤�������������੤�������������੤�������������੤��������������\n" );
	fprintf( fp, "��    ס��������%14.2f��    ҽ�Ʒ�ͳ�辶%14.2f��  ���ϱ���ͳ�辶%14.2f��\n", \
				sAg_itemlist.am18, sAg_itemlist.am19, sAg_itemlist.am20 );
	fprintf( fp, "�����������������੤�������������੤�������������੤�������������੤�������������੤��������������\n" );
	fprintf( fp, "��  ָ��ʾ��ש�%14.2f��      Ӧ�����ʩ�%14.2f��      ʵ�����ʩ�%14.2f��\n", \
				sAg_itemlist.am21, sAg_itemlist.am17, sAg_itemlist.am24 );
	fprintf( fp, "�����������������ة��������������ة��������������ة��������������ة��������������ة���������������\n" );

	fclose( fp );
	set_zd_data(DC_FILE_SND,"1");

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���ɲ����������´��ļ��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"���ɲ����������´��ļ�ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
