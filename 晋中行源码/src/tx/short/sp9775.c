/*************************************************
* �� �� ��:  sp9775.c
* ���������� �ɻ����Ż��Ի�������
*            82��ĵ�11λ�ŵ��ǻ�������
* ��    ��:  rob 
* ������ڣ� 2003��5��21�� 
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

int sp9775()
{
	struct	com_branch_c	sComBranch;		/* ����������ṹ */
	int	ret;
	char cBrsts[50];
	memset(cBrsts, 0 , sizeof(cBrsts));

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

	set_zd_data( "0830", sComBranch.br_name );	/* �������� */
	memcpy(cBrsts,sComBranch.wrk_sts,sizeof(sComBranch.wrk_sts)-1);
	pub_base_full_space(cBrsts,sizeof(cBrsts)-1);
	cBrsts[10]=sComBranch.br_type[0];
	set_zd_data( "0820", cBrsts);
	vtcp_log("%s,%d brsts===[%s]",__FILE__,__LINE__,cBrsts);

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
