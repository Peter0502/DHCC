/*************************************************
* �� �� ��:  spB507.c
* ���������� �жϲ�������ʽ
*
* ��    ��:  rob
* ������ڣ� 2003��7��06��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"

int spB507()
{
    int ret;
	char flag[2];

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

	get_zd_data("0670", flag);

	if (flag[0] == '1')
	{
		/* ͬ�� */
		sprintf(acErrMsg, "��������ʽΪ: [ͬ��]");
		WRITEMSG
		goto OkExit1;
	}
	else if (flag[0] == '2')
	{
		/* ��� */
		sprintf(acErrMsg, "��������ʽΪ: [���]");
		WRITEMSG
		goto OkExit;
	}
	else
	{
		sprintf(acErrMsg, "��������ʽ����!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg," ��� success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg," ͬ�� success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

