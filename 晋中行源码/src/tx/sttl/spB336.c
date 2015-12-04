/*************************************************
* �� �� ��:  spB336.c
* ���������� �����յ�ί���տ��������յǼǡ�
*
* ��    ��:  jane
* ������ڣ� 2003��5��30��
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

int spB336()
{
	struct	mo_bank_po_c		sMoBankPo;		/* �жһ�Ʊ�Ǽǲ� */
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	char	cPoNo1[17];							/* �жһ�Ʊ������Ļȡֵ */
	char	cPoNo[9];							/* �жһ�Ʊ������Чλ */
	char	cPoNoHead[9];						/* �жһ�Ʊ�������ͷ */
	char	cPoNo2[17];							/* �жһ�Ʊ�������ͷ+��Чλ */
	double	dPoAmt;								/* Ʊ���� */
	int	ret;
	char	cPact_no[21];

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cPoNo1, 0x00, sizeof( cPoNo1 ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );
	memset( cPact_no, 0x00, sizeof( cPact_no ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0370", cPact_no );		/* �жһ�ƱЭ���� */
/***	get_zd_double( "100E", &dPoAmt );		 Ʊ���� */

	/* �������жһ�Ʊ�����Ƿ������ͷ 
	ret = pub_com_ChkNoteHead1( "111", cPoNo1 );
	if( ret )
	{
		sprintf( acErrMsg, "���жһ�Ʊ�������![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	 ȡ�жһ�Ʊ����ͷ 
	strncpy( cPoNoHead, cPoNo1, 8 );
	cPoNoHead[8]='\0';
	pub_base_strpack( cPoNoHead );

	 ȡ�жһ�Ʊ��Чλ 
	strncpy( cPoNo, cPoNo1+8, 8 );
	cPoNo[8] = '\0';
sprintf( acErrMsg, "zxxxyu    cPoNo[%s]", cPoNo );
WRITEMSG

	 �жһ�Ʊ�������ͷ+��Чλ 
	sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo );
sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
WRITEMSG
	pub_base_strpack_all( cPoNo2 );
	******/
	/* ��ѯ��Ʊ���˺ţ�Ϊ����ǽ�����ˮ��ֵ�� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", cPact_no );
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
	if(sMoBankAccPo.acc_po_ind[0]=='0')
	{
		sprintf( acErrMsg, "δ�����ڴ���![%d]", ret );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"δ�����ڴ���!!!");
		goto ErrExit;
	}
	pub_base_strpack(cPact_no);
	/*** �޸ĳжһ�Ʊ�ǼǱ�,�Ǽ��յ����ڡ�״̬��Ϊ���� **/
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and po_sts not in ('2','3') ", sMoBankAccPo.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		memset(&sMoBankPo,0x00,sizeof(sMoBankPo));
		ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
		if( ret == 100 )
		{
			break;
		}else if( ret )
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
		}else if( sMoBankPo.po_sts[0] == '3' )
		{
			sprintf( acErrMsg, "�˳жһ�Ʊ������Ʊ!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B212" );
			goto ErrExit;
		}else if( sMoBankPo.po_sts[0] == '2' )
		{
			sprintf( acErrMsg, "�˳жһ�Ʊ�Ѻ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B234" );
			goto ErrExit;
		}
		
		/* �޸ĳжһ�Ʊ������Ϣ�Ǽǲ���״̬Ϊ����*/
		strcpy( sMoBankPo.po_sts, "4" );		/* ��־��4 ���� */
		sMoBankPo.receive_date = g_pub_tx.tx_date;	/* �������� */
	
		ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸ĳжһ�Ʊ�ǼǱ���[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		/* ����ˮ��־ */
		/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
		strcpy( g_pub_tx.note_type, "111" );				/* ���гжһ�Ʊ */
		strcpy(g_pub_tx.ac_no,sMoBankPo.pact_no);
		strcpy(g_pub_tx.beg_note_no,sMoBankPo.po_no);
		g_pub_tx.tx_amt1=0.00;
		strcpy( g_pub_tx.brf, "���гжһ�Ʊ���յǼ�" );
		ret = pub_ins_trace_log();
		if ( ret )
		{
			sprintf( acErrMsg, "����ˮ��־��" );
			WRITEMSG
			goto ErrExit;
		}
	}
	Mo_bank_po_Clo_Upd();

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
