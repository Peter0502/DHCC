/*************************************************
* �� �� ��:  spB347.c
* ���������� �жһ�Ʊ��ѯ�����
*
* ��    ��:  xxx
* ������ڣ� 2003��9��25��
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
#include "mo_bank_po_query_c.h"

int spB347()
{
	struct	mo_bank_po_c		sMoBankPo;		/* �жһ�Ʊ�Ǽǲ� */
	struct	mo_bank_po_query_c	sMoBankPoQuery;	/* �жһ�Ʊ��ѯ��Ǽǲ� */
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	char	cPoNo1[17];							/* �жһ�Ʊ������Ļȡֵ */
	char	cPoNo[9];							/* �жһ�Ʊ������Чλ */
	char	cPoNoHead[9];						/* �жһ�Ʊ�������ͷ */
	char	cPoNo2[17];							/* �жһ�Ʊ�������ͷ+��Чλ */
	char	po_ind[2];							/* ����״̬��1-��ʧ2-���3-����4-�ⶳ */
	int	ret;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankPoQuery, 0x00, sizeof( struct mo_bank_po_query_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cPoNo1, 0x00, sizeof( cPoNo1 ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", cPoNo1 );		/* �жһ�Ʊ���� */
	get_zd_long( "0450", &sMoBankPoQuery.query_date);
	get_zd_data( "0590", sMoBankPoQuery.query_br_no);
	get_zd_data( "0820", sMoBankPoQuery.query_brf);

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
	ret = Mo_bank_po_Sel( g_pub_tx.reply,&sMoBankPo,"po_no='%s'", cPoNo2 );
	if ( ret )
	{
		sprintf( acErrMsg, "�޴˳жһ�Ʊ!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B217" );
		goto ErrExit;
	}

	if( sMoBankPo.po_sts[0] == '3' )
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

	/* �Ǽǳжһ�Ʊ��ѯ��ǼǱ� */
	sMoBankPoQuery.tx_date=g_pub_tx.tx_date;
	sMoBankPoQuery.trace_no=g_pub_tx.trace_no;
	strcpy(sMoBankPoQuery.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sMoBankPoQuery.po_no,cPoNo2);
	strcpy(sMoBankPoQuery.sts,"1");		/** ���� **/
	ret=Mo_bank_po_query_Ins(sMoBankPoQuery,g_pub_tx.reply);
	if ( ret )
	{
		sprintf( acErrMsg, "�Ǽǳжһ�Ʊ��ѯ�����![%d]",ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B323" );
		goto ErrExit;
	}

	/* ����ˮ��־ */
	strcpy(g_pub_tx.ac_no,sMoBankPo.pact_no);
	strcpy(g_pub_tx.beg_note_no,cPoNo2);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.brf,"�жһ�Ʊ��ѯ��Ǽ�");
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
