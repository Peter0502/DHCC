#include "find_debug.h" 
/*************************************************
* �ļ���:    pubf_com_noteUndo.c
* ���������� ƾ֤������������
*		1. pub_com_NoteUndo    ƾ֤ʹ�ó�������
*		2. pub_com_CheqUndoNo  �ͻ�ƾ֤��������
*		3. pub_com_NoteUndoNo  ��Աƾ֤��������
*
* ��   ��:  jack
* ������ڣ� 2003��1��22��
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
#include "note_mst_hst_c.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_parm_c.h"

/**********************************************************************
 * ��  �� ����  pub_com_NoteUndo
 * ���� ����:   ƾ֤������������ʹ�ù���ƾ֤��
 *	       ���������ͻ�ƾ֤�͹�Աƾ֤�����֣�����ƾ֤�������жϣ�������Գ���
 *		���ͻ��������ͻ�ƾ֤��������ɳ��۸��ͻ�������Աƾ֤��
 * ����/ʱ�䣺  jack/2003��1��22��
 * 
 * ��  ����
 *     ���룺   
 *	  char	   *note_type	   ����ƾ֤����   
 *        long     trace_no        ��ˮ��
 *        long     trace_seq       ��ˮ���
 *        long     tx_date         ��������
 *        char     *hst_flag       �Ƿ�ɾ����ϸ��¼��־ Yɾ�� N��ɾ��
 *
 *     �����  char g_pub_tx.reply[]     ��Ӧ��
 *
 *   ����ֵ��  0: �ɹ�    -1: ���ɹ�
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_NoteUndo(
	char	*note_type,
	long	trace_no,
	long	trace_cnt,
	long	tx_date,
	char	*hst_flag)
{
	struct note_parm_c sNoteParm;
	
	int	ret;
	
	MEMSET_DEBUG( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
	
	/* ȡƾ֤������:����ƾ֤����ȡƾ֤�ṹ */
	ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type );
	if( ret == 100 )
	{
	    sprintf(acErrMsg,"ȡƾ֤����note_parm���!note_type=[%s]",note_type);
	    WRITEMSG
	    strcpy( g_pub_tx.reply, "N001" );
	    return (-1);
	}else if ( ret )
	{
	    sprintf( acErrMsg, "ִ��Note_parm_Sel����![%d]", ret );
	    WRITEMSG
	    return(-1);
	}
	
	/* �ж��Ƿ�������������ֱ�ӷ�����ȷ */
	if ( sNoteParm.ctl_no_ind[0] == 'N' )
	{
		return(0);	/* ��������룬ֱ�ӷ�����ȷ0 */
	}

	
	/* �ж��Ƿ����������ͻ� */
	if ( sNoteParm.sell_ind[0] == 'N' )	/* ��������۸��ͻ� */
	{
		ret = pub_com_NoteUndoNo( trace_no, trace_cnt, tx_date, hst_flag );
		if ( ret )
		{
	   		sprintf( acErrMsg, "����Աƾ֤��" );
	   		WRITEMSG
	   		return(-1);				
		}
	}else if ( sNoteParm.sell_ind[0] == 'Y' )
   	{
   		ret = pub_com_CheqUndoNo( trace_no, trace_cnt, tx_date, hst_flag );
		if ( ret )
		{
	   		sprintf( acErrMsg, "����Աƾ֤��" );
	   		WRITEMSG
	   		return(-1);				
		}   		
   	}else
   	{
   		sprintf( acErrMsg, "ƾ֤������۱�־��sell_ind=[%s]",sNoteParm.sell_ind );
   		WRITEMSG
   		return(-1);
   	}
   	
   	return(0);
}

