#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_intstHQ.c
* 功能描述：    活期计算利息
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
#include "mo_bail_rate_c.h"
#include "dd_mst_c.h"
#include "mo_sxc_c.h"
#include "mo_sxc_hst_c.h"
#include "mo_sxc_hst_intst_c.h"

/**********************************************************************
* 函数名：    pub_base_CalIntstHQ()
* 函数功能：  活期计算利息
* 作者/时间： 2003年12月28日
*
* 参数：
*     输入：
            模式              int
            1--销户计息 2--结息日计息
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
int pub_base_CalIntstHQ(int mode,double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    double lxjs;          /* 利息积数 */   
    double rate_val;      /* 利率值 */
    double sdsl;          /* 税率值 */
    double lx;            /* 利息 */
    double flt_ratio;     /* 浮动比率 */
    double tx_amt;        /* 发生额 */
    char intst_term_type[2]; /* 计息天数类型 */
    char llbh[4];         /* 利率编号 */
    char cur_no[3];       /* 币种 */
    char tax_no[4];       /*税率编号 */
    long tx_date;         /* 交易日期 */
    long grjxrq ;         /* 个人结息日期 */
    int year,month,day;
    long dayterm;
    int ret;

    struct mo_bail_rate_c g_mo_bail_rate_c;
    memset( &g_mo_bail_rate_c,0,sizeof(struct mo_bail_rate_c));

