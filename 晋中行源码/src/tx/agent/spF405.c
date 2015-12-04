/*************************************************
* �� �� ��: spF405.c
* �����������˸����˸�(������������ʻ��򸶿����ʻ�)
*
* ��    ��: mike
* �������: 2004��03��01��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/

#define EXTERN
#include "public.h"

spF405()  
{ 	
    int ret ;

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0260",g_pub_tx.name);  /* ���������� */
    get_zd_data("0310",g_pub_tx.crd_no);  /* �������ʺ� */
    get_zd_double("0420",&g_pub_tx.tx_amt1); /* ���׽�� */

    /* ��鿨�ĺϷ��� */
    if (strcmp(g_pub_tx.crd_no,"") != 0  || g_pub_tx.crd_no[0] != '\0')
    {
        if (pub_base_CheckCrdNo())
        {
            sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"F003");
            goto ErrExit;
        }
    }
    else
    {
        /* ���ʺŸ�ֵ(����������ڲ��ʻ�) */
    }

    /* ��ѯ�����ɷѼ�¼ ����� */

    /* ȡ�� */
    strcpy(g_pub_tx.cur_no,"01"); /* ����� */

    /********
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
        goto ErrExit;
    }
    *******/

    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn = 0;
    g_pub_tx.hst_ind[0] = '1';   /* �ռ�����ϸ */

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"�����ɷѿͻ�ȡ�����!!");
        WRITEMSG
        goto ErrExit;
    }

    /* �Ǽǲ�����˰������ɵǼǲ� */

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
