/*************************************************
* �� �� ��:  spN102.c
* ���������� 
*	ƾ֤�������·�������֧��,����֧���·�������;
*	ƾ֤�������սɸ�֧���Ͻɵ�ƾ֤,���߸���֧��
*	�սɸ������Ͻɵ�ƾ֤;
*	�˽��װ����������ף�ƾ֤�·�/ƾ֤�ս�;
*	�·���ǰ�������,�սɵ�ǰ�����Ͻ�,
*	�����Ͻɷ�Ϊ����ƾ֤�սɺ�����ƾ֤�սɡ�
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
char	g_cTwBrno[6];			/* �·����� */
char	g_cTwTel[5];			/* �·���Ա */

int spN102()
{
	struct 	note_mst_c	sNoteMst, sTwNoteMst;
	struct 	note_mst_hst_c	sNoteHst;
	struct	com_tel_c	sComTel;		/* ��Ա��Ϣ��ṹ */
	char	cNoteAct[2];		/* ƾ֤��Ϊ */
	char	cTwNoteAct[2];		/* �Է���Աƾ֤��Ϊ */
	char	cInOut[2];		/* �Ǽ�ƾ֤��С���ã�Iת�� Oת�� */
	int	iRelType;		/* ��ϵ���� */
	int	ret;			/* ��������ֵ */

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sTwNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sNoteHst, 0x00, sizeof( struct note_mst_hst_c ) );
	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( cTwNoteAct, 0x00, sizeof( cTwNoteAct ) );
	memset( cInOut, 0x00, sizeof( cInOut ) );

	iRelType = 2;			/* 2��ƾ֤������ϵ */
	
	/* ��ʼ��g_pub_tx�ṹ */
	init_pub_tx( );
	
	strcpy( sNoteMst.tel , g_pub_tx.tel );
	strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
	strcpy( sNoteMst.note_type, g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
	sNoteMst.cnt = (long)g_pub_tx.tx_amt1;

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
	
	/* ƾ֤��Ч�Լ��:�Ƿ����,�Ƿ���ʹ��״̬ */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "���ƾ֤�Ƿ���ڣ��Ƿ����ô�" );
		WRITEMSG
		goto ErrExit;		
	}
	
	/* ��������, 0 �·� 1 �ս� */
	switch( g_cOperCode[0] )
	{
		case '0':			/* �·� */
			/**����Ȩ�޼��**/
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,102,0 ) )
			{
				vtcp_log("[%s][%d]����Ȩ�޴�\n",__FILE__,__LINE__);
					goto ErrExit;
			}

			strcpy( cNoteAct, "2" );	/* 2��ʾ�·� */
			strcpy( cInOut, "O" );		/* ��ʾת�� */

			strcpy( g_pub_tx.brf,"�·�" );
			strcpy( g_pub_tx.add_ind,"0" );
			strcpy( g_pub_tx.ac_no,"702" );


			/* ����·������Ƿ�Ϊ���׻������¼� */
			sprintf(acErrMsg, "CGH [%s], up [%s]111", 
				g_cTwBrno, g_pub_tx.tx_br_no);
			WRITEMSG
			ret = pub_com_ChkBrRel( 2, g_cTwBrno, g_pub_tx.tx_br_no );
			if( ret == 3 )
			{
				sprintf( acErrMsg, "���׻������·�����������ƾ֤���������¼���ϵ!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N077" );
				goto ErrExit;
			}else if ( ret )
			{
				sprintf( acErrMsg, "������֮��Ĺ�ϵ��" );
				WRITEMSG
				goto ErrExit;
			}

			/**�Է�Ȩ�޼��**/
			if( !strcmp(g_cTwBrno,g_pub_tx.tx_br_no) )
			{
				if( pub_com_NoteAuthChk( g_cTwBrno,g_cTwTel,102,10 ) ){
					vtcp_log("[%s][%d]Ȩ�޴�\n",__FILE__,__LINE__);
					goto ErrExit;
				}
			}
			else
			{
				if( pub_com_NoteAuthChk( g_cTwBrno,g_cTwTel,102,1 ) ){
					vtcp_log("[%s][%d]Ȩ�޴�\n",__FILE__,__LINE__);
						goto ErrExit;
				}
			}
			/* ����·���Ա�Ƿ�Ϊ�·�����������Ա */
			ret=Com_tel_Sel( g_pub_tx.reply , &sComTel ,\
			 		"tel='%s'" , g_cTwTel );	
			if( ret==100 )
			{
				sprintf( acErrMsg, "ȡ��Ա��Ϣ���!\
					tel=[%s]", g_pub_tx.tel );
				WRITEMSG
				strcpy( g_pub_tx.reply, "S009");
				goto	ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "ִ��Com_tel_Sel����!ret=[%d]", ret );
				WRITEMSG
				goto	ErrExit;
			}

			/* add by rob at 20040908 **
			if (sComTel.csts[0] == '1' || sComTel.csts[0] == '2')
			{
				sprintf(acErrMsg, "�ù�Ա�Ѿ�ǩ�˲����·�!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply, "N096");
				goto ErrExit;
			}
			*************20041102****************/
			if (sComTel.csts[0] != '0')
			{
				sprintf(acErrMsg, "�ù�Ա����ǩ��״̬�����·�!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "N097");
				goto ErrExit;
			}
			
			sprintf(acErrMsg, "ע��tel:[%s], [%s]", sComTel.br_no, g_cTwBrno);
			WRITEMSG
			pub_base_strpack(sComTel.br_no);
			if( strcmp( sComTel.br_no, g_cTwBrno ) != 0 )
			{
				sprintf( acErrMsg, "�·������޴˹�Ա!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "N076");
				goto	ErrExit;
			}

			/* ������Ա�Ƿ�ӵ������ƾ֤ */
			ret = pub_com_ChkNote( cNoteAct, sNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "����Աƾ֤��" );
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
			break;

		case '1':			/* �ս� */
			/**����Ȩ�޼��**/
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,102,0 ) )
					goto ErrExit;

			strcpy( g_pub_tx.brf,"�ս�" );
			strcpy( g_pub_tx.add_ind,"1" );
			strcpy( g_pub_tx.ac_no,"702" );

			/* ȡ��Աƾ֤��ϸ , �ж��Ƿ���� */
			/* ���ݽ�������.ƾ֤����.��ʼƾ֤.��ֹƾ֤.�Է���Ա.ƾ֤��Ϊ */
			strcpy( cTwNoteAct, "7" );
			ret = pub_com_GetNoteHst( g_pub_tx.tx_date, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, g_pub_tx.tx_br_no, \
				g_pub_tx.tel, cTwNoteAct, &sNoteHst );
			if ( ret == 100 )
			{
				strcpy( cTwNoteAct, "8" );
				ret = pub_com_GetNoteHst( g_pub_tx.tx_date, \
					g_pub_tx.note_type, g_pub_tx.beg_note_no, \
					g_pub_tx.end_note_no, g_pub_tx.tx_br_no, \
					g_pub_tx.tel, cTwNoteAct, &sNoteHst );				
			}	
			if ( ret ) 
			{
				sprintf( acErrMsg, "��ѯ��Աƾ֤��ϸ��" );
				WRITEMSG
				goto ErrExit;	
			}
			
			strcpy( g_cTwBrno, sNoteHst.br_no );
			strcpy( g_cTwTel, sNoteHst.tel );
									
			/* ����Աƾ̨֤���Ƿ���ڶԷ���¼ */
			if ( sNoteHst.sts_ind[0] == '7' )
			{
				strcpy( cNoteAct, "9" );	/* 9��ʾ�����ս� */
				strcpy( cInOut, "I" );		/*����ʾת�� */
			}else if ( sNoteHst.sts_ind[0] == '8' )
			{
				strcpy( cNoteAct, "A" );	/* A��ʾ�����ս� */
			}else
			{
				sprintf( acErrMsg, "ƾ֤��Ϊ��" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N041" );
				goto ErrExit;
			}
			
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
				
			/* ƾ֤���Σ��Ǽǹ�Աƾ̨֤��) */	
			ret = pub_com_RegTelNote(g_pub_tx.tel, g_pub_tx.tx_br_no,\
				      cNoteAct, g_pub_tx.note_type, \
				      g_pub_tx.beg_note_no,g_pub_tx.end_note_no );
			if ( ret )
			{
				sprintf( acErrMsg, "�Ǽǹ�Աƾ̨֤�ʣ����δ�����" );
				WRITEMSG
				goto ErrExit;					
			}
			/* �޸�/ɾ���Է���Աƾ̨֤�� */
			ret = pub_com_NoteDel ( cTwNoteAct, sTwNoteMst );
			if ( ret )
			{
				sprintf( acErrMsg, "ɾ��/�޸��·���Ա��ƾ֤��" );
				WRITEMSG
				goto ErrExit;					
			}
			break;

		default:
			sprintf( acErrMsg, "ƾ֤�����벻����" );
			WRITEMSG
			goto ErrExit;
	}

	/* �Ǽǹ�Աƾ֤��ϸ */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, g_cTwBrno, g_cTwTel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
		WRITEMSG
		goto ErrExit;;
	}
	
	/* �Ǽ�ƾ֤��С�� */	
	if ( strlen( cInOut ) )
	{
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
		vtcp_log("[%s][%d]!!!!!���������[%s]\n",__FILE__,__LINE__,g_pub_tx.reply);
		if(memcmp(g_pub_tx.reply,"0000",4)==0)
        	strcpy( g_pub_tx.reply, "0001" );
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
	memset( g_cTwBrno, 0x00, sizeof( g_cTwBrno ) );
	memset( g_cTwTel, 0x00, sizeof( g_cTwTel ) );
	memset( dc_ind,0,sizeof(dc_ind) );
	 
	pub_base_sysinit();

	get_zd_data( "0670", g_cOperCode );		/* �������� */

	dc_ind[0]=g_cOperCode[0]+1;
	set_zd_data( "0680", dc_ind );		/* �������� */

	get_zd_data( "0890", g_pub_tx.note_type );	/* ƾ֤���� */
	get_zd_data( "0580", g_pub_tx.beg_note_no );
	get_zd_data( "0590", g_pub_tx.end_note_no );
	get_zd_long( "0520", &lNoteCnt );	/* ƾ֤���� */
	g_pub_tx.tx_amt1 = lNoteCnt;
	get_zd_data( "0910", g_cTwBrno );		/* �·����� */
	get_zd_data( "0920", g_cTwTel );		/* �·���Ա */

	return	0;
}
