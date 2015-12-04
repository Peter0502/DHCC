/*************************************************************
* �� �� ��:  sp9975.c
* ���������� ���֧����ݲ�ѯ
*			 ����˻�����ҵ���ط����׿��
*			 ��������֧����ź�ί�����ڲ�ѯ���˻ص����ʽ�����Ϣ
*			 ������Ƿ���˻�ҵ���˻ص����
*
* ��    ��:  jane
* ������ڣ� 2003��8��8��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9975()
{
	struct mbfeghvsendlist_c ghvsendlist_v;
	struct mbfeghvrecvlist_c ghvrecvlist_v;

	memset( &ghvsendlist_v, 0x00, sizeof( struct mbfeghvsendlist_c ) );
	memset( &ghvrecvlist_v, 0x00, sizeof( struct mbfeghvrecvlist_c ) );

	get_zd_long( "0440", &ghvsendlist_v.consigndate );    /*ԭ����ί������*/
	get_zd_data( "0860", ghvsendlist_v.txssno );    /*ԭ����֧�����*/
	get_zd_long( "0450", &ghvrecvlist_v.consigndate );    /*�˻�ҵ���յ�����*/
	get_zd_long( "0530", &ghvrecvlist_v.flowno );    /*�˻�ҵ����ˮ��*/

	pub_base_sysinit();

	g_reply_int = Mbfeghvrecvlist_Sel( g_pub_tx.reply, &ghvrecvlist_v, 
		"rectime[1,8]='%ld' and flowno='%ld' and cmtno='108'", 
		ghvrecvlist_v.consigndate, ghvrecvlist_v.flowno );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "�˻�ҵ�񲻴���!" );
		WRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ������ϸ�����ݿ��������[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	/*�˻�ҵ����*/
	if( ghvrecvlist_v.procstate[0] == '5' )
	;
	else if( ghvrecvlist_v.procstate[0]=='6' )
	{
		sprintf( acErrMsg, "�ñ��˻����˻أ������ط�!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( ghvrecvlist_v.procstate[0]=='7' )
	{
		sprintf( acErrMsg, "�ñ��˻����ֹ�������ط��������ٴ��ط�!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else
	{
		sprintf( acErrMsg, "�ñ��˻�����״̬�������������ֹ�����!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( ghvrecvlist_v.warrantdate != ghvsendlist_v.consigndate ||
		strcmp( ghvrecvlist_v.warrantno, ghvsendlist_v.txssno ) )
	{
		sprintf( acErrMsg, "�ñ��˻�ҵ����ԭ������Ϣ����!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int = Mbfeghvsendlist_Sel( g_pub_tx.reply, &ghvsendlist_v, 
		"consigndate='%ld' and txssno='%s' and procstate='01'", 
		ghvsendlist_v.consigndate, ghvsendlist_v.txssno );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "�˻ص�����ҵ�񲻴���!" );
		WRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "��ѯ������ϸ�����ݿ��������[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if( ghvrecvlist_v.amount != ghvsendlist_v.amount )
	{
		sprintf( acErrMsg, "�ñ��˻�ҵ��Ľ����ԭ���ʽ���,�����˻��в�ѯ!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( get_bkno() ) goto ErrExit;
	if( strcmp( ghvsendlist_v.cnapsodfi, mbfetran.cnapsodfi ) )
	{
		sprintf( acErrMsg, "�Ǳ��з��������ҵ��!" );
		MBFEWRITEMSG
		goto ErrExit;
	}


	set_zd_data( "0810", ghvsendlist_v.recipientacc );
	set_zd_data( "0961", ghvsendlist_v.recipientname );
	set_zd_data( "0300", ghvsendlist_v.payeracc );
	set_zd_data( "0250", ghvsendlist_v.payername );
	set_zd_double( "0390", ghvsendlist_v.amount );
	set_zd_data( "0580", ghvsendlist_v.rdficode );
	set_zd_data( "0964", ghvsendlist_v.strinfo );
	set_zd_data( "0240", ghvsendlist_v.cmtno );

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
