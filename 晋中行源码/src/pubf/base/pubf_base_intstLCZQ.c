#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_intstLCZQ.c
* 功能描述：    零存整取计算利息
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
#include "td_mst_hst_c.h"


/**********************************************************************
* 函数名：    pub_base_CalIntstLCZQ()
* 函数功能：  零存整取计算利息
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
int pub_base_CalIntstLCZQ(double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    long tx_date=0;
    long mtr_date=0;
    long ic_date=0; 
    long date99=0;
    long tmprq=0;  /* 存放date99之前最后一笔交易日期 */
    double	js99;	/*991101的积数*/
	double	ye99;	/*991101的余额*/
    long ac_id;     /* 帐户ID */
    int  ac_seqn;   /* 帐户序号 */
    long dayterm=0;
    char intst_term_type[2];
    double intst_acm;  /* 利息积数 */
    double tx_amt;     /* 发生额 */
    double rate_val;   /* 利率值 */
    char cur_no[3];    /* 币种 */
    char llbh[4];      /* 利率编号 */
    char rate_no[4];   /* 产品表中的利率编号 */
    char intst_tax_no[4]; /* 利息税税率编号 */
    double lx;            /* 利息 */
    double flt_ratio;     /* 浮动比率 */
    double sdsl;          /* 税率值 */
    int ret;
    long lzsbrq;	 	/*上笔发生日期*/
	long lzzcrq=0;	 	/*正常按月零存的最后一笔的日期*/
    double lzsbjs; 	/*上笔发生的积数*/
	double lzzcjs; 	/*正常按月存款的积数*/
    double lzsbye;	/*上笔余额*/
	double lzzcye; 	/*正常按月存款的余额*/
    long lzrq;	 		/*用来记录交易的日期;*/
    double lzjs;	 	/*用来记录积数*/
    double lzye;	/*余额*/
    double lzfse;	/*发生额*/
    double	khje;	/*零整开户存入的金额*/
    int lzflag=0;		/*0:正常按月存款;
						 1:有某个月份没有存款,下月补上;
						 2:下个月没有补存或没有存入当月存款*/
    double bal;
    struct td_mst_hst_c td_mst_hst_c;
    double dd_rate_val=0,hh_rate_val=0; /*定期和活期利率值,传给分段利息税函数省去里面取利率的操作*/  
    date99 = 19991101;
    tx_date = g_pub_tx.tx_date;
    tx_amt = g_pub_tx.tx_amt1;

    mtr_date = g_td_mst.mtr_date;
    ic_date = g_td_mst.ic_date;
    ac_id = g_td_mst.ac_id;
    ac_seqn = g_td_mst.ac_seqn;
    intst_acm = g_td_mst.intst_acm;
    flt_ratio = g_td_mst.flt_ratio;
    khje = g_td_mst.td_amt;
    bal = g_td_mst.bal;

    MEMSET_DEBUG(cur_no, 0x0, sizeof(cur_no));
    MEMSET_DEBUG(intst_tax_no, 0x0, sizeof(intst_tax_no));
    MEMSET_DEBUG(rate_no, 0x0, sizeof(rate_no));

    strcpy(cur_no,g_td_parm.cur_no);
    strcpy(intst_tax_no, g_td_parm.intst_tax_no);
    strcpy(rate_no,g_td_parm.rate_no);

    
    (*dealval) = 0.00;
    (*val) = 0.00;
    (*factval) = 0.00;
    (*keepval) = 0.00;

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

