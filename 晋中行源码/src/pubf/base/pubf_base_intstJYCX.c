#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_intstJYCX.c
* 功能描述：    零存整取计算利息(教育储蓄)
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
* 函数名：    pub_base_CalIntstJYCX()
* 函数功能：  零存整取计算利息
* 作者/时间： 2003年12月28日
*
* 参数：
*     输入：
*     输出: 应付利息          double
*           实付利息          double
*           代扣利息税        double
*           保值利息          double
*	    教育证明	      char(2)  1-有 0-无
*           响应码            char(4)
* 返 回 值: 0 成功
*
* 修改历史：
*
********************************************************************/
int pub_base_CalIntstJYCX(double *dealval, double *factval,
                  double *val, double *keepval, char edu_ind[2], char *reply)
{
	char llbh[4];
	int ret;
	double sdsl;
	long tmprq, date99;
	double ye99,js99;
	double lx;
	long dayterm;
	double rate_val;
    double dd_rate_val=0,hh_rate_val=0; /*定期和活期利率值,传给分段利息税函数省去里面取利率的操作*/
	*dealval = 0.00;
	*factval = 0.00;
	*val = 0.00;
	*keepval = 0.00;

	sprintf(acErrMsg, "dealval=[%lf], factval=[%lf], val=[%lf], keepval=[%lf]",
		*dealval, *factval, *val, *keepval);
	WRITEMSG

	date99 = 19991101;

	if (edu_ind[0] == 'Y')
	{
		sprintf(acErrMsg, "有教育证明的教育储蓄");
		WRITEMSG
		if (g_pub_tx.tx_date < g_td_mst.mtr_date)
		{
			sprintf(acErrMsg, "未到期支取");
			WRITEMSG

			ret = pub_base_CalMouths(g_td_mst.opn_date, g_pub_tx.tx_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "计算月数错误!! [%ld], [%ld]", 
					g_td_mst.opn_date, g_pub_tx.tx_date);
				WRITEMSG
				return ret;
			}

			if (ret < 3)
			{
				/* 按活期 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, HQLLBH);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_pub_tx.tx_date, &rate_val); 
			}
			else if ( ret < 6 )
			{
				/* 按三个月 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ03);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else if ( ret < 12 )
			{
				/* 按六个月 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ06);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else if (ret < 24)
			{
				/* 按整存整取一年 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ12);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else if ( ret < 36 )
			{
				/* 按整存整取二年  */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ24);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else if ( ret < 60 )
			{
				/* 按整存整取三年 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ36);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else 
			{
				/* 按整存整取五年 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, "205");
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}


sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}

			*dealval = g_td_mst.intst_acm * rate_val * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
			(*dealval) = pub_base_PubDround_1( *dealval );
			(*val) = 0.00;
			(*factval) = (*dealval);
dd_rate_val=rate_val;
		}
		else
		{
			/* 到期或逾期 */
			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, g_td_parm.rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
dd_rate_val=rate_val;
			/* 到期利息 */
sprintf(acErrMsg, "intst_acm=[%lf], rate_val=[%lf] ", g_td_mst.intst_acm, rate_val);
WRITEMSG

			*dealval = g_td_mst.intst_acm * rate_val * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
			(*val) = 0.00;

sprintf(acErrMsg, "利息值:[%lf], 利息税值:[%lf]", *dealval, *val);
WRITEMSG

			/* 逾期利息 */

			ret=pub_base_CalIntstDays(g_td_mst.mtr_date, 
					g_pub_tx.tx_date, g_td_parm.intst_term_type,&dayterm); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"计算天数错误!!"); 
				WRITEMSG 
				return 1; 
			}

			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_pub_tx.tx_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
hh_rate_val=rate_val;
			(*val) = rate_val * dayterm * g_pub_tx.tx_amt1 * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
			
			(*dealval) += (*val);
			(*dealval) = pub_base_PubDround_1( *dealval );
		}
	}
	else if (edu_ind[0] == 'N')
	{
		sprintf(acErrMsg, "无教育证明的教育储蓄");
		WRITEMSG
		if (g_pub_tx.tx_date < g_td_mst.mtr_date)
		{
			sprintf(acErrMsg, "未到期支取");
			WRITEMSG
			
			/* 按活期利率计算 */
			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_pub_tx.tx_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
hh_rate_val=rate_val;
			lx = g_td_mst.intst_acm * rate_val * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"lx=[%lf],rate_val=[%lf]date99=[%ld]",lx,rate_val,date99);
WRITEMSG					
			(*dealval) = pub_base_PubDround_1( lx );
			
			if (g_td_mst.ic_date >= date99)
        		{
TRACE
            			js99 = 0.00;
           			ye99 = 0.00;
        		}
        		else
        		{   /* 查询date99之前的定期存款明细表并给js99 ye99赋值 */
TRACE
           	 		tmprq = date99;
            		ret = pub_base_gettdmsthst(g_td_mst.ac_id,
							g_td_mst.ac_seqn,&tmprq,&js99,&ye99,reply);
            		if (ret != 0)
            		{
                		sprintf(acErrMsg,"取定期存款明细错误 [%d]",ret);
                		WRITEMSG
                		return 1;
            		}
        		}

 			ret=pub_base_CalIntstDays(date99,g_td_mst.mtr_date,
 				g_td_parm.intst_term_type,&dayterm);
 			if (ret != 0)
 			{
 				sprintf(acErrMsg,"计算天数错误!!");
 				WRITEMSG
				 return 1;
 			}
 			/* (*val) = lx * (1 - (js99 - ye99 * dayterm) / g_td_mst.intst_acm);*/
			(*val) = lx;

			(*dealval) = pub_base_PubDround_1( *dealval );

		}
		else
		{
			/* 到期或逾期 */
			sprintf(acErrMsg, "到期或逾期支取!!");
			WRITEMSG
			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, g_td_parm.felbck_rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
dd_rate_val=rate_val;
sprintf(acErrMsg,"lx=[%lf],rate_val=[%lf]date99=[%ld]",lx,rate_val,date99);
WRITEMSG			
			/* 到期利息 */
			lx = g_td_mst.intst_acm * rate_val * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"lx=[%lf],rate_val=[%lf]date99=[%ld]",lx,rate_val,date99);
