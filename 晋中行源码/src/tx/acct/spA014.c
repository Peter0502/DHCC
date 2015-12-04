/***************************************************************
* 文 件 名: spA014.c
* 功能描述：内部帐现金借贷记帐
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

spA014()
{
	char cJzlx[2];

	memset(cJzlx,0x0,sizeof(cJzlx));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	strcpy(g_pub_tx.reply,"0000");

	get_zd_data("0670",cJzlx);	/*记帐类型*/
	if (cJzlx[0] == '1')
	{
		strcpy( g_pub_tx.reply,"0001" );
	}
	else if (cJzlx[0] == '2')
	{
		strcpy( g_pub_tx.reply,"0002" );
	}
	else
	{
		sprintf(acErrMsg,"记帐类型出错!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf(acErrMsg,"内部帐借贷记帐成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"内部帐借贷记帐失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
