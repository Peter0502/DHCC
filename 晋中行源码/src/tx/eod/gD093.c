/*************************************************************
* 文 件 名: gD093.c
* 功能描述：撤并所测试
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

gD093()
{
	int ret = 0;

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0020","61055");
	set_zd_data("0030","61070");
	if ( pubf_m_cbs()) goto ErrExit;

    /**------- 初始化全局变量 --------**/
vtcp_log("hao --- look991");


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
