/*************************************************
* �� �� ��: spF409.c
* �����������ж���һ��ɿ���Ƿ�˰�ʻ�֧��
*
* ��    ��: mike
* �������: 2004��03��28��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"

spF409()  
{ 	
    char flag[2];

    get_zd_data("0680",flag);

    if (flag[0] == '0')
    {
        sprintf(acErrMsg,"תһ��ɿ�Ĵ���!!!");
        WRITEMSG
        goto OkExit1;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"ת��˰�ʻ�֧���Ĵ���OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    strcpy(g_pub_tx.reply,"0001");
    sprintf(acErrMsg,"תһ��ɿ�Ĵ���OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
