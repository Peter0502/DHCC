/*************************************************
* 文 件 名: sp9111.c
* 功能描述：开启业务系统通讯
*
* 作    者: rob
* 完成日期：20030323
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#include <errno.h>
#define EXTERN
#include "public.h"

int sp9111( )
{
    int ret;
    char comm[100];

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    memset(comm,0x0,sizeof(comm));
    sprintf(comm,"tcpappsvr -p 6100 1>/home/xxx/log/null 2>/home/xxx/log/nul"); 
    ret = system(comm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"开启业务系统通讯失败!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O152");
        goto ErrExit;
    }

    memset(comm,0x0,sizeof(comm));
    sprintf(comm,"tcpappsvr 1>/home/xxx/log/null 2>/home/xxx/log/nul");
    ret = system(comm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"开启业务系统通讯失败!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O152");
        goto ErrExit;
    }

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"开启业务系统通讯成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"开启业务系统通讯失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
