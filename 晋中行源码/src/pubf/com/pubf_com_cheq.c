#include "find_debug.h"
/*********************************************************************
* �� �� ��:  pubf_com_cheq.c
* ��������:  �ͻ�ƾ֤��������
*
* ��    ��:  jack
* �������:  2004��12��16��
*
* �޸ļ�¼:
*   ��    ��:
*   �� �� ��:
*   �޸�����:
*********************************************************************/
#define MYSQLERR if (ret) { \
            sprintf(acErrMsg, "�����ݿ��[%d]", ret);\
            WRITEMSG \
            goto ErrExit; \
            }
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

/* ������ֹ����ȷ������ͷ */
int pub_com_ChkCheqHead_all(ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no)
    long ac_id;
    int  ac_seqn;
    char *note_type;
    char *cheq_beg_no;
    char *cheq_end_no;
{
    struct note_parm_c sNoteParm;
    int len;
    int ret;

    pub_base_strpack(cheq_beg_no);
    pub_base_strpack(cheq_end_no);

    if (strlen(cheq_beg_no) < 16)
        ;
    else if (cheq_beg_no[0] != '0' && cheq_beg_no[0] != ' ')
        goto OkExit;
    else
        ;

    /* ����ƾ֤����ȡƾ֤������ */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, \
          "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        goto ErrExit;
    }
    MYSQLERR

    /* ȥ������ͷ����볤�� */
/*
    len=strlen(cheq_beg_no);

    lCheqNo = atol(cheq_beg_no);
    sprintf(cForm, "%%0%dd", 16 - sNoteParm.head_len);
    sprintf(cCheqNo, cForm, lCheqNo);

    * ���note_cheq_mst���Ƿ���ں���Ϊcheq_no��ƾ֤ *
    ret = sql_count("note_cheq_mst", "ac_id = '%ld' and ac_seq =%d \
                and note_type = '%s' \
                and beg_note_no[%d,16]<='%s' and end_note_no[%d,16]>='%s'",\
                ac_id, ac_seqn, note_type, sNoteParm.head_len+1, cCheqNo, \
                sNoteParm.head_len+1, cCheqNo);
    if (ret < 0)
    {
        sprintf(acErrMsg,"ִ��sql_count��, ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if (ret > 1)
    {
        sprintf(acErrMsg,"�ҵ�����ƾ֤,�������̫��,ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N008");
        return 1;
    }
    if (ret == 0)
    {
        sprintf(acErrMsg,"��Ա�޴�ƾ֤note_no[%s]", cheq_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }

    * ����ƾ֤������ʼƾ֤(������ͷ)*
    ret = Note_cheq_mst_Sel(g_pub_tx.reply, &sCheqMst, "ac_id = '%ld' and ac_seq = %d \
                    and note_type = '%s' and beg_note_no[%d,16]<='%s' and \
                    end_note_no[%d,16]>='%s'", ac_id, ac_seqn, note_type, \
                    sNoteParm.head_len+1, cCheqNo, sNoteParm.head_len+1, cCheqNo);
    if (ret == 100)
    {
        sprintf(acErrMsg,"ִ��Note_mst_Sel��,[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N012");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg,"ִ��Note_mst_Sel��,[%d]", ret);
        WRITEMSG
        return(-1);
    }

    * ȡ����ͷ+ƾ֤����,���� *
    strncpy(cHead, sCheqMst.beg_note_no, sNoteParm.head_len);
    cHead[sNoteParm.head_len] = '\0';

    strcpy(cheq_no, cHead);
    strcat(cheq_no, cCheqNo);
*/

OkExit:
    return 0;
ErrExit:
    return 1;
}

/*********************************************************************
* �� �� ��:  pub_com_ChkCheqHead
* ��������:  �������Ŀͻ�ƾ֤�����Ƿ�С��16λ
*            ���С��16λ, �����Ƿ����ظ��Ĺ���ͷ, �����, ���ش�����ʾ,
*            ���û��, �򷵻ش�����ͷ�Ŀͻ�ƾ֤����
*            �������16λ, �򷵻ؿͻ�ƾ֤����
*
* ��    ��:  jack
* ʱ    ��:  2004��12��27��
*
* ��    ��:
*     ����:
*            long  ac_id       �˻���ʶ��
*            int   ac_seqn     �˻����
*            char  *note_type  ƾ֤����
*            char  *cheq_no    ֧Ʊ����(���ܴ�����ͷ, Ҳ���ܲ���)
*
*     ���:
*            char  *cheq_no    ֧Ʊ����(������ͷ)
*
*   ����ֵ:  0: �ɹ�  1: ʧ��
*
* �޸���ʷ:  �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
*
*********************************************************************/

