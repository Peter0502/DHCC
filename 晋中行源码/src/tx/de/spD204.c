/***************************************************************
* 文 件 名: spD204.c
* 功能描述：销介质
* 作    者: jack
* 完成日期：2004年1月17日
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

spD204()
{
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
	get_zd_data("1021",g_pub_tx.ac_no);
	get_zd_data("1023",g_pub_tx.note_type);
	get_zd_data("1024",g_pub_tx.beg_note_no);
	get_zd_data("1024",g_pub_tx.end_note_no);
	get_zd_data("1025",g_pub_tx.draw_uncon_yn);	
	get_zd_data("1026",g_pub_tx.draw_pwd_yn);	
	get_zd_data("1027",g_pub_tx.qry_pwd);
	get_zd_data("1028",g_pub_tx.draw_pwd);
	get_zd_data("1029",g_pub_tx.draw_id_yn);	
	get_zd_data("102A",g_pub_tx.id_no);	
	get_zd_data("102B",g_pub_tx.draw_seal_yn);	
	get_zd_data("102C",g_pub_tx.pwd_mach_yn);	
	get_zd_data("102D",g_pub_tx.draw_mach);	
	get_zd_long("102E",&g_pub_tx.mach_date);
	/** 币种 **/

	g_reply_int=pub_acct_cls_mdm();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用销介质模块败!");
		WRITEMSG
		goto ErrExit;
	}

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"销介质成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"销介质失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
