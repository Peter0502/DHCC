/***************************************************************
* �� �� ��:     spA012.c
* ����������	���˺�
*
* ��    ��:     jack
* ������ڣ�    2004��1��18��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spA012()
{
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	get_zd_data("1039",g_pub_tx.ac_no);	/*��ʾ�˺�*/
	vtcp_log("[%s]",g_pub_tx.ac_no);
	get_zd_data("0230",g_pub_tx.prdt_code);
	get_zd_data("0210",g_pub_tx.cur_no);
	get_zd_data("0250",g_pub_tx.name);

	strcpy(g_pub_tx.ac_type,"D");

	get_zd_data("0710",g_pub_tx.intst_type);

	strcpy(g_pub_tx.add_ind,"1");
	g_pub_tx.tx_amt1 = 0.00;

	if( pub_acct_opn_ac() )
	{
		sprintf(acErrMsg,"��������!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("0320",g_pub_tx.ac_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ڲ��˻��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���ڲ��˻�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

