#include "find_debug.h"
/*************************************************
* �� �� ��:  pubf_com_note.c
* ��������:  ƾ֤��������
*            pub_com_ChkNote    ƾ֤��鴦��
*
* ��    ��:  jack
* �������:  2003��12��16��
*
* �޸ļ�¼:
*   ��    ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_min_no_c.h"
#include "note_parm_c.h"
#include "com_tx_br_rel_c.h"

/*********************************************************************
* �� �� ��:  pub_com_ChkNote
* ��������:  ����ƾ֤��Ϊ�ж�ƾ֤״̬�Ƿ����Ҫ��
*
*       ��Աƾ֤��Ϊ(ǰ״̬/����״̬)
*       1���(��/0)  2�·�(����)(0/2)  3����(����)(2/0)  4ƾ֤ת��ת��(0/2)
*       5ƾ֤ת��ת��(2/0)  6����(0/5)  7�����Ͻ�(0/2)  8�����Ͻ�(6/3)
*       9�����ս�(2/0)  A�����ս�(3/6)  B��ʧ(0/1)  C���(1/0)
*       Dʹ��(0/4)  E����(0/6)  F�����ջ�(5/6)  M ����(6/ *)  N����(4/6)
*
*       �ͻ�ƾ֤��Ϊ(ǰ״̬/����״̬)
*       B��ʧ(1/2)  C���(2/1)  G�ͻ�ʹ��(1/5)  Hע��(1/4)  F�����ջ�(1/3)
*       N����(5/1)
*
*       ��Աƾ֤״̬(ǰ״̬/����״̬)
*       0����  1��ʧ  2��������(����)  3��������(����)  *����
*       5����  4��Աʹ��  6����/���ۻ���
*
*       �ͻ�ƾ֤״̬
*       0��  1����δʹ��  2��ʧ  3����  4����  5ʹ��
*
* ����/ʱ��:  2003��12��16��
*
* ��  ��:
*     ����:  char *fpNoteAct              ƾ֤��Ϊ
*            struct note_mst_c fsNoteMst  ��Աƾ֤
*
*     ���:  char g_pub_tx.reply[]        ��Ӧ��
*
*   ����ֵ:  0: ���ɹ�  1: ���ɹ�
*
* �޸���ʷ:  �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
*
*********************************************************************/

