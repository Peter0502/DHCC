/*************************************************
* �� �� ��:  spN108.c
* ���������� ƾ֤��ϵͳ�����ϵͳʹ�ô˽���;����ǰ����ƾ֤��������
*	     �ڹ�Աƾ̨֤�ʻ��߿ͻ�ƾ̨֤���У������Ѿ����١�
*
* ��    ��:  andy
* ������ڣ� 2004��1��9��
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
#include "com_branch_c.h"

int spN108()
{
	struct	note_mst_c	sNoteMst;	/* ��Աƾ̨֤�� */
	char	cNoteAct[2];			/* ƾ֤��Ϊ */
	char	tw_br_no[6];			/* �Է������� */
	char	tw_tel[6];			/* �Է�����Ա */
	char	cInOut[2];			/* �Ǽ���С��ƾ֤���� */
	int	ret;
	
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( tw_br_no, 0x00, sizeof( tw_br_no ) );
	memset( tw_tel, 0x00, sizeof( tw_tel ) );
	
	strcpy( cNoteAct, "1" );		/* "1"��ʾΪƾ֤������ */
	strcpy( cInOut, "I" );			/* ��ʾת��ƾ֤ */
	
	/* ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ�� */
	init_pub_tx( );	

	/**����Ȩ�޼��**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,101,0 ) )
			goto ErrExit;

	/* ƾ֤��ʼ���롢��ֹ�����飨������ͷ�� */
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "���ƾ֤��ʼ�������ͷ��" );
		WRITEMSG
		goto ErrExit;	
	}
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel );
	if( ret == -1 )	
	{
		sprintf( acErrMsg, "���ƾ֤��ֹ�������ͷ��" );
		WRITEMSG
		goto ErrExit;	
	}
	/* ���ƾ֤�Ƿ���ڣ��Ƿ����� */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "���ƾ֤�Ƿ���ڣ��Ƿ����ô�" );
		WRITEMSG
		goto ErrExit;		
	}

	/* ��Ա�Ƿ�ӵ��ƾ֤ */
	strcpy( sNoteMst.tel, g_pub_tx.tel );
	strcpy( sNoteMst.br_no,  g_pub_tx.tx_br_no );
	strcpy( sNoteMst.note_type,  g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no,  g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no,  g_pub_tx.end_note_no );
	sNoteMst.cnt = (double)g_pub_tx.tx_amt1;
	
	/* ����Աƾ֤ */
	ret=pub_com_ChkNote( cNoteAct, sNoteMst );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "����Աƾ֤��" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* �Ǽǹ�Աƾ̨֤�� */
	ret=pub_com_RegTelNote( g_pub_tx.tel, g_pub_tx.tx_br_no, cNoteAct, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "�Ǽǹ�Աƾ̨֤�ʴ�" );
		WRITEMSG
		goto ErrExit;
	}	

	/* �Ǽǹ�Աƾ֤��ϸ */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, tw_br_no, tw_tel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
		WRITEMSG
		goto ErrExit;
	}

	/* ���ƾ֤��С�� */
	ret = pub_com_ChkNoteMin( g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				  g_pub_tx.tel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "���ƾ֤��С�Ŵ�" );
		WRITEMSG
		goto ErrExit;	
	}
	if ( !ret  )
	{
		/* �Ǽ���Сƾ֤���� */
		ret = pub_com_RegNoteMin( g_pub_tx.tx_br_no, g_pub_tx.note_type, \
				g_pub_tx.beg_note_no, g_pub_tx.tel, cInOut);
		if ( ret )
		{
			sprintf( acErrMsg, "�Ǽ�ƾ֤��С�Ŵ�" );
			WRITEMSG
			goto ErrExit;	
		}
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
 * ����/ʱ�䣺  andy/2004��1��9��
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
	 
	get_zd_data( "0890", g_pub_tx.note_type );
	get_zd_data( "0580", g_pub_tx.beg_note_no );
	get_zd_data( "0590", g_pub_tx.end_note_no );
	get_zd_long( "0520", &lCount );
	g_pub_tx.tx_amt1 = (double)lCount; 
	 
	return	0;
	 
}
