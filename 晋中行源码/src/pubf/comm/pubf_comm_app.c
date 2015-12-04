#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_comm_app.c
* 功能描述：
*              Encrypt     通讯包加密函数
*              bal_open_database     打开数据库
*              bal_close_database    关闭数据库
*
* 作    者:    jack
* 完成日期：   2003年12月16日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/
# include <stdio.h>
#define EXTERN
# include "public.h"
# include "com_sys_parm_c.h"
# include "svrpub_c.h"

/*******************************************/
int Encrypt(char *string,int len,char *key,int flag)
{
    register i;
    int     klen;

    klen=strlen(key);

    for(i=0;i<len;i++) 
    {
        if(flag) string[i]=string[i]+key[i%klen];
        else string[i]=string[i]-key[i%klen];

    }

    return 0;
}

long bal_open_database()
{

    if ( sql_database( RtCode ) )
    {
        vtcp_log("connect error");
        return -1;
    }
    if ( bal_set_lockwaittime( ) )
    {
		vtcp_log("set lock mode to wait 5 err");
        return -1;
	}

    return 0;
}

long bal_close_database()
{
    if ( sql_close( ) )
    {
        return -1;
    }

    return 0;
}
