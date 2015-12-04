/***************************************************************
* 文 件 名:     spW701.c
* 功能描述：	外汇兑换判断买入卖出方的现金转账
* 作    者:     andy
* 完成日期：    2004年4月08日
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

spW701()
{
	char ct_ind[2];
	get_zd_data("0700",ct_ind);
		vtcp_log("[%d]ct_ind=[%s]",__LINE__,ct_ind);
	if( ct_ind[0]=='1' )
		{vtcp_log("[%d]ct_ind=[%s]",__LINE__,ct_ind);
		goto ErrExit1;
		}
	else if( ct_ind[0]=='2' )
		{vtcp_log("[%d]ct_ind=[%s]",__LINE__,ct_ind);
		goto ErrExit2;
		}

ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"判断现转标志失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}
