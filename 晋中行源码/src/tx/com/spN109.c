/*************************************************
* �� �� ��:  spN109.c
* ���������� ���۸��ͻ���ƾ֤��������ԭ��ԭ������������ʹ�ô˽���;
*	     �ջص�ƾ֤Ҫ�ǼǺ���;
*	     �ջص�ƾ֤�����ٽ��г���;
*	     ��Ȩʹ�á�
*
* ��    ��:  andy
* ������ڣ� 2004��1��10��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"
#include "note_mst_hst_c.h"

/*************************  ȫ�ֱ������� *******************/
char	g_sell_way[2];		/* ƾ֤���۷�ʽ 0���ǣ�1����*/

int spN109()
{
	struct	note_mst_c	sNoteMst;	/* ��Աƾ̨֤�� */
	struct	note_cheq_mst_c	sCheqMst;	/* �ͻ�ƾ̨֤�� */
	struct	note_mst_hst_c	sNoteMstHst;	/* ��Աƾ̨֤����ϸ */
	char	cOpenBrNo[6];			/* ���������� */
	char    cNoteSts[2];            	/* ƾ֤״̬ */
	char    cPNoteSts[2];            	/* ƾ֤״̬ */
	char	cNoteAct[2];			/* ƾ֤��Ϊ */
	char	tw_br_no[6];			/* �Է������� */
	char	tw_tel[6];			/* �Է�����Ա */
	char	who[6];				/* ʹ���� */
	int	ret;
	
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	memset( &sNoteMstHst, 0x00, sizeof( struct note_mst_hst_c ) );
	memset( cOpenBrNo, 0x00, sizeof( cOpenBrNo ) );
	memset( cNoteSts, 0x00, sizeof( cNoteSts ) );
	memset( cPNoteSts, 0x00, sizeof( cPNoteSts ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( tw_br_no, 0x00, sizeof( tw_br_no ) );
	memset( tw_tel, 0x00, sizeof( tw_tel ) );
	memset( who, 0x00, sizeof( who ) );
	
	strcpy( cNoteAct, "F" );		/* "F"��ʾΪƾ֤�����ջز��� */

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
	
	/* ƾ֤��ʼ���롢��ֹ�����飨������ͷ�� */
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

	/* ����Ƿ�������������ǣ���Ҫ�ж���ʼ������ֹ�����Ƿ񹹳����� */
	if ( g_sell_way[0] == '1' )
	{
		ret = pub_com_ChkPile( g_pub_tx.note_type, g_pub_tx.beg_note_no, \
					g_pub_tx.end_note_no );
		if ( ret )
		{
			sprintf( acErrMsg, "���ƾ֤�Ƿ���������" );
			WRITEMSG
			goto ErrExit;	
		}
	}
	
/***add amelia  �����ʺŲ�ѯ�˻���ź��˻�ID  **/

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
/***add over***/

	/* �ʻ���Ч�Լ�� */
	ret= pub_base_ChkAc( g_pub_tx.ac_no, g_pub_tx.ac_seqn,cOpenBrNo , &g_pub_tx.ac_id );
	if( ret )
	{
		sprintf( acErrMsg, "�ʻ���Ч�Լ���" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* ����ƾ֤��Ϊ���ض�Ӧƾ֤״̬ */
	strcpy( who, "C" );			/* "C"��ʾ�ͻ���Ϊ */
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
	if( cPNoteSts[0]!='1' )				/* ֧Ʊ״̬ӦΪ����δʹ�� */
	{
		sprintf( acErrMsg, "֧Ʊ״̬��" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N022" );
		goto ErrExit;
	}
	
	/* �޸Ŀͻ�ƾ̨֤�ʣ��޸�״̬�� */
	strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
	strcpy( sNoteMst.tel, g_pub_tx.tel );
	strcpy( sNoteMst.note_type, g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
	sNoteMst.cnt = (int)g_pub_tx.tx_amt1;
	
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
	
	/* ����ƾ֤��Ϊ���ض�Ӧƾ֤״̬ */
	strcpy( who, "T" );			/* "T"��ʾ��Ա��Ϊ */
	ret=pub_com_NoteActSts( who, cNoteAct, cNoteSts, cPNoteSts );
	if ( ret )
	{
		sprintf( acErrMsg, "����ƾ֤��Ϊ���ض�Ӧƾ֤״̬��" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* �Ǽǹ�Աƾ̨֤�ʣ�ƾ֤�ֶδ��� */
	ret=pub_com_NoteApart( cNoteAct, sNoteMst );
	if ( ret )
	{
		sprintf( acErrMsg, "ƾ֤�ֶδ����!" );
		WRITEMSG
		goto ErrExit;
	}

	/* �Ǽǹ�Աƾ֤��ϸ */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, tw_br_no, tw_tel );
	if ( ret )
	{
		sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
		WRITEMSG
		goto ErrExit;
	}

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
	long	lCount;
	
	pub_base_sysinit();
	 memset( g_sell_way, 0x00, sizeof( g_sell_way ) );
	 	 
	get_zd_data( "0310", g_pub_tx.ac_no );			/* �ʺ� */
    pub_base_old_acno( g_pub_tx.ac_no );
	get_zd_int( "0340", &g_pub_tx.ac_seqn );		/* �˻���� */
	get_zd_data( "0890", g_pub_tx.note_type );		/* ƾ֤���� */
	get_zd_data( "0670", g_sell_way );			/* �ջط�ʽ */
	get_zd_data( "0580", g_pub_tx.beg_note_no );		/* ��ʼ���� */
	get_zd_data( "0590", g_pub_tx.end_note_no );		/* ��ֹ���� */
	get_zd_long( "0520", &lCount );				/* �ջ����� */
	g_pub_tx.tx_amt1 = lCount; 
	return	0;
	 
}
