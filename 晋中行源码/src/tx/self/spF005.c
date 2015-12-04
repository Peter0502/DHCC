/*************************************************
* 文 件 名:  spF005.c
* 功能描述： ATM 取款(他行卡在本行ATM上取款)
*
* 作    者:  rob
* 完成日期： 2003年2月17日
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
#include "card.h" /* add by LiuHuafeng 20070127 */

int spF005()
{

    int ret=0;
    char crd_no[25];
    char sb_flag[2];
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    memset(crd_no, 0x0, sizeof(crd_no));
    memset(sb_flag, 0x0, sizeof(sb_flag));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */
    get_zd_data("0370",g_pub_tx.ac_no); /* 卡号 */
    get_zd_double("0400",&g_pub_tx.tx_amt1); /* 交易金额 */

    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);
    /* strcpy(g_pub_tx.ct_ind,"1"); */
    get_zd_data("102K", g_pub_tx.ct_ind);
    strcpy(g_pub_tx.opn_br_no, g_pub_tx.tx_br_no);

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
    get_zd_long("0520", &chnl_self_trade.stra);
    get_zd_long("0450", &chnl_self_trade.dz_date);  /* 平台日期*/

    chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;

    strcpy(chnl_self_trade.dz_flag,"0");
    strcpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind);
    strcpy(chnl_self_trade.info_source,"2");

	get_zd_data("0300", crd_no);
	/* rem by LiuHuafeng 20070127 
	if (strncmp(crd_no, "694302", 6) == 0) 
	** replace by next line ************/
	if ((strncmp(crd_no, CARDHEADCODE, 6) == 0  || strncmp(crd_no, ICCARDHEADCODE, 6) == 0 ) && memcmp(crd_no+6,CARD_UNION_AREA,2) ==0 ) 
	{ 
		get_zd_data("0300", chnl_self_trade.ac_no); 
	} 

    /* strcpy(chnl_self_trade.deal_flag,"1"); */
	/* modify by rob at 20031112 */ 
	get_zd_data("0680", sb_flag); 
	if ( chnl_self_trade.info_source[0] == '1' ) 
	{ 
		if ( sb_flag[0] == '1' ) /* 他行ATM */ 
			strcpy(chnl_self_trade.deal_flag, "3"); 
		else    /* 六市设备 */ 
			strcpy(chnl_self_trade.deal_flag, "2"); 
	} 
	else 
	{ 
		strcpy(chnl_self_trade.deal_flag, "1"); /* 本行设备 */ 
	}

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
		strcpy(g_pub_tx.reply, "AT07");
        goto ErrExit;
    }

    /*  登记交易流水 */
    strcpy(g_pub_tx.brf,"ATM取款!!");

	strcpy(g_pub_tx.ac_no, TESTCARD);
   g_pub_tx.ac_id= TESTCARDID;
	/* strcpy(g_pub_tx.ct_ind, "1");*/
	strcpy(g_pub_tx.add_ind, "1");

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

