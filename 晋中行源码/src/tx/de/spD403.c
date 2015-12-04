/***************************************************************
* 文 件 名:     spD403.c
* 功能描述：	利息测算
*
* 作    者:     jack
* 完成日期：    2004年1月10日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD403()
{
    char lx[2];

	 /*清空结构*/
    if (init_struct())
    {
        strcpy(acErrMsg,"结构清空出错!");
        WRITEMSG
        goto ErrExit;
    }

    /*** 初始化全局变量 ***/
    if (pub_base_sysinit())
    {
        strcpy(acErrMsg,"初始化全局变量出错!");
        WRITEMSG
        goto ErrExit;
    }
    


    get_zd_data("0670",lx);
    if (lx[0] == '2')
    {
        goto ErrExit;
    }

	/*** 输出变量 ***/
OkExit:
	sprintf(acErrMsg,"计息器计息子交易!! [%s]",lx);
	WRITEMSG
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"具体帐户计息子交易!! [%s]",lx);
	WRITEMSG
    strcpy(g_pub_tx.reply,"0003");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
}

int init_struct()
{
    return 0;
}
