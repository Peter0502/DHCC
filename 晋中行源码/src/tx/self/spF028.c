/*************************************************
* �� �� ��:  spF028.c
* ���������� POS ���ѳ���
*            (ת�뵽�����ʻ����ӽ���)(���п�)
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

int spF028()
{

    int ret;

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* ���� */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* ���ѽ�� */



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