/**********************************************************************
 * ��  �� ����  pub_com_NoteUndoNo
 * ���� ����:   ��Աƾ֤��������
 * ����/ʱ�䣺  jack/2003��1��4��
 * 
 * ��  ����
 *     ���룺      
 *        long     trace_no        ��ˮ��
 *        long     trace_seq       ��ˮ���
 *        long     tx_date         ��������
 *        char     *hst_flag       �Ƿ�ɾ��note_hst��¼��־ Yɾ��N��ɾ��
 *
 *     �����  char g_pub_tx.reply[]     ��Ӧ��
 *
 *   ����ֵ��  0: �ɹ�    -1: ���ɹ�
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_NoteUndoNo( trace_no, trace_cnt, tx_date, hst_flag )
       long     trace_no;
       long     trace_cnt;
       long     tx_date;
       char     *hst_flag;
{
	struct	note_mst_c	sNoteMst;	/* ��Աƾ̨֤�� */
	struct 	note_mst_hst_c 	sNoteHst;	/* ��Աƾ֤��ϸ */
	struct 	note_mst_hst_c 	sNoteHstTmp;	/* ��Աƾ֤��ϸ��������ʱ���� */
	char	cNoteSts[2], cPNoteSts[2];	/* ƾ֤��ǰ״̬��֮ǰ��״̬ */
	char	cNoteAct[2];			/* ƾ֤��Ϊ */
	
	int	ret;
	
	MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	MEMSET_DEBUG( &sNoteHst, 0x00, sizeof( struct note_mst_hst_c ) );
	MEMSET_DEBUG( &sNoteHstTmp, 0x00, sizeof( struct note_mst_hst_c ) );
	
	MEMSET_DEBUG( cNoteSts, 0x00, sizeof( cNoteSts ) );
	MEMSET_DEBUG( cPNoteSts, 0x00, sizeof( cPNoteSts ) );
	MEMSET_DEBUG( cNoteAct, 0x00, sizeof( cNoteAct ) );
	
	strcpy( cNoteAct, "N" );		/* ���� */
	
	/* ���ƾ֤��ϸ�Ƿ���� */
	ret = sql_count( "note_mst_hst", "tx_date = %ld and trace_no = %ld \
			and trace_cnt = %ld", tx_date, trace_no, trace_cnt );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "���ҹ�Աƾ֤��ϸ��[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N057" );
		return(-1);
	}else if ( ret = 0 )
	{
		sprintf( acErrMsg, "û�ҵ����������ļ�¼");
		WRITEMSG
		strcpy( g_pub_tx.reply, "N041" );
		return(-1);
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "�ҵ��������������ļ�¼" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N042" );
		return(-1);
	}
	
	
	/* ������ˮ��ȡ��Աƾ֤��ϸ sNoteHst */
	ret = Note_mst_hst_Sel( g_pub_tx.reply , &sNoteHst , "tx_date = %ld \
				and trace_no = %ld and trace_cnt = %d", \
				tx_date, trace_no, trace_cnt );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "��Ա̨����ϸ�����ڷ��������ļ�¼��ret = [%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N041" );
		return(-1);
	}else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Note_mst_hst_Fet_Sel��ret = [%d]",ret);
		WRITEMSG
		return(-1);			
	}
	
	/* ����ϸ��ֵ�� sNoteMst */
	strcpy( sNoteMst.tel, sNoteHst.tel );
	strcpy( sNoteMst.note_type, sNoteHst.note_type );
	strcpy( sNoteMst.br_no, sNoteHst.br_no );
	strcpy( sNoteMst.beg_note_no, sNoteHst.beg_note_no );
	strcpy( sNoteMst.end_note_no, sNoteHst.end_note_no );	
	sNoteMst.cnt = sNoteHst.note_cnt;

	/* �޸�note_mst�� */
	/* �ֶδ��� */
	ret = pub_com_NoteApart( cNoteAct, sNoteMst);
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "ƾ֤�ֶδ�");
		WRITEMSG
		strcpy( g_pub_tx.reply, "N023" );
		return(-1);				
	}
	
	/* �޸�ƾ֤��ϸ�� */
	if ( hst_flag[0] == 'Y' )	/* ɾ��ԭ������ϸ */
	{
		/* ɾ�����֮ǰ������ */
		ret = sql_execute("delete from note_mst_hst where tx_date = %ld \
				and trace_no = %ld and trace_cnt = %ld", \
				sNoteHst.tx_date, sNoteHst.trace_no, \
				sNoteHst.trace_cnt );
		if ( ret )
		{
			sprintf( acErrMsg, "ɾ����Աƾ֤��ϸnote_hst��!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"N043");
			return(-1);
		}		
	}else				/* ��ɾ����ϸ����Ҫ������ϸ */
	{
		sNoteHstTmp = sNoteHst;
		sNoteHstTmp.tx_date = g_pub_tx.tx_date;
		sNoteHstTmp.tx_time = g_pub_tx.tx_time;
		sNoteHstTmp.trace_no = g_pub_tx.trace_no;
		sNoteHstTmp.trace_cnt = g_pub_tx.trace_cnt;
		sNoteHstTmp.trace_cnt = g_pub_tx.trace_cnt;
		strcpy( sNoteHstTmp.sts_ind, cNoteAct );
				
		ret = Note_mst_hst_Ins( sNoteHstTmp, g_pub_tx.reply );
		if ( ret == -239 )
		{
			sprintf( acErrMsg, "�����Աƾ֤��ϸ�����ظ���¼!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"N015");
			return(-1);			
		}else if ( ret )
		{
			sprintf( acErrMsg, "�����Աƾ֤��ϸ��!ret=[%d]", ret );
			WRITEMSG
			return(-1);			
		}
	}
	
    return 0;
}

/**********************************************************************
 * ��  �� ����  pub_com_CheqUndo
 * ���� ����:   �ͻ�ƾ֤����
 *             
 * ����/ʱ�䣺  jack/2003��1��4��
 * 
 * ��  ����
 *     ���룺  
 *            	long	trace_no	��ˮ��
 *		long	trace_cnt	��ˮ�ʴ�
 *            	long   	tx_date  	��������
 *            	char   	*hst_flag    	��־λ��"Y"ɾ����ϸ��"N"��ɾ����ϸ��
 *
 *     �����  
 *            	char    *g_pub_tx.reply	��Ӧ��
 *
 *   ����ֵ��  0: �ɹ�   -1: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_CheqUndoNo( trace_no, trace_cnt, tx_date, hst_flag )
	long	trace_no;
	long	trace_cnt;
	long	tx_date;
	char	*hst_flag;
{
	struct	note_cheq_hst_c	note_cheq_hst;
	struct	note_cheq_mst_c	note_cheq_mst;
	struct  note_mst_c sNoteMst;
	char	cNoteAct[2];			/* ƾ֤��Ϊ */		
	char	note_sts[2];			/* ƾ֤״̬ */
	char	p_note_sts[2];			/* ƾ֤ʹ��ǰ״̬ */
	char    tw_br_no[6];            	/* �Է����� */
    	char    tw_tel[6];            		/* �Է�����Ա */
    	char 	who[2];		       		/* ʹ���� */
    	long	ac_id;				/* �ʻ�ID */
	int	ret;
	
	MEMSET_DEBUG( &note_cheq_hst, 0x00, sizeof(struct note_cheq_hst_c) );
	MEMSET_DEBUG( &note_cheq_mst, 0x00, sizeof(struct note_cheq_mst_c) );
	MEMSET_DEBUG( &sNoteMst, 0x00, sizeof(struct note_mst_c) );
	
	MEMSET_DEBUG( cNoteAct, 0x00, sizeof(cNoteAct) );
	MEMSET_DEBUG( note_sts, 0x00, sizeof(note_sts) );
	MEMSET_DEBUG( p_note_sts, 0x00, sizeof(p_note_sts) );
	MEMSET_DEBUG( tw_br_no, 0x00, sizeof( tw_br_no ) );
    	MEMSET_DEBUG( tw_tel, 0x00, sizeof( tw_tel ) );
    	MEMSET_DEBUG( who, 0x00, sizeof( who ) );
	
	strcpy( cNoteAct, "N" );		/* ��ʾ������Ϊ */
	strcpy( who, "C" );			/* ��ʾ�ͻ�ƾ֤��Ϊ */
		
	/* ���ƾ֤��ϸ�Ƿ���� */
	ret = sql_count( "note_cheq_hst", "tx_date = %ld and trace_no = %ld \
			and trace_cnt = %ld", tx_date, trace_no, trace_cnt );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "���ҿͻ�ƾ֤��ϸ��[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N058" );
		return(-1);
	}else if ( ret = 0 )
	{
		sprintf( acErrMsg, "û�ҵ����������ļ�¼");
		WRITEMSG
		strcpy( g_pub_tx.reply, "N059" );
		return(-1);
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "�ҵ��������������ļ�¼" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N060" );
		return(-1);
	}
		
	/* ������ˮ�š���š���������ȡnote_cheq_hst */
	ret=Note_cheq_hst_Sel( g_pub_tx.reply , &note_cheq_hst , "trace_no=%ld \
				and trace_cnt=%ld and tx_date=%ld" , trace_no, \
				trace_cnt, tx_date );
	if ( ret == 100 )
    	{
        	sprintf( acErrMsg, "û�и���������¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "N059" );
        	return(-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Note_cheq_hst_Sel��![%d]", ret );
        	WRITEMSG
        	return(-1);
    	}
			
	/* ���ݿͻ�ƾ֤��Ϊȡƾ֤ǰ��״̬ */	
	ret=pub_com_NoteActSts( who, cNoteAct, note_sts, p_note_sts );
	if ( ret )
	{
		sprintf( acErrMsg, "���ݿͻ�ƾ֤��Ϊȡƾ֤״̬���ɹ�!" );
		WRITEMSG
		return(-1);
	}
		
	/* ���ͻ�ƾ̨֤��note_cheq_mst */
	ret=pub_com_ChkAcCheq( note_cheq_hst.ac_id, note_cheq_hst.ac_seq, \
			note_cheq_hst.note_type, note_cheq_hst.beg_note_no, \
			note_cheq_hst.end_note_no, p_note_sts );
	if ( ret )
	{
		sprintf( acErrMsg, "�ͻ�ƾ̨֤���в����ڴ�ƾ֤" );
		WRITEMSG
		return(-1);	
	}
		
	strcpy( sNoteMst.tel, note_cheq_hst.tel );
	strcpy( sNoteMst.note_type, note_cheq_hst.note_type );
	strcpy( sNoteMst.beg_note_no, note_cheq_hst.beg_note_no );
	strcpy( sNoteMst.end_note_no, note_cheq_hst.end_note_no );
	sNoteMst.cnt = note_cheq_hst.cnt;
	
	/* �Ǽǿͻ�ƾ̨֤�� */ /*  �ͻ�ƾ֤ȥ��20091217 del by martin 
	ret = pub_com_RegCheq( sNoteMst, note_cheq_hst.ac_id, \
			note_cheq_hst.ac_seq, g_pub_tx.tx_date, cNoteAct );
	if ( ret )
	{
		sprintf( acErrMsg, "�Ǽǿͻ�ƾ̨֤�ʴ�" );
		WRITEMSG
		return(-1);	
	}*/
        	
	/* ���ݱ�־λ���ж��Ƿ�ɾ����ϸ */
	if( hst_flag[0] == 'Y' )
	{
		/* ɾ�����֮ǰ������ */
		ret = sql_execute( "delete from note_cheq_hst where trace_no=%ld \
				and trace_cnt=%ld and tx_date=%ld" , trace_no, \
				trace_cnt, tx_date );
		if ( ret )
		{
			sprintf( acErrMsg, "ɾ���ͻ�ƾ̨֤����ϸnote_cheq_hst��!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"N044");
			return(-1);
		}
	}
	
	/* �����ɾ��������Ҫ����һ����ϸ */
	else
	{
		note_cheq_hst.tx_date = g_pub_tx.tx_date;
		note_cheq_hst.tx_time = g_pub_tx.tx_time;
		note_cheq_hst.trace_no = g_pub_tx.trace_no;
		note_cheq_hst.trace_cnt = g_pub_tx.trace_cnt;
		strcpy( note_cheq_hst.tel, g_pub_tx.tel );
		strcpy( note_cheq_hst.use_ind, cNoteAct );
		
		ret=Note_cheq_hst_Ins( note_cheq_hst , g_pub_tx.reply );
		if ( ret == -239 )
    		{
        		sprintf(acErrMsg,"��ͻ�ƾ֤��ϸ������¼��!���ظ���¼!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N024");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"ִ��Note_cheq_hst_Ins��!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}
	}
	
	return 0; 
}
