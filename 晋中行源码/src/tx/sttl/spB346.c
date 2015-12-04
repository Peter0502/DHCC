/*************************************************
* �� �� ��:  spB346.c
* ���������� �жһ�Ʊ��ʧ������
*
* ��    ��:  xxx
* ������ڣ� 2003��9��24��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"
#include "mo_bank_acc_po_c.h"

int spB346()
{
	struct	mo_bank_po_c		sMoBankPo;		/* �жһ�Ʊ�Ǽǲ� */
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	char	cPoNo1[17];							/* �жһ�Ʊ������Ļȡֵ */
	char	cPoNo[9];							/* �жһ�Ʊ������Чλ */
	char	cPoNoHead[9];						/* �жһ�Ʊ�������ͷ */
	char	cPoNo2[17];							/* �жһ�Ʊ�������ͷ+��Чλ */
	char	po_ind[2];							/* ����״̬��1-��ʧ2-���3-����4-�ⶳ */
	int	ret;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cPoNo1, 0x00, sizeof( cPoNo1 ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", cPoNo1 );		/* �жһ�Ʊ���� */
	get_zd_data( "0670", po_ind );

	/* �������жһ�Ʊ�����Ƿ������ͷ */
	ret = pub_com_ChkNoteHead1( "111", cPoNo1 );
	if( ret )
	{
		sprintf( acErrMsg, "���жһ�Ʊ�������![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ȡ�жһ�Ʊ����ͷ */
	strncpy( cPoNoHead, cPoNo1, 8 );
	cPoNoHead[8]='\0';
	pub_base_strpack( cPoNoHead );

	/* ȡ�жһ�Ʊ��Чλ */
	strncpy( cPoNo, cPoNo1+8, 8 );
	cPoNo[8] = '\0';
sprintf( acErrMsg, "zxxxyu    cPoNo[%s]", cPoNo );
WRITEMSG

	/* �жһ�Ʊ�������ͷ+��Чλ */
	sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo );
sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
WRITEMSG
	pub_base_strpack_all( cPoNo2 );

	/* �޸ĳжһ�Ʊ�ǼǱ�,�Ǽ��յ����ڡ�״̬��Ϊ���� */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "po_no='%s'", cPoNo2 );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˳жһ�Ʊ!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B217" );
		goto ErrExit;
    }
	else if( ret )
    {
    	sprintf( acErrMsg, "ִ��Mo_bank_po_Fet_Upd��![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

	if( sMoBankPo.po_sts[0] == '4' )
	{
		sprintf( acErrMsg, "�˳жһ�Ʊ��������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B220" );
		goto ErrExit;
	}
	else if( sMoBankPo.po_sts[0] == '3' )
	{
		sprintf( acErrMsg, "�˳жһ�Ʊ������Ʊ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B212" );
		goto ErrExit;
	}
	else if( sMoBankPo.po_sts[0] == '2' )
	{
		sprintf( acErrMsg, "�˳жһ�Ʊ�Ѻ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B234" );
		goto ErrExit;
	}
	else if( sMoBankPo.po_sts[0]!='1' && po_ind[0]=='1' )
	{
		sprintf( acErrMsg, "�˳жһ�Ʊ��ǩ��״̬!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B320" );
		goto ErrExit;
	}

	if( po_ind[0]=='1' && sMoBankPo.po_ind[0]=='0' )	/** ��ʧ **/
	{
		sprintf( acErrMsg, "�˳жһ�Ʊ�Ѿ���ʧ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B321" );
		goto ErrExit;
	}
	if( po_ind[0]=='2' && sMoBankPo.po_ind[0]!='0' )	/** ��� **/
	{
		sprintf( acErrMsg, "�˳жһ�Ʊδ��ʧ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B322" );
		goto ErrExit;
	}


	/* ��ѯ��Ʊ���˺ţ�Ϊ����ǽ�����ˮ��ֵ�� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoBankPo.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˳ж�Э����![%s]", sMoBankPo.pact_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B187" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( po_ind[0]=='1' )	/** ��ʧ **/
	{
		strcpy( sMoBankPo.po_ind, "0" );		/* ��־��4 ��ʧ */
		strcpy( g_pub_tx.brf, "���гжһ�Ʊ��ʧ" );
	}
	if( po_ind[0]=='2' )    /** ��� **/		
	{
		strcpy( sMoBankPo.po_ind, "1" );		/* ��־��1 ���� */
		strcpy( g_pub_tx.brf, "���гжһ�Ʊ���" );
	}

	ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸ĳжһ�Ʊ�ǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_bank_po_Clo_Upd();

	/* ����ˮ��־ */
	strcpy(g_pub_tx.ac_no,sMoBankPo.pact_no);
	strcpy(g_pub_tx.beg_note_no,cPoNo2);
	g_pub_tx.tx_amt1=0.00;
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
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
