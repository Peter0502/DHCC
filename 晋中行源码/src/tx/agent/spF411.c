/*************************************************
* �� �� ��: spF411.c
* ���������������տ����ʺŲ��Ǳ����ʺ�(���˴���)
*
* ��    ��: mike
* �������: 2004��03��29��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"

spF411()  
{ 	

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�տ����ʺŹ��˴���ɹ�OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    strcpy(g_pub_tx.reply,"0001");
    sprintf(acErrMsg,"ת���пͻ��Ĵ���OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
