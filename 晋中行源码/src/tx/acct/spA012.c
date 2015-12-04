/***************************************************************
* 文 件 名:     spA012.c
* 功能描述：	开账号
*
* 作    者:     jack
* 完成日期：    2004年1月18日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spA012()
{
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	get_zd_data("1039",g_pub_tx.ac_no);	/*显示账号*/
	vtcp_log("[%s]",g_pub_tx.ac_no);
	get_zd_data("0230",g_pub_tx.prdt_code);
	get_zd_data("0210",g_pub_tx.cur_no);
	get_zd_data("0250",g_pub_tx.name);

	strcpy(g_pub_tx.ac_type,"D");

	get_zd_data("0710",g_pub_tx.intst_type);

	strcpy(g_pub_tx.add_ind,"1");
	g_pub_tx.tx_amt1 = 0.00;

	if( pub_acct_opn_ac() )
	{
		sprintf(acErrMsg,"开户错误!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("0320",g_pub_tx.ac_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"开内部账户成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"开内部账户失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

