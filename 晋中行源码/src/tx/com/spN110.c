/*************************************************
* �� �� ��:  spN110.c
* ���������� ��Աȫ���򲿷�ƾ֤ע��ʹ�ô˽���
*
* ��    ��:  andy
* ������ڣ� 2004��3��25��
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

int spN110()
{
	struct	note_mst_c	sNoteMst;	/* ��Աƾ̨֤�� */
	char	who[6];				/* ʹ���� */
	char    cNoteSts[2];            	/* ƾ֤״̬ */
	char    cPNoteSts[2];            	/* ƾ֤״̬ */
	char	cNoteAct[2];			/* ƾ֤��Ϊ */
	int	ret;

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( who, 0x00, sizeof( who ) );
	memset( cNoteSts, 0x00, sizeof( cNoteSts ) );
	memset( cPNoteSts, 0x00, sizeof( cPNoteSts ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	
	strcpy( cNoteAct, "D" );		/* "D"��ʾʹ�� */
	
	/* ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ�� */
	init_pub_tx( );	

	/**����Ȩ�޼��**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,401,0 ) )
			goto ErrExit;

	/* ����ƾ֤��Ϊ���ض�Ӧƾ֤״̬ */
	strcpy( who, "T" );			/* "T"��ʾ��Ա��Ϊ */

	strcpy( g_pub_tx.brf,"ע��" );
	strcpy( g_pub_tx.add_ind,"0" );
	strcpy( g_pub_tx.ac_no,"702" );

	ret=pub_com_NoteActSts( who, cNoteAct, cNoteSts, cPNoteSts );
	if ( ret )
	{
		sprintf( acErrMsg, "����ƾ֤��Ϊ���ض�Ӧƾ֤״̬��" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* ��Աƾ֤��� */
	ret=pub_com_ChkNoteUse( g_pub_tx.note_type );
	if( ret )
	{
		sprintf( acErrMsg, "��Աƾ֤����" );
		WRITEMSG
		goto ErrExit;
	}
	if( cPNoteSts[0]!='0' )				/* ƾ֤״̬ӦΪ���� */
	{
		sprintf( acErrMsg, "ƾ֤״̬��" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N022" );
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

	/* ��Աƾ֤���Ŵ��� */
	ret=pub_com_NoteClsNo( g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "��Աƾ֤���Ŵ����" );
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
 * ����/ʱ�䣺  andy/2004��3��25��
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

	get_zd_data( "0890", g_pub_tx.note_type );		/* ƾ֤���� */
	get_zd_data( "0580", g_pub_tx.beg_note_no );		/* ��ʼ���� */
	get_zd_data( "0590", g_pub_tx.end_note_no );		/* ��ֹ���� */
	get_zd_long( "0520", &lCount );				/* ƾ֤���� */
	g_pub_tx.tx_amt1 = lCount; 
	return	0; 
}
