/*************************************************
* �ļ���:    pubf_sttl_cashier.c
* ���������� ���б�Ʊ��������           
*
* ��   ��:   jack
* ������ڣ� 2004��1��21��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_cashier_c.h"

/**********************************************************************
 * ��  �� ����  pub_sttl_RegMoCashier
 * ���� ����:   �Ǽ����б�Ʊ�ǼǱ�mo_cashier
 *             
 * ����/ʱ�䣺  jack/2004��1��21��
 * 
 * ��  ����
 *     ���룺 
 *
 *     ����� char	g_pub_tx.reply	��Ӧ��
 *
 *   ����ֵ��  0: �ɹ�  -1: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/

int pub_sttl_RegMoCashier( struct mo_cashier_c mo_cashier )
{
	int	ret;

	ret=Mo_cashier_Ins( mo_cashier, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "�����б�Ʊ�ǼǱ������¼����!���ظ���¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B040" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Mo_cashier_Ins����!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}
