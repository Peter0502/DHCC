/*************************************************
* �� �� ��: spF407.c
* ������������¼�ɿ���
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

spF407()  
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

    sprintf(acErrMsg,"SSSS ac_no= [%s] ",g_pub_tx.crd_no);
    WRITEMSG

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
