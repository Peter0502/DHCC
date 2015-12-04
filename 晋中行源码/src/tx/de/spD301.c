/***************************************************************
* 文 件 名:     spD301.c
* 功能描述：	协定存款登记子交易
* 作    者:     mike
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

spD301()
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
	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_int("0340",&g_pub_tx.ac_seqn);
	get_zd_double("0390",&g_pub_tx.tx_amt1);
	get_zd_double("0840",&g_pub_tx.rate);
	/*** 必须在开户行交易 ***/
	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"账号不存在![%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"取介质账号对照错误![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P147");
		goto ErrExit;
	}
	if(strcmp(g_mdm_ac_rel.opn_br_no ,g_pub_tx.tx_br_no)!=0)
	{
		sprintf(acErrMsg,"此交易必须在开户行办理![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O171");
		goto ErrExit;
	}
	
	g_reply_int=pub_base_prot("XD1",1);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"理财协定登记主控出错!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"协定存款登记成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"协定存款登记失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
