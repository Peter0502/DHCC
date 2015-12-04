/***********************************************************
* �� �� ��:  spN105.c
* ��������:  ��֧Ʊ��ƾ֤�����ͻ�, ����������, Ҳ��������
*
* ��    ��:  andy
* �������:  2004��1��8��
*
* �޸ļ�¼:
*   ��    ��:
*   �� �� ��:
*   �޸�����:
***********************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_parm_c.h"

/*********************  ȫ�ֱ�������  *********************/
char g_sell_way[2];     /* ƾ֤���۷�ʽ 0����, 1���� */

int spN105()
{
    struct note_mst_c      sNoteMst;       /* ��Աƾ̨֤�� */
    struct note_mst_c      sNoteMstTmp;    /* ��Աƾ̨֤�� */
    struct note_cheq_mst_c sCheqMst;       /* �ͻ�ƾ̨֤�� */
    struct note_mst_hst_c  sNoteMstHst;    /* ��Աƾ̨֤����ϸ */
    struct note_parm_c     sNoteParm;      /* ƾ֤���Բ����� */
    char                   cNoteAct[2];    /* ƾ֤��Ϊ */
    char                   tw_br_no[6];    /* �Է������� */
    char                   tw_tel[6];      /* �Է�����Ա */
    char                   cCheqSts[2];    /* �ͻ�ƾ֤״̬ */
    char                   cOpenBrNo[6];   /* ���������� */
    char                   cWho[2];        /* ƾִ֤����, c: �ͻ�, t: ��Ա */
    char                   cInOut[2];      /* ƾ֤��С�ŷ��� */
    int                    ret;

    memset(&sNoteMst,    0x00, sizeof(struct note_mst_c));
    memset(&sNoteMstTmp, 0x00, sizeof(struct note_mst_c));
    memset(&sCheqMst,    0x00, sizeof(struct note_cheq_mst_c));
    memset(&sNoteMstHst, 0x00, sizeof(struct note_mst_hst_c));
    memset(&sNoteParm,   0x00, sizeof(struct note_parm_c));
    memset(cNoteAct,     0x00, sizeof(cNoteAct));
    memset(tw_br_no,     0x00, sizeof(tw_br_no));
    memset(tw_tel,       0x00, sizeof(tw_tel));
    memset(cCheqSts,     0x00, sizeof(cCheqSts));
    memset(cOpenBrNo,    0x00, sizeof(cOpenBrNo));
    memset(cWho,         0x00, sizeof(cWho));
    memset(cInOut,       0x00, sizeof(cInOut));

    /* ϵͳ��ʼ��(��ʼ��g_pub_tx)���ݳ�ʼ�� */
    init_pub_tx();

    /* ����Ȩ�޼�� */
    if (pub_com_NoteAuthChk(g_pub_tx.tx_br_no, g_pub_tx.tel, 401, 0))
        goto ErrExit;

    strcpy(cNoteAct, "6");    /* 6��ʾ��Աƾ֤���۲��� */
    strcpy(cInOut,   "O");

    /* ���ƾ֤�Ƿ����(ƾ֤��������Ƿ��Ѿ�����, �Ƿ���Ϊ����) */
    ret = pub_com_ChkNoteUse(g_pub_tx.note_type);
    if (ret)
    {
        sprintf(acErrMsg, "���ƾ֤�Ƿ����, �Ƿ����ô�");
        WRITEMSG
        goto ErrExit;
    }

    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type='%s'",
        g_pub_tx.note_type);
    if (ret) goto ErrExit;

    /* ƾ֤��ʼ���롢��ֹ������(������ͷ) */
    ret = pub_com_ChkNoteHead(g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel);
    if (ret)
    {
        sprintf(acErrMsg, "���ƾ֤��ʼ�������ͷ��");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_com_ChkNoteHead(g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel);
    if (ret)
    {
        sprintf(acErrMsg, "���ƾ֤��ֹ�������ͷ��");
        WRITEMSG
        goto ErrExit;
    }

    /* ����Ƿ�������, �����, ��Ҫ�ж���ʼ������ֹ�����Ƿ񹹳����� */
    if (g_sell_way[0] == '1')
    {
        ret = pub_com_ChkPile(g_pub_tx.note_type, g_pub_tx.beg_note_no, \
            g_pub_tx.end_note_no);
        if (ret)
        {
            sprintf(acErrMsg, "���ƾ֤�Ƿ���������");
            WRITEMSG
            goto ErrExit;
        }
    }

    strcpy(sNoteMst.tel,         g_pub_tx.tel);
    strcpy(sNoteMst.br_no,       g_pub_tx.tx_br_no);
    strcpy(sNoteMst.note_type,   g_pub_tx.note_type);
    strcpy(sNoteMst.beg_note_no, g_pub_tx.beg_note_no);
    strcpy(sNoteMst.end_note_no, g_pub_tx.end_note_no);
    sNoteMst.cnt = (int)g_pub_tx.tx_amt1;

    /* ��Աƾ֤��Ч�Լ�� */
    ret = pub_com_ChkNote(cNoteAct, sNoteMst);
    if (ret)
    {
        sprintf(acErrMsg, "����Աƾ֤��");
        WRITEMSG
        goto ErrExit;
    }

    /* �����˺Ų�ѯ�˻���ź��˻�ID */
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
    if (ret == 100)
    {
        sprintf(acErrMsg, "�����ڸ��˺�!!ac_no=[%s]", g_pub_tx.ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "M003");
        goto ErrExit;
    }else if (ret)
    {
        sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel����!ret=[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg, "ac_seqn=[%d]", g_mdm_ac_rel.ac_seqn);
    WRITEMSG

    /* �˻���Ч�Լ�� */
    ret = pub_base_ChkAc(g_pub_tx.ac_no, g_mdm_ac_rel.ac_seqn, cOpenBrNo, &g_pub_tx.ac_id);
    if (ret)
    {
        sprintf(acErrMsg, "�˻���Ч�Լ���");
        WRITEMSG
        goto ErrExit;
    }

    if (strcmp(cOpenBrNo, g_pub_tx.tx_br_no) != 0)
    {
        sprintf(acErrMsg, "���˻����������Ǳ����׻���, �������ڱ���������֧Ʊ!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N087");
        goto ErrExit;
    }
        
    /* ��֤�𻧺�Э�������ܳ���ƾ֤ 20131224 wudawei */
     /* ӡ�������� 20140522 wudawei ע����  |||| 20140618 ����133��Ʒ  wudawei*/
   /* if (strncmp(g_pub_tx.note_type ,"026" ,3) != 0 )
    {
    */
		    if (g_dd_mst.ac_type[0]=='6' || strcmp(g_dd_mst.prdt_no,"131")==0 || strcmp(g_dd_mst.prdt_no,"132")==0 || strcmp(g_dd_mst.prdt_no,"133")==0 
		    	|| strcmp(g_dd_mst.prdt_no,"134")==0 || strcmp(g_dd_mst.prdt_no,"142")==0
		    	|| strcmp(g_dd_mst.prdt_no,"144")==0)
		    {
		        sprintf(acErrMsg, "���˻���������, ���ܹ����ֽ�֧Ʊ");
		        WRITEMSG
		        strcpy(g_pub_tx.reply, "D188");
		        goto ErrExit;
		    }
    /*}*/
    /* modify by wudawei end 20131224  */

    /* ���ֽ�֧Ʊ���ر���***begin */
    if (g_dd_mst.ac_type[0]=='2' && !strcmp(sNoteMst.note_type, "001"))
    {
        sprintf(acErrMsg, "���˻���������, ���ܹ����ֽ�֧Ʊ");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D188");
        goto ErrExit;
    }

    if (strcmp(sNoteMst.note_type, "005") && (g_dd_mst.ac_type[0] == '5' || g_dd_mst.ac_type[0] == '7'))
    {
        sprintf(acErrMsg,"���˻�����, ���ܹ����ֽ�֧Ʊ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D242");
        goto ErrExit;
    }
    /* ���ֽ�֧Ʊ���ر���***end */

    g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn;

    /* �ͻ�ƾ֤��� */
    ret = pub_com_ChkCheqSell(g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
        sNoteMst.note_type, sNoteMst.beg_note_no, sNoteMst.end_note_no);
    if (ret)
    {
        sprintf(acErrMsg, "���ͻ�ƾ֤��");
        WRITEMSG
        goto ErrExit;
    }

    sNoteMstTmp = sNoteMst;

    /* �Թ�Աƾ֤���зֶδ��� */
    ret = pub_com_NoteApart(cNoteAct, sNoteMst);
    if (ret)
    {
        sprintf(acErrMsg, "��Աƾ֤�ֶδ���ʧ��!");
        WRITEMSG
        goto ErrExit;
    }

    ret = wrt_NoteUse_log(g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
        sNoteMst.note_type, sNoteMst.beg_note_no, sNoteMst.end_note_no, \
        atol(sNoteMst.end_note_no + sNoteParm.head_len) -
        atol(sNoteMst.beg_note_no + sNoteParm.head_len) + 1);
    if (ret)
    {
        sprintf(acErrMsg, "��Աƾ֤ʹ�ü��˴�");
        WRITEMSG
        return(-1);
    }

    /* �Ǽǹ�Աƾ֤��ϸ */
    ret = pub_com_RegNoteHst(cNoteAct, sNoteMst, tw_br_no, tw_tel);
    if (ret)
    {
        sprintf(acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��!");
        WRITEMSG
        goto ErrExit;
    }

    /* �Ǽǹ�Աƾ֤��С�� */
    ret = pub_com_RegNoteMin(sNoteMst.br_no, sNoteMst.note_type, \
        sNoteMst.beg_note_no, sNoteMst.tel, cInOut);
    if (ret)
    {
        sprintf(acErrMsg, "�Ǽ�ƾ֤��С�Ŵ�");
        WRITEMSG
        goto ErrExit;
    }

    strcpy(cNoteAct, "K");  /* �ͻ�ƾ֤��Ϊ: K���� */

    /* �Ǽǿͻ�ƾ̨֤�� */
    ret = pub_com_RegCheq(sNoteMstTmp, g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
        g_pub_tx.tx_date, cNoteAct);
    if (ret)
    {
        sprintf(acErrMsg, "�Ǽǿͻ�ƾ̨֤�˴�!");
        WRITEMSG
        goto ErrExit;
    }

    /* �Ǽǿͻ�ƾ̨֤����ϸ */
    sCheqMst.ac_id  = g_pub_tx.ac_id;
    sCheqMst.ac_seq = g_pub_tx.ac_seqn;
    sCheqMst.cnt    = (int)g_pub_tx.tx_amt1;
    strcpy(sCheqMst.br_no,       cOpenBrNo);
    strcpy(sCheqMst.note_type,   g_pub_tx.note_type);
    strcpy(sCheqMst.beg_note_no, g_pub_tx.beg_note_no);
    strcpy(sCheqMst.end_note_no, g_pub_tx.end_note_no);

    ret = pub_com_RegCheqHst(cNoteAct, sCheqMst, sNoteMstHst.tw_br_no, sNoteMstHst.tw_tel);
    if (ret == -1)
    {
        sprintf(acErrMsg, "�Ǽǿͻ�ƾ̨֤����ϸ��!");
        WRITEMSG
        goto ErrExit;
    }

    g_pub_tx.tx_amt1 = 0.00;
    /* ��ˮ�Ǽ�ƾ֤��ƾ֤���� del by wudawei 20131224
    strcpy(g_pub_tx.note_type, "150");
    strcpy(g_pub_tx.beg_note_no, "");
    strcpy(g_pub_tx.end_note_no, "");
		del end 20131224*/
    /* �ǽ�����ˮ */
    ret = pub_ins_trace_log();
    if (ret)
    {
        sprintf(acErrMsg, "����ˮ��־��");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}

/*********************************************************************
* �� �� ��:  init_pub_tx
* ��������:  ��ʼ��g_pub_tx�ṹ, ���Ҵ���Ļȡֵ����g_pub_tx��
*
* ��    ��:  andy
* ʱ    ��:  2004��1��7��
*
* ��    ��:
*     ����:
*
*     ���:
*
*   ����ֵ:  0: �ɹ�  -1: ʧ��
*
* �޸���ʷ:  �����޸��ˡ�ʱ�䡢�޸�����, ��ʱ�䵹��ʽ
*
*********************************************************************/
static int init_pub_tx()
{
    long lNoteCnt;      /* ƾ֤���� */

    pub_base_sysinit();
    memset(g_sell_way, 0x00, sizeof(g_sell_way));

    get_zd_data("0310", g_pub_tx.ac_no);
    get_zd_data("0890", g_pub_tx.note_type);
    get_zd_data("0670", g_sell_way);            /* ���۷�ʽ */
    get_zd_data("0580", g_pub_tx.beg_note_no);
    get_zd_data("0590", g_pub_tx.end_note_no);
    get_zd_long("0520", &lNoteCnt);             /* ��ǰ̨����ȡƾ֤���� */
    g_pub_tx.tx_amt1 = (double)lNoteCnt;
    pub_base_old_acno(g_pub_tx.ac_no);
    set_zd_data("0310", g_pub_tx.ac_no);
    strcpy(g_pub_tx.brf, "ƾ֤����");
    return 0;
}

int wrt_NoteUse_log(ac_id, ac_seqn, note_type, beg_no, end_no, cnt)
    long ac_id,ac_seqn;
    char *note_type;
    char *beg_no;
    char *end_no;
    long cnt;
{
    struct pub_tx v_pub_tx;
    int ret = 0;

    memcpy(&v_pub_tx, &g_pub_tx, sizeof(g_pub_tx));

    g_pub_tx.ct_ind[0] = ' ';
    strcpy(g_pub_tx.ac_no, "702");
    g_pub_tx.ac_seqn = 0;
    g_pub_tx.ac_id   = 0;
    g_pub_tx.svc_ind = 7021;
    strcpy(g_pub_tx.note_type,   note_type);
    strcpy(g_pub_tx.beg_note_no, beg_no);
    strcpy(g_pub_tx.end_note_no, end_no);
    strcpy(g_pub_tx.prdt_code,   "");
    strcpy(g_pub_tx.cur_no,      "01");
    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.hst_ind[0] = '1';
    g_pub_tx.tx_amt1 = (double)cnt;

    ret = pub_ins_trace_log();
    if (ret)
    {
        TRACE
        goto ErrExit;
    }

TRACE
    get_zd_double("0520", &g_pub_tx.tx_amt2);
    set_zd_double("0520", g_pub_tx.tx_amt1);
    memcpy(&g_pub_tx, &v_pub_tx, sizeof(g_pub_tx));
TRACE

    return 0;
ErrExit:
    return 1;
}
