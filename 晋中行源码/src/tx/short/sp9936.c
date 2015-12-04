/*************************************************************
* �� �� ��:  sp9936.c
* ���������� ���֧����ݲ�ѯ
*			 ���������뽻��
*			 ����������ˮ�źͽ������ڲ�ѯ���ʽ�����Ϣ
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

int sp9936()
{
	struct mbfeghvrecvlist_c ghvrecvlist_v;
	struct mbfe_cmt_fmt_c mbfe_cmt_fmt_v;
	struct mbfenetbank_c mbfenetbank_v;
	struct mbfeeisbank_c mbfeeisbank_v;

	memset(&ghvrecvlist_v, 0x00, sizeof(struct mbfeghvrecvlist_c));
	memset(&mbfe_cmt_fmt_v, 0x00, sizeof(struct mbfe_cmt_fmt_c));
	memset(&mbfenetbank_v, 0x00, sizeof(struct mbfenetbank_c));
	memset(&mbfeeisbank_v, 0x00, sizeof(struct mbfeeisbank_c));

	pub_base_sysinit();

	get_zd_long("0450",&ghvrecvlist_v.consigndate);
	get_zd_long("0530",&ghvrecvlist_v.flowno);

	g_reply_int=Mbfeghvrecvlist_Sel( g_pub_tx.reply, &ghvrecvlist_v,
		"rectime[1,8]='%ld' and flowno='%ld'", ghvrecvlist_v.consigndate, ghvrecvlist_v.flowno);
	if(g_reply_int==100)
	{
		sprintf( acErrMsg, "û�иñ�����!" );
		WRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ������ϸ�����ݿ��������[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if (get_bkno()) goto ErrExit;
	if( strcmp( ghvrecvlist_v.cnapsrdfi, mbfetran.cnapsodfi ) )
	{
		sprintf( acErrMsg, "�Ǳ��н�������!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( ghvrecvlist_v.procstate[0] == '5' )
	{
		if( ghvrecvlist_v.procstate[1] == 'C' )
		{
			sprintf( acErrMsg, "��Ʊ�����˻��ʽ���ͨ��[4147]���ڶҸ���������!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}
	else if( ghvrecvlist_v.procstate[0]=='0' )
	{
		sprintf( acErrMsg, "�ñ����������㣬�����ֹ�����!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( ghvrecvlist_v.procstate[0]=='6' )
	{
		sprintf( acErrMsg, "�ñ��������˻أ������ֹ�����!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( ghvrecvlist_v.procstate[0]=='7' )
	{
		sprintf( acErrMsg, "�ñ��������ֹ����룬�����ٴ�����!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else
	{
		sprintf( acErrMsg, "�ñ�����״̬�������������ֹ�����!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mbfe_cmt_fmt_Sel( g_pub_tx.reply, &mbfe_cmt_fmt_v,
		"cmtno='%s'", ghvrecvlist_v.cmtno);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ���ĸ�ʽ��������ݿ��������![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(mbfe_cmt_fmt_v.cmt_name);

	set_zd_data("0230", ghvrecvlist_v.cmtno);
	set_zd_double("0390", ghvrecvlist_v.amount);
	set_zd_data("0580", ghvrecvlist_v.odficode);
	set_zd_data("0590", ghvrecvlist_v.rdficode);
	set_zd_data("0810", ghvrecvlist_v.payeracc);
	set_zd_data("0820", ghvrecvlist_v.recipientacc);
	set_zd_data("0860", ghvrecvlist_v.txssno);
	set_zd_data("0963", ghvrecvlist_v.payername);
	set_zd_data("0964", ghvrecvlist_v.recipientname);
	set_zd_data("0210", ghvrecvlist_v.procstate);
	pub_base_strpack( ghvrecvlist_v.odficode );
	pub_base_strpack( ghvrecvlist_v.rdficode );
	if( strlen( ghvrecvlist_v.odficode ) == 12 )
	{
		g_reply_int=Mbfenetbank_Sel( g_pub_tx.reply, &mbfenetbank_v,
			"nbkcode='%s'", ghvrecvlist_v.odficode);
		if(g_reply_int)
		{
			sprintf( acErrMsg, "��ѯ�������кű����ݿ��������![%d]", g_reply_int );
			MBFEWRITEMSG
		}
		set_zd_data("0961", mbfenetbank_v.nbkname);

		memset(&mbfenetbank_v, 0x00, sizeof(struct mbfenetbank_c));
		g_reply_int=Mbfenetbank_Sel( g_pub_tx.reply, &mbfenetbank_v,
			"nbkcode='%s'", ghvrecvlist_v.rdficode);
		if(g_reply_int)
		{
			sprintf( acErrMsg, "��ѯ�������кű����ݿ��������![%d]", g_reply_int );
			MBFEWRITEMSG
		}
		set_zd_data("0962", mbfenetbank_v.nbkname);
	}
	else if( strlen( ghvrecvlist_v.odficode ) == 6 )
	{
		g_reply_int=Mbfeeisbank_Sel( g_pub_tx.reply, &mbfeeisbank_v,
			"bnkno='%s'", ghvrecvlist_v.odficode);
		if(g_reply_int)
		{
			sprintf( acErrMsg, "��ѯ�������кű����ݿ��������![%d]", g_reply_int );
			MBFEWRITEMSG
		}
		set_zd_data("0961", mbfeeisbank_v.bnklname);

		memset(&mbfeeisbank_v, 0x00, sizeof(struct mbfeeisbank_c));
		g_reply_int=Mbfeeisbank_Sel( g_pub_tx.reply, &mbfeeisbank_v,
			"bnkno='%s'", ghvrecvlist_v.rdficode);
		if(g_reply_int)
		{
			sprintf( acErrMsg, "��ѯ�������кű����ݿ��������![%d]", g_reply_int );
			MBFEWRITEMSG
		}
		set_zd_data("0962", mbfeeisbank_v.bnklname);
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
