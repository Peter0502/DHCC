/***************************************************************
* �� �� ��: spH627.c
* �������������˽ɷ���ϸ��ѯ
*
* ��    ��: jack
* ������ڣ�2004��03��29��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_itemlist_c.h"

spH627()
{
	int ret = 0;
	int num = 0;
	char cItem_no[21];
	char cUnit_no[5];
	char cPay_month[7];

	char cFileName[128];
	FILE *fp;

	struct ag_itemlist_c sAg_itemlist; /*�ɷ���Ϣ��ϸ��*/

	memset(&sAg_itemlist,0x0,sizeof(sAg_itemlist));
	memset(cItem_no,0x0,sizeof(cItem_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cPay_month,0x0,sizeof(cPay_month));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );

	/*---------------- ��Ҫ�����Ȳ������ ------------------*
	ret = Ag_itemlist_Ins(sAg_itemlist,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"����ɷ���ϸ�����![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	*---------------- ��Ҫ�����Ȳ������ ------------------*/

	memset( cFileName, 0x00, sizeof( cFileName ) );

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

	ret = Ag_itemlist_Dec_Sel( g_pub_tx.reply , "item = '%s' and \
			unit_no = '%s' and stcd = '1'", cItem_no, cUnit_no);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_itemlist_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%20s|%10s|%10s|%12s|\n", \
				"�ɷѺ���","Ƿ������","�ɷ�����","�ɷѽ��");

	while(1)
	{
		memset( &sAg_itemlist, 0x00, sizeof(sAg_itemlist));
		
		ret = Ag_itemlist_Fet_Sel( &sAg_itemlist, g_pub_tx.reply );
		if ( ret == 100)
		{
			if (num == 0)
			{
				sprintf( acErrMsg, "�ɷ���ϸ������!!!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "H013");
				goto ErrExit;
			}
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Ag_itemlist_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%s|%d|\n", sAg_itemlist.item_no, sAg_itemlist.owe_month, sAg_itemlist.trn_date, sAg_itemlist.am06);

		num++;
	}
	Ag_itemlist_Clo_Upd( );

	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���˽ɷ���Ϣ��ѯ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���˽ɷ���Ϣ��ѯʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
