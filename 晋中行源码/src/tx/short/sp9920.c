/*****************************************************
* 文 件 名:  sp9920.c
* 功能描述： 支付系统状态查询
* 作    者:  jane
* 完成日期： 2003年6月13日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "mbfe_pub.h"

int sp9920()
{
	char statcode[3],sysstat[9];
	char date1[9],date2[9],date3[9];

	pub_base_sysinit();

	if (get_sys_par_val("05",statcode)) goto ErrExit;
	if (get_sys_par_val("09",date1)) goto ErrExit;
	if (get_sys_par_val("10",date2)) goto ErrExit;
	if (get_sys_par_val("11",date3)) goto ErrExit;

	pub_base_dic_show_str(sysstat,"sysstat",statcode);

	set_zd_data("0880",sysstat);
	set_zd_data("0440",date1);
	set_zd_data("0450",date2);
	set_zd_data("0460",date3);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
