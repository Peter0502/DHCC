#include "find_debug.h" 
/*************************************************
* �� �� ��: pubf_base_sysparm
* �������������ֹ���ϵͳ��ļ�飻
* ��    ��:	Terry
* ������ڣ�2003��1��15��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include "stdio.h"
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"

/**********************************************************************
 * �� �� ����   pub_base_GetSysparm
 * �������ܣ�   ȡ����������
 * ����/ʱ�䣺
 *
 * ������
 *     ����: 
 *     ���: ���������ṹ
 *     ����: -1ʧ�� 0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_base_GetSysparm( struct com_sys_parm_c *com_sys_parm )
{
	int	ret;

	ret = Com_sys_parm_Sel( g_pub_tx.reply, com_sys_parm , "1 = 1");
 	if ( ret == 100 )
 	{
 		sprintf(acErrMsg,"������������û������!");
 		WRITEMSG
 		strcpy ( g_pub_tx.reply , "G001" );
 		return -1;
 	}
 	else if ( ret )
 	{
 		sprintf(acErrMsg,"ȡ�����������!" );
 		WRITEMSG
 		return -1;
 	}

 	return (0);
}
