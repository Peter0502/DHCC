/*************************************************
* 文 件 名:    pubf_comm_SvrPub.c
* 功能描述：
*              sql_err     生成数据库操作错误信息
*
* 作    者:    jack
* 完成日期：   2003年12月16日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/

#include   <stdio.h>
#include   <time.h>
#define    EXTERN
#include   "public.h"
#include   "svrpub_c.h"

void pubf_comm_free_buf()
{
	vtcp_log("[%s][%d]开始调用清楚内存空间函数!\n",__FILE__,__LINE__);
	free(Pub_Buf);
	free(Old_Pub_Buf);
	free(Pu_Val);
	free(Old_Pu_Val);
	vtcp_log("[%s][%d]恭喜!恭喜!内存空间被清除，程序继续.......\n",__FILE__,__LINE__);
}
