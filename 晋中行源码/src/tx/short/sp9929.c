/*************************************************************
* �� �� ��:  sp9929.c
* ���������� ���֧����ݲ�ѯ
*			 ��Ʊǩ����Ʊ������
*
* ��    ��:  jane
* ������ڣ� 2003��6��20��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************************/

#define EXTERN
#include "public.h"

int sp9929()
{
	char billcode[9];

	pub_base_sysinit();

	get_zd_data( "0860", billcode );

	/*���û����ù�Ա�Ƿ��и�������ƾ֤����һ��*/
	g_reply_int = pub_com_ChkNtExt( billcode, billcode, "110", g_pub_tx.tx_br_no, g_pub_tx.tel );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"���ú�������Ʊ�������!");
		WRITEMSG
		goto ErrExit;
	}

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
