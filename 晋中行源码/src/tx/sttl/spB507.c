/*************************************************
* 文 件 名:  spB507.c
* 功能描述： 判断拆入拆出方式
*
* 作    者:  rob
* 完成日期： 2003年7月06日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"

int spB507()
{
    int ret;
	char flag[2];

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

	get_zd_data("0670", flag);

	if (flag[0] == '1')
	{
		/* 同城 */
		sprintf(acErrMsg, "拆入拆出方式为: [同城]");
		WRITEMSG
		goto OkExit1;
	}
	else if (flag[0] == '2')
	{
		/* 大额 */
		sprintf(acErrMsg, "拆入拆出方式为: [大额]");
		WRITEMSG
		goto OkExit;
	}
	else
	{
		sprintf(acErrMsg, "拆入拆出方式错误!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg," 大额 success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg," 同城 success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

