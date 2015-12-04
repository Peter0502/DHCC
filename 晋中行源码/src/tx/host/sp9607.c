/*************************************************
* 文 件 名: sp9607.c
* 功能描述：合并营业机构
*
* 作    者: rob
* 完成日期：20030310
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

int sp9607( )
{
    int ret;
    struct com_branch_c com_branch_c;

    memset(&com_branch_c,0x0,sizeof(struct com_branch_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
 
    get_zd_data("0910",com_branch_c.br_no);
    get_zd_data("0250",com_branch_c.br_name);
    get_zd_data("0640",com_branch_c.up_br_no);
    get_zd_data("0260",g_pub_tx.name);

       
    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"合并营业机构");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"合并营业机构成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"合并营业机构失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
