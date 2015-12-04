/*************************************************
* 文 件 名: spF407.c
* 功能描述：补录缴款书
*
* 作    者: mike
* 完成日期: 2004年03月01日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/

#define EXTERN
#include "public.h"

spF407()  
{ 	
    int ret ;

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0260",g_pub_tx.name);  /* 付款人名称 */
    get_zd_data("0310",g_pub_tx.crd_no);  /* 付款人帐号 */
    get_zd_double("0420",&g_pub_tx.tx_amt1); /* 交易金额 */

    sprintf(acErrMsg,"SSSS ac_no= [%s] ",g_pub_tx.crd_no);
    WRITEMSG

    /* 登记财政非税收入代缴登记簿 */

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
