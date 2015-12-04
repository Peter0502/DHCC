#include "find_debug.h"
/*********************************************************************
* 文 件 名:     pubf_base_intst.c       (计算利息)
* 功能描述:     各种活期产品和定期产品的利息计算
* 调用函数说明: pubf_base_CalIntstTd()  定期利息计算总控函数
*               pubf_base_CalIntst()    利息计算总控函数
*
* 作    者:     rob
* 完成日期:     2003年12月28日
*
* 修改记录:
* 1. 日    期:
*    修 改 人:
*    修改内容:
*
*********************************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

/*********************************************************************
* 函 数 名:     pub_base_CalIntst()
* 函数功能:     利息计算总控函数
* 作者/时间:    2003年12月28日
*
* 参    数:
*     输入:
*     输出:
* 返 回 值:     0 成功
*
* 修改历史:
*
*********************************************************************/
int pub_base_CalIntst(struct S_pub_intst *s_pub_intst)
{
    int mode;
    int year, month, day;
    double dealval = 0.00;
    double factval = 0.00;
    double val     = 0.00;
    double keepval = 0.00;
    char tmp_ind[2];

    memset(tmp_ind, 0x00, sizeof(tmp_ind));
    strcpy(tmp_ind, g_pub_intst.edu_ind);
    MEMSET_DEBUG(s_pub_intst, 0x00, sizeof(struct S_pub_intst));
    strcpy(g_pub_intst.edu_ind, tmp_ind);
    year  = g_pub_tx.tx_date / 10000;
    month = g_pub_tx.tx_date / 100 % 100;
    day   = g_pub_tx.tx_date % 100;

    mode = 1; /* 以前分销户还是结息日计息 现在不用 */
    sprintf(acErrMsg, " mode = [%d]", mode);
    WRITEMSG

    switch (g_pub_tx.ac_id_type[0])
    {
        case '1':
            /* 活期存款 */
        TRACE
            if (pub_base_CalIntstHQ(mode, &dealval, &factval, &val,
                                    &keepval, g_pub_tx.reply))
            {
                strcpy(acErrMsg, "计算活期利息出错!!!");
                WRITEMSG
                return 1;
            }
            break;

        case '2':
            /* 定期 */
        TRACE
            if (pub_base_CalIntstTd(&dealval, &factval, &val,
                                    &keepval, g_pub_tx.reply))
            {
                strcpy(acErrMsg, "计算定期利息出错!!!");
                WRITEMSG
                return 1;
            }
            break;

        case '3':
            /* 贷款 */
            break;

        case '4':
            /* 透支 */
            break;

        case '9':
            /* 内部 */
            if (pub_base_CalIntstIn(&dealval, &factval, &val,
                                    &keepval, g_pub_tx.reply))
            {
                sprintf(acErrMsg, "计算内部帐户利息错误!!");
                WRITEMSG
                return 1;
            }
            break;
        default:
                TRACE
            break;
    }
    sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        dealval, factval, val, keepval);
    WRITEMSG

    /* 给计息结构赋值 */
    s_pub_intst->dealval = dealval;
    s_pub_intst->factval = factval;
    s_pub_intst->val     = val;
    s_pub_intst->keepval = keepval;
    return 0;
}

/*********************************************************************
* 函 数 名:     pub_base_CalIntstTd()
* 函数功能:     定期产品利息计算总控函数
* 作者/时间:    2003年12月28日
*
* 参数:
*     输入:
*     输出:
*               响应码      char(4)
* 返 回 值:     0 成功
*
* 修改历史:
*
*********************************************************************/

