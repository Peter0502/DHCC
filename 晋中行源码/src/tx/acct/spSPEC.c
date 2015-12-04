/***************************************************************
* 文 件 名: spSPEC.c
* 功能描述：返回零交易
* 作    者: 
* 完成日期：2004年8月12日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spSPEC()
{
OkExit:
	strcpy(g_pub_tx.reply,"0000");
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
