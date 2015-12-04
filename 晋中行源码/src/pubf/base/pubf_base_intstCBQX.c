#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_intstCBQX.c
* 功能描述：    存本取息计算利息
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
* 函数名：    pub_base_CalIntstCBQX()
* 函数功能：  存本取息计算利息
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
int pub_base_CalIntstCBQX(double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    double rate_val;      /* 利率值 */
    double sdsl;          /* 税率值 */
    double lx;            /* 利息 */
    double flt_ratio;     /* 浮动比率 */
    double tx_amt;        /* 发生额 */
    char intst_term_type[2]; /* 计息天数类型 */
    char llbh[4];         /* 利率编号 */
    char rate_no[4];      /* 利率编号 */
    char cur_no[3];       /* 币种 */
    char intst_tax_no[4]; /*税率编号 */
    long tx_date;         /* 交易日期 */
    long ic_date,mtr_date; /* 起息日期 到期日期 */
    long date99;
    int ret;
    int tx_cnt;            /* 已取息次数 */
    double td_amt;         /* 每次取息额 */
    long dayterm;
   
    date99 = 19991101;
    tx_date = g_pub_tx.tx_date;
    tx_amt = g_pub_tx.tx_amt1;
  
    (*dealval) = 0.00;
    (*factval) = 0.00;
    (*keepval) = 0.00;
    (*val)   = 0.00;

    mtr_date = g_td_mst.mtr_date;
    ic_date  = g_td_mst.ic_date;
    flt_ratio = g_td_mst.flt_ratio;
    tx_cnt = g_td_mst.tx_cnt;
    td_amt = g_td_mst.td_amt;

    strcpy(cur_no, g_td_parm.cur_no);
    strcpy(intst_tax_no,g_td_parm.intst_tax_no);
    strcpy(rate_no, g_td_parm.rate_no);

    strcpy(intst_term_type,g_td_parm.intst_term_type);

	if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
         sprintf(acErrMsg,"产品表中的计息天数类型错误 type=[%s]",
                        g_td_parm.intst_term_type );
         WRITEMSG
         MEMSET_DEBUG(reply, 0x0, sizeof(reply));
         strcpy(reply, "W107");
         return 1;
    }

    if (tx_date == ic_date && g_pub_tx.svc_ind == 1000)
    {
        tx_date = mtr_date;  /* 开户日计算到期利息 */
        td_amt = 0.00;
        tx_cnt = 0;
    }

sprintf(acErrMsg,"计算存本取息利息mtr_date=[%ld],tx_date=[%ld]",
        mtr_date,tx_date);
WRITEMSG

    if (mtr_date > tx_date)
    {

sprintf(acErrMsg,"存本取息--提前支取");
WRITEMSG

        /* 提前支取 */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld] ",llbh,cur_no,tx_date);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate表中取活期利率错误",ret);
            WRITEMSG
            MEMSET_DEBUG(reply,0x0,sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }

		ret=pub_base_CalIntstDays(ic_date,tx_date,intst_term_type,&dayterm);
	    if (ret != 0)
	    {
	        sprintf(acErrMsg,"计算天数错误!!");
	        WRITEMSG
	        return 1;
	    }


sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

        lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
        (*dealval) = pub_base_PubDround_1(lx);
           

        if (strcmp(intst_tax_no,SLBH) == 0)
        {
			ret=pub_base_CalIntstDays(PubMax(date99,ic_date),tx_date,
	            intst_term_type,&dayterm);
	        if (ret != 0)
	        {
	            sprintf(acErrMsg,"计算天数错误!!");
	            WRITEMSG
	            return 1;
	        }

            (*val) =dayterm * rate_val * tx_amt * (1 + flt_ratio)/L360/L100;
			g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
        }

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

    }
    else
    {

sprintf(acErrMsg,"存本取息--到期逾期支取");
WRITEMSG

        /* 计算到期或逾期利息 */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,rate_no);

sprintf(acErrMsg,"llbh = [%s]",llbh);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,ic_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate表中取利率错误",ret);
            WRITEMSG
            MEMSET_DEBUG(reply,0x0,sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }

		ret=pub_base_CalIntstDays(ic_date,mtr_date,intst_term_type,&dayterm);
	    if (ret != 0)
	    {
	        sprintf(acErrMsg,"计算天数错误!!");
	        WRITEMSG
	        return 1;
	    }


sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

        lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
        (*dealval) = lx;
    
        if (strcmp(intst_tax_no,SLBH) == 0)
        {
			ret=pub_base_CalIntstDays(PubMax(ic_date,date99),mtr_date,
	            intst_term_type,&dayterm);
	        if (ret != 0)
	        {
	            sprintf(acErrMsg,"计算天数错误!!");
	            WRITEMSG
	            return 1;
	        }

            lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
            (*val) = lx;
			g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
        }    
       
sprintf(acErrMsg,"到期：dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh = [%s] ",llbh);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate表中取利率错误",ret);
            WRITEMSG
            MEMSET_DEBUG(reply,0x0,sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }
        
		if (g_td_parm.ovrd_intst_ind[0] == 'Y')
		{
			ret=pub_base_CalIntstDays(mtr_date,tx_date,
				intst_term_type,&dayterm);
	        if (ret != 0)
	        {
	            sprintf(acErrMsg,"计算天数错误!!");
	            WRITEMSG
	            return 1;
	        }
        }
        else
        {
			dayterm = 0;
		}

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

        lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
        (*dealval) += lx;
     
        if (strcmp(intst_tax_no,SLBH) == 0)
        {
			if (g_td_parm.ovrd_intst_ind[0] == 'Y')
            {
				ret=pub_base_CalIntstDays(PubMax(mtr_date,date99),tx_date, 
					intst_term_type,&dayterm);
	        	if (ret != 0)
	        	{
	            	sprintf(acErrMsg,"计算天数错误!!");
	            	WRITEMSG
	            	return 1;
	        	}
            }
            else
            {
                dayterm = 0;
            }

            lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
            (*val) += lx;
			g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
        }   

sprintf(acErrMsg,"到期+逾期：dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

    }

    (*dealval) = pub_base_PubDround_1(*dealval);

    
    ret = pub_base_GetRate(SLBH,tx_date,&sdsl);
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

sprintf(acErrMsg,"td_amt = [%lf],tx_cnt=[%d]", td_amt,tx_cnt);
WRITEMSG

    (*dealval) -= td_amt * tx_cnt;
	g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
    (*val) = (*val) * sdsl ;
    (*val)  = pub_base_PubDround_1(*val);
    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG
    
    return 0;
}