int pub_base_CalIntstTd(double * dealval, double * factval, double * val,
                        double * keepval, char * reply)
{
    char    td_type;
    long    tmp_tx_date  = 0;
    long    tmp_ic_date  = 0;
    long    tmp_opn_date = 0;
    long    tmp_mtr_date = 0;
    double  tmp_dealval  = 0.00;
    double  tmp_factval  = 0.00;
    double  tmp_val      = 0.00;
    double  tmp_keepval  = 0.00;
    double  tmp_amt      = 0.00;
    int     l_term       = 0;

    td_type = g_td_parm.td_type[0];
    sprintf(acErrMsg, " td_type = [%c]", td_type);
    WRITEMSG
    sprintf(acErrMsg, "CGHCGH*****交易日期:[%ld]", g_pub_tx.tx_date);
    WRITEMSG
    switch(td_type)
    {
        case '0' :
            /* 整存整取计息函数 */
            strcpy(acErrMsg, "计算整存整取利息!");
            WRITEMSG

            /**------- hao -------- 增加定期自动转存计息 ---------**/
            tmp_tx_date  = g_pub_tx.tx_date;
            tmp_ic_date  = g_td_mst.ic_date;
            tmp_opn_date = g_td_mst.opn_date;
            tmp_mtr_date = g_td_mst.mtr_date;
            tmp_amt      = g_pub_tx.tx_amt1;
            do
            {
                if (g_td_mst.tfr_ind[0] == 'Y' && tmp_tx_date > g_td_mst.mtr_date)
                    g_pub_tx.tx_date = g_td_mst.mtr_date;
                else
                    g_pub_tx.tx_date = tmp_tx_date;
                tmp_dealval = 0.00;
                tmp_factval = 0.00;
                tmp_val     = 0.00;
                tmp_keepval = 0.00;
                if (pub_base_CalIntstTdZCZQ(&tmp_dealval, &tmp_factval,
                                           &tmp_val, &tmp_keepval, reply))
                {
                    strcpy(acErrMsg, "计算整存整取利息出错!!");
                    WRITEMSG
                    return 1;
                }
                vtcp_log("AFTER [%d--%d]", g_td_mst.ic_date, g_td_mst.mtr_date);
                vtcp_log("deal[%.2lf] factval[%.2lf] val[%.2lf][%.2lf]",
                         tmp_dealval, tmp_factval, tmp_val, tmp_keepval);
                *dealval += tmp_dealval;
                *factval += tmp_factval;
                *val     += tmp_val;
                *keepval += tmp_keepval;
                /******XXXXXXXXX
                g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 + (*factval) + (*keepval);
                ****************/
                g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 + (tmp_factval) + (tmp_keepval);

                vtcp_log("after [%d--%d],tx_date[%ld]", g_td_mst.ic_date, g_td_mst.mtr_date, g_pub_tx.tx_date);
                vtcp_log("HAO --- LOOK [%lf][%lf][%lf][%lf][%lf]", *dealval, *factval, *keepval, g_pub_tx.tx_amt1, *val);
                g_td_mst.ic_date  = g_td_mst.mtr_date;
                g_td_mst.opn_date = g_td_mst.mtr_date;
                if (g_td_parm.term_type[0] == 'D')
                {
                    l_term = g_td_parm.term;
                    pub_base_deadlineD(g_td_mst.opn_date, l_term, &g_td_mst.mtr_date);
                }
                else if (g_td_parm.term_type[0] == 'Y')
                {
                    l_term = g_td_parm.term * 12;
                    pub_base_deadlineM(g_td_mst.opn_date, l_term, &g_td_mst.mtr_date);
                }
                else if (g_td_parm.term_type[0] == 'M')
                {
                    l_term = g_td_parm.term;
                    pub_base_deadlineM(g_td_mst.opn_date, l_term, &g_td_mst.mtr_date);
                }
            } while (tmp_tx_date > g_pub_tx.tx_date);
            g_pub_tx.tx_date  = tmp_tx_date;
            g_td_mst.ic_date  = tmp_ic_date;
            g_td_mst.opn_date = tmp_opn_date;
            g_td_mst.mtr_date = tmp_mtr_date;
            g_pub_tx.tx_amt1  = tmp_amt;
            /**------------------------------------------------------**/
            break;

        case '1' :
            /* 零存整取计息函数 */

            strcpy(acErrMsg,"计算零存整取利息!");
            WRITEMSG
            if (pub_base_CalIntstLCZQ(dealval,factval,val,keepval,reply))
            {
                strcpy(acErrMsg,"计算零存整取利息出错!!");
                WRITEMSG
                return 1;
            }


            break;

        case '2' :
            /* 整存零取计息函数*/

            strcpy(acErrMsg,"计算整存零取利息!!");
            WRITEMSG
            if (pub_base_CalIntstZCLQ(dealval,factval,val,keepval,reply))
            {
                strcpy(acErrMsg,"计算整存零取利息出错!!");
                WRITEMSG
                return 1;
            }

            break;

        case '3' :
            /* 存本取息计息函数*/

            strcpy(acErrMsg,"存本取息计息函数!!");
            WRITEMSG
            if (pub_base_CalIntstCBQX(dealval,factval,val,keepval,reply))
            {
                strcpy(acErrMsg,"计算存本取息利息出错!!");
                WRITEMSG
                return 1;
            }
            break;

        case '4' :
            /* 通知存款计息函数*/

            strcpy(acErrMsg,"通知存款计息!!");
            WRITEMSG
            if (pub_base_CalIntstTZCK(dealval,factval,val,keepval,reply))
            {
                strcpy(acErrMsg,"计算通知存款利息出错!!");
                WRITEMSG
                return 1;
            }
            break;

        case '5' :
            /* 协议存款计息函数*/
            sprintf(acErrMsg,"计算协议存款计息利息!!");
            WRITEMSG
            break;

        case '6' :
            /* 定活两便 */

            strcpy(acErrMsg,"计算定活两便利息!!");
            WRITEMSG
            if (pub_base_CalIntstDHLB(dealval,factval,val,keepval,reply))
            {
                sprintf(acErrMsg,"计算定活两便利息出错!!");
                WRITEMSG
                return 1;
            }
            break;

        case '7' :
            /* 不定额续存 教育储蓄 */
            strcpy(acErrMsg,"计算不定额续存利息!!");
            WRITEMSG
            sprintf(acErrMsg, "CGH*** 教育证明:[%s]", g_pub_intst.edu_ind);
            WRITEMSG
            if (pub_base_CalIntstJYCX(dealval, factval, val, keepval, \
                g_pub_intst.edu_ind, reply))
            {
                sprintf(acErrMsg, "计算不定额续存利息错误--教育储蓄");
                WRITEMSG
                return 1;
            }
            break;
/* begin add by LiuHuafeng 2009-2-6 18:21:32 for 理财型通知存款 */
    case '8' :

        strcpy(acErrMsg,"通知存款利息!");
        WRITEMSG

        /**------- hao -------- 增加定期自动转存计息 ---------**/
        tmp_tx_date=g_pub_tx.tx_date;
        tmp_ic_date=g_td_mst.ic_date;
        vtcp_log("[%s][%d]test[%ld]",__FILE__,__LINE__,g_td_mst.ic_date);
        tmp_opn_date=g_td_mst.opn_date;
        tmp_mtr_date=g_td_mst.mtr_date;
        tmp_amt=g_pub_tx.tx_amt1;
        do
        {
            if (g_td_mst.tfr_ind[0]=='Y' && tmp_tx_date>g_td_mst.mtr_date)
                g_pub_tx.tx_date=g_td_mst.mtr_date;
            else
                g_pub_tx.tx_date=tmp_tx_date;
            tmp_dealval = 0.00;
            tmp_factval = 0.00;
            tmp_val = 0.00;
            tmp_keepval = 0.00;
            if (pub_base_CalIntstTdZCZQ_TZ(tmp_tx_date,&tmp_dealval,&tmp_factval,
                &tmp_val,&tmp_keepval,reply))
            {
                strcpy(acErrMsg,"计算整存整取利息出错!!");
                WRITEMSG
                    return 1;
            }
            vtcp_log("AFTER [%d--%d]",g_td_mst.ic_date,g_td_mst.mtr_date);
            vtcp_log("deal[%.2lf] factval[%.2lf] val[%.2lf][%.2lf]",
                tmp_dealval,tmp_factval,tmp_val,tmp_keepval);
            *dealval+=tmp_dealval;
            *factval+=tmp_factval;
            *val+=tmp_val;
            *keepval+=tmp_keepval;
            /******XXXXXXXXX
            g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+(*factval)+(*keepval);
            ****************/
            g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+(tmp_factval)+(tmp_keepval);

            vtcp_log("after [%d--%d]",g_td_mst.ic_date,g_td_mst.mtr_date);
            vtcp_log("HAO --- LOOK [%lf][%lf][%lf][%lf][%lf]",*dealval,*factval,*keepval,g_pub_tx.tx_amt1,*val);
            g_td_mst.ic_date=g_td_mst.mtr_date;
            g_td_mst.opn_date=g_td_mst.mtr_date;
            if (g_td_parm.term_type[0]=='D')
            {
                l_term=g_td_parm.term;
                pub_base_deadlineD(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
            }
            else if (g_td_parm.term_type[0]=='Y')
            {
                l_term=g_td_parm.term*12;
                pub_base_deadlineM(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
            }
            else if (g_td_parm.term_type[0]=='M')
            {
                l_term=g_td_parm.term;
                pub_base_deadlineM(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
            }
        }while(tmp_tx_date>g_pub_tx.tx_date);
        g_pub_tx.tx_date=tmp_tx_date;
        g_td_mst.ic_date=tmp_ic_date;
        g_td_mst.opn_date=tmp_opn_date;
        g_td_mst.mtr_date=tmp_mtr_date;
        g_pub_tx.tx_amt1=tmp_amt;
        /**------------------------------------------------------**/
        break;
    /* end add by LiuHuafeng 2009-2-6 18:21:32 for 理财型通知存款 */
        default :
            /* 错误返回 */
            sprintf(acErrMsg, "定期存款类型错误: [%c]", td_type);
            WRITEMSG
            return 1;
    }

    return 0;
}

/**********************************************************************
* 函数名:     pub_base_CalIntstTdZCZQ()
* 函数功能:   计算整存整取利息
* 作者/时间:  2003年12月28日
*
* 参数:
*     输入:
*     输出: 应付利息          double
*           实付利息          double
*           代扣利息税        double
*           保值利息          double
*           响应码            char(4)
* 返 回 值: 0 成功
*
* 修改历史:
*
********************************************************************/
int pub_base_CalIntstTdZCZQ(double *dealval, double *factval,
       double *val, double * keepval, char *reply)
{
    int draw_flag; /* 到期标志  1-提前支取,2-到期支取,3-逾期支取 */
    int monterm,monterm1;
    long dayterm; /* 月 日存期 */
    long daytermxd;/**单位定期协定存款计算存期天数**/
    long daytermval;      /* 代扣个人所得利息天数 */
    double  amtval;      /* 计算所得税金额 */
    double  sdsl;        /* 个人所得税率 */
    double rate_val; /* 利率 */
    double lx;      /* 利息 */
    long date88,date89,date93,date9307,date99;
    long date91,date97,date98,date00;
    char   llbh[3]; /* 利率编号 */
    int ret;
    double a1,a2,a3,s1=0,s1_tmp=0,s2=0; /* 存期*利率 */
    char   intst_term_type[2] ; /* 计算天数类型 */
    char  Type[2];

    sprintf(acErrMsg,"g_td_parm.term_type=[%s],term=[%d],prdt_no=[%s], \
            tx_amt=[%lf]",g_td_parm.term_type,g_td_parm.term,
            g_td_parm.prdt_no, g_pub_tx.tx_amt1);
    WRITEMSG
    

    date88 = 19880910;
    date89 = 19890201;
    date91 = 19911201;
    date93 = 19930301;
    date9307 = 19930711;
    date97 = 19971023;
    date98 = 19981206;
    date99 = 19991101;
    date00 = 20000713;
    (*keepval) = 0.00; /* 保值利息 */
    (*dealval) = 0.00; /* 应付利息 */
    (*factval) = 0.00; /* 实付利息 */
    (*val) = 0.00;     /* 代扣利息税 */
    strcpy(Type,"0");/*实际天数 */

    /* 判断交易日期是否到期 */
    if (g_pub_tx.tx_date <  g_td_mst.mtr_date)
    {
        draw_flag = 1; /* 提前 */
    }
    else if (g_pub_tx.tx_date == g_td_mst.mtr_date)
    {
        draw_flag = 2; /* 到期 */
    }
    else
    {
        draw_flag = 3; /* 逾期 */
    }

    /* 计算存期 */
    switch (g_td_parm.term_type[0])
    {
        case 'Y' :
            monterm = g_td_parm.term * 12;
            break;
        case 'M' :
            monterm = g_td_parm.term;
            break;
        case 'D' :
            monterm = g_td_parm.term / 30;
            break;
        /**单位定期协定存款计算存期（按照启息日期-到期日期计算天数 除以30为得到的实际存期）add by zhangguofeng strart 2014/4/11***/
        case 'H' : 
        		ret=pub_base_CalIntstDays(g_td_mst.ic_date,g_td_mst.mtr_date,Type,&daytermxd);
            if (ret != 0)
            {
                sprintf(acErrMsg,"计算天数错误!!");
                WRITEMSG
                return 1;
            }
            monterm = daytermxd / 30;
            break;
         /**zhangguofeng end 2014/4/11***/
        default:
            /* 错误返回 */
            sprintf(acErrMsg,"定期产品表中存期类型错误 %c",g_td_parm.term_type);
            WRITEMSG
            strcpy(reply,"W108");
            return 1;
    }
vtcp_log("[%s][%d]存款天数[%d]得出存期[%ld]\n",__FILE__,__LINE__,daytermxd,monterm);
    /**根据长治要求,在20061104之前的按年利率计算利息,在这之后的改为按实际天数计算利息. modify by wanglei 20061118**  晋中不需要了
    if (g_td_mst.opn_date < 20061104)
        strcpy(intst_term_type,"1");***按每月30天计息***
    else
    */
        strcpy(intst_term_type,g_td_parm.intst_term_type);/***按实际天数计息***/

    if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
         sprintf(acErrMsg,"产品表中的计息天数类型错误 type=[%s]",
                        g_td_parm.intst_term_type);
         WRITEMSG
         MEMSET_DEBUG(reply, 0x0, sizeof(reply));
         strcpy(reply, "W107");
         return 1;
    }


sprintf(acErrMsg,"开始计算利息monterm=[%d],draw_flag=[%d]",monterm,draw_flag);
WRITEMSG

    if (monterm <= 24)
    {

sprintf(acErrMsg,"普通利息计算");
WRITEMSG

        /* 普通利息计算 */
        if (g_td_mst.ic_date <= date93)
        {
            /* 93.03.01之前的整存整取 */

sprintf(acErrMsg,"普通利息计算--930301之前的整存整取");
WRITEMSG

            switch (draw_flag)
            {
                case 1:

                    /* 提前支取实行同档次定期利率
                     1.根据开户日期 交易日期 和产品 界定期限码
                     2.根据期限码赋相应的利率编号,并查询出利率
                     3.计算利息
                     4.计算所得税
                     */

sprintf(acErrMsg,"普通利息计算--提前支取");
WRITEMSG

                    /* 按实存时间同档次定期利率计息 界定期限 */
                    MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

                    ret = pub_base_intstJDQX(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,g_pub_tx.tx_date,intst_term_type);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"界定期限错误");
                        WRITEMSG
                        return 1;
                    }

sprintf(acErrMsg,"界定利率:llbh=[%s]",llbh);
WRITEMSG
                    ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                         g_pub_tx.tx_date, intst_term_type,&dayterm);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"计算天数错误!!");
                        WRITEMSG
                        strcpy(reply,"W102");
                        return 1;
                    }

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                    /* llbh 根据利率编号和交易日期查询利率 */
                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = dayterm * rate_val * g_pub_tx.tx_amt1
                           * (1 + g_td_mst.flt_ratio) / L360 / L100;

                    (*dealval) = pub_base_PubDround_1(lx);

                    /* 计算所得税 */
                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        ret=pub_base_CalIntstDays(PubMax(date99,
               g_td_mst.ic_date),g_pub_tx.tx_date, intst_term_type,&daytermval);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }
                        /* 需扣利息税的利息 */
                        amtval = lx * daytermval / dayterm;

