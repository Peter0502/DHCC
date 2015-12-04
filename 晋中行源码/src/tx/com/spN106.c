/*************************************************
* �� �� ��:  spN106.c
* ���������� 
*	��Աƾ֤����,����ֻ��ƾ֤״̬����ʱ����Խ���
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
#include <math.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_branch_c.h"

int spN106()
{
	struct 	note_mst_c	sNoteMst;
	char	cNoteAct[2];		/* ƾ֤��Ϊ */
	char	cInOut[2];		/* �Ǽ���С�ŷ��� I ת�� O ת�� */
	char	cTwBrno[6];		/* �Է����� */
	char	cTwTel[5];		/* �Է���Ա */
	int	ret;			/* ��������ֵ */

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( cInOut, 0x00, sizeof( cInOut ) );
	memset( cTwBrno, 0x00, sizeof( cTwBrno ) );
	memset( cTwTel, 0x00, sizeof( cTwTel ) );
	
	/* ��ʼ��g_pub_tx�ṹ */
	init_pub_tx( );
	
	/**updated start 20101021 by sunlicheng ����Ҫ��˽���ֻ��6�Ź�Ա������
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,101,0 )==0 )
		{
			strcpy( g_pub_tx.reply,"N089" );
      goto ErrExit;
    }
  if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,401,0 ) )
			goto ErrExit;
	**/
	/**����Ȩ�޼��
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,101,0 ) )
	{
		if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,401,0 ) )
			goto ErrExit;
	}**/
	strcpy( g_pub_tx.reply,"����" );
	strcpy( g_pub_tx.add_ind,"0" );
	strcpy( g_pub_tx.ac_no,"702" );

	strcpy( cNoteAct, "E" );	/* E��ʾ����ƾ֤ */
	strcpy( cInOut, "O" );
		
	strcpy( sNoteMst.tel , g_pub_tx.tel );
	strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
	strcpy( sNoteMst.note_type, g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
	sNoteMst.cnt = (long)g_pub_tx.tx_amt1;

	/* ƾ֤��Ч�Լ��:�Ƿ����,�Ƿ���ʹ��״̬ */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "���ƾ֤�Ƿ���ڣ��Ƿ����ô�" );
		WRITEMSG
		goto ErrExit;		
	}

	/* ƾ֤��ʼ���롢��ֹ�������ͷ��� */
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
			
	/* ������Ա�Ƿ�ӵ������ƾ֤ */
	/* ����״̬Ϊ���� */
	ret = pub_com_ChkNote( cNoteAct, sNoteMst );
	if( ret )
	{
		sprintf( acErrMsg, "����Աƾ֤��" );
		WRITEMSG
		goto ErrExit;
	}

	sprintf( acErrMsg, "note_type='%s' and beg_note_no='%s'\
	and end_note_no='%s' and tel='%s'", g_pub_tx.note_type, g_pub_tx.beg_note_no,\
	g_pub_tx.end_note_no, g_pub_tx.tel );
WRITEMSG
	/* �������ƾ֤�Ƿ����ڽ��׹�Ա */
	ret = pub_com_ChkNtExt( g_pub_tx.beg_note_no, g_pub_tx.end_note_no, \
				g_pub_tx.note_type, g_pub_tx.tx_br_no, g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "����pub_com_ChkNtExt����!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* �ֶδ���(�޸�ƾ֤״̬) */
	ret = pub_com_NoteApart( cNoteAct, sNoteMst );
	if( ret )
	{
		sprintf( acErrMsg, "�ֶδ���(�޸�ƾ֤״̬)��" );
		WRITEMSG
		goto ErrExit;;
	}

	/* �Ǽǹ�Աƾ֤��ϸ */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, cTwBrno, cTwTel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
		WRITEMSG
		goto ErrExit;;
	}
	
	/* �Ǽ�ƾ֤��С�� */
	ret = pub_com_RegNoteMin( g_pub_tx.tx_br_no, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.tel, cInOut );
	if ( ret )
	{
		sprintf( acErrMsg, "�Ǽ�ƾ֤��С�Ŵ�" );
		WRITEMSG
		goto ErrExit;	
	}

	/* д������־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;;
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
 * ����/ʱ�䣺  andy/2004��1��7��
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
	long	lNoteCnt;		/* ��ǰ̨����ȡƾ֤���� */
	 
	pub_base_sysinit();

	get_zd_data( "0890", g_pub_tx.note_type );	/* ƾ֤���� */
	get_zd_data( "0580", g_pub_tx.beg_note_no );
	get_zd_data( "0590", g_pub_tx.end_note_no );
	get_zd_long( "0520", &lNoteCnt );	/* ƾ֤���� */
	g_pub_tx.tx_amt1 = lNoteCnt;

	return	0;
}
