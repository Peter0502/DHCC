/***************************************************************
* �� �� ��:     sp8606.c
* ����������	�ж��Ƿ���֧�й���TIPS�˻�
* ��    ��:     ZHGF
* ������ڣ�    2009��6��27��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
sp8606()
{
	int             ret = 0;

	if (memcmp(g_pub_tx.tx_br_no,"10001", 5)){
		goto ErrExit2;
	}else {
		goto ErrExit2;
	}

ErrExit:
	strcpy(g_pub_tx.reply, "P156");
	sprintf(acErrMsg, "TIPS�����ж�ʧ��![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
ErrExit1:
	strcpy(g_pub_tx.reply, "0001");
	sprintf(acErrMsg, "֧�й��ɳɹ�![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply, "0002");
	sprintf(acErrMsg, "���гɹ�![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
}
