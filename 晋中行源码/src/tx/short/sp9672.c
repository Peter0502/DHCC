/*************************************************************
* 文 件 名:  sp9672.c
* 功能描述： 授权交易之显示名称
*
*************************************************************/

#define EXTERN
#include "public.h"
#include "tx_def_c.h"

int 
sp9672()
{
	struct tx_def_c   menu_v;
	char            code[6];

	pub_base_sysinit();

	get_zd_data("0640", code);
	if (!strlen(code))
		goto OkExit;

	pub_base_sysinit();
	g_reply_int = Tx_def_Sel(g_pub_tx.reply, &menu_v,
			       "tx_code='%s'", code);
	if (g_reply_int) {
		sprintf(acErrMsg, "查询录入交易名称出错![%d]", g_reply_int);
		WRITEMSG
			strcpy(g_pub_tx.reply, "S060");
		goto ErrExit;
	}
	pub_base_strpack(menu_v.tx_name);
	set_zd_data("0630", menu_v.tx_name);

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
