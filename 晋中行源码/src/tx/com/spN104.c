/*************************************************
* �� �� ��:  spN104.c
* ���������� 
*	�������ڲ�����Ա����Խ���ƾ֤�ĵ������ڲ�������
*	���ո��������������������������������������롣
*	�����ǰ���Ǹ�����
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
#include "com_tel_c.h"

/******************* ����ȫ�ֱ��� ********************/
char	g_cOperCode[2];			/* �������� */
char	g_cTwBrno[6];			/* ����ʱ�Է����� */
char	g_cTwTel[5];			/* ����ʱ�Է���Ա */

int spN104()
{
	struct 	note_mst_c	sNoteMst;	/* ��Աƾ̨֤�� */
	struct 	note_mst_c	sTwNoteMst;	/* �Է���Աƾ̨֤�� */
	struct	note_mst_hst_c	sNoteHst;	/* ��Աƾ֤��ϸ */
	struct  com_tel_c	sComTel;	/* ��Ա�ļ� */
	struct  com_tel_c com_tel;
	char	cNoteAct[2];		/* ƾ֤��Ϊ */
	char	cTwNoteAct[2];		/* �Է�ƾ֤��Ϊ */
	char	cNoteSts[2];		/* ƾ֤״̬���Ͻ�ʱ���� */
	char	cInOut[2];
	char	prtime[9];
	int  	hour,min,sec;
	
	int	ret;			/* ��������ֵ */

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sTwNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sNoteHst, 0x00, sizeof( struct note_mst_hst_c ) );
	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( cTwNoteAct, 0x00, sizeof( cTwNoteAct ) );
	memset( cNoteSts, 0x00, sizeof( cNoteSts ) );
	memset( cInOut, 0x00, sizeof( cInOut ) );
	memset(&com_tel,0x0,sizeof(struct com_tel_c));
	/* ��ʼ��g_pub_tx�ṹ */
	init_pub_tx( );

	/**����Ȩ�޼��**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,301,0 ) )
	{
			vtcp_log("������ԱȨ�޴���!");
			goto ErrExit;
	}

	/**�Է�Ȩ�޼��**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_cTwTel,301,1 ) )
	{
			vtcp_log("�Է���ԱȨ�޴���!");
			goto ErrExit;
	}
	if(strlen(g_cTwTel))
	{	
			ret=Com_tel_Sel(g_pub_tx.reply,&com_tel,"tel='%s'",g_cTwTel);
			if(ret)
			{
					vtcp_log("�Է���ԱȨ�޴���!");
					goto ErrExit;
			}
			if(!strcmp(com_tel.tx_type,"0000100000"))
			{
						sprintf(acErrMsg, "���������ù�Ա����ƾ֤!! "); 
						WRITEMSG 
						strcpy(g_pub_tx.reply, "T064"); 
						goto ErrExit; 
			}
  }
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
	
	strcpy( cNoteAct, "4" );	/* 2��ʾ���� �޸�Ϊ4  ƾ֤ת��ת��*/	
	
	/* ��������, 0 ���� 1 ���� */
	switch( g_cOperCode[0] )
	{
		case '0':		/* ���� */
			strcpy( cInOut, "O" );
			
			strcpy( g_pub_tx.brf,"����" );
			strcpy( g_pub_tx.add_ind,"0" );
			strcpy( g_pub_tx.ac_no,"702" );

			/* ���Է���Ա�Ƿ��Ǳ������ù�Ա */
			ret = pub_base_GetTelInfo( g_cTwTel, &sComTel );
			if ( ret )
			{
				sprintf( acErrMsg, "�������뷽��Ա�ļ���,ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}

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
			
			/* �ж��Ƿ�ͽ��׹�Ա����ͳһ�����������㣩*/
			if ( strcmp( g_pub_tx.tx_br_no, sComTel.br_no ) )
			{
				sprintf( acErrMsg, "�Է���Ա�����ڱ�����" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "S041" );
				goto ErrExit;	
			}
			strcpy( g_cTwBrno, sComTel.br_no );
			
			/* ��������Աƾ֤���:�Ƿ�ӵ�У�״̬�Ƿ����� */
			ret = pub_com_ChkNote( cNoteAct, sNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "����Աƾ֤��" );
				WRITEMSG
				goto ErrExit;
			}
			
			/* �Ǽ�ת����Աƾ̨֤�ʣ��ֶδ��� */
			ret = pub_com_NoteApart( cNoteAct, sNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "�Ǽ�ת����Աƾ̨֤�ʣ��ֶδ�����" );
				WRITEMSG
				goto ErrExit;
			}			

			/*��ӡʱ�� add by zy*/
			pub_base_GetTime2( &hour, &min, &sec );
			memset( prtime, 0x00, sizeof( prtime ) );
			sprintf( prtime, "%02d:%02d:%02d", hour, min, sec );    /*��ӡʱ��*/
			set_zd_data( "0600", prtime );
vtcp_log( "zy   prtime[%s]",prtime );
			break;
			
		case '1':		/* ���� */
			strcpy( cInOut, "I" );

			strcpy( g_pub_tx.brf,"����" );
			strcpy( g_pub_tx.add_ind,"1" );
			strcpy( g_pub_tx.ac_no,"702" );
			
			/* ���ҹ�Աƾ֤��ϸ */
			/* ���ݽ�������.ƾ֤����.��ʼƾ֤.��ֹƾ֤.�Է���Ա.ƾ֤��Ϊ */
			ret = pub_com_GetNoteHst( g_pub_tx.tx_date, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, g_pub_tx.tx_br_no, \
				g_pub_tx.tel, cNoteAct, &sNoteHst );
			if ( ret ) 
			{
				sprintf( acErrMsg, "��ѯ��Աƾ֤��ϸ��" );
				WRITEMSG
				goto ErrExit;	
			}
			
			strcpy( g_cTwBrno, sNoteHst.br_no );
			strcpy( g_cTwTel, sNoteHst.tel );
			
			/****���Է���Ա�Ƿ������ƾ֤����Աadd by wl****/
      if ( strcmp( g_cTwTel, "0006" ) ==0 )
      {
      sprintf( acErrMsg, "�������׵ĶԷ���Ա����Ϊ���ƾ֤����Ա!" );
      WRITEMSG
             sprintf( g_pub_tx.reply, "T079" );
      goto ErrExit;
      }
			
			/* ���Է���Ա�Ƿ��Ǳ������ù�Ա */
			if ( strcmp( g_pub_tx.tx_br_no, sNoteHst.br_no ) )
			{
				sprintf( acErrMsg, "�Է���Ա�����ڱ�����" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "S041" );
				goto ErrExit;	
			}
			
			/* ���ƾ֤���ڣ�ȡ��״̬ */
			strcpy( cNoteAct, "5" );	/* 3��ʾ���� �޸�Ϊ 5 ƾ֤ת��ת��*/	
						
			strcpy( sTwNoteMst.tel , sNoteHst.tel );
			strcpy( sTwNoteMst.br_no , sNoteHst.br_no );
			strcpy( sTwNoteMst.note_type , sNoteHst.note_type );
			strcpy( sTwNoteMst.beg_note_no , sNoteHst.beg_note_no );
			strcpy( sTwNoteMst.end_note_no , sNoteHst.end_note_no );
			
			/* ���Է�����Ա�Ƿ�ӵ������ƾ֤ */
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
			strcpy( cTwNoteAct, "4" );	/* 2��ʾ���� �޸�Ϊ 4 ƾ֤ת��ת��*/				
			ret = pub_com_NoteDel ( cTwNoteAct, sTwNoteMst );
			if ( ret )
			{
				sprintf( acErrMsg, "ɾ��/�޸��·���Ա��ƾ֤��" );
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
		goto ErrExit;;
	}
		
	/* ���ƾ֤��С�� *
	ret = pub_com_ChkNoteMin( g_pub_tx.note_type, \
				g_pub_tx.beg_note_no, g_pub_tx.tel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "���ƾ֤��С�Ŵ�" );
		WRITEMSG
		goto ErrExit;	
	}
	if ( !ret )
	{	
	*/
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
	
	memset( g_cOperCode, 0x00, sizeof( g_cOperCode ) );
	memset( g_cTwTel, 0x00, sizeof( g_cTwTel ) );
	 
	pub_base_sysinit();

	get_zd_data( "0670", g_cOperCode );		/* �������� */
	get_zd_data( "0890", g_pub_tx.note_type );	/* ƾ֤���� */
	get_zd_data( "0580", g_pub_tx.beg_note_no );
	get_zd_data( "0590", g_pub_tx.end_note_no );
	get_zd_long( "0520", &lNoteCnt );		/* ƾ֤���� */
	g_pub_tx.tx_amt1 = lNoteCnt;
	get_zd_data( "0920", g_cTwTel );		/* ����ʱ����д�Է���Ա */

	return	0;
}
