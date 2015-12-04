/*************************************************
* 文 件 名: spF411.c
* 功能描述：处理收款人帐号不是本行帐号(挂账处理)
*
* 作    者: mike
* 完成日期: 2004年03月29日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"

spF411()  
{ 	

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"收款人帐号挂账处理成功OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    strcpy(g_pub_tx.reply,"0001");
    sprintf(acErrMsg,"转本行客户的处理OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
