
/*************************************************
* �� �� ��:  sp9210.c
* ���������� �����Ա�ţ����Թ�Ա����
*
* ��    ��:  rob
* ������ڣ� 2003��07��15��
*
* �޸ļ�¼��modified by wyw 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
insert into tx_def (tx_code, tx_name, bit_map, log_ind, tx_type, wrk_sts) values ('9210', '�����Աǩ�����', '10000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100', '0', '4', '0');
insert into tx_flow_def (tx_code, flow_code, sub_tx_code, sub_tx_rslt) values ('9210', 0, '9210', '#$');
insert into tx_sub_def (sub_tx_code, sub_tx_name, tx_func, acct_ind, chnl_ind) values ('9210', '�����Աǩ�����', 'sp9210', '0', '0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "com_branch_c.h"
int sp9210()
{
	int             ret = 0;
	char            tel[TEL_LENGTH+1];

	struct com_tel_c g_com_tel;
	struct com_branch_c sCom_branch;

	memset(&g_com_tel, 0x00, sizeof(struct com_tel_c));
	memset(&sCom_branch, 0x00, sizeof(struct com_branch_c));
	pub_base_sysinit();
	ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s' and wrk_sts = '1'", g_pub_tx.tx_br_no);
	if (ret) {
		sprintf(acErrMsg, "�û���������!!tel=[%s]", g_pub_tx.tx_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply, "O050");
		goto ErrExit;
	}

	ret = Com_tel_Sel(g_pub_tx.reply, &g_com_tel, "tel='%s' and csts = '0'", g_pub_tx.tel);
	if (ret) {
		sprintf(acErrMsg, "�ù�Ա��δǩ��!!tel=[%s]", g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply, "S013");
		goto ErrExit;
	}
	set_zd_data("0250", g_com_tel.name);	/* ��Ա���� */

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
