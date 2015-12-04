#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_intstDHLB.c
* 功能描述：    定活两便计算利息
* 调用函数说明：
*
*
* 作    者:     rob
* 完成日期：    2003年12月28日
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
* 函数名：    pub_base_CalIntstDHLB()
* 函数功能：  定活两便计算利息
* 作者/时间： 2003年12月28日
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
int pub_base_CalIntstDHLB(double *dealval, double *factval,
       double *val, double * keepval, char *reply)
{   
    char  intst_term_type[2] ;/* 计息天数类型 */
    long  dayterm,dayterm1; 
    int monterm;
    char llbh[4];  /* 利率编号 */
    char cur_no[3];   /* 币种 */
    double rate_val,rate_val1; /* 利率值 */
    double lx ,kslx;      /* 利息值 */
    long tx_date;    /* 交易日期 */
    long ic_date;    /* 起息日期 */
    double tx_amt;   /* 发生额 */
    double flt_ratio; /* 浮动比率 */
    double sdsl;      
    long date99,date9303,date9201,date8910;
    int ret;
   
    MEMSET_DEBUG(cur_no, 0x0, sizeof(cur_no));
    strcpy(cur_no, g_td_parm.cur_no);
    tx_date = g_pub_tx.tx_date;
    tx_amt  = g_pub_tx.tx_amt1;
    flt_ratio = g_td_mst.flt_ratio;
    ic_date = g_td_mst.ic_date;
    date99 = 19991101;
    date9303 = 19930301;
    date9201 = 19920101;
    date8910 = 19891001;
    (*dealval) = 0.00;
    (*factval) = 0.00;
    (*val) = 0.00;
    (*keepval) = 0.00;
    
sprintf(acErrMsg,"tx_amt=[%lf],[%lf]",tx_amt,g_pub_tx.tx_amt1);
WRITEMSG
    if (ic_date == 0 || tx_date == 0)
    {
        sprintf(acErrMsg,"起息日[%ld]或交易日[%ld]为空无法计息",
                ic_date, tx_date);
        WRITEMSG
        MEMSET_DEBUG(reply, 0x0, sizeof(reply));
        strcpy(reply, "W112");
        return 1; 
    }

		strcpy(intst_term_type,g_td_parm.intst_term_type);/***按实际天数计息***/
    if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
	{
	    sprintf(acErrMsg,"产品表中的计息天数类型错误 type=[%s]",
	            g_td_parm.intst_term_type );
	    WRITEMSG
	    MEMSET_DEBUG(reply, 0x0, sizeof(reply));
	    strcpy(reply, "W107");
	    return 1;
	}

    ret=pub_base_CalIntstDays(ic_date,tx_date,intst_term_type,&dayterm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"计算天数错误!!");
        WRITEMSG
        return 1;
    }
    
    monterm = dayterm / 30;
    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
    
sprintf(acErrMsg,"开始计算定活两便利息");
WRITEMSG
    if (g_td_mst.ic_date >= date9303)
    {
        if (monterm < 3)
        {
            strcpy(llbh,HQLLBH);
        }      
        else if (monterm < 6)
        {
            strcpy(llbh,ZCZQ03);
            
        }
        else if (monterm < 12)
        {
            strcpy(llbh,ZCZQ06);
        }
        else 
        {
            strcpy(llbh,ZCZQ12);
        }
 sprintf(acErrMsg,"llbh =  [%s]",llbh);
 WRITEMSG

        ret = pub_base_getllz(HQLLBH,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
            MEMSET_DEBUG(reply, 0x0, sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }

sprintf(acErrMsg,"rate_val =  [%f]",rate_val);
WRITEMSG

        if (strcmp(llbh,HQLLBH) != 0)
        {
            ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val1);
            if (ret != 0)
            {
                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                WRITEMSG
                MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                strcpy(reply,"W110");
                return ret;
            }
            rate_val1 = rate_val1 * 0.60; 
            if (rate_val <= rate_val1)
            {
                rate_val = rate_val1;
            }
        }
sprintf(acErrMsg,"rate_val =  [%f]",rate_val);
WRITEMSG
    }
    else if (g_td_mst.ic_date >= date9201)
    {
    }
    else if (g_td_mst.ic_date >= date8910)
    {
    }
    else 
    {
    }
    
sprintf(acErrMsg,"day =[%d],rate_val=[%f] tx_amt=[%lf],flt_ratio=[%f]",
       dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

    lx = dayterm * rate_val * floor(tx_amt*L100)/L100 * (1 + flt_ratio) / L360 / L100;
    lx = pub_base_PubDround_1(lx);

sprintf(acErrMsg,"利息lx =[%lf]", lx);
WRITEMSG
    
    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
    {
		ret=pub_base_CalIntstDays(PubMax(ic_date,date99),tx_date,
			intst_term_type,&dayterm1);
	    if (ret != 0)
	    {
	        sprintf(acErrMsg,"计算天数错误!!");
	        WRITEMSG
	        return 1;
	    }
    
        kslx = lx * dayterm1 / dayterm;
        
        ret = pub_base_GetRate(SLBH, tx_date, &sdsl);
        if (ret != 0 && ret == 1)
        {
             sprintf(acErrMsg,"定期产品利息税税率不存在 %d",ret);
             WRITEMSG
             MEMSET_DEBUG(reply, 0x0, sizeof(reply));
             strcpy(reply,"W100");
             return 1;
        }
        else if (ret < 0)
        {
             sprintf(acErrMsg,"取利息税税率错误 %d",ret);
             WRITEMSG
             strcpy(reply,"W101");
             return 1;
        }

		g_pub_intst.tax_intst = pub_base_PubDround_1(kslx);
        kslx = pub_base_PubDround_1(kslx * sdsl);
        (*val) = kslx;
    }
    (*dealval) = lx;
    (*factval) = (*dealval) - (*val);

WRITEMSG
sprintf(acErrMsg, "------>应税利息tax_intst: [%lf]", g_pub_intst.tax_intst);
WRITEMSG
        /* added by liuxuan 2007-08-01 修改利息税 */
	if(strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
		char edu_ind[2];
		vtcp_log("[%s][%d]开始调用分段算利息税函数\n",__FILE__,__LINE__);
		if(iGetAreaTax(val,factval,dealval,intst_term_type,6,rate_val,rate_val,edu_ind)){
			vtcp_log("[%s][%d]调用分段算利息税函数出错\n",__FILE__,__LINE__);
			return(1);
		}
	}

sprintf(acErrMsg,"调用分段算利息税函数后dealval =[%lf],val=[%lf] fact_val=[%lf]",
       *dealval,*val,*factval);
	/* add over */
    return 0;
}
