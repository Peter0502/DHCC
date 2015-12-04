#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_intst_acm.c
* 功能描述：    
*              1、pub_base_CalcIntstAcm计算利息积数(按每月实际天数计算)；
*              2、pub_base_CalcIntstAcm1计算利息积数(按每月30天计算,计算
*		  个人帐户利息积数,本外币不同)；
*              3、pub_base_CalcIntstAcm2计算利息积数(按每月30天计算)；
*              4、pub_base_CalcIntstAcm3计算利息积数(按实际天数计算，余额
*		  取整后计算积数)；
*
* 调用函数说明：pub_base_CalDays  计算实际天数差(原PubJsts)
*               pub_base_CalTriDays 计算天数，按每月30天计算(原PubTs)
*          
* 作    者:    
* 完成日期：    2003年12月17日
*
* 修改记录：    
* 1. 日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/

#define loc_t sys_loc_t

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define EXTERN
#include "public.h"

/**********************************************************************
* 函数名：    pub_base_CalcIntstAcm(原CalLxjs)
* 函数功能：  计算利息积数(按每月实际天数计算)
* 作者/时间： 2003年12月17日
* 
* 参数：
*     输入：  
*            double *pdAccInt : 利息积数
*            long lLastDate   : 上次余额变动日期
*            long lCurrDate   : 本次余额变动日期
*            double dLastBal  : 上次余额
*           
*     输出： double *pdAccInt : 利息积数
*
* 返 回 值: 0 成功
*           
* 修改历史：
*
********************************************************************/

int pub_base_CalcIntstAcm( double *pdAccInt, long lLastDate, long lCurrDate, double dLastBal, char xym[] )
{
    double dOldAccInt, dNewAccInt;
    double dCrassDate, dTmpAccInt;

    dOldAccInt = *pdAccInt;

    /* 计算天数 */
    dCrassDate = (double)pub_base_CalDays(lLastDate,lCurrDate);
    if ( dCrassDate < 0 ) 
    {
        dCrassDate = 0.00;
        strcpy(xym,"0000");
    }

    /* 计算新增加的积数 */
    dTmpAccInt = dLastBal * dCrassDate;
    /* 原有积数+新增积数 ＝ 当前积数 */
    dNewAccInt = dOldAccInt + dTmpAccInt;

    *pdAccInt = dNewAccInt;

    return 0;
}

/**********************************************************************
* 函数名：    pub_base_CalcIntstAcm1(原CalLxjs1)
* 函数功能：  计算利息积数(按每月30天计算,计算个人帐户利息积数,本外币不同)
* 作者/时间： 2003年12月17日
* 
* 参数：
*     输入： 
*            double *pdAccInt : 利息积数
*            long lLastDate   : 上次余额变动日期
*            long lCurrDate   : 本次余额变动日期
*            double dLastBala : 上次余额
*           
*     输出： double *pdAccInt : 利息积数
*
* 返 回 值: 0 成功 ，1 失败
*           
* 修改历史：
*
********************************************************************/

int pub_base_CalcIntstAcm1( double *pdAccInt, long lCurrDate, long lLastDate, double dLastBal, char pubjz_bz1[], char xym[] )
{
    double dOldAccInt, dNewAccInt;
    double dCrassDate, dTmpAccInt;
    long grjxr;            /* 个人计息日期 */
    char grhjxr[5];
    struct  ggxtcs_c    xtcs;

    dOldAccInt = *pdAccInt;
    dCrassDate = 0;
    
    if ( lLastDate == 0 )
    {
        dCrassDate = 1;
        /* 取系统参数 */
        if (PubQxtcs(&xtcs,xym) )
        {
            return(1);
        }
        
        /* 取个人计息日期，本外币不同 */
        if ( !strcmp(pubjz_bz1,RMB) )
        {
            strncpy(grhjxr,xtcs.grhjxrq+2,4);
        }else
        {
            strncpy(grhjxr,xtcs.wbgrhjxrq+2,4);
        }
        
        grhjxr[4]='\0';
        lLastDate  = lCurrDate/10000;
        lLastDate  = lLastDate*10000+atol(grhjxr);
        
        if (lLastDate < lCurrDate)
        {
            lLastDate += 10000; 
        }
    }
    
    dCrassDate += (double)pub_base_CalTriDays(lCurrDate,lLastDate) ;
    dTmpAccInt = floor(dLastBalL100)/L100 * dCrassDate;
    sprintf(acErrMsg,"dOldAccInt=[%lf],lLastDate=[%ld],lCurrDate=[%ld],dCrassDate=[%f],dLastBala=[%f],dTmpAccInt=[%f],dNewAccInt=[%f]",dOldAccInt,lLastDate,lCurrDate,dCrassDate,dLastBala,dTmpAccInt,dNewAccInt);
    WRITEMSG
    
    dNewAccInt = dOldAccInt + dTmpAccInt;

    *pdAccInt = dNewAccInt;
    return (0);
}

/**********************************************************************
* 函数名：    pub_base_CalcIntstAcm2(原CalLxjs2)
* 函数功能：  计算利息积数(按每月30天计算)
* 作者/时间： 2003年12月17日
* 
* 参数：
*     输入： 
*            double *pdAccInt : 利息积数
*            long lCurrDate   : 本次余额变动日期
*            double dLastBal  : 上次余额
*           
*     输出： double *pdAccInt : 利息积数
*
* 返 回 值: 0 成功
*           
* 修改历史：
*
********************************************************************/

int pub_base_CalcIntstAcm2( double *pdAccInt, long lCurrDate, double dLastBal )
{
    double dOldAccInt, dNewAccInt;
    double dCrassDate, dTmpAccInt;
    long lLastDate;                    /*结息日*/

    dOldAccInt = *pdAccInt;
    dCrassDate = (double)PubTs(lCurrDate,lLastDate);
    if ( dCrassDate < 0 )
    {
        dCrassDate = 0.00;
    }
    
    dTmpAccInt = dLastBal * dCrassDate;
    dNewAccInt = dOldAccInt + dTmpAccInt;

    *pdAccInt = dNewAccInt;
    return(0);
}

/**********************************************************************
* 函数名：    pub_base_CalcIntstAcm3(CalLxjs3)
* 函数功能：  计算利息积数(按实际天数计算，余额取整后计算积数)
* 作者/时间： 2003年12月17日
* 
* 参数：
*     输入： 
*            double *pdAccInt : 利息积数
*            long lLastDate   : 上次余额变动日期
*            long lCurrDate   : 本次余额变动日期
*            double dLastBal  : 上次余额
*           
*     输出： double *pdAccInt : 利息积数
*
* 返 回 值: 0 成功
*           
* 修改历史：
*
********************************************************************/

int pub_base_CalcIntstAcm3( double *pdAccInt, long lLastDate, long lCurrDate, double dLastBal )
{
    double dOldAccInt, dNewAccInt;
    double dCrassDate, dTmpAccInt;

    dOldAccInt = *pdAccInt;

    dCrassDate = (double)pub_base_CalDays(lLastDate,lCurrDate);
    if ( dCrassDate < 0 )
    {
        dCrassDate = 0.00;
    }

    /* 对余额取整后计算积数 */
    dTmpAccInt = floor(dLastBal*L100)/L100 * dCrassDate;
    dNewAccInt = dOldAccInt + dTmpAccInt;

    *pdAccInt = dNewAccInt;

    return(0);
}
