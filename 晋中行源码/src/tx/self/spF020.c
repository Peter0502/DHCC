/*************************************************
* 文 件 名:  spF020.c
* 功能描述： POS 消费(判断该卡是本行卡还是外行卡)
*            POS 预授权完成(判断该卡是本行卡还是外行卡)
*            POS 预授权完成撤销(判断该卡是本行卡还是外行卡)
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
#include "card.h"
int spF020()
{
    int ret;

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */
    /**if (strncmp(g_pub_tx.crd_no,CARDHEADCODE,6) != 0 || strncmp(g_pub_tx.crd_no+6,CARD_UNION_AREA,2)!=0)**/
    if ((strncmp(g_pub_tx.crd_no,CARDHEADCODE,6) != 0 && strncmp(g_pub_tx.crd_no,ICCARDHEADCODE,6) != 0 ) || strncmp(g_pub_tx.crd_no+6,CARD_UNION_AREA,2)!=0)
    {
        vtcp_log("%s,%d 当前卡号=[%s]非本行卡号",__FILE__,__LINE__,g_pub_tx.crd_no);
        sprintf(acErrMsg,"调用不是本行卡消费子交易!!");
/* Modified by ChenMing 2007.08.21.
        goto OkExit1;
*/
    	strcpy( g_pub_tx.reply, "E099" );
        goto ErrExit; /* 外行卡直接报错 */
    }

OkExit:
    /* 本行卡 */
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    /* 外行卡 */
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

