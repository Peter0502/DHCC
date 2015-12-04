/*************************************************
* 文 件 名: spF409.c
* 功能描述：判断是一般缴款户还是非税帐户支出
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

spF409()  
{ 	
    char flag[2];

    get_zd_data("0680",flag);

    if (flag[0] == '0')
    {
        sprintf(acErrMsg,"转一般缴款户的处理!!!");
        WRITEMSG
        goto OkExit1;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"转非税帐户支出的处理OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    strcpy(g_pub_tx.reply,"0001");
    sprintf(acErrMsg,"转一般缴款户的处理OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
