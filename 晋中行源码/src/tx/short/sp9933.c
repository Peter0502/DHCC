/*************************************************************
* �� �� ��:  sp9933.c
* ���������� ���֧����ݲ�ѯ
*			 ��Ʊ��Ϣ¼���Ʊ���
*
* ��    ��:  jane
* ������ڣ� 2003��6��21��
*
* �޸ļ�¼�� 
*   ��   ��: 2003��7��28��
*   �� �� ��:jane
*   �޸�����:�����ж�ֻ����Ϣ¼���в����޸ĺ�ɾ���ñ�ҵ����Ϣ
*            �꾡��ʾ��Ϣ
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "com_parm_c.h"

int sp9933()
{
	struct mbfebillopnreg_c mbfebillopnreg_v;

	memset( &mbfebillopnreg_v, 0x00, sizeof( struct mbfebillopnreg_c ) );

	pub_base_sysinit();

	get_zd_data( "0860", mbfebillopnreg_v.pact_no );

	g_reply_int = Mbfebillopnreg_Sel( g_pub_tx.reply, &mbfebillopnreg_v,
		"pact_no='%s' and tx_br_no='%s' and billsts[1]!='*'", 
		mbfebillopnreg_v.pact_no, g_pub_tx.tx_br_no );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "û�еǼǸñ�ҵ����Ϣ��ñ�ҵ����ǩ����Ʊ������!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ��Ʊ�Ǽǲ����ݿ��������[%d]", g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( strcmp( mbfebillopnreg_v.tx_br_no, g_pub_tx.tx_br_no ) )
	{
		sprintf( acErrMsg, "�Ǳ���¼���ҵ����Ϣ!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( mbfebillopnreg_v.billsts[0] == '0' )
	;
	else if( mbfebillopnreg_v.billsts[0] == '1' )
	{
		sprintf( acErrMsg, "�Ѿ�ǩ����Ʊ!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( mbfebillopnreg_v.billsts[0] == '2' )
	{
		sprintf( acErrMsg, "�Ѿ�ǩ�����ƴ��Ʊ!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else
	{
		sprintf( acErrMsg, "��Ʊ״̬�쳣!����ϵ���Ļ���" );
		MBFEWRITEMSG
		vtcp_log( "��Ʊ״̬�쳣!ҵ����[%s]", mbfebillopnreg_v.pact_no );
		goto ErrExit;
	}

	set_zd_double( "0390", mbfebillopnreg_v.amount );
	set_zd_data( "0210", mbfebillopnreg_v.billtype );
	set_zd_data( "0300", mbfebillopnreg_v.payeracc );
	set_zd_data( "0250", mbfebillopnreg_v.payername );
	set_zd_data( "0962", mbfebillopnreg_v.payeraddr );
	set_zd_data( "0810", mbfebillopnreg_v.recipientacc );
	set_zd_data( "0961", mbfebillopnreg_v.recipientname );
	set_zd_data( "0590", mbfebillopnreg_v.cashbillbkno );
	set_zd_data( "0370", mbfebillopnreg_v.brf );

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
