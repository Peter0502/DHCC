/***************************************************************
* 文 件 名:     spA015.c
* 功能描述：	现金记帐(借贷)
* 作    者:     jack
* 完成日期：    2004年1月19日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spA015()
{
	char cJzlx[2];
	memset(cJzlx,0x0,sizeof(cJzlx));

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0670",cJzlx);	/*记帐类型*/
	if (cJzlx[0] == '1')
	{
		get_zd_double("0420",&g_pub_tx.tx_amt1);

		strcpy(g_pub_tx.add_ind,"0");/*借贷标志*/
	}
	else if (cJzlx[0] == '2')
	{
		get_zd_double("0420",&g_pub_tx.tx_amt1);

		strcpy(g_pub_tx.add_ind,"1");/*借贷标志*/
	}
	else
	{
		sprintf(acErrMsg,"记帐类型出错!");
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int=pub_acct_cash();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用现金收付主控失败!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"现金记帐成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"现金记帐失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
