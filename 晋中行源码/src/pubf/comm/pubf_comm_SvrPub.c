#include "find_debug.h" 
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
#include    <stdio.h>
#define EXTERN
#include    "public.h"
#include    "svrpub_c.h"

char *sqlerr(char *tabl,int errcode)
{
    char errmesg[50];

    MEMSET_DEBUG(errmesg,0,sizeof(errmesg));
    sprintf(errmesg,"主机数据库 [%s] 操作错误 -> [%d]",tabl,errcode);

    return errmesg;
}
