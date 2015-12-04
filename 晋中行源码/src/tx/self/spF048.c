/*************************************************
* 文 件 名:  spF048.c
* 功能描述： POS 外行卡预授权完成的处理(从消费帐户上转出子交易)(外行卡)
*
* 作    者:  rob
* 完成日期： 2003年2月26日
*
* 修改记录： 外行卡本行商户要走这个子交易吗
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "chnl_marchinfo_c.h"

int spF048()
{

    int ret;
    char flag;
    struct chnl_self_trade_c chnl_self_trade;
    struct chnl_marchinfo_c chnl_marchinfo;

    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]进入到外行卡预授权完成子交易\n",__FILE__,__LINE__);
    get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */
    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* 消费金额 */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_data("0730",chnl_marchinfo.mrchno); /* 商户号 */
    
    get_zd_long("0520",&g_pub_tx.old_trace_no); /* 终端流水号 */
    get_zd_long("0470",&g_pub_tx.auth_code);       /* 预授权号 */
    get_zd_char("0670",&flag); /* 信息来源 */

    if (flag != '1')
    {
        /* 本行商户 对商户的检查*/
        ret = pub_base_Selmrchacno(chnl_marchinfo.mrchno,&chnl_marchinfo);
        if (ret != 0)
        {
            sprintf(acErrMsg,"检查商户信息错误!!");
            WRITEMSG
			strcpy(g_pub_tx.reply, "AT08");
            goto ErrExit;
        }
    }
    
    /* 根据预授权号查询出预授权的交易日记 */
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,
          "premise_no=%ld and brf_no='PSSQ'", g_pub_tx.auth_code);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询预授权交易日记错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
  
    sprintf(acErrMsg,"预授权的状态stat===[%s]",chnl_self_trade.stat);
        WRITEMSG
    /* 判断预授权的状态是否正常 */
    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"预授权的状态不正常!!!stat===[%s]",chnl_self_trade.stat);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
	vtcp_log("chnl_self_trade.premise_amt=====[%lf]",chnl_self_trade.premise_amt);
    vtcp_log("g_pub_tx.tx_amt1=====[%lf]",g_pub_tx.tx_amt1);

    /* 判断金额是否超出预授权金额 */
    if (pub_base_CompDblVal(chnl_self_trade.premise_amt,g_pub_tx.tx_amt1) < 0)
    {
        sprintf(acErrMsg,"金额超出了预授权金额!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
    /* 检查卡号是否一致 */
    pub_base_strpack(g_pub_tx.crd_no);
    pub_base_strpack(chnl_self_trade.ac_no);
    if (strcmp(g_pub_tx.crd_no,chnl_self_trade.ac_no) != 0)
    {
        sprintf(acErrMsg,"卡号不一致！！！");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
    /* 检查商户是否一致 */
    pub_base_strpack(chnl_marchinfo.mrchno);
    pub_base_strpack(chnl_self_trade.spcd);
    if (strcmp(chnl_marchinfo.mrchno,chnl_self_trade.spcd) != 0)
    {
        sprintf(acErrMsg,"商户号不一致!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

