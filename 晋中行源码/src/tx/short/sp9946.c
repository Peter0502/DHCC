/*************************************************************
* �� �� ��:  sp9946.c
* ���������� ���֧����ݲ�ѯ
*			 �����������ӽ���
*			 ����������ˮ�źͽ������ڼ�����ʽ�����Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��6��27��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9946()
{
	struct mbfeghvrecvlist_c mbfeghvrecvlist_v;

	memset(&mbfeghvrecvlist_v, 0x00, sizeof(struct mbfeghvrecvlist_c));

	pub_base_sysinit();

	get_zd_long("1234",&mbfeghvrecvlist_v.consigndate);
	get_zd_long("1235",&mbfeghvrecvlist_v.flowno);

	g_reply_int=Mbfeghvrecvlist_Sel( g_pub_tx.reply, &mbfeghvrecvlist_v,
		"consigndate='%ld' and flowno='%ld'", mbfeghvrecvlist_v.consigndate, mbfeghvrecvlist_v.flowno);
	if(g_reply_int==100)
	{
		vtcp_log( "[%ld][%ld]", mbfeghvrecvlist_v.consigndate,mbfeghvrecvlist_v.flowno);
		sprintf( acErrMsg, "û�иñ�����!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ������ϸ������ݿ��������[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if( mbfeghvrecvlist_v.procstate[0] == '5' )
	{
		if( mbfeghvrecvlist_v.procstate[1] == 'C' )
		{
			sprintf( acErrMsg, "��Ʊ�����˻��ʽ���ͨ��[4147]���ڶҸ���������!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfeghvrecvlist_v.procstate[1] == 'D' )
		{
			sprintf( acErrMsg, "��ʧ��Ʊ�˻��ʽ���ͨ��[4147]���ڶҸ���������!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}
	else if( mbfeghvrecvlist_v.procstate[0]=='0' )
	{
		sprintf( acErrMsg, "�ñ����������㣬��������!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( mbfeghvrecvlist_v.procstate[0]=='6' )
	{
		sprintf( acErrMsg, "�ñ��������˻أ���������!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( mbfeghvrecvlist_v.procstate[0]=='7' )
	{
		sprintf( acErrMsg, "�ñ��������ֹ����룬�����ٴ�����!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else
	{
		sprintf( acErrMsg, "�ñ�����״̬����������������!" );
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
