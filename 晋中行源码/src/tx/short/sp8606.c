/***************************************************************
* 文 件 名:     sp8606.c
* 功能描述：	判断是否走支行过渡TIPS账户
* 作    者:     ZHGF
* 完成日期：    2009年6月27日
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
sp8606()
{
	int             ret = 0;

	if (memcmp(g_pub_tx.tx_br_no,"10001", 5)){
		goto ErrExit2;
	}else {
		goto ErrExit2;
	}

ErrExit:
	strcpy(g_pub_tx.reply, "P156");
	sprintf(acErrMsg, "TIPS过渡判断失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
ErrExit1:
	strcpy(g_pub_tx.reply, "0001");
	sprintf(acErrMsg, "支行过渡成功![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply, "0002");
	sprintf(acErrMsg, "总行成功![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
}
