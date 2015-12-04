/*************************************************
* 文 件 名:  spF043.c
* 功能描述： POS 预授权完成撤销(从消费帐户上转出子交易)(外行卡)
*
* 作    者:  rob
* 完成日期： 2003年2月22日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_marchinfo_c.h"
#include "chnl_self_trade_c.h"

int spF043()
{

    int ret;
    char flag;
    long old_date;
    struct chnl_marchinfo_c chnl_marchinfo;
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */
    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* 消费金额 */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_data("0730",chnl_marchinfo.mrchno); /* 商户号 */
   
    get_zd_long("0780",&g_pub_tx.old_trace_no);  /*原终端流水号 */
    get_zd_long("0450",&old_date);   
    get_zd_long("0440",&chnl_self_trade.stra);  /*  原平台流水号*/
    get_zd_long("0440",&chnl_self_trade.dz_date);  /*  平台日期*/
    get_zd_long("0870",&g_pub_tx.auth_code);
    get_zd_char("0670",&flag); /* 信息来源 */
    if (flag != '1')
    {
        /* 本行商户 对商户的检查
        ret = pub_base_Selmrchacno(chnl_marchinfo.mrchno,&chnl_marchinfo);
        if (ret != 0)
        {
            sprintf(acErrMsg,"检查商户信息错误!!");
            WRITEMSG
			strcpy(g_pub_tx.reply, "AT08");
            goto ErrExit;
        }*/
    }

    /* 查询预授权完成的交易流水日记 */
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"stra=%ld and dz_date=%ld",
          g_pub_tx.old_trace_no, old_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询自助设备交易日记错误!!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"该笔流水为非正常状态,不能冲正!!");
        WRITEMSG
        goto ErrExit;
    }
	vtcp_log("chnl_self_trade.tx_amt====[%lf]",chnl_self_trade.tx_amt);
	vtcp_log("g_pub_tx.tx_amt1====[%lf]",g_pub_tx.tx_amt1);

    /* 检查交易金额 */
    if (pub_base_CompDblVal(chnl_self_trade.tx_amt,g_pub_tx.tx_amt1) != 0)
    {
        sprintf(acErrMsg,"冲正交易金额和原交易金额不一致!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }

    /* 检查卡号是否和原来的一致 */
    pub_base_strpack(chnl_self_trade.ac_no1);
    pub_base_strpack(g_pub_tx.crd_no);
    if (strcmp(chnl_self_trade.ac_no1,g_pub_tx.crd_no) != 0)
    {
        sprintf(acErrMsg,"卡号和原来的不一致!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
    /* 检查商户是否一致
    pub_base_strpack(chnl_marchinfo.mrchno);
    pub_base_strpack(chnl_self_trade.spcd);
    if (strcmp(chnl_marchinfo.mrchno,chnl_self_trade.spcd) != 0)
    {
        sprintf(acErrMsg,"商户号不一致!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    } */

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

