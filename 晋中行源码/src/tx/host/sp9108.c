/*************************************************
* 文 件 名: sp9108.c
* 功能描述：恢复营业所状态
*
* 作    者: rob
* 完成日期：20030322
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
#include "com_branch_c.h"

int sp9108( )
{
    int ret;
    char br_no[6];
  
    struct com_branch_c com_branch_c;

    memset(&com_branch_c, 0x0, sizeof(struct com_branch_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0910",br_no);

    ret = Com_branch_Dec_Upd(g_pub_tx.reply,"br_no='%s'",br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Com_branch_Fet_Upd(&com_branch_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (com_branch_c.wrk_sts[0] == '1')
    {
        sprintf(acErrMsg,"该营业所已经签到!![%s]",
                com_branch_c.wrk_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O069");
        goto ErrExit;
    }

    if (com_branch_c.wrk_sts[0] == '*')
    {
        sprintf(acErrMsg,"该营业所已经关闭!![%s]",
                com_branch_c.wrk_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O068");
        goto ErrExit;
    }

    strcpy(com_branch_c.wrk_sts, "1");
    ret = Com_branch_Upd_Upd(com_branch_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Com_branch_Clo_Upd();

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"恢复营业所状态成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"恢复营业所状态失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
