/*************************************************************
* 文 件 名:  sp9916.c
* 功能描述： 大额支付快捷查询
*			 汇票处理中心行号查询
*
* 作    者:  jane
* 完成日期： 2003年6月12日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9916()
{
	get_sys_par_val("14",mbfetran.rdficode);
	set_zd_data("0580",mbfetran.rdficode);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
