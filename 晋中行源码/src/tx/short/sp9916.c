/*************************************************************
* �� �� ��:  sp9916.c
* ���������� ���֧����ݲ�ѯ
*			 ��Ʊ���������кŲ�ѯ
*
* ��    ��:  jane
* ������ڣ� 2003��6��12��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9916()
{
	get_sys_par_val("14",mbfetran.rdficode);
	set_zd_data("0580",mbfetran.rdficode);

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
