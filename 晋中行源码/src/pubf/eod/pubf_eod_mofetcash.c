/*************************************************
* 文 件 名:     pubf_eod_mofetcash.c
* 功能描述：    登记大额提现登记薄
* 作    者:     rob
* 完成日期：    2004年03月16日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "mo_fet_cash_c.h"
#include "com_parm_c.h"

/**********************************************************************
 * 函 数 名：   pub_eod_ins_mofetcash
 * 函数功能：   登记大额提现登记簿
 * 作者/时间：  rob/20040316
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
int pub_eod_ins_mofetcash()
{
    int ret;

    /* 统计活期大额提现 */
    ret = pub_eod_stat_ddhst();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计活期大额提现错误!!");
        WRITEMSG
        return ret;
    }

    /* 统计定期大额提现 */
    ret = pub_eod_stat_tdhst();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计定期大额提现错误!!");
        WRITEMSG
        return ret;
    }
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_ddhst
 * 函数功能：   统计活期大额提现
 * 作者/时间：  rob/20040316
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
int pub_eod_stat_ddhst()
{
    int ret,flag;
    long ac_id;
    int ac_seqn;
    double max_amt;
    double total_amt;
    struct dd_mst_hst_c dd_mst_hst_c;
    struct mo_fet_cash_c mo_fet_cash_c;
    struct com_parm_c com_parm_c;
    
    memset(&mo_fet_cash_c, 0x0, sizeof(struct mo_fet_cash_c));
    memset(&dd_mst_hst_c, 0x0, sizeof(struct dd_mst_hst_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    
    total_amt = 0.00;
    ac_id = 0;
    ac_seqn = 0;
    flag = 1;
    
    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='DEQX' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    max_amt = atof(com_parm_c.val);
    sprintf(acErrMsg,"大额取现的参数为：[%lf]",max_amt);
    WRITEMSG
    
    ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and add_ind='0' \
          and ct_ind='1' order by ac_id,ac_seqn", g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    while(flag)
    {
        ret = Dd_mst_hst_Fet_Sel(&dd_mst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            flag = 0;
            memset(&dd_mst_hst_c, 0x0, sizeof(struct dd_mst_hst_c));
        }
        
        if (ac_id != 0 && (ac_id != dd_mst_hst_c.ac_id \
            || ac_seqn != dd_mst_hst_c.ac_seqn))
        {
            if (pub_base_CompDblVal(total_amt,max_amt) >= 0)
            {
                /* 汇总大于参数值：登记大额取现登记簿 */
                mo_fet_cash_c.ac_id = ac_id;
                mo_fet_cash_c.ac_seqn = ac_seqn;
                mo_fet_cash_c.tx_date = g_pub_tx.tx_date;
                mo_fet_cash_c.amt = total_amt;
                mo_fet_cash_c.trace_no = 0;
                strcpy(mo_fet_cash_c.type,"1");

                ret = Mo_fet_cash_Ins(mo_fet_cash_c, g_pub_tx.reply);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"登记汇总大额取现登记簿错误!![%d]",ret);
                    WRITEMSG
                    sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",ac_id,ac_seqn);
                    WRITEMSG
                    return ret;
                }
            }
            total_amt = 0.00;    
        }
        if (flag == 1)
        {
            total_amt += dd_mst_hst_c.tx_amt;
            ac_id = dd_mst_hst_c.ac_id;
            ac_seqn = dd_mst_hst_c.ac_seqn;
            
            if (pub_base_CompDblVal(dd_mst_hst_c.tx_amt,max_amt) >= 0)
            {
                /* 单笔大于参数值：登记大额取现登记簿 */
                mo_fet_cash_c.ac_id = dd_mst_hst_c.ac_id;
                mo_fet_cash_c.ac_seqn = dd_mst_hst_c.ac_seqn;
                mo_fet_cash_c.tx_date = g_pub_tx.tx_date;
                mo_fet_cash_c.amt = dd_mst_hst_c.tx_amt;
                mo_fet_cash_c.trace_no = dd_mst_hst_c.trace_no; 
                strcpy(mo_fet_cash_c.type,"0");
                ret = Mo_fet_cash_Ins(mo_fet_cash_c, g_pub_tx.reply);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"登记单笔大额取现登记簿错误!![%d]",ret);
                    WRITEMSG
                    sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d],tx_date[%ld]",
                            dd_mst_hst_c.ac_id,dd_mst_hst_c.ac_seqn,g_pub_tx.tx_date);
                    WRITEMSG
                    sprintf(acErrMsg,"amt=[%lf],trace_no=[%ld]",
                            dd_mst_hst_c.tx_amt,dd_mst_hst_c.trace_no);
                    WRITEMSG                  
                    return ret;
                }
            }
        }
    }
    Dd_mst_hst_Clo_Sel();
