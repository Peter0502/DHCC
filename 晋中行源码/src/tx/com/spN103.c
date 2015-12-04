/*************************************************
* �� �� ��:  spN103.c
* ���������� 
*	�����ϼ����²���ƾ֤;������ƾ֤�Ͻɵ��ϼ���
*	��Ϊ��0���롢1(�����Ͻɡ������Ͻ�)
*	�����ǰ�����·���
*	�����Ͻɵ�ǰ��������״̬ƾ֤������������ս�
*	�����Ͻɵ�ǰ��������״̬ƾ֤������������ս�
*
* ��    ��:  andy
* ������ڣ� 2004��1��7��
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
#include "com_tel_c.h"

/******************* ����ȫ�ֱ��� ********************/
char	g_cOperCode[2];			/* �������� */
char	g_cTwBrno[6];			/* �Ͻ�ʱ�Է����� */
char	g_cTwTel[5];			/* �Ͻ�ʱ�Է���Ա */


int spN103()
{
	struct 	note_mst_c	sNoteMst;	/* ��Աƾ̨֤�� */
	struct 	note_mst_c	sTwNoteMst;	/* �Է���Աƾ̨֤�� */
	struct	note_mst_hst_c	sNoteHst;	/* ��Աƾ֤��ϸ */
	struct  com_tel_c	sComTel;	/* ��Ա�ļ� */
	
	char	cNoteAct[2];		/* ƾ֤��Ϊ */
	char	cTwNoteAct[2];		/* �Է�ƾ֤��Ϊ */
	char	cNoteSts[2];		/* ƾ֤״̬���Ͻ�ʱ���� */
	char	cInOut[2];
	int	iTxRelType;		/* ��ϵ���� */
	
	int	ret;			/* ��������ֵ */

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sTwNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sNoteHst, 0x00, sizeof( struct note_mst_hst_c ) );
	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( cTwNoteAct, 0x00, sizeof( cTwNoteAct ) );
	memset( cNoteSts, 0x00, sizeof( cNoteSts ) );
	memset( cInOut, 0x00, sizeof( cInOut ) );

	iTxRelType = 2;			/* 2��ƾ֤������ϵ */
	
	/* ��ʼ��g_pub_tx�ṹ */
	init_pub_tx( );
		
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
	if ( ret )
	{
		sprintf( acErrMsg, "���ƾ֤��ʼ�������ͷ��" );
		WRITEMSG
		goto ErrExit;
	}
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel );
	if ( ret )	
	{
		sprintf( acErrMsg, "���ƾ֤��ֹ�������ͷ��" );
		WRITEMSG
		goto ErrExit;
	}
			
	/* ��������, 0 ���� 1 �Ͻ� */
	switch( g_cOperCode[0] )
	{
		case '0':		/* ���� */
			/**����Ȩ�޼��**/
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,103,0 ) )
					goto ErrExit;

			strcpy( cNoteAct, "3" );	/* 3��ʾ���� */
			strcpy( cTwNoteAct, "2" );	/* 2��ʾ�·���Ϊ */			
			strcpy( cInOut, "I" );

			strcpy( g_pub_tx.brf,"����" );
			strcpy( g_pub_tx.add_ind,"1" );
			strcpy( g_pub_tx.ac_no,"702" );
			
			/* ���ҹ�Ա̨����ϸ */
			/* ���ݽ�������.ƾ֤����.��ʼƾ֤.��ֹƾ֤.�Է���Ա.ƾ֤��Ϊ */
			ret = pub_com_GetNoteHst( g_pub_tx.tx_date, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, g_pub_tx.tx_br_no, \
				g_pub_tx.tel, cTwNoteAct, &sNoteHst );
			if ( ret ) 
			{
				sprintf( acErrMsg, "��ѯ��Աƾ֤��ϸ��" );
				WRITEMSG
				goto ErrExit;	
			}

			strcpy( g_cTwBrno, sNoteHst.br_no );
			strcpy( g_cTwTel, sNoteHst.tel );

			/* ����Աƾ̨֤���Ƿ���ڸ���ƾ֤ */
			strcpy( sTwNoteMst.tel , sNoteHst.tel );
			strcpy( sTwNoteMst.br_no , sNoteHst.br_no );
			strcpy( sTwNoteMst.note_type , sNoteHst.note_type );
			strcpy( sTwNoteMst.beg_note_no , sNoteHst.beg_note_no );
			strcpy( sTwNoteMst.end_note_no , sNoteHst.end_note_no );
			
			ret = pub_com_ChkNote( cNoteAct, sTwNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "����Աƾ֤��" );
				WRITEMSG
				goto ErrExit;
			}

			/* �Ǽǹ�Աƾ̨֤�ʣ����δ��� */
			ret = pub_com_RegTelNote(g_pub_tx.tel, g_pub_tx.tx_br_no,\
				      cNoteAct, g_pub_tx.note_type, \
				      g_pub_tx.beg_note_no,g_pub_tx.end_note_no );
			if ( ret )
			{
				sprintf( acErrMsg, "�Ǽǹ�Աƾ̨֤�ʣ����δ�����" );
				WRITEMSG
				goto ErrExit;					
			}

			/* ɾ��/�޸��·���Ա��ƾ֤ */
			ret = pub_com_NoteDel ( cTwNoteAct, sTwNoteMst );
			if ( ret )
			{
				sprintf( acErrMsg, "ɾ��/�޸��·���Ա��ƾ֤��" );
				WRITEMSG
				goto ErrExit;					
			}
			break;

		case '1':		/* �Ͻ� */
			strcpy( g_pub_tx.brf,"�Ͻ�" );
			strcpy( g_pub_tx.add_ind,"0" );
			strcpy( g_pub_tx.ac_no,"702" );
			
			/* ���Է���Ա�����Ƿ��ǽ��׻������ϼ����� */
			ret = pub_base_GetTelInfo( g_cTwTel, &sComTel );
			if ( ret )
			{
				sprintf( acErrMsg, "���ݹ�Ա��ȡ��Ա��Ϣ��" );
				WRITEMSG
				goto ErrExit;
			}

			/* add by rob at 20040908 **del xxx 20041102
			if (sComTel.csts[0] == '1' || sComTel.csts[0] == '2') 
			{ 
				sprintf(acErrMsg, "�ù�Ա�Ѿ�ǩ�˲����·�!! "); 
				WRITEMSG 
				strcpy(g_pub_tx.reply, "N096"); 
				goto ErrExit; 
			} 
			if (sComTel.csts[0] != '0') 
			{ 
				sprintf(acErrMsg, "�ù�Ա����ǩ��״̬�����·�!!"); 
				WRITEMSG 
				strcpy(g_pub_tx.reply, "N097"); 
				goto ErrExit; 
			}
			**/

			/**�Է�Ȩ�޼��**/
			if( pub_com_NoteAuthChk( sComTel.br_no,g_cTwTel,104,1 ) )
					goto ErrExit;

			/**����Ȩ�޼��**/
			if( !strcmp(g_pub_tx.tx_br_no,sComTel.br_no) )
			{
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,104,10 ) )
					goto ErrExit;
			}
			else
			{
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,104,0 ) )
					goto ErrExit;
			}
			ret = pub_com_ChkBrRel( 2, g_pub_tx.tx_br_no, sComTel.br_no );
			if( ret == 3 )
			{
				sprintf( acErrMsg, "����Է������뽻�׻���������ƾ֤���������¼���ϵ!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N078" );
				goto ErrExit;
			}else if ( ret )
			{
				sprintf( acErrMsg, "������֮��Ĺ�ϵ��" );
				WRITEMSG
				goto ErrExit;
			}
			
			strcpy( g_cTwBrno, sComTel.br_no );
			
			/* ���ƾ֤���ڣ�ȡ��״̬ */
			ret = pub_com_NoteMstSts( sNoteMst.note_type, \
				sNoteMst.beg_note_no, sNoteMst.end_note_no, \
				sNoteMst.tel, cNoteSts );
			if ( ret )
			{
				sprintf( acErrMsg, "���ƾ֤���ڷ�ȡ��״̬��" );
				WRITEMSG
				/**return(-1);**/	
			}
			
			/* �ж��������Ͻɻ��������Ͻ� */
			if ( cNoteSts[0] == '0' )	/* �����Ͻ� */
			{
				strcpy( cNoteAct, "7" );/* 7��ʾ�����Ͻ� */
				strcpy( cInOut, "O" );
			}else if ( cNoteSts[0] == '7' )	/* �����Ͻ� */
			{
				strcpy( cNoteAct, "8" );/* 8��ʾ�����Ͻ� */
			}else
			{
				sprintf( acErrMsg, "ƾ֤������������״̬�������Ͻ�!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N050" );
				/**return(-1);**/
				goto ErrExit;
			}
			
			/* ������Ա�Ƿ�ӵ������ƾ֤ */
			ret = pub_com_ChkNote( cNoteAct, sNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "����Աƾ֤��" );
				WRITEMSG
				goto ErrExit;
			}
			
			/* �Ǽǹ�Աƾ̨֤�ʣ��ֶδ��� */
			ret = pub_com_NoteApart( cNoteAct, sNoteMst );
			if ( ret )
			{
				sprintf( acErrMsg, "�Ǽǹ�Աƾ̨֤�ʣ��ֶδ�����" );
				WRITEMSG
				goto ErrExit;				
			}
			break;
			
		default:
			sprintf( acErrMsg, "ƾ֤�����벻����,������=[%s]", g_cOperCode );
			WRITEMSG
			goto ErrExit;
	}	/* end of switch */
	
	/* �Ǽǹ�Աƾ֤��ϸ */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, g_cTwBrno, g_cTwTel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
		WRITEMSG
		goto ErrExit;
	}
		
	/* �жϸ���ƾ֤״̬�Ƿ�Ϊ����0 */
	/* �Ǽ�ƾ֤��С�� 	
	if ( strlen( cInOut ) )
	{
		* ���ƾ֤��С�� *
		ret = pub_com_ChkNoteMin( g_pub_tx.note_type, \
					g_pub_tx.beg_note_no, g_pub_tx.tel );
		if ( ret == -1 )
		{
			sprintf( acErrMsg, "���ƾ֤��С�Ŵ�" );
			WRITEMSG
			goto ErrExit;	
		}
		if ( !ret )
		{	*/
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
		/*}*/
	/*}*/

	/* д������־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
		
OkExit:
        strcpy( g_pub_tx.reply, "0000" );
		set_zd_data(DC_GET_MSG,"");
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
	char dc_ind[2];
	
	memset( g_cOperCode, 0x00, sizeof( g_cOperCode ) );
	memset( g_cTwTel, 0x00, sizeof( g_cTwTel ) );
	memset( dc_ind,0,sizeof(dc_ind) );
	 
	pub_base_sysinit();

	get_zd_data( "0670", g_cOperCode );		/* �������� */

	if( g_cOperCode[0]=='0' )
		set_zd_data( "0680", "2" );
	else
		set_zd_data( "0680", "1" );

	get_zd_data( "0890", g_pub_tx.note_type );	/* ƾ֤���� */
	get_zd_data( "0580", g_pub_tx.beg_note_no );
	get_zd_data( "0590", g_pub_tx.end_note_no );
	get_zd_long( "0520", &lNoteCnt );		/* ƾ֤���� */
	g_pub_tx.tx_amt1 = lNoteCnt;
	get_zd_data( "0920", g_cTwTel );		/* �ϽɶԷ���Ա */

	return	0;
}
