/*************************************************
* �� �� ��: spF402.c
* �������������������ʺ�(����)
*
* ��    ��: mike
* �������: 2004��02��28��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/

#define EXTERN
#include "public.h"

spF402()  
{ 	
    int ret;

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0250",g_pub_tx.name);  /* ���������� */
    get_zd_data("0300",g_pub_tx.crd_no);  /* �������ʺ� */
    get_zd_data("0790",g_pub_tx.draw_pwd);  /* �ʻ����� */
    get_zd_double("0420",&g_pub_tx.tx_amt1); /* ���׽�� */

    /* ��鿨�ĺϷ��� */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"F003");
        goto ErrExit;
    }

    /* ��鿨��֧ȡ��ʽ */
    if (g_mdm_ac_rel.draw_pwd_yn[0] == 'Y')
    {
        /* ���֧ȡ��ʽ�ĺϷ��� */
        ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
                      g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
        if (ret != 0)
        {
            sprintf(acErrMsg,"����֧ȡ�������!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"F004");
            goto ErrExit;
        }
    }

    /* ȡ�� */
    strcpy(g_pub_tx.cur_no,"01"); /* ����� */
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
        goto ErrExit;
    }

    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;
    strcpy(g_pub_tx.ct_ind,"2"); /* ת�� */
    g_pub_tx.hst_ind[0] = '1';   /* �ռ�����ϸ */

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"�����ɷѿͻ�ȡ�����!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
