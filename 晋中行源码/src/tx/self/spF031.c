/*************************************************
* 文 件 名:  spF031.c
* 功能描述： POS 消费撤销
*            (帐号不是本行的商户)
*
* 作    者:  rob
* 完成日期： 2003年2月21日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "chnl_marchinfo_c.h"

int spF031()
{

    int ret;
    char flag;
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

    get_zd_data("0370",g_pub_tx.crd_no);      /*  卡号 */
    get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */
    get_zd_long("0440",&g_pub_tx.old_trace_no); /* 原终端流水号 */
    get_zd_long("0450",&oldDate); 

    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);
    strcpy(g_pub_tx.ct_ind,"2"); /* 转帐 */

    /* 更新原交易流水的状态 */
    ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply," stra=%ld and dz_date=%ld",
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

    strcpy(chnl_self_trade.stat,"1");
    chnl_self_trade.strb = g_pub_tx.trace_no;

    ret = Chnl_self_trade_Upd_Upd(chnl_self_trade,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Update chnl_self_trade error [%d]",ret);
        WRITEMSG
        Chnl_self_trade_Clo_Upd();
        goto ErrExit;
    }
    Chnl_self_trade_Clo_Upd();

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
    strcpy(chnl_self_trade.ttyp,"2");
    strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
    strcpy(chnl_self_trade.spcd,chnl_marchinfo.mrchno);
    /* 本交易对帐中心流水号 */
    get_zd_long("0520", &chnl_self_trade.stra);

    chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;

    chnl_self_trade.strb = g_pub_tx.old_trace_no;

    strcpy(chnl_self_trade.dz_flag,"0");
    strcpy(chnl_self_trade.ct_ind,"2");
    strcpy(chnl_self_trade.info_source,"1");
    strcpy(chnl_self_trade.deal_flag,"1");
    strcpy(chnl_self_trade.brf_no,"PSCX");
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

	/*  登记交易流水 */
	strcpy(g_pub_tx.brf,"POS撤销!!");

	strcpy(g_pub_tx.ac_no, TESTCARD);
   	g_pub_tx.ac_id= TESTCARDID;
	strcpy(g_pub_tx.ct_ind, "2");

	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!!");
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

