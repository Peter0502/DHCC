/*************************************************
* �� �� ��:  spF029.c
* ���������� POS ���ѳ���
*            (�жϸ��̻��Ǳ��л�������)
*
* ��    ��:  rob
* ������ڣ� 2003��2��21��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"

int spF029()
{

    char flag;

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_char("0670",&flag); /* ������Դ */
    if (flag == '1')
    {
        sprintf(acErrMsg,"���ò��Ǳ����̻��ӽ���!!");
        goto OkExit1;
    }


OkExit:
    /* �����̻� */
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    /* �����̻� */
    strcpy( g_pub_tx.reply, "0001" );
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

