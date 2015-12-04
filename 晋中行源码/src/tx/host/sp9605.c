/*************************************************
* 文 件 名: sp9605.c
* 功能描述：修改营业机构
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

int sp9605( )
{
    int ret;
    char br_type[2];
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

    /* 检查机构是否存在 */
    ret = Com_branch_Dec_Upd(g_pub_tx.reply,"br_no='%s'",com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Com_branch_Fet_Upd(&com_branch_c, g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询公共机构码表错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"公共机构码表中不存在机构!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O127");
        goto ErrExit;
    }

    if (atoi(com_branch_c.br_type) < 4 && \
             strcmp(com_branch_c.br_type,"") != 0)
    {
        sprintf(acErrMsg,"该机构没有上级机构!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O128");
        goto ErrExit;
    }

    get_zd_data("0250",com_branch_c.br_name);
    if ( strcmp(com_branch_c.br_type,"") != 0)
    {
        /* 检查上级机构是否存在 */
        get_zd_data("0640",com_branch_c.up_br_no);
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
        get_zd_data("0670",br_type);   
        if (atoi(br_type) < 4)
        {
            sprintf(acErrMsg,"机构类型不能从支行改成总行级别!!");
            WRITEMSG
            goto ErrExit;
        }
        strcpy(com_branch_c.br_type,br_type);
    }

    get_zd_data("0580",com_branch_c.ipaddr);
    get_zd_data("0680",com_branch_c.ind);
    get_zd_long("0480",&com_branch_c.city_no);
    get_zd_data("0730",com_branch_c.spe_ind);
	get_zd_data("0630", com_branch_c.br_tele);
	strcpy(com_branch_c.wrk_sts, "2");

    sprintf(acErrMsg,"spe_ind =[%s]",com_branch_c.spe_ind);
    WRITEMSG

    ret = Com_branch_Upd_Upd(com_branch_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Com_branch_Clo_Upd();
       
    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"修改营业机构");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"修改营业机构成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"修改营业机构失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
