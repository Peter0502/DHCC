/*************************************************
* �� �� ��:  sp9802.c
* ���������� �ɻ����Ż��Ի�������
*
* ��    ��:  jane
* ������ڣ� 2003��1��12�� 
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

int sp9802()
{
	struct	com_branch_c	sComBranch;		/* ����������ṹ */
	int	ret;

	get_zd_data( "0910", g_pub_tx.tx_br_no );	/* ������ */

	ret=Com_branch_Sel( g_pub_tx.reply , &sComBranch ,\
			 "br_no='%s'" , g_pub_tx.tx_br_no );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "ȡ������������!\
			br_no=[%s]", g_pub_tx.tx_br_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S019" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Com_branch_Sel����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	set_zd_data( "0250", sComBranch.br_name );	/* �������� */
	set_zd_data( "0680", sComBranch.wrk_sts );	/* Ӫҵ״̬ */

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
