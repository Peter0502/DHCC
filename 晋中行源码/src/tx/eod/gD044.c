/*************************************************************
* 文 件 名: gD044.c
* 功能描述：操作员业务量统计
*
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "com_sys_parm_c.h"

gD044()
{
	int ret = 0;
	struct com_sys_parm_c com_sys_parm_c;

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

    /* 查询系统参数表 */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	if( ret ) goto ErrExit;
    g_pub_tx.tx_date = com_sys_parm_c.lst_date; /* 翻盘后处理 */

	ret=pub_pubdb_gyywl_Dec();
	if( ret ) goto ErrExit;

OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"操作员业务量统计成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"操作员业务量统计失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
