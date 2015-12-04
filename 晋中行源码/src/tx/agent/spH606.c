/*************************************************
* �� �� ��:  spH606.c
* ���������� ��ɸ��˱����Ϣ��ѯ
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
#include "ag_peracctinfo_c.h"
#include "ag_peritemmap_c.h"

int spH606()
{
	struct ag_peracctinfo_c sPerAcctInfo;
	struct ag_peritemmap_c sPerItemMap;

	char	cFileName[50];			/* ��ʱ�ļ��� */
	int	ret = 0;
	FILE 	*fp;

	char cPer_no[13];
	char cStat[7];

	memset( &sPerAcctInfo,0x0,sizeof(sPerAcctInfo));
	memset( &sPerItemMap,0x0,sizeof(sPerItemMap));

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cPer_no ,0x0, sizeof(cPer_no));
	
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

	get_zd_data("0590",cPer_no);

		sprintf( acErrMsg, "���˱��Ϊ[%s]!!!!!\n\n",cPer_no);
		WRITEMSG
	
	ret = Ag_peritemmap_Dec_Sel( g_pub_tx.reply , "per_no = '%s' order by unit_no",cPer_no);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_peritemmap_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%s|%s|%s|%s|%s|\n", \
				"��λ���","������","״̬","������","���˽ɷѺ���");

	while(1)
	{
		memset( &sPerItemMap, 0x00, sizeof(sPerItemMap));
		
		ret = Ag_peritemmap_Fet_Sel( &sPerItemMap, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Ag_peritemmap_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset(cStat,0,sizeof(cStat));
		if (sPerItemMap.stat[0] == '0')
		{
			strcpy(cStat,"����");
		}
		if (sPerItemMap.stat[0] != '0')
		{
			strcpy(cStat,"������");
		}
		
		fprintf(fp,"%s|%s|%s|%s|%s|\n",sPerItemMap.unit_no,sPerItemMap.opn_date,cStat,sPerItemMap.cls_date,sPerItemMap.item_no);

	}
	Ag_peritemmap_Clo_Upd( );

	ret = Ag_peracctinfo_Sel(g_pub_tx.reply, &sPerAcctInfo, "per_no = '%s' and per_stat = '0'", cPer_no);
	if (ret != 0)
	{
		sprintf( acErrMsg, "ִ��Ag_peracctinfo_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	memset(cStat,0,sizeof(cStat));
	if (sPerAcctInfo.per_stat[0] == '0')
	{
		strcpy(cStat,"����");
	}
	if (sPerAcctInfo.per_stat[0] != '0')
	{
		strcpy(cStat,"������");
	}
	fprintf(fp,"`���˱�� %s ���� %s ״̬ %s\n",sPerAcctInfo.per_no,sPerAcctInfo.per_name,cStat);
	fprintf(fp,"`������ %s �����˻����� %s �����˺� %s\n",sPerAcctInfo.opn_br_no,sPerAcctInfo.acct_type,sPerAcctInfo.bank_acct_no);
	fprintf(fp,"`�������� %s ������Ա %s �������� %s ������Ա %s\n",sPerAcctInfo.opn_date, sPerAcctInfo.opn_tel, sPerAcctInfo.cls_date, sPerAcctInfo.cls_tel);
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
