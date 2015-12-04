/*************************************************
* 文 件 名:  spF011.c
* 功能描述： ATM 取款(处理收手续费交易)
*
* 作    者:  rob
* 完成日期： 2003年2月24日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"

int spF011()
{

    int ret;
    struct chnl_tradeadd_c chnl_tradeadd;
    struct chnl_self_trade_c chnl_self_trade;


    memset(&chnl_tradeadd,0x0,sizeof(struct chnl_tradeadd_c));
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_double("0390",&g_pub_tx.tx_amt1); /* 取款金额 */


    sprintf(acErrMsg,"crd_no=[%s]",g_pub_tx.crd_no);
    WRITEMSG

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

    /* 检查单次取款金额是否超限 *
     * 检查当天累计取款金额是否超限 */
    ret = pub_base_Checktradelimit('0');
    if (ret != 0)
    {
        sprintf(acErrMsg,"检查atm取款的交易限制出错!!!");
        WRITEMSG
        goto ErrExit;
    }

    /* 调用取款函数 */
    strcpy(g_pub_tx.cur_no,"01");
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }

    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;
    strcpy(g_pub_tx.ct_ind,"1"); /* 现金 */
    g_pub_tx.hst_ind[0] = '1';   /* 日间入明细 */

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"atm取款--卡帐户取款错误!!");
        WRITEMSG
        goto ErrExit;
    }

    /* 给自助设备交易日记结构赋值 */
    chnl_self_trade.tx_date = g_pub_tx.tx_date;
    chnl_self_trade.tx_time = g_pub_tx.tx_time;
    chnl_self_trade.trace_no = g_pub_tx.trace_no;
    strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);
    strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);
    strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
    strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(chnl_self_trade.tel,g_pub_tx.tel);
    strcpy(chnl_self_trade.ttyp,"1");
    strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
    /* 本交易对帐中心流水号 */

    chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;

    strcpy(chnl_self_trade.dz_flag,"0");
    strcpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind);
    strcpy(chnl_self_trade.info_source,"0");
    strcpy(chnl_self_trade.deal_flag,"1");
    strcpy(chnl_self_trade.brf_no,"ATMQ");
    strcpy(chnl_self_trade.add_ind,"0");
    strcpy(chnl_self_trade.batch_flag,"0");
    strcpy(chnl_self_trade.stat,"0");


    /* 登记自助设备交易日记 */
    ret = pub_base_Insertselftrade(chnl_self_trade);
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记自助设备交易日记错误!!");
        WRITEMSG
        goto ErrExit;
    }

    /* 更新交易记录累计控制表 */
    ret = Chnl_tradeadd_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and ac_no = '%s' \
          and tx_type = '1'",g_pub_tx.tx_date,g_pub_tx.crd_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Declare error [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
   
    ret = Chnl_tradeadd_Fet_Upd(&chnl_tradeadd,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch error [%d] ",ret);
        WRITEMSG
        Chnl_tradeadd_Clo_Upd();
        goto ErrExit;
    }

    chnl_tradeadd.add_amt = chnl_tradeadd.add_amt + g_pub_tx.tx_amt1;
    chnl_tradeadd.add_qk_cnt = chnl_tradeadd.add_qk_cnt + 1;

    ret = Chnl_tradeadd_Upd_Upd(chnl_tradeadd,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Update error [%d] ",ret);
        WRITEMSG
        Chnl_tradeadd_Clo_Upd();
        goto ErrExit;
    }
    Chnl_tradeadd_Clo_Upd();


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

