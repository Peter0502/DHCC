/***************************************************************
* �� �� ��:     spH604.c
* ����������	���˽ɷѺ�����Ϣά��
*
* ��    ��:     jack
* ������ڣ�    2005��1��29��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_peritemmap_c.h"
#include "ag_peracctinfo_c.h"

spH604()
{
	int ret = 0;
	char proc_flag[2];
	char tbwhere[128];

	char per_pwd[7];
	char per_no[13];
	char unit_no[5];
	char item[21];
	char bat_flag[2];

	struct ag_peritemmap_c sPerItemMap;		  /*���˽ɷѺ�����Ʊ�*/

	memset(proc_flag,0,sizeof(proc_flag));
	memset(tbwhere,0,sizeof(tbwhere));

	memset(&sPerItemMap,0x0,sizeof(sPerItemMap));

	memset(per_pwd,0x0,sizeof(per_pwd));
	memset(per_no,0x0,sizeof(per_no));
	memset(unit_no,0x0,sizeof(unit_no));
	memset(item,0x0,sizeof(item));
	memset(bat_flag,0x0,sizeof(bat_flag));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*-------- ȡ�������  ---------*/
	get_zd_data("0700",proc_flag);

	get_zd_data("0590",per_no);
	get_zd_data("0800",per_pwd);
	get_zd_data("0920",unit_no);
	get_zd_data("0630",item);
	get_zd_data("0710",bat_flag);

	if (proc_flag[0] == '2')	/*--- ɾ�� ---*/
	{
		sprintf(tbwhere,"per_no = '%s' and unit_no = '%s' and item_no = '%s'",per_no,unit_no,item);
		ret = deldb("ag_peritemmap",tbwhere);
		if (ret != 0)
		{
			sprintf(acErrMsg,"ɾ�����˽ɷѺ����¼��[%s][%s]",per_no,item);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H007");
			goto ErrExit;
		}
	}
	else if (proc_flag[0] == '1')	/*--- �޸� ---*/
	{

		ret = Ag_peritemmap_Dec_Upd( g_pub_tx.reply , "per_no = '%s' and unit_no = '%s' and item_no = '%s'",per_no,unit_no,item);
		if (ret != 0)
		{
			sprintf(acErrMsg,"�޸ĸ��˽ɷѺ����1[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		ret = Ag_peritemmap_Fet_Upd( &sPerItemMap, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"�޸ĸ��˽ɷѺ����2[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		strcpy(sPerItemMap.per_no,per_no);
		strcpy(sPerItemMap.unit_no,unit_no);
		strcpy(sPerItemMap.item_no,item);
		strcpy(sPerItemMap.bat_flag,bat_flag);
		strcpy(sPerItemMap.stat,"1");
		opn_date=g_pub_tx.tx_date;
		strcpy(sPerItemMap.opn_tel,g_pub_tx.tel);
		sPerItemMap.cls_date=0;
		strcpy(sPerItemMap.cls_tel,"0000");

		ret = Ag_peritemmap_Upd_Upd( sPerItemMap, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"�޸ĸ��˽ɷѺ����3[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		ret = Ag_peritemmap_Clo_Upd( );
	}
	else if (proc_flag[0] == '0')
	{
		strcpy(sPerItemMap.per_no,per_no);
		strcpy(sPerItemMap.unit_no,unit_no);
		strcpy(sPerItemMap.item_no,item);
		strcpy(sPerItemMap.bat_flag,bat_flag);
		strcpy(sPerItemMap.stat,"0");
		sPerItemMap.opn_date=g_pub_tx.tx_date;
		strcpy(sPerItemMap.opn_tel,g_pub_tx.tel);
		sPerItemMap.cls_date=0;
		strcpy(sPerItemMap.cls_tel,"0000");

		ret = Ag_peritemmap_Ins(sPerItemMap,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"������˽ɷѺ�����[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"�������ʹ�[%s]",proc_flag);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H006");
		goto ErrExit;
	}

	/*** ������� ***/
	set_zd_data("0590",per_no);
OkExit:
	ret = Ag_peritemmap_Clo_Upd( );
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ά�����˽ɷѺ�����Ϣ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	ret = Ag_peritemmap_Clo_Upd( );
	sprintf(acErrMsg,"ά�����˽ɷ���Ϣʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