sprintf(acErrMsg,"dayterm=[%ld],daytermval=[%ld],lx=[%lf],amtval=[%lf],flt_ratio=[%lf]", dayterm,daytermval,lx,amtval,g_td_mst.flt_ratio);
WRITEMSG

                        /* 取利息税税率 */
                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &sdsl);
                        if ((ret != 0) && (ret == 1))
                        {
                            sprintf(acErrMsg,"定期产品利息税税率不存在 %d",ret);
                            WRITEMSG
                            strcpy(reply,"W100");
                            return 1;
                        }
                        else if (ret < 0)
                        {
                            sprintf(acErrMsg,"取利息税税率错误 %d ", ret);
                            WRITEMSG
                            strcpy(reply,"W101");
                            return 1;
                        }

sprintf(acErrMsg,"利息税税率sdsl=[%lf] ", sdsl);
WRITEMSG

                        g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
                        (*val) = amtval * sdsl;
                        (*val) = pub_base_PubDround_1(*val) ;
                    }

                    (*factval) = (*dealval) -(*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    break;
                case 2 :
                    /* 到期支取利息处理 */
                case 3 :
                    /* 逾期支取利息处理
                     1.根据利率编号和开户日期 起息日期 交易日期查询出分段利率表
                       并计算出利率和存期之积
                     2.计算到期利息
                     3.如果逾期,计算逾期利息
                     4.计算个人所得税
                     */

sprintf(acErrMsg,"普通利息计算--到期或逾期支取");
WRITEMSG

                    /* 计算到期利率*到期时间 分段计算 */
                    MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                    strcpy(llbh, g_td_parm.rate_no);

sprintf(acErrMsg," llbh=[%s],cur_no=[%s],ic_date=[%ld],mtr_date=[%ld]",
        llbh,g_td_parm.cur_no,g_td_mst.ic_date,g_td_mst.mtr_date);
WRITEMSG

                    ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,g_td_mst.mtr_date,
                          intst_term_type, Lone, &a1);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"计算分段利率*天数错误 [%d] ",ret);
                        WRITEMSG
                        strcpy(reply,"W111");
                        return ret;
                    }

sprintf(acErrMsg," 到期 : 计算分段利率*天数 = [%lf]",a1);
WRITEMSG

                    /* 计算逾期利率*逾期时间 */

                    /* date93 之前按原订存期利率计算
                       date93 之后按支取日挂牌公告的活期储蓄利率计算 */

                    if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                    {
                        ret=pub_base_CalIntstDays(g_td_mst.mtr_date,date93,
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
                        dayterm = 0; /* 不计息 */
                    }

                    /* 取相应存期的利率 */
                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

                    a2 = dayterm * rate_val; /* date93前 */

sprintf(acErrMsg," 逾期 (date93前) : 计算分段利率*天数 = [%lf]",a2);
WRITEMSG


                    if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                    {
                        ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,
                            date93),g_pub_tx.tx_date,intst_term_type,&dayterm);
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

                    /* 根据利率编号 利率类型取出当时挂牌活期利息 */
                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,HQLLBH);
                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

                    a3 = dayterm * rate_val; /* date93后 */

sprintf(acErrMsg," 逾期 (date93后) : 计算分段利率*天数 = [%lf]",a3);
WRITEMSG

                    lx = (a1+a2+a3) * floor(g_pub_tx.tx_amt1*L100) / L100
                        * (1+g_td_mst.flt_ratio) / L360 / L100;
                    (*dealval) = pub_base_PubDround_1(lx) ;

                    /* 计算利息税 */
                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        ret=pub_base_CalIntstDays(PubMax(date99,
                            g_td_mst.mtr_date),g_pub_tx.tx_date,
                            intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                        /* 应缴税利息 */
                 (*val)= rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100
                                * (1+g_td_mst.flt_ratio) / L360 / L100;

                        /* 取利息税编号并取利率 */
                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &sdsl);
                        if (ret != 0 && ret == 1)
                        {
                            sprintf(acErrMsg,"定期产品利息税税率不存在 %d",ret);
                            WRITEMSG
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

                        g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
                        (*val) = pub_base_PubDround_1((*val) * sdsl);
                    }
                    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    break;
                default:
                    break;
            }
        }
        else      /* 93.03.01 后存入的整存整取 */
        {

sprintf(acErrMsg,"普通利息计算--930301之后存入的整存整取");
WRITEMSG

            switch (draw_flag)
            {
                case 1:
                    /*
                    提前支取 按支取日挂牌公告的活期储蓄存款利率计付利息
                    1.计算计息日期到交易日期的总天数
                    2.取当前挂牌公告的活期储蓄存款利率
                    3.计算利率
                    4.99年后个人需要交个人所得税
                    */

sprintf(acErrMsg,"普通利息计算--提前支取");
WRITEMSG

sprintf(acErrMsg,"ic_date=[%ld],tx_date=[%ld]",g_td_mst.ic_date,g_pub_tx.tx_date);
WRITEMSG
                    /**ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                           g_pub_tx.tx_date, intst_term_type,&dayterm); 算实际天数算 20130608 wjl**/
                  ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                           g_pub_tx.tx_date,Type,&dayterm);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"计算天数错误!!");
                        WRITEMSG
                        return 1;
                    }


                    /* 取活期利率 */
                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"活期 : llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

