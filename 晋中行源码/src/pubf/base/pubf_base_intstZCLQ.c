#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_intstZCLQ.c
* 功能描述：    整存零取计算利息
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
* 函数名：    pub_base_CalIntstZCLQ()
* 函数功能：  整存零取计算利息
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
*          1. 原来的算息方式不正确，因为是用倒扣积数的算息方式（开户先算出
*          至到期日的积数，以后每次取款扣除取款金额至到期日产生的积数）
*          所以开户应该计算出积数放到intst_acm字段。
*          2. 开户时交易日应该使用到期日计算利息。
*
********************************************************************/
int pub_base_CalIntstZCLQ(double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    long tx_date,ic_date,mtr_date;
    double  js99;    /*991101的积数*/
	double  ye99;    /*991101的余额*/
    double  intst_acm; 
    double  tx_amt;
    double  bal;
    long    tmprq,date99;
    long ac_id;
    int  ac_seqn;
    int ret;
    long dayterm;
    char intst_term_type[2];
    char llbh[4];
    char cur_no[3];
    double rate_val;
    double sdsl;
    double lx;
    double flt_ratio;
    char intst_tax_no[4];
    char rate_no[4];


sprintf(acErrMsg,"整存零取计息");
WRITEMSG

    date99 = 19991101;  

    tx_date = g_pub_tx.tx_date;
    tx_amt = g_pub_tx.tx_amt1;

    ic_date = g_td_mst.ic_date;
    mtr_date = g_td_mst.mtr_date;  
    ac_id = g_td_mst.ac_id;
    ac_seqn = g_td_mst.ac_seqn;
    intst_acm = g_td_mst.intst_acm;
    flt_ratio = g_td_mst.flt_ratio;
    bal = g_td_mst.bal;

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

    MEMSET_DEBUG(cur_no,0x0,sizeof(cur_no));
    MEMSET_DEBUG(intst_tax_no,0x0,sizeof(intst_tax_no));
    MEMSET_DEBUG(rate_no,0x0,sizeof(rate_no));

    strcpy(cur_no,g_td_parm.cur_no);
    strcpy(intst_tax_no,g_td_parm.intst_tax_no);
    strcpy(rate_no,g_td_parm.rate_no);

    /*取991101日的积数和余额*/
    if (ic_date >= date99)
    {
        js99 = 0.00;
        ye99 = 0.00;
    }
    else
    {
         /* 查询date99之前的定期存款明细表并给js99 ye99赋值 */

sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",ac_id,ac_seqn);
WRITEMSG

         tmprq = date99;
         ret = pub_base_gettdmsthst(ac_id,ac_seqn,&tmprq,&js99,&ye99,reply);
         if (ret != 0)
         {
             sprintf(acErrMsg,"取定期存款明细错误 [%d]",ret);
             WRITEMSG
             return 1;
         }
    }   
    
sprintf(acErrMsg,"js99=[%lf],ye99=[%lf]",js99,ye99);
WRITEMSG

		/* begin.gyf.20060918 */
    if (tx_date == ic_date && g_pub_tx.svc_ind == 1000)
    {
        tx_date = mtr_date;  /* 开户日计算到期利息 */
    }
    /* end.gyf.20060918 */

    if (mtr_date > tx_date)
    {
        /* 提前支取 */
 		ret=pub_base_CalIntstDays(tx_date,mtr_date,intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"计算天数错误!!");
            WRITEMSG
            return 1;
        }
        dayterm = dayterm + pub_base_tuneday(tx_date,ic_date,mtr_date);

sprintf(acErrMsg,"+++++++++intst_acm=[%lf]+++++++++++++",intst_acm);
WRITEMSG
        /***begin.gyf.20060918 积数有专门函数处理，这里不用处理
        intst_acm -= dayterm * tx_amt;
            end.gyf.20060918 ***/
        
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
            MEMSET_DEBUG(reply, 0x0, sizeof(reply));
            strcpy(reply,"W110");
            return ret;

        }

sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio);
WRITEMSG

        lx = rate_val * intst_acm * (1 + flt_ratio) / L360 / L100;

        if (ic_date >= date99)
        {
            (*val) = lx;
        }
        else
        {
            ret = pub_base_CalIntstDays(date99,mtr_date,
                  g_td_parm.intst_term_type,&dayterm);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"计算天数错误!!!");
            	WRITEMSG
            	return 1;
        	}

sprintf(acErrMsg,"dayterm=[%ld],js99=[%lf],ye99=[%lf],lx=[%lf],intst_acm=[%lf]",
        dayterm,js99,ye99,lx,intst_acm);
WRITEMSG

            (*val) = lx *(1 - (js99 -ye99 * dayterm) / intst_acm);
        }

        if (strcmp(intst_tax_no,SLBH) != 0)
        {
            (*val) = 0.00;
        }
        (*dealval) = pub_base_PubDround_1(lx);
        (*val) = pub_base_PubDround_1(*val);
    }
    else
    {
        /* 到期支取 */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
            strcpy(reply,"W110");
            return ret;
        }

sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio);
WRITEMSG

        lx = intst_acm * rate_val * (1 + flt_ratio) / L360 / L100;       
        
        if (strcmp(intst_tax_no,SLBH) == 0)
        {
            if (ic_date >= date99)
            {
                (*val) = lx;
            }
            else
            {
                ret = pub_base_CalIntstDays(date99,mtr_date,
                  	intst_term_type,&dayterm);
        		if (ret != 0)
        		{
            		sprintf(acErrMsg,"计算天数错误!!!");
            		WRITEMSG
            		return 1;
        		}

sprintf(acErrMsg,"dayterm=[%ld],js99=[%lf],ye99=[%lf],lx=[%lf],intst_acm=[%lf]",
        dayterm,js99,ye99,lx,intst_acm);
WRITEMSG

                (*val) = lx * (1 - (js99 - ye99 * dayterm) / intst_acm);
            }
        }
        
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
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

sprintf(acErrMsg,"dayterm=[%ld],rate_val=[%lf],bal=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,bal,flt_ratio);
WRITEMSG

        lx += bal * dayterm * rate_val * (1 + flt_ratio) / L360 / L100;

        if (strcmp(intst_tax_no,SLBH) == 0)
        {
            ret=pub_base_CalIntstDays(PubMax(date99,mtr_date),tx_date,
                intst_term_type,&dayterm);
            if (ret != 0)
            {
                sprintf(acErrMsg,"计算天数错误!!");
                WRITEMSG
                return 1;
            }

sprintf(acErrMsg,"dayterm=[%ld],rate_val=[%lf],bal=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,bal,flt_ratio);
WRITEMSG

            (*val) += bal *dayterm * rate_val * (1 + flt_ratio) / L360 / L100;
        }
    }

    lx = pub_base_PubDround_1(lx);
    (*dealval) = lx;

    /* 取利息税税率 */
    ret = pub_base_GetRate(SLBH, tx_date, &sdsl);
    if ((ret != 0) && (ret == 1))
    {
        sprintf(acErrMsg,"定期产品利息税税率不存在 %d",ret);
        WRITEMSG
        strcpy(reply,"W100");
        return 1;
    }
    else if(ret < 0)
    {
        sprintf(acErrMsg,"取利息税税率错误 %d ", ret);
        WRITEMSG
        strcpy(reply,"W101");
        return 1;
    }

	g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
    (*val) = (*val) * sdsl;
    (*val) = pub_base_PubDround_1(*val);
    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

    return 0;
}
