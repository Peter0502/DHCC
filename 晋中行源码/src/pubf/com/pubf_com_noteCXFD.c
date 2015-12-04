#include "find_debug.h" 
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_min_no_c.h"
#include "note_parm_c.h"

/**********************************************************************
 * �� �� ����  pub_com_NoteApartCX
 * ��������:   ƾ֤�ֶδ�����ԭ������һ������ε�ƾ֤���մ������ʼ
 *	       �������ֹ����ֿ������������ʼ���뵽������ʼ����ǰ��
 *	       һ�δ�����ʼ���뵽��ֹ������һ�Ρ�������ֹ����󵽸���
 *	       ����ֹ����Ϊһ�Ρ��������ʼ������ͬ����ֹ������ͬ��
 *	       ͬ���ܷ����λ�һ�Ρ�
 *             �ֶκ���ܴ��ڲ������������״̬��ͬ�ĺ���������������Ҫ���в��δ���
 *		�����⳷������Ҫɾ��̨��
 *
 *		��ƾ֤�������г����ô˺�����
 *
 * ����/ʱ�䣺 jack/2004��12��16��
 * 
 * ������
 *     ���룺
 *            char   *note_act             ƾ֤��Ϊ
 *            struct note_mst_c note_mst   ��Աƾ̨֤��
 *
 *     �����  char g_pub_tx.reply ��Ӧ��
 *
 *   ����ֵ��  0: ���γɹ� -1: ���ɹ�
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_NoteApartCX(
         char *note_act,
         struct note_mst_c note_mst )
{
    struct note_mst_c sNoteMst;  /* �ӹ�Աƾ̨֤��ȡ����ֵ */
    struct note_mst_c sNoteMstTmp; /* ��Ź�Աƾ̨֤�ʵ���ʱ���� */
    char   cNoteSts[2];          /* ƾ֤״̬ */
    char   cPerNoteSts[2];	/* ��Ϊִ�е�״̬ */
    char   cWho[2];		/* ƾ֤��Ϊִ���� */
    char   str_tmp[32];
    int    ret;
    
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof( struct note_mst_c ));
    MEMSET_DEBUG(cNoteSts, 0x00, sizeof(cNoteSts) );
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts) );
    MEMSET_DEBUG(cWho, 0x00, sizeof(cWho) );
    
    strcpy( cWho, "T" );   /* T��ʾ��Ա */

    /* ����ƾ֤��Ϊ�ҳ�ƾ֤״̬ */
    ret = pub_com_NoteActSts( cWho, note_act, cNoteSts,cPerNoteSts);
    if ( ret )
    {
        sprintf(acErrMsg, "����ƾ֤��Ϊȡƾ֤״̬��!");
        WRITEMSG
        strcpy( g_pub_tx.reply, "N002" );
        return(-1);    
    }
    strcpy( note_mst.sts, cPerNoteSts );   /* ƾ֤��Ϊִ�к�ƾ֤��״̬ */
   
    /* ����Ƿ�������������� */
    /* ��ԭƾ֤״̬�Ĳ��� */
    ret = Note_mst_Dec_Upd(g_pub_tx.reply, "tel = '%s' and note_type = '%s' \
                      and beg_note_no <= '%s' and end_note_no >= '%s' \
                      and sts = '%s'", note_mst.tel, note_mst.note_type,\
                      note_mst.beg_note_no, note_mst.end_note_no, cNoteSts );
    if ( ret )
    {
        sprintf( acErrMsg, "ִ��Note_mst_Dec_Upd��!ret=[%d]",ret );
        WRITEMSG
        return(-1);
    }
   
    ret = Note_mst_Fet_Upd( &sNoteMst,g_pub_tx.reply );
    
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "������������ƾ֤����!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "N007" );
        return(-1);
    }else if ( ret )
    {
        sprintf( acErrMsg, "ִ��Note_mst_Fet_Upd����!ret=[%d]",ret );
        WRITEMSG
        return(-1);
    }else                 /* 0 �ҵ����������ļ�¼ */
    {
        /* ���ͷ�Ƿ���ͬ */
        if( !strcmp(sNoteMst.beg_note_no, note_mst.beg_note_no) ) /* ͷ��ͬ */
        {
            if( !strcmp(sNoteMst.end_note_no, note_mst.end_note_no) ) /* β��ͬ */
            {
                /* �޸�ƾ֤״̬ */
                strcpy( sNoteMst.sts , note_mst.sts );
                
                ret = Note_mst_Upd_Upd( sNoteMst, g_pub_tx.reply );
             	if ( ret == -239 )
                {
                        sprintf( acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��,�����ظ���¼!" );
                	WRITEMSG
                	strcpy(g_pub_tx.reply,"N019");
                	return(-1);
            	}else if( ret )
                {
                    sprintf( acErrMsg,"ִ��Note_mst_Upd_Upd��!ret=[%d]",ret );
                    WRITEMSG
                    return(-1);
                }
                /**********************************************
                  �����δ���                                
                           ����Ƿ�������޸ĺ�ͷβ�����ĺ����  
                           ��Ҫ���ͷβ�Ƿ�ͬʱ��ͬ��,��sNoteMst����note_mst           
                **********************************************/
    		ret = pub_com_NoteApartMerg( note_mst );
    		if ( ret == -1 )
    		{
    			sprintf( acErrMsg, "ƾ֤����ʧ��" );
    			WRITEMSG
    			return(-1);
    		}
            }else  /* β��ͬ */
            {
                /* 1: ����һ���¼�¼*/
                ret = Note_mst_Ins( note_mst,g_pub_tx.reply );
    		if ( ret == -239 )
    		{
    			sprintf(acErrMsg,"���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N020");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"ִ��Note_mst_Ins��!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}
                /*********************************************
                  �����δ���                                
                           ����Ƿ�������޸ĺ�ͷβ�����ĺ����  
                           ��Ҫ���ͷ�Ƿ�ͬʱ��ͬ�ģ���note_mst          
                **********************************************/
    		ret = pub_com_NoteApartMerg( note_mst );
    		if ( ret == -1 )
    		{
    			sprintf( acErrMsg, "ƾ֤����ʧ��" );
    			WRITEMSG
    			return(-1);
    		}
                            
                /* 2: �޸�ԭ��¼����ʼ���롢״̬������ */
                ret = pub_com_CalNoteNo(note_mst.note_type,\
                          note_mst.end_note_no, str_tmp,1);
                if ( ret == -1 ) 
                {
                	sprintf( acErrMsg, "����ƾ֤�����!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);    
                }
                
                strcpy( sNoteMst.beg_note_no, str_tmp );
                
                /* ����ƾ֤���� */
                sNoteMst.cnt -=  note_mst.cnt;

                /* �޸�ԭƾ֤ */
                ret = Note_mst_Upd_Upd( sNoteMst,g_pub_tx.reply );
             	if ( ret == -239 )
                {
                        sprintf( acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��,���ظ���¼!" );
                	WRITEMSG
                	strcpy(g_pub_tx.reply,"N019");
                	return(-1);
            	}else if( ret )
                {
                    sprintf( acErrMsg,"ִ��Note_mst_Upd_Upd��!ret=[%d]",ret );
                    WRITEMSG
                    return(-1);
                }
            }
        }else /* ͷ��ͬ */
        {
            if( !strcmp(sNoteMst.end_note_no, note_mst.end_note_no) ) /* β��ͬ */
            {
                /* 1�������µ����� */
                ret = Note_mst_Ins( note_mst,g_pub_tx.reply );
    		if ( ret == -239 )
    		{
    			sprintf(acErrMsg,"���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N020");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"ִ��Note_mst_Ins��!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}
                /**********************************************
                  �����δ���                                
                           ����Ƿ�������޸ĺ�ͷβ�����ĺ����  
                           ��Ҫ���β�Ƿ�ͬʱ��ͬ��,��note_mst        
                **********************************************/
    		ret = pub_com_NoteApartMerg( note_mst );
    		if ( ret == -1 )
    		{
    			sprintf( acErrMsg, "ƾ֤����ʧ��" );
    			WRITEMSG
    			return(-1);
    		}    		    		
    		
                /* 2���޸�ԭ���� */
                /* ����ƾ֤��ֹ���� */
                ret = pub_com_CalNoteNo(note_mst.note_type,\
                          note_mst.beg_note_no, str_tmp, -1);
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "����ƾ֤�����!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);    
                }                
                strcpy( sNoteMst.end_note_no, str_tmp );
                
                /* ����ƾ֤���� */
                sNoteMst.cnt = sNoteMst.cnt - note_mst.cnt;
                
                ret = Note_mst_Upd_Upd( sNoteMst,g_pub_tx.reply );
             	if ( ret == -239 )
                {
                    sprintf( acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��,���ظ���¼!" );
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"N019");
                    return(-1);
            	}else if( ret )
                {
                    sprintf( acErrMsg,"ִ��Note_mst_Upd_Upd��!ret=[%d]",ret );
                    WRITEMSG
                    return(-1);
                }
            }else   /* β��ͬ */
            {
                sNoteMstTmp = sNoteMst;
                
                /* �޸�ԭ����ƾ֤���ֳ����� */
                /* 1������ƾ֤��ֹ���� */
                ret = pub_com_CalNoteNo(sNoteMst.note_type,note_mst.beg_note_no,\
                                      sNoteMst.end_note_no, -1);
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "����ƾ֤�����!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
               		return(-1);    
                }
                
                ret = pub_com_CalNoteCnt( note_mst.note_type, \
                        sNoteMst.beg_note_no, sNoteMst.end_note_no );
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "����ƾ֤������!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);    
                }
                sNoteMst.cnt = ret;
             
                /* �޸Ĺ�Աƾ֤ */                             
                ret = Note_mst_Upd_Upd( sNoteMst,g_pub_tx.reply );
             	if ( ret == -239 )
                {
                        sprintf( acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��!" );
                	WRITEMSG
                	strcpy(g_pub_tx.reply,"N019");
                	return(-1);
            	}else if( ret )
                {
                    sprintf( acErrMsg,"ִ��Note_mst_Upd_Upd��!ret=[%d]",ret );
                    WRITEMSG
                    return(-1);
                }
            
                /* 2�������µ� */
                /* ����һ���¼�¼*/
                ret = Note_mst_Ins( note_mst,g_pub_tx.reply );
    		if ( ret == -239 )
    		{
    			sprintf(acErrMsg,"���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N020");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"ִ��Note_mst_Ins��!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}
                            
                /* 3��������һ�� */
                /* ����ƾ֤��ʼ���� */
                ret = pub_com_CalNoteNo(sNoteMstTmp.note_type,\
                          note_mst.end_note_no, sNoteMstTmp.beg_note_no, 1);
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "����ƾ֤�����!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);
                }
                /* ����ƾ֤���� */
                ret = pub_com_CalNoteCnt( sNoteMstTmp.note_type, \
                        sNoteMstTmp.beg_note_no, sNoteMstTmp.end_note_no );
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "����ƾ֤�����!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);    
                }
                sNoteMstTmp.cnt = ret;
                /* ����һ���¼�¼*/
                ret = Note_mst_Ins( sNoteMstTmp,g_pub_tx.reply );
    		if ( ret == -239 )
    		{
    			sprintf(acErrMsg,"���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N020");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"ִ��Note_mst_Ins��!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}               
            } 
        }
        
        /* ���Ϊ�����Ϊ����Ҫɾ�� */
        if ( note_act[0] == '1' )
        {
        	ret = sql_execute("delete from note_mst where note_type = '%s' \
        			and beg_note_no = '%s' and end_note_no = '%s' \
        			and tel = '%s'", note_mst.note_type, \
        			note_mst.beg_note_no, note_mst.end_note_no, \
        			note_mst.tel );
        	if ( ret )
        	{
        		sprintf( acErrMsg, "ɾ����Աƾ֤�ǼǱ���[%d]", ret );
        		WRITEMSG
        		strcpy( g_pub_tx.reply, "N061" );
        		return(-1);	
        	}
        }
    }
    
    /* �ر��α� */
    Note_mst_Clo_Upd( );
    
    return(0);
}

/**********************************************************************
 * ��  �� ����  pub_com_RegCheqCX
 * ���� ����:   �Ǽ�֧Ʊ�Ǽǲ�/�޸�ƾ֤״̬,����֧Ʊ�ĺ���ǼǶ�Ӧ�ļ�¼������Ѿ������Ȿ
 *             ƾ֤������Ҫ�޸Ķ�Ӧ״̬��ƾ֤�������Ƿ�ռ�ñ������������ڡ���
 *             �����ڣ���������һ����¼�ȿɡ�
 *             
 *             ��ƾ֤�������г����ô˺�����
 *
 * ����/ʱ�䣺  jack//2004��12��27��
 * 
 * ��  ����
 *     ���룺  
 *            struct note_mst_c note_mst  ��Աƾ̨֤��
 *            long ac_id       �ʻ�id
 *	      long ac_seqn	�ʺ����
 *            long tx_date     ��������
 *            char *note_act   �ͻ�ƾ֤��Ϊ
 *
 *     �����  
 *
 *   ����ֵ��  0: �ɹ�   -1: ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_RegCheqCX(
    struct note_mst_c note_mst,
    int ac_id,
    long ac_seqn,
    long tx_date,
    char *note_act)
{
    struct note_mst_c sNoteMst;       	/* ��Աƾ̨֤�� */
    struct note_parm_c sNoteParm;       /* ƾ֤���� */
    struct note_cheq_mst_c sCheqMst;    /* �ͻ�ƾ̨֤�� */
    
    char    cBegNoteNo[17], cEndNoteNo[17];
    char    cHead[17];            /* ����ͷ */
    char    str_tmp[17];
    char    cForm[32];            /* �����ʽ */
    char    cBookSts[51];         /* ֧Ʊ״̬ */
    char    cABookSts[2];	  /* ֧Ʊִ��֮���״̬ */
    char    cBBookSts[2];	  /* ֧Ʊִ��֮ǰ��״̬ */
    char    cWho[2];  		  /* ƾִ֤���� */
	    
    long    lBegNoteNo, lEndNoteNo;    /* note_mst�е���ʼ����ֹ���� */
    long    lBegNo, lEndNo;    	/* �ܵ���ʼ����ֹ���� */
    long    lDbBeg, lDbEnd; 	/* �������ݿ�ʱ���õ�������� */
    
    int    i, j, k, m = 0;    	/* ѭ����,m ������¼�ܹ�ѭ������ */    
    int    iNoteCnt;    	/* ����ƾ֤���� */
    int    ret;
    
    MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
    MEMSET_DEBUG( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
    MEMSET_DEBUG( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
    
    MEMSET_DEBUG( cBegNoteNo, 0x00, sizeof( cBegNoteNo ) );
    MEMSET_DEBUG( cEndNoteNo, 0x00, sizeof( cEndNoteNo ) );
    MEMSET_DEBUG( cBookSts, 0x00, sizeof( cBookSts ) );
    MEMSET_DEBUG( cABookSts, 0x00, sizeof( cABookSts ) );
    MEMSET_DEBUG( cBBookSts, 0x00, sizeof( cBBookSts ) );
        
    MEMCPY_DEBUG( &sNoteMst, &note_mst, sizeof( struct note_mst_c ) );
    strcpy( cWho , "C" );		/* ��ʾ�ͻ� */
    
    /* ȡƾ֤�������� */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", sNoteMst.note_type );
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "�޴�ƾ֤����note_type=[%s]", sNoteMst.note_type );
        WRITEMSG
        strcpy( g_pub_tx.reply, "N001" );
        return(-1);
    }else if ( ret )
    {
        sprintf( acErrMsg, "ִ��Note_parm_Sel��![%d]", ret );
        WRITEMSG
        return(-1);        
    }    
        
    /* ��ƾ֤״̬��ʼ����0 */
    MEMSET_DEBUG( &cBookSts, '0', sNoteParm.unit_cnt );
    
    /* ���ݿͻ�ƾ֤��Ϊȡִ��֮���ƾ֤״̬ */
    ret = pub_com_NoteActSts( cWho, note_act, cABookSts, cBBookSts );
    if ( ret )
    {
        sprintf(acErrMsg, "����ƾ֤��Ϊȡƾ֤״̬��!");
        WRITEMSG
        strcpy( g_pub_tx.reply, "N002" );
        return(-1);    
    }
   vtcp_log("ROL aaaaaaaaaa[%s][%s][%s][%s]", cWho, note_act, cABookSts, cBBookSts );
    
    /* ȥ������ͷ��ƾ֤����ʼ�������ֹ���� */
    lBegNoteNo = atol( sNoteMst.beg_note_no + sNoteParm.head_len );
    lEndNoteNo = atol( sNoteMst.end_note_no + sNoteParm.head_len );
    
    /* �ܵ���ʼ���롢��ֹ���� */
    lBegNo = (lBegNoteNo - 1) / sNoteParm.unit_cnt * sNoteParm.unit_cnt + 1;
    lEndNo = ( (lEndNoteNo-1) / sNoteParm.unit_cnt + 1 ) * sNoteParm.unit_cnt;
   
    for ( i = lBegNo; i <= lEndNo; i = i + sNoteParm.unit_cnt )
    {
        m++;
        
        /* �������ݿ��õ�ƾ֤��ʼ����ֹ���� */
        lDbBeg = i;
        lDbEnd = i + sNoteParm.unit_cnt - 1;
        
        /* �������ͷ */
        strncpy( cBegNoteNo, sNoteMst.beg_note_no, sNoteParm.head_len );
        strncpy( cEndNoteNo, sNoteMst.end_note_no, sNoteParm.head_len );
        cBegNoteNo[sNoteParm.head_len] = '\0';
        cEndNoteNo[sNoteParm.head_len] = '\0';
        
        /* ������ͷ����ʼ���� */
        sprintf( cForm, "%%0%dld", 16 - sNoteParm.head_len );
        sprintf( str_tmp, cForm, lDbBeg );
        strcat( cBegNoteNo, str_tmp);
        
        /* ������ͷ����ֹ���� */
        sprintf( str_tmp, cForm, lDbEnd );
        strcat( cEndNoteNo, str_tmp);
     
        /* �����ʺš�ƾ֤���ࡢ��ʼ���롢��ֹ����������ݿ� */
        ret = Note_cheq_mst_Dec_Upd(g_pub_tx.reply, "ac_id = %ld \
        		and ac_seq = %d and note_type = '%s' \
        		and beg_note_no = '%s' and end_note_no = '%s'", \
        		ac_id, ac_seqn, sNoteMst.note_type, cBegNoteNo, \
        		cEndNoteNo );
        if ( ret )
        {
            sprintf( acErrMsg, "ִ��Note_cheq_mst_Dec_Upd��!ret= [%d]",ret);
            WRITEMSG
            return(-1);
        }
        ret = Note_cheq_mst_Fet_Upd( &sCheqMst, g_pub_tx.reply);
        if ( ret == 100 )    /* û�ҵ������ļ�¼ */
        {
		sprintf( acErrMsg, "�޴˼�¼" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N012" );
		return(-1);
        }else if ( ret )
        {
            sprintf( acErrMsg, "ִ��Note_cheq_mst_Fet_Upd��!ret= [%d]",ret);
            WRITEMSG
            return(-1);            
        }else
        {        	
            /* �޸ı�־ */
            iNoteCnt = 0;
        
   vtcp_log("ROL bbbbbbbbbbbbbbbb[%s][%s][%s][%s]", cWho, note_act, cABookSts, cBBookSts );
            for ( j = lDbBeg, k = 0; j <= lDbEnd; j++, k++ )
            {
                if( j >= lBegNoteNo && j <= lEndNoteNo )
                {
		/***MODIFY BY ZHGF 20100514 ��������
		Ϊʲô��ֵΪcBBookSts��������ģ�
		Ӧ����cABookSts��Ϊ����ǰ��״̬
			sCheqMst.book_sts[k] = cBBookSts[0]; 
		**XRB cB->cA*/
			sCheqMst.book_sts[k] = cABookSts[0]; /**XRB cB->cA*/
			iNoteCnt++;
                }
            } /* end for */
            
            
             /* ��鳷����ƾ֤��־�Ƿ���0,������ǣ���ɾ�� */   
             /** ����wangyongwei�ĵ��޸ĵ��� 20100514 ADD BY ZHGF ��������
             add by wangyongwei 20070827
	     for ( j = lBegNoteNo, k = 0; j <= lEndNoteNo; j++, k++ )
             **/
             for (j=lDbBeg,k=0; j<=lDbEnd; j++,k++)
             {
                if( sCheqMst.book_sts[k] == '0' )
                {
                    	continue;
                }else
                {
                	break;	
                }
            } /* end for */ 
           
            /* ɾ��ȫΪ0�ü�¼ */
            if ( k == sNoteParm.unit_cnt )
            {
            	continue;
            	ret = sql_execute("delete from note_cheq_mst where  ac_id = %d \
            			and ac_seq = %d and note_type = '%s' \
            			and beg_note_no = '%s' and end_note_no = '%s'", \
            			ac_id, ac_seqn, sNoteMst.note_type, cBegNoteNo, \
            			cEndNoteNo );
            	if ( ret )
            	{
            		sprintf( acErrMsg, "ɾ���ͻ�ƾ̨֤�ʴ�[%d]", ret );
            		WRITEMSG
            		strcpy( g_pub_tx.reply, "B061" );
            		return(-1);
            	}
            	continue;
            }
                        
            /* ���ƾ֤��Ϊ�ǹ�����߽�ң�ƾ֤������ */
            if ( cABookSts[0] == '1' && cBBookSts[0] != '1' )
            {
		/**REPLACE BY NEXT 20100514 ZHGF
		ԭ����1�������1��˵���Ǵ����ˣ�����ʱӦ�ûָ�
		sCheqMst.cnt -= iNoteCnt;            
		**/
		sCheqMst.cnt += iNoteCnt;            
            }else if ( cABookSts[0] != '1' && cBBookSts[0] == '1' )
            {
		/* ADD BY ZHGF 20100514 ��������
		��ԭ������1���������1����Ϊ�ǳ������������Ƿ���
            	sCheqMst.cnt += iNoteCnt;
		*/
            	sCheqMst.cnt -= iNoteCnt;
            }

            /* �������ƾ֤���������涨��������ռ�ñ��� */
            if ( sCheqMst.cnt >= sNoteParm.ocp_cnt )
            {
                sCheqMst.grp_ind[0] = 'Y';
            }else
            {
                sCheqMst.grp_ind[0] = 'N';
            }
	    
	   
            /* �����޸����� */
            ret = Note_cheq_mst_Upd_Upd( sCheqMst, g_pub_tx.reply);
            if ( ret == -239 )
            {
                sprintf( acErrMsg, "�޸Ŀͻ�ƾ̨֤�����ݴ�,���ظ���¼" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N021" );
                return(-1);                
            }else if ( ret )
            {
                sprintf( acErrMsg, "ִ��Note_cheq_mst_Upd_Upd��" );
                WRITEMSG
                return(-1);
            }
        } /* end else */
    }/* end for */
    Note_cheq_mst_Clo_Upd( );  
      
    return(0);
}
