/*************************************************
* �ļ���:    pubf_sttl_fax.c
* ���������� ���д��洦������           
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
#include "mo_fax_c.h"

/**********************************************************************
 * ��  �� ����  pub_sttl_RegMoFax
 * ���� ����:   �Ǽ����д���ǼǱ�mo_fax
 *             
 * ����/ʱ�䣺  jack/2004��1��27��
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

int pub_sttl_RegMoFax( struct mo_fax_c mo_fax )
{
	int	ret;

	ret=Mo_fax_Ins( mo_fax, g_pub_tx.reply );
	if( ret == -239 )
    	{
        	sprintf( acErrMsg, "�����д���ǼǱ������¼����!���ظ���¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B075" );
        	return (-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Mo_fax_Ins����!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

	return 0;
}
