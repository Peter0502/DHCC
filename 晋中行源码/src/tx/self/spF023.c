/*************************************************
* 文 件 名:  spF023.c
* 功能描述： POS 消费(判断该商户是本行还是外行)
*            POS 预授权完成(判断该商户是本行还是外行)
*            POS 预授权完成撤销(判断该商户是本行还是外行)
*            ATM 取款(判断是本行ATM还是外行ATM)
*	
* 作    者:  rob
* 完成日期： 2003年2月19日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"

int spF023()
{
    	char flag;

    	if (pub_base_sysinit())
    	{
        	sprintf(acErrMsg,"初始化公用结构错误!!");
        	WRITEMSG
        	goto ErrExit;
    	}

    	get_zd_char("0670",&flag); /* 数据来源 */

    	if (flag == '1')
    	{
        	sprintf(acErrMsg,"调用不是本行商户子交易!!");
        	goto OkExit1;
    	}
	else
	{
		/*  flag = '0';  For Test ChenMing 20070821 */
		strcpy( g_pub_tx.reply, "LS03" );
        sprintf(acErrMsg,"长治没有本行商户子交易!!");
        goto ErrExit;
	}


OkExit:
    /* 本行商户 */
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    /* 外行商户 */
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

