/***************************************************************
* �� �� ��:     spH601.c
* ����������	���˱�Ž���
*
* ��    ��:     jack
* ������ڣ�    2004��1��27��
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

spH601()
{
	int ret = 0;
	char per_name[51];
	char ac_type[2];
	char acct_no[26];
	char ac_pwd[7];
	char per_pwd[7];
	char per_no[13];

	struct ag_peracctinfo_c sPerAcctInfo; /*������Ϣ��*/

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
	get_zd_data("0250",per_name);
	get_zd_data("0670",ac_type);
	get_zd_data("0300",acct_no);
	get_zd_data("0790",ac_pwd);
	get_zd_data("0800",per_pwd);

	/*---  ��mdm_ac_rel���ж��˻��������ȷ��  ---*
	ret = pub_base_check_drawtype(acct_no,"","",ac_pwd);
	{
		sprintf(acErrMsg,"�˻�֧ȡ�����!!![%s]",g_pub_tx.reply);
		WRITEMSG
	}
	---------------------------------------------*/

	/*---  �����˺źͻ�����Ӧ�ĸ��˱���Ƿ����  ---*/
	ret = sql_count("ag_peracctinfo", "per_name = '%s' and bank_acct_no = '%s' and per_stat ='0'", per_name, acct_no);
	if (ret > 0)
	{
		sprintf(acErrMsg,"���˺Ż�����Ӧ�ĸ��������Ѵ���!!![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H001");
		goto ErrExit;
	}

	/*---  ���ɸ��˱��  ---*/
	ret = pub_agent_perno(per_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"���ɸ��˱�ų���![%s]",g_pub_tx.reply);
		WRITEMSG
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

	ret = Ag_peracctinfo_Ins(sPerAcctInfo,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"������˱���Ϣ�����![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	/*** ������� ***/
	set_zd_data("0590",per_no);
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�������˱�ųɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�������˱��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
