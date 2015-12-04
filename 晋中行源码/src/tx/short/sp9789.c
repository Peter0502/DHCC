/*************************************************************
* �� �� ��:  sp9789.c
* ���������� ���֧����ݲ�ѯ
*			 �˻�֧������������ˮ�źͽ������ڲ�ѯ���ʽ�����Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��6��8��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9789()
{
	char date[9];
	struct mbfeghvrecvlist_c mbfeghvrecvlist_v;
	struct mbfe_cmt_fmt_c mbfe_cmt_fmt_v;
	struct mbfenetbank_c mbfenetbank_v;

	memset(&mbfeghvrecvlist_v, 0x00, sizeof(struct mbfeghvrecvlist_c));
	memset(&mbfe_cmt_fmt_v, 0x00, sizeof(struct mbfe_cmt_fmt_c));
	memset(&mbfenetbank_v, 0x00, sizeof(struct mbfenetbank_c));

	pub_base_sysinit();

	get_zd_long("0450",&mbfeghvrecvlist_v.consigndate);
	get_zd_long("0530",&mbfeghvrecvlist_v.flowno);
	sprintf(date,"%8ld",mbfeghvrecvlist_v.consigndate);

	g_reply_int=Mbfeghvrecvlist_Sel( g_pub_tx.reply, &mbfeghvrecvlist_v,
		"rectime[1,8]='%s' and flowno='%ld'", date, mbfeghvrecvlist_v.flowno);
	if(g_reply_int==100)
	{
		sprintf( acErrMsg, "û�иñ�����!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ������ϸ������ݿ��������[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( mbfeghvrecvlist_v.procstate[0]!='5' )
	{
		if( mbfeghvrecvlist_v.procstate[0]=='0' )
		{
			sprintf( acErrMsg, "�ñ����������㣬�����˻�!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfeghvrecvlist_v.procstate[0]=='6' )
		{
			sprintf( acErrMsg, "�ñ��������˻أ������˻�!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfeghvrecvlist_v.procstate[0]=='7' )
		{
			sprintf( acErrMsg, "�ñ��������ֹ����룬�����˻�!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else
		{
			sprintf( acErrMsg, "�ñ�����״̬�������������˻�!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}

	if( get_bkno() ) goto ErrExit;
	if( strcmp(mbfeghvrecvlist_v.cnapsrdfi, mbfetran.cnapsodfi) )
	{
		sprintf( acErrMsg, "�Ǳ��н��յ�����ҵ�񣬲����˻�!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mbfe_cmt_fmt_Sel( g_pub_tx.reply, &mbfe_cmt_fmt_v,
		"cmtno='%s'", mbfeghvrecvlist_v.cmtno);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ���ĸ�ʽ��������ݿ��������![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(mbfe_cmt_fmt_v.cmt_name);

	if( strcmp(mbfeghvrecvlist_v.cmtno,"100") &&
		strcmp(mbfeghvrecvlist_v.cmtno,"101") &&
		strcmp(mbfeghvrecvlist_v.cmtno,"102") )
	{
		sprintf( acErrMsg, "%s����ҵ�����˻�!", mbfe_cmt_fmt_v.cmt_name );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mbfenetbank_Sel( g_pub_tx.reply, &mbfenetbank_v,
		"nbkcode='%s'", mbfeghvrecvlist_v.odficode);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ�������кű����ݿ��������![%d]", g_reply_int );
		MBFEWRITEMSG
	}

	set_zd_data("0860", mbfeghvrecvlist_v.txssno);
	set_zd_data("0230", mbfeghvrecvlist_v.procstate);
	set_zd_data("0240", mbfeghvrecvlist_v.cmtno);
	set_zd_data("0870", mbfe_cmt_fmt_v.cmt_name);
	set_zd_data("0580", mbfeghvrecvlist_v.odficode);
	set_zd_data("0963", mbfenetbank_v.nbkname);
	set_zd_double("0390", mbfeghvrecvlist_v.amount);
	set_zd_data("0300", mbfeghvrecvlist_v.recipientacc);
	mbfeghvrecvlist_v.recipientname[50]='\0';
	set_zd_data("0250", mbfeghvrecvlist_v.recipientname);
	set_zd_data("0810", mbfeghvrecvlist_v.payeracc);
	set_zd_data("0961", mbfeghvrecvlist_v.payername);

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
