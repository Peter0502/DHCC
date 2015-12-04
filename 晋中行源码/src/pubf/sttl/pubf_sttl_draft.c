/*************************************************
* �ļ���:    pubf_sttl_draft.c
* ���������� ���л�Ʊ��������           
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
#include "mo_bank_po_c.h"
#include "mo_po_qry_c.h"

/**********************************************************************
 * ��  �� ����  pub_sttl_RegMoBankPo
 * ���� ����:   �Ǽ����л�Ʊ�ǼǱ�mo_bank_po
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

int pub_sttl_RegMoBankPo( struct mo_bank_po_c mo_bank_po )
{
	int	ret;

	ret=Mo_bank_po_Ins( mo_bank_po, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "�����л�Ʊ�ǼǱ������¼����!���ظ���¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B002" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Mo_bank_po_Ins����!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}

/**********************************************************************
 * ��  �� ����  pub_sttl_RegMoPoQry
 * ���� ����:   �Ǽǲ�ѯ�鸴�ǼǱ�mo_po_qry
 *             
 * ����/ʱ�䣺  jack/2004��1��19��
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

int pub_sttl_RegMoPoQry( struct mo_po_qry_c mo_po_qry )
{
	int	ret;

	ret=Mo_po_qry_Ins( mo_po_qry, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "���ѯ�鸴�ǼǱ������¼����!���ظ���¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B020" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Mo_po_qry_Ins����!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}
