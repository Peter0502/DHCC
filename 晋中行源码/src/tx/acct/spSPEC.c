/***************************************************************
* �� �� ��: spSPEC.c
* ���������������㽻��
* ��    ��: 
* ������ڣ�2004��8��12��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spSPEC()
{
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ڲ��ʵ��߼���![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ڲ��ʵ��߼���![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
