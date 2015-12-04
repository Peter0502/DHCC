/*************************************************
* 文 件 名: spF404.c
* 功能描述：退付书退付(处理收款人帐号)
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

spF404()  
{ 	
    int ret;

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0250",g_pub_tx.name);  /* 收款人名称 */
    get_zd_data("0300",g_pub_tx.crd_no);  /* 收款人帐号 */
    get_zd_double("0420",&g_pub_tx.tx_amt1); /* 交易金额 */

    /* 检查财政缴费记录 做检查 */

    /* 检查卡的合法性 */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"检查卡的合法性错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"F003");
        goto ErrExit;
    }

    /* 存款 */
    strcpy(g_pub_tx.cur_no,"01"); /* 人民币 */
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
        goto ErrExit;
    }

    g_pub_tx.add_ind[0] = '1';
    g_pub_tx.ac_id = 0;
    strcpy(g_pub_tx.ct_ind,"2"); /* 转帐 */
    g_pub_tx.hst_ind[0] = '1';   /* 日间入明细 */

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"财政缴费客户取款错误!!");
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