sprintf(acErrMsg,"利息 : dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                         * (1 + g_td_mst.flt_ratio) / L360 / L100;
                    lx = pub_base_PubDround_1(lx) ;
                    (*dealval) = lx;

                    if (floor(lx * 100) / 100 < 0.005)
                    {
                        lx = 0.00;
                        (*dealval) = 0.00;
                        (*factval) = 0.00;
                        (*val) = 0.00;
                    }

                    /* 计算个人所得税 */
                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        ret=pub_base_CalIntstDays(PubMax(date99,
                               g_td_mst.ic_date),g_pub_tx.tx_date,
                               intst_term_type,&daytermval);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                        amtval = lx * daytermval / dayterm;

                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &sdsl);
                        if ((ret != 0) && (ret == 1))
                        {
                           sprintf(acErrMsg,"定期产品利息税税率不存在 %d",ret);
                           WRITEMSG
                           strcpy(reply,"W100");
                           return 1;
                        }
                        else if (ret < 0)
                        {
                            sprintf(acErrMsg,"取利息税税率错误 %d ", ret);
                            WRITEMSG
                            strcpy(reply,"W101");
                            return 1;
                        }
                        g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
                        (*val) = amtval * sdsl;
                        (*val) = pub_base_PubDround_1(*val);
                    }

                    (*factval) = (*dealval) -(*val);

sprintf(acErrMsg,"利息 : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    break;
                case 2:

                    /* 到期支取
                     1. 计算到期天数
                     2. 取对应产品的日利率
                     3. 计算利息
                     4. 99后计算个人所得税
                     */
                case 3:
                    /* 逾期支取
                     1.930301--930711 按活期利息计算
                                         2.930711--到期日 按930711同档次利率计算
                                         3.到期日--交易日 按交易日活期利率计算
                                         4.计算99后的个人所得税

                     */

sprintf(acErrMsg,"普通利息计算-- 到期或逾期支取");
WRITEMSG

sprintf(acErrMsg,"ic_date=[%ld],tx_date=[%ld]",g_td_mst.ic_date,g_pub_tx.tx_date);
WRITEMSG

                    if (monterm <= 6)
                    {
                        ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                               g_td_mst.mtr_date, intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                        /* 取该产品的对应日利率 */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[%ld],定期天数[%d]",
        llbh,g_td_parm.cur_no,g_td_mst.ic_date,dayterm);
WRITEMSG

					/**晋中单位定期协定存款在计息时，使用分户利率 add by zhangguofeng strart 2014/4/11**/
					if (memcmp(g_td_parm.prdt_no,"259",3)==0)
					{
              rate_val=g_td_mst.rate;
              vtcp_log("[%s] [%d] martin look----g_td_mst.rate=rate_val=[%lf]\n",__FILE__,__LINE__,rate_val);
          }else
          {	

              ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                    g_td_mst.ic_date,&rate_val);
              if (ret != 0)
              {
                  sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                  WRITEMSG
                  strcpy(reply,"W110");
                  return ret;
              }
          }
					/**晋中单位定期协定存款在计息时，使用分户利率 add by zhangguofeng end 2014/4/11**/
sprintf(acErrMsg,"到期:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                     lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        (*dealval) = lx;
sprintf(acErrMsg,"注意到期利息:[%lf]",lx);
WRITEMSG

                        /* 逾期利息 */
                        if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                        {
                            /*
                            ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,
                                 date99), g_pub_tx.tx_date, intst_term_type,
                                 &dayterm);
                            */
                            ret=pub_base_CalIntstDays(g_td_mst.mtr_date,
                                 g_pub_tx.tx_date, intst_term_type,
                                 &dayterm);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算天数错误!!");
                                WRITEMSG
                                return 1;
                            }
                            vtcp_log("[%s][%d]逾期[%ld][%ld]后的天数[%d]\n",__FILE__,__LINE__,g_td_mst.mtr_date,g_pub_tx.tx_date,dayterm);
                        }
                        else
                        {
                            dayterm = 0;

                        }
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"逾期: llbh=[%s],cur_no=[%s],date=[%ld],dayterm[%d]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date,dayterm);
WRITEMSG

                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_pub_tx.tx_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

                     lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"逾期利息: dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],\
        flt_ratio=[%lf] lx[%lf]",dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio,lx);
WRITEMSG
                        (*dealval) = (*dealval) + lx;
sprintf(acErrMsg,"注意总利息: dealval[[%lf]",*dealval);
WRITEMSG
                         /* 计算个人所得税 */

                        if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                        {
                            ret=pub_base_CalIntstDays(PubMax(date99,
                                g_td_mst.ic_date), g_td_mst.mtr_date,
                                intst_term_type,&daytermval);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算天数错误!!");
                                WRITEMSG
                                return 1;
                            }

                            /* 利率编号从产品里取 */
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"利息税:llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                  g_td_mst.ic_date,&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"定期部分应该缴利息税的参数1:dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                            amtval = rate_val * daytermval
                                     * floor(g_pub_tx.tx_amt1*L100)/L100
                                     * (1 + g_td_mst.flt_ratio) / L360 / L100;

                            ret=pub_base_CalIntstDays(PubMax(date99,
                                g_td_mst.mtr_date), g_pub_tx.tx_date,
                                intst_term_type,&daytermval);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算天数错误!!");
                                WRITEMSG
                                return 1;
                            }
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"利息税:llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                  g_pub_tx.tx_date,&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"逾期部分应该缴利息税的参数2:dayterm=[%d],rate_val=[%lf],tx_amt=[%lf], \
   flt_ratio=[%lf]", daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

              lx = daytermval * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                                 * (1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"逾期部分应该缴税的利息是:lx[%lf]",lx);
WRITEMSG
sprintf(acErrMsg,"定期应该应缴税的利息是amtval[%lf]",amtval);
WRITEMSG
                            amtval = amtval + lx;
sprintf(acErrMsg,"合计应该缴税的利息是amtval[%lf]",amtval);
WRITEMSG
                            ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                            if ((ret != 0) && (ret == 1)) /*日期如果小于19991101则利率为0 gujy 20070419*/
                            {
                                sprintf(acErrMsg,"定期产品利息税税率不存在 %d",
                                      ret);
                                WRITEMSG
                                strcpy(reply,"W100");
                                return 1;
                            }
                            else if (ret < 0)
                            {
                                sprintf(acErrMsg,"取利息税税率错误 %d ",
                                     ret);
                                WRITEMSG
                                strcpy(reply,"W101");
                                return 1;
                            }
                            g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
                            (*val) = amtval * sdsl;
                            (*val) = pub_base_PubDround_1(*val);
                        }

                        (*dealval)= pub_base_PubDround_1(*dealval);
                        (*factval) = (*dealval) -(*val);
                        if (floor((*dealval)*100)/100<0.005)
                        {
                            lx = 0.00;
                            (*dealval) = 0.00;
                            (*factval) = 0.00;
                            (*val) = 0.00;
                        }

sprintf(acErrMsg,"利息 : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    }
                    else /* monterm >6 */
                    {

sprintf(acErrMsg,"存期大于六个月");
WRITEMSG
                        ret=pub_base_CalIntstDays(g_td_mst.ic_date,date9307,
                               intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }


                        /* 取930711时活期利率 */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[19930711]",
        llbh,g_td_parm.cur_no);
WRITEMSG

												/**晋中单位定期协定存款在计息时，使用分户利率 add by zhangguofeng strart 2014/4/11**/
												if (memcmp(g_td_parm.prdt_no,"259",3)==0)
												{
							              rate_val=g_td_mst.rate;
							              vtcp_log("[%s] [%d] martin look----g_td_mst.rate=rate_val=[%lf]\n",__FILE__,__LINE__,rate_val);
							          }else
							          {
	                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
	                              date9307,&rate_val);
	                        if (ret != 0)
	                        {
	                            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
	                            WRITEMSG
	                            strcpy(reply,"W110");
	                            return ret;
	                        }
	                      }
												/**晋中单位定期协定存款在计息时，使用分户利率 add by zhangguofeng end 2014/4/11**/
sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                         * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        (*dealval) = lx;