TRACE
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_tdhst
 * 函数功能：   统计定期大额提现
 * 作者/时间：  rob/20040316
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
int pub_eod_stat_tdhst()
{
    int ret,flag;
    long ac_id;
    int ac_seqn;
    double max_amt;
    double total_amt;
    struct td_mst_hst_c td_mst_hst_c;
    struct mo_fet_cash_c mo_fet_cash_c;
    struct com_parm_c com_parm_c;
    
    memset(&mo_fet_cash_c, 0x0, sizeof(struct mo_fet_cash_c));
    memset(&td_mst_hst_c, 0x0, sizeof(struct td_mst_hst_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    
    total_amt = 0.00;
    ac_id = 0;
    ac_seqn = 0;
    flag = 1;
    
    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='DEQX' \
          and parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    max_amt = atof(com_parm_c.val);
    sprintf(acErrMsg,"大额取现的参数为：[%lf]",max_amt);
    WRITEMSG
    
    ret = Td_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and add_ind='0' \
          and ct_ind='1' order by ac_id,ac_seqn", g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    while(flag)
    {
        ret = Td_mst_hst_Fet_Sel(&td_mst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            flag = 0;
            memset(&td_mst_hst_c, 0x0, sizeof(struct td_mst_hst_c));
        }
        
        if (ac_id != 0 && (ac_id != td_mst_hst_c.ac_id \
            || ac_seqn != td_mst_hst_c.ac_seqn))
        {
            if (pub_base_CompDblVal(total_amt,max_amt) >= 0)
            {
                /* 汇总大于参数值：登记大额取现登记簿 */
                memset(&mo_fet_cash_c, 0x0, sizeof(struct mo_fet_cash_c));
                mo_fet_cash_c.ac_id = ac_id;
                mo_fet_cash_c.ac_seqn = ac_seqn;
                mo_fet_cash_c.tx_date = g_pub_tx.tx_date;
                mo_fet_cash_c.amt = total_amt;
                mo_fet_cash_c.trace_no = 0;
                strcpy(mo_fet_cash_c.type,"1");
                ret = Mo_fet_cash_Ins(mo_fet_cash_c, g_pub_tx.reply);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"登记汇总大额取现登记簿错误!![%d]",ret);
                    WRITEMSG
                    sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",ac_id,ac_seqn);
                    WRITEMSG
                    return ret;
                }
            }
            total_amt = 0.00;    
        }
        if (flag == 1)
        {
            total_amt += td_mst_hst_c.tx_amt;
            ac_id = td_mst_hst_c.ac_id;
            ac_seqn = td_mst_hst_c.ac_seqn;
            
            if (pub_base_CompDblVal(td_mst_hst_c.tx_amt,max_amt) >= 0)
            {
                /* 单笔大于参数值：登记大额取现登记簿 */
                memset(&mo_fet_cash_c, 0x0, sizeof(struct mo_fet_cash_c));
                mo_fet_cash_c.ac_id = td_mst_hst_c.ac_id;
                mo_fet_cash_c.ac_seqn = td_mst_hst_c.ac_seqn;
                mo_fet_cash_c.tx_date = g_pub_tx.tx_date;
                mo_fet_cash_c.amt = td_mst_hst_c.tx_amt;
                mo_fet_cash_c.trace_no = td_mst_hst_c.trace_no; 
                strcpy(mo_fet_cash_c.type,"0");
                ret = Mo_fet_cash_Ins(mo_fet_cash_c, g_pub_tx.reply);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"登记单笔大额取现登记簿错误!![%d]",ret);
                    WRITEMSG
                    sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",
                            td_mst_hst_c.ac_id,td_mst_hst_c.ac_seqn);
                    WRITEMSG
                    return ret;
                }
            }
        }
    }
    Td_mst_hst_Clo_Sel();
    return 0;
}
