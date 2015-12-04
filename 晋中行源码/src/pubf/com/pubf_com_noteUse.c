#include "find_debug.h" 
/*************************************************
* �ļ���:    pubf_com_noteUse.c
* ���������� ƾ֤ʹ�ô�������
*		1. pub_com_NoteUse    ƾ֤ʹ�ú���
*		2. pub_com_CheqClsNo  �ͻ�ƾ֤���Ŵ���
*		3. pub_com_NoteClsNo  ��Աƾ֤���Ŵ���
*
* ��   ��:  jack
* ������ڣ� 2003��12��16��
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
#include "note_min_no_c.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_parm_c.h"

/**********************************************************************
 * ��  �� ����  pub_com_NoteUse
 * ���� ����:   ƾ֤���Ŵ������׹�����ʹ��ƾ֤��
 *	      �������ͻ�ƾ֤�͹�Աƾ֤�����֣�����ƾ֤�������жϣ�������Գ���
 *		���ͻ��������ͻ�ƾ֤��������ɳ��۸��ͻ�������Աƾ֤���������
 *		����룬��ֱ�ӷ���0��������ƾ֤����
 * ����/ʱ�䣺  Terry.Cui/2003��1��12��
 * 
 * ��  ����
 *     ���룺      
 *        char     note_type[4]    ƾ֤����
 *        char     beg_no[17]      ��ʼ����
 *        char     end_no[17]      ��ֹ����
 *        char     tel[5]          ��Ա��
 *
 *     �����  char g_pub_tx.reply[]     ��Ӧ��
 *
 *   ����ֵ��  0: �ɹ�    -1: ���ɹ�
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_NoteUse(
	long	ac_id,
	int	ac_seqn,
    	char    *note_type,
    	char    *beg_no,
    	char    *end_no,
    	char    *tel)
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

	
	sprintf( acErrMsg, "111111111111111sNoteParm.sell_ind=[%s]",sNoteParm.sell_ind);
	WRITEMSG
	sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
	WRITEMSG

	/* �ж��Ƿ����������ͻ� */
	if ( sNoteParm.sell_ind[0] == 'N' )	/* ��������۸��ͻ� */
	{
		
		/**����Ȩ�޼��**/
		if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,401,0 ) )
			return -1;

		ret = pub_com_NoteClsNo( note_type, beg_no, end_no, tel );
		if ( ret )
		{
	   		sprintf( acErrMsg, "����Աƾ֤��" );
	   		WRITEMSG
	   		return(-1);				
		}
	   		sprintf( acErrMsg, "��Աƾ֤ʹ�ü���" );
	   		WRITEMSG
		sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
		WRITEMSG
		ret = pub_acct_NoteUse_log( ac_id, ac_seqn, note_type, beg_no, end_no
			,atol(end_no+sNoteParm.head_len)-atol(beg_no+sNoteParm.head_len)+1);
		if( ret )
		{
	   		sprintf( acErrMsg, "��Աƾ֤ʹ�ü��ʴ�" );
	   		WRITEMSG
	   		return(-1);				
		}
	}else if ( sNoteParm.sell_ind[0] == 'Y' )
   	{
   		ret = pub_com_CheqClsNo( ac_id, ac_seqn, note_type, beg_no, end_no );
		if ( ret )
		{
	   		sprintf( acErrMsg, "���ͻ�ƾ֤��" );
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
 * ��  �� ����  pub_com_NoteClsNo
 * ���� ����:   ��Աƾ֤���Ŵ�����Ա���׹�����ʹ��ƾ֤��
 * ����/ʱ�䣺  jack/2003��12��30��
 * 
 * ��  ����
 *     ���룺      
 *        char     note_type[4]    ƾ֤����
 *        char     beg_no[17]      ��ʼ����
 *        char     end_no[17]      ��ֹ����
 *        char     tel[5]          ��Ա��
 *
 *     �����  char g_pub_tx.reply[]     ��Ӧ��
 *
 *   ����ֵ��  0: �ɹ�    -1: ���ɹ�
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_NoteClsNo( note_type, beg_no, end_no, tel )
    char    *note_type;
    char    *beg_no;
    char    *end_no;
    char    *tel;
{
    struct    note_mst_c    sNoteMst;
    struct    note_min_no_c    sNoteMinNo;
    
    char    cNoteAct[2];            /* ƾ֤��Ϊ */
    char    tw_br_no[6];            /* �Է����� */
    char    tw_tel[6];            /* �Է�����Ա */
    char     cInOut[2];            /* ת�Ʊ�־ I��ת�� O��ת�� */
    
    int    iFlag;                /* �Ƿ������С���� */
    int    ret;
        
    MEMSET_DEBUG( &sNoteMinNo, 0x00, sizeof( struct note_min_no_c ) );
    MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
    
    MEMSET_DEBUG( cNoteAct, 0x00, sizeof( cNoteAct ) );
    MEMSET_DEBUG( tw_br_no, 0x00, sizeof( tw_br_no ) );
    MEMSET_DEBUG( tw_tel, 0x00, sizeof( tw_tel ) );
    MEMSET_DEBUG( cInOut, 0x00, sizeof( cInOut ) );
    
    strcpy( cNoteAct, "D" ); /* ��ʾ��Աʹ��ƾ֤ */
    strcpy( cInOut, "O" );   /* ��Աʹ��ƾ֤,����ת������ */

    ret = pub_com_ChkNoteHead( note_type, beg_no , tel );	
    if ( ret )
    {
	sprintf( acErrMsg, "�����ʼƾ֤����ͷ��" );
	WRITEMSG
	return(-1);
    }
    ret = pub_com_ChkNoteHead( note_type, end_no , tel );	
    if ( ret )
    {
	sprintf( acErrMsg, "�����ֹƾ֤����ͷ��" );
	WRITEMSG
	return(-1);
    }
    
    strcpy( sNoteMst.tel, tel );
    strcpy( sNoteMst.note_type, note_type );
    strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
    strcpy( sNoteMst.beg_note_no, beg_no );
    strcpy( sNoteMst.end_note_no, end_no );

    /* ����ƾ֤���� */
    ret = pub_com_CalNoteCnt( note_type, beg_no, end_no );
    if ( ret == -1 ) 
    {
     	sprintf( acErrMsg, "����ƾ֤������!" );
    	WRITEMSG
   	/*strcpy( g_pub_tx.reply, "N004" );*/	/* ���ú�����reply��ֵ�� */
  	return(-1);    
    }
    sNoteMst.cnt = ret;
    
    /* ��Աƾ֤���:ƾ֤�����Ƿ���ڣ�ƾ֤�Ƿ���ʹ��״̬ */
    ret=pub_com_ChkNoteUse( note_type );
    if( ret==-1 )
    {
        sprintf( acErrMsg, "��Աƾ֤������!" );
        WRITEMSG
        return -1;
    }

    /* ����Ա�Ƿ��д�ƾ֤ */
    ret=pub_com_ChkNote( cNoteAct, sNoteMst );
    if( ret==-1 )	/* ��ǰ�����жϵľ�Ȼ�� 1  ��ֱ̬�Ӳ���0������ modified by liuxuan 2006-12-13*/
    {
        sprintf( acErrMsg, "ƾ֤״̬������!" );
        WRITEMSG
        /*strcpy( g_pub_tx.reply, "N002" );*/ /* ���ú������Ѿ���ֵ�� */
        return -1;
    }
 
    /* ƾ֤��С������ */
    iFlag = pub_com_ChkNoteMin( note_type, beg_no, tel );
    if( iFlag == -1 )
    {
        sprintf( acErrMsg, "ƾ֤��С�ż�����!" );
        WRITEMSG
        return -1;
    }
    else if( iFlag == 2 )  /* ������С���룬������С���� */
    {
        sprintf( acErrMsg, "ƾ֤�Ų�����С��!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "N014" );
        return -1;
    }
    else
    {
        /* ƾ֤�ֶδ��� */
        ret=pub_com_NoteApart( cNoteAct, sNoteMst );
        if( ret==-1 )
        {
            sprintf( acErrMsg, "ƾ֤�ֶβ��ɹ�!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "N023" );
            return -1;
        }

        /* �Ǽǹ�Աƾ̨֤����ϸ */
        ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, tw_br_no, tw_tel );
        if( ret==-1 )
        {
            sprintf( acErrMsg, "�Ǽǹ�Աƾ̨֤����ϸʧ��!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "N015" );
            return -1;
        }
            
        if ( !iFlag ) /* ���iFlag = 0, �������С�ţ���Ҫ�Ǽ���С�� */
        {
            /* �Ǽ�ƾ֤��С�ŵǼǲ�������ƾ֤��С�ż�鷵��ֵΪ0ʱ�Ǽǣ�*/
            ret=pub_com_RegNoteMin( g_pub_tx.tx_br_no, note_type, beg_no, tel, cInOut);
            if( ret==-1 )
            {
                sprintf( acErrMsg, "�Ǽ�ƾ֤��С�ŵǼǲ�ʧ��!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N017" );
                return -1;
            }
        }
    }
    return 0;
}

/**********************************************************************
 * ��  �� ����  pub_com_CheqClsNo
 * ���� ����:   �ͻ�ƾ֤���Ŵ���
 * ����/ʱ�䣺  jack/2003��12��30��
 * 
 * ��  ����
 *     ���룺  	
 *       	long ac_id		�ʻ�ID
 *		int  ac_seqn		�ʻ����
 *              char *note_type         ƾ֤����
 *		char *cheq_beg_no	��ʼ����
 *		char *cheq_end_no	��ֹ����
 *
 *     �����  char g_pub_tx.reply[] 	��Ӧ��
 *
 *   ����ֵ��  0: �ɹ� 	��0: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_CheqClsNo( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no )
	long	ac_id;
	int	ac_seqn;
	char	*note_type;
	char	*cheq_beg_no;
	char	*cheq_end_no;
{
	struct	note_cheq_mst_c	sCheqMst;	/* ��¼�ͻ�ƾ̨֤�� */
	struct	note_mst_c	sNoteMst;
	char	sts[51];	/* ֧Ʊ״̬ */
	long	tx_date;	/* �������� */
	char	tw_br_no[6];/* �Է������� */
   	char   	tw_tel[6];	/* �Է���Ա�� */
	int	ret;
	
	MEMSET_DEBUG( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	
	/***xxx 20030624 upd �����ʼ�����Ƿ������ͷ *
	ret = pub_com_ChkCheqHead( ac_id, ac_seqn, note_type, cheq_beg_no );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "����Ƿ������ͷʧ��!ret=[%d]",ret );
		WRITEMSG
		return(-1);
	}
	* �����ֹ�����Ƿ������ͷ *
	ret = pub_com_ChkCheqHead( ac_id, ac_seqn, note_type, cheq_end_no );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "����Ƿ������ͷʧ��!ret=[%d]",ret );
		WRITEMSG
		return(-1);
	}
	*********************/
	/*
	ret=pub_com_ChkCheqHead_all( ac_id,ac_seqn,note_type,
		cheq_beg_no,cheq_end_no );
	if( ret )
	{
		sprintf( acErrMsg, "����Ƿ������ͷʧ��!ret=[%d]",ret );
		WRITEMSG
		return(-1);
	}
	*/
	
	/* �����ʼ���롢��ֹ�����������,����ͷ�Ƿ���ͬ */
	ret = pub_com_ChkBoeNote( note_type, cheq_beg_no, cheq_end_no);
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "���������,����ͷʧ��!ret=[%d]",ret );
		WRITEMSG
		return(-1);		
	}
	/*******************************/
	
	/* �����ʻ�ID���֧Ʊ�Ƿ�Ϊ���ʺ����� */
	ret=pub_com_ChkAcCheq( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no, sts );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "�ͻ��޴�ƾ֤��ƾ֤״̬��һ��!ac_id=[%d],\
		         note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",\
		         ac_id,note_type,cheq_beg_no, cheq_end_no );
		WRITEMSG
		return(-1);
	}

	if ( sts[0] == '0' )
	{
		sprintf( acErrMsg, "�ͻ��޴�ƾ֤!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N080" );
		return(-1);
	}else if ( sts[0] == '2' )
	{
		sprintf( acErrMsg, "��ƾ֤�ѹ�ʧ!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N081" );
		return(-1);
	}else if ( sts[0] == '3' )
	{
		sprintf( acErrMsg, "��ƾ֤�ѻ���!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N082" );
		return(-1);
	}else if ( sts[0] == '4' )
	{
		sprintf( acErrMsg, "��ƾ֤������!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N083" );
		return(-1);
	}else if ( sts[0] == '5' )
	{
		sprintf( acErrMsg, "��ƾ֤��ʹ��!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N084" );
		return(-1);
	}
	
	/*****************************/
	
	/* ���ͻ�ƾ̨֤���и�֧Ʊ�����־��Ϊ'5'��ʹ��״̬�� */
	strcpy( sNoteMst.note_type, note_type );
	strcpy( sNoteMst.beg_note_no, cheq_beg_no );
	strcpy( sNoteMst.end_note_no, cheq_end_no );
	tx_date=g_pub_tx.tx_date; /*JYW JINZHONG20070421*/
	
	ret=pub_com_RegCheq( sNoteMst, ac_id, ac_seqn, tx_date, "G" );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "�޸�֧Ʊ״̬λ����!ret=[%d]",ret );
		WRITEMSG
		return(-1);
	}
	/* ����ƾ֤���� */
	ret = pub_com_CalNoteCnt( note_type,  cheq_beg_no, cheq_end_no );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "����ƾ֤������ret = [%d]", ret );	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N010" );
		return(-1);
	}	
	sCheqMst.cnt = ret ;
	sCheqMst.ac_id = g_pub_tx.ac_id;
	sCheqMst.ac_seq = g_pub_tx.ac_seqn;
	strcpy( sCheqMst.note_type, note_type );
	strcpy( sCheqMst.beg_note_no, cheq_beg_no );
	strcpy( sCheqMst.end_note_no ,cheq_end_no );
	
	/* �Ǽǿͻ�ƾ̨֤����ϸ C��ʾ�ͻ�ʹ��ƾ֤ */
	strcpy( tw_br_no , "" );
	strcpy( tw_tel, "" );
	
	ret=pub_com_RegCheqHst( "C", sCheqMst, tw_br_no, tw_tel );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "�Ǽǿͻ�ƾ̨֤����ϸʧ��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N021" );
		return(-1);
	}
	
	return 0;
}
