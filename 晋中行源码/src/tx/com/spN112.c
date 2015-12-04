/*************************************************
* �� �� ��:  spN112.c
* ���������� �ͻ�ƾ֤�Ĺ�ʧ���ʹ�ô˽��ף�
*	     ��Ϊ����ʧ����ң�
*
* ��    ��:  andy
* ������ڣ� 2004��1��10��
*
* �޸ļ�¼�� 
*   ��    ��:2004��3��29��
*   �� �� ��:andy
*   �޸�����:��Ϊ�ɹ�ʧ��Ҷ���ƾ֤�����
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"

int spN112()
{
	struct	note_mst_c	sNoteMst;	/* ��Աƾ̨֤�� */
	struct	note_cheq_mst_c	sCheqMst;	/* �ͻ�ƾ̨֤�� */
	char	cOpenBrNo[6];			/* ���������� */
	char	who[6];				/* ʹ���� */
	char    cNoteSts[2];            	/* ƾ֤״̬ */
	char    cPNoteSts[2];            	/* ƾ֤״̬ */
	char	cNoteAct[2];			/* ƾ֤��Ϊ */
	char	tw_br_no[6];			/* �Է������� */
	char	tw_tel[6];			/* �Է�����Ա */
	char	cOptCode[2];			/* ƾ֤��Ϊ */
	int	ret;

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	memset( cOpenBrNo, 0x00, sizeof( cOpenBrNo ) );
	memset( who, 0x00, sizeof( who ) );
	memset( cNoteSts, 0x00, sizeof( cNoteSts ) );
	memset( cPNoteSts, 0x00, sizeof( cPNoteSts ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( tw_br_no, 0x00, sizeof( tw_br_no ) );
	memset( tw_tel, 0x00, sizeof( tw_tel ) );
	memset( cOptCode, 0x00, sizeof( cOptCode ) );

	strcpy( tw_br_no, "" );			/* ƾ֤�����ջز������漰�Է����� */
	strcpy( tw_tel, "" );			/* ƾ֤�����ջز������漰�Է�����Ա */

	get_zd_data( "0670", cOptCode );	/* ������ */

	/* ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ�� */
	init_pub_tx( );	
	/**����Ȩ�޼��**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,401,0 ) )
			goto ErrExit;

	/* ���ƾ֤�Ƿ���ڣ��Ƿ����� */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "���ƾ֤�Ƿ���ڣ��Ƿ����ô�" );
		WRITEMSG
		goto ErrExit;
	}

	/* ƾ֤��ʼ���롢��ֹ�����飨������ͷ��*/
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "���ƾ֤��ʼ�������ͷ��" );
		WRITEMSG
		goto ErrExit;	
	}

	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel );
	if( ret )	
	{
		sprintf( acErrMsg, "���ƾ֤��ֹ�������ͷ��" );
		WRITEMSG
		goto ErrExit;	
	}

	/* �����ʺŲ�ѯ�˻���ź��˻�ID */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}else if( ret )
	{
		sprintf(acErrMsg,"ִ��Mdm_ac_rel_Sel����!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
sprintf( acErrMsg, "ac_seqn=[%d]",g_mdm_ac_rel.ac_seqn );
WRITEMSG

	/* �ʻ���Ч�Լ�� */
	ret = pub_base_ChkAc( g_pub_tx.ac_no, g_mdm_ac_rel.ac_seqn,cOpenBrNo, &g_pub_tx.ac_id );
	if( ret )
	{
		sprintf( acErrMsg, "�ʻ���Ч�Լ���" );
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;

	/* ����ƾ֤��Ϊ���ض�Ӧƾ֤״̬ */
	strcpy( who, "C" );			/* "C"��ʾ�ͻ���Ϊ */

	if( cOptCode[0]=='0' )			/* ��ʧ */
	{
		strcpy( cNoteAct, "B" );	/* "B"��ʾΪƾ֤��ʧ���� */
	}
	else					/* ��� */
	{
		strcpy( cNoteAct, "C" );	/* "C"��ʾΪƾ֤��Ҳ��� */
	}

	ret=pub_com_NoteActSts( who, cNoteAct, cNoteSts, cPNoteSts );
	if ( ret )
	{
		sprintf( acErrMsg, "����ƾ֤��Ϊ���ض�Ӧƾ֤״̬��" );
		WRITEMSG
		goto ErrExit;
	}

	/* �ͻ�ƾ֤��� */
	ret=pub_com_ChkAcCheq( g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
			g_pub_tx.note_type, g_pub_tx.beg_note_no, \
			g_pub_tx.end_note_no, cPNoteSts);
	if( ret )
	{
		sprintf( acErrMsg, "�ͻ�ƾ֤����" );
		WRITEMSG
		goto ErrExit;
	}

	/* ���ƾ֤�Ƿ��ǿͻ���ʹ�õ� */
	if( strcmp( cPNoteSts, "5" ) == 0 )
	{
		sprintf( acErrMsg, "ƾ֤�ͻ��Ѿ�ʹ�ò������ʧ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N073" );
		goto ErrExit;
	}
	if( cOptCode[0]=='0' )			/* ��ʧ */
	{
		if( cPNoteSts[0]!='1' )				/* ֧Ʊ״̬ӦΪ����δʹ�� */
		{
			sprintf( acErrMsg, "֧Ʊ״̬��" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "N022" );
			goto ErrExit;
		}
	}
	else
	{
		if( cPNoteSts[0]!='2' )				/* ֧Ʊ״̬ӦΪ����δʹ�� */
		{
			sprintf( acErrMsg, "֧Ʊ״̬��" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "N022" );
			goto ErrExit;
		}
	}

	/* �޸Ŀͻ�ƾ̨֤�ʣ��޸�״̬�� */
	strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
	strcpy( sNoteMst.tel, g_pub_tx.tel );
	sNoteMst.cnt = (int)g_pub_tx.tx_amt1;
	strcpy( sNoteMst.note_type, g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
	sNoteMst.cnt = 1;

	ret=pub_com_RegCheq( sNoteMst, g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
				g_pub_tx.tx_date, cNoteAct );
	if( ret )
	{
		sprintf( acErrMsg, "�޸Ŀͻ�ƾ̨֤�ʴ�" );
		WRITEMSG
		goto ErrExit;
	}

	/* �Ǽǿͻ�ƾ̨֤����ϸ */
	sCheqMst.ac_id = g_pub_tx.ac_id;
	sCheqMst.ac_seq = g_pub_tx.ac_seqn;
	sCheqMst.cnt = (int)g_pub_tx.tx_amt1;
	strcpy( sCheqMst.br_no, cOpenBrNo );
	strcpy( sCheqMst.note_type, g_pub_tx.note_type );
	strcpy( sCheqMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sCheqMst.end_note_no, g_pub_tx.end_note_no );

	ret=pub_com_RegCheqHst( cNoteAct, sCheqMst, tw_br_no, tw_tel );
	if ( ret )
	{
		sprintf( acErrMsg, "�Ǽǿͻ�ƾ̨֤����ϸ��!" );
		WRITEMSG
		goto ErrExit;
	}

	/* �Ǽǹ�Աƾ̨֤�ʣ�ƾ֤�ֶδ��� *
	ret=pub_com_NoteApart( cNoteAct, sNoteMst );
	if ( ret )
	{
		sprintf( acErrMsg, "ƾ֤�ֶδ����!" );
		WRITEMSG
		goto ErrExit;
	}
	***/

	/* �Ǽǹ�Աƾ֤��ϸ *
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, tw_br_no, tw_tel );
	if ( ret )
	{
		sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
		WRITEMSG
		goto ErrExit;
	}
	***/

	/* ����ˮ��־ */
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
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/**********************************************************************
 * ��  �� ����  init_pub_tx
 * ���� ����:   ��ʼ��g_pub_tx�ṹ�����Ҵ���Ļȡֵ����g_pub_tx��
 *             
 * ����/ʱ�䣺  andy/2004��1��10��
 * 
 * ��  ����
 *     ���룺  
 *
 *     �����  
 *
 *   ����ֵ��  0: �ɹ�   -1: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
static int init_pub_tx( )
{
	long	lNoteCnt;					/* ƾ֤���� */

	pub_base_sysinit();

	get_zd_data( "0310", g_pub_tx.ac_no );			/* ��    �� */
	get_zd_data( "0890", g_pub_tx.note_type );		/* ƾ֤���� */
	get_zd_data( "0580", g_pub_tx.beg_note_no );		/* ��ʼ���� */
	get_zd_data( "0590", g_pub_tx.end_note_no );		/* ��ֹ���� */
	get_zd_long( "0520", &lNoteCnt );			/* ��ǰ̨����ȡƾ֤���� */
	g_pub_tx.tx_amt1 = (double)lNoteCnt;
	return	0;
	 
}
