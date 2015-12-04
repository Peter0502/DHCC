/*************************************************
* 文 件 名: sp9604.c
* 功能描述：增加营业机构
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

int sp9604( )
{
    int ret;
    struct com_branch_c com_branch_c,com_branch;

    memset(&com_branch_c,0x0,sizeof(struct com_branch_c));
    memset(&com_branch, 0x0, sizeof(struct com_branch_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
 
    get_zd_data("0910",com_branch_c.br_no);
    get_zd_data("0250",com_branch_c.br_name);
    get_zd_data("0640",com_branch_c.up_br_no);
    get_zd_data("0580",com_branch_c.ipaddr);
    get_zd_data("0670",com_branch_c.br_type);   
    get_zd_data("0680",com_branch_c.ind);
	get_zd_data("0630", com_branch_c.br_tele);
       

TRACE
    /* 检查机构是否存在 */   
    ret = Com_branch_Sel(g_pub_tx.reply,&com_branch_c,"br_no='%s'",
          com_branch_c.br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询公共机构码表错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 0)
    {
        sprintf(acErrMsg,"公共机构码表中已经存在该记录!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O125");
        goto ErrExit;
    }

TRACE
    /* 检查上级机构是否存在 */
    ret = Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",
          com_branch_c.up_br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询公共机构码表错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"公共机构码表中没有该上级机构!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O126");
        goto ErrExit;
    }

TRACE
    strcpy(com_branch_c.wrk_sts,"0");
    com_branch_c.city_no = 0;
    strcpy(com_branch_c.spe_ind,"0");

TRACE
    ret = Com_branch_Ins(com_branch_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"向公共机构表中增加记录错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"增加营业机构");
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }

    /* 增加会计机构 */
    

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"增加营业机构成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"增加营业机构失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
