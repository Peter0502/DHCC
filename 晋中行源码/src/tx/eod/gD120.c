/*************************************************************
* 文 件 名: gD120.c
* 功能描述：日终预提利息
*
* 作    者: mike
* 完成日期: 20030705
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

gD120()
{
	int ret = 0;

	ret=sql_begin();
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	strcpy(g_pub_tx.tx_code,"Z120");
	strcpy(g_pub_tx.sub_tx_code,"Z120");
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.brf,"预提利息");

	ret=pub_eod_put_intst();
	if( ret )
	{
		sprintf( acErrMsg, "预提利息主控函数错误!" );
		WRITEMSG
		goto ErrExit;
	}
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
