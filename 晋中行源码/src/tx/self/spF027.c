/*************************************************
* 文 件 名:  spF027.c
* 功能描述： POS 消费撤销
*            (从商户帐户上转到消费帐户上子交易)(本行卡)
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

int spF027()
{

    int ret;
    char marchno[10];
    char tty[9];
    char note_no[21];
    char in_type[5];
    long oldTx_date;
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    memset(tty,0x00,sizeof(tty));
    memset(note_no,0x00,sizeof(note_no));
    memset(in_type,0x00,sizeof(in_type));
    memset(marchno,0x00,sizeof(marchno));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */
    get_zd_data("0800",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */
    set_zd_double("1013",g_pub_tx.tx_amt1);
    sprintf(acErrMsg,"传入金额[%lf]",g_pub_tx.tx_amt1);
    WRITEMSG
/*
	memset(cTrace_no, 0, sizeof(cTrace_no));
    get_zd_data("0610", cTrace_no);  原终端流水号 
    vtcp_log("原终端流水号 =====[%s] ", cTrace_no);
	
	g_pub_tx.old_trace_no = atol(cTrace_no);
    vtcp_log("原终端流水号 =====[%d] ", g_pub_tx.old_trace_no);
*/

	/*New:  Changed by ChenMing 2007.07.27. */	
	get_zd_long("0440", &g_pub_tx.old_trace_no);  /* 被冲交易的原平台流水 */
	get_zd_long("0450", &oldTx_date);  /* 被冲交易的原交易日期 */
	
	vtcp_log("被冲交易的原平台流水号 =====[%d] ", g_pub_tx.old_trace_no);
	vtcp_log(" 被冲交易的原交易日期 =====[%d] ", oldTx_date);

    	/* 检查卡的合法性 */
    	if (pub_base_CheckCrdNo())
    	{
       	 	sprintf(acErrMsg,"检查卡的合法性错误!!");
        	WRITEMSG
        	goto ErrExit;
    	}

	if ( pub_mobile_pay(in_type, tty, note_no) )
	{ 
		/* 不是移动支付 */
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
	}

    /* 查询自助设备交易日记 */
/*    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"trace_no=%ld",
          g_pub_tx.old_trace_no);
*/
    ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"stra=%ld and dz_date=%ld ", g_pub_tx.old_trace_no, oldTx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询自助设备交易日记表错误!! [%d]",ret);
        WRITEMSG
        sprintf(acErrMsg,"trace_no=[%ld]",g_pub_tx.old_trace_no);
        WRITEMSG
        goto ErrExit;
    }

    /* 检查该笔流水是否正常 */
    if (chnl_self_trade.stat[0] != '0')
    {
        sprintf(acErrMsg,"该笔流水不允许做撤销!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* 检查输入的卡号和原交易流水中的是否正常 */
    pub_base_strpack(chnl_self_trade.ac_no);
    pub_base_strpack(g_pub_tx.crd_no);
    if (strcmp(chnl_self_trade.ac_no,g_pub_tx.crd_no) != 0)
    {
        sprintf(acErrMsg,"输入的卡号和原交易流水中的卡号不一致!!");
        WRITEMSG
        sprintf(acErrMsg,"chnl_self_trade: [%s], g_pub_tx: [%s]",
               chnl_self_trade.ac_no,g_pub_tx.crd_no);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT01");
        goto ErrExit;
    }

    /* 检查该笔流水是否可以做撤销 
       不能对已冲正的交易进行取消；不能对已撤销的交易再次撤销。*/
    pub_base_strpack(chnl_self_trade.brf_no);
    if (strcmp(chnl_self_trade.brf_no,"PSXF") != 0)
    {
       sprintf(acErrMsg,"该笔流水不能做撤销交易 [%s]",chnl_self_trade.brf_no);
       WRITEMSG
	   strcpy(g_pub_tx.reply, "AT13");
       goto ErrExit;
    }
   
    /* 检查该笔流水是否是当日交易 */
    if (chnl_self_trade.tx_date != g_pub_tx.tx_date)
    {
        sprintf(acErrMsg,"隔日不能做撤销交易!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* 检查该笔流水的金额 Added by ChenMing 2007.07.28. */
    if (chnl_self_trade.tx_amt < g_pub_tx.tx_amt1)
    {
        sprintf(acErrMsg,"撤消金额不能超过原交易金额!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }

    /* 检查商户是否相同 *
    pub_base_strpack(chnl_self_trade.spcd);
    pub_base_strpack(marchno);
    if (strcmp(marchno,chnl_self_trade.spcd) != 0)
    {
        sprintf(acErrMsg,"商户号不同,不能做撤销交易!!");
        WRITEMSG
        sprintf(acErrMsg," [%s] ?= [%s] ",marchno,chnl_self_trade.spcd);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT13");
        goto ErrExit;
    }   
    
    * 检查是否是同一台POS *
    pub_base_strpack(g_pub_tx.tty);
    pub_base_strpack(chnl_self_trade.ttyn);
    if (strcmp(chnl_self_trade.ttyn,g_pub_tx.tty) != 0)
    {
        sprintf(acErrMsg,"POS设备号不同,不能做撤销交易!!");
        WRITEMSG
        goto ErrExit;
    }
	*/

    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);

    /* 调用存款函数 
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
    strcpy(g_pub_tx.ct_ind,"2"); * 转帐 *
    g_pub_tx.hst_ind[0] = '1';  * 日间入明细 *

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos消费撤销--帐户存款错误!!");
        WRITEMSG
        goto ErrExit;
    }*/

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

