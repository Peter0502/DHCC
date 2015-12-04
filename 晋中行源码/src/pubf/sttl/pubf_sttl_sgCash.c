/*************************************************
* �ļ���:    pubf_sttl_cashier.c
* ���������� ���Ҳִ�������           
*
* ��   ��:   jack
* ������ڣ� 2004��1��26��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_sg_cash_po_c.h"

/**********************************************************************
 * ��  �� ����  pub_sttl_RegMoCashier
 * ���� ����:   �ǼǱ��ҲֵǼǱ�mo_sg_cash_po
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
int pub_sttl_RegMoSgCash( struct mo_sg_cash_po_c mo_sg_cash_po )
{
	int	ret;

	ret=Mo_sg_cash_po_Ins( mo_sg_cash_po, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "�򱣶ҲֵǼǱ������¼����!���ظ���¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B069" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Ins����![%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}
