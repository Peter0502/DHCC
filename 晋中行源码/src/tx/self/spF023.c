/*************************************************
* �� �� ��:  spF023.c
* ���������� POS ����(�жϸ��̻��Ǳ��л�������)
*            POS Ԥ��Ȩ���(�жϸ��̻��Ǳ��л�������)
*            POS Ԥ��Ȩ��ɳ���(�жϸ��̻��Ǳ��л�������)
*            ATM ȡ��(�ж��Ǳ���ATM��������ATM)
*	
* ��    ��:  rob
* ������ڣ� 2003��2��19��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"

int spF023()
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
	else
	{
		/*  flag = '0';  For Test ChenMing 20070821 */
		strcpy( g_pub_tx.reply, "LS03" );
        sprintf(acErrMsg,"����û�б����̻��ӽ���!!");
        goto ErrExit;
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
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

