/*************************************************
* 文 件 名:  gD114.c
* 功能描述： 贴现登记簿的清理
*
* 作    者:  rob
* 完成日期： 2003年11月19日
*
* 修改记录：
* 日   期:  
* 修 改 人: 
* 修改内容:
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

gD114()
{
	int ret = 0;
	
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx));

	sql_begin(); /*打开事务*/

	/** 初始化全局变量 **/
	pub_base_sysinit();



	ret = sql_execute("delete from mo_discnt where sts='1' and pn_type!='4'");
	if ( ret )
	{
		sprintf(acErrMsg, "清理贴现登记簿(录入)错误!![%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--提交事务--*/
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"清理贴现登记簿(录入)成功![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 0;
ErrExit:
	sql_rollback(); /*--事务回滚--*/
        sprintf(acErrMsg,"清理贴现登记簿(录入)失败![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 1;
}
