#include "find_debug.h" 
/*************************************************
* �� �� ��:	pubf_base_tel
* ��������
*                
* ��    ��:	
* ������ڣ�	2003��12��16��
* �޸ļ�¼��	
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*************************************************/

#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

/**********************************************************************
* ��������	pub_base_GetTelInfo()
* �������ܣ�	���ݹ�Ա�����øò���Ա�ļ�¼
* ����/ʱ�䣺	Terry/2003.1.6
* 
* ������
*     ���룺
*	    	char	*tel	����Ա
*  
*     �����
*		struct com_tel_c *com_tel  ��Ա����
*		g_pub_tx.reply	��Ӧ����
*     
* �� �� ֵ: 0 �ɹ�  -1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int	pub_base_GetTelInfo( char *tel, struct com_tel_c *com_tel )
{
	int	ret;
	
	ret = Com_tel_Sel(g_pub_tx.reply, com_tel, "tel = '%s'", tel );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�˹�Ա������" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "S009" );
		return(-1);
	}else if ( ret )
	{
		sprintf( acErrMsg, "��ѯ��Ա���!ret=[%d]", ret );
		WRITEMSG
		return(-1);
	}
	return(0);
}