WRITEMSG
			(*dealval) = lx;
sprintf(acErrMsg,"lx=[%lf],rate_val=[%lf]dealval=[%ld]",lx,rate_val,dealval);
WRITEMSG				
			/* 到期部分利息税 */	
			if (g_td_mst.ic_date >= date99)
        	{
TRACE
           		TRACE
           		js99 = 0.00;
           		ye99 = 0.00;
        	}
        	else
        	{   /* 查询date99之前的定期存款明细表并给js99 ye99赋值 */
TRACE
           		tmprq = date99;
           		ret = pub_base_gettdmsthst(g_td_mst.ac_id, 
						g_td_mst.ac_seqn,&tmprq,&js99,&ye99,reply);
           		if (ret != 0)
           		{
               		sprintf(acErrMsg,"取定期存款明细错误 [%d]",ret);
               		WRITEMSG
               		return 1;
           		}
        	}
			
			/***
			if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)

			****/
 			ret=pub_base_CalIntstDays(date99,g_td_mst.mtr_date,
 				g_td_parm.intst_term_type,&dayterm);
 			if (ret != 0)
 			{
 				sprintf(acErrMsg,"计算天数错误!!");
 				WRITEMSG
				 return 1;
 			}
 			/* (*val) += lx * (1 - (js99 - ye99 * dayterm) / g_td_mst.intst_acm);*/
			(*val) = lx;
			sprintf(acErrMsg, "CGH***到期部分利息税:[%lf]", *val);
			WRITEMSG

			/* 逾期利息 */

			ret=pub_base_CalIntstDays(g_td_mst.mtr_date, 
					g_pub_tx.tx_date, g_td_parm.intst_term_type,&dayterm); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"计算天数错误!!"); 
				WRITEMSG 
				return 1; 
			}

			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_pub_tx.tx_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
hh_rate_val=rate_val;
			lx = rate_val * dayterm * g_pub_tx.tx_amt1 * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
			
			(*dealval) += lx;
			
			/* 逾期部分利息 */
			/*****
			if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
 			
			*******/

			(*val) += lx;
			sprintf(acErrMsg, "逾期部分利息税:[%lf]", lx);
			WRITEMSG

			(*dealval) = pub_base_PubDround_1( *dealval );
		}
	}
	else
	{
		sprintf(acErrMsg, "教育证明标志错误!! [%s]", edu_ind);
		WRITEMSG
		return 1;
	}
	
   	if( strcmp(g_td_parm.intst_tax_no, SLBH) == 0 || pub_base_CompDblVal((*val), 0.00) == 1)
   	{
 		/* 取利息税税率 */
       	ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
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
    }
	(*factval) = (*dealval) - (*val);

	/* added by liuxuan 2007-08-01 修改利息税 */
	if(g_pub_tx.tx_date<=g_td_mst.mtr_date && edu_ind[0]=='Y'){
		vtcp_log("[%s][%d]刚到期或者未到期且有证明则不收税,不调用分段算息函数\n",__FILE__,__LINE__);
		return(0);
	}
	if(strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
		vtcp_log("[%s][%d]开始调用分段算利息税函数\n",__FILE__,__LINE__);
		if(iGetAreaTax(val,factval,dealval,g_td_parm.intst_term_type,7,dd_rate_val,hh_rate_val,edu_ind)){
			vtcp_log("[%s][%d]调用分段算利息税函数出错\n",__FILE__,__LINE__);
			return(1);
		}
	}
	/* add over */
	return 0;
}
