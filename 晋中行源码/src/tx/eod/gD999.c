/*************************************************************
* 文 件 名: gD999.c
* 功能描述：测试函数用的临时程序。
*
* 作    者: 
* 完成日期: 
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

gD999()
{
	int ret = 0;

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

vtcp_log("xxx--- look001");
	ret=sql_execute("update dd_mst set prdt_no='101' where ac_id=100000025 and ac_seqn=1 ");
	if( ret )
	{
		vtcp_log("sql_execute erro ![%d]",ret);
		strcpy(g_pub_tx.reply,"P001");
		return -1;
	}
vtcp_log("xxx --- look991");


OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
