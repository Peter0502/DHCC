/*************************************************
* 文 件 名:  spF050.c
* 功能描述： POS 消费冲正
*
* 作    者:  rob
* 完成日期： 2003年6月21日
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

int spF050()
{

    int ret;
    long oldTrace_no=0;
    long oldTx_date=0;
    char flag[2];
    struct chnl_tradeadd_c chnl_tradeadd;
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_tradeadd,0x0,sizeof(struct chnl_tradeadd_c));
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    memset(flag,0x00,sizeof(flag));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */
    get_zd_data("0800",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_double("0400",&g_pub_tx.tx_amt1); /* 交易金额 */
    
    get_zd_long("0440", &oldTrace_no);/* 被冲交易原平台流水 */ 
    get_zd_long("0450", &oldTx_date); /* 冲正日期 */
    get_zd_long("0520", &g_pub_tx.trace_no); /* 平台流水 */
    /* set_zd_long("0440", trace_no); */ 

    set_zd_data("0210","01");  /* 币种 */
    strcpy(g_pub_tx.cur_no, "01");



	sprintf(acErrMsg,"------->冲正卡号: [%s], 金额: [%lf], 日期: [%ld], 流水号: \
	          [%ld]",  g_pub_tx.crd_no, g_pub_tx.tx_amt1, oldTx_date,	oldTrace_no);
	WRITEMSG


    /* 检查卡的合法性 *
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"检查卡的合法性错误!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT01");
        goto ErrExit;
    }

    * 检查卡的支取方式 *
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
        sprintf(acErrMsg,"该卡的支取方式错误,必须是凭密码支取!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT02");
        goto ErrExit;
    }

    * 检查支取方式的合法性 *
    ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
                      g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    if (ret != 0)
    {
        sprintf(acErrMsg,"卡的支取密码错误!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT04");
        goto ErrExit;
    }
	***/

    /* 更新原交易流水的状态为非正常状态
	ret = sql_execute("update chnl_self_trade set stat='2' where \
			tx_date=%ld and trace_no=%ld and ac_no='%s' and tx_amt=%lf",
			oldTx_date, trace_no, g_pub_tx.crd_no, g_pub_tx.tx_amt1);
	if (ret != 0)
	{
		sprintf(acErrMsg,"修改自助设备的状态错误!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
		goto ErrExit;
	}	 */


    /* 更新原交易流水的状态为非正常状态
	并获取原记录信息    Added by ChenMing  2007.07.28.*/

    ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply," dz_date=%ld and stra=%ld and ac_no='%s' and tx_amt=%lf", 
    oldTx_date, oldTrace_no, g_pub_tx.crd_no, g_pub_tx.tx_amt1);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Declare error [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    memset(&chnl_self_trade, 0, sizeof(chnl_self_trade));   
    ret = Chnl_self_trade_Fet_Upd(&chnl_self_trade,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch error [%d] ",ret);
        WRITEMSG
        Chnl_self_trade_Clo_Upd();
        goto ErrExit;
    }

	chnl_self_trade.stat[0] = '1';
	chnl_self_trade.strb = g_pub_tx.trace_no;	

    sprintf(acErrMsg," chnl_self_trade.trace_no=== [%ld] ",chnl_self_trade.trace_no);
    WRITEMSG
		
	set_zd_long("0440", chnl_self_trade.trace_no);  /* 被冲交易的原后台流水 */
	

    ret = Chnl_self_trade_Upd_Upd(chnl_self_trade, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Update error [%d] ",ret);
        WRITEMSG
        Chnl_self_trade_Clo_Upd();
        goto ErrExit;
    }
    Chnl_self_trade_Clo_Upd();
	

    /* 给自助设备交易日记结构赋值 */
    memset(&chnl_self_trade, 0, sizeof(chnl_self_trade));   	
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
	/* 本交易对帐中心流水号 */ 
	get_zd_long("0520", &chnl_self_trade.stra);
	get_zd_long("0530", &chnl_self_trade.strb);

	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1; 
	chnl_self_trade.sl_sxamt = 0.00; 
	chnl_self_trade.fk_sxamt = 0.00;

	strcpy(chnl_self_trade.dz_flag,"0"); 
	strcpy(chnl_self_trade.ct_ind,"2"); 
	strcpy(chnl_self_trade.info_source,"0"); 
	strcpy(chnl_self_trade.deal_flag,"1"); 
	strcpy(chnl_self_trade.brf_no,"PSCZ"); 
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

   	/* 查询活期帐户主文件 */ 
	strcpy(g_pub_tx.cur_no,"01"); 
	ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&g_dd_mst); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"查询活期帐户主文件错误!!!"); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"AT06"); 
		goto ErrExit; 
	} 
	
	g_dd_mst.bal = g_dd_mst.bal + g_pub_tx.tx_amt1;
	set_zd_double("0420",g_dd_mst.bal);   /* 帐户余额 */ 
	set_zd_data("0270",g_dd_mst.name);    /* 户名 */


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