sprintf(acErrMsg,"lx=[%lf]",lx);
WRITEMSG

                        ret=pub_base_CalIntstDays(PubMax(date9307,
                            g_td_mst.ic_date), g_td_mst.mtr_date,
                            intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                        /* 在930711后开户,取该产品的对应日利率
                           否则,取pubmax(930711,ic_date)当天同档次利率 */

                        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                        strcpy(llbh, g_td_parm.rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[19930711]",
        llbh,g_td_parm.cur_no);
WRITEMSG
     /**  根据晋中同业存放定期款项需求 打印CZFY利率部提后维持原利率 add by martin 2009/7/27 10:46:50 **/
          if (memcmp(g_td_parm.prdt_no,"25A",3)==0)
              {
                rate_val=g_td_mst.rate;
                vtcp_log("[%s] [%d] martin look----g_td_mst.rate=rate_val=[%lf]\n",__FILE__,__LINE__,rate_val);
              }else
                   {
                        ret = pub_base_getllz(llbh, g_td_parm.cur_no,
                              PubMax(date9307,g_td_mst.ic_date), &rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }
               }

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                     lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                            * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        (*dealval) = (*dealval) + lx;

sprintf(acErrMsg,"1lx=[%lf]",dayterm * rate_val);
WRITEMSG
sprintf(acErrMsg,"2lx=[%lf]", floor(g_pub_tx.tx_amt1*L100));
WRITEMSG
sprintf(acErrMsg,"3lx=[%lf]",dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100);
WRITEMSG
sprintf(acErrMsg,"4lx=[%lf]",dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100*(1+g_td_mst.flt_ratio));
WRITEMSG
sprintf(acErrMsg,"4lx=[%lf]",dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100*(1+g_td_mst.flt_ratio)/L360);
WRITEMSG
sprintf(acErrMsg,"4lx=[%lf]",dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100*(1+g_td_mst.flt_ratio)/L360/L100);
WRITEMSG
sprintf(acErrMsg,"lx=[%lf]",lx);
WRITEMSG
                        /* 逾期利息计算 */
                        if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                        {
                           /** ret=pub_base_CalIntstDays(g_td_mst.mtr_date,
                                g_pub_tx.tx_date,intst_term_type,&dayterm); 逾期利息按实际天数计算wjl 20130608**/
                               ret=pub_base_CalIntstDays(g_td_mst.mtr_date,
                                g_pub_tx.tx_date,Type,&dayterm);
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

                        /* 取活期利率  */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"逾期: llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_pub_tx.tx_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"从com_rate取利率值错误[%s][%s][%ld][%.2lf]",llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                     lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        (*dealval) = (*dealval) + lx;

sprintf(acErrMsg,"lx=[%lf]",lx);
WRITEMSG

                        /* 计算个人所得税 */
                        if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                        {
                            ret=pub_base_CalIntstDays(PubMax(date99,
                                g_td_mst.ic_date), g_td_mst.mtr_date,
                                intst_term_type,&daytermval);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算天数错误!!");
                                WRITEMSG
                                return 1;
                            }

                             /* 取产品表的利率编号 */
                             MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                             strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"利息税: llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_td_mst.ic_date);
WRITEMSG

                             ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                   g_td_mst.ic_date,&rate_val);
                             if (ret != 0)
                             {
                                 sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                                 WRITEMSG
                                 strcpy(reply,"W110");
                                 return ret;
                             }

sprintf(acErrMsg,"利息dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG
                             amtval = rate_val * daytermval
                                      * floor(g_pub_tx.tx_amt1*L100)/L100
                                      * (1 + g_td_mst.flt_ratio) / L360 /L100;


                            if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                            {
                                ret=pub_base_CalIntstDays(PubMax(date99,
                                    g_td_mst.mtr_date), g_pub_tx.tx_date,
                                    intst_term_type,&daytermval);
                                if (ret != 0)
                                {
                                    sprintf(acErrMsg,"计算天数错误!!");
                                    WRITEMSG
                                    return 1;
                                }
                            }
                            else
                            {
                                daytermval = 0;

                            }

                            /* 取活期利率 */
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"利息: llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_td_mst.ic_date);
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                  g_pub_tx.tx_date,&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"利息dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                            amtval += rate_val * daytermval
                                     * floor(g_pub_tx.tx_amt1*L100)/L100
                                     * (1 + g_td_mst.flt_ratio) / L360 /L100;

                            ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                            if ((ret != 0) && (ret == 1))
                            {
                                sprintf(acErrMsg,"定期产品利息税税率不存在 %d",
                                       ret);
                                WRITEMSG
                                strcpy(reply,"W100");
                                return 1;
                            }
                            else if (ret < 0)
                            {
                                sprintf(acErrMsg,"取利息税税率错误 %d ",
                                     ret);
                                WRITEMSG
                                strcpy(reply,"W101");
                                return 1;
                            }
                            g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
                            (*val) = amtval * sdsl;
                            (*val) = pub_base_PubDround_1(*val);
                        }
                        (*dealval) = pub_base_PubDround_1(*dealval);
                        (*factval) = (*dealval) -(*val);
                        if (floor((*dealval) * 100)/100<0.005)
                        {
                            (*dealval) = 0.00;
                            (*factval) = 0.00;
                            (*val) = 0.00;
                        }

sprintf(acErrMsg,"利息 : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    }
                    break;
                default:
                    /* 错误返回 */
                    return 1;
            }
        }
    }
    else  /* monterm >24 */
    {

sprintf(acErrMsg,"保值利息计算");
WRITEMSG

        /* 保值利息计算 */
        switch (draw_flag)
        {
            case 1 :
                /* 未到期支取 */

sprintf(acErrMsg,"未到期支取 ");
WRITEMSG

                if (g_td_mst.ic_date >= date93) /* date93之后开户的 */
                {

sprintf(acErrMsg,"date93 之后开户的,按活期利率计息 ");
WRITEMSG

                    /* 按到期活期利率计算 */
                    ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                        g_pub_tx.tx_date, intst_term_type,&dayterm);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"计算天数错误!!");
                        WRITEMSG
                        return 1;
                    }

                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

sprintf(acErrMsg,"rate_val=[%lf],dayterm=[%d],tx_amt=[%lf],flt_ratio=[%lf]",
        rate_val,dayterm,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100
                         * (1 + g_td_mst.flt_ratio) / L360 / L100;

                    (*dealval) = pub_base_PubDround_1(lx);

                    /* 计算利息税 */
                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                            date99), g_pub_tx.tx_date,intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                     lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;

                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                        if (ret != 0 && ret == 1)
                        {
                            sprintf(acErrMsg,"定期产品利息税税率不存在 %d",
                                     ret);
                            WRITEMSG
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
                        (*val) = pub_base_PubDround_1(lx * sdsl);
                    }
                    if ((*dealval) < 0.005)
                    {
                        (*dealval) = 0.00;
                        (*val) = 0.00;
                    }
                    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG
                }  /* end of date93之后开户的 */
                else  /* date93 之前开户的 */
                {

sprintf(acErrMsg,"date93 之前开户的,按活期利率计息 ");
WRITEMSG

                    /* 从ic_date,tx_date分段计息,遇调即调,不分高低 */

                    MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                    ret = pub_base_intstJDQX(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,g_pub_tx.tx_date,intst_term_type);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"界定期限错误");
                        WRITEMSG
                        return 1;
                    }

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],ic_date=[%ld],tx_date=[%ld],\
        intst_term_type=[%d]",llbh,g_td_parm.cur_no,g_td_mst.ic_date,
        g_pub_tx.tx_date,intst_term_type);
WRITEMSG

                    /* rate_val = 利率 * 天数之和 */
                    ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,g_pub_tx.tx_date,intst_term_type,
                          0,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"计算分段积数错误");
                        WRITEMSG
                        return 1;
                    }

sprintf(acErrMsg,"分段积数=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                         * (1 + g_td_mst.flt_ratio) / L360 / L100;
                    (*dealval) = lx;

                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        /* 界定期限 */
                        /* 从开户日到PubMin(到期日,date99) */
                        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

                        ret = pub_base_intstJDQX(llbh,g_td_parm.cur_no,
                            g_td_mst.ic_date,PubMin(g_td_mst.mtr_date,date99),
                            intst_term_type);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"界定期限错误");
                            WRITEMSG
                            return 1;
                        }

                        /* rate_val = 利率 * 天数之和 */
                        ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                             g_td_mst.ic_date,g_pub_tx.tx_date,intst_term_type,
                             0,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算分段积数错误");
                            WRITEMSG
                            return 1;
                        }

