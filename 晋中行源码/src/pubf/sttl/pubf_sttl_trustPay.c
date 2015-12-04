/*************************************************
* �ļ���:    pubf_sttl_trustPay.c
* ���������� ����ί���տ����ճи���������           
*
* ��   ��:   jack
* ������ڣ� 2004��2��2��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_trust_pay_c.h"

/**********************************************************************
 * ��  �� ����  pub_sttl_RegMoTrustPay
 * ���� ����:   �Ǽ�ί���տ�ǼǱ�mo_trust_pay
 *             
 * ����/ʱ�䣺  jack/2004��2��2��
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

int pub_sttl_RegMoTrustPay( struct mo_trust_pay_c mo_trust_pay )
{
	int	ret;

	ret=Mo_trust_pay_Ins( mo_trust_pay, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "��ί���տ�ǼǱ������¼����!���ظ���¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B098" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Mo_trust_pay_Ins����!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}