sprintf(acErrMsg,"零存整取计息" );
WRITEMSG
    
    if (tx_date < mtr_date)
    {   /* 提前支取 */

sprintf(acErrMsg,"零存整取计息--提前支取" );
WRITEMSG

        if (ic_date >= date99)
        {
TRACE
            js99 = 0.00;
            ye99 = 0.00;
        }
        else
        {   /* 查询date99之前的定期存款明细表并给js99 ye99赋值 */
TRACE
            tmprq = date99;
            ret = pub_base_gettdmsthst(ac_id,ac_seqn,&tmprq,&js99,&ye99,reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"取定期存款明细错误 [%d]",ret);
                WRITEMSG
                return 1;
            }
        }
        
        /********取款函数里已经计算了积数***
        ret = pub_base_CalIntstDays(tx_date,mtr_date,
                  g_td_parm.intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"计算天数错误!!!");
            WRITEMSG
            return 1;
        }
        dayterm = dayterm +  pub_base_tuneday(tx_date, ic_date, mtr_date);
        
        sprintf(acErrMsg,"intst_acm=[%lf]",intst_acm);
        WRITEMSG

        intst_acm -= dayterm * tx_amt;
        ******/
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date );
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
hh_rate_val=rate_val;
sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio );
WRITEMSG
        if (floor(intst_acm * 100) / 100 <= 0.00)
        {
            lx = 0.00;
            (*dealval) = 0.00;
            (*val)     = 0.00;
        }
        else if (ic_date >= date99)
        {

sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio );
WRITEMSG

            lx = rate_val * intst_acm * (1 + flt_ratio) / L360 / L100;
            (*dealval) = pub_base_PubDround_1( lx );
            (*val) = (*dealval);
            if (strcmp(intst_tax_no,SLBH) != 0)
            { 
                (*val) = 0.00;  
            }
        }
        else
        {

sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio );
WRITEMSG

            lx = rate_val * intst_acm * (1 + flt_ratio) / L360 / L100;
            (*dealval) = pub_base_PubDround_1(lx);

            if (strcmp(intst_tax_no,SLBH) == 0)
            { 
                ret=pub_base_CalIntstDays(date99,mtr_date,
                               intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"计算天数错误!!");
                    WRITEMSG
                    return 1;
                }
                (*val) = lx * (1 - (js99 - ye99 * dayterm) / intst_acm);

            }
        }
    }   
    else
    {   /* 到期支取 逾期支取 */

sprintf(acErrMsg,"零存整取计息--到期或逾期支取" );
WRITEMSG

sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",ac_id,ac_seqn);
WRITEMSG

		/******
        ret = Td_mst_hst_Dec_Sel(reply,"ac_id = %ld and ac_seqn = %d \
        	order by tx_date,trace_no ",ac_id, ac_seqn);
        if (ret != 0)
        {
            sprintf(acErrMsg, "prepare error code =[%d]",ret);
            WRITEMSG
            return ret;
        }       

        MEMSET_DEBUG(&td_mst_hst_c,0x0,sizeof(struct td_mst_hst_c));
        ret = Td_mst_hst_Fet_Sel(&td_mst_hst_c, reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg, "fetch com_rate error [%d] ", ret);
            WRITEMSG
            * strcpy(g_pub_tx.reply, "D103"); *
            Td_mst_hst_Clo_Sel();
            return ret;

        }
        else if (ret == 100)
        {
            sprintf(acErrMsg, "NOFOUND in td_mst_hst !! code =[%d]",ret);
            WRITEMSG
            Td_mst_hst_Clo_Sel();
            return ret;
        }

        lzsbrq=td_mst_hst_c.tx_date;
        lzsbjs=td_mst_hst_c.intst_acm;
        lzsbye=td_mst_hst_c.bal;
        lzzcrq=td_mst_hst_c.tx_date;
        lzzcjs=td_mst_hst_c.intst_acm;
        lzzcye=td_mst_hst_c.bal;
        lzrq = td_mst_hst_c.tx_date;
        lzjs = td_mst_hst_c.intst_acm;
        lzye = td_mst_hst_c.bal;
        lzfse = td_mst_hst_c.tx_amt;

sprintf(acErrMsg,"零存整取计息--判断正常零存整取" );
WRITEMSG
sprintf(acErrMsg,"lzsbrq=[%ld],lzsbjs=[%lf],lzsbye=[%lf]",lzsbrq,lzsbjs,lzsbye );
WRITEMSG

        while(1)
        {

sprintf(acErrMsg,"lzflag=[%d] ",lzflag);
WRITEMSG

            if (lzflag == 0)
            {

sprintf(acErrMsg,"0 正常续存");
WRITEMSG

                if (pub_base_getyear(lzrq) == pub_base_getyear(lzsbrq) + 1)
                {

sprintf(acErrMsg,"跨年" );
WRITEMSG

                    *跨年*
               if (pub_base_getmonth(lzrq) + 12 <= pub_base_getmonth(lzrq) + 1)
                    {
                        lzzcrq = lzrq;
                        lzzcjs = lzjs;
                        lzzcye = lzye;
                    }
              else if (pub_base_getmonth(lzrq)+12 == pub_base_getmonth(lzrq)+2)
                    {
                        if (lzfse >= khje * 2)
                        {
                            lzzcrq = lzrq;
                            lzzcjs = lzjs;
                            lzzcye = lzye;
                        }
                        else
                        {
                            lzflag = 1;
                        }
                    }
                    else
                    {
                        lzflag = 2;
                        break;
                    }
                }
                else *不跨年*
                {

sprintf(acErrMsg,"不跨年" );
WRITEMSG

                    if (pub_base_getmonth(lzrq)<=pub_base_getmonth(lzsbrq)+1)
                    {
                        *  每月正常存入 *

sprintf(acErrMsg,"每月正常存入 lzrq=[%d],lzsbrq=[%d]",
        pub_base_getmonth(lzrq),pub_base_getmonth(lzsbrq));
WRITEMSG
                        lzzcrq = lzrq;
                        lzzcjs = lzjs;
                        lzzcye = lzye;
                    }
                    else* 某月漏存 *
                    {
                  
sprintf(acErrMsg,"某月漏存" );
WRITEMSG

                   if (pub_base_getmonth(lzrq) == pub_base_getmonth(lzsbrq)+2)
                        {
                            if (lzfse >= khje * 2)
                            {
                                lzzcrq = lzrq;
                                lzzcjs = lzjs;
                                lzzcye = lzye;
                            }
                            else 
                            {
                                lzflag = 1;
                            }
                        }
                        else
                        {   * 第二月未补存 *
                            lzflag = 2;
                            break;
                        }
                    }
                } * end else 不跨年*
            }  * end lzflag =0 *
            else if (lzflag == 1) *某月漏存,下月补上*
            {

sprintf(acErrMsg,"1某月漏存，下月补上" );
WRITEMSG

                if ((pub_base_getyear(lzrq)==pub_base_getyear(lzsbrq)) \
                    && (pub_base_getmonth(lzrq)==pub_base_getmonth(lzsbrq)))
                {
                    lzflag = 0;
                    lzzcrq = lzrq;
                    lzzcjs = lzjs;
                    lzzcye = lzye;
                }
                else
                {
                    lzflag = 2;
                    break;
                }
            } *end lzflag==1*
            else
            {

sprintf(acErrMsg,"2某月漏存，下月未补上" );
WRITEMSG

                break; *其他情况* 
            }
            lzsbrq = lzrq;
            lzsbjs = lzjs;
            lzsbye = lzye;

            MEMSET_DEBUG(&td_mst_hst_c,0x0,sizeof(struct td_mst_hst_c));
            ret = Td_mst_hst_Fet_Sel(&td_mst_hst_c, reply);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg, "fetch com_rate error [%d] ", ret);
                WRITEMSG
                * strcpy(g_pub_tx.reply, "D103"); *
                Td_mst_hst_Clo_Sel();
                return ret;
            }
            else if (ret == 100)
            {
                break;
            }
            lzrq = td_mst_hst_c.tx_date;
            lzjs = td_mst_hst_c.intst_acm;
            lzye = td_mst_hst_c.bal;
            lzfse = td_mst_hst_c.tx_amt;

        }  *end while取正常按月存款的日期和积数完毕*
        Td_mst_hst_Clo_Sel();
		**/

sprintf(acErrMsg,"lzzcrq=[%ld],lzzcjs=[%lf],lzzcye=[%lf]",
        lzzcrq,lzzcjs,lzzcye);
WRITEMSG
        
        if (ic_date >= date99)
        {
            js99 = 0.00;
            ye99 = 0.00;
        }
        else
        {
            /* 查询date99之前的定期存款明细表并给js99 ye99赋值 */
            tmprq = date99;
            js99 = 0.00;
            ye99 = 0.00;
            ret = pub_base_gettdmsthst(ac_id,ac_seqn,&tmprq,&js99,&ye99,reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"取定期存款明细错误 [%d]",ret);
                WRITEMSG
                return 1;
            }

sprintf(acErrMsg,"js99=[%lf],ye99=[%lf],tmprq=[%ld],ac_id=[%ld],ac_seqn=[%d]",
        js99,ye99,tmprq,ac_id,ac_seqn);
WRITEMSG

        }
        
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],ic_date=[%ld],rate_val=[%lf]",
        llbh,cur_no,ic_date,rate_val);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,ic_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
            strcpy(reply,"W110");
            return ret;
        }
