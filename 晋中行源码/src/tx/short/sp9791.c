/*************************************************************
* �� �� ��:  sp9791.c
* ���������� ���֧����ݲ�ѯ
*			 ��Ʊǩ������ҵ���ŷ���¼����Ϣ
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
#include "com_branch_c.h"

int sp9791()
{
	char oprtflg[2];
	struct mbfebillopnreg_c mbfebillopnreg_v;
	struct com_branch_c com_branch_v;

	memset( &mbfebillopnreg_v, 0x00, sizeof( struct mbfebillopnreg_c ) );
	memset( &com_branch_v, 0x00, sizeof( struct com_branch_c ) );
	memset( &mbfetran, 0x00, sizeof( struct MBFETRAN ) );

	pub_base_sysinit();

	if( get_bkno() ) goto ErrExit;

	get_zd_data( "0730", mbfebillopnreg_v.pact_no );
	get_zd_data( "0700", oprtflg );

	g_reply_int = Mbfebillopnreg_Sel( g_pub_tx.reply, &mbfebillopnreg_v, 
		"pact_no='%s' and billsts!='*'", mbfebillopnreg_v.pact_no );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "û�еǼǸñ�ҵ����Ϣ!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ��Ʊ�Ǽǲ����ݿ��������[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if( oprtflg[0] == '1' )	/*ǩ��*/
	{
		if( mbfebillopnreg_v.billsts[0] == '0' )
		;
		else if(mbfebillopnreg_v.billsts[0]=='1')
		{
			sprintf( acErrMsg, "��ǩ����Ʊ����Ʊ���룺��%s��", mbfebillopnreg_v.billcode );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if(mbfebillopnreg_v.billsts[0]=='2')
		{
			sprintf( acErrMsg, "��ǩ�����ƴ��Ʊ����Ʊ���룺��%s��", mbfebillopnreg_v.billcode );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else
		{
			sprintf( acErrMsg, "�ñ�¼����Ϣ״̬������,����ϵ���Ļ���![%s]",mbfebillopnreg_v.billsts );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}
	else if( oprtflg[0] == '2' || oprtflg[0] == '3' || oprtflg[0] == '4' )    /*�ش�/��ǩ/��ǩ*/
	{
		if( mbfebillopnreg_v.billsts[0] == '0' )
		{
			sprintf( acErrMsg, "�ñ�ҵ����Ϣ��δǩ����Ʊ" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfebillopnreg_v.billsts[0] == '2' )
		{
			sprintf( acErrMsg, "��Ʊ�Ѿ��ƴ棬��Ʊ���룺��%s��", mbfebillopnreg_v.billcode );
			MBFEWRITEMSG
			goto ErrExit;
		}

		if( strcmp( mbfebillopnreg_v.payeropenaccbkcode, mbfetran.cnapsodfi ) )
		{
        	sprintf( acErrMsg, "�Ǳ���ǩ����Ʊ!");
        	MBFEWRITEMSG
			goto ErrExit;
		}

		if( oprtflg[0] == '3' )
		{
			memset( mbfebillopnreg_v.billcode, 0x00, sizeof( mbfebillopnreg_v.billcode ) );
			memset( mbfebillopnreg_v.billofxchngseal, 0x00, sizeof( mbfebillopnreg_v.billofxchngseal ) );
		}
	}

	g_reply_int = Com_branch_Sel( g_pub_tx.reply, &com_branch_v, 
		"br_no='%s'", mbfebillopnreg_v.tx_br_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ¼��������Ƴ���![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0860", mbfebillopnreg_v.billcode );
	set_zd_data( "0880", mbfebillopnreg_v.billofxchngseal );
	set_zd_data( "0910", mbfebillopnreg_v.tx_br_no );
	set_zd_data( "0620", com_branch_v.br_name );
	set_zd_data( "0630", mbfebillopnreg_v.brf );
	set_zd_long( "0460", mbfebillopnreg_v.tx_date );
	set_zd_double( "0390", mbfebillopnreg_v.amount );
	set_zd_data( "0210", mbfebillopnreg_v.billtype );
	set_zd_data( "0300", mbfebillopnreg_v.payeracc );
	set_zd_data( "0250", mbfebillopnreg_v.payername );
	set_zd_data( "0260", mbfebillopnreg_v.payeraddr );
	set_zd_data( "0810", mbfebillopnreg_v.recipientacc );
	set_zd_data( "0961", mbfebillopnreg_v.recipientname );
	set_zd_data( "0590", mbfebillopnreg_v.cashbillbkno );

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
