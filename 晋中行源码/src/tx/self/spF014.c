/*************************************************
* �� �� ��:  spF014.c
* ���������� ������ATMȡ��ǰ�ļ��
*
* ��    ��:  rob
* ������ڣ� 2003��10��02��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"

int spF014()
{

    int ret;
    struct cash_mst_c sCash_mst;

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

	memset(&sCash_mst, 0x0, sizeof(struct cash_mst_c));
	get_zd_double("0400", &g_pub_tx.tx_amt1);

  	sprintf(acErrMsg,"tel = [%s], amt = [%lf]",g_pub_tx.tel, g_pub_tx.tx_amt1);
  	WRITEMSG

	ret = Cash_mst_Sel(g_pub_tx.reply, &sCash_mst, "tel='%s' and cur_no='01' \
		and sts='0'", g_pub_tx.tel);
	if (ret != 0)
	{
		sprintf(acErrMsg, "select cash_mst error [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
		goto ErrExit;
	}

	/* �������Ǯ����1000Ԫ */
	sprintf(acErrMsg, "Ǯ�����Ϊ:[%lf]", sCash_mst.bal);
	WRITEMSG
	sCash_mst.bal = sCash_mst.bal - 1000;

	if (pub_base_CompDblVal(g_pub_tx.tx_amt1, sCash_mst.bal) > 0)
	{
		sprintf(acErrMsg, "Ǯ������ȡ�����[%lf], [%lf]",
			g_pub_tx.tx_amt1, sCash_mst.bal);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
		goto ErrExit;
	}

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