dd_rate_val=rate_val;
sprintf(acErrMsg,"零存整取正常计息部分 rate_val=[%lf],lzzcjs=[%lf], \
	flt_ratio=[%lf], js=[%lf]", rate_val,lzzcjs,flt_ratio, g_td_mst.intst_acm);
WRITEMSG

        /* lx = lzzcjs * rate_val * (1 + flt_ratio) / L360 / L100; */
        lx = g_td_mst.intst_acm * rate_val * (1 + flt_ratio) / L360 / L100;
        (*dealval) = lx;

sprintf(acErrMsg,"零存整取正常计息部分 dealval =[%lf]",*dealval);
WRITEMSG

        if (ic_date >= date99)
        {
            if (strcmp(intst_tax_no,SLBH) == 0)
            {
                (*val) = lx;
            }
        }
        else if( lzzcjs > 0.00)
        {
            if(strcmp(intst_tax_no,SLBH) == 0)
            {
                ret=pub_base_CalIntstDays(date99,mtr_date,
                               intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"计算天数错误!!");
                    WRITEMSG
                    return 1;
                }

                if (lzzcrq < date99)
                {
                    (*val) = lx * (1 - (lzzcjs - lzzcye * dayterm) / lzzcjs);
                }
                else
                {
                    (*val) = lx * (1 - (js99 - ye99 * dayterm) /lzzcjs);
                }
            }
        }
        else
        {
            (*val) = 0.00;
        }
       
