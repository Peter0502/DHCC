#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_intstin.c
* 功能描述：    内部帐户计算利息
* 调用函数说明：
*
*
* 作    者:     rob
* 完成日期：    2003年03月12日
*
* 修改记录：
* 1. 日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"


/**********************************************************************
* 函数名：    pub_base_CalIntstIn()
* 函数功能：  内部帐户计算利息
* 作者/时间： 2003年03月12日
*
* 参数：
*     输入：
*     输出: 应付利息          double
*           实付利息          double
*           代扣利息税        double
*           保值利息          double
*           响应码            char(4)
* 返 回 值: 0 成功
*
* 修改历史：
*
********************************************************************/
int pub_base_CalIntstIn(double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    double lxjs;          /* 利息积数 */   
    double rate_val;      /* 利率值 */
    double lx;            /* 利息 */
    int ret;

    long tx_date;

    (*dealval) = 0.00;
    (*factval) = 0.00;
    (*val)   = 0.00;
    (*keepval) = 0.00;   
    
    lxjs = g_in_mst.intst_acm;
    tx_date = g_pub_tx.tx_date;
  
sprintf(acErrMsg,"FFFFFFFFFFF intst_acm = [%lf]", g_in_mst.intst_acm);
WRITEMSG
    
    if (g_pub_tx.tx_date > g_in_mst.end_date)
    {
        rate_val = g_in_mst.over_rate;
    }
    else
    {
        rate_val = g_in_mst.rate;
    }

sprintf(acErrMsg,"rate_val=[%lf],lxjs=[%lf]", rate_val,lxjs);
WRITEMSG

    lx = rate_val * lxjs / L360 / L100;
            
    (*dealval) = pub_base_PubDround_1(lx);

    if (*dealval < 0)
    {
        (*dealval) = 0.00;
        (*factval) = 0.00;
    }
            
    (*val)     = 0.00;
    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

    return 0;

}