int pub_com_ChkCheqHead(
    long  ac_id,
    int   ac_seqn,
    char  *note_type,
    char  *cheq_no)
{
    struct  note_cheq_mst_c  sCheqMst;
    struct  note_parm_c      sNoteParm;
    char    cForm[32];
    char    cCheqNo[17];
    char    cHead[9];
    long    lCheqNo;          /* ȥ������ͷ��֧Ʊ���� */
    int     iNoHeadLen;       /* ȥ������ͷ����볤�� */
    int     ret;

    /* ���ƾ֤�ų���16λ, ��ֱ�ӷ��� */
    if (strlen(cheq_no) == 16)
    {
        sprintf(acErrMsg, "ƾ֤�ų�����16λ, ��������ͷ[%s]", cheq_no);
        WRITEMSG
        return 0;
    }

    /* ����ƾ֤����ȡƾ֤������ */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, \
          "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel��![%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* ȥ������ͷ����볤�� */
    lCheqNo = atol(cheq_no);            /* ƾ֤���� */
    sprintf(cForm, "%%0%dd", 16 - sNoteParm.head_len);
    sprintf(cCheqNo, cForm, lCheqNo);

    /* ���note_cheq_mst���Ƿ���ں���Ϊcheq_no��ƾ֤ */
    ret = sql_count("note_cheq_mst", "ac_id = '%ld' and ac_seq = %d \
        and note_type = '%s' \
        and beg_note_no[%d,16] <= '%s' and end_note_no[%d,16] >= '%s'",\
        ac_id, ac_seqn, note_type, sNoteParm.head_len+1, cCheqNo, \
        sNoteParm.head_len+1, cCheqNo);
    if (ret < 0)
    {
        sprintf(acErrMsg,"ִ��sql_count��, ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if (ret > 1)
    {
        sprintf(acErrMsg,"�ҵ�����ƾ֤,�������̫��,ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N008");
        return 1;
    }
    if (ret == 0)
    {
        sprintf(acErrMsg, "��Ա�޴�ƾ֤note_no[%s]", cheq_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }

    /* ����ƾ֤������ʼƾ֤(������ͷ)*/
    ret = Note_cheq_mst_Sel(g_pub_tx.reply, &sCheqMst, "ac_id = '%ld' and ac_seq = %d \
                    and note_type = '%s' and beg_note_no[%d,16]<='%s' and \
                    end_note_no[%d,16]>='%s'", ac_id, ac_seqn, note_type, \
                    sNoteParm.head_len+1, cCheqNo, sNoteParm.head_len+1, cCheqNo);
    if (ret == 100)
    {
        sprintf(acErrMsg,"ִ��Note_mst_Sel��,[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N012");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg,"ִ��Note_mst_Sel��,[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* ȡ����ͷ+ƾ֤����,���� */
    strncpy(cHead, sCheqMst.beg_note_no, sNoteParm.head_len);
    cHead[sNoteParm.head_len] = '\0';

    strcpy(cheq_no, cHead);
    strcat(cheq_no, cCheqNo);

    return(0);
}

/**********************************************************************
 * ��  �� ��:   pub_com_ChkAcCheq
 * ���� ����:   ����˺š�ƾ֤���ࡢƾ֤��ʼ����ֹ�����Ƿ����,
 *             ������ڷ���ƾ֤״̬,����, �ú��������ƾ֤������������,
 *             ����״̬��ͬ��
 *
 * ����/ʱ��:   jack/2004��12��27��
 *
 * ��  ��:
 *     ����:
 *            long    ac_id       �˻���ʶ��
 *        int    ac_seqn     �˺����
 *            char   *note_type  ƾ֤����
 *            char   *cheq_beg_no    ֧Ʊ��ʼ����
 *            char   *cheq_end_no    ֧Ʊ��ֹ����
 *
 *     ���:
 *            char   *book_sts   ƾ֤״̬
 *
 *   ����ֵ:   0: ��ƾ֤   -1: ��ƾ֤
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_ChkAcCheq(
    long     ac_id,
    int     ac_seqn,
    char    *note_type,
    char    *cheq_beg_no,
    char    *cheq_end_no,
    char    *book_sts)
{
    struct note_parm_c sNoteParm;        /* ƾ֤���� */
    struct note_cheq_mst_c sCheqMst;    /* �ͻ�ƾ̨֤�� */

    char    cBegNoteNo[17], cEndNoteNo[17];
    char    str_tmp[17];
    char    cForm[32];            /* �����ʽ */
    char    cBookSts[2];            /* ֧Ʊ״̬ */

    long    lBegNoteNo, lEndNoteNo;    /* note_mst�е���ʼ����ֹ���� */
    long    lBegNo, lEndNo;    /* �ܵ���ʼ����ֹ���� */
    long    lDbBeg, lDbEnd; /* �������ݿ�ʱ���õ�������� */

    int    i, j, k;    /* ѭ���� */
    int    iNoteCnt;    /* ����ƾ֤���� */
    int    iFlag = 0;
    int    ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sCheqMst, 0x00, sizeof(struct note_cheq_mst_c));

    MEMSET_DEBUG(cBegNoteNo, 0x00, sizeof(cBegNoteNo));
    MEMSET_DEBUG(cEndNoteNo, 0x00, sizeof(cEndNoteNo));
    MEMSET_DEBUG(cBookSts, 0x00, sizeof(cBookSts));

    /* ȡƾ֤�������� */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel��![%d]", ret);
        WRITEMSG
        return(-1);
    }

    vtcp_log("cheq_beg_no����ʼ��[%16s]",cheq_beg_no);
    vtcp_log("cheq_end_no����ֹ��[%16s]",cheq_end_no);


    /* ȥ������ͷ��ƾ֤����ʼ�������ֹ���� */
    lBegNoteNo = atol(cheq_beg_no + sNoteParm.head_len);
    lEndNoteNo = atol(cheq_end_no + sNoteParm.head_len);

    vtcp_log("lBegNotoNo1����ʼ��[%ld]",lBegNoteNo);
    vtcp_log("lEndNotoNo1����ֹ��[%ld]",lEndNoteNo);

    /* �ܵ���ʼ���롢��ֹ���� */
    vtcp_log("lBegNo����ʼ��1[%ld]",lBegNo);
    lBegNo = (lBegNoteNo - 1) / sNoteParm.unit_cnt * sNoteParm.unit_cnt + 1;
    lEndNo = ((lEndNoteNo-1) / sNoteParm.unit_cnt + 1) * sNoteParm.unit_cnt;
    vtcp_log("lBegNo����ʼ��[%ld]",lBegNo);
    vtcp_log("lEndNo����ֹ��[%ld]",lEndNo);
    for (i = lBegNo; i <= lEndNo; i = i + sNoteParm.unit_cnt)
    {
        /* �������ݿ��õ�ƾ֤��ʼ����ֹ���� */
        lDbBeg = i;
        lDbEnd = i + sNoteParm.unit_cnt - 1;

        /* �������ͷ */
        strncpy(cBegNoteNo, cheq_beg_no, sNoteParm.head_len);
        strncpy(cEndNoteNo, cheq_end_no, sNoteParm.head_len);
        cBegNoteNo[sNoteParm.head_len] = '\0';
        cEndNoteNo[sNoteParm.head_len] = '\0';

        /* ������ͷ����ʼ���� */
        sprintf(cForm, "%%0%dld", 16 - sNoteParm.head_len);
        sprintf(str_tmp, cForm, lDbBeg);
        strcat(cBegNoteNo, str_tmp);

        /* ������ͷ����ֹ���� */
        sprintf(str_tmp, cForm, lDbEnd);
        strcat(cEndNoteNo, str_tmp);

        /* �����˺š�ƾ֤���ࡢ��ʼ���롢��ֹ����������ݿ� */
        sprintf(acErrMsg,"ac_id=%ld and ac_seq=%d and note_type='%s' and beg_note_no<='%s' and end_note_no>='%s'",
                ac_id, ac_seqn, note_type, cBegNoteNo, cEndNoteNo);
            WRITEMSG
        ret = Note_cheq_mst_Dec_Upd(g_pub_tx.reply, "ac_id = %ld \
                and ac_seq = %d and note_type = '%s' \
                and beg_note_no<='%s' and end_note_no>='%s'", \
                ac_id, ac_seqn, note_type, cBegNoteNo, cEndNoteNo);
        if (ret)
        {
            sprintf(acErrMsg,
                "ac_id=%ld and ac_seq=%d and note_type='%s' and beg_note_no<='%s' and end_note_no>='%s'",
                ac_id, ac_seqn, note_type, cBegNoteNo, cEndNoteNo);
            WRITEMSG
            return(-1);
        }

        ret = Note_cheq_mst_Fet_Upd(&sCheqMst, g_pub_tx.reply);
        if (ret == 100)    /* û�ҵ������ļ�¼ */
        {
            sprintf(acErrMsg, "����ƾ֤������");
            WRITEMSG
        strcpy(g_pub_tx.reply, "N069");
            return(-1);
        }
        else if (ret)
        {
            sprintf(acErrMsg, "ִ��Note_cheq_mst_Fet_Upd��!ret= [%d]",ret);
            WRITEMSG
            return(-1);
        }
        else
        {
sprintf(acErrMsg, "lBegNoteNo=[%ld],lEndNoteNo=[%ld]",lBegNoteNo,lEndNoteNo);
WRITEMSG
sprintf(acErrMsg, "lBeg=[%ld],lEnd=[%ld][%s]",lDbBeg,lDbEnd,sCheqMst.book_sts);
WRITEMSG
            for (j = lDbBeg, k = 0; j <= lDbEnd; j++, k++)
            {
                if (j >= lBegNoteNo && j <= lEndNoteNo)
                {
sprintf(acErrMsg, "lBeglEnd=[%ld] k[%d]",j,k);
WRITEMSG
                    iFlag++;
                    if (iFlag == 1)
                    {
                        cBookSts[0] = sCheqMst.book_sts[k];
                    }
                    if (sCheqMst.book_sts[k] == '0')
                    {
                        sprintf(acErrMsg, "����ƾ֤������");
                        WRITEMSG
                        strcpy(g_pub_tx.reply, "N069");
                        return(-1);
                    }else
                    {
                        if (cBookSts[0] != sCheqMst.book_sts[k])
                        {
                            sprintf(acErrMsg, "ƾ֤״̬��һ��");
                            WRITEMSG
                            strcpy(g_pub_tx.reply, "N070");
                            return(-1);
                        }
                    }
                    cBookSts[0] = sCheqMst.book_sts[k];
                }
            } /* end for */
            strcpy(book_sts, cBookSts);
        }
        Note_cheq_mst_Clo_Upd();
    }/* end for */

    return(0);
}

/**********************************************************************
 * ��  �� ��:   pub_com_RegCheq
 * ���� ����:   �Ǽ�֧Ʊ�Ǽǲ�/�޸�ƾ֤״̬, ����֧Ʊ�ĺ���ǼǶ�Ӧ�ļ�¼������Ѿ������Ȿ
 *             ƾ֤, ����Ҫ�޸Ķ�Ӧ״̬��ƾ֤�������Ƿ�ռ�ñ������������ڡ���
 *             ������, ��������һ����¼�ȿɡ�
 *
 *             ֧Ʊת��/����/ʹ��/����
 *                   0:��  1:����δʹ��  2:��ʧ  3:����  4:����5ʹ��
 *
 * ����/ʱ��:   jack//2004��12��27��
 *
 * ��  ��:
 *     ����:
 *            struct note_mst_c note_mst  ��Աƾ̨֤��
 *            long ac_id       �˻�id
 *        long ac_seqn  �˺����
 *            long tx_date     ��������
 *            char *note_act   �ͻ�ƾ֤��Ϊ
 *
 *     ���:
 *
 *   ����ֵ:   0: �ɹ�   -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_RegCheq(
    struct note_mst_c note_mst,
    long ac_id,
    int ac_seqn,
    long tx_date,
    char *note_act)
{
    struct note_mst_c sNoteMst;         /* ��Աƾ̨֤�� */
    struct note_parm_c sNoteParm;       /* ƾ֤���� */
    struct note_cheq_mst_c sCheqMst;    /* �ͻ�ƾ̨֤�� */

    char    cBegNoteNo[17], cEndNoteNo[17];
    char    cHead[17];            /* ����ͷ */
    char    str_tmp[17];
    char    cForm[32];            /* �����ʽ */
    char    cBookSts[51];         /* ֧Ʊ״̬ */
    char    cABookSts[2];     /* ֧Ʊִ��֮���״̬ */
    char    cBBookSts[2];     /* ֧Ʊִ��֮ǰ��״̬ */
    char    cSts[2];              /* ��ʱ����״̬, �����Ƚ����������� */
    char    cWho[2];          /* ƾִ֤���� */

    long    lBegNoteNo, lEndNoteNo;    /* note_mst�е���ʼ����ֹ���� */
    long    lBegNo, lEndNo;     /* �ܵ���ʼ����ֹ���� */
    long    lDbBeg, lDbEnd;     /* �������ݿ�ʱ���õ�������� */

    int    i, j, k, m = 0;      /* ѭ����,m ������¼�ܹ�ѭ������ */
    int    iNoteCnt;        /* ����ƾ֤���� */
    int    ret;

    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sCheqMst, 0x00, sizeof(struct note_cheq_mst_c));

    MEMSET_DEBUG(cBegNoteNo, 0x00, sizeof(cBegNoteNo));
    MEMSET_DEBUG(cEndNoteNo, 0x00, sizeof(cEndNoteNo));
    MEMSET_DEBUG(cBookSts, 0x00, sizeof(cBookSts));
    MEMSET_DEBUG(cABookSts, 0x00, sizeof(cABookSts));
    MEMSET_DEBUG(cBBookSts, 0x00, sizeof(cBBookSts));

    MEMCPY_DEBUG(&sNoteMst, &note_mst, sizeof(struct note_mst_c));
    strcpy(cWho , "C");     /* ��ʾ�ͻ� */

    /* ȡƾ֤�������� */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", sNoteMst.note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����note_type=[%s]", sNoteMst.note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel��![%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* ��ƾ֤״̬��˼����0 */
    MEMSET_DEBUG(&cBookSts, '0', sNoteParm.unit_cnt);

    /* ���ݿͻ�ƾ֤��Ϊȡִ��֮���ƾ֤״̬ */
    ret = pub_com_NoteActSts(cWho, note_act, cABookSts, cBBookSts);
    if (ret)
    {
        sprintf(acErrMsg, "����ƾ֤��Ϊȡƾ֤״̬��!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N002");
        return(-1);
    }

    /* ȥ������ͷ��ƾ֤����ʼ�������ֹ���� */
    lBegNoteNo = atol(sNoteMst.beg_note_no + sNoteParm.head_len);
    lEndNoteNo = atol(sNoteMst.end_note_no + sNoteParm.head_len);

    /* �ܵ���ʼ���롢��ֹ���� */
    lBegNo = (lBegNoteNo - 1) / sNoteParm.unit_cnt * sNoteParm.unit_cnt + 1;
    lEndNo = ((lEndNoteNo-1) / sNoteParm.unit_cnt + 1) * sNoteParm.unit_cnt;

    for (i = lBegNo; i <= lEndNo; i = i + sNoteParm.unit_cnt)
    {
        m++;

        /* �������ݿ��õ�ƾ֤��ʼ����ֹ���� */
        lDbBeg = i;
        lDbEnd = i + sNoteParm.unit_cnt - 1;

        /* �������ͷ */
        strncpy(cBegNoteNo, sNoteMst.beg_note_no, sNoteParm.head_len);
        strncpy(cEndNoteNo, sNoteMst.end_note_no, sNoteParm.head_len);
        cBegNoteNo[sNoteParm.head_len] = '\0';
        cEndNoteNo[sNoteParm.head_len] = '\0';
        /* ������ͷ����ʼ���� */
        sprintf(cForm, "%%0%dld", 16 - sNoteParm.head_len);
        sprintf(str_tmp, cForm, lDbBeg);
        strcat(cBegNoteNo, str_tmp);

        /* ������ͷ����ֹ���� */
        sprintf(str_tmp, cForm, lDbEnd);
        strcat(cEndNoteNo, str_tmp);

        /* �����˺š�ƾ֤���ࡢ��ʼ���롢��ֹ����������ݿ� */
        ret = Note_cheq_mst_Dec_Upd(g_pub_tx.reply, "ac_id = %ld \
                and ac_seq = %d and note_type = '%s' \
                and beg_note_no = '%s' and end_note_no = '%s'", \
                ac_id, ac_seqn, sNoteMst.note_type, cBegNoteNo, cEndNoteNo);
        if (ret)
        {
            sprintf(acErrMsg, "ִ��Note_cheq_mst_Dec_Upd��!ret= [%d]",ret);
            WRITEMSG
            return(-1);
        }
        ret = Note_cheq_mst_Fet_Upd(&sCheqMst, g_pub_tx.reply);
        if (ret == 100)    /* û�ҵ������ļ�¼ */
        {
            /* ���note_act=K,���ʾ������ƾ֤ */
            if (note_act[0] != 'K')
            {
                sprintf(acErrMsg, "�ͻ��޴�ƾ֤");
                WRITEMSG
                sprintf(g_pub_tx.reply, "N012");
                return(-1);
            }else
            {
                /* ����һ����¼ */
                sCheqMst.ac_id = ac_id;    /* �˺�id */
                sCheqMst.ac_seq = ac_seqn;
        strcpy(sCheqMst.br_no, g_pub_tx.tx_br_no);
                strcpy(sCheqMst.note_type, sNoteMst.note_type);
                strcpy(sCheqMst.beg_note_no, cBegNoteNo);
                strcpy(sCheqMst.end_note_no, cEndNoteNo);
                strcpy(sCheqMst.book_sts, cBookSts);
                sCheqMst.sell_date = tx_date;
                iNoteCnt = 0;

                sprintf(acErrMsg, "33333333 lBegNoteNo=[%d], lEndNoteNo=[%d]", lBegNoteNo, lEndNoteNo);
                WRITEMSG
                sprintf(acErrMsg, "33333333 sCheqMst.cnt=[%d]", sCheqMst.cnt);
                WRITEMSG
                for (j = lDbBeg, k = 0; j <= lDbEnd; j++, k++)
        {
                    if (j >= lBegNoteNo && j <= lEndNoteNo)
                    {
                        sCheqMst.book_sts[k] = cABookSts[0];
                            iNoteCnt++;
                    }
                } /* end for */
                sprintf(acErrMsg, "33333333 iNoteCnt=[%d]", iNoteCnt);
                WRITEMSG
                sCheqMst.cnt = iNoteCnt;
                /* �������ƾ֤���������涨����, ��ռ�ñ��� */
                if (sCheqMst.cnt >= sNoteParm.ocp_cnt)
                {
                    sCheqMst.grp_ind[0] = 'Y';
                }else
                {
                    sCheqMst.grp_ind[0] = 'N';
                }
sprintf(acErrMsg, "111111111111sCheqMst.grp_ind=[%s],sNoteParm.ocp_cnt=[%d] ",\
    sCheqMst.grp_ind,sNoteParm.ocp_cnt);
WRITEMSG
                /* д�����ݿ���� */
            ret = Note_cheq_mst_Ins(sCheqMst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "��ͻ�ƾ̨֤�˲������ݴ�,���ظ���¼");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N021");
                return(-1);
            }else if (ret)
            {
                sprintf(acErrMsg, "ִ��Note_cheq_mst_Ins��");
                WRITEMSG
                return(-1);
            }
        }
        }else if (ret)
        {
            sprintf(acErrMsg, "ִ��    Note_cheq_mst_Fet_Upd��!ret= [%d]",ret);
            WRITEMSG
            return(-1);
        }else
        {
            /* �޸ı�־ */
            iNoteCnt = 0;
            if (m == 1)  /* ������¼��һ��ƾ֤��״̬ */
            {
                cSts[0] = sCheqMst.book_sts[lBegNoteNo - lDbBeg];
            }

            for (j = lDbBeg, k = 0; j <= lDbEnd; j++, k++)
            {
                if (j >= lBegNoteNo && j <= lEndNoteNo)
                {
                    sCheqMst.book_sts[k] = cABookSts[0];
                    iNoteCnt++;
                }
            } /* end for */

            /* ���ƾ֤��Ϊ�ǹ�����߽��, ƾ֤������ */
            /*
            if (note_act[0] == 'K' || note_act[0] == 'C')
            {
                sCheqMst.cnt += iNoteCnt;
            }else if (cSts[0] == '1' && (note_act[0] != 'K'||note_act[0] != 'C'))
            {
                sCheqMst.cnt -= iNoteCnt;
            }
            */
            if ((cBBookSts[0] == '5' || cBBookSts[0] == '0') && cABookSts[0] == '1')
            {
                sCheqMst.cnt += iNoteCnt;
            }else if ((cABookSts[0] == '5' || cABookSts[0] == '3' || cABookSts[0] == '4') && cBBookSts[0] == '1')
            {
                sCheqMst.cnt -= iNoteCnt;
            }

            /* �������ƾ֤���������涨����, ��ռ�ñ��� */
            if (sCheqMst.cnt >= sNoteParm.ocp_cnt)
            {
                sCheqMst.grp_ind[0] = 'Y';
            }else
            {
                sCheqMst.grp_ind[0] = 'N';
            }
            sCheqMst.sell_date = tx_date;
sprintf(acErrMsg, "2222222222sCheqMst.grp_ind=[%s],sNoteParm.ocp_cnt=[%d]",\
    sCheqMst.grp_ind,sNoteParm.ocp_cnt);
WRITEMSG
            /* �޸����� */
            ret = Note_cheq_mst_Upd_Upd(sCheqMst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "�޸Ŀͻ�ƾ̨֤�����ݴ�,���ظ���¼");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N021");
                return(-1);
            }else if (ret)
            {
                sprintf(acErrMsg, "ִ��Note_cheq_mst_Upd_Upd��");
                WRITEMSG
                return(-1);
            }
        }
        Note_cheq_mst_Clo_Upd();
    }/* end for */

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_RegCheqHst
 * ��������:   �ͻ�ƾ֤����ǼǴ����ͻ�ƾ֤�����ʱ��Ҫ�Ǽ�note_cheq_hst
 * ����/ʱ��:  jack/2004��12��26��
 *
 * ����:
 *     ����:
 *            char      *note_act      ƾ֤��Ϊ
 *            struct note_cheq_mst_c cheq_mst �ͻ�ƾ̨֤��
 *            char    *tw_br_no    �Է�����
 *          char    *tw_tel        �Է�����Ա
 *
 *     ���:  char     *g_pub_tx.reply         ��Ӧ��
 *
 *   ����ֵ:   0: �Ǽǳɹ� -1: �Ǽ�ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_RegCheqHst(note_act, cheq_mst, tw_br_no, tw_tel)
    char   *note_act;
    struct note_cheq_mst_c   cheq_mst;
    char   *tw_br_no;
    char   *tw_tel;
{
    struct note_cheq_hst_c sCheqHst;
    int    ret;

    MEMSET_DEBUG(&sCheqHst, 0x00, sizeof(struct note_cheq_hst_c));

    sCheqHst.ac_id = cheq_mst.ac_id;                           /* �˻�id */
    sCheqHst.ac_seq = cheq_mst.ac_seq;                /* �˻���� */
    strcpy(sCheqHst.note_type, cheq_mst.note_type);        /* ƾ֤���� */
    strcpy(sCheqHst.tx_br_no, g_pub_tx.tx_br_no);        /* �������� */
    strcpy(sCheqHst.beg_note_no, cheq_mst.beg_note_no);    /* ��ʼ���� */
    strcpy(sCheqHst.end_note_no, cheq_mst.end_note_no);    /* ��ֹ���� */
    strcpy(sCheqHst.use_ind, note_act);            /* ʹ�ñ�־ */
    sCheqHst.cnt = cheq_mst.cnt;                /* ƾ֤���� */
    sCheqHst.tx_date = g_pub_tx.tx_date;            /* �������� */
    sCheqHst.tx_time = g_pub_tx.tx_time;            /* ����ʱ�� */
    sCheqHst.trace_no = g_pub_tx.trace_no;            /* �� ˮ �� */
    sCheqHst.trace_cnt = g_pub_tx.trace_cnt;            /* ��ˮ�ʴ� */
    strcpy(sCheqHst.tel, g_pub_tx.tel);                /* ������Ա */
    strcpy(sCheqHst.chk, g_pub_tx.chk);            /* ����Ա   */
    strcpy(sCheqHst.auth, g_pub_tx.auth);            /* ��Ȩ��   */

    ret = Note_cheq_hst_Ins(sCheqHst, g_pub_tx.reply);
    if (ret == -239)
    {
        sprintf(acErrMsg,"��ͻ�ƾ֤��ϸ������¼��!���ظ���¼!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"N024");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg,"ִ��Note_cheq_hst_Ins��!ret=[%d]",ret);
        WRITEMSG
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_ChkCheqSell
 * ��������:   ����ƾ֤ʱ���ƾ֤����Ч�ԡ�
 *            1������ƾ֤����ȡƾ֤������
 *            2���ж��Ƿ��������ƾ֤��
 *            3���ж��Ƿ���Գ��۸��ͻ���
 *            4���ж��Ƿ��ǿ��ñ�־��
 *            5���ж��Ƿ���ʹ�ÿͻ����ͣ�
 *            6���ж��Ƿ�ÿ�οɳ��۱���
 *            7���ж��Ƿ񳬹�ÿ�οɳ�������
 *            8���ж��Ƿ��ۼƿɳ��۱���
 *            9���ж��Ƿ��ۼ�ÿ�οɳ�������
 *
 * ����/ʱ��:  jack/2004��12��26��
 *
 * ����:
 *     ����:
 *            long      ac_id           �˺�id
 *      int ac_seqn
 *            char    *note_type    ƾ֤����
 *          char    *beg_note_no    ƾ֤����
 *      char    *end_note_no
 *
 *     ���:  char     *g_pub_tx.reply         ��Ӧ��
 *
 *   ����ֵ:   0: �ɹ� -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_ChkCheqSell(
        long ac_id,
        int ac_seqn,
        char *note_type,
        char *beg_note_no,
        char *end_note_no)
{
    struct note_parm_c sNoteParm;

    int    iGrp;        /* ƾ֤���� */
    int    iCnt;        /* ƾ֤���� */
    int    iGrp_Count;  /* �ۼ�ƾ֤���� */
    long   iCnt_Count;  /* �ۼ�ƾ֤���� */
    long   ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));

    /* �����˺�idȡ�ͻ����� */

    /* ����ƾ֤����ȡƾ֤���� */
    /* 1:�ж��Ƿ���ڸ�ƾ֤���� */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel��![%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 2:����Ƿ���Գ��۸��ͻ� */
    if (sNoteParm.sell_ind[0] == 'N')
    {
        sprintf(acErrMsg, "��ƾ֤���Ͳ��ܳ��۸��ͻ�note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N016");
        return(-1);
    }

    /* 4:�ж��Ƿ��ǿ��ñ�־ */
    if (sNoteParm.use_ind[0] == 'N')
    {
        sprintf(acErrMsg, "��ƾ֤������ֹͣʹ��״̬note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N025");
        return(-1);
    }

    /* ���㱾��ƾ֤���� */
    iCnt = pub_com_CalNoteCnt(note_type, beg_note_no, end_note_no);
    if (iCnt == -1)
    {
        sprintf(acErrMsg, "����ƾ֤������,note_type=[%s],beg_note_no=[%s],\
                end_note_no=[%s]",note_type,beg_note_no, end_note_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N010");
        return(-1);
    }

    sprintf(acErrMsg,"oooooooooooo  iCnt[%d]", iCnt);WRITEMSG

    /* �ж��Ƿ񳬹��涨���� */
    if (iCnt > sNoteParm.buy_cnt)
    {
        sprintf(acErrMsg, "����ƾ֤���������涨����!note_type=[%s],beg_note_no=[%s],\
                end_note_no=[%s]",note_type,beg_note_no, end_note_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N026");
        return(-1);
    }

    /* ���㱾��ƾ֤���� */
    iGrp = pub_com_CalPileCnt(sNoteParm, beg_note_no, end_note_no);
    if (iGrp == -1)
    {
        sprintf(acErrMsg, "����ƾ֤������");
        WRITEMSG
        return(-1);
    }
    /* �ж��Ƿ�ÿ�οɳ��۱��� */
    if (iGrp > sNoteParm.buy_unit)
    {
        sprintf(acErrMsg, "����ƾ֤�������������Ʊ���");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N029");
        return(-1);
    }

    /* �����ۼ�ƾ֤���� */
    iCnt_Count = pub_com_CalAcCnt(ac_id, ac_seqn, note_type);
    if (iCnt_Count == -1)
    {
        sprintf(acErrMsg, "����ƾ֤������,note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N010");
        return(-1);
    }

    sprintf(acErrMsg,"oooooooooooo  iCnt_Count[%l]", iCnt_Count);WRITEMSG

    /* �ж��Ƿ񳬹��ۼ����� */
    if ((iCnt_Count + iCnt) > sNoteParm.cnt_lnt)
    {
        sprintf(acErrMsg, "�ͻ�����ƾ֤�����涨����!note_type=[%s],�ƻ�����[%d]\
                �ۼƳ���[%l]",note_type,iCnt, iCnt_Count);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N032");
        return(-1);
    }

    /* �����ۼ�ƾ֤���� */
    iGrp_Count = pub_com_CalAcGrp(ac_id, ac_seqn, note_type);
    if (iGrp_Count == -1)
    {
        sprintf(acErrMsg, "����ƾ֤������,note_type=[%s]",note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N010");
        return(-1);
    }

    /* �ж��Ƿ񳬹��ۼƱ��� */
    if ((iGrp_Count + iGrp) > sNoteParm.unit_lmt)
    {
        sprintf(acErrMsg, "�ͻ�����ƾ֤�����涨����!note_type=[%s],�ƻ�����[%d]\
                �ۼƳ���[%l]",note_type,iGrp, iGrp_Count);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N033");
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_CalPileCnt
 * ��������:   ����ƾ֤��ʼ���롢��ֹ����ȷ��ƾ֤����
 *
 * ����/ʱ��:  jack/2004��12��26��
 *
 * ����:
 *     ����:
 *            struct note_parm_c note_parm ƾ֤����
 *            char    *beg_note_no    ��ʼ����
 *            char    *end_note_no    ��ֹ����
 *
 *     ���:
 *
 *   ����ֵ:   >0: �ɹ�(ƾ֤����) -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_CalPileCnt(
        struct note_parm_c note_parm,
        char    *beg_note_no,
        char    *end_note_no)
{
    char    cBegHead[17], cEndHead[17];    /* ��ʼ���롢��ֹ����Ĺ���ͷ */

    long    lBegNo, lEndNo;
    long    lBeg, lEnd;

    int    iPile = 0;    /* ���� */
    int    ret;

    /* ȡ����ͷ */
    strncpy(cBegHead, beg_note_no, note_parm.head_len);
    strncpy(cEndHead, end_note_no, note_parm.head_len);
    cBegHead[note_parm.head_len] = '\0';
    cEndHead[note_parm.head_len] = '\0';

    if (strcmp(cBegHead, cEndHead))
    {
        sprintf(acErrMsg, "����ƾ֤����Ĺ���ͷ��ͬ");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N027");
        return(-1);
    }
    /* ȥ������ͷ����ʼ���롢��ֹ���� */
    lBegNo = atol(beg_note_no + note_parm.head_len);
    lEndNo = atol(end_note_no + note_parm.head_len);

    if (lBegNo > lEndNo)
    {
        sprintf(acErrMsg, "��ʼ���������ֹ����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N028");
        return(-1);
    }

    /* �����ܵ���ʼ����ֹ���� */
    lBeg = (lBegNo - 1)/note_parm.unit_cnt * note_parm.unit_cnt +1;
    lEnd = ((lEndNo - 1)/note_parm.unit_cnt + 1) * note_parm.unit_cnt;

    /* ���㱾�� */
    iPile = (lEnd - lBeg + 1) / note_parm.unit_cnt;

    return iPile ;
}

/**********************************************************************
 * �� �� ��:   pub_com_CalAcGrp
 * ��������:   �����˻�id��ӵ�е�ƾ֤����
 *
 * ����/ʱ��:  jack/2004��12��28��
 *
 * ����:
 *     ����:
 *            long    ac_id;       �˻�id
 *      int ac_seqn
 *            char    *note_type    ƾ֤����
 *
 *     ���:
 *
 *   ����ֵ:   >0: �ɹ�(ƾ֤����) -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_CalAcGrp(
        long    ac_id,
        int ac_seqn,
        char    *note_type)
{
    int    ret;

    ret = sql_count("note_cheq_mst", "ac_id = %ld and ac_seq = %d \
            and note_type = '%s' and grp_ind = '%s' and cnt > 0", ac_id, ac_seqn, note_type, "Y");
    if (ret < 0)
    {
        sprintf(acErrMsg, "�����˺�ƾ֤������ac_id=[%ld],note_type=[%s]", \
                 ac_id, note_type);
        WRITEMSG
        return(-1);
    }

    return ret;
}

/**********************************************************************
 * �� �� ��:   pub_com_CalAcCnt
 * ��������:   �����˻�id��ӵ�е�ƾ֤��������
 *
 * ����/ʱ��:  jack/2004��12��28��
 *
 * ����:
 *     ����:
 *            long    ac_id;       �˻�id
 *            char    *note_type    ƾ֤����
 *
 *     ���:
 *
 *   ����ֵ:   >0: �ɹ�(ƾ֤����) -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_CalAcCnt(
        long    ac_id,
        int ac_seqn,
        char    *note_type)
{
    long    lSumCnt;
    int    ret;

    ret = sql_sum_long("note_cheq_mst", "cnt", &lSumCnt, "ac_id= %ld \
                    and ac_seq = %d and note_type = '%s'", \
                    ac_id, ac_seqn, note_type);
    if (ret == -1)
    {
        sprintf(acErrMsg, "��Ϳ���ƾ֤������!ac_id=[%ld],note_type=[%s]",\
                 ac_id, note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N010");
        return(-1);
    }

    return lSumCnt;
}

/**********************************************************************
 * �� �� ��:   pub_com_ChkPile
 * ��������:   ����ƾ֤��ʼ���롢��ֹ����ȷ���Ƿ�������ƾ֤
 *
 * ����/ʱ��:  jack/2003��1��11��
 *
 * ����:
 *     ����:
 *        char    *note_type      ƾ֤����
 *            char    *beg_note_no    ��ʼ����
 *            char    *end_note_no    ��ֹ����
 *
 *     ���:
 *
 *   ����ֵ:   0: �� -1: ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_ChkPile(
    char    *note_type,
        char    *beg_note_no,
        char    *end_note_no)
{
    struct note_parm_c sNoteParm;

    char    cBegHead[17], cEndHead[17];    /* ��ʼ���롢��ֹ����Ĺ���ͷ */

    long    lBegNo, lEndNo;
    long    lBeg, lEnd;

    int    iPile = 0;    /* ���� */
    int    ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cBegHead, 0x00, sizeof(cBegHead));
    MEMSET_DEBUG(cEndHead, 0x00, sizeof(cEndHead));

    /* ����ƾ֤����ȡƾ֤���� */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel��![%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* ȡ����ͷ */
    strncpy(cBegHead, beg_note_no, sNoteParm.head_len);
    strncpy(cEndHead, end_note_no, sNoteParm.head_len);
    cBegHead[sNoteParm.head_len] = '\0';
    cEndHead[sNoteParm.head_len] = '\0';

    /* �ж��������ʼ�������ֹ�������ͷ�Ƿ���ͬ */
    if (strcmp(cBegHead, cEndHead))
    {
        sprintf(acErrMsg, "����ƾ֤����Ĺ���ͷ��ͬ");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N027");
        return(-1);
    }

    /* ȥ������ͷ����ʼ���롢��ֹ���� */
    lBegNo = atol(beg_note_no + sNoteParm.head_len);
    lEndNo = atol(end_note_no + sNoteParm.head_len);

    /* �����ʼ���������ֹ����, ���������� */
    if (lBegNo > lEndNo)
    {
        sprintf(acErrMsg, "��ʼ���������ֹ����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N028");
        return(-1);
    }

    if (((lBegNo % sNoteParm.unit_cnt) != 1)
        || ((lEndNo % sNoteParm.unit_cnt) != 0))
    {
        sprintf(acErrMsg, "������벻������ƾ֤�Ŀ�ʼ�ͽ�������");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N052");
        return(-1);
    }

    return(0);
}
