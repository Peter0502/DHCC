/*************************************************
* �� �� ��:  spB491.c
* ���������� �жһ�ƱЭ���ź�ͬ�Ŷ��ձ�ά��
*
* ��    ��:  jane
* ������ڣ� 2003��11��1��
*
* �޸ļ�¼��
*   ��    ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define EXTERN
#include "public.h"
#include "mo_po_co_c.h"

int spB491()
{
	struct mo_po_co_c sMoPoCo;
	char flag[2];

	memset( &sMoPoCo,0x00,sizeof(struct mo_po_co_c) );

	pub_base_sysinit();

	get_zd_data("0330", sMoPoCo.pact_no );

	pub_base_strpack( sMoPoCo.pact_no );

	g_reply_int = Mo_po_co_Sel( g_pub_tx.reply, &sMoPoCo, "pact_no='%s'", sMoPoCo.pact_no );
	if( g_reply_int == 100 )
	{
		strcpy( g_pub_tx.reply, "B187" );
		sprintf( acErrMsg, "���ձ����޴˳ж�Э����!" );
		WRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ����[%ld]!", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( sMoPoCo.contract_no, 0x00, sizeof(sMoPoCo.contract_no) );

	get_zd_data("0320", sMoPoCo.contract_no );

	g_reply_int = sql_execute( "update mo_po_co set contract_no='%s' where pact_no='%s'", 
		sMoPoCo.contract_no, sMoPoCo.pact_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "��mo_po_co���ݿ��������[%ld]!", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
