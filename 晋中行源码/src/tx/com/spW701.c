/***************************************************************
* �� �� ��:     spW701.c
* ����������	���һ��ж��������������ֽ�ת��
* ��    ��:     andy
* ������ڣ�    2004��4��08��
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

spW701()
{
	char ct_ind[2];
	get_zd_data("0700",ct_ind);
		vtcp_log("[%d]ct_ind=[%s]",__LINE__,ct_ind);
	if( ct_ind[0]=='1' )
		{vtcp_log("[%d]ct_ind=[%s]",__LINE__,ct_ind);
		goto ErrExit1;
		}
	else if( ct_ind[0]=='2' )
		{vtcp_log("[%d]ct_ind=[%s]",__LINE__,ct_ind);
		goto ErrExit2;
		}

ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"�ж���ת��־ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}
