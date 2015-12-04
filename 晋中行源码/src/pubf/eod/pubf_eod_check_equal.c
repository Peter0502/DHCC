/*************************************************
* 文 件 名:     pubf_eod_check_equal.c
* 功能描述：    日终帐务平衡检查
* 作    者:     jack
* 完成日期：    2003年03月14日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "gl_mst_c.h"
#include "com_sys_parm_c.h"
#include "dc_log_c.h"
#include "cash_mst_c.h"

/**********************************************************************
 * 函 数 名：   pub_eod_checktrace_mst
 * 函数功能：   业务流水和主文件检查平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
 *           =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checktrace_mst()
{
    int ret;
    int endflag;
    double total_amt;
    struct trace_log_c trace_log_c,trace_log;

    memset(&trace_log_c, 0x0, sizeof(struct trace_log_c));
    memset(&trace_log, 0x0, sizeof(struct trace_log_c));

    endflag = 1;
    total_amt = 0.00;
    ret = Trace_log_Dec_Sel(g_pub_tx.reply,"tx_date=%ld order by \
          ac_id,ac_seqn",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Trace_log_Fet_Sel(&trace_log_c,g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    else if (ret == 100)
    {
        return 0; 
    }
    
    if (trace_log_c.add_ind[0] == '1')
    {
        total_amt = trace_log_c.amt;
    }
    else 
    {
        total_amt = 0.00 - trace_log_c.amt;
    }
    memcpy(&trace_log,&trace_log_c,sizeof(struct trace_log_c));

    while(endflag)
    {
        ret = Trace_log_Fet_Sel(&trace_log_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            endflag = 0;
            memset(&trace_log_c,0x0,sizeof(struct trace_log_c));
        }
        
        if (strcmp(trace_log_c.ac_no,trace_log.ac_no) != 0 || \
             trace_log_c.ac_seqn != trace_log.ac_seqn)
        {
            /* 查询帐户主文件，检查余额变化 */

            sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",
                    trace_log.ac_no,trace_log.ac_seqn);
            WRITEMSG

            ret = pub_eod_checkmst(trace_log.ac_no,trace_log.ac_seqn,
                  trace_log.tel,trace_log.cur_no,total_amt);
            if (ret != 0)
            {
                sprintf(acErrMsg,"检查该账户今日的发生额错误!!");
                WRITEMSG
                return 1;
            }

            if (trace_log_c.add_ind[0] == '1')
            {
                total_amt = trace_log_c.amt;
            }
            else 
            {
                total_amt = 0.00 - trace_log_c.amt;
            }
            memcpy(&trace_log,&trace_log_c,sizeof(struct trace_log_c));
        }
        else
        {
            if (trace_log_c.add_ind[0] == '1')
            {
                total_amt = total_amt + trace_log_c.amt;
            }
            else 
            {
                total_amt = total_amt - trace_log_c.amt;
            }
            memcpy(&trace_log,&trace_log_c,sizeof(struct trace_log_c));
        }
    }
    Trace_log_Clo_Sel();

    /* 检查现金台帐 *
    endflag = 1;
    ret = Trace_log_Dec_Sel(g_pub_tx.reply,"(sub_tx_code='M001' or \
          sub_tx_code='M002') and tx_date=%ld order by ac_id,ac_seqn",
          g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    ret = Trace_log_Fet_Sel(&trace_log_c,g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    else if (ret == 100)
    {
        return 0; 
    }
    
    if (trace_log_c.add_ind[0] == '1')
    {
        total_amt = trace_log_c.amt;
    }
    else 
    {
        total_amt = 0.00 - trace_log_c.amt;
    }
    memcpy(&trace_log,&trace_log_c,sizeof(struct trace_log_c));

    while(endflag)
    {
        ret = Trace_log_Fet_Sel(&trace_log_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            endflag = 0;
            memset(&trace_log_c,0x0,sizeof(struct trace_log_c));
        }

        if (strcmp(trace_log_c.ac_no,trace_log.ac_no) != 0 || \
             trace_log_c.ac_seqn != trace_log.ac_seqn)
        {
            * 查询帐户主文件，检查余额变化 *
            ret = pub_eod_checkcashmst(trace_log.tel,trace_log.cur_no,
                  total_amt);
            if (ret != 0)
            {
                sprintf(acErrMsg,"检查该账户今日的发生额错误!!");
                WRITEMSG
                return 1;
            }

            if (trace_log_c.add_ind[0] == '1')
            {
                total_amt = trace_log_c.amt;
            }
            else 
            {
                total_amt = 0.00 - trace_log_c.amt;
            }
            memcpy(&trace_log,&trace_log_c,sizeof(struct trace_log_c));
        }
        else
        {
            if (trace_log_c.add_ind[0] == '1')
            {
                total_amt = total_amt + trace_log_c.amt;
            }
            else 
            {
                total_amt = total_amt - trace_log_c.amt;
            }
            memcpy(&trace_log,&trace_log_c,sizeof(struct trace_log_c));
        }
    }
    Trace_log_Clo_Sel();
    *******/
    return 0;
}

