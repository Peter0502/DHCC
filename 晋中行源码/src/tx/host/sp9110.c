/*************************************************
* 文 件 名: sp9110.c
* 功能描述：暂停业务系统通讯
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

int sp9110( )
{
    int ret;
    char comm[100];

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(comm,"tcpappsvr -k 1>/home/rob/log/null 2>/home/rob/log/nul");
    ret = system(comm);
    if (ret !=0)
    {
        sprintf(acErrMsg,"关闭服务失败!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O151");
        goto ErrExit;
    }

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"暂停业务系统通讯成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"暂停业务系统通讯失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
