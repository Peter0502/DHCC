/*************************************************
* 文 件 名:  spF033.c
* 功能描述： POS 消费退货
*
* 作    者:  rob
* 完成日期： 2003年2月19日
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

int spF033()
{

    int ret;
    long oldDate;

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
    get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* 消费金额 */
    get_zd_data("0730",chnl_marchinfo.mrchno); /* 商户号 */
    /*get_zd_long("0780",&g_pub_tx.old_trace_no);  原终端流水号 */
    get_zd_long("0440",&g_pub_tx.old_trace_no);  /* 原交易平台流水号  */
    get_zd_long("0450",&oldDate);  /* 原交易日期 */

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

    /* 检查商户的状态  Remmed by ChenMing For Test
    ret = pub_base_Selmrchacno(chnl_marchinfo.mrchno,&chnl_marchinfo);
    if (ret != 0)
    {
        sprintf(acErrMsg,"检查商户的状态错误!!!");
        WRITEMSG
        goto ErrExit;
    }*/

    /* 查询自助设备交易日记(原交易) */
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"stra=%ld and dz_date=%ld",
          g_pub_tx.old_trace_no, oldDate);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询自助设备交易日记表错误!! [%d]",ret);
        WRITEMSG
        sprintf(acErrMsg,"trace_no= [%ld]",g_pub_tx.old_trace_no);
        WRITEMSG
        goto ErrExit;
    }

    /* 检查该笔流水是否正常 非正常状态(已被取消 被冲正或已退货) */
    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"该笔流水不允许做撤销!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* 检查原交易日期和输入的是否一致
    if (oldDate != chnl_self_trade.tx_date)
    {
        sprintf(acErrMsg,"原交易日期和原流水号的交易日期不一致!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
     }*/

    /* 检查卡号和原流水里的卡号是否一致 */
    pub_base_strpack(g_pub_tx.crd_no);
    pub_base_strpack(chnl_self_trade.ac_no);
    if (strcmp(g_pub_tx.crd_no,chnl_self_trade.ac_no) != 0)
    {
        sprintf(acErrMsg,"输入的卡号和原交易卡号不一致!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }
   
    /* 检查原交易日期是否正确 
    if (oldDate != chnl_self_trade.tx_date)
    {
        sprintf(acErrMsg,"原交易日期不一致!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }*/

    /* 检查是否超出一个月期限 */
    if (g_pub_tx.tx_date - oldDate > 30)
    {
        sprintf(acErrMsg,"退货超出了一个月!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* 检查交易金额是否超出原发生额 */
    if (g_pub_tx.tx_amt1 > chnl_self_trade.tx_amt)
    {
        sprintf(acErrMsg,"退货金额超出了原发生额!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }
    
    /* 检查POS终端号是否一致 */
    pub_base_strpack(g_pub_tx.tty);
    pub_base_strpack(chnl_self_trade.ttyn);
    if (strcmp(g_pub_tx.tty,chnl_self_trade.ttyn) != 0)
    {
        sprintf(acErrMsg,"退货只能在同一台POS终端上!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* 检查商户号是否一致 Remmed by ChenMing for Test
    pub_base_strpack(chnl_marchinfo.mrchno);
    pub_base_strpack(chnl_self_trade.spcd);
    if (strcmp(chnl_marchinfo.mrchno,chnl_self_trade.spcd) != 0)
    {
        sprintf(acErrMsg,"商户号不一致不能退货!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }*/
    
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }

    g_pub_tx.add_ind[0] = '1';
    g_pub_tx.ac_id = 0;
    strcpy(g_pub_tx.ct_ind,"2"); /* 转帐 */
    g_pub_tx.hst_ind[0] = '1';   /* 日间入明细 */

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos退货--客户的卡存款错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    memset(g_pub_tx.crd_no,0x0,sizeof(g_pub_tx.crd_no));
    strcpy(g_pub_tx.ac_no,chnl_marchinfo.mrchacno);
    
    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.ct_ind,"2"); /* 转帐 */
    g_pub_tx.hst_ind[0] = '1';   /* 日间入明细 */
    g_pub_tx.ac_id_type[0] = '9';

sprintf(acErrMsg,"ac_no=[%s],ac_id=[%ld],ac_seqn=[%ld]",
    g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
WRITEMSG

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos退货--商户帐户取款错误!!");
        WRITEMSG
        goto ErrExit;
    }

    /* 更新原自助设备交易日记的发生额 */
    ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply,"stra=%ld and dz_date=%ld",
           g_pub_tx.old_trace_no, oldDate);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询原POS消费的自助设备交易日记错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Chnl_self_trade_Fet_Upd(&chnl_self_trade,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch chnl_self_trade error [%d]",ret);
        WRITEMSG
        Chnl_self_trade_Clo_Upd();
        goto ErrExit;
    }

    chnl_self_trade.tx_amt = chnl_self_trade.tx_amt - g_pub_tx.tx_amt1;
    strcpy(chnl_self_trade.stat,"1");
  get_zd_long("0520",&chnl_self_trade.strb);   /* */	

    ret = Chnl_self_trade_Upd_Upd(chnl_self_trade, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Update chnl_self_trade error [%d]",ret);
        WRITEMSG
        Chnl_self_trade_Clo_Upd();
        goto ErrExit;
    }
    Chnl_self_trade_Clo_Upd();


    /* 给自助设备交易日记结构赋值 */
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    chnl_self_trade.tx_date = g_pub_tx.tx_date;
    chnl_self_trade.tx_time = g_pub_tx.tx_time;
    chnl_self_trade.trace_no = g_pub_tx.trace_no;
    strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);

    get_zd_data("0370",g_pub_tx.ac_no);
    strcpy(chnl_self_trade.ac_no,g_pub_tx.ac_no);

    strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
    strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(chnl_self_trade.tel,g_pub_tx.tel);
    strcpy(chnl_self_trade.ttyp,"2");
    strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
    strcpy(chnl_self_trade.spcd,chnl_marchinfo.mrchno);
    /* 本交易对帐中心流水号 */

    chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;

/*
    chnl_self_trade.strb = g_pub_tx.old_trace_no;
    get_zd_long("0770",&g_pub_tx.old_trace_no);
    chnl_self_trade.stra = g_pub_tx.old_trace_no;
*/
    get_zd_long("0520",&chnl_self_trade.stra);
    chnl_self_trade.strb = g_pub_tx.old_trace_no;
    strcpy(chnl_self_trade.dz_flag,"0");
    strcpy(chnl_self_trade.ct_ind,"2");
    strcpy(chnl_self_trade.info_source,"0");
    strcpy(chnl_self_trade.deal_flag,"1");
    strcpy(chnl_self_trade.brf_no,"PSTH");
    strcpy(chnl_self_trade.add_ind,"1");
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