sprintf(acErrMsg,"lx=[%lf],分段积数=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        lx,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                        lx = lx - rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;

                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                        if (ret != 0 && ret == 1)
                        {
                            sprintf(acErrMsg,"定期产品利息税税率不存在 %d",
                                     ret);
                            WRITEMSG
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
                        (*val) = pub_base_PubDround_1(lx * sdsl);
                    }

                    if (floor((*dealval) * 100) / 100 < 0.005)
                    {
                        (*dealval) = 0.00;
                        (*val) = 0.00;
                    }
                    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                }
                break;
            case 2 :
                /* 到期支取 */
                /*
                  1. 88/09/10 以前开户的保值储蓄,保值期从88/09/10至到期日
                  2. 89/02/01 以后到期的保值储蓄,从88/09/10起执行89/02/01调整后
                     的储蓄存款利率
                  3. 91/12/01--93/03/01 之间存入的保值储蓄,保值期从93/07/11
                     至到期日
                  4. 93/03/01--93/07/11 之间存入的一年期以上储蓄存款,从开户日
                     到93/07/11 按93/07/11公布的活期利率计息,93/07/11至到期日
                     按93/07/11公布的同档次定期利率计息
                  5. 93/07/11(含)起存入的三五年期按存单上原定的利率计息
                */
            case 3 :
                /* 逾期支取 */

sprintf(acErrMsg,"到期逾期利息计算");
WRITEMSG

                if (g_td_mst.ic_date < date88)
                {

    sprintf(acErrMsg, "----------->起息日期小于88.09.10");
    WRITEMSG

                    if (g_td_mst.mtr_date >= date89)
                    {
                        /* 从开户日到date89分段计息 */
                        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                        strcpy(llbh, g_td_parm.rate_no);
                        ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                              g_td_mst.opn_date,date89,intst_term_type,
                              Lone, &a1);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算分段利率*天数错误 [%d] ",ret);
                            WRITEMSG
                            strcpy(reply,"W111");
                            return ret;
                        }

                        ret=pub_base_CalIntstDays(date88, g_td_mst.mtr_date,
                               intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                        /* date89 相应储种的利率 */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,g_td_parm.rate_no);
                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              date89,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

                        a1 = a1 + dayterm * rate_val;
                    }
                    else
                    {
                        /* 从开户日到到期日分段计息 */
                        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                        strcpy(llbh, g_td_parm.rate_no);
                        ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                              g_td_mst.opn_date,g_td_mst.mtr_date,
                              intst_term_type, Lone, &a1);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算分段利率*天数错误 [%d] ",ret);
                            WRITEMSG
                            strcpy(reply,"W111");
                            return ret;
                        }

                    }

                    /* 计算保值利息 */
                    /* 取交易日的保值利率 */
                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,BAOZHI);
                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                         sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                         WRITEMSG
                         strcpy(reply,"W110");
                         return ret;
                    }

                    ret=pub_base_CalIntstDays(date88,g_td_mst.mtr_date,
                               intst_term_type,&dayterm);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"计算天数错误!!");
                        WRITEMSG
                        return 1;
                    }

                    (*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100 / L360 / L100;
                }
                else /* 88.9.10后 */
                {

    sprintf(acErrMsg, "----------->起息日期大于88.9.10");
    WRITEMSG
                    if (g_td_mst.ic_date < date91)
                    {

sprintf(acErrMsg,"到期逾期利息计算,88.9.10后开户的,date91前开户");
WRITEMSG

                        ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                            g_td_mst.mtr_date,intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                        /* 取date89当日的相应的储种的利率 */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,g_td_parm.rate_no);
                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                   date89,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

                        a1 = rate_val * dayterm ;

                        ret=pub_base_CalIntstDays(date99, g_td_mst.mtr_date,
                               intst_term_type,&daytermval);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                        s1 += rate_val * (double)daytermval;


                        /* 取到期日的保值利率 */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,BAOZHI);
                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_td_mst.mtr_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

                        (*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        s2 += rate_val * daytermval;

                    } /* end of date91 前开户 */
                    else  /* date91 后开户的 */
                    {
                        if (g_td_mst.ic_date < date93)
                        {

sprintf(acErrMsg,"到期逾期利息计算,date91后开户,date93 前开户");
WRITEMSG

                            /* 从计息日到到期日按分段利率计息 */
                            MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                            strcpy(llbh, g_td_parm.rate_no);
                            ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                                  g_td_mst.ic_date,g_td_mst.mtr_date,
                                  intst_term_type, Lone, &a1);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算分段利率*天数错误 [%d] ",ret);
                                WRITEMSG
                                strcpy(reply,"W111");
                                return ret;
                            }

                            /* 到期日 PubMin(到期日，date99) */
                            MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                            strcpy(llbh, g_td_parm.rate_no);
                            ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                                  g_td_mst.ic_date,PubMin(g_td_mst.mtr_date,
                                  date99), intst_term_type, Lone, &s1_tmp);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算分段利率*天数错误 [%d] ",ret);
                                WRITEMSG
                                strcpy(reply,"W111");
                                return ret;
                            }
                            s1 += a1 - s1_tmp;

                        }
                        else /*1993/03/01之后*/
                        {

sprintf(acErrMsg,"到期逾期利息计算,date93 后开户");
WRITEMSG

                            ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                                date9307, intst_term_type,&dayterm);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算天数错误!!");
                                WRITEMSG
                                return 1;
                            }

                            /* 取date9307活期利率 */
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"date93,date9307这段时间按活期计息,llbh=[%s],cur_no=[%s],\
        date=9307",llbh,g_td_parm.cur_no);
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                  date9307,&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"date93,date9307这段时间按活期计息,date9307时的rate_val\
        =[%lf],dayterm=[%d]",rate_val,dayterm);
WRITEMSG

                            a1 = rate_val * dayterm ;

sprintf(acErrMsg,"date93,date9307这段时间按活期计息,积数为=[%lf]",a1);
WRITEMSG

                            ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                                date9307),g_td_mst.mtr_date,
                                intst_term_type,&dayterm);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算天数错误!!");
                                WRITEMSG
                                return 1;
                            }

                            /* 取PubMax(开户日期,date9307)日的相应的利率 */
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"date9307后到到期日按date97同档次利率计息,llbh=[%s], \
        cur_no=[%s],date=[%ld]",llbh,g_td_parm.cur_no,
        PubMax(g_td_mst.ic_date,date9307));
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                PubMax(g_td_mst.ic_date,date9307),&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"date93,date9307这段时间按活期计息,date9307时的rate_val\
        =[%lf],dayterm=[%d]",rate_val,dayterm);
WRITEMSG

                            a2 = rate_val * dayterm ;

sprintf(acErrMsg,"date9307后到到期日,这段时间积数为=[%lf]",a2);
WRITEMSG

                            a1 = a1 + a2 ;

                            ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                                date99),g_td_mst.mtr_date,
                                intst_term_type,&daytermval);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算天数错误!!");
                                WRITEMSG
                                return 1;
                            }

                            s1 = 0.00;
                            s1 = daytermval * rate_val ;
                        }

                        /******* delete by rob at 20030728 ***
                        ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                            date9307),g_td_mst.mtr_date,
                            intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                        * 取到期日的保值利率 *
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,BAOZHI);

sprintf(acErrMsg,"到期日的保值利率 llbh=[%s],cur_no=[%s],mtr_date=[%ld]",
       llbh,g_td_parm.cur_no,g_td_mst.mtr_date);
WRITEMSG

                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_td_mst.mtr_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

sprintf(acErrMsg,"保值利率 rate_val=[%lf],dayterm=[%d],tx_amt=[%lf],\
        flt_ratio=[%lf]",rate_val,dayterm,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                        (*keepval) = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;

sprintf(acErrMsg, "保值利息: keepval=[%lf]", *keepval);
WRITEMSG
                        ******/

                        ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                            date99),g_td_mst.mtr_date,intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算天数错误!!");
                            WRITEMSG
                            return 1;
                        }

                        s2 = 0.00;
                        s2 = rate_val * dayterm ;

                    }

                }

                /* 逾期利息 */
                /* 到期日到date93按分段利率计息 */

                MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                strcpy(llbh, g_td_parm.rate_no);

sprintf(acErrMsg,"逾期利率 到期日到date93,llbh=[%s],cur_no=[%s]",
       llbh,g_td_parm.cur_no);
WRITEMSG
                if (g_td_mst.mtr_date < date93)
                {
                    ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                          g_td_mst.mtr_date,date93,intst_term_type, Lone, &a2);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"计算分段利率*天数错误 [%d] ",ret);
                        WRITEMSG
                        strcpy(reply,"W111");
                        return ret;
                    }

                }
                else
                {
                    a2 = 0.00;
                }

sprintf(acErrMsg,"逾期利率 到期日到date93,这段分段积数为: [%lf]",a2);
WRITEMSG

                /* 取交易当日的活期利率 */
                MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"逾期利率 date93后,llbh=[%s],cur_no=[%s],tx_date=[%ld]",
       llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                      g_pub_tx.tx_date,&rate_val);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                    WRITEMSG
                    strcpy(reply,"W110");
                    return ret;
                }

                ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,date93),
                    g_pub_tx.tx_date, intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"计算天数错误!!");
                    WRITEMSG
                    return 1;
                }


