/*************************************************
* �ļ���:    pubf_sttl_draft.c
* ���������� �жһ�Ʊ��������           
*
* ��   ��:   jack
* ������ڣ� 2004��1��17��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "auth_oper_brno_c.h"
#include "mo_po_qry_c.h"

/**********************************************************************
 * ��  �� ����  pub_sttl_RegMoBankAccPo
 * ���� ����:   �Ǽǳжһ�Ʊ�ǼǱ�mo_bank_acc_po
 *             
 * ����/ʱ�䣺  jack/2004��1��17��
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

int pub_sttl_RegMoBankAccPo( struct mo_bank_acc_po_c mo_bank_acc_po )
{
	int	ret;

	ret=Mo_bank_acc_po_Ins( mo_bank_acc_po, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "��жһ�Ʊ�ǼǱ������¼����!���ظ���¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B035" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Mo_bank_po_acc_Ins����!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}