sprintf(acErrMsg,"活期计息函数开始调用!!");
WRITEMSG
    (*dealval) = 0.00;
    (*factval) = 0.00;
    (*val)   = 0.00;
    (*keepval) = 0.00;   
    
    lxjs = g_dd_mst.intst_acm;
    flt_ratio = g_dd_mst.flt_ratio;

    strcpy(intst_term_type,g_dd_parm.intst_term_type);

    tx_amt = g_pub_tx.tx_amt1;
    tx_date = g_pub_tx.tx_date;
  
    MEMSET_DEBUG(cur_no, 0x0, sizeof(cur_no));
    MEMSET_DEBUG(tax_no, 0x0, sizeof(tax_no));
    strcpy(cur_no, g_dd_parm.cur_no);
    strcpy(tax_no, g_dd_parm.tax_no);
    
	if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
         sprintf(acErrMsg,"产品表中的计息天数类型错误 type=[%s]",
                        g_dd_parm.intst_term_type );
         WRITEMSG
         MEMSET_DEBUG(reply, 0x0, sizeof(reply));
         strcpy(reply, "W107");
         return 1;
    }

    pub_base_get_date(&year,&month,&day);
    if (month > 6)
    {
        grjxrq = (year + Lone) * 10000 + 06 * 100 + 30;
    }
    else
    {
        grjxrq = year * 10000 + 06 * 100 + 30;
    }

    mode = 1; /* 不用计算积数 */
 
    switch (mode)
    {
        case 1 :     /* 销户计息 */
            

sprintf(acErrMsg,"销户计息");
WRITEMSG
            /* 计算销户日到结息日的天数 *
			ret=pub_base_CalIntstDays(tx_date,grjxrq,
                intst_term_type,&dayterm);
            if (ret != 0)
            {
                sprintf(acErrMsg,"计算天数错误!!");
                WRITEMSG
                return 1;
            }

            dayterm = dayterm + Lone;
            
            if (tx_amt < 0.005)
            {
                tx_amt = - floor(-tx_amt*L100)/L100;
            }
            else
            {
                tx_amt = floor(tx_amt*L100)/L100;
            }
            
            * 计算利息积数 *

sprintf(acErrMsg,"lxjs=[%lf], tx_amt=[%lf],dayterm=[%ld]",lxjs,tx_amt,dayterm);
WRITEMSG

            lxjs = lxjs - tx_amt * dayterm;

sprintf(acErrMsg,"lxjs=[%lf]",lxjs);
WRITEMSG
            *********/
            
            /* 取活期利率 */
            MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			/*modified by liuxuan 20061102 按活期参数表取利率编号计算利息 */
				/**modified by ligl 2007-1-4 15:33***/
				if(memcmp(g_dd_parm.rate_no,"B00",3)==0)/**如果为一户通取活期利率**/
				{
            	strcpy(llbh,HQLLBH);
				}
				else
				/*******end********/
            strcpy(llbh,g_dd_parm.rate_no);
			if(strlen(llbh)==0)
            	strcpy(llbh,HQLLBH);
			/* modify over */

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

	if (memcmp(g_dd_parm.prdt_no,"131",3)==0)
	{
		ret=Mo_bail_rate_Sel(g_pub_tx.reply,&g_mo_bail_rate_c,"bail_ac_no='%s'",g_pub_tx.ac_no);
		if (ret==0)
		{	
		rate_val=g_mo_bail_rate_c.ic_rate;
		}
		else
		{
		sprintf(acErrMsg,"查找出错!![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		return ret;
		}
	}else 	if (memcmp(g_dd_parm.prdt_no,"142",3)==0)
	{
		rate_val=g_dd_mst.rate;
	}
	else
	{
		ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
            if (ret != 0)
            {
                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                WRITEMSG
                MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                strcpy(reply,"W110");
                return ret;
            }
	}
		/* added by liuxuan 20071010 处理协定存款户 */
		struct dd_mst_c tmp_dd_mst;
		memset(&tmp_dd_mst,'\0',sizeof(tmp_dd_mst));
		ret = Dd_mst_Sel(g_pub_tx.reply, &tmp_dd_mst, "ac_id=%ld and ac_seqn=%d",
		g_pub_tx.ac_id, g_pub_tx.ac_seqn);
		if (ret != 0 && ret!=100)
		{
			sprintf(acErrMsg, "查询活期主文件错误!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			return 1;
		}
		if(g_dd_mst.ac_type[0]=='6'){
			vtcp_log("[%s][%d]此户[%ld]是协定存款户,取分户利率\n",__FILE__,__LINE__,g_pub_tx.ac_id);
			rate_val=g_dd_mst.rate;
		}
		if (memcmp(g_dd_mst.prdt_no,"142",3)==0)/**  担保保证金取分户利率 add by chenchao 20120619**/
		{
			rate_val = g_dd_mst.rate;
		}
		/* added over */

sprintf(acErrMsg,"rate_val=[%lf],lxjs=[%lf],flt_ratio=[%lf]，g_dd_mst.ac_type[0]=[%c],g_dd_mst.prdt_no=[%s]",
        rate_val,lxjs,flt_ratio,g_dd_mst.ac_type[0],g_dd_mst.prdt_no);
WRITEMSG

            lx = rate_val * lxjs * (1 + flt_ratio) / L360 / L100;
            
            if (strcmp(tax_no,SLBH) == 0)
            {
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
				g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
                (*val) = lx * sdsl;
				vtcp_log("[%s][%d]税率是[%.5f]\n",__FILE__,__LINE__,sdsl);
            }
            
            (*dealval) = pub_base_PubDround_1(lx);
            (*val)     = pub_base_PubDround_1(*val);

            if (*dealval < 0)
            {
                (*dealval) = 0.00;
                (*val)     = 0.00;
                (*factval) = 0.00;
            }
            
            (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

            break;

        case 2 :     /* 结息日计息 */

sprintf(acErrMsg,"结息日计息");
WRITEMSG

            /* 取活期利率 *
            if (month != 6 || day != 30)
            {
                sprintf(acErrMsg, "今天不是结息日 [%d]/[%d]/[%d]",
                        year, month, day);
                WRITEMSG
                MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                strcpy(reply, "W113");
                return 1;
            }*/
            MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
            strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

	if (memcmp(g_dd_mst.prdt_no,"142",3)==0)/**  担保保证金取分户利率 add by chenchao 20120619**/
	{
		rate_val = g_dd_mst.rate;
	}
	else
	{
            ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
            if (ret != 0)
            {
                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                WRITEMSG
                MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                strcpy(reply,"W110");
                return ret;
            }
        }
            /* added by liuxuan 20071010 处理协定存款户 */
		struct dd_mst_c tmp_dd_mst1;
		memset(&tmp_dd_mst1,'\0',sizeof(tmp_dd_mst1));
		ret = Dd_mst_Sel(g_pub_tx.reply, &tmp_dd_mst1, "ac_id=%ld and ac_seqn=%d",
		g_pub_tx.ac_id, g_pub_tx.ac_seqn);
		if (ret != 0 && ret!=100)
		{
			sprintf(acErrMsg, "查询活期主文件错误!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			return 1;
		}
		if(g_dd_mst.ac_type[0]=='6'){
			vtcp_log("[%s][%d]此户[%ld]是协定存款户,取分户利率\n",__FILE__,__LINE__,g_pub_tx.ac_id);
			rate_val=g_dd_mst.rate;
		}
sprintf(acErrMsg,"lxjs=[%lf],rate_val=[%lf],flt_ratio=[%lf]",
        lxjs,rate_val,flt_ratio);
WRITEMSG

            lx = lxjs * rate_val * (1 + flt_ratio) / L360 / L100;
            
            if (strcmp(tax_no,SLBH) == 0)
            {
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
				g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
                (*val) = lx * sdsl;
            }
            
            (*dealval) = pub_base_PubDround_1(lx);
            (*val)     = pub_base_PubDround_1(*val);

            if (*dealval < 0)
            {
                (*dealval) = 0.00;
                (*val)     = 0.00;
                (*factval) = 0.00;
            }
            
            (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        dealval,factval,val,keepval);
WRITEMSG

            break;
        default :
            break;
    }
    
sprintf(acErrMsg,"活期计息函数调用结束!!");
WRITEMSG
    return 0;
}

/******************************************************************
 ****    计算遂心存利息  add by zqbo 20141121                  ****
 ****    参数说明：                                            ****
 ****    tx_amt 释放金额，如果为遂心存登记簿余额表示全释放     ****
 ****    flag   标志：                                         ****
 ****               '0'-试算利息、不需要登记明细               ****
 ****               '2'-释放算息、需要登记明细                 ****
 ****               '4'-解约结息、需要登记明细                 ****
 ******************************************************************/
 
int iSxc_intst(long ac_id, long ac_seqn, double tx_amt, double *intst, char flag)
{
    int iRet = 0;
    double dIntst = 0.00;        /**总共结出利息返回值**/
    double dIntst_hst = 0.00;    /**每笔释放的结出利息**/
    double dTx_amt = 0.00;       /**每笔需释放金额**/
    double dSy_tx_amt = tx_amt;  /**剩余释放金额=初始化为释放金额**/
    
    struct mo_sxc_hst_c       sMo_sxc_hst;
    struct mo_sxc_hst_c       sMo_sxc_hst1;
    memset(&sMo_sxc_hst,       0x00, sizeof(sMo_sxc_hst));
    memset(&sMo_sxc_hst1,      0x00, sizeof(sMo_sxc_hst1));
    
    if(flag != '2' && flag != '4' && flag != '0')
    {
        strcpy(g_pub_tx.reply, "S036");
        sprintf( acErrMsg, "传入的flag标志[%c]必须为2 4 0!", flag );
        WRITEMSG
        goto ErrExit;
    }
    
    iRet = Mo_sxc_hst_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%ld and sts='1' order by rate_date desc ", ac_id, ac_seqn); /**排序时无法用Dec_Upd**/
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D101");
        sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    while(1)
    {
        /***每次循环初始化***/
        dIntst_hst = 0.00;
        dTx_amt = 0.00;
        memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
        
        iRet = Mo_sxc_hst_Fet_Sel(&sMo_sxc_hst, g_pub_tx.reply);
        if(iRet == 100)
        {
            break;
        }
        else if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!!" );
            WRITEMSG
            goto ErrExit;
        }
        /***确定每条明细中需释放的金额***/
        if(pub_base_CompDblVal(dSy_tx_amt, sMo_sxc_hst.hst_bal) <0 )
        {
            dTx_amt = dSy_tx_amt;
        }
        else
        {
            dTx_amt = sMo_sxc_hst.hst_bal;
        }
        
        /**------------计算释放金额的利息 并登记结息明细-------------***/
        iRet = iSxc_hst_intst(sMo_sxc_hst, dTx_amt, &dIntst_hst, flag);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf(acErrMsg, "利息支出科目记账错误!!");
            WRITEMSG
            Mo_sxc_hst_Clo_Sel();
            goto ErrExit;
        }
        
        /**----------------更新原登记明细----------------**/
        if(flag != '0')
        {
            sMo_sxc_hst.intst += dIntst_hst;
            sMo_sxc_hst.hst_bal -= dTx_amt;
            if(sMo_sxc_hst.hst_bal == 0.00)  /**明细余额为0，状态改为结清**/
            {
                    sMo_sxc_hst.sts[0] = '*';
            }
            iRet = sql_execute("update mo_sxc_hst set intst=%lf, hst_bal=%lf, sts='%s' where ac_id=%ld and ac_seqn=%d and rate_date=%ld and sts='1'", \
                                       sMo_sxc_hst.intst, sMo_sxc_hst.hst_bal, sMo_sxc_hst.sts, ac_id, ac_seqn, sMo_sxc_hst.rate_date);
            if(iRet)
            {
                strcpy(g_pub_tx.reply, "D102");
                sprintf( acErrMsg, "Mo_sxc_hst_Upd_Upd出错!!!" );
                WRITEMSG
                Mo_sxc_hst_Clo_Sel();
                goto ErrExit;
            }
            /*更新随心存余额,这样已结利息会在日终转入随心存，产生复利 add by wp 20151126*/
            iRet = sql_execute("update mo_sxc set bal=bal-%lf where ac_id=%ld and ac_seqn=%d and  sts='1'", \
                                       dTx_amt, ac_id, ac_seqn);
            if(iRet)
            {
                strcpy(g_pub_tx.reply, "D102");
                sprintf( acErrMsg, "更新Mo_sxc.bal出错!!!" );
                WRITEMSG
                goto ErrExit;
            }
        }
        /**----------累加释放总利息------------**/
        dIntst += dIntst_hst; 
        
        /**-----------最后检查是否可以退出-----------------**/
        dSy_tx_amt -= dTx_amt;
        if(dSy_tx_amt == 0.00)
        {
            break;
        }
    }
    Mo_sxc_hst_Clo_Sel();
    
    (*intst) = dIntst;
    vtcp_log("总利息为[%lf]", dIntst);
    
OkExit:
    sprintf(acErrMsg, "释放计息处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
    
    sprintf(acErrMsg, "释放计息处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}

/******************************************************************
 ****    计算遂心存每一条明细的利息  add by zqbo 20141121      ****
 ****    参数说明：                                            ****
 ****    tx_amt 释放金额                                       ****
 ****    flag   标志：                                         ****
 ****               '0'-试算利息、不需要登记明细               ****
 ****               '2'-释放算息、需要登记明细                 ****
 ****               '3'-结息、需要登记明细                     ****
 ****               '4'-解约结息、需要登记明细                 ****
 ******************************************************************/

int iSxc_hst_intst(struct mo_sxc_hst_c mo_sxc_hst, double tx_amt, double *intst_hst, char flag)
{
    int iRet = 0;
    int iDays = 0;
    int iIntst_days = 0;
    long   lRate_date = 0;
    double dRate_val = 0.00;
    double dIntst_hst = 0.00;
    
    char cRate_code[4];
    struct mo_sxc_hst_intst_c sMo_sxc_hst_intst;
    
    memset(cRate_code, 0x00, sizeof(cRate_code));
    memset(&sMo_sxc_hst_intst, 0x00, sizeof(sMo_sxc_hst_intst));
    
    /***----计算取利率的天数----***/
    vtcp_log("aaaaaaaaaaaaaaaaaa系统日期==[%ld]", g_pub_tx.tx_date);
    iDays = pub_base_CalTrueDays(mo_sxc_hst.rate_date, g_pub_tx.tx_date);
    
    lRate_date = mo_sxc_hst.ic_date; /**定期为起息日的利率**/
    
    /***----取靠档利率编号----***/
    if(iDays > 0 && iDays < 7)
    {
        strcpy(cRate_code, HQLLBH);
        
        lRate_date = g_pub_tx.tx_date;  /**活期为支取日的利率**/
    }
    else if(iDays >= 7 && iDays < 90)
    {
        strcpy(cRate_code, QTTZCK);
        
        lRate_date = g_pub_tx.tx_date; /**通知存款为支取日的利率**/
    }
    else if(iDays >= 90 && iDays < 180)
    {
         strcpy(cRate_code, ZCZQ03);
         
    }
    else if(iDays >= 180 && iDays < 360)
    {
        strcpy(cRate_code, ZCZQ06);
        
    }
    else if(iDays >=360 && iDays < 720)
    {
        strcpy(cRate_code, ZCZQ12);
        
    }
    else if(iDays >=720 && iDays < 1080)
    {
        strcpy(cRate_code, ZCZQ24);
        
    }
    else if(iDays >=1080 && iDays < 1800)
    {
        strcpy(cRate_code, ZCZQ36);
        
    }
    else if(iDays >= 1800)
    {
        strcpy(cRate_code, "205");
        
    }
    else if(iDays <= 0)
    {
        iDays = 0;
        strcpy(cRate_code, HQLLBH);
    }
    else
    {
        sprintf(acErrMsg, "计算天数出错![%d]", iDays);
        WRITEMSG
        goto ErrExit;
    }
    
    iRet = pub_base_getllz(cRate_code, "01", lRate_date, &dRate_val);
    if (iRet != 0)
    {
        sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply, "W110");
    }
    /**----计算计息天数, 目前是按每天30天算-----**/
    iIntst_days = pub_base_CalTrueDays(mo_sxc_hst.ic_date, g_pub_tx.tx_date);
    /** -----计算利息---------**/
    dIntst_hst = pub_base_PubDround_1(tx_amt * iIntst_days * dRate_val / L360 / L100);
    vtcp_log("iIntst_days[%ld], dRate_val[%lf], tx_amt[%lf] dIntst_hst[%lf]", iIntst_days, dRate_val, tx_amt, dIntst_hst);
    
    /***-------登记释放结息明细---------***/
    if(flag != '0')
    {
        sMo_sxc_hst_intst.ac_id     = mo_sxc_hst.ac_id;
        sMo_sxc_hst_intst.ac_seqn   = mo_sxc_hst.ac_seqn;
        sMo_sxc_hst_intst.rate_date = mo_sxc_hst.rate_date;
        sMo_sxc_hst_intst.ic_date   = mo_sxc_hst.ic_date;
        if(flag != '3')
        {
            sMo_sxc_hst_intst.hst_bal = mo_sxc_hst.hst_bal - tx_amt;
        }
        else
        {
            sMo_sxc_hst_intst.hst_bal = mo_sxc_hst.hst_bal;	
        }
        strcpy(sMo_sxc_hst_intst.opn_br_no, mo_sxc_hst.opn_br_no);
        strcpy(sMo_sxc_hst_intst.tx_br_no,  g_pub_tx.tx_br_no);
        sMo_sxc_hst_intst.tx_amt = tx_amt;
        sMo_sxc_hst_intst.intst = dIntst_hst;
        sMo_sxc_hst_intst.rate = dRate_val;
        sMo_sxc_hst_intst.days = iIntst_days;
        strcpy(sMo_sxc_hst_intst.rate_code, cRate_code);
        strcpy(sMo_sxc_hst_intst.tx_tel,    g_pub_tx.tel);
        sMo_sxc_hst_intst.tx_date = g_pub_tx.tx_date;
        sMo_sxc_hst_intst.acct_date = g_pub_tx.tx_date;
        sMo_sxc_hst_intst.trace_no = g_pub_tx.trace_no;
        strcpy(sMo_sxc_hst_intst.tx_br_no,  g_pub_tx.tx_br_no);
        strcpy(sMo_sxc_hst_intst.tx_tel,    g_pub_tx.tel);
        sMo_sxc_hst_intst.type[0] = flag;   /**类型为 2-释放 3-结息 4-解约结息**/
        sMo_sxc_hst_intst.sts[0] = '*';     /**状态都为*-结清**/
		
	iRet = Mo_sxc_hst_intst_Ins(sMo_sxc_hst_intst, g_pub_tx.reply);
	if(iRet)
	{
	    strcpy(g_pub_tx.reply, "D102");
	    sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!!" );
	    WRITEMSG
	    goto ErrExit;
	}
    }
    
    (*intst_hst) = dIntst_hst;
    vtcp_log("明细利息为[%lf]", dIntst_hst);
	    
OkExit:
    sprintf(acErrMsg, "结息明细登记成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
        
    sprintf(acErrMsg, "结息明细登记失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
	    
}
