/***************************************************************
* 文 件 名: spA022.c
* 功能描述：内部帐贷方记帐
* 作    者: jack
* 完成日期：2004年1月19日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spA022()
{
	int ret=0;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("1211",g_pub_tx.ac_no);
	pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_double("1218",&g_pub_tx.tx_amt1);
	get_zd_data("1216",g_pub_tx.note_type);
	get_zd_data("1217",g_pub_tx.beg_note_no);
	get_zd_data("121A",g_pub_tx.brf);
	get_zd_data("1215",g_pub_tx.ct_ind);
	get_zd_data("1214",g_pub_tx.cur_no);

	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
	vtcp_log("%s,%d,opn_br_no[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("1214",g_pub_tx.cur_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"内部帐贷方记帐成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"内部帐贷方记帐失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
