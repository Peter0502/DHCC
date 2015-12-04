/*************************************************
* 文 件 名:  spF021.c
* 功能描述： POS 消费(从消费帐户上转出子交易)(本行卡)
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
#include "card.h"
int spF021()
{
    	int ret=0;
    	char tty[9];
    	char note_no[21];
    	char in_type[5];
    	char tmpstr[100];

    	if (pub_base_sysinit())
    	{
        	sprintf(acErrMsg,"初始化公用结构错误!!");
        	WRITEMSG
        	goto ErrExit;
    	}

	memset(tty, 0x0, sizeof(tty));
	memset(note_no, 0x0, sizeof(note_no));
	memset(in_type, 0x0, sizeof(in_type));

    	get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */
    	get_zd_data("0800",g_pub_tx.draw_pwd); /* 支取密码 */
    	get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */
	get_zd_data("0380", tty); /* 终端号 */
	get_zd_data("0620", note_no); /* 商户号 */
	get_zd_data("0910", in_type); /* 商户服务方式 */

	sprintf(acErrMsg, " ----卡号: [%s], 密码:, 交易金额: [%lf]", 
		g_pub_tx.crd_no, g_pub_tx.tx_amt1);
	WRITEMSG

    	/* 检查卡的合法性 */
    	if (pub_base_CheckCrdNo())
    	{
       	 	sprintf(acErrMsg,"检查卡的合法性错误!!");
        	WRITEMSG
			strcpy(g_pub_tx.reply, "AT01");
        	goto ErrExit;
    	}

	if ( pub_mobile_pay(in_type, tty, note_no) )
	{
		/* 不可以移动支付 */
    	/* 检查卡的支取方式 */
    	if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    	{
        	sprintf(acErrMsg,"该卡的支取方式错误,必须是凭密码支取!!");
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"AT02");
        	goto ErrExit;
    	}
		
		/*vtcp_log(" g_pub_tx.draw_pwd====[%s]  g_mdm_ac_rel.draw_pwd===[%s]",g_pub_tx.draw_pwd ,g_mdm_ac_rel.draw_pwd);*/
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

    /* 调用取款函数 */
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);

	/********
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
        goto ErrExit;
    }

    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;
    strcpy(g_pub_tx.ct_ind,"2"); * 转帐 *
    g_pub_tx.hst_ind[0] = '1';   * 日间入明细 *

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos消费--帐户取款错误!!");
        WRITEMSG
        goto ErrExit;
    }
	*******/

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

