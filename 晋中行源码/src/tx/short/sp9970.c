/*************************************************************
* �� �� ��:  sp9970.c
* ���������� ���֧����ݲ�ѯ
*			 ��Ʊ���л�Ʊ��ʧ
*
* ��    ��:  jane
* ������ڣ� 2003��8��1��
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

int sp9970()
{
	char oprt_flag[2];
	struct mbfebillopnreg_c mbfebillopnreg_v;

	memset( &mbfebillopnreg_v, 0x00, sizeof( struct mbfebillopnreg_c ) );

	pub_base_sysinit();

	get_zd_data( "0700", oprt_flag );
	get_zd_data( "0860", mbfebillopnreg_v.billcode );
	sprintf( mbfebillopnreg_v.billcode, "%08ld", atol( mbfebillopnreg_v.billcode ) );

	g_reply_int = Mbfebillopnreg_Sel( g_pub_tx.reply, &mbfebillopnreg_v,
		"billcode='%s'", mbfebillopnreg_v.billcode );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "û��ǩ�����Ż�Ʊ!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ��Ʊ�Ǽǲ����ݿ��������[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if( oprt_flag[0] == '0' )    /*����ǩ����Ʊ��ʧ*/
	{
		/*��Ʊ״̬Ϊ�ƴ棬��ʧ״̬Ϊ��������*/
		if( mbfebillopnreg_v.billsts[0] =='2' && 
			( mbfebillopnreg_v.los_rpt_ind[0] == '0' || mbfebillopnreg_v.los_rpt_ind[0] == '2' ) )
		;
		else if( mbfebillopnreg_v.billsts[0] != '2' )
		{
			vtcp_log( "���Ż�Ʊ״̬�������ʧ!��Ʊ����[%s]", mbfebillopnreg_v.billcode);
			sprintf( acErrMsg, "���Ż�Ʊ״̬�����Ѿ��ƴ�,�������ʧ!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfebillopnreg_v.billsts[0] == '2' && mbfebillopnreg_v.los_rpt_ind[0] == '1' )
		{
			sprintf( acErrMsg, "���Ż�Ʊ�Ѿ���ʧ!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}
	else if( oprt_flag[0] == '1' )    /*����ǩ����Ʊ���*/
	{
		/*��Ʊ״̬Ϊ�ƴ棬��ʧ״̬Ϊ��ʧ*/
		if( mbfebillopnreg_v.billsts[0] =='2' && mbfebillopnreg_v.los_rpt_ind[0] == '1' )
		;
		else
		{
			vtcp_log( "���Ż�Ʊ״̬��������!��Ʊ����[%s]", mbfebillopnreg_v.billcode);
			sprintf( acErrMsg, "���Ż�Ʊ״̬�����Ѿ��ƴ沢��ʧ,���ܽ��!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		set_zd_data( "0300", mbfebillopnreg_v.los_rpt_bookno );    /*��ʧ�������*/
	}

	set_zd_double( "0390", mbfebillopnreg_v.amount );    /*��Ʊ���*/
	set_zd_long( "0450", mbfebillopnreg_v.signdate );    /*��Ʊ����*/
	set_zd_data( "0600", mbfebillopnreg_v.payeropenaccbkcode );    /*ǩ���к�*/
	set_zd_data( "0590", mbfebillopnreg_v.cashbillbkno );    /*�ֽ�Ҹ��к�*/

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
