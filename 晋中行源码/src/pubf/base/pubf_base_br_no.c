#include "find_debug.h" 
/*************************************************
* �� �� ��:     pub_base_br_no
* ����������    1������ģ��pub_base_get_br_parm���ݻ�������ȡ����������
* ��    ��:
* ������ڣ�    2003��12��30��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include "stdio.h"
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

/**********************************************************************
 * �� �� ����   pub_base_get_br_parm
 * �������ܣ�   ���ݻ�������ȥ����������
 * ����/ʱ�䣺
 *
 * ������
 *     ����: ��������
 *     ���: ��������
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_base_get_br_parm(char br_no[6],struct com_branch_c *com_branch_c)
{

 MEMSET_DEBUG(com_branch_c,0x00,sizeof(struct com_branch_c));
 pub_base_strpack(br_no);

 g_reply_int=Com_branch_Sel( com_branch_c," br_no='%s' ",br_no);
 if ( g_reply_int==100 )
 {
 	sprintf(acErrMsg,"�û�����ҵ��ϵͳ�в�����!br_no=[%s]",br_no);
 	WRITEMSG
 	strcpy ( g_pub_tx.reply , "S019" );
 	return -1;
 }
 else if ( g_reply_int )
 {
 	sprintf(acErrMsg,"����������쳣!br_no=[%s][%d]",br_no,g_reply_int);
 	WRITEMSG
 	strcpy ( g_pub_tx.reply , "S019" );
 	return -1;
 }

 return (0);
}

/**********************************************************************
 * ��  �� ����  pub_base_ChkBrnoLvl
 * ���� ����:   �����������֮��ļ����ϵ
 *             
 * ����/ʱ�䣺  Terry.Cui/2005��1��7��
 * 
 * ��  ����
 *     ���룺  
 *		char	*dw_br_no	�¼�����
 *		char	*up_br_no	�ϼ�����
 *
 *     �����  
 *            	char    *g_pub_tx.reply	��Ӧ��
 *
 *   ����ֵ��   0: ���ϼ�����
 *	       -1: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_base_ChkBrnoLvl( char *dw_br_no, char *up_br_no )
{
	struct  com_branch_c	sComBranch;	/* �������� */
	
	int	iret;
	
	MEMSET_DEBUG( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	
	if ( !strcmp( dw_br_no, up_br_no ) )
	{
		sprintf( acErrMsg, "��������ͬ" );	
		WRITEMSG
		return(-1);		
	}
	
	iret = Com_branch_Sel( g_pub_tx.reply , &sComBranch, "br_no = '%s'", \
				dw_br_no );
	if ( iret == 100 )
	{
		sprintf( acErrMsg, "�·�����������" );	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N048" );
		return(-1);		
	}else if ( iret )
	{
		sprintf( acErrMsg, "���һ������,iret=[%d]", iret );	
		WRITEMSG
		return(-1);
	}

	if ( strcmp( sComBranch.up_br_no, up_br_no ) )
	{
		sprintf( acErrMsg, "��������֮�䲻�����¼���ϵ" );	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N047" );
		return(-1);	
	}	
	return(0);
}

/**********************************************************************
 * ��  �� ����  chk_br_no
 * ���� ����:   ����������
 *             
 * ����/ʱ�䣺  Terry.Cui/2005��1��6��
 * 
 * ��  ����
 *     ���룺  
 *		char	*br_no		��������
 *     �����  
 *            	char    *g_pub_tx.reply	��Ӧ��
 *
 *   ����ֵ��  0: ����   1: ����֧��  2: ����  -1: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_base_ChkBrno( char *br_no )
{
	struct  com_branch_c	sComBranch;	/* �������� */

	int	iType;				/* ���ػ������� */	
	int	ret;
	
	MEMSET_DEBUG( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	
	ret = Com_branch_Sel(g_pub_tx.reply,&sComBranch,"br_no ='%s'",br_no);
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˻�������,br_no=[%s]", br_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S019" );
		return(-1);
	}else if ( ret )
	{
		sprintf( acErrMsg, "���ҹ����������,ret=[%d]", ret );	
		WRITEMSG
		return(-1);
	}
	
	return( sComBranch.br_type[0] );
}

