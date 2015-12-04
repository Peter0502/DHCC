/***************************************************************
* 文 件 名:     spD303.c
* 功能描述：	个人理财登记子交易
* 作    者:     jack
* 完成日期：    2004年1月17日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD303()
{
	char	prot_type[4];
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
	get_zd_data("0230",prot_type);
	get_zd_data("0370",g_pub_tx.crd_no);
	get_zd_double("0390",&g_pub_tx.tx_amt1);
	strcpy(g_pub_tx.ac_no,g_pub_tx.crd_no);
	g_pub_tx.ac_seqn=1;
	get_zd_data("0670",g_pub_tx.draw_pwd_yn);
	get_zd_data("0790",g_pub_tx.draw_pwd);
	get_zd_data("0680",g_pub_tx.draw_id_yn);
	get_zd_data("0690",g_pub_tx.id_type);
	get_zd_data("0620",g_pub_tx.id_no);

	g_reply_int=pub_base_prot(prot_type,1);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"理财协定登记主控出错!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"个人理财登记成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"个人理财登记失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