int pub_com_ChkNote(char *fpNoteAct, struct note_mst_c fsNoteMst)
{
    struct note_parm_c      sNoteParm;  /* ƾ֤���� */
    struct note_cheq_mst_c  sCheqMst;   /* �ͻ�ƾ̨֤�� */
    int    num, num1;
    int    i, from, to, qshm, zzhm;     /* qshm: ��ʼ����  zzhm: ��ֹ���� */
    int    ret;
    /*char   cHead[9];                     ����ͷ */
    char   cHead[13];
    char   cPerNoteSts[2];
    char   cNoteSts[2];
    char   cWho[2];

    MEMSET_DEBUG(&sNoteParm,  0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts));
    MEMSET_DEBUG(cNoteSts,    0x00, sizeof(cNoteSts));
    MEMSET_DEBUG(cWho,        0x00, sizeof(cWho));

    num = 0;
    num1= 0;

    /* ����ƾ֤����ȡƾ֤�ṹ */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", fsNoteMst.note_type);
    if(ret == 100)
    {
        sprintf(acErrMsg, "ȡƾ֤����note_parm���! note_type=[%s]", fsNoteMst.note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel����! ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }

    strncpy(cHead, fsNoteMst.beg_note_no, sNoteParm.head_len);
    cHead[sNoteParm.head_len]='\0';

    qshm = atoi(fsNoteMst.beg_note_no + sNoteParm.head_len);
    zzhm = atoi(fsNoteMst.end_note_no + sNoteParm.head_len);

    qshm = qshm%sNoteParm.unit_cnt;
    zzhm = zzhm%sNoteParm.unit_cnt;

    if(zzhm == 0) zzhm=sNoteParm.unit_cnt;

    /**** ���ö�ƾ֤�Ƿ���� ���Ϊ��ⲻӦ����, ����Ӧ���� ****/
    /**** ����ͻ�ƾ֤����, �����ȫ��������״̬(*), ��������� ***/
    if (fpNoteAct[0] == '1')  /* ��� */
    {
        /* �����ʼ������� ������ֹ�������, ������ʼ������ֹ���붼���� */
        /* ����Աƾ̨֤���Ƿ����ظ���ƾ֤���� */
        /* ���״̬Ϊ����״̬3, ��������, ���򶼲�������� */
        /* ���״̬Ϊ����״̬6, ��������, ���򶼲�������� */
        num = sql_count("note_mst", "note_type = '%s' \
                 and ((beg_note_no <= '%s' and end_note_no >= '%s') \
                 or (beg_note_no <= '%s' and end_note_no >= '%s') \
                 or (beg_note_no >= '%s' and end_note_no <= '%s')) \
                 and sts != '3' and sts != '6'", \
                 fsNoteMst.note_type, \
                 fsNoteMst.beg_note_no, fsNoteMst.beg_note_no, \
                 fsNoteMst.end_note_no, fsNoteMst.end_note_no, \
                 fsNoteMst.beg_note_no, fsNoteMst.end_note_no);

        if (num > 0)
        {
            sprintf(acErrMsg, "��Աƾ̨֤�������ظ���ƾ֤����");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N006");
            return(-1);
        }

        /* ���ͻ�ƾ̨֤���Ƿ����ظ���ƾ֤���� */
        num = sql_count("note_cheq_mst", "note_type = '%s' \
                  and ((beg_note_no <= '%s' and end_note_no >= '%s') \
                  or (beg_note_no <= '%s' and end_note_no >= '%s') \
                  or (beg_note_no >= '%s' and end_note_no <= '%s'))", \
                          fsNoteMst.note_type, \
                          fsNoteMst.beg_note_no, fsNoteMst.beg_note_no, \
                          fsNoteMst.end_note_no, fsNoteMst.end_note_no, \
                          fsNoteMst.beg_note_no, fsNoteMst.end_note_no);

        if (num > 0) /* ������� */
        {
            ret = Note_cheq_mst_Dec_Sel(g_pub_tx.reply, "note_type = '%s' \
                         and substr(beg_note_no, 1, %d)='%s' and end_note_no >= '%s' \
                         order by beg_note_no", fsNoteMst.note_type, \
                         sNoteParm.head_len, cHead, fsNoteMst.beg_note_no);
            if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_cheq_mst_Dec_Sel��!ret=[%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D101");
                return(-1);
            }
            while(1)
            {
                ret = Note_cheq_mst_Fet_Sel(&sCheqMst, g_pub_tx.reply);
                if(ret == 100)
                {
                    break;
                }else if(ret)
                {
                    sprintf(acErrMsg, "ִ��Note_cheq_mst_Fet_Sel��!ret=[%d]", ret);
                    WRITEMSG;
                    strcpy(g_pub_tx.reply, "D101");
                    return(-1);
                }
                if(strcmp(sCheqMst.beg_note_no , fsNoteMst.end_note_no) > 0) break;

                from = atoi(sCheqMst.beg_note_no + sNoteParm.head_len);
                  to = atoi(sCheqMst.end_note_no + sNoteParm.head_len);
                from = from%sNoteParm.unit_cnt;
                  to = to%sNoteParm.unit_cnt;
                if(from == 0) from = sNoteParm.unit_cnt;
                if(to == 0) to = sNoteParm.unit_cnt;

                (strcmp(sCheqMst.beg_note_no, fsNoteMst.beg_note_no)<=0) ? (from=qshm-1):(from=from-1);
                (strcmp(sCheqMst.end_note_no, fsNoteMst.end_note_no)<=0) ? (to = to-1):(to = zzhm-1);

                if(from < 0) from = 0;

                for(i=from;i<=to;i++)
                {
                    if(sCheqMst.book_sts[i] !='0' && sCheqMst.book_sts[i] !='5')
                    {
                        sprintf(acErrMsg, "����δʹ�õ�֧Ʊ[%s][%s][%s][%d]", \
                            sCheqMst.beg_note_no, sCheqMst.end_note_no, sCheqMst.book_sts, i);
                        WRITEMSG
                        strcpy(g_pub_tx.reply, "N037");
                        Note_cheq_mst_Clo_Sel();
                        return(-1);
                    }
                } /* end for */

                if(strcmp(sCheqMst.end_note_no , fsNoteMst.end_note_no) >= 0)
                {
                    break;
                }
            } /* end while */
            Note_cheq_mst_Clo_Sel();
        }
    }else  /* �������� */
    {
        /* ����ƾ֤��Ϊ��ƾִ֤���������״̬��ִ�к��״̬ */
        strcpy(cWho, "T");

        ret = pub_com_NoteActSts(cWho, fpNoteAct, cNoteSts, cPerNoteSts);
        if (ret)
        {
            sprintf(acErrMsg, "����ƾ֤��Ϊȡƾ֤״̬��!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N002");
            return(-1);
        }

        num1 = sql_count("note_mst", "tel = '%s' and note_type = '%s' \
            and beg_note_no <= '%s' and end_note_no >= '%s' \
            and sts = '%s'", fsNoteMst.tel, fsNoteMst.note_type, \
            fsNoteMst.beg_note_no, fsNoteMst.end_note_no, cPerNoteSts);

        ret = sql_count("note_mst", "tel = '%s' and note_type = '%s' \
            and beg_note_no <= '%s' and end_note_no >= '%s'", \
            fsNoteMst.tel, fsNoteMst.note_type, \
            fsNoteMst.beg_note_no, fsNoteMst.end_note_no);

       if (num1 == 0 && ret > 0)
       {
            if (ret == 0)
            {
                sprintf(acErrMsg, "����ƾ֤������[%s][%s][%s][%s]", \
                    fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                    fsNoteMst.end_note_no, fsNoteMst.tel);
                WRITEMSG
                strcpy(g_pub_tx.reply, "N040");
                return (-1);
            }else
            {
                if(fsNoteMst.sts[0] == '1')
                {
                    sprintf(acErrMsg, "����ƾ֤�ѹ�ʧ[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '2')
                {
                    sprintf(acErrMsg, "����ƾ֤Ϊ��������(����)״̬[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '3')
                {
                    sprintf(acErrMsg, "����ƾ֤Ϊ��������(����)״̬[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '4')
                {
                    sprintf(acErrMsg, "����ƾ֤Ϊ��Աʹ��״̬[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '5')
                {
                    sprintf(acErrMsg, "����ƾ֤�ѳ���[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '6')
                {
                    sprintf(acErrMsg, "����ƾ֤�����ϻ���ۻ���[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '*')
                {
                    sprintf(acErrMsg, "����ƾ֤������[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }
            }
        }
    } /* end if */

    if (num<0 || num1<0)
    {
        sprintf(acErrMsg, "��ѯ note_mst ����![%s] [%s] [%s] [%s]", \
                fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                fsNoteMst.end_note_no, fsNoteMst.tel);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N038");
        return(-1);
    }

    return(0);
}

/**********************************************************************
* �� �� ��:  pub_com_CalNoteCnt
* ��������:  ����ƾ֤����������ƾ֤��ʼ����ֹ�����������
* ����/ʱ��: 2003��12��22��
*
* ��    ��:
*     ����:
*            char note_type[4]  ƾ֤����
*            char beg_no[17]    ��ʼ����
*            char end_no[17]    ��ֹ����
*
*     ���:
*            char g_pub_tx.reply[]   ��Ӧ��
*
*   ����ֵ:  >0: ƾ֤���� <=0: ���ɹ�
*
* �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
*
********************************************************************/

int pub_com_CalNoteCnt(
        char *note_type,
        char *beg_note_no,
        char *end_note_no)
{
    struct note_parm_c    note_parm;    /* ƾ֤���� */
    char   cBegHead[17], cEndHead[17];  /* ����ͷ */
    long   lBegNo, lEndNo;
    int    iNoteCnt;                    /* ƾ֤���� */
    int    ret;

    MEMSET_DEBUG(&note_parm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cBegHead,   0x00, sizeof(cBegHead));
    MEMSET_DEBUG(cEndHead,   0x00, sizeof(cEndHead));

    /* ����ƾ֤����ȡƾ֤�ṹ */
    ret = Note_parm_Sel(g_pub_tx.reply, &note_parm , "note_type = '%s'" , note_type);
    if(ret == 100)
    {
        sprintf(acErrMsg, "ȡƾ֤����note_parm���!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }
    else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }

    /* ȡ����ͷ */
    strncpy(cBegHead, beg_note_no, note_parm.head_len);
    strncpy(cEndHead, end_note_no, note_parm.head_len);
    cBegHead[note_parm.head_len] = '\0';
    cEndHead[note_parm.head_len] = '\0';
    /* �ȽϹ���ͷ */
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
    /* �ȽϺ����С */
    if (lBegNo > lEndNo)
    {
        sprintf(acErrMsg, "��ʼ���������ֹ����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N028");
        return(-1);
    }

    iNoteCnt = lEndNo - lBegNo + 1;

    return iNoteCnt;
}

/**********************************************************************
* �� �� ��:   pub_com_NoteActSts
* ��������:   ����ƾ֤��Ϊ���ض�Ӧƾ֤״̬
*     ��Աƾ֤��Ϊ(ǰ״̬/����״̬)
*      1���(��/0)  2�·�(����)(0/2)  3����(����)(2/0)  4ƾ֤ת��ת��(0/2)
*      5ƾ֤ת��ת��(2/0)  6����(0/5)  7�����Ͻ�(0/2)  8�������Ͻ�(6/3)
*      9�����ս�(2/0)  A�����ս�(3/6)  B��ʧ(0/1)  C���(1/0)
*      Dʹ��(0/4)  E����(0/6)  F�����ջ�(5/6)  M ����(6/ *) N���� (4/6)
*
*      �ͻ�ƾ֤��Ϊ
*      K����(0/1) B��ʧ(1/2) C���(2/1) G�ͻ�ʹ��(1/5) Hע��(1/4)
*      F�����ջ�(1/3) N����(5/1)
*
*          ��Աƾ֤״̬
*      0����  1��ʧ  2��������(����)3��������(����)  *����
*      5����  4��Աʹ��  6����/���ۻ���
*
*      �ͻ�ƾ֤״̬
*      0��  1����δʹ��  2��ʧ  3����  4����  5ʹ��
*
* ����/ʱ��:  2003��12��16��
*
* ��    ��:
*     ����:
*            char *who             ʹ����
*        char *note_act        ƾ֤��Ϊ
*
*     ���:
*        char *note_sts        ƾ֤״̬
*        char *p_note_sts      ƾ֤ʹ��ǰ״̬
*            char *g_pub_tx.reply      ��Ӧ��
*
*   ����ֵ:   0: �ɹ� 1: ���ɹ�
*
* �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
*
********************************************************************/
int pub_com_NoteActSts(
    char *who,
    char *note_act,
    char *note_sts,
    char *p_note_sts)
{
    if (who[0] == 'T') /* ��Ա��Ϊ */
    {
        switch(note_act[0])
        {
            case '1':   /*1���(��/0)*/
                p_note_sts[0] = '\0';
                note_sts[0] = '0';
                break;
            case '2':   /*2�·�(0/2)*/
                p_note_sts[0] = '0';
                note_sts[0] = '2';
                break;
            case '3':   /*3����(2/0)*/
                p_note_sts[0] = '2';
                note_sts[0] = '0';
                break;
            case '4':   /*4ƾ֤ת��ת��(0/2)*/
                p_note_sts[0] = '0';
                note_sts[0] = '2';
                break;
            case '5':   /*5ƾ֤ת��ת��(2/0)*/
                p_note_sts[0] = '2';
                note_sts[0] = '0';
                break;
            case '6':   /*6����(0/5)*/
                p_note_sts[0] = '0';
                note_sts[0] = '5';
                break;
            case '7':   /*7�����Ͻ�(0/2)*/
                p_note_sts[0] = '0';
                note_sts[0] = '2';
                break;
            case '8':   /*  8�������Ͻ�(6/3)  */
                p_note_sts[0] = '6';
                note_sts[0] = '3';
                break;
            case '9':   /*9�����ս�(2/0)*/
                p_note_sts[0] = '2';
                note_sts[0] = '0';
                break;
            case 'A':   /*  A�����ս�(3/6)*/
                p_note_sts[0] = '3';
                note_sts[0] = '6';
                break;
            case 'B':   /*  B��ʧ(0/1) */
                p_note_sts[0] = '0';
                note_sts[0] = '1';
                break;
            case 'C':   /* C���(1/0)  */
                p_note_sts[0] = '1';
                note_sts[0] = '0';
                break;
            case 'D':   /*Dʹ��(0/4)*/
                p_note_sts[0] = '0';
                note_sts[0] = '4';
                break;
            case 'e':   /*  E����(4/6)ʹ�� */
                p_note_sts[0] = '4';
                note_sts[0] = '6';
                break;
            case 'E':   /*  E����(0/6)*/
                p_note_sts[0] = '0';
                note_sts[0] = '6';
                break;
            case 'F':   /*  F�����ջ�(5/6)*/
                p_note_sts[0] = '5';
                note_sts[0] = '6';
                break;
            case 'M':   /*  M����(6/ *)*/
                p_note_sts[0] = '6';
                note_sts[0] = '*';
                break;
            case 'N':   /*  N����(ʹ�ó���)*/
                p_note_sts[0] = '4';
                note_sts[0] = '6';
                break;
            default:
                sprintf(acErrMsg, "��Աƾ֤��Ϊ��������Χ, ƾ֤��Ϊ=[%s]", note_act);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N009");
                    return(-1);
        }
    }else if (who[0] == 'C')  /* �ͻ�ƾ֤��Ϊ */
    {
        switch(note_act[0])
        {
            case 'K':   /* K����(0/1) */
                p_note_sts[0] = '0';
                note_sts[0] = '1';
                break;
            case 'B':   /*B��ʧ(1/2)*/
                p_note_sts[0] = '1';
                note_sts[0] = '2';
                break;
            case 'C':   /*C���(2/1)*/
                p_note_sts[0] = '2';
                note_sts[0] = '1';
                break;
            case 'G':   /*G�ͻ�ʹ��(1/5)*/
                p_note_sts[0] = '1';
                note_sts[0] = '5';
                break;
            case 'H':   /*Hע��(1/4)*/
                p_note_sts[0] = '1';
                note_sts[0] = '4';
                break;
            case 'F':   /*  F�����ջ�(1/3)*/
                p_note_sts[0] = '1';
                note_sts[0] = '3';
                break;
            case 'N':   /*  N����(ʹ�ó���)*/
                p_note_sts[0] = '5';
                note_sts[0] = '1';
                break;
            default:
                sprintf(acErrMsg, "�ͻ�ƾ֤��Ϊ��������Χ, ƾ֤��Ϊ=[%s]", note_act);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N009");
                    return(-1);
        }

    }else  /* ƾ֤��Ϊ�߲������� */
    {
        sprintf(acErrMsg, "ƾ֤��Ϊ�߲�������, who =[%s]", who);
            WRITEMSG
            return(-1);
    }

    p_note_sts[1] = '\0';
    note_sts[1] = '\0';

        return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_NoteApart
 * ��������:   ƾ֤�ֶδ�����ԭ������һ������ε�ƾ֤���մ������ʼ
 *         �������ֹ����ֿ�, ���������ʼ���뵽������ʼ����ǰ��
 *         һ�δ�����ʼ���뵽��ֹ������һ�Ρ�������ֹ����󵽸���
 *         ����ֹ����Ϊһ�Ρ��������ʼ������ͬ����ֹ������ͬ��
 *         ͬ���ܷ����λ�һ�Ρ�
 *             �ֶκ���ܴ��ڲ������(����״̬��ͬ�ĺ�������), ����Ҫ���в��δ���
 *
 * ����/ʱ��:  jack/2003��12��16��
 *
 * ����:
 *     ����:
 *            char   *note_act             ƾ֤��Ϊ
 *            struct note_mst_c note_mst   ��Աƾ̨֤��
 *
 *     ���:   char g_pub_tx.reply ��Ӧ��
 *
 *   ����ֵ:   0: ���γɹ� -1: ���ɹ�
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_NoteApart(
         char *note_act,
         struct note_mst_c note_mst)
{
    struct note_mst_c sNoteMst;  /* �ӹ�Աƾ̨֤��ȡ����ֵ */
    struct note_mst_c sNoteMstTmp; /* ��Ź�Աƾ̨֤�ʵ���ʱ���� */
    char   cNoteSts[2];          /* ƾ֤״̬ */
    char   cPerNoteSts[2];  /* ��Ϊִ�е�״̬ */
    char   cWho[2];     /* ƾ֤��Ϊִ���� */
    char   str_tmp[32];
    int    ret;

    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(cNoteSts, 0x00, sizeof(cNoteSts));
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts));
    MEMSET_DEBUG(cWho, 0x00, sizeof(cWho));

    strcpy(cWho, "T");   /* T��ʾ��Ա */

    /* ����ƾ֤��Ϊ�ҳ�ƾ֤״̬ */
    ret = pub_com_NoteActSts(cWho, note_act, cNoteSts, cPerNoteSts);
    if (ret)
    {
        sprintf(acErrMsg, "����ƾ֤��Ϊȡƾ֤״̬��!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N002");
        return(-1);
    }

    strcpy(note_mst.sts, cNoteSts);   /* ƾ֤��Ϊִ�к�ƾ֤��״̬ */

    /* ����Ƿ�������������� */
    vtcp_log("tel = '%s' and note_type = '%s' and beg_note_no <= '%s' and end_note_no >= '%s' and sts = '%s'",
        note_mst.tel, note_mst.note_type, note_mst.beg_note_no, note_mst.end_note_no, cPerNoteSts);
    /* ��ԭƾ֤״̬�Ĳ��� */
    ret = Note_mst_Dec_Upd(g_pub_tx.reply, "tel = '%s' and note_type = '%s' \
                      and beg_note_no <= '%s' and end_note_no >= '%s' \
                      and sts = '%s'", note_mst.tel, note_mst.note_type, \
                      note_mst.beg_note_no, note_mst.end_note_no, cPerNoteSts);
    if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_mst_Dec_Upd��!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_mst_Fet_Upd(&sNoteMst, g_pub_tx.reply);

    if (ret == 100)
    {
        sprintf(acErrMsg, "������������ƾ֤����!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N003");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_mst_Fet_Upd����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }else                 /* 0 �ҵ����������ļ�¼ */
    {
        /* ���ͷ�Ƿ���ͬ */
        if(!strcmp(sNoteMst.beg_note_no, note_mst.beg_note_no)) /* ͷ��ͬ */
        {
            if(!strcmp(sNoteMst.end_note_no, note_mst.end_note_no)) /* β��ͬ */
            {
                /* �޸�ƾ֤״̬ */
                strcpy(sNoteMst.sts , note_mst.sts);

                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��, �����ظ���¼!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "ִ��Note_mst_Upd_Upd��!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
                /**********************************************
                  �����δ���:
                           ����Ƿ�������޸ĺ�ͷβ�����ĺ����
                           ��Ҫ���ͷβ�Ƿ�ͬʱ��ͬ��, ��sNoteMst����note_mst
                **********************************************/
            ret = pub_com_NoteApartMerg(note_mst);
            if (ret == -1)
            {
                sprintf(acErrMsg, "ƾ֤����ʧ��");
                WRITEMSG
                return(-1);
            }
            }else  /* β��ͬ */
            {
                /* 1: ����һ���¼�¼*/
sprintf(acErrMsg, "����һ���¼�¼ beg_no=[%s], end_no=[%s]", note_mst.beg_note_no, note_mst.end_note_no);
                WRITEMSG
                ret = Note_mst_Ins(note_mst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_mst_Ins��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
                /*********************************************
                  �����δ���:
                           ����Ƿ�������޸ĺ�ͷβ�����ĺ����
                           ��Ҫ���ͷ�Ƿ�ͬʱ��ͬ��, ��note_mst
                **********************************************/
            ret = pub_com_NoteApartMerg(note_mst);
            if (ret == -1)
            {
                sprintf(acErrMsg, "ƾ֤����ʧ��");
                WRITEMSG
                return(-1);
            }

                /* 2: �޸�ԭ��¼����ʼ���롢״̬������ */
                ret = pub_com_CalNoteNo(note_mst.note_type, \
                          note_mst.end_note_no, str_tmp, 1);
                if (ret == -1)
                {
                    sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }

                strcpy(sNoteMst.beg_note_no, str_tmp);

                /* ����ƾ֤���� */
                sNoteMst.cnt -=  note_mst.cnt;

                /* �޸�ԭƾ֤ */
                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��, ���ظ���¼!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "ִ��Note_mst_Upd_Upd��!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
            }
        }else /* ͷ��ͬ */
        {
            if(!strcmp(sNoteMst.end_note_no, note_mst.end_note_no)) /* β��ͬ */
            {
                /* 1: �����µ����� */
                ret = Note_mst_Ins(note_mst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_mst_Ins��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
                /**********************************************
                  �����δ���:
                           ����Ƿ�������޸ĺ�ͷβ�����ĺ����
                           ��Ҫ���β�Ƿ�ͬʱ��ͬ��, ��note_mst
                **********************************************/
            ret = pub_com_NoteApartMerg(note_mst);
            if (ret == -1)
            {
                sprintf(acErrMsg, "ƾ֤����ʧ��");
                WRITEMSG
                return(-1);
            }

                /* 2: �޸�ԭ���� */
                /* ����ƾ֤��ֹ���� */
                ret = pub_com_CalNoteNo(note_mst.note_type, \
                          note_mst.beg_note_no, str_tmp, -1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }
                strcpy(sNoteMst.end_note_no, str_tmp);

                /* ����ƾ֤���� */
                sNoteMst.cnt = sNoteMst.cnt - note_mst.cnt;

                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                    sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��, ���ظ���¼!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "ִ��Note_mst_Upd_Upd��!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
            }else   /* β��ͬ */
            {
                sNoteMstTmp = sNoteMst;

                /* �޸�ԭ����ƾ֤, �ֳ����� */
                /* 1: ����ƾ֤��ֹ���� */
                ret = pub_com_CalNoteNo(sNoteMst.note_type, note_mst.beg_note_no, \
                                      sNoteMst.end_note_no, -1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }

                ret = pub_com_CalNoteCnt(note_mst.note_type, \
                        sNoteMst.beg_note_no, sNoteMst.end_note_no);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤������!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }
                sNoteMst.cnt = ret;

                /* �޸Ĺ�Աƾ֤ */
                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "ִ��Note_mst_Upd_Upd��!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }

                /* 2: �����µ� */
                /* ����һ���¼�¼*/
                ret = Note_mst_Ins(note_mst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_mst_Ins��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }

                /* 3: ������һ�� */
                /* ����ƾ֤��ʼ���� */
                ret = pub_com_CalNoteNo(sNoteMstTmp.note_type, \
                          note_mst.end_note_no, sNoteMstTmp.beg_note_no, 1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }
                /* ����ƾ֤���� */
                ret = pub_com_CalNoteCnt(sNoteMstTmp.note_type, \
                        sNoteMstTmp.beg_note_no, sNoteMstTmp.end_note_no);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }
                sNoteMstTmp.cnt = ret;
                /* ����һ���¼�¼*/
                ret = Note_mst_Ins(sNoteMstTmp, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_mst_Ins��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
            }
        }
    }

    /* �ر��α� */
    Note_mst_Clo_Upd();

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_NoteApartMerg
 * ��������:   ƾ֤����:�ֶι������õ��Ĳ��κ���
 * ����/ʱ��:  Terry.cui/2003��12��30��
 *
 * ��    ��:
 *     ����:
 *          struct note_mst_c note_mst;
 *
 *     ���:
 *            char *g_pub_tx.reply               ��Ӧ��
 *
 *   ����ֵ:  0:   �ɹ�
 *            ��0: ���ɹ�
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_NoteApartMerg(note_mst)
    struct note_mst_c note_mst;
{
    struct note_mst_c  note_mst1, note_mst2;        /* ��Աƾ̨֤�ʽṹ */
    char   beg_no[17];                     /* ��ʼƾ֤�� */
    char   end_no[17];                             /* ��ֹƾ֤�� */
    char   cFilter[512];                           /* ɾ�����where���� */
    int    cnt;                                    /* ƾ֤����   */
    int    ret;

    MEMSET_DEBUG(&note_mst1, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(&note_mst2, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(beg_no, 0x00, sizeof(beg_no));
    MEMSET_DEBUG(end_no, 0x00, sizeof(end_no));
    MEMSET_DEBUG(cFilter, 0x00, sizeof(cFilter));

    /* ������note_mst���Ƿ��Ѿ�������ʼ�������ֹ�����ص���� */
sprintf(acErrMsg, "note_mst.beg_note_no[%s], note_mst.end_note_no[%s], sts=[%s]", \
         note_mst.beg_note_no, note_mst.end_note_no, note_mst.sts);
WRITEMSG

    ret = sql_count("note_mst", "tel = '%s' and note_type = '%s' \
                     and ((beg_note_no <= '%s' and end_note_no >= '%s') \
                     or (beg_note_no <= '%s' and end_note_no >= '%s')) \
                     and sts = '%s'", note_mst.tel, note_mst.note_type, \
                     note_mst.beg_note_no, note_mst.beg_note_no, \
                     note_mst.end_note_no, note_mst.end_note_no, note_mst.sts);
    if (ret < 0)
    {
        sprintf(acErrMsg, "���ƾ֤�Ƿ��Ѿ����ڴ�");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N035");
        return(-1);
    }else if (ret > 1)
    {
        sprintf(acErrMsg, "����ƾ֤�Ѿ�����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N034");
        return(-1);
    }

    /* ����ƾ֤��ʼ���� */
    ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.beg_note_no, \
                              beg_no, -1);
    if (ret == -1)
    {
        sprintf(acErrMsg, "����ƾ֤�����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N004");
        return(-1);
    }

    /* ����ƾ֤��ֹ���� */
    ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.end_note_no, \
                              end_no, 1);
    if (ret == -1)
    {
        sprintf(acErrMsg, "����ƾ֤�����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N004");
        return(-1);
    }

    /* ���������������Ƿ������ֹ��������������ʼ���������� */
    ret = Note_mst_Sel(g_pub_tx.reply, &note_mst1, "tel='%s' and sts='%s' \
                      and note_type='%s' and end_note_no='%s'", note_mst.tel, \
                      note_mst.sts, note_mst.note_type, beg_no);
    if (ret == 100)  /* 1:������ƾ֤��ֹ������������ʼ����������ƾ֤ */
    {
        /* ���������������Ƿ������ʼ��������������ֹ���������� */
        ret=Note_mst_Sel(g_pub_tx.reply, &note_mst2, "tel = '%s' and \
                     sts = '%s' and note_type='%s' and beg_note_no='%s'", \
                     note_mst.tel, note_mst.sts, note_mst.note_type, end_no);
        /* 3333 if */
        if (ret==100)  /* ������ƾ֤��ʼ������������ֹ���������� */
        {
        return(0);

         }else if (ret)
         {
            sprintf(acErrMsg, "ִ��Note_mst_Sel����!ret=[%d]", ret);
            WRITEMSG
            return(-1);
         }
         else  /* �ڶ������: ���ں�������, ����ƾ֤��ʼ������������ֹ���������� */
         {
             note_mst2.cnt += note_mst.cnt;

             /* ���δ���, ����ʼ�����Ϊ�������ʼ���� */
             ret = sql_execute("update note_mst set beg_note_no = '%s', \
                                cnt = %d where tel = '%s' and sts = '%s' \
                                and note_type = '%s' and beg_note_no = '%s'", \
                                note_mst.beg_note_no, note_mst2.cnt, \
                                note_mst.tel, note_mst.sts, note_mst.note_type, \
                                end_no);
             if (ret)
             {
                 sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "N019");
                 return(-1);
             }
         } /* end 3333 if */
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_mst_Sel����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }else  /* ����ƾ֤��ֹ������������ʼ����������ƾ֤ */
    {
        /* ���������������Ƿ������ʼ��������������ֹ���������� */
        ret=Note_mst_Sel(g_pub_tx.reply, &note_mst2, "tel = '%s' and \
                     sts = '%s' and note_type='%s' and beg_note_no='%s'", \
                     note_mst.tel, note_mst.sts, note_mst.note_type, end_no);
        /* 4444 if */
        if (ret == 100)/*������: ǰ�������, ������ƾ֤��ʼ������������ֹ����������*/
        {
            /* ���δ���, ����ֹ�����Ϊ�������ֹ���� */
                sprintf(acErrMsg, "������: ǰ�������, note_mst.end_note_no=[%s]!", note_mst.end_note_no);
                WRITEMSG
            strcpy(note_mst1.end_note_no, note_mst.end_note_no);
            note_mst1.cnt += note_mst.cnt;
      sprintf(acErrMsg, "������: ǰ�������, note_mst1.end_note_no=[%s]!", note_mst1.end_note_no);
                WRITEMSG
            ret = sql_execute("update note_mst set end_note_no = '%s', \
                               cnt = %d where tel = '%s' and sts = '%s' \
                               and note_type = '%s' and beg_note_no = '%s'", \
                               note_mst1.end_note_no, note_mst1.cnt, \
                               note_mst1.tel, note_mst1.sts, note_mst1.note_type, \
                               note_mst1.beg_note_no);
            if (ret)
            {
                sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N019");
                return(-1);
            }
        }else if (ret)
        {
            sprintf(acErrMsg, "ִ��Note_mst_Dec_Upd����!ret=[%d]", ret);
            WRITEMSG
            return(-1);
        }else
        {
            /* ������: ǰ������ */
            /*�������ƾ֤����β��*/
            strcpy(note_mst1.end_note_no, note_mst2.end_note_no);
            note_mst1.cnt = note_mst1.cnt + note_mst2.cnt + note_mst.cnt;

            /* ���δ���, �������ƾ֤����ԭ��ƾ֤��β���� */
            ret = sql_execute("update note_mst set end_note_no = '%s', \
                               cnt = %d where tel = '%s' and sts = '%s' \
                               and note_type = '%s' and beg_note_no = '%s'", \
                               note_mst1.end_note_no, note_mst1.cnt, \
                               note_mst1.tel, note_mst1.sts, note_mst1.note_type, \
                               note_mst1.beg_note_no);
            if (ret)
            {
                sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N019");
                return(-1);
            }

            /* ɾ����һ�μ�¼note_mst2 */
            sprintf(cFilter, "tel = '%s' and note_type = '%s' \
                     and beg_note_no = '%s' and end_note_no = '%s' \
                     and sts = '%s'", note_mst2.tel, note_mst2.note_type, \
                     note_mst2.beg_note_no, note_mst2.end_note_no, note_mst2.sts);
            ret = deldb("note_mst", cFilter);
            if (ret)
            {
                 sprintf(acErrMsg, "ɾ����Աƾ̨֤�ʴ�");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "N005");
                 return(-1);
            }
        } /** end 4444 if */
    } /* end 1111 if */

    /* ɾ���Ѿ�д������� */
    sprintf(cFilter, "tel = '%s' and note_type = '%s' \
                     and beg_note_no = '%s' and end_note_no = '%s' \
                     and sts = '%s'", note_mst.tel, note_mst.note_type, \
                     note_mst.beg_note_no, note_mst.end_note_no, note_mst.sts);
    ret = deldb("note_mst", cFilter);
    if (ret)
    {
        sprintf(acErrMsg, "ɾ����Աƾ̨֤�ʴ�");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N005");
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_NoteMerg
 * ��������:   ƾ֤����
 * ����/ʱ��:  jack/2003��12��23��
 *
 * ��    ��:
 *     ����:
 *          struct note_mst_c note_mst;
 *
 *     ���:
 *            char *g_pub_tx.reply               ��Ӧ��
 *
 *   ����ֵ:  0:   �ɹ�
 *            ��0: ���ɹ�
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_NoteMerg(note_mst)
    struct note_mst_c note_mst;
{
    struct note_mst_c  note_mst1, note_mst2;        /* ��Աƾ̨֤�ʽṹ */
    char   beg_no[17];                     /* ��ʼƾ֤�� */
    char   end_no[17];                             /* ��ֹƾ֤�� */
    char   cFilter[512];                           /* ɾ�����where���� */
    int    cnt;                                    /* ƾ֤����   */
    int    ret;

    MEMSET_DEBUG(&note_mst1, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(&note_mst2, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(beg_no, 0x00, sizeof(beg_no));
    MEMSET_DEBUG(end_no, 0x00, sizeof(end_no));
    MEMSET_DEBUG(cFilter, 0x00, sizeof(cFilter));

    /* ������note_mst���Ƿ��Ѿ�������ʼ�������ֹ�����ص���� */
sprintf(acErrMsg, "note_mst.beg_note_no[%s], note_mst.end_note_no[%s], sts=[%s]", \
         note_mst.beg_note_no, note_mst.end_note_no, note_mst.sts);
WRITEMSG

    ret = sql_count("note_mst", "tel = '%s' and note_type = '%s' \
                     and ((beg_note_no <= '%s' and end_note_no >= '%s') \
                     or (beg_note_no <= '%s' and end_note_no >= '%s')) \
                     and sts = '%s'", note_mst.tel, note_mst.note_type, \
                     note_mst.beg_note_no, note_mst.beg_note_no, \
                     note_mst.end_note_no, note_mst.end_note_no, note_mst.sts);
    if (ret < 0)
    {
        sprintf(acErrMsg, "���ƾ֤�Ƿ��Ѿ����ڴ�");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N035");
        return(-1);
    }else if (ret > 0)
    {
        sprintf(acErrMsg, "����ƾ֤�Ѿ�����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N034");
        return(-1);
    }

    /* ����ƾ֤��ʼ���� */
    ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.beg_note_no, \
                              beg_no, -1);
    if (ret == -1)
    {
        sprintf(acErrMsg, "����ƾ֤�����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N004");
        return(-1);
    }

    /* ����ƾ֤��ֹ���� */
    ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.end_note_no, \
                              end_no, 1);
    if (ret == -1)
    {
        sprintf(acErrMsg, "����ƾ֤�����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N004");
        return(-1);
    }

    /* ���������������Ƿ������ֹ��������������ʼ���������� */
    ret = Note_mst_Dec_Upd(g_pub_tx.reply, "tel='%s' and sts='%s' and note_type='%s' \
                      and end_note_no='%s'", note_mst.tel, note_mst.sts, \
                      note_mst.note_type, beg_no);
    if (ret)   /* �������α���� */
    {
        sprintf(acErrMsg, "ִ��Note_mst_Dec_Upd����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_mst_Fet_Upd(&note_mst1, g_pub_tx.reply);
    /* 1111 if */
    if (ret == 100)  /* 1:������ƾ֤��ֹ������������ʼ����������ƾ֤ */
    {
        /* ���������������Ƿ������ʼ��������������ֹ���������� */
        ret=Note_mst_Sel(g_pub_tx.reply, &note_mst2, "tel = '%s' and \
                     sts = '%s' and note_type='%s' and beg_note_no='%s'", \
                     note_mst.tel, note_mst.sts, note_mst.note_type, end_no);
        /* 3333 if */
        if (ret==100)  /* ������ƾ֤��ʼ������������ֹ���������� */
        {
            /* ��һ�����: ֱ�Ӳ����¼�¼, �������ƾֱ֤�Ӳ������ */
            ret=Note_mst_Ins(note_mst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N020");
            return(-1);
             }else if(ret)
             {
             printf(acErrMsg, "ִ��Note_mst_Ins��!ret=[%d]", ret);
             WRITEMSG
             return(-1);
             }

         }else if (ret)
         {
            sprintf(acErrMsg, "ִ��Note_mst_Sel����!ret=[%d]", ret);
            WRITEMSG
            return(-1);
         }
         else  /* �ڶ������: ���ں�������, ����ƾ֤��ʼ������������ֹ���������� */
         {
             note_mst2.cnt += note_mst.cnt;

             /* ���δ���, ����ʼ�����Ϊ�������ʼ���� */
             ret = sql_execute("update note_mst set beg_note_no = '%s', \
                                cnt = %d where tel = '%s' and sts = '%s' \
                                and note_type = '%s' and beg_note_no = '%s'", \
                                note_mst.beg_note_no, note_mst2.cnt, \
                                note_mst.tel, note_mst.sts, note_mst.note_type, \
                                end_no);
             if (ret)
             {
                 sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "N019");
                 return(-1);
             }
         } /* end 3333 if */
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_mst_Fet_Upd����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }else  /* ����ƾ֤��ֹ������������ʼ����������ƾ֤ */
    {
        /* ���������������Ƿ������ʼ��������������ֹ���������� */
        ret=Note_mst_Sel(g_pub_tx.reply, &note_mst2, "tel = '%s' and \
                     sts = '%s' and note_type='%s' and beg_note_no='%s'", \
                     note_mst.tel, note_mst.sts, note_mst.note_type, end_no);
        /* 4444 if */
        if (ret == 100)/*������: ǰ�������, ������ƾ֤��ʼ������������ֹ����������*/
        {
            /* ���δ���, ����ֹ�����Ϊ�������ֹ���� */
            strcpy(note_mst1.end_note_no, note_mst.end_note_no);
            note_mst1.cnt += note_mst.cnt;

            ret = Note_mst_Upd_Upd(note_mst1, g_pub_tx.reply);

            if (ret == -239)
            {
                sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N019");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_mst_Upd_Upd��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
        }else if (ret)
        {
            sprintf(acErrMsg, "ִ��Note_mst_Dec_Upd����!ret=[%d]", ret);
            WRITEMSG
            return(-1);
        }else
        {
            /* ������: ǰ������ */
            /*�������ƾ֤����β��*/
            strcpy(note_mst1.end_note_no, note_mst2.end_note_no);
            note_mst1.cnt = note_mst1.cnt + note_mst2.cnt + note_mst.cnt;

            /* ���δ���, �������ƾ֤����ԭ��ƾ֤��β���� */
            ret=Note_mst_Upd_Upd(note_mst1, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N019");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_mst_Upd_Upd��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }

            /* ɾ����һ�μ�¼note_mst2 */
            sprintf(cFilter, "tel = '%s' and note_type = '%s' \
                     and beg_note_no = '%s' and end_note_no = '%s' \
                     and sts = '%s'", note_mst2.tel, note_mst2.note_type, \
                     note_mst2.beg_note_no, note_mst2.end_note_no, note_mst2.sts);
            ret = deldb("note_mst", cFilter);
            if (ret)
            {
                 sprintf(acErrMsg, "ɾ����Աƾ̨֤�ʴ�");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "N005");
                 return(-1);
            }
        } /** end 4444 if */
    } /* end 1111 if */
    /* �ر��α� */
    Note_mst_Clo_Upd();

    return(0);
}


/**********************************************************************
 * �� �� ��:   pub_com_NoteDel
 * ��������:   ƾ֤�ֶ�ɾ������
 *        ��ԭ������һ������ε�ƾ֤���մ������ʼ�������ֹ����ֿ�, ��ɾ����
 *        ���������ʼ���뵽������ʼ����ǰ��һ��, ������ʼ���뵽��ֹ������һ
 *        ��, ������ֹ���뵽��������ֹ����Ϊһ�Ρ��������ʼ������ͬ����ֹ
 *        ������ͬ��ͬ���ܷ����λ�һ�Ρ�
 *        �ֳ�һ��: ɾ��    ����: �޸�  ����: �޸ġ����ӡ�ɾ��
 *
 * ����/ʱ��:  jack/2005��1��6��
 *
 * ����:
 *     ����:
 *        char  *note_act       ƾ֤��Ϊ
 *            struct note_mst_c note_mst    ��Աƾ̨֤��
 *
 *     ���:   char g_pub_tx.reply      ��Ӧ��
 *
 *   ����ֵ:   0: ���γɹ� -1: ���ɹ�
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_NoteDel(char *note_act, struct note_mst_c note_mst)
{
    struct note_mst_c sNoteMst;  /* �ӹ�Աƾ̨֤��ȡ����ֵ */
    struct note_mst_c sNoteMstTmp; /* ��Ź�Աƾ̨֤�ʵ���ʱ���� */

    char   cPerNoteSts[2];  /* ��Ϊִ�е�״̬ */
    char   cWho[2];     /* ƾ֤��Ϊִ���� */
    char   str_tmp[32];

    int    ret;

    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(cWho, 0x00, sizeof(cWho));
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts));

    strcpy(cWho, "T");   /* T��ʾ��Ա */

    /* ����ƾ֤��Ϊ�ҳ�ƾ֤״̬ */
    ret = pub_com_NoteActSts(cWho, note_act, note_mst.sts, cPerNoteSts);
    if (ret)
    {
        sprintf(acErrMsg, "����ƾ֤��Ϊȡƾ֤״̬��!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N002");
        return(-1);
    }

    strcpy(cWho, "T");   /* T��ʾ��Ա */

    /* ����Ƿ�������������� */
    /* ��ԭƾ֤״̬�Ĳ��� */
    ret = Note_mst_Dec_Upd(g_pub_tx.reply, "br_no = '%s' and tel = '%s' \
                and note_type = '%s' and beg_note_no <= '%s' \
                and end_note_no >= '%s' and sts = '%s'", note_mst.br_no, \
                note_mst.tel, note_mst.note_type, note_mst.beg_note_no, \
                note_mst.end_note_no, note_mst.sts);
    if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_mst_Dec_Upd��!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_mst_Fet_Upd(&sNoteMst, g_pub_tx.reply);

    if (ret == 100)
    {
        sprintf(acErrMsg, "������������ƾ֤����!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N003");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_mst_Fet_Upd����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }else                 /* 0 �ҵ����������ļ�¼ */
    {
        /* ���ͷ�Ƿ���ͬ */
        if(!strcmp(sNoteMst.beg_note_no, note_mst.beg_note_no)) /* ͷ��ͬ */
        {
            if(!strcmp(sNoteMst.end_note_no, note_mst.end_note_no)) /* β��ͬ */
            {
                /* 1: ͷβ��ͬ, ɾ����¼ */
        ret = sql_execute("delete from note_mst \
                where br_no='%s' and tel='%s'\
                                and note_type = '%s' and beg_note_no <= '%s' \
                                and end_note_no >= '%s' and sts = '%s'", \
                                note_mst.br_no, \
                note_mst.tel, note_mst.note_type, note_mst.beg_note_no, \
                note_mst.end_note_no, note_mst.sts);
                if (ret)
                {
                    sprintf(acErrMsg, "ɾ����Աƾ̨֤�ʴ�!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N005");
                    return(-1);
                }
            }else  /* β��ͬ */
            {
                /* 2: ͷ��ͬβ��ͬ */
                /* �޸�ԭ��¼����ʼ���롢���� */
                ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.end_note_no, \
                             str_tmp, 1);
                if (ret == -1)
                {
                    sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    return(-1);
                }

                strcpy(sNoteMst.beg_note_no, str_tmp);

                /* ����ƾ֤���� */
                sNoteMst.cnt -=  note_mst.cnt;

                /* �޸�ԭƾ֤ */
                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��, ���ظ���¼!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "ִ��Note_mst_Upd_Upd��!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
            }
        }else /* ͷ��ͬ */
        {
            if(!strcmp(sNoteMst.end_note_no, note_mst.end_note_no)) /* β��ͬ */
            {
                /* 3: ͷ��ͬβ��ͬ: �޸�ԭ���� */
                /* ����ƾ֤��ֹ���� */
                ret = pub_com_CalNoteNo(note_mst.note_type, \
                          note_mst.beg_note_no, str_tmp, -1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    return(-1);
                }
                strcpy(sNoteMst.end_note_no, str_tmp);

                /* ����ƾ֤���� */
                sNoteMst.cnt -= note_mst.cnt;

                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                    sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��, ���ظ���¼!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "ִ��Note_mst_Upd_Upd��!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
            }else   /* β��ͬ */
            {
                /* 4: ͷβ����ͬ */
                sNoteMstTmp = sNoteMst;

                /* �޸�ԭ����ƾ֤, �ֳ�2�� */
                /* 1: ����ƾ֤��ֹ���� */
                ret = pub_com_CalNoteNo(sNoteMst.note_type, note_mst.beg_note_no, \
                                      sNoteMst.end_note_no, -1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    return(-1);
                }

                /* ����ƾ֤���� */
                ret = pub_com_CalNoteCnt(note_mst.note_type, \
                        sNoteMst.beg_note_no, sNoteMst.end_note_no);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤������!");
                    WRITEMSG
                    return(-1);
                }
                sNoteMst.cnt = ret;

                /* �޸Ĺ�Աƾ֤ */
                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "�޸Ĺ�Աƾ̨֤��note_mst��!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "ִ��Note_mst_Upd_Upd��!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }

                /* 2: ������һ�� */
                /* ����ƾ֤��ʼ���� */
                ret = pub_com_CalNoteNo(sNoteMstTmp.note_type, \
                          note_mst.end_note_no, sNoteMstTmp.beg_note_no, 1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    return(-1);
                }
                /* ����ƾ֤���� */
                ret = pub_com_CalNoteCnt(sNoteMstTmp.note_type, \
                        sNoteMstTmp.beg_note_no, sNoteMstTmp.end_note_no);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "����ƾ֤�����!");
                    WRITEMSG
                    return(-1);
                }
                sNoteMstTmp.cnt = ret;

                /* ����һ���¼�¼*/
                ret = Note_mst_Ins(sNoteMstTmp, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "���Աƾ̨֤��note_mst�����¼��!���ظ���¼!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_mst_Ins��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
            }
        }
    }

    /* �ر��α� */
    Note_mst_Clo_Upd();

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_CalNoteNo
 * ��������:   ����ƾ֤��������ֵ�����µ�ƾ֤����
 * ����/ʱ��:  jack / 2003��12��16��
 *
 * ��    ��:
 *     ����:
 *            char *note_type    ƾ֤����
 *            char *note_no      ��ʼ����
 *            int  cnt           ƾ֤����
 *
 *     ���:
 *            char *re_note_no   ��ֹ����
 *            char *g_pub_tx.reply        ��Ӧ��
 *
 *   ����ֵ:   0: �ɹ� -1: ���ɹ�
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_CalNoteNo(
    char *note_type,
    char *note_no,
    char *re_note_no,
    int  cnt)
{
    struct  note_parm_c  sNoteParm;  /* ȡƾ֤���� */
    char    cForm[20], cNoteNo[17];
    long    lNoteNo;
    int     iValLen;      /* ȥ������ͷ��ƾ֤�ų��� */
    int     ret;

    MEMSET_DEBUG(cForm, 0x00, sizeof(cForm));
    MEMSET_DEBUG(cNoteNo, 0x00, sizeof(cNoteNo));

    /* ����ƾ֤����ȡƾ֤������ */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* ȥ������ͷ��ƾ֤��ŵĳ��� */
    iValLen = 16 - sNoteParm.head_len;

    /* ��ʼ����ֵ */
    lNoteNo = atol(note_no + sNoteParm.head_len);
    lNoteNo += cnt;

    /* �����Ÿ�ʽ */
    sprintf(cForm, "%%0%dld", iValLen);
    sprintf(cNoteNo, cForm, lNoteNo);    /* ��ֹ���� */

    /* ����ͷ������ţ�ƾ֤���� */
    strncpy(re_note_no, note_no, sNoteParm.head_len);
    re_note_no[sNoteParm.head_len] = '\0';
    strcat(re_note_no, cNoteNo);

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_RegTelNote
 * ��������:   �Ǽǹ�Աƾ֤
 * ����/ʱ��:  jack/2003��12��24��
 *
 * ��    ��:
 *     ����:
  *           char *tel          ��Ա��
 *        char *br_no    ��������
 *            char *pNoteAct     ƾ֤��Ϊ
 *            char *note_typ     ƾ֤����
 *            char *beg_no       ��ʼ����
 *            char *end_no       ��ֹ����
 *
 *     ���:
 *            char g_pub_tx.reply[]       ��Ӧ��
 *
 *   ����ֵ:  0:   �ɹ�
 *            ��0: ���ɹ�
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_RegTelNote(tel, br_no, pNoteAct, note_type, beg_no, end_no)
    char *tel;
    char *br_no;
    char *pNoteAct;
    char *note_type;
    char *beg_no;
    char *end_no;
{
    struct note_mst_c note_mst;     /* ��Աƾ̨֤�ʽṹ */
    int    ret;
    char   pNoteSts[2];
    char   cPerNoteSts[2];  /* ��Ϊִ�е�״̬ */
    char   cWho[2];     /* ƾ֤��Ϊִ���� */

    MEMSET_DEBUG(&note_mst, 0X00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts));
    MEMSET_DEBUG(cWho, 0x00, sizeof(cWho));

    strcpy(cWho, "T");

    /* ��ֵ */
    strcpy(note_mst.tel, tel);        /* ��Ա��   */
    strcpy(note_mst.br_no, br_no);        /* �������� */
    strcpy(note_mst.note_type, note_type);    /* ƾ֤���� */
    strcpy(note_mst.beg_note_no, beg_no); /* ��ʼ���� */
    strcpy(note_mst.end_note_no, end_no); /* ��ֹ���� */

    /* ����ƾ֤��Ϊȡ����Ӧ��ƾ֤״̬ */
    ret=pub_com_NoteActSts(cWho, pNoteAct, pNoteSts, cPerNoteSts);
    if(ret == 1)
    {
        sprintf(acErrMsg, "ƾ֤��Ϊ��������Χ!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N009");
        return(-1);
    }

    strcpy(note_mst.sts, pNoteSts);       /* ƾ֤״̬ */

    /* ����ƾ֤���� */
    ret=pub_com_CalNoteCnt(note_type, beg_no, end_no);
    if(ret <= 0)
    {
        sprintf(acErrMsg, "����ƾ֤������!");
        WRITEMSG
        return(-1);
    }

    note_mst.cnt = ret;             /* ƾ֤���� */

    /* ƾ֤�����Լ��(�ж��Ƿ񲢶�) */
    ret=pub_com_NoteMerg(note_mst);
    if(ret == -1)
    {
        sprintf(acErrMsg, "ƾ֤���δ������!");
        WRITEMSG
        return(-1);
    }

    /* ƾ֤��С�ż�鲢�Ǽ� */

    return(0);

}

/**********************************************************************
 * �� �� ��:  pub_com_ChkNoteMin
 * ��������:  ƾ֤��С�����顣 
 *            ����ƾ֤�����Ƿ�Ϊ��С�ſ���,
 *            �����Ҫ���Ƽ���Ƿ�Ϊ��С���� 
 * ����/ʱ��:  jack/2003��12��16�� 
 *
 * ��    ��:
 *     ����:
 *            char *note_type   ƾ֤���� 
 *            char *beg_note_no ��ʼ���� 
 *            char *tel         �� �� Ա 
 *
 *     ���:
 *            char *beg_note_no          ��ʼ����(��С��)
 *            char *g_pub_tx.reply       ��Ӧ�� 
 *
 * �� �� ֵ:   0: Ϊ��С���� 1: ��������С�� 2:�����µ���С�� -1:���� 
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ 
 *
********************************************************************/

int pub_com_ChkNoteMin(
       char *note_type,
       char *beg_note_no,
       char *tel)
{
    struct    note_mst_c    sNoteMst;
    struct    note_parm_c   sNoteParm;
    struct    note_min_no_c sNoteMinNo;
    char    cMinNo[17];     /* ������С���� */
    char    cInHead[17];    /*������ƾ֤�ŵĹ���ͷ*/
    char    cMinHead[17];   /*ƾ֤��С����Ĺ���ͷ*/
    long    lBegNo, lMinNo; /* ��ʼ�������С����(��������ͷ) */

    int    ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(&sNoteMinNo, 0x00, sizeof(struct note_min_no_c));

    /* ����ƾ֤����ȡƾ֤�ṹ */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret== 100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }

    /*** ��С�ű�־ΪN, �򲻽��п��� ***/
    if (sNoteParm.order_ind[0] == 'N')
    {
        sprintf(acErrMsg, "��ƾ֤���Ͳ�������С�ſ���");
        WRITEMSG
        return(0);
    }

    /* ȡƾ֤�������ͷ */
    strncpy(cInHead, beg_note_no, sNoteParm.head_len);
    cInHead[sNoteParm.head_len] = '\0';

    /* ȡƾ֤��С�� */
    ret = Note_min_no_Sel(g_pub_tx.reply, &sNoteMinNo, "tel = '%s' \
                          and note_type = '%s'", tel, note_type);
    if (ret == 100)  /* ���ƾ֤��С�ű��в����ڼ�¼, ���note_mst���в�����С�� */
    {
        sprintf(acErrMsg, "note_min_no�����޴�ƾ֤��С��!tel=[%s], note_type=[%s]", tel, note_type);
        WRITEMSG

        ret = sql_min_string("note_mst", "beg_note_no", cMinNo, sizeof(cMinNo)-1,
        "tel='%s' and note_type='%s' and substr(beg_note_no, 1, %ld)='%s' and sts='0'"
                 , tel, note_type, sNoteParm.head_len, cInHead);
        if(ret)
        {
            sprintf(acErrMsg, "ִ��sql_min_string����");
            WRITEMSG
            strcpy(g_pub_tx.reply, "D110");
            return(-1);
        }

        if(!strlen(cMinNo))
        {
            strcpy(g_pub_tx.reply, "N075");
            return(-1);
        }

        /* ����Ƿ�����С��(��note_mst�ֲ���), ���������С��, �򷵻���С�� */
        if(strcmp(beg_note_no, cMinNo) > 0)
        {
            sprintf(acErrMsg, "ƾ֤����С����, ƾ֤��[%s] ��С��[%s]", \
                    beg_note_no, cMinNo);
            WRITEMSG
            strcpy(beg_note_no, cMinNo);
            return(2);
        }
    }
    else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_min_no_Sel����!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D101");
        return(-1);
    }

    /* ȡƾ֤��С����Ĺ���ͷ */
    strncpy(cMinHead, sNoteMinNo.note_min_no, sNoteParm.head_len);
    cMinHead[sNoteParm.head_len] = '\0';

    /* ȥ������ͷ��ƾ֤�������С���� */
    lBegNo = atol(beg_note_no + sNoteParm.head_len);
    lMinNo = atol(sNoteMinNo.note_min_no + sNoteParm.head_len);

    /* ���note_mst���Ƿ������cInHead��ͷ��ƾ֤ */

    vtcp_log("tel = '%s' and sts = '0' and note_type = '%s' and beg_note_no[1, %d] = '%s'", tel, note_type, sNoteParm.head_len, cInHead);

    ret = sql_count("note_mst", "tel = '%s' and sts = '0' \
                      and note_type = '%s' and substr(beg_note_no, 1, %d) = '%s'", \
                      tel, note_type, sNoteParm.head_len, cInHead);
    if(ret < 0)
    {
        sprintf(acErrMsg, "ִ��sql_count��, ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if(ret == 0)
    {
        sprintf(acErrMsg, "��Ա�޴�ƾ֤tel=[%s], note_type[%s]", tel, note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }

    /* �ȽϹ���ͷ�Ƿ���ͬ */
    if(!strcmp(cInHead, cMinHead))  /* �������ͷ��ͬ */
    {
        if(lBegNo != lMinNo)  /* �Ƚ�ƾ֤���� */
        {
            sprintf(acErrMsg, "ƾ֤����С����, ƾ֤��[%d]��С��[%d][%s]", lBegNo, lMinNo, note_type);
            WRITEMSG
            strcpy(g_pub_tx.reply, "N014");
            /* ����ǰ̨��С�� */
            sprintf(beg_note_no, "%d", lMinNo);
            sprintf(acErrMsg, "��С����%d", lMinNo);
            set_zd_data(DC_GET_MSG, acErrMsg);
            WRITEMSG
            return(2);
        }
    }else  /* �������ͷ��ͬ */
    {
        ret = sql_min_string("note_mst", "beg_note_no", cMinNo, sizeof(cMinNo)-1, \
                 "tel = '%s' and note_type = '%s' and substr(beg_note_no, 1, %ld) = '%s' \
                 and sts = '0'", tel, note_type, sNoteParm.head_len, cInHead);
        if(ret)
        {
            sprintf(acErrMsg, "ִ��sql_min_string����");
            WRITEMSG
            strcpy(g_pub_tx.reply, "D110");
            return(-1);
        }
        /* ����Ƿ�����С��(��note_mst�ֲ���) */
        if(strcmp(beg_note_no, cMinNo) != 0)
        {
            strcpy(g_pub_tx.reply, "N014");
            sprintf(acErrMsg, "ƾ֤����С����, ƾ֤��[%s] ��С��[%s]", \
                    beg_note_no, cMinNo);
            WRITEMSG
            strcpy(beg_note_no, cMinNo);
            /* ����ǰ̨��С�� */
            sprintf(acErrMsg, "��С����%s", cMinNo);
            set_zd_data(DC_GET_MSG, acErrMsg);
            WRITEMSG
            return(2);
        }
    }

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_ChkNoteHead
 * ��������:   ���ƾ֤�Ƿ������ͷ,
 *            ���������ͷ, ��ֱ���˳���
 *            �������, ��Ҫ���Ҹù�Ա�Ƿ������ʼ����Ϊ�ú����ƾ֤, ������Ҫ
 *                ���ӹ���ͷ, ����������ʾ������Ϣ
 * ����/ʱ��:  jack/2003��12��24��
 *
 * ��    ��:
 *     ����:
 *            char *note_type   ƾ֤����
 *            char *note_no     ƾ֤����(���ܴ�����ͷ, ���ܲ�������ͷ)
 *            char *tel         �� �� Ա
 *
 *     ���:
 *            char *note_no     ƾ֤��(������ͷ)
 *            char *g_pub_tx.reply       ��Ӧ��
 *
 * �� �� ֵ:   0: �ɹ� -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_ChkNoteHead(
    char *note_type,
    char *note_no,
    char *tel)
{
    struct    note_parm_c   sNoteParm;
    struct    note_mst_c    sNoteMst;

    char    cForm[32];      /* ��ʽ����� */
    char    cNoteNo[17];    /* ���������벻������ͷ, ����������� */
    char    cNoteNoTmp[17];

    long    lNoteNo;
    int     ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));

    /* ���note_no����Ϊ16λ, ��ֱ�ӷ�����ȷ */
    if (strlen(note_no) == 16)
    {
        return(0);
    }

    /* ����ƾ֤����ȡƾ֤�ṹ */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* �������ƾ֤��ת����long���� */
    lNoteNo = atol(note_no);

    /* �ж�ƾ֤���Ƿ������ͷ 16λ �������, ���Զ����Ϲ���ͷ*/
    sprintf(cForm, "%%0%dd", 16 - sNoteParm.head_len);
    sprintf(cNoteNo, cForm, lNoteNo);

    /* ���note_mst���Ƿ������cInHead��ͷ��ƾ֤ */
    vtcp_log("AA  tel = '%s' and sts = '0' \
                  and note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", tel, note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    ret = sql_count("note_mst", "tel = '%s' and sts = '0' \
                  and note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", tel, note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    if(ret < 0)
    {
        sprintf(acErrMsg, "ִ��sql_count��!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if(ret > 1)
    {
        sprintf(acErrMsg, "�ҵ�����ƾ֤, �������̫��, ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N008");
        return(-1);
    }
    if(ret == 0)
    {
        sprintf(acErrMsg, "��Ա�޴�ƾ֤note_no[%s]", note_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }

    /* ����ƾ֤������ʼƾ֤(������ͷ)*/
    ret = Note_mst_Sel(g_pub_tx.reply, &sNoteMst, "tel = '%s' and sts = '0' \
                  and note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", tel, note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    if(ret==100)
    {
        sprintf(acErrMsg, "��Ա�޴�ƾ֤!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }
    else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_mst_Sel����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    strncpy(cNoteNoTmp, sNoteMst.beg_note_no, sNoteParm.head_len);
    cNoteNoTmp[sNoteParm.head_len] = '\0';

    strcat(cNoteNoTmp, cNoteNo);

    strcpy(note_no, cNoteNoTmp);

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_ChkNoteHead1
 * ��������:   ���ƾ֤�Ƿ������ͷ,
 *            ���������ͷ, ��ֱ���˳���
 *            �������, ��Ҫ���Ҹù�Ա�Ƿ������ʼ����Ϊ�ú����ƾ֤, ������Ҫ
 *                ���ӹ���ͷ, ����������ʾ������Ϣ
 *            Ϊ���гжһ�Ʊ��Ʊ�����յǼǺͺ���ʱ����ã�
 * ����/ʱ��:  jack/2003��12��24��
 *
 * ��    ��:
 *     ����:
 *            char *note_type   ƾ֤����
 *            char *note_no     ƾ֤����(���ܴ�����ͷ, ���ܲ�������ͷ)
 *
 *     ���:
 *            char *note_no     ƾ֤��(������ͷ)
 *            char *g_pub_tx.reply       ��Ӧ��
 *
 * �� �� ֵ:   0: �ɹ� -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *              jack/2005.08.04/ƾ֤״̬��Ϊ4
 *
********************************************************************/

int pub_com_ChkNoteHead1(
    char *note_type,
    char *note_no
)
{
    struct    note_parm_c   sNoteParm;
    struct    note_mst_c    sNoteMst;

    char    cForm[32];      /* ��ʽ����� */
    char    cNoteNo[17];    /* ���������벻������ͷ, ����������� */
    char    cNoteNoTmp[17];

    long    lNoteNo;
    int     ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));

    /* ���note_no����Ϊ16λ, ��ֱ�ӷ�����ȷ */
    if (strlen(note_no) == 16)
    {
        return(0);
    }

    /* ����ƾ֤����ȡƾ֤�ṹ */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* �������ƾ֤��ת����long���� */
    lNoteNo = atol(note_no);

    /* �ж�ƾ֤���Ƿ������ͷ 16λ �������, ���Զ����Ϲ���ͷ*/
    sprintf(cForm, "%%0%dd", 16 - sNoteParm.head_len);
    sprintf(cNoteNo, cForm, lNoteNo);

    /* ���note_mst���Ƿ������cInHead��ͷ��ƾ֤ */
    vtcp_log("AA note_type = '%s' and beg_note_no[%d, 16] <= '%s' \
                  and end_note_no[%d, 16] >= '%s'", note_type, \
                  sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, cNoteNo);
    ret = sql_count("note_mst", " note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    if(ret < 0)
    {
        sprintf(acErrMsg, "ִ��sql_count��!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if(ret > 1)
    {
        sprintf(acErrMsg, "�ҵ�����ƾ֤, �������̫��, ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N008");
        return(-1);
    }
    if(ret == 0)
    {
        sprintf(acErrMsg, "�޴�ƾ֤note_no[%s]", note_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N069");
        return(-1);
    }

    /* ����ƾ֤������ʼƾ֤(������ͷ)*/
    ret = Note_mst_Sel(g_pub_tx.reply, &sNoteMst, " note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    if(ret==100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N069");
        return(-1);
    }
    else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_mst_Sel����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    strncpy(cNoteNoTmp, sNoteMst.beg_note_no, sNoteParm.head_len);
    cNoteNoTmp[sNoteParm.head_len] = '\0';

    strcat(cNoteNoTmp, cNoteNo);

    strcpy(note_no, cNoteNoTmp);

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_RegNoteMin
 * ��������:   ƾ֤��С������Ǽǡ�
 *            ���ת����Ƿ������ƾ֤����С, ���С�Ǽ�;
 *            ���ת����ƾ֤�Ƿ��ǵ�ǰƾ֤�������С����, ���ҳ�ת������С����
 *            ���Ǽ�
 * ����/ʱ��:  jack/2003��12��26��
 *
 * ��    ��:
 *     ����:
 *            char *br_no        ��������
 *            char *note_type    ƾ֤����
 *            char *note_no      ��ʼ����
 *            char *tel          �� �� Ա
 *            char *in_out       ת�Ʊ�־ I: ת�� O: ת��
 *
 *     ���:
 *            char *g_pub_tx.reply   ��Ӧ��
 *
 *   ����ֵ:   0: �ɹ� -1: ���ɹ�
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_RegNoteMin(
    char *br_no,
    char *note_type,
    char *note_no,
    char *tel,
    char *in_out)
{
    struct    note_parm_c     sNoteParm;
    struct    note_min_no_c   sNoteMinNoTmp;   /* ������ҵ���ƾ֤��С���� */
    struct    note_min_no_c   sNoteMinNo;      /* ������ڲ�����ƾ֤��С���� */
    char      cHead[17], cMinHead[17];

    int        ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMinNoTmp, 0x00, sizeof(struct note_min_no_c));
    MEMSET_DEBUG(&sNoteMinNo, 0x00, sizeof(struct note_min_no_c));
    MEMSET_DEBUG(cHead, 0x00, sizeof(cHead));
    MEMSET_DEBUG(cMinHead, 0x00, sizeof(cMinHead));

    strcpy(sNoteMinNo.br_no, br_no);
    strcpy(sNoteMinNo.note_type, note_type);
    strcpy(sNoteMinNo.tel , tel);
    strcpy(sNoteMinNo.note_min_no, note_no);

    if (in_out[0] == 'I') /* ת��ƾ֤ */
    {
        /* ȡƾ֤������ */
        ret = Note_parm_Sel (g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
        if (ret == 100)
        {
            sprintf(acErrMsg, "�޴�ƾ֤����!note_type=[%s]", note_type);
            WRITEMSG
            strcpy(g_pub_tx.reply, "N001");
            return(-1);
        }
        else if (ret)
        {
            sprintf(acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]", ret);
            WRITEMSG
            return(-1);
        }

    /* ȡƾ֤��С�ű� */
    ret = Note_min_no_Dec_Upd(g_pub_tx.reply, "tel = '%s' \
                              and note_type = '%s'", tel, note_type);
    if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_min_no_Dec_Upd����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_min_no_Fet_Upd(&sNoteMinNoTmp, g_pub_tx.reply);
    if (ret == 100)   /* û�ҵ���С��, ����ǰ������Ϊ��С��*/
    {
        /* ��ƾ֤��С�� */
            ret = pub_com_GetNoteMin(note_type, sNoteMinNo.note_min_no, tel);
            if(ret==-1)
        {
            sprintf(acErrMsg, "����ƾ֤��С��ʧ��!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N036");
            return -1;
        }

            /* ������С������ */
            ret = Note_min_no_Ins(sNoteMinNo, g_pub_tx.reply);
            if (ret==-239)
            {
                sprintf(acErrMsg, "��ƾ֤��С�ű�����¼����!���ظ���¼!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N017");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_min_no_Ins��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            } /* end if */
    }else if(ret)
    {
        sprintf(acErrMsg, "ִ��Note_min_no_Fet_Upd����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }
    else   /* �ҵ���С��, �����������бȽ�, ���޸���С�ű� */
        {
            /* �ȽϹ���ͷ�Ƿ���ͬ */
            strncpy(cHead, sNoteMinNo.note_min_no, sNoteParm.head_len);
            strncpy(cMinHead, sNoteMinNoTmp.note_min_no, sNoteParm.head_len);
            cHead[sNoteParm.head_len] = '\0';
            cMinHead[sNoteParm.head_len] = '\0';

            if (!strcmp(cHead, cMinHead)) /* �������ͷ��ͬ */
            {
                if(strcmp(sNoteMinNo.note_min_no, sNoteMinNoTmp.note_min_no)<0)
                {
                    /* �޸�ƾ֤��С�� */
                    strcpy(sNoteMinNoTmp.note_min_no, sNoteMinNo.note_min_no);
                }
            }else    /* �����ͬ����ͷ */
            {
                ret = pub_com_GetNoteMin(note_type, sNoteMinNo.note_min_no, tel);
                if(ret == -1)
            {
                sprintf(acErrMsg, "����ƾ֤��С��ʧ��!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N036");
                return -1;
            }
            strcpy(sNoteMinNoTmp.note_min_no, sNoteMinNo.note_min_no);
            }
            /* �޸�ƾ֤��С�� */
            ret = Note_min_no_Upd_Upd(sNoteMinNoTmp, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "����ƾ֤��С�ű�note_min_no��!���ظ���¼!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N017");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_min_no_Upd_Upd��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
        }
        Note_min_no_Clo_Upd();
    }

    else if (in_out[0] == 'O')  /* ת�� */
    {
    /* ��ƾ֤��С���� */
        ret = pub_com_GetNoteMin(note_type, sNoteMinNo.note_min_no, tel);
        if(ret==-1)
    {
        sprintf(acErrMsg, "����ƾ֤��С��ʧ��!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N036");
        return -1;
    }

        /* ȡƾ֤��С�ű� */
    ret = Note_min_no_Dec_Upd(g_pub_tx.reply, "tel = '%s' \
                            and note_type = '%s'", tel, note_type);
    if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_min_no_Dec_Upd����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_min_no_Fet_Upd(&sNoteMinNoTmp, g_pub_tx.reply);
    if (ret == 100)   /* û�ҵ���С��, �����¼�¼ */
    {
            /* ������Сƾ֤���� */
            ret = Note_min_no_Ins(sNoteMinNo, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "����ƾ֤��С�ű��!���ظ���¼!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N017");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_min_no_Ins��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
        }
        else if(ret)
    {
        sprintf(acErrMsg, "ִ��Note_min_no_Fet_Upd����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
        }else   /* �ҵ���С��, ���޸���С���� */
        {
            strcpy(sNoteMinNoTmp.note_min_no, sNoteMinNo.note_min_no);

            /* �޸���Сƾ֤���� */
            ret = Note_min_no_Upd_Upd(sNoteMinNoTmp, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "����ƾ֤��С�ű��!���ظ���¼!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N017");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "ִ��Note_min_no_Upd_Upd��!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
        }
        Note_min_no_Clo_Upd();
    }else
    {
        sprintf(acErrMsg, "ƾ֤ת�Ʒ������, brno=[%s], in_out=[%s]", br_no, in_out);
        WRITEMSG
        return(-1);
    }
    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_NoteMstSts
 * ��������:   ȡƾ֤״̬����
 *            ���������ƾ֤����(��ʼ����ֹ)������Ա��ƾ֤���ͻ��ƾ֤״̬
 *
 * ����/ʱ��:  jack/2003��12��26��
 *
 * ��    ��:
 *     ����:
 *            char *note_type    ƾ֤����
 *            char *beg_note_no  ��ʼ����
 *            char *end_note_no  ��ֹ����
  *           char *tel          �� �� Ա
 *
 *     ���:
 *            char *sts     ƾ֤״̬
 *            char *g_pub_tx.reply   ��Ӧ��
 *
 *   ����ֵ:   0: �ɹ� -1: ���ɹ�
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_NoteMstSts(
    char *note_type,
    char *beg_note_no,
    char *end_note_no,
    char *tel,
    char *sts)
{
    struct note_mst_c sNoteMst;
    int    ret;

    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));

    ret = Note_mst_Sel(g_pub_tx.reply, &sNoteMst , "tel = '%s' \
             and beg_note_no <= '%s' and end_note_no >= '%s' \
             and note_type = '%s'", tel, beg_note_no, end_note_no, note_type);
        if(ret==100)
        {
                sprintf(acErrMsg, "��Ա�޴�ƾ֤!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N007");
            return(-1);
        }else if (ret)
        {
            sprintf(acErrMsg, "ִ��Note_mst_Sel����!ret=[%d]", ret);
            WRITEMSG
            return(-1);
        }

        strcpy(sts, sNoteMst.sts);

        return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_RegNoteHst
 * ��������:   ƾ֤����ǼǴ�����ƾ֤�����ʱ��Ҫ�ǼǸõǼǲ���
 * ����/ʱ��:  jack/2003��12��26��
 *
 * ����:
 *     ����:
 *            char  *note_act   ƾ֤��Ϊ
 *            struct note_mst_c note_mst ƾ̨֤��
 *            char  *tw_br_no   �Է�����
 *        char  *tw_tel     �Է�����Ա
 *
 *     ���:  char  *g_pub_tx.reply         ��Ӧ��
 *
 *   ����ֵ:   0: �Ǽǳɹ� 1: �Ǽ�ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_RegNoteHst(note_act, note_mst, tw_br_no, tw_tel)
    char   *note_act;
    struct note_mst_c   note_mst;
    char   *tw_br_no;
    char   *tw_tel;
{
    struct note_mst_hst_c sNoteHst;
    int    ret;

    MEMSET_DEBUG(&sNoteHst, 0x00, sizeof(struct note_mst_hst_c));

    strcpy(sNoteHst.tel, g_pub_tx.tel);               /* ������Ա */
    strcpy(sNoteHst.note_type, note_mst.note_type);       /* ƾ֤���� */
    strcpy(sNoteHst.br_no, note_mst.br_no);           /* �������� */
    strcpy(sNoteHst.beg_note_no, note_mst.beg_note_no);   /* ��ʼ���� */
    strcpy(sNoteHst.end_note_no, note_mst.end_note_no);   /* ��ֹ���� */
    sNoteHst.note_cnt = note_mst.cnt;               /* ƾ֤���� */
    sNoteHst.tx_date = g_pub_tx.tx_date;            /* �������� */
    sNoteHst.tx_time = g_pub_tx.tx_time;            /* ����ʱ�� */
    sNoteHst.trace_no = g_pub_tx.trace_no;          /* �� ˮ �� */
    sNoteHst.trace_cnt = g_pub_tx.trace_cnt;            /* ��ˮ�ʴ� */
    strcpy(sNoteHst.auth, g_pub_tx.auth);         /* ��Ȩ��   */
    strcpy(sNoteHst.tw_br_no, tw_br_no);          /* �Է����� */
    strcpy(sNoteHst.tw_tel, tw_tel);              /* �Է���Ա */
    strcpy(sNoteHst.sts_ind, note_act);           /* �������� */

    ret = Note_mst_hst_Ins(sNoteHst, g_pub_tx.reply);
    if (ret == -239)
    {
        sprintf(acErrMsg, "���Աƾ֤��ϸ������¼��!���ظ���¼!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N024");
        return(-1);
    }else if(ret)
    {
        sprintf(acErrMsg, "ִ��Note_mst_hst_Ins��!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * ��  �� ��:   pub_com_ChkNoteUse
 * ���� ����:   ��Աƾ֤���:
 *                        ���ƾ֤�����Ƿ����
 *                        ���ƾ֤�Ƿ���ʹ��״̬
 * ����/ʱ��:   jack/2003��12��29��
 *
 * ��  ��:
 *     ����:   char note_type[4]        ƾ֤����
 *
 *     ���:   char g_pub_tx.reply[]    ��Ӧ��
 *
 *   ����ֵ:   0: ���� -1: ͣ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_ChkNoteUse(char *note_type)
{
    struct  note_parm_c note_parm;
    int ret;

    MEMSET_DEBUG(&note_parm, 0X00, sizeof(struct note_parm_c));

    /* ���ƾ֤�����Ƿ���� */
    ret=Note_parm_Sel(g_pub_tx.reply , &note_parm , "note_type='%s'" , note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }
    else if(ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }

    /* ���ƾ֤�Ƿ���ʹ��״̬ */
    if(note_parm.use_ind[0] == 'N')
    {
        sprintf(acErrMsg, "ƾ֤����ͣ��״̬!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N025");
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_ChkBoeNote
 * ��������:   ���ƾ֤��ʼ����ֹ�����������
 *            ����ͷ������ͬ����ֹ���������ڵ�����ʼ����
 *
 * ����/ʱ��:  Terry.cui/2003��12��30��
 *
 * ����:
 *     ����:
 *            char      *note_type      ƾ֤����
 *            char  *beg_note_no    ��ʼ����
 *        char  *end_note_no    ��ֹ����
 *
 *     ���:  char  *g_pub_tx.reply         ��Ӧ��
 *
 *   ����ֵ:   0: �ɹ� -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/
int pub_com_ChkBoeNote(
        char *note_type,
        char *beg_note_no,
        char *end_note_no)
{
    struct note_parm_c sNoteParm;
    char    cBegHead[17], cEndHead[17];
    long    lBegNo, lEndNo;

    int ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cBegHead, 0x00, sizeof(cBegHead));
    MEMSET_DEBUG(cEndHead, 0x00, sizeof(cEndHead));

    ret=Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type='%s'", note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }
    else if(ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    strncpy(cBegHead, beg_note_no, sNoteParm.head_len);
    strncpy(cEndHead, end_note_no, sNoteParm.head_len);
    cBegHead[sNoteParm.head_len] = '\0';
    cEndHead[sNoteParm.head_len] = '\0';

    if (strcmp(cBegHead, cEndHead))
    {
        sprintf(acErrMsg, "��ʼ�������ֹ�������ͷ��ͬ");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N027");
        return(-1);
    }

    lBegNo = atol(beg_note_no + sNoteParm.head_len);
    lEndNo = atol(end_note_no + sNoteParm.head_len);

    if (lBegNo > lEndNo)
    {
        sprintf(acErrMsg, "��ʼ���������ֹ����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N028");
        return(-1);
    }

    return(0);

}

/**********************************************************************
 * �� �� ��:   pub_com_GetNoteMin
 * ��������:
 *            ���ҹ�Աƾ֤��С��(��Աƾ֤������)
 *
 * ����/ʱ��:  Terry.cui/2003��12��30��
 *
 * ����:
 *     ����:
 *            char      *note_type      ƾ֤����
 *            char  *note_no    ��ʼ����
 *        char  *tel        ����Ա
 *
 *     ���:  char  *g_pub_tx.reply         ��Ӧ��
 *
 *   ����ֵ:   0: �ɹ� -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_GetNoteMin(
       char *note_type,
       char *note_no,
       char *tel)
{
    struct    note_parm_c   sNoteParm;
    char    cMinNo[17];     /* ������С���� */
    char    cInHead[17];    /*������ƾ֤�ŵĹ���ͷ*/

    int    ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cMinNo, 0x00, sizeof(cMinNo));
    MEMSET_DEBUG(cInHead, 0x00, sizeof(cInHead));
        sprintf(acErrMsg, "pub_com_GetNoteMin, note_no=[%s]", note_no);
        WRITEMSG

    /* ����ƾ֤����ȡƾ֤�ṹ */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret== 100)
    {
        sprintf(acErrMsg, "�޴�ƾ֤����!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* ȡ����ƾ֤�������ͷ */
    strncpy(cInHead, note_no, sNoteParm.head_len);
    cInHead[sNoteParm.head_len] = '\0';

    /* ���note_mst���Ƿ������cInHead��ͷ��ƾ֤ */
    ret = sql_count("note_mst", "tel = '%s' and sts = '0' \
                      and note_type = '%s' and substr(beg_note_no, 1, %d) = '%s'", \
                      tel, note_type, sNoteParm.head_len, cInHead);
    if(ret < 0)
    {
        sprintf(acErrMsg, "ִ��sql_count��, ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }else if(ret == 0) /* ��������ڹ���ͷ��ͬ��, ����Ҹù�Ա����ƾ֤����С�� */
    {
        ret = sql_min_string("note_mst", "beg_note_no", cMinNo, \
                       sizeof(cMinNo)-1, "tel = '%s' and note_type = '%s' \
                       and sts = '0'", tel, note_type);
        if(ret)
        {
            sprintf(acErrMsg, "ִ��sql_min_string����ret = [%d]", ret);
            WRITEMSG
            strcpy(g_pub_tx.reply, "D110");
            return(-1);
        }
    }else     /* ��note_mst���в��ҹ���ͷ��ͬ����С�� */
    {
        ret = sql_min_string("note_mst", "beg_note_no", cMinNo, sizeof(cMinNo)-1, \
                  "tel = '%s' and note_type = '%s' and substr(beg_note_no, 1, %ld) = '%s' \
                  and sts = '0'", tel, note_type, sNoteParm.head_len, cInHead);
        if(ret)
        {
            sprintf(acErrMsg, "ִ��sql_min_string����");
            WRITEMSG
            strcpy(g_pub_tx.reply, "D110");
            return(-1);
        }
    }
    strcpy(note_no, cMinNo);

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_GetNoteHst
 * ��������:
 *            ���ݽ������ڡ�ƾ֤���ࡢ��ʼƾ֤����ֹƾ֤���Է��������Է���Ա��
 *        ƾ֤��Ϊ���ҹ�Աƾ֤��ϸ, ���ҷ���
 *
 * ����/ʱ��:  Terry.cui/2003��12��30��
 *
 * ����:
 *     ����:
 *      long    tx_date     ��������
 *      char    *note_type  ƾ֤����
 *      char    *beg_note_no    ��ʼ����
 *      char    *end_note_no    ��ֹ����
 *      char    *tw_br_no   �Է�����
 *      char    *tw_tel     �Է���Ա
 *      char    *note_act   ƾ֤��Ϊ
 *
 *     ���:
 *      struct  note_mst_hst_c  *note_mst_hst   ��Աƾ֤��ϸ
 *      char    *g_pub_tx.reply         ��Ӧ��
 *
 *   ����ֵ:   0: �ɹ� -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_GetNoteHst(
    long    tx_date,
    char    *note_type,
    char    *beg_note_no,
    char    *end_note_no,
    char    *tw_br_no,
    char    *tw_tel,
    char    *note_act,
    struct  note_mst_hst_c  *note_mst_hst)
{
    int icount = 0;
    int iret;

    vtcp_log("note_type = '%s'  and beg_note_no = '%s' and end_note_no = '%s'  and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s'",
    note_type, beg_note_no, end_note_no, tx_date, tw_br_no, tw_tel, note_act);

    iret = Note_mst_hst_Dec_Sel(g_pub_tx.reply , "note_type = '%s' \
            and beg_note_no = '%s' and end_note_no = '%s' \
            and tx_date = %ld and tw_br_no = '%s' \
            and tw_tel = '%s' and sts_ind = '%s' order by trace_no DESC ",
            note_type, beg_note_no, end_note_no, tx_date, tw_br_no, \
            tw_tel, note_act);

    if (iret)
    {
        sprintf(acErrMsg, "ִ��    Note_mst_hst_Dec_Sel��, iret=[%d]", iret);
        WRITEMSG
        return(-1);
    }
    while(1)
    {
        iret = Note_mst_hst_Fet_Sel(note_mst_hst , g_pub_tx.reply);
        if (icount > 0 && iret == 100)
        {
            break;
        }
        if (iret == 100)
        {
            sprintf(acErrMsg, "��Աƾ֤��ϸ������note_type = '%s' and beg_note_no = '%s' and end_note_no = '%s' and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s'", note_type, beg_note_no, end_note_no, tx_date, tw_br_no, tw_tel, note_act);
            WRITEMSG
            strcpy(g_pub_tx.reply, "N041");
            return(-1);
        }else if (iret)
        {
            sprintf(acErrMsg, "��ѯ��Աƾ֤��ϸ��!iret=[%d]", iret);
            WRITEMSG
            return(-1);
        }

        icount++;
        break;
    }

    if (icount > 1)
    {
        sprintf(acErrMsg, "��Աƾ֤��ϸ���ڶ�����¼");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N042");
        return(-1);
    }
    Note_mst_hst_Clo_Sel ();

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_GetNoteHst1
 * ��������:
 *            ������ˮ�š��������ڡ�ƾ֤���ࡢ��ʼƾ֤����ֹƾ֤���Է�������
 *              �Է���Ա��ƾ֤��Ϊ���ҹ�Աƾ֤��ϸ, ���ҷ���
 *
 * ����/ʱ��:  jack/2003��12��30��
 *
 * ����:
 *     ����:
 *      long    tx_date     ��������
 *      long    trace_no    ������ˮ
 *      char    *note_type  ƾ֤����
 *      char    *beg_note_no    ��ʼ����
 *      char    *end_note_no    ��ֹ����
 *      char    *tw_br_no   �Է�����
 *      char    *tw_tel     �Է���Ա
 *      char    *note_act   ƾ֤��Ϊ
 *
 *     ���:
 *      struct  note_mst_hst_c  *note_mst_hst   ��Աƾ֤��ϸ
 *      char    *g_pub_tx.reply         ��Ӧ��
 *
 *   ����ֵ:   0: �ɹ� -1: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_GetNoteHst1(
    long    trace_no,
    long    tx_date,
    char    *note_type,
    char    *beg_note_no,
    char    *end_note_no,
    char    *tw_br_no,
    char    *tw_tel,
    char    *note_act,
    struct  note_mst_hst_c  *note_mst_hst)
{
    int icount = 0;
    int iret;

    iret = Note_mst_hst_Dec_Sel(g_pub_tx.reply , "note_type = '%s' \
            and beg_note_no = '%s' and end_note_no = '%s' \
            and tx_date = %ld and tw_br_no = '%s' \
            and tw_tel = '%s' and sts_ind = '%s' and trace_no=%ld",
            note_type, beg_note_no, end_note_no, tx_date, tw_br_no, \
            tw_tel, note_act, trace_no);

    if (iret)
    {
        sprintf(acErrMsg, "ִ��    Note_mst_hst_Dec_Sel��, iret=[%d]", iret);
        WRITEMSG
        return(-1);
    }
    while(1)
    {
        iret = Note_mst_hst_Fet_Sel(note_mst_hst , g_pub_tx.reply);
        if (icount > 0 && iret == 100)
        {
            break;
        }
        if (iret == 100)
        {
            sprintf(acErrMsg, "��Աƾ֤��ϸ������note_type = '%s' and beg_note_no = '%s' and end_note_no = '%s' and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s' and trace_no=%ld", note_type, beg_note_no, end_note_no, tx_date, tw_br_no, tw_tel, note_act, trace_no);
            WRITEMSG
            strcpy(g_pub_tx.reply, "N041");
            return(-1);
        }else if (iret)
        {
            sprintf(acErrMsg, "��ѯ��Աƾ֤��ϸ��!iret=[%d]", iret);
            WRITEMSG
            return(-1);
        }

        icount++;
    }

    if (icount > 1)
    {
        sprintf(acErrMsg, "��Աƾ֤��ϸ���ڶ�����¼");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N042");
        return(-1);
    }
    Note_mst_hst_Clo_Sel ();

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_ChkBrRel
 * ��������:
 *            ���ݹ�ϵ���͡��������롢�ϼ������Ų��ҽ��׻�����ϵ��
 *
 * ����/ʱ��:  jack/2005��3��26��
 *
 * ����:
 *     ����:
 *      int *rel_type   ��ϵ����
 *      char    *br_no      ��������
 *      char    *up_br_no   �ϼ�������
 *
 *     ���:
 *      char    *g_pub_tx.reply         ��Ӧ��
 *
 *   ����ֵ:   0: �ɹ�  1/2/3/4/5: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_ChkBrRel(int rel_type, char *br_no, char *up_br_no)
{

    struct  com_tx_br_rel_c sComTxBrRel;    /* ���׻�����ϵ�� */
    int ret;

    MEMSET_DEBUG(&sComTxBrRel, 0x00, sizeof(struct com_tx_br_rel_c));

    sprintf(acErrMsg, "CGH [%s], up [%s] rel_type=[%d]",
                    br_no, up_br_no, rel_type);
    WRITEMSG

    ret = Com_tx_br_rel_Sel(g_pub_tx.reply, &sComTxBrRel, "rel_type=%d \
                and br_no='%s' and up_br_no='%s'", rel_type, \
                br_no, up_br_no);
    if(ret == 100)
    {
        if(rel_type == 1)     /* �ֽ������ϵ */
            return 2;
        else if(rel_type == 2)    /* ƾ֤������ϵ */
            return 3;
        else if(rel_type == 3)    /* ͬ��Ʊ�ݴ��� */
            return 4;
        else if(rel_type == 4)    /* ͬ���ڲ����� */
            return 5;
        else if(rel_type == 5)    /* ���л�Ʊǩ�� */
            return 6;
        else                        /* ��ϵ������ */
            return 1;
    }
    else if(ret)
    {
        sprintf(acErrMsg, "ִ��Com_tx_br_rel_Sel����!ret=[%d]", ret);
        WRITEMSG
        return 1;
    }

    return(0);
}

/**********************************************************************
 * �� �� ��:   pub_com_ChkNtExt
 * ��������:
 *            ʹ��ƾ֤���:����Ա�Ƿ��д�ƾ֤������ʹ��
 *
 * ����/ʱ��:  jack/2005��6��20��
 *
 * ����:
 *     ����:
 *      char    *beg_note_no    ��ʼƾ֤����
 *      char    *end_note_no    ��ֹƾ֤����
 *      char    *note_type      ƾ֤����
 *      char    *br_no          ��������
 *      char    *tel            ��Ա��
 *
 *   ����ֵ:   0: �ɹ�  1/2/3/4/5: ʧ��
 *
 * �޸���ʷ:   �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
 *
********************************************************************/

int pub_com_ChkNtExt(char *beg_note_no, char *end_note_no, char *note_type, char *br_no, char *tel)
{
    char cCond[200];
    char beg_note_no_tmp[17], end_note_no_tmp[17];
    char tmp_str[10];
    int note_no_len;
    struct note_parm_c sNoteParm;
    struct note_mst_c sNoteMst;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));

    g_reply_int = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type='%s'",
        note_type);
    if(g_reply_int == 100)
    {
        strcpy(g_pub_tx.reply, "N001");
        sprintf(acErrMsg, "ƾ֤���ڼ��: ƾ֤���Ͳ�����[%s]", note_type);
        WRITEMSG
        return(-1);
    }
    else if(g_reply_int)
    {
        strcpy(g_pub_tx.reply, "D103");
        sprintf(acErrMsg, "ƾ֤���ڼ��: ��ѯƾ֤��������[%d]", g_reply_int);
        WRITEMSG
        return(-1);
    }

    note_no_len = 16 - sNoteParm.head_len;
    MEMSET_DEBUG(beg_note_no_tmp, 0x00, sizeof(beg_note_no_tmp));
    if(strlen(beg_note_no) >= note_no_len)
        strncpy(beg_note_no_tmp, beg_note_no + (strlen(beg_note_no) - note_no_len), note_no_len);
    else if(strlen(beg_note_no) < note_no_len)
    {
        sprintf(tmp_str, "%%0%ldld", note_no_len);
        sprintf(beg_note_no_tmp, tmp_str, atol(beg_note_no));
    }
    MEMSET_DEBUG(end_note_no_tmp, 0x00, sizeof(end_note_no_tmp));
    if(strlen(end_note_no) >= note_no_len)
        strncpy(end_note_no_tmp, end_note_no + (strlen(end_note_no) - note_no_len), note_no_len);
    else if(strlen(end_note_no) < note_no_len)
    {
        sprintf(tmp_str, "%%0%ldld", note_no_len);
        sprintf(end_note_no_tmp, tmp_str, atol(end_note_no));
    }
    sprintf(cCond, "note_type='%s' and br_no='%s' and tel='%s' and \
                    substr(beg_note_no, %d, 16-%d+1)<='%s' and substr(end_note_no, %d, 16-%d+1)>='%s'", \
                    note_type, br_no, tel, sNoteParm.head_len+1, sNoteParm.head_len+1, beg_note_no_tmp, sNoteParm.head_len+1, sNoteParm.head_len+1, end_note_no_tmp);
    strcpy(acErrMsg, cCond);
    WRITEMSG

    g_reply_int = Note_mst_Sel(g_pub_tx.reply, &sNoteMst, cCond);
    if(g_reply_int == 100)
    {
        strcpy(g_pub_tx.reply, "N007");
        sprintf(acErrMsg, "ƾ֤���ڼ��: ��Ա�޴�ƾ֤.����[%s]��ʼ����[%s]��ֹ����[%s]",
            note_type, beg_note_no_tmp, end_note_no_tmp);
        WRITEMSG
        return(-1);
    }
    else if(g_reply_int)
    {
        strcpy(g_pub_tx.reply, "D103");
        sprintf(acErrMsg, "ƾ֤���ڼ��: ��ѯ��Աƾ̨֤�ʳ���[%d]", g_reply_int);
        WRITEMSG
        return(-1);
    }

    if(sNoteMst.sts[0] != '0')
    {
        if(sNoteMst.sts[0] == '1')
        {
            strcpy(g_pub_tx.reply, "N081");
            sprintf(acErrMsg, "ƾ֤���ڼ��: ƾ֤�ѹ�ʧ������[%s]��ʼ����[%s]��ֹ����[%s]",
                note_type, beg_note_no_tmp, end_note_no_tmp);
            WRITEMSG
            return(-1);
        }
        else if(sNoteMst.sts[0] == '3' || sNoteMst.sts[0] == '6' || sNoteMst.sts[0] == '*')
        {
            strcpy(g_pub_tx.reply, "N083");
            sprintf(acErrMsg, "ƾ֤���ڼ��: ƾ֤�����ϡ�����[%s]��ʼ����[%s]��ֹ����[%s]",
                note_type, beg_note_no_tmp, end_note_no_tmp);
            WRITEMSG
            return(-1);
        }
        else if(sNoteMst.sts[0] == '4' || sNoteMst.sts[0] == '5')
        {
            strcpy(g_pub_tx.reply, "N084");
            sprintf(acErrMsg, "ƾ֤���ڼ��: ƾ֤��ʹ�á�����[%s]��ʼ����[%s]��ֹ����[%s]",
                note_type, beg_note_no_tmp, end_note_no_tmp);
            WRITEMSG
            return(-1);
        }
        else
        {
            strcpy(g_pub_tx.reply, "N022");
            sprintf(acErrMsg, "ƾ֤���ڼ��: ƾ֤״̬������������[%s]��ʼ����[%s]��ֹ����[%s]",
                note_type, beg_note_no_tmp, end_note_no_tmp);
            WRITEMSG
            return(-1);
        }
    }

    sprintf(acErrMsg, "ƾ֤���ڼ��ͨ��, ��Ա�д�ƾ֤");
    WRITEMSG

    return(0);
}