sprintf(acErrMsg,"零存整取正常计息部分 val =[%lf]",*val);
WRITEMSG

        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
        strcpy(llbh, HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],ic_date=[%ld],rate_val=[%lf]",
        llbh,cur_no,ic_date,rate_val);
WRITEMSG

        ret = pub_base_getllz(llbh, cur_no, tx_date, &rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
            MEMSET_DEBUG(reply, 0x0, sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }
hh_rate_val=rate_val;
sprintf(acErrMsg,"存期内不满足零整条件部分的计息");
WRITEMSG

sprintf(acErrMsg,"intst_acm=[%lf],lzzcjs=[%lf],rate_val=[%lf],flt_ratio=[%lf]",
        intst_acm,lzzcjs,rate_val,flt_ratio);
WRITEMSG

        /*intst_acm-lzzcjs:存期内不满足零整条件部分的积数 delete by rob at 1029*
        (*dealval) +=(intst_acm-lzzcjs) * rate_val * (1+flt_ratio)/L360/L100;*/

sprintf(acErrMsg,"零存整取不正常部分和正常部分之和 dealval =[%lf]",*dealval);
WRITEMSG

        if (strcmp(intst_tax_no,SLBH) == 0)
        {
            if (date99 <= lzzcrq)
            {vtcp_log("[%s][%d]lzzcrq=[%ld],mtr_date=[%ld]",__FILE__,__LINE__,lzzcrq,mtr_date );
                ret=pub_base_CalIntstDays(lzzcrq,mtr_date,
                               intst_term_type,&dayterm);
                vtcp_log("[%s][%d]lzzcrq=[%ld],mtr_date=[%ld]",__FILE__,__LINE__,lzzcrq,mtr_date );
                if (ret != 0)
                {
                    sprintf(acErrMsg,"计算天数错误!!");
                    WRITEMSG
                    return 1;
                }

                (*val) +=(intst_acm-lzzcjs)*rate_val*(1+flt_ratio)/L360/L100;
            }
            else if (date99 <= mtr_date)
            {vtcp_log("[%s][%d]lzzcrq=[%ld],mtr_date=[%ld]",__FILE__,__LINE__,lzzcrq,mtr_date );
                ret=pub_base_CalIntstDays(date99,mtr_date,
                               intst_term_type,&dayterm);
                vtcp_log("[%s][%d]lzzcrq=[%ld],mtr_date=[%ld]",__FILE__,__LINE__,lzzcrq,mtr_date );
                if (ret != 0)
                {
                    sprintf(acErrMsg,"计算天数错误!!");
                    WRITEMSG
                    return 1;
                }

                (*val) +=(intst_acm-js99+(ye99-lzzcye)*dayterm)*rate_val \
                         * (1 + flt_ratio) / L360 / L100;
            }
        }

sprintf(acErrMsg,"零存整取不正常部分和正常部分之和 val =[%lf]",*val);
WRITEMSG

        /*逾期部分的利息用余额计算*/
        ret=pub_base_CalIntstDays(mtr_date,tx_date,
                               intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"计算天数错误!!");
            WRITEMSG
            return 1;
        }


