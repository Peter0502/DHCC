/***************************************************************
* 文 件 名: spA013.c
* 功能描述：内部帐单边记帐
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

spA013()
{
	int ret=0;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_data("0890",g_pub_tx.note_type);
	get_zd_data("0580",g_pub_tx.beg_note_no);
	get_zd_double("0400",&g_pub_tx.tx_amt1);
	get_zd_data("0660",g_pub_tx.add_ind);		/*借贷标志1-2*/
	get_zd_data("0880",g_pub_tx.brf);

	if( g_pub_tx.add_ind[0]=='1' )
		strcpy( g_pub_tx.reply,"0001" );
	else
		strcpy( g_pub_tx.reply,"0002" );

OkExit:
	sprintf(acErrMsg,"内部帐单边记帐![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"内部帐单边记帐![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
