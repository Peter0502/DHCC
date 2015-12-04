/*************************************************
* �ļ���:    pubf_sttl_draft.c
* ���������� ���л�Ʊ�������ּ�鴦������          
*
* ��   ��:   jack
* ������ڣ� 2004��1��18��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "auth_oper_brno_c.h"

/**********************************************************************
 * ��  �� ����  pub_sttl_ChkInputBrno
 * ���� ����:   ����Ʊ¼������Ƿ����Ȩ��
 *             
 * ����/ʱ�䣺  jack/2004��1��17��
 * 
 * ��  ����
 *     ���룺 
 *		char *tx_brno		��Ҫ���Ļ������
 *     ����� 
 *		char	g_pub_tx.reply	��Ӧ��
 *
 *   ����ֵ��  0: �ɹ�  -1: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_sttl_ChkInputBrno( char *oper_code, char *tx_brno )
{
	int	ret;

	ret = sql_count( "auth_oper_brno", "oper_code='%s' and dw_brno='%s'",\
			 oper_code, tx_brno );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "��ѯauth_oper_brno��,ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B015" );
		return(-1);	
	}else if ( ret == 0 )
	{
		sprintf( acErrMsg, "�û�����Ȩ���˽���" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B014" );
		return(-1);	
	}

	return 0;
}

/**********************************************************************
 * ��  �� ����  pub_sttl_ChkSignBrno
 * ���� ����:   ����Ʊǩ�������Ƿ����ǩ��Ȩ��
 *             
 * ����/ʱ�䣺  jack/2004��1��17��
 * 
 * ��  ����
 *     ���룺 
 *		char *in_brno		¼��������
 *		char *sign_brno		ǩ���������
 *     ����� 
 *		char	g_pub_tx.reply	��Ӧ��
 *
 *   ����ֵ��  0: �ɹ�  -1: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_sttl_ChkSignBrno( char *oper_code, char *in_brno, char *sign_brno )
{
	int	ret;

	ret = sql_count( "auth_oper_brno", "oper_code='%s' and dw_brno='%s' \
			and up_brno = '%s'", oper_code, in_brno, sign_brno );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "��ѯauth_oper_brno��,ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B015" );
		return(-1);	
	}else if ( ret == 0 )
	{
		sprintf( acErrMsg, "�û�����Ȩ���˽���" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B014" );
		return(-1);	
	}

	return 0;
}