/**********************************************************************
 * 函 数 名：   pub_eod_checkmst
 * 函数功能：   检查账户主文件的今日发生额和流水中是否一致
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checkmst(char *ac_no,int ac_seqn,char *tel,
                     char *cur_no,double tx_amt)
{
    int ret;
    double amt;
    char ac_type[2];
    struct cash_mst_c cash_mst_c;

    memset(&cash_mst_c, 0x0, sizeof(struct cash_mst_c));
  
    pub_base_strpack(ac_no);
    sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",ac_no,ac_seqn);
    WRITEMSG

    if (strncmp(ac_no,CASH_HEAD,4) == 0)
    {
        ret = Cash_mst_Sel(g_pub_tx.reply,&cash_mst_c,"tel='%s' and \
              cur_no='%s' and sts='0'", tel,cur_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"查询现金台帐错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        ac_type[0]='8';
    }
    else 
    { 
        ret = pub_base_disac_file(ac_no,ac_seqn,ac_type);
        if (ret != 0)
        {
            sprintf(acErrMsg,"查询帐户主文件错误!!");
            WRITEMSG
            return ret;
        }
    }

    switch(ac_type[0])
    {
        case '1':         /*1-活期*/
            amt = g_dd_mst.bal - g_dd_mst.ys_bal;
            if (pub_base_CompDblVal(tx_amt,amt) != 0)
            {
                sprintf(acErrMsg,"流水中今日交易金额和活期帐户\
                       主文件中的发生额不平衡!!!");
                WRITEMSG
                return 1;
            }
            break;
        case '2':         /*2-定期*/
            amt = g_td_mst.bal - g_td_mst.ys_bal;
            if (pub_base_CompDblVal(tx_amt,amt) != 0)
            {
                sprintf(acErrMsg,"流水中今日交易金额和定期帐户\
                       主文件中的发生额不平衡!!!");
                WRITEMSG
                return 2;
            }
            break;
        case '3':         /*3-贷款*/
            amt = g_ln_mst.bal - g_ln_mst.ys_bal;
            if (pub_base_CompDblVal(tx_amt,amt) != 0)
            {
                sprintf(acErrMsg,"流水中今日交易金额和贷款帐户\
                       主文件中的发生额不平衡!!!");
                WRITEMSG
                return 3;
            }
            break;
        case '4':         /*4-透支*/
            amt = g_od_mst.bal - g_od_mst.ys_bal;
            if (pub_base_CompDblVal(tx_amt,amt) != 0)
            {
                sprintf(acErrMsg,"流水中今日交易金额和透支帐户\
                       主文件中的发生额不平衡!!!");
                WRITEMSG
                return 4;
            }
            break;
        case '8':         /* 现金台帐 */
            amt = cash_mst_c.bal - cash_mst_c.ys_bal;
            if (pub_base_CompDblVal(amt,tx_amt) != 0)
            {
                sprintf(acErrMsg,"业务流水和现金台帐中的金额变化不一致!!");
                WRITEMSG
                return 8;
            }
            break;
        case '9':         /*内部产品*/
            amt = g_in_mst.bal - g_in_mst.ys_bal;
            if (pub_base_CompDblVal(tx_amt,amt) != 0)
            {
                sprintf(acErrMsg,"流水中今日交易金额和内部帐户\
                       主文件中的发生额不平衡!!!");
                WRITEMSG
                return 9;
            }
            break;
        default:
            sprintf( acErrMsg, "帐户类型错" );
            WRITEMSG
            return -1;
    }

    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_checkcashmst
 * 函数功能：   检查现金台帐的今日发生额和流水中是否一致
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checkcashmst(char *tel,char *cur_no,double tx_amt)
{
    int ret;
    double amt;
    struct cash_mst_c cash_mst_c;
    
    ret = Cash_mst_Sel(g_pub_tx.reply,&cash_mst_c,"tel='%s' and cur_no='%s' and sts='0'",
          tel,cur_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询现金台帐错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    amt = cash_mst_c.bal - cash_mst_c.ys_bal;
    if (pub_base_CompDblVal(amt,tx_amt) != 0)
    {
        sprintf(acErrMsg,"业务流水和现金台帐中的金额变化不一致!!");
        WRITEMSG
        return -1;
    }
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_checkmst_trace
 * 函数功能：   检查账户主文件和业务流水是否平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checkmst_trace()
{
    int ret;

    /* 检查活期帐户主文件和业务流水的平衡 */
    ret = pub_eod_checkddmst_trace();
    if (ret != 0)
    {
        sprintf(acErrMsg,"活期帐户主文件和业务流水不平衡!!");
        WRITEMSG
        return 1;
    }

    ret = pub_eod_checktdmst_trace();
    if (ret != 0)
    {
        sprintf(acErrMsg,"定期账户主文件和业务流水不平衡!!");
        WRITEMSG
        return 2;
    }

    ret = pub_eod_checklnmst_trace();
    if (ret != 0)
    {
        sprintf(acErrMsg,"贷款账户主文件和业务流水不平衡!!");
        WRITEMSG
        return 3;
    }

    ret = pub_eod_checkodmst_trace();
    if (ret != 0)
    {
        sprintf(acErrMsg,"透支帐户主文件和业务流水不平衡!!");
        WRITEMSG
        return 4;
    }

    ret = pub_eod_checkinmst_trace();
    if (ret != 0)
    {
        sprintf(acErrMsg,"内部账户主文件和业务流水不平衡!!");
        WRITEMSG
        return 9;
    }
    
    ret = pub_eod_checkcashmst_trace();
    if (ret != 0)
    {
        sprintf(acErrMsg,"现金台帐和业务流水不平衡!!");
        WRITEMSG
        return -1;
    }
    return 0;
}

/**********************************************************************
 * 函 数 名：   pub_eod_checkddmst_trace
 * 函数功能：   检查活期账户主文件和业务流水是否平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checkddmst_trace()
{
    int ret;
    double total_amt;
    char ac_no[20];

    struct dd_mst_c dd_mst_c;

    memset(&dd_mst_c, 0x0, sizeof(struct dd_mst_c));

    total_amt = 0.00;

    ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"1=1 order by ac_id,ac_seqn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Dd_mst_Fet_Sel(&dd_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        if (pub_base_CompDblVal(dd_mst_c.bal,dd_mst_c.ys_bal) == 0)
        {
            continue;
        }

        total_amt = dd_mst_c.bal - dd_mst_c.ys_bal;

        sprintf(acErrMsg,"AAA ac_id=[%ld], ac_seqn=[%d]",
               dd_mst_c.ac_id,dd_mst_c.ac_seqn);
        WRITEMSG

        /***********
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_c,"ac_id=%ld and \
              ac_seqn =%d ",dd_mst_c.ac_id,dd_mst_c.ac_seqn);
        if (ret != 0)
        {
            sprintf(acErrMsg,"SELECT MDM_AC_REL ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        ***********/

        ret = pub_base_acid_acno(dd_mst_c.ac_id,dd_mst_c.ac_seqn,ac_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"转化显示帐号错误!!");
            WRITEMSG
            return ret;
        }
        ret = pub_eod_checktrace_log(ac_no,dd_mst_c.ac_seqn,
              total_amt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"活期帐户主文件今日发生额和业务流水中的不一致!!");
            WRITEMSG
            return ret;
        }
    }
    Dd_mst_Clo_Sel();
   
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_checktdmst_trace
 * 函数功能：   检查定期主文件和业务流水是否平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checktdmst_trace()
{
    int ret;
    double total_amt;
    char ac_no[20];

    struct td_mst_c td_mst_c;

    memset(&td_mst_c, 0x0, sizeof(struct td_mst_c));

    total_amt = 0.00;
    
    ret = Td_mst_Dec_Sel(g_pub_tx.reply,"1=1 order by ac_id,ac_seqn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Td_mst_Fet_Sel(&td_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        if (pub_base_CompDblVal(td_mst_c.bal,td_mst_c.ys_bal) == 0)
        {
            continue;
        }

        total_amt = td_mst_c.bal - td_mst_c.ys_bal;

        /************
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_c,"ac_id=%ld and \
              ac_seqn =%d ",td_mst_c.ac_id,td_mst_c.ac_seqn);
        if (ret != 0)
        {
            sprintf(acErrMsg,"SELECT MDM_AC_REL ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        *********/
        ret = pub_base_acid_acno(td_mst_c.ac_id,td_mst_c.ac_seqn,ac_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"转化显示帐号错误!!");
            WRITEMSG
            return ret;
        }

        ret = pub_eod_checktrace_log(ac_no,td_mst_c.ac_seqn,
              total_amt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"定期帐户主文件今日发生额和业务流水中的不一致!!");
            WRITEMSG
            return ret;
        }
    }
    Td_mst_Clo_Sel();
   
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_checklnmst_trace
 * 函数功能：   检查贷款主文件和业务流水是否平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checklnmst_trace()
{
    int ret;
    double total_amt;
    char ac_no[20];

    struct ln_mst_c ln_mst_c;
    
    memset(&ln_mst_c, 0x0, sizeof(struct ln_mst_c));
    
    ret = Ln_mst_Dec_Sel(g_pub_tx.reply,"1=1 order by ac_id,ac_seqn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Ln_mst_Fet_Sel(&ln_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        if (pub_base_CompDblVal(ln_mst_c.bal,ln_mst_c.ys_bal) == 0)
        {
            continue;
        }

        total_amt = ln_mst_c.bal - ln_mst_c.ys_bal;

        /***************
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_c,"ac_id=%ld and \
              ac_seqn =%d ",ln_mst_c.ac_id,ln_mst_c.ac_seqn);
        if (ret != 0)
        {
            sprintf(acErrMsg,"SELECT MDM_AC_REL ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        ****************/
        ret = pub_base_acid_acno(ln_mst_c.ac_id,ln_mst_c.ac_seqn,ac_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"转化显示帐号错误!!");
            WRITEMSG
            return ret;
        }
        ret = pub_eod_checktrace_log(ac_no,ln_mst_c.ac_seqn,
              total_amt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"贷款帐户主文件今日发生额和业务流水中的不一致!!");
            WRITEMSG
            return ret;
        }
    }
    Ln_mst_Clo_Sel();
    
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_checkodmst_trace
 * 函数功能：   检查透支主文件和业务流水是否平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checkodmst_trace()
{
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_checkinmst_trace
 * 函数功能：   检查内部主文件和业务流水是否平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checkinmst_trace()
{
    int ret;
    double total_amt;
    char ac_no[20];

    struct in_mst_c in_mst_c;
    
    memset(&in_mst_c, 0x0, sizeof(struct in_mst_c));
    
    ret = In_mst_Dec_Sel(g_pub_tx.reply,"1=1 order by ac_id,ac_seqn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = In_mst_Fet_Sel(&in_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        if (pub_base_CompDblVal(in_mst_c.bal,in_mst_c.ys_bal) == 0)
        {
            continue;
        }

        total_amt = in_mst_c.bal - in_mst_c.ys_bal;

        /*************
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_c,"ac_id=%ld and \
              ac_seqn =%d ",in_mst_c.ac_id,in_mst_c.ac_seqn);
        if (ret != 0)
        {
            sprintf(acErrMsg,"SELECT MDM_AC_REL ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        ***************/
        ret = pub_base_acid_acno(in_mst_c.ac_id,in_mst_c.ac_seqn,ac_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"转化显示帐号错误!!");
            WRITEMSG
            return ret;
        }
        ret = pub_eod_checktrace_log(ac_no,in_mst_c.ac_seqn,
              total_amt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"内部帐户主文件今日发生额和业务流水中的不一致!!");
            WRITEMSG
            return ret;
        }
    }
    In_mst_Clo_Sel();
    
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_checkcashmst_trace
 * 函数功能：   检查现金台帐和业务流水是否平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checkcashmst_trace()
{
    int ret;
    double total_amt;

    struct cash_mst_c cash_mst_c;
    
    memset(&cash_mst_c, 0x0, sizeof(struct cash_mst_c));
    
    ret = Cash_mst_Dec_Sel(g_pub_tx.reply,"1=1 order by tel,cur_no");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Cash_mst_Fet_Sel(&cash_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        if (pub_base_CompDblVal(cash_mst_c.bal,cash_mst_c.ys_bal) == 0)
        {
            continue;
        }

        total_amt = cash_mst_c.bal - cash_mst_c.ys_bal;

        ret = pub_eod_checkcashmst_tracelog(cash_mst_c.tel,cash_mst_c.cur_no,
              total_amt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"现金台帐今日发生额和业务流水中的不一致!!");
            WRITEMSG
            return ret;
        }
    }
    Cash_mst_Clo_Sel();
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_checktrace_log
 * 函数功能：   检查账户主文件和业务流水是否平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checktrace_log(char *ac_no,int seqn,double tx_amt)
{
    int ret;
    double amt;
    struct trace_log_c trace_log_c;

    memset(&trace_log_c, 0x0, sizeof(struct trace_log_c));
    
    amt = 0.00;

    ret = Trace_log_Dec_Sel(g_pub_tx.reply,"ac_no='%s' and ac_seqn=%d and \
          tx_date=%ld and sts='0'",ac_no, seqn,g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Trace_log_Fet_Sel(&trace_log_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        if (trace_log_c.add_ind[0] == '1')
        {
            amt += trace_log_c.amt;
        }
        else 
        {
            amt -= trace_log_c.amt;
        }
    }
    Trace_log_Clo_Sel();
    
    if (pub_base_CompDblVal(amt,tx_amt) != 0)
    {
        sprintf(acErrMsg,"业务流水的发生额和主文件中的不一致!!");
        WRITEMSG
        return 1;
    }

    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_checkcashmst_tracelog
 * 函数功能：   检查现金台帐业务流水是否平衡
 * 作者/时间：  power/20030314
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_checkcashmst_tracelog(char *tel,char *cur_no,double tx_amt)
{
    int ret;
    double amt;
    struct trace_log_c trace_log_c;

    memset(&trace_log_c, 0x0, sizeof(struct trace_log_c));
    
    amt = 0.00;

    ret = Trace_log_Dec_Sel(g_pub_tx.reply,"tel='%s' and cur_no='%s' \
          and sts='0'and tx_date=%ld and (sub_tx_code ='M001' or \
          sub_tx_code = 'M002')",tel,cur_no,g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Trace_log_Fet_Sel(&trace_log_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        if (trace_log_c.add_ind[0] == '1')
        {
            amt += trace_log_c.amt;
        }
        else 
        {
            amt -= trace_log_c.amt;
        }
    }
    Trace_log_Clo_Sel();
    
    if (pub_base_CompDblVal(amt,tx_amt) != 0)
    {
        sprintf(acErrMsg,"业务流水的发生额和主文件中的不一致!!");
        WRITEMSG
        return 1;
    }

    return 0;
}
