/***************************************************************
* �� �� ��:     spA015.c
* ����������	�ֽ����(����)
* ��    ��:     jack
* ������ڣ�    2004��1��19��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spA015()
{
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	strcpy(g_pub_tx.add_ind,"0");

	g_reply_int=pub_acct_cash();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�����ֽ��ո�����ʧ��!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ֽ𸶳��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ֽ𸶳�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
