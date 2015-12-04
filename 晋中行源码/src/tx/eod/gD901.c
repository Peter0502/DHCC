/*************************************************************
* 文 件 名: gD901.c
* 功能描述：数据库优化
*
* 完成日期: 2003年3月5日
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

gD901()
{
	int ret = 0;
	int br_type;
sprintf(acErrMsg, "%d@%s  ====> iimyaimyaimyai", __LINE__, __FILE__);
WRITEMSG
	ret = sql_execute("UPDATE STATISTICS");
	if ( ret )
	{
		sprintf( acErrMsg, "出错!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "G005" );
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终数据优化通过!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"日终数据优化失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
