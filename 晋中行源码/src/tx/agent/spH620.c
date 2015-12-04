/*************************************************
* �� �� ��:  spH620.c
* ���������� ��ɵ�λ��Ϣ�ۺϲ�ѯ
*
* ��    ��:  jack
* ������ڣ� 2004��1��31��
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
#include "ag_unitinfo_c.h"
#include "ag_unitacctmap_c.h"

int spH620()
{
	struct ag_unitinfo_c sUnitInfo;
	struct ag_unitacctmap_c sUnitAcctMap;

	char	cFileName[50];			/* ��ʱ�ļ��� */
	int	ret, num=0;
	FILE 	*fp;

	char cUnit_no[5];
	char cStat[7];

	memset( &sUnitInfo,0x0,sizeof(sUnitInfo));
	memset( &sUnitAcctMap,0x0,sizeof(sUnitAcctMap));

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cUnit_no ,0x0, sizeof(cUnit_no));
	
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

	get_zd_data("0920",cUnit_no);

	ret = Ag_unitacctmap_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' order by br_no",cUnit_no);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_unitacctmap_Dec_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%s|%s|%s|%s|\n", \
				"������","����������ʱ�˺�","����","״̬");

	while(1)
	{
		memset( &sUnitAcctMap, 0x00, sizeof(sUnitAcctMap));
		
		ret = Ag_unitacctmap_Fet_Sel( &sUnitAcctMap, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Ag_unitacctmap_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset(cStat,0,sizeof(cStat));
		if (sUnitAcctMap.stat[0] == '0')
		{
			strcpy(cStat,"����");
		}
		if (sUnitAcctMap.stat[0] != '0')
		{
			strcpy(cStat,"������");
		}
		
		fprintf(fp,"%s|%s|%s|%s|\n",sUnitAcctMap.br_no,sUnitAcctMap.bank_acct_no,sUnitAcctMap.acct_type,cStat);

		num++;	
	}
	Ag_unitacctmap_Clo_Sel( );

	ret = Ag_unitinfo_Sel(g_pub_tx.reply, &sUnitInfo, "unit_no = '%s' and unit_stat = '0'", cUnit_no);
	if (ret != 0)
	{
		sprintf( acErrMsg, "ִ��Ag_unitinfo_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	fprintf(fp,"--------------------------------------------------------\n");
	fprintf(fp,"` ��λ��� %s  ��λ���� %s\n", sUnitInfo.unit_no,sUnitInfo.unit_name);
	fprintf(fp,"` �����˺����� %s  �����˺� %s\n", sUnitInfo.acct_type,sUnitInfo.bank_acct_no);
	fprintf(fp,"--------------------------------------------------------\n");

	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