sprintf(acErrMsg,"逾期 bal=[%lf],dayterm=[%d],rate_val=[%lf],flt_ratio=[%lf]",
        bal,dayterm,rate_val,flt_ratio);
WRITEMSG

		sprintf(acErrMsg, "--------CGH---逾期加正常利息:[%lf]", *dealval);
		WRITEMSG

        (*dealval) += floor(bal*L100)/L100 * dayterm * rate_val * (1+flt_ratio)/L360/L100;

		sprintf(acErrMsg, "--------CGH---逾期加正常利息:[%lf]", *dealval);
		WRITEMSG

        if (strcmp(intst_tax_no,SLBH) == 0)
        {
            ret=pub_base_CalIntstDays(PubMax(mtr_date,date99),tx_date,
                               intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"计算天数错误!!");
                    WRITEMSG
                    return 1;
                }

            (*val) +=floor(bal*L100)/L100 *dayterm *rate_val * (1+flt_ratio)/L360/L100;
        }        
        
    }
    
	if( g_pub_intst.edu_ind[0]=='1' )
	{
		*val=0.00;
	}
    else if( strcmp(intst_tax_no, SLBH) == 0)
    {
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
		if (*val > *dealval)
			*val = *dealval;
        (*val) = (*val) * sdsl;
        (*val) = pub_base_PubDround_1(*val);
    }
    (*dealval) = pub_base_PubDround_1(*dealval);  
    (*factval) = (*dealval) - (*val);
    (*keepval) = 0.00;

sprintf(acErrMsg,"调用分段算税前函数dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf] ",
        *dealval,*factval,*val,*keepval);
WRITEMSG
	/* added by liuxuan 2007-08-01 调整利息税 */
	if( strcmp(intst_tax_no, SLBH) == 0){
		char edu_ind[2];
		vtcp_log("[%s][%d]开始调用分段算利息税函数\n",__FILE__,__LINE__);
		if(iGetAreaTax(val,factval,dealval,intst_term_type,1,dd_rate_val,hh_rate_val,edu_ind)){
			vtcp_log("[%s][%d]调用分段算利息税函数出错\n",__FILE__,__LINE__);
			return(1);
		}
	}

sprintf(acErrMsg,"调用分段算税后函数dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf] ",
        *dealval,*factval,*val,*keepval);
WRITEMSG
	/* add over */
    return 0;
}