sprintf(acErrMsg,"逾期利率 date93后  rate_val= [%lf],dayterm=[%d]",
        rate_val,dayterm);
WRITEMSG
                a3 = 0;
                a3 = rate_val * dayterm ;

sprintf(acErrMsg,"逾期利率 date93后利息积数为: [%lf]",a3);
WRITEMSG

                ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date, date99),
                    g_pub_tx.tx_date, intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"计算天数错误!!");
                    WRITEMSG
                    return 1;
                }

                s1 += rate_val * dayterm ;

sprintf(acErrMsg,"到期利息积数=[%lf],逾期9307前积数=[%lf],逾期9307后积数=\
        [%lf],tx_amt=[%lf],flt_ratio=[%lf]",a1,a2,a3,g_pub_tx.tx_amt1,
        g_td_mst.flt_ratio);
WRITEMSG
                lx = (a1 + a2 + a3) * floor(g_pub_tx.tx_amt1*L100)/L100
                     * (1 + g_td_mst.flt_ratio) / L360 / L100;
                (*dealval) = lx;

sprintf(acErrMsg, "--------->利息值为:[%lf]", *dealval);
WRITEMSG

                if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0)
                {
                    ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                    if (ret != 0 && ret == 1)
                    {
                        sprintf(acErrMsg,"定期产品利息税税率不存在 %d",
                                 ret);
                        WRITEMSG
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
                    lx = s1 * floor(g_pub_tx.tx_amt1*L100)/L100 * (1 + g_td_mst.flt_ratio)
                         / L360 / L100;
                    /* add by rob at 20030904 */
                    g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
                    (*val) = lx * sdsl ;
                }

                (*dealval) = pub_base_PubDround_1(*dealval);
                (*val) = pub_base_PubDround_1(*val);
                (*keepval) = pub_base_PubDround_1(*keepval);

                (*factval) = (*dealval) - (*val);
                (*keepval) = (*keepval);/* 税后保值利息 */

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                break;
            default:
                return 1;

        }
    }
        /* added by liuxuan 2007-08-01 修改利息税 */
    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
        char edu_ind[2];
        vtcp_log("[%s][%d]开始调用分段算利息税函数\n",__FILE__,__LINE__);
        if (iGetAreaTax(val,factval,dealval,intst_term_type,0,rate_val,rate_val,edu_ind)){
            vtcp_log("[%s][%d]调用分段算利息税函数出错\n",__FILE__,__LINE__);
            return(1);
        }
    }
    /* add over */
