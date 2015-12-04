/***************************************************************
* �� �� ��:     spH602.c
* ����������	���˱��ɾ��/�޸�
*
* ��    ��:     jack
* ������ڣ�    2004��1��28��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_peracctinfo_c.h"

spH602()
{
	int ret = 0;
	char proc_flag[2];
	char tbwhere[128];

	char per_name[51];
	char ac_type[2];
	char acct_no[26];
	char ac_pwd[7];
	char per_pwd[7];
	char per_no[13];

	struct ag_peracctinfo_c sPerAcctInfo; /*������Ϣ��*/

	memset(proc_flag,0,sizeof(proc_flag));
	memset(tbwhere,0,sizeof(tbwhere));

	memset(&sPerAcctInfo,0x0,sizeof(sPerAcctInfo));
	memset(per_name,0x0,sizeof(per_name));
	memset(per_pwd,0x0,sizeof(per_pwd));
	memset(per_no,0x0,sizeof(per_no));
	memset(ac_type,0x0,sizeof(ac_type));
	memset(acct_no,0x0,sizeof(acct_no));
	memset(ac_pwd,0x0,sizeof(ac_pwd));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*-------- ȡ�������  ---------*/
	get_zd_data("0700",proc_flag);

	get_zd_data("0250",per_name);
	get_zd_data("0590",per_no);
	get_zd_data("0670",ac_type);
	get_zd_data("0300",acct_no);
	get_zd_data("0790",ac_pwd);
	get_zd_data("0800",per_pwd);

	if (proc_flag[0] == '2')	/*--- ɾ�� ---*/
	{
		sprintf(tbwhere,"per_no = '%s' ",per_no);
		ret = deldb("ag_peracctinfo",tbwhere);
		if (ret != 0)
		{
			sprintf(acErrMsg,"ɾ�����˱�Ŵ�[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H004");
			goto ErrExit;
		}
	}
	else if (proc_flag[0] == '1')	/*--- �޸� ---*/
	{
		/*---  ��mdm_ac_rel���ж��˻��������ȷ��  ---*
		ret = pub_base_check_drawtype(acct_no,"","",ac_pwd);
		{
			sprintf(acErrMsg,"�˻�֧ȡ�����!!![%s]",g_pub_tx.reply);
			WRITEMSG
		}
		---------------------------------------------*/

		ret = Ag_peracctinfo_Dec_Upd( g_pub_tx.reply , "per_no = '%s' and per_pwd = '%s'",per_no,per_pwd);
		if (ret != 0)
		{
			sprintf(acErrMsg,"�޸ĸ��˱�Ŵ�1[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H005");
			goto ErrExit;
		}

		ret = Ag_peracctinfo_Fet_Upd( &sPerAcctInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"�޸ĸ��˱�Ŵ�2[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H005");
			goto ErrExit;
		}

		strcpy(sPerAcctInfo.per_no,per_no);
		strcpy(sPerAcctInfo.per_name,per_name);
		strcpy(sPerAcctInfo.per_stat,"0");
		strcpy(sPerAcctInfo.acct_type,ac_type);
		strcpy(sPerAcctInfo.bank_acct_no,acct_no);
		strcpy(sPerAcctInfo.per_pwd,per_pwd);
		strcpy(sPerAcctInfo.opn_br_no,g_pub_tx.tx_br_no);
		sprintf(sPerAcctInfo.opn_date,"%ld",g_pub_tx.tx_date);
		strcpy(sPerAcctInfo.opn_tel,g_pub_tx.tel);
		strcpy(sPerAcctInfo.cls_date,"000000");
		strcpy(sPerAcctInfo.cls_tel,"0000");

		ret = Ag_peracctinfo_Upd_Upd( sPerAcctInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"�޸ĸ��˱�Ŵ�3[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H005");
			goto ErrExit;
		}

		ret = Ag_peracctinfo_Clo_Upd( );
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
	ret = Ag_peracctinfo_Clo_Upd( );
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ά�����˱����Ϣ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	ret = Ag_peracctinfo_Clo_Upd( );
	sprintf(acErrMsg,"ά�����˱����Ϣʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
