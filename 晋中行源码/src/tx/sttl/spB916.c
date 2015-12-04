/*************************************************
* �� �� ��:  spB916.c
* ���������� �������������˹��ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��8��27��
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
#include "mo_discnt_c.h"
#include "mo_bank_po_c.h"

int spB916()
{
	struct	mo_discnt_c		sMoDiscnt;	/* ���ֵǼǱ� */
	struct	mo_bank_po_c	sMoBankPo;	/* ���гжһ�Ʊ�Ǽǲ� */
	char	cPoNo[9];					/* �жһ�Ʊ������Чλ */
	char	cPoNoHead[9];				/* �жһ�Ʊ�������ͷ */
	char	cPoNo2[17];					/* �жһ�Ʊ�������ͷ+��Чλ */
	int		ret, num;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0620", sMoDiscnt.pact_no );	/* ����Э���� */

	/* ��������Э���ź����������޸����ֵǼǲ� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoDiscnt.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�����Э����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B113" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	if( sMoDiscnt.pn_type[0] != '1' )
	{
		sprintf( acErrMsg, "��Э����Ϊת���ֻ�������Э����!����Ҫ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B151" );
		goto ErrExit;
	}

	if( sMoDiscnt.sts[0] == '2' )
	{
		sprintf( acErrMsg, "������Э�������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B152" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '3' )
	{
		sprintf( acErrMsg, "�˱������Ѽ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B148" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "�˱������ѻ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	pub_base_strpack( sMoDiscnt.pnote_no );
	/* �������жһ�Ʊ�����Ƿ������ͷ */
	ret = pub_com_ChkNoteHead1( "111", sMoDiscnt.pnote_no );
	if( ret )
	{
		sprintf( acErrMsg, "���жһ�Ʊ�������![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

vtcp_log( "zxxxyu222      pnote_no[%s]", sMoDiscnt.pnote_no );
	/* ȡ�жһ�Ʊ����ͷ */
	strncpy( cPoNoHead, sMoDiscnt.pnote_no, 8 );
	cPoNoHead[8]='\0';
	pub_base_strpack( cPoNoHead );

	/* ȡ�жһ�Ʊ��Чλ */
	strncpy( cPoNo, sMoDiscnt.pnote_no+8, 8 );
	cPoNo[8] = '\0';
sprintf( acErrMsg, "zxxxyu    cPoNo[%s]", cPoNo );
WRITEMSG

	/* �жһ�Ʊ�������ͷ+��Чλ */
	sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo );
sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
WRITEMSG
	pub_base_strpack_all( cPoNo2 );

	/* ���Ʊ���Ƿ�Ϊ���п����ĳжһ�Ʊ */
	ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "po_no='%s'", cPoNo2 );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�����гжһ�Ʊ�Ǳ��п���![%s]", cPoNo2 );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B305" );
		goto ErrExit;
	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_bank_po_Sel��![%d]", ret );
        WRITEMSG
        goto ErrExit;
   	}

	/* �޸����ֵǼǱ���״̬Ϊ�����*/
	strcpy( sMoDiscnt.sts, "2" );			/* ��־��2����� */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_discnt_Clo_Upd();

	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "�����������" );
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!!");
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
