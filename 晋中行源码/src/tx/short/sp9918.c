/*************************************************************
* �� �� ��:  sp9918.c
* ���������� ���֧����ݲ�ѯ
*			 ��Ʊ�����˻���Ѻ���
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

int sp9918()
{
	char billofxchngseal[11];
	struct mbfebillopnreg_c mbfebillopnreg_v;

	memset(&mbfebillopnreg_v,0x00,sizeof(struct mbfebillopnreg_c));

	get_zd_data("0860",mbfebillopnreg_v.billcode);
	get_zd_data("0880",billofxchngseal);

	g_reply_int=Mbfebillopnreg_Sel(g_pub_tx.reply,&mbfebillopnreg_v,
		"billcode='%s'",mbfebillopnreg_v.billcode);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ��Ʊǩ���Ǽǲ����ݿ��������![%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if (strcmp(mbfebillopnreg_v.billofxchngseal,billofxchngseal))
	{
		sprintf( acErrMsg, "��Ʊ��Ѻ�������飡");
		MBFEWRITEMSG
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