sprintf(acErrMsg,"[%s][%d]dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        __FILE__,__LINE__,*dealval,*factval,*val,*keepval);

    return 0;
}
/**********************************************************************
* 函数名:     pub_base_CalIntstTdZCZQ_TZ()
* 函数功能:   计算1,7天整存整取利息（理财型通知存款）
* 作者/时间:  LiuHuafeng 2009-2-6 18:16:21
*
* 参数:
*     输入:
*     输出: 应付利息          double
*           实付利息          double
*           代扣利息税        double
*           保值利息          double
*           响应码            char(4)
* 返 回 值: 0 成功
*
* 修改历史:
* 注意事项: 本函数仅支持理财型通知存款处理方式，不兼容以前老的通知存款处理模式
********************************************************************/
int pub_base_CalIntstTdZCZQ_TZ(long real_tx_date,double *dealval, double *factval,
    double *val, double * keepval, char *reply)
{
    int draw_flag; /* 到期标志  1-提前支取,2-到期支取,3-逾期支取 */
    int monterm,monterm1;
    long dayterm; /* 月 日存期 */
    long daytermval;      /* 代扣个人所得利息天数 */
    double  amtval;      /* 计算所得税金额 */
    double  sdsl;        /* 个人所得税率 */
    double rate_val; /* 利率 */
    double lx;      /* 利息 */
    long date88,date89,date93,date9307,date99;
    long date91,date97,date98,date00;
    char   llbh[4]; /* 利率编号 */
    int ret;
    double a1,a2,a3,s1,s1_tmp,s2; /* 存期*利率 */
    char   intst_term_type[2] ; /* 计算天数类型 */

    memset(llbh,0,sizeof(llbh));
    memset(intst_term_type,0,sizeof(intst_term_type));

    sprintf(acErrMsg,"g_td_parm.term_type=[%s],term=[%d],prdt_no=[%s], \
        tx_amt=[%lf],mtr_date[%ld]",g_td_parm.term_type,g_td_parm.term,
        g_td_parm.prdt_no, g_pub_tx.tx_amt1,g_td_mst.mtr_date);
    WRITEMSG

        date88 = 19880910;
    date89 = 19890201;
    date91 = 19911201;
    date93 = 19930301;
    date9307 = 19930711;
    date97 = 19971023;
    date98 = 19981206;
    date99 = 19991101;
    date00 = 20000713;
    (*keepval) = 0.00; /* 保值利息 */
    (*dealval) = 0.00; /* 应付利息 */
    (*factval) = 0.00; /* 实付利息 */
    (*val) = 0.00;     /* 代扣利息税 */

    /* 判断交易日期是否到期 */
    if (g_pub_tx.tx_date <  g_td_mst.mtr_date)
    {
        /******************start modified by ligl 增加节假日判断********************************/
        if (g_td_parm.holi_ind[0]=='1')
        {
            ret=pub_base_checktxdate(g_pub_tx.tx_date,g_td_mst.mtr_date,'2');
            if (ret==100)
            {
                draw_flag = 1; /* 提前 */
            }
            else if (ret==0)
            {
                draw_flag = 2; /* 到期 */
            }
            else
            {
                draw_flag = 1; /* 提前 */
            }
        }
        else
        {
            draw_flag = 1; /* 提前 */
        }
        /********************end modified by ligl*************************************/
    }
    else if (g_pub_tx.tx_date == g_td_mst.mtr_date)
    {
        draw_flag = 2; /* 到期 */
    }
    else
    {
        draw_flag = 3; /* 逾期 */
    }
    /* 计算存期 */
    switch (g_td_parm.term_type[0])
    {
    case 'Y' :
        monterm = g_td_parm.term * 12;
        break;
    case 'M' :
        monterm = g_td_parm.term;
        break;
    case 'D' :
        monterm = g_td_parm.term / 30;
        break;
    default:
        /* 错误返回 */
        sprintf(acErrMsg,"定期产品表中存期类型错误 %c",g_td_parm.term_type);
        WRITEMSG
            strcpy(reply,"W108");
        return 1;
    }
    strcpy(intst_term_type,g_td_parm.intst_term_type);/***按实际天数计息***/
    if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
        sprintf(acErrMsg,"产品表中的计息天数类型错误 type=[%s]",
            g_td_parm.intst_term_type);
        WRITEMSG
            MEMSET_DEBUG(reply, 0x0, sizeof(reply));
        strcpy(reply, "W107");
        return 1;
    }
    sprintf(acErrMsg,"开始计算利息monterm=[%d],draw_flag=[%d]",monterm,draw_flag);
    WRITEMSG
    vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
    sprintf(acErrMsg,"普通利息计算");
    WRITEMSG
        vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
    /* 普通利息计算 */
    sprintf(acErrMsg,"普通利息计算--930301之后存入的整存整取");
    WRITEMSG
        vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
    switch (draw_flag)
    {
    case 1:
        /*
        提前支取 按支取日挂牌公告的活期储蓄存款利率计付利息
        1.计算计息日期到交易日期的总天数
        2.取当前挂牌公告的活期储蓄存款利率
        3.计算利率
        4.99年后个人需要交个人所得税
        */
        vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        sprintf(acErrMsg,"普通利息计算--提前支取");
        WRITEMSG
            sprintf(acErrMsg,"ic_date=[%ld],tx_date=[%ld]",g_td_mst.ic_date,g_pub_tx.tx_date);
        WRITEMSG
            ret=pub_base_CalIntstDays(g_td_mst.ic_date,
            g_pub_tx.tx_date, intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"计算天数错误!!");
            WRITEMSG
                return 1;
        }
        /* 取活期利率 */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);
        vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        sprintf(acErrMsg,"活期 : llbh=[%s],cur_no=[%s],date=[%ld]",
            llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
        WRITEMSG

        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
            g_pub_tx.tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
                strcpy(reply,"W110");
            return ret;
        }
        sprintf(acErrMsg,"利息 : dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
        WRITEMSG

        lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
            * (1 + g_td_mst.flt_ratio) / L360 / L100;
        lx = pub_base_PubDround_1(lx) ;
        (*dealval) = lx;

        if (floor(lx * 100) / 100 < 0.005)
        {
            lx = 0.00;
            (*dealval) = 0.00;
            (*factval) = 0.00;
            (*val) = 0.00;
        }
        /* 计算个人所得税 */
        if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
        {
            ret=pub_base_CalIntstDays(PubMax(date99,
                g_td_mst.ic_date),g_pub_tx.tx_date,
                intst_term_type,&daytermval);
            if (ret != 0)
            {
                sprintf(acErrMsg,"计算天数错误!!");
                WRITEMSG
                    return 1;
            }
            amtval = lx * daytermval / dayterm;

            ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &sdsl);
            if ((ret != 0) && (ret == 1))
            {
                sprintf(acErrMsg,"定期产品利息税税率不存在 %d",ret);
                WRITEMSG
                    strcpy(reply,"W100");
                return 1;
            }
            else if (ret < 0)
            {
                sprintf(acErrMsg,"取利息税税率错误 %d ", ret);
                WRITEMSG
                    strcpy(reply,"W101");
                return 1;
            }
            g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
            (*val) = amtval * sdsl;
            (*val) = pub_base_PubDround_1(*val);
        }
        (*factval) = (*dealval) -(*val);

        sprintf(acErrMsg,"利息 : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
            *dealval,*factval,*val,*keepval);
        WRITEMSG
            vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        break;
    case 2:
        /* 到期支取
        1. 计算到期天数
        2. 取对应产品的日利率
        3. 计算利息
        4. 99后计算个人所得税
        */
    case 3:
        /* 逾期支取
        1.930301--930711 按活期利息计算
        2.930711--到期日 按930711同档次利率计算
        3.到期日--交易日 按交易日活期利率计算
        4.计算99后的个人所得税
        */
        vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

        sprintf(acErrMsg,"普通利息计算-- 到期或逾期支取");
        WRITEMSG

        sprintf(acErrMsg,"ic_date=[%ld],tx_date=[%ld]",g_td_mst.ic_date,g_pub_tx.tx_date);
        WRITEMSG
        vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        ret=pub_base_CalIntstDays(g_td_mst.ic_date,
            g_td_mst.mtr_date, intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"计算天数错误!!");
            WRITEMSG
                return 1;
        }
        /* 取该产品的对应日利率 */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,g_td_parm.rate_no);

        sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[%ld],定期天数[%ld]",
            llbh,g_td_parm.cur_no,g_td_mst.ic_date,dayterm);
        WRITEMSG
        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
            real_tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
                strcpy(reply,"W110");
            return ret;
        }
        sprintf(acErrMsg,"到期:dayterm=[%ld],rate_val=[%lf],tx_amt=[%d],flt_ratio=[%lf]",
            dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
        WRITEMSG
        lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
            * (1 + g_td_mst.flt_ratio) / L360 / L100;
        (*dealval) = lx;
        sprintf(acErrMsg,"注意到期利息:[%lf]",lx);
        WRITEMSG
            vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

        /* 逾期利息 */
        if (g_td_parm.ovrd_intst_ind[0] == 'Y')
        {
            /*
            ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,
            date99), g_pub_tx.tx_date, intst_term_type,
            &dayterm);
            */
            ret=pub_base_CalIntstDays(g_td_mst.mtr_date,
                g_pub_tx.tx_date, intst_term_type,
                &dayterm);
            if (ret != 0)
            {
                sprintf(acErrMsg,"计算天数错误!!");
                WRITEMSG
                    return 1;
            }
            vtcp_log("[%s][%d]逾期后的天数[%ld]\n",__FILE__,__LINE__,dayterm);
        }
        else
        {
            dayterm = 0;

        }
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

        sprintf(acErrMsg,"逾期: llbh=[%s],cur_no=[%s],date=[%ld],dayterm[%ld]",
            llbh,g_td_parm.cur_no,g_pub_tx.tx_date,dayterm);
        WRITEMSG

        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
            g_pub_tx.tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
            strcpy(reply,"W110");
            return ret;
        }
        lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100
            * (1 + g_td_mst.flt_ratio) / L360 / L100;
        sprintf(acErrMsg,"逾期利息: dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],\
            flt_ratio=[%lf] lx[%lf]",dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio,lx);
        WRITEMSG
            (*dealval) = (*dealval) + lx;
        sprintf(acErrMsg,"注意总利息: dealval[[%lf]",*dealval);
        WRITEMSG
        /* 计算个人所得税 */
        if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
        {
            ret=pub_base_CalIntstDays(g_td_mst.ic_date, g_td_mst.mtr_date,
                intst_term_type,&daytermval);
            if (ret != 0)
            {
                sprintf(acErrMsg,"计算天数错误!!");
                WRITEMSG
                    return 1;
            }
            /* 利率编号从产品里取 */
            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
            strcpy(llbh,g_td_parm.rate_no);

            sprintf(acErrMsg,"利息税:llbh=[%s],cur_no=[%s],date=[%ld]",
                llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
            WRITEMSG
            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                real_tx_date,&rate_val);
            if (ret != 0)
            {
                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                WRITEMSG
                    strcpy(reply,"W110");
                return ret;
            }
            sprintf(acErrMsg,"定期部分应该缴利息税的参数1:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
            WRITEMSG
                vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
            amtval = rate_val * daytermval
                * floor(g_pub_tx.tx_amt1*L100)/L100
                * (1 + g_td_mst.flt_ratio) / L360 / L100;

            ret=pub_base_CalIntstDays(PubMax(date99,
                g_td_mst.mtr_date), g_pub_tx.tx_date,
                intst_term_type,&daytermval);
            if (ret != 0)
            {
                sprintf(acErrMsg,"计算天数错误!!");
                WRITEMSG
                    return 1;
            }
            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
            strcpy(llbh,HQLLBH);

            sprintf(acErrMsg,"利息税:llbh=[%s],cur_no=[%s],date=[%ld]",
                llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
            WRITEMSG

            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                g_pub_tx.tx_date,&rate_val);
            if (ret != 0)
            {
                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                WRITEMSG
                    strcpy(reply,"W110");
                return ret;
            }
            sprintf(acErrMsg,"逾期部分应该缴利息税的参数2:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf], \
                flt_ratio=[%lf]", daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
            WRITEMSG

            lx = daytermval * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                * (1 + g_td_mst.flt_ratio) / L360 / L100;
            sprintf(acErrMsg,"逾期部分应该缴税的利息是:lx[%lf]",lx);
            WRITEMSG
                sprintf(acErrMsg,"定期应该应缴税的利息是amtval[%lf]",amtval);
            WRITEMSG
                amtval = amtval + lx;
            sprintf(acErrMsg,"总的应该缴税的利息是amtval[%lf]",amtval);
            WRITEMSG
                ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
            if ((ret != 0) && (ret == 1))
            {
                sprintf(acErrMsg,"定期产品利息税税率不存在 %d",
                    ret);
                WRITEMSG
                    strcpy(reply,"W100");
                return 1;
            }
            else if (ret < 0)
            {
                sprintf(acErrMsg,"取利息税税率错误 %d ",
                    ret);
                WRITEMSG
                    strcpy(reply,"W101");
                return 1;
            }
            g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
            (*val) = amtval * sdsl;
            (*val) = pub_base_PubDround_1(*val);
        }
        (*dealval)= pub_base_PubDround_1(*dealval);
        (*factval) = (*dealval) -(*val);
        if (floor((*dealval)*100)/100<0.005)
        {
            lx = 0.00;
            (*dealval) = 0.00;
            (*factval) = 0.00;
            (*val) = 0.00;
        }
        sprintf(acErrMsg,"利息 : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
            *dealval,*factval,*val,*keepval);
        WRITEMSG
        break;
    default:
        /* 错误返回 */
        return 1;
    }
    vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

    vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

    vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

    /* added by liuxuan 2007-08-01 修改利息税 */
    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
    {
        char edu_ind[2];
        vtcp_log("[%s][%d]开始调用分段算利息税函数intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        if (iGetAreaTax_ZCZQ_TK(real_tx_date,val,factval,dealval,intst_term_type,0,rate_val,rate_val,edu_ind))
        {
            vtcp_log("[%s][%d]调用分段算利息税函数出错\n",__FILE__,__LINE__);
            return(1);
        }
    }
    /* add over */

    sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);

    return 0;
}
