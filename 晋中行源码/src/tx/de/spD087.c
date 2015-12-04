/***************************************************************
* 文 件 名: spD087.c
* 功能描述：实现理财产品客户发起的提前中止
考虑到循环的问题，直接在本交易内记科目账了
* 作    者: shangyongchao
* 完成日期：2013/5/21
*
* 修改记录:
* 日    期:
* 修 改 人:
* 备    注:
*******************************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_reg_hst_c.h"
#include "redempt_rate_c.h"
#include "fn_redempt_hst_c.h"
#include "fn_ac_rel_c.h"
#include "fn_br_profit_c.h"
#include "fn_br_quota_parm_c.h"
int spD087()
{
    int iRet = 0;
    long lDays = 0;/*天数*/
    double dFct_rate = 0.000000;/*实际利率*/
    double dFct_intst = 0.00;/*本次收益*/
    double dTmp_amt = 0.00;/*临时金额*/
    double dRtbj_amt = 0.00;/*本次归还的实际本金*/
    double dProfit_amt = 0.00;/*本次分润金额*/
    double dProfit_rate = 0.000000;/*分润利率*/
    char cFd123[1000];
    char cTmp[21];
    char cBuy_date[9];
    char cBuy_trace_no[10];
    char cAgent_id_type[2];/*代理人证件类型*/
    char cAgent_id[21];/*代理人证件号码*/
    char cAgent_name[61];/*代理人姓名*/
    struct fn_reg_hst_c sFn_reg_hst;
    struct fn_reg_c sFn_reg;
    struct fn_parm_c sFn_parm;
    struct fn_redempt_hst_c sFn_redempt_hst;
    struct redempt_rate_c sRedempt_rate;
    struct fn_ac_rel_c sFn_ac_rel;
    struct fn_br_profit_c sFn_br_profit;
    
    memset(cFd123,0x00,sizeof(cFd123));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cAgent_id,0x00,sizeof(cAgent_id));
    memset(cAgent_name,0x00,sizeof(cAgent_name));

    pub_base_sysinit();

    get_zd_data("0690",cAgent_id_type);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0250",cAgent_name);
    get_zd_data("1230",cFd123);
    vtcp_log("123域[%s]",cFd123);
    for(int i = 1; ; i++)
    {
        lDays = 0;/*天数*/
        dFct_rate = 0.000000;/*实际利率*/
        dProfit_rate = 0.000000;/*分润利率*/
        dFct_intst = 0.00;/*本次收益*/
        dProfit_amt = 0.00;/*本次分润*/
        dTmp_amt = 0.00;/*临时金额*/
        dRtbj_amt = 0.00;/*本次归还的实际本金*/
        memset(cBuy_date,0x00,sizeof(cBuy_date));
        memset(cBuy_trace_no,0x00,sizeof(cBuy_trace_no));
        memset(cTmp,0x00,sizeof(cTmp));
        memset(&sFn_reg,0x00,sizeof(sFn_reg));
        memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        memset(&sFn_redempt_hst,0x00,sizeof(sFn_redempt_hst));
        memset(&sRedempt_rate,0x00,sizeof(sRedempt_rate));
        memset(&sFn_ac_rel,0x00,sizeof(sFn_ac_rel));
        memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
        
        pub_base_cut_str( cFd123, cTmp, "|", i );
        vtcp_log("cTmp[%s] i[%d]",cTmp,i);
        if( strlen(cTmp) == 0 )
        {
            vtcp_log("数据为空!");
            break;
        }
        strncpy(cBuy_date,cTmp,8);
        strncpy(cBuy_trace_no,cTmp+8,9);
        vtcp_log("cBuy_date[%s],cBuy_trace_no[%s]",cBuy_date,cBuy_trace_no);
        /*根据日期和流水号找认购记录 暂时按每笔全部赎回处理*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld",atoi(cBuy_date),atoi(cBuy_trace_no));
        if(iRet)
        {
            vtcp_log("建立更新游标错误! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("取更新游标错误! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D335");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            vtcp_log("无此认购记录 iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D336 ");
            goto ErrExit;
        }
        if(strcmp(g_pub_tx.tx_br_no,sFn_reg.tx_br_no) != 0)
        {
            vtcp_log("不是本机构认购的不能在本机构赎回tx_br_no[%s]",sFn_reg.tx_br_no);
            strcpy(g_pub_tx.reply,"P511");
            goto ErrExit;
        }
        if(sFn_reg.sts[0] != '1')
        {
            vtcp_log("记录不是扣款状态不能赎回 sts[%s] ",sFn_reg.sts);
            strcpy(g_pub_tx.reply,"D337");
            goto ErrExit;
        }
        /*查询产品是否能赎回*/
        iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
        if(iRet)
        {
            vtcp_log("没有这个理财产品! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
            strcpy(g_pub_tx.reply,"D104");
            goto ErrExit;
        }
        /*这里应该加一下这个产品当天一共赎回了多少，防止挤兑*/
        /*验证产品状态*/
        if(sFn_parm.stat[0] != '1')
        {
            vtcp_log("理财产品不是正在运行的状态,不能赎回 stat[%s] ",sFn_parm.stat);
            strcpy(g_pub_tx.reply,"D338");
            goto ErrExit;
        }
        /*验证产品是否赎回*/
        if(sFn_parm.redempt[0] != 'Y')
        {
            vtcp_log("理财产品不允许赎回 redempt[%s] ",sFn_parm.redempt);
            strcpy(g_pub_tx.reply,"D339");
            goto ErrExit;
        }
        /*取产品的清算账号信息*/
        iRet = Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code='%s'",sFn_reg.prdt_code);
        if(iRet)
        {
            vtcp_log("没有这个理财产品的清算账号信息! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
            strcpy(g_pub_tx.reply,"D340");
            goto ErrExit;
        }
        /*赎回的总和不能大于认购总额*/
        iRet = pub_base_CompDblVal(sFn_reg.redempt_amt + sFn_reg.tx_amt,sFn_reg.tx_amt);
        if(iRet > 0)
        {
            vtcp_log("赎回总额[%lf]大于认购总额[%lf]",sFn_reg.redempt_amt+sFn_reg.tx_amt,sFn_reg.tx_amt);
            strcpy(g_pub_tx.reply,"D341");
            goto ErrExit;
        }
        else if(iRet == 0)
        {
            strcpy(sFn_reg.sts,"2");/*改成已分配收益*/
        }
        /*根据是否是开放式的进行处理*/
        if(sFn_parm.prdt_type[0] == '1')
        {
            /*封闭式的不存在利率调整的问题*/
        }
        else if(sFn_parm.prdt_type[0] == '2')
        {
            /*开放式的存在因利率调整出现的未分配收益 按本金的比例取收益*/
            /*分润金额也按本金的比例统计出来*/
            if(pub_base_CompDblVal(sFn_reg.tx_amt,sFn_reg.tx_amt - sFn_reg.redempt_amt) == 0)
            {
                dFct_intst += sFn_reg.amt1;
                sFn_reg.amt1 =0.00;
                dProfit_amt += sFn_reg.amt2;
                sFn_reg.amt2 =0.00;
            }
            else
            {
                dTmp_amt = pub_base_PubDround_1((sFn_reg.tx_amt/(sFn_reg.tx_amt - sFn_reg.redempt_amt)) * sFn_reg.amt1);
                vtcp_log("赎回本金对应的未分配收益[%lf]",dTmp_amt);
                dFct_intst += dTmp_amt;
                sFn_reg.amt1 -= dTmp_amt;/*把本次的减掉*/
                if(pub_base_CompDblVal(sFn_reg.amt1,0.00) < 0)
                {
                    sFn_reg.amt1 = 0.00;
                }
                dTmp_amt = pub_base_PubDround_1((sFn_reg.tx_amt/(sFn_reg.tx_amt - sFn_reg.redempt_amt)) * sFn_reg.amt2);
                vtcp_log("赎回本金对应的未统计的分润金额[%lf]",dTmp_amt);
                dProfit_amt += dTmp_amt;
                sFn_reg.amt2 -= dTmp_amt;/*把本次的减掉*/
                if(pub_base_CompDblVal(sFn_reg.amt2,0.00) < 0)
                {
                    sFn_reg.amt2 = 0.00;
                }
            }
        }
        else
        {
            vtcp_log("产品类型错误[%s]",sFn_parm.prdt_type);
            strcpy(g_pub_tx.reply,"S092");
            goto ErrExit;
        }

        /*根据收益类型确定 返回本金 确定是不是算收益 确定计息天数*/
        /*目前按每笔全部赎回算，redempt_amt其实都是0*/
        if(sFn_parm.intst_type[0] == '1' || sFn_parm.intst_type[0] == '2')
        {
            lDays = 1 +  pub_base_CalTrueDays(sFn_reg.ic_date,g_pub_tx.tx_date);
            dRtbj_amt = sFn_reg.tx_amt  - sFn_reg.redempt_amt;
        }
        else
        {
            if(sFn_parm.capital_percent - 100.000000 > 0.0000005)
            {               
                vtcp_log("返还本金比例大于百分之100 参数错误! sFn_parm.capital_percent=[%lf]",sFn_parm.capital_percent);
                strcpy(g_pub_tx.reply,"P101");
                goto ErrExit; 
            }
            else if(sFn_parm.capital_percent - 100.000000 < 0.0000005)
            {
                /*本金都收不回来了 就不用算收益了 天数设置成0 就没收益了*/
                lDays = 0;
                dRtbj_amt = pub_base_PubDround_1((sFn_reg.tx_amt - sFn_reg.redempt_amt) * sFn_parm.capital_percent/100);
            }
            else /*本金能全部返回 再考虑是不是 有收益*/
            {
                lDays = 1 +  pub_base_CalTrueDays(sFn_reg.ic_date,g_pub_tx.tx_date);
                dRtbj_amt = sFn_reg.tx_amt  - sFn_reg.redempt_amt;
            }
        }
        sFn_reg.return_capital += dRtbj_amt;
        vtcp_log("确定返回本金[%lf] 累计返回本金[%lf]",dRtbj_amt,sFn_reg.return_capital);
        sFn_reg.redempt_amt += sFn_reg.tx_amt;/*累加赎回金额*/
        /*算赎回费用 暂时按赎回本金*赎回费率算*/
        sFn_redempt_hst.redempt_chrg_amt =  sFn_reg.tx_amt * sFn_parm.redempt_chrg / 100;
        sFn_redempt_hst.redempt_chrg_amt = pub_base_PubDround_1(sFn_redempt_hst.redempt_chrg_amt);
        /*取实际赎回收益率 根据天数、收益率、浮动比例算收益 */
        iRet = get_fct_rate("001",sFn_reg.prdt_code,lDays,&sFn_reg.return_rate,&dProfit_rate);
        if(iRet)
        {
            vtcp_log("获取赎回收益率错误");
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        /*收益在这里都按日利率算 分润金额按实际天数重新算*/
        dTmp_amt = dRtbj_amt * lDays * sFn_reg.return_rate * (1+ sFn_reg.flt_radio)/L360/L100;
        dFct_intst = pub_base_PubDround_1(dTmp_amt);
        sFn_reg.fct_intst += dFct_intst;
        vtcp_log("tmp_amt[%lf] dFct_intst[%lf] fct_intst[%lf]",dTmp_amt,dFct_intst,sFn_reg.fct_intst);

        dTmp_amt= dRtbj_amt * lDays * dProfit_rate/L360/L100;
        dProfit_amt = pub_base_PubDround_1(dTmp_amt);
        sFn_reg.fct_profit_amt += dProfit_amt;
        vtcp_log("tmp_amt[%lf] dProfit_amt[%lf] fct_profit_amt[%lf]",dTmp_amt,dProfit_amt,sFn_reg.fct_profit_amt);

        /*记理财本金账户  分户-*/
        if(pub_base_CompDblVal(dRtbj_amt,0.00) > 0)
        {
            if(sFn_reg.cif_type[0] == '1')
            {
                strcpy(g_pub_tx.ac_no,sFn_parm.per_ac_no);
            }
            else
            {
                strcpy(g_pub_tx.ac_no,sFn_parm.com_ac_no);
            }
            strcpy(g_pub_tx.name,"");
            g_pub_tx.ac_id = 0;
            g_pub_tx.ac_seqn = 0;
            strcpy(g_pub_tx.ac_wrk_ind,"00000");
            strcpy(g_pub_tx.ac_get_ind, "00");
            strcpy(g_pub_tx.ac_id_type, "9");
            g_pub_tx.add_ind[0]='0';
            strcpy(g_pub_tx.hst_ind,"1");
            strcpy(g_pub_tx.ct_ind,"2");
            g_pub_tx.svc_ind = 9001;
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"理财返还本金");
            g_pub_tx.tx_amt1 = dRtbj_amt;
            vtcp_log("本金[%f]",g_pub_tx.tx_amt1);
            iRet = pub_acct_trance();
            if (iRet)
            {
                vtcp_log("扣理财本金账号错误!!");
                goto ErrExit;
            }
            /*记理财本金账户 科目*/
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("102I",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC01") )
            {
                vtcp_log("登记会计流水错误!");
                goto ErrExit;
            }
        }
        if(pub_base_CompDblVal(dFct_intst,0.00) > 0)
        {
            /*记理财清算账户 分户-*/
            strcpy(g_pub_tx.ac_no,sFn_ac_rel.qs_ac_no);
            strcpy(g_pub_tx.name,"");
            g_pub_tx.ac_id = 0;
            g_pub_tx.ac_seqn = 0;
            strcpy(g_pub_tx.ac_wrk_ind,"00000");
            strcpy(g_pub_tx.ac_get_ind, "00");
            strcpy(g_pub_tx.ac_id_type, "9");
            g_pub_tx.add_ind[0]='0';
            strcpy(g_pub_tx.hst_ind,"1");
            strcpy(g_pub_tx.ct_ind,"2");
            g_pub_tx.svc_ind = 9001;
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"理财赎回收益");
            g_pub_tx.tx_amt1 = dFct_intst;
            vtcp_log("收益[%f]",g_pub_tx.tx_amt1);
            iRet = pub_acct_trance();
            if (iRet)
            {
                vtcp_log("扣理财收益清算账号错误!!");
                goto ErrExit;
            }
            /*记理财清算账户 科目*/
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("102I",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC01") )
            {
                vtcp_log("登记会计流水错误!");
                goto ErrExit;
            }
        }
        if(pub_base_CompDblVal(sFn_redempt_hst.redempt_chrg_amt,0.00) > 0)
        {
            /*记理财清算的赎回费用收益 分户+*/
            strcpy(g_pub_tx.ac_no,sFn_ac_rel.sy_ac_no);
            strcpy(g_pub_tx.name,"");
            g_pub_tx.ac_id = 0;
            g_pub_tx.ac_seqn = 0;
            strcpy(g_pub_tx.ac_wrk_ind,"00000");
            strcpy(g_pub_tx.ac_get_ind, "00");
            strcpy(g_pub_tx.ac_id_type, "9");
            g_pub_tx.add_ind[0]='1';
            strcpy(g_pub_tx.hst_ind,"1");
            strcpy(g_pub_tx.ct_ind,"2");
            g_pub_tx.svc_ind = 9001;
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"理财赎回收益");
            g_pub_tx.tx_amt1 = sFn_redempt_hst.redempt_chrg_amt;
            vtcp_log("收益[%f]",sFn_redempt_hst.redempt_chrg_amt);
            iRet = pub_acct_trance();
            if (iRet)
            {
                vtcp_log("扣理财收益清算账号错误!!");
                goto ErrExit;
            }
            /*记理财清算的赎回费用收益 科目*/
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("102I",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC02") )
            {
                vtcp_log("登记会计流水错误!");
                goto ErrExit;
            }
        }

        g_pub_tx.tx_amt1 = dRtbj_amt + dFct_intst - sFn_redempt_hst.redempt_chrg_amt;
        vtcp_log("返还金额[%f]",g_pub_tx.tx_amt1);
        if(pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00) > 0)
        {
            /*记客户账的本金加收益 分户+*/
            strcpy(g_pub_tx.ac_no,sFn_reg.ac_no);
            strcpy(g_pub_tx.name,"");
            g_pub_tx.ac_id = sFn_reg.ac_id;
            g_pub_tx.ac_seqn = sFn_reg.ac_seqn;
            strcpy(g_pub_tx.ac_wrk_ind,"00000");
            strcpy(g_pub_tx.ac_get_ind, "00");
            strcpy(g_pub_tx.ac_id_type, "1");
            g_pub_tx.add_ind[0]='1';
            strcpy(g_pub_tx.hst_ind,"1");
            strcpy(g_pub_tx.ct_ind,"2");
            g_pub_tx.svc_ind = 1001;
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"理财赎回入账");
            iRet = pub_acct_trance();
            if (iRet)
            {
                vtcp_log("扣理财收益清算账号错误!!");
                goto ErrExit;
            }
            /*记客户账的本金加收益 科目*/
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("102I",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC02") )
            {
                vtcp_log("登记会计流水错误!");
                goto ErrExit;
            }
        }
        sFn_reg.end_date = g_pub_tx.tx_date;
        strcpy(sFn_reg.profit_flag,"Y");
        sFn_reg.return_date  = g_pub_tx.tx_date;
        sFn_reg.return_trace_no  = g_pub_tx.trace_no;
        strcpy(sFn_reg.filler,"客户提前赎回");
        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("更新fn_reg错误! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D333");
            goto ErrExit;
        }
        /*登记赎回明细*/
        sFn_redempt_hst.tx_date = g_pub_tx.tx_date;
        sFn_redempt_hst.trace_no = g_pub_tx.trace_no;
        strcpy(sFn_redempt_hst.tx_br_no,g_pub_tx.tx_br_no);
        strcpy(sFn_redempt_hst.pact_no,sFn_reg.pact_no);
        strcpy(sFn_redempt_hst.prdt_code,sFn_reg.prdt_code);
        strcpy(sFn_redempt_hst.ac_no,sFn_reg.ac_no);
        sFn_redempt_hst.ac_id  = sFn_reg.ac_id;
        sFn_redempt_hst.ac_seqn  = sFn_reg.ac_seqn;
        /*strcpy(sFn_redempt_hst.chnl_type,g_pub_rule.chnl_type);*/
        sFn_redempt_hst.redempt_amt = sFn_reg.tx_amt;
        sFn_redempt_hst.return_rate = sFn_reg.return_rate;
        sFn_redempt_hst.ic_date = sFn_reg.ic_date;
        sFn_redempt_hst.fct_intst = sFn_reg.fct_intst;
        sFn_redempt_hst.return_capital = dRtbj_amt;
        sFn_redempt_hst.profit_amt = dProfit_amt;
        strcpy(sFn_redempt_hst.profit_flag,"Y");
        strcpy(sFn_redempt_hst.tel,g_pub_tx.tel);
        sFn_redempt_hst.return_date = g_pub_tx.tx_date;
        sFn_redempt_hst.return_trace_no = g_pub_tx.trace_no;
        strcpy(sFn_redempt_hst.sts,"1");
        sFn_redempt_hst.buy_date = sFn_reg.tx_date;
        sFn_redempt_hst.buy_trace_no = sFn_reg.trace_no;
        iRet = Fn_redempt_hst_Ins(sFn_redempt_hst,g_pub_tx.reply);
        if(iRet)
        {
            Fn_redempt_hst_Debug(&sFn_redempt_hst);
            vtcp_log("登记赎回明细错误");
            strcpy(g_pub_tx.reply,"D342");
            goto ErrExit;
        }
        /*登记机构分润*/
        iRet = Fn_br_profit_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s'",sFn_reg.prdt_code,g_pub_tx.tx_br_no);
        if(iRet)
        {
            vtcp_log("定义更新机构分润表失败");
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        iRet = Fn_br_profit_Fet_Upd(&sFn_br_profit,g_pub_tx.reply);
        if(iRet  == 100)
        {
            vtcp_info("没有机构的分润信息，登记一条");
            strcpy(sFn_br_profit.prdt_code,sFn_reg.prdt_code);
            strcpy(sFn_br_profit.br_no,g_pub_tx.tx_br_no);
            sFn_br_profit.tx_amt = sFn_reg.tx_amt;
            sFn_br_profit.redempt_amt = sFn_reg.tx_amt;
            sFn_br_profit.profit_amt = dProfit_amt;
            sFn_br_profit.reg_date = g_pub_tx.tx_date;
            sFn_br_profit.reg_trace_no = g_pub_tx.trace_no;
            strcpy(sFn_br_profit.sts,"0");
            strcpy(sFn_br_profit.brf,"赎回登记");
            iRet = Fn_br_profit_Ins(sFn_br_profit,g_pub_tx.reply);
            if(iRet)
            {
                Fn_br_profit_Debug(&sFn_br_profit);
                vtcp_log("登记机构分润表失败");
                strcpy(g_pub_tx.reply,"D343");
                goto ErrExit;
            }
        }
        else if(iRet == 0)
        {
            vtcp_info("有分润信息 更新");
            sFn_br_profit.tx_amt += sFn_reg.tx_amt;
            sFn_br_profit.redempt_amt += sFn_reg.tx_amt;
            sFn_br_profit.profit_amt += dProfit_amt;
            iRet = Fn_br_profit_Upd_Upd(sFn_br_profit,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("更新机构分润表失败");
                strcpy(g_pub_tx.reply,"D344");
                goto ErrExit;
            }
        }
        Fn_br_profit_Clo_Upd();
        /*登记操作纪录*/
        sFn_reg_hst.tx_date = g_pub_tx.tx_date;
        sFn_reg_hst.trace_no = g_pub_tx.trace_no;
        strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
        strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
        strcpy(sFn_reg_hst.type,"6");
        sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
        strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
        strcpy(sFn_reg_hst.agent_id,cAgent_id);
        strcpy(sFn_reg_hst.agent_name,cAgent_name);
        sFn_reg_hst.buy_date=sFn_reg.tx_date;
        sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
        strcpy(sFn_reg_hst.brf,"客户赎回");
        iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
        if(iRet)
        {
            Fn_reg_hst_Debug(&sFn_reg_hst);
            vtcp_log("登记理财产品登记簿错误iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D345");
            goto ErrExit;
        }
        Fn_reg_Clo_Upd();
        /*更改本机构配额 赎回时把本机构的配额加回去*/
        iRet = upd_br_quota(sFn_parm,sFn_reg.tx_amt,1);
        if(iRet)
        {
            vtcp_log("更新机构配额错误[%d]",iRet);
            goto ErrExit;
        }
        /*往回返信息这里 应该写个总值  先按单条记录的测试吧*/
        set_zd_double("0390",sFn_reg.tx_amt);
        set_zd_double("0400",sFn_reg.fct_intst);
        set_zd_double("0840",sFn_reg.return_rate);
        set_zd_double("1001",dRtbj_amt);
        set_zd_double("0410",sFn_redempt_hst.redempt_chrg_amt);
        set_zd_double("0420",dRtbj_amt + sFn_reg.fct_intst - sFn_redempt_hst.redempt_chrg_amt);
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
/*根据产品 天数 取赎回利率*/
int get_fct_rate(char *rate_code,char * prdt_code,long days, double *fct_rate,double *profit_rate)
{
    int iRet = 0;
    struct redempt_rate_c sRedempt_rate;

    memset(&sRedempt_rate,0x00,sizeof(sRedempt_rate));

    iRet = Redempt_rate_Sel(g_pub_tx.reply,&sRedempt_rate,"prdt_code='%s' and rate_code='%s' and beg_date<=%ld and end_date>=%ld and min_days<=%ld and max_days>=%ld",\
            prdt_code,rate_code,g_pub_tx.tx_date,g_pub_tx.tx_date,days,days);
    if(iRet == 100)
    {
        vtcp_log("没有该理财产品的分段利率信息");
        strcpy(g_pub_tx.reply,"D346");
        goto ErrExit;
    }
    else if(iRet)
    {
        vtcp_log("查询理财产品的分段利率信息错误");
        strcpy(g_pub_tx.reply,"D347");
        goto ErrExit;
    }
    *fct_rate = sRedempt_rate.redempt_rate;
    *profit_rate = sRedempt_rate.profit_rate;
    vtcp_log("取出的实际利率[%lf] 分润利率[%lf]",*fct_rate,*profit_rate);
OkExit:
    vtcp_log("取分段利率成功");
    strcpy(g_pub_tx.reply,"0000");
    return 0;
ErrExit:
    vtcp_log("取分段利率失败");
    return 1;
}
/*更新机构配额*/
int upd_br_quota(struct fn_parm_c sfn_parm,double tx_amt,int mode)
{
    int iRet  = 0;
    struct fn_br_quota_parm_c sFn_br_qp;
    memset(&sFn_br_qp        ,0x00,sizeof(sFn_br_qp));
    if(sfn_parm.quota_flag[0] == 'Y')
    {
        /*检查本机构理财产品是否能认购*/
        iRet = Fn_br_quota_parm_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s'",\
            sfn_parm.prdt_code,g_pub_tx.tx_br_no);
        if(iRet)
        {
            vtcp_log("查询理财产品配额参数表失败iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        iRet = Fn_br_quota_parm_Fet_Upd(&sFn_br_qp,g_pub_tx.reply);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("查询理财产品配额参数表失败iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            vtcp_info("没有限额不更新");
        }
        else
        {
            if(mode == 0)
            {
                sFn_br_qp.available_amt -= tx_amt;
            }
            else if(mode == 1)
            {
                sFn_br_qp.available_amt += tx_amt;
            }
            else
            {
                strcpy(g_pub_tx.reply,"S035");
                goto ErrExit;
            }
            iRet = Fn_br_quota_parm_Upd_Upd(sFn_br_qp,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("更新理财产品配额参数表失败iRet[%d]",iRet);
                strcpy(g_pub_tx.reply,"D348");
                goto ErrExit;
            }
        }
        Fn_br_quota_parm_Clo_Upd();
    }
    else/*没有配额不做*/
    {
        
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("更新配额成功 ");
    return 0;
ErrExit:
    vtcp_log("更新配额失败");
    return 1;
}
