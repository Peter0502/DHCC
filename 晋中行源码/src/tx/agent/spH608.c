/***************************************************************
* �� �� ��:     spH608.c
* ����������	���˱����������
*
* ��    ��:     jack
* ������ڣ�    2004��1��30��
*
* ���ü�¼��
* ��    ��:
* �� �� ��:
* ��������:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_peracctinfo_c.h"

spH608()
{
	int ret = 0;

	char old_pwd[7];
	char new_pwd[7];
	char per_no[13];
	char acct_no[26];
	char ac_pwd[7];

	struct ag_peracctinfo_c sPerAcctInfo; /*������Ϣ��*/

	memset(&sPerAcctInfo,0x0,sizeof(sPerAcctInfo));
	memset(old_pwd,0x0,sizeof(old_pwd));
	memset(new_pwd,0x0,sizeof(new_pwd));
	memset(per_no,0x0,sizeof(per_no));
	memset(acct_no,0x0,sizeof(acct_no));
	memset(ac_pwd,0x0,sizeof(ac_pwd));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*-------- ȡ�������  ---------*/
	get_zd_data("0590",per_no);
	get_zd_data("0800",new_pwd);
	get_zd_data("0790",ac_pwd);
	get_zd_data("0300",acct_no);

	/*---  ��mdm_ac_rel���ж��˻��������ȷ��  ---*
	ret = pub_base_check_drawtype(acct_no,"","",ac_pwd);
	{
		sprintf(acErrMsg,"�˻�֧ȡ�����!!![%s]",g_pub_tx.reply);
		WRITEMSG
	}
	---------------------------------------------*/

	ret = Ag_peracctinfo_Dec_Upd( g_pub_tx.reply , "per_no = '%s'",per_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"���ø��˱�������1[%s]",per_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H005");
		goto ErrExit;
	}

	ret = Ag_peracctinfo_Fet_Upd( &sPerAcctInfo, g_pub_tx.reply );
	if (ret != 0)
	{
		sprintf(acErrMsg,"���ø��˱�Ŵ�����2[%s]",per_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H005");
		goto ErrExit;
	}

	strcpy(sPerAcctInfo.per_pwd,new_pwd);

	ret = Ag_peracctinfo_Upd_Upd( sPerAcctInfo, g_pub_tx.reply );
	if (ret != 0)
	{
		sprintf(acErrMsg,"���ø��˱�Ŵ�����3[%s]",per_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H005");
		goto ErrExit;
	}

	ret = Ag_peracctinfo_Clo_Upd( );

	/*** ������� ***/
	set_zd_data("0590",per_no);
OkExit:
	ret = Ag_peracctinfo_Clo_Upd( );
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ø��˱������ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	ret = Ag_peracctinfo_Clo_Upd( );
	sprintf(acErrMsg,"���ø��˱������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
