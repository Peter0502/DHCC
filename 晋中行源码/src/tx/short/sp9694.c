/*************************************************
* �� �� ��:  sp9694.c
* ���������� ����������ѯ
*
* ��    ��:  rob 
* ������ڣ� 2003��9��27��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_finance_info_c.h"

int sp9694()
{
	int	ret;

	struct mo_finance_info_c sMo_finance_info;

	memset(&sMo_finance_info, 0x0, sizeof(struct mo_finance_info_c));

    pub_base_sysinit();

	get_zd_long("0440", &g_pub_tx.old_trace_no);

	ret = Mo_finance_info_Sel(g_pub_tx.reply, &sMo_finance_info,
		"trace_no=%ld and tx_date=%ld", g_pub_tx.old_trace_no,
		g_pub_tx.tx_date);
	if ( ret == 100 )
	{
		sprintf(acErrMsg, "������Ҫ��������ˮ��!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "S045");
		goto ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg, "��ѯmo_finance_info�쳣!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	if (sMo_finance_info.sts[0] == '*')
	{
		sprintf(acErrMsg, "�ñ�ҵ���Ѿ�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "S052");
		goto ErrExit;
	}
	set_zd_data("0330", sMo_finance_info.pay_no);

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
