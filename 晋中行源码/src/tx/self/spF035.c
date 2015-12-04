/*************************************************
* 文 件 名:  spF035.c
* 功能描述： POS 消费撤销冲正
*
* 作    者:  rob
* 完成日期： 2003年2月20日
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

int spF035()
{

    int ret;
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
    get_zd_long("0520",&g_pub_tx.trace_no); /* 终端流水号 */
    get_zd_long("0780",&g_pub_tx.old_trace_no); /* 原终端流水号 */


    /* 查询自助设备交易日记(原交易) */
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"stra='%ld'",
          g_pub_tx.old_trace_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询自助设备交易日记表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 检查该笔流水是否正常 非正常状态(已冲正) */
    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"该笔流水不允许做撤销!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* 检查该笔流水是否是撤销交易 */
    if (strcmp(chnl_self_trade.brf_no,"PSCX") != 0)
    {
        sprintf(acErrMsg,"该笔流水不是撤销交易!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }
    
    /* 检查商户号是否相同 */
    if (strcmp(chnl_self_trade.spcd,chnl_marchinfo.mrchno) != 0)
    {
        sprintf(acErrMsg,"输入的商户号和上笔流水中的不一致!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* 检查交易金额是否相同 */
    if (pub_base_CompDblVal(g_pub_tx.tx_amt1,chnl_self_trade.tx_amt) != 0)
    {
        sprintf(acErrMsg,"交易金额和流水中的金额不一致!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    if (strncmp(g_pub_tx.crd_no,CRD_HEAD,9) != 0)
    {
        sprintf(acErrMsg,"该卡不是本行的卡!!!");
        WRITEMSG
        goto NOUSE;
    }

    strcpy(chnl_self_trade.ac_no1,chnl_self_trade.ac_no);

    /* 检查卡的合法性 */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"检查卡的合法性错误!!");
        WRITEMSG
        goto ErrExit;
    }

    /* 检查卡的支取方式 */
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
        sprintf(acErrMsg,"该卡的支取方式错误,必须是凭密码支取!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT02");
        goto ErrExit;
    }

    /* 检查支取方式的合法性 */
    ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
                      g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    if (ret != 0)
    {
        sprintf(acErrMsg,"卡的支取密码错误!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT04");
        goto ErrExit;
    }

NOUSE:
    /* 检查卡号和流水中的是否一致 */
    if (strcmp(g_pub_tx.crd_no,chnl_self_trade.ac_no1) != 0)
    {
        sprintf(acErrMsg,"卡号和流水中的不一致!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT13");
        goto ErrExit;
    }

    /* 调用撤销函数前的处理 */

    /* 调用后台当天撤销冲正函数 */   
    
    /* 更新原自助设备交易日记的状态为非正常状态  消费撤销交易流水*/

    /* 更新原自助设备交易日记的状态为正常状态  消费交易流水*/

    /* 给自助设备交易日记结构赋值 */
    
    /* 登记自助设备交易日记 */
    ret = pub_base_Insertselftrade(chnl_self_trade);
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记自助设备交易日记错误!!");
        WRITEMSG
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

