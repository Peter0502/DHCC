/*************************************************
* 文 件 名: spF410.c
* 功能描述：处理付款人帐号不是本行帐号(挂账处理)
*
* 作    者: mike
* 完成日期: 2004年03月28日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"

spF410()  
{ 	

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"付款人帐号转帐处理成功OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
