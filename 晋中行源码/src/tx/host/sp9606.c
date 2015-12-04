/*************************************************
* 文 件 名: sp9606.c
* 功能描述：撤并营业机构
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
#include "trace_log_c.h"
#include "dc_log_c.h"

int sp9606( )
{
    int ret;
    struct com_branch_c com_branch_c;
    struct trace_log_c trace_log;
    struct dc_log_c dc_log;

    memset(&com_branch_c,0x0,sizeof(struct com_branch_c));
    memset(&trace_log, 0x0, sizeof(struct trace_log_c));
    memset(&dc_log, 0x0, sizeof(struct dc_log_c));

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

       
    /* 检查该机构今天是否做过交易 */
    ret = Trace_log_Sel(g_pub_tx.reply,&trace_log,"tx_br_no='%s' and \
          tx_date=%ld",com_branch_c.br_no,g_pub_tx.tx_date);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询业务流水错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 0)
    {
        sprintf(acErrMsg,"该机构今天做过交易，不能做撤并机构交易!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O129");
        goto ErrExit;
    }

    ret = Dc_log_Sel(g_pub_tx.reply,&dc_log,"tx_br_no='%s' and \
          tx_date=%ld",com_branch_c.br_no,g_pub_tx.tx_date);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询业务流水错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 0)
    {
        sprintf(acErrMsg,"该机构今天做过交易，不能做撤并机构交易!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O129");
        goto ErrExit;
    }

    ret = pub_mage_update(com_branch_c);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新数据库中的表错误!!");
        WRITEMSG
        goto ErrExit;
    }

    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"撤并营业机构");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"撤并营业机构成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"撤并营业机构失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
int pub_mage_update(struct com_branch_c com_branch_c)
{
    int ret;

    ret = sql_execute("update dd_mst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update dd_mst_hst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update ln_mst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update ln_mst_hst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update od_mst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update od_mst_hst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update td_mst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update td_mst_hst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update mo_hold set hold_br_no='%s' where hold_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update mo_loss set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update mo_loss set loss_br_no='%s' where loss_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update mo_merge_ac set br_no='%s' where br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update cash_mst set br_no='%s' where br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update cash_mst_hst set br_no='%s' where br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update note_cheq_hst set tx_br_no='%s' where tx_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update note_cheq_mst set br_no='%s' where br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update note_mst set br_no='%s' where br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update note_mst_hst set br_no='%s' where br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update note_min_no set br_no='%s' where br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update mo_opn_cls set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update mo_coll set ret_br_no='%s' where ret_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update com_branch set wrk_sts='*' where br_no\
          ='%s'",com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update com_tel set br_no='%s' where br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update dc_log set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update dc_log set tx_opn_br_no='%s' where tx_opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update dc_log set tx_opn_br_no='%s' where tx_opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update in_mst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update in_mst_hst set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update trace_log set opn_br_no='%s' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update mo_discnt set payee _brno='%s' where payee _brno\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update ass_br_def set sub_br_no='*' where sub_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = sql_execute("update ag_peracctinfo set opn_br_no='*' where opn_br_no\
          ='%s'",com_branch_c.up_br_no,com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更形数据库中的表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    return 0;
}
