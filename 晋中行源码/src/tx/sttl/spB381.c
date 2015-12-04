/**************************************************************
* 文 件 名:  spB381.c
* 功能描述： 承兑保证金到期日联机取款功能,配合spB380一起使用
*
* 作    者:  xyy
* 完成日期： 2009-9-20 15:17:52
*
***************************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "com_parm_c.h"
#include "mo_bail_rate_c.h"
#include "protect_note_c.h"
#include "dd_parm_c.h"
#include "dc_acc_rel_c.h"
int spB381()
{
    int     ret;
    long    day = 0;
    long     HQday = 0;
    
    char    cAcc_hrt[6];
    char    cPact_no[21];
    char    cIntst_type[2];
    char    cBail_ac_no[25];
    char    cIntst_ac_no[25];
    
    double  HQ_rate   = 0.00;
    double  HQdealval = 0.00;
    double  dTempAmt  = 0.00;   /* 退票时应该从保证金户扣划到*/
    double  dYfintst  = 0.00;   /* 应付利息 */
    double  dTotintst = 0.00;   /* 利息合计 */
    
    struct mo_bail_rate_c   sMoBailRate;     /* 保证金利率登记薄 */
    struct mo_bail_rate_c   sMoBailRate1;    /* 保证金利率登记薄 */
    struct mo_bank_acc_po_c sMoBankAccPo;  /* 承兑汇票基本信息登记簿 */
    
    /* 数据初始化 */
    
    memset(cAcc_hrt    , 0 , sizeof(cAcc_hrt));
    memset(cPact_no    , 0 , sizeof(cPact_no));
    memset(cBail_ac_no , 0 , sizeof(cBail_ac_no));
    memset(cIntst_type , 0 , sizeof(cIntst_type));
    memset(cIntst_ac_no, 0 , sizeof(cIntst_ac_no));
    
    memset(&g_pub_intst , 0 , sizeof(struct S_pub_intst));
    memset(&sMoBailRate , 0 , sizeof(struct mo_bail_rate_c));
    memset(&sMoBailRate1, 0 , sizeof(struct mo_bail_rate_c));
    memset(&sMoBankAccPo, 0 , sizeof(struct mo_bank_acc_po_c));
    
    pub_base_sysinit();

    /* 从前台屏幕取值 */
    get_zd_data("0330", cPact_no);      /* 承兑协议编号 */
    get_zd_data("0320", cBail_ac_no);   /* 保证金帐号   */
    get_zd_data("0310", cIntst_ac_no);  /* 收息帐号     */
    get_zd_data("0670", cIntst_type);   /* 计息标志     */
    get_zd_double("0390", &dTempAmt);   /* 取款金额     */
    get_zd_double("1001", &dYfintst);   /* 应付利息     */

    /** 旧帐号 **/
    pub_base_old_acno(cBail_ac_no);
    pub_base_old_acno(cIntst_ac_no);

    vtcp_log("[%s][%d] 收息帐号=[%s]",__FILE__,__LINE__,cIntst_ac_no);
    vtcp_log("[%s][%d] dTempAmt=[%f]",__FILE__,__LINE__,dTempAmt);
    vtcp_log("[%s][%d] dYfintst=[%f]",__FILE__,__LINE__,dYfintst);
 
    /* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
    ret = Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply, "pact_no='%s'", cPact_no);
    if (ret)
    {
        sprintf(acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Mo_bank_acc_po_Fet_Upd(&sMoBankAccPo, g_pub_tx.reply);
    if (ret) 
    {
        sprintf(acErrMsg, "执行Mo_bank_acc_po_Fet_Upd错![%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B199");
        goto ErrExit;
    }
    Mo_bank_acc_po_Debug(&sMoBankAccPo);
    
    pub_base_strpack(sMoBankAccPo.bail_ac_no);
    pub_base_strpack(cBail_ac_no);

    if(strcmp(sMoBankAccPo.bail_ac_no,cBail_ac_no) ) 
    { 
        sprintf( acErrMsg, "协议编号对应的保证金帐号与录入保证金帐号不一致!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B329" ); 
        goto ErrExit; 
    }
    /* 修改承兑汇票基本信息登记簿 */
    vtcp_log("[%s][%d] sMoBankAccPo.bail_amt =[%f]\n",__FILE__,__LINE__,sMoBankAccPo.bail_amt);
    /****
    if (pub_base_CompDblVal(dTempAmt, sMoBankAccPo.bail_amt) != 0)
    {
        sprintf( acErrMsg, "保证金必须按合同一次取完!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B342" ); 
        goto ErrExit; 
    }
    ****/
    sMoBankAccPo.bail_amt = sMoBankAccPo.bail_amt - dTempAmt;
    
    if (pub_base_CompDblVal(sMoBankAccPo.bail_amt, 0.00) < 0)
    {
        sprintf( acErrMsg, "保证金取款超额!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B342" ); 
        goto ErrExit; 
    }
    /***到期标志置为已到期处理***/
    strcpy(sMoBankAccPo.acc_po_ind, "1");   /* 承兑协议到期处理标志: * 1 已到期处理 */

    /**这种联机扣款的还需要判断状态吗?
    if( sMoBankAccPo.acc_po_sts[0]!='3' && sMoBankAccPo.acc_po_sts[0]!='A' && sMoBankAccPo.req_date>20090516) 
    {
        sprintf( acErrMsg, "保证金状态非退票状态不能取款!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B302" ); 
        goto ErrExit; 
    }
    **/
    ret = Mo_bank_acc_po_Upd_Upd(sMoBankAccPo, g_pub_tx.reply);
    if (ret) 
    {
        sprintf(acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_bank_acc_po_Clo_Upd();
        
    /** 撤销必须要有协议编号 **/
    strcpy(g_pub_tx.ac_no, cPact_no);
    g_pub_tx.tx_amt1 = dTempAmt;
    strcpy(g_pub_tx.beg_note_no, cBail_ac_no);
    strcpy(g_pub_tx.brf, "保证金联机取款");
    g_pub_tx.svc_ind = 0;
    strcpy(g_pub_tx.note_type, cIntst_type);    /** 撤销用是否计息了 **/
    strcpy(g_pub_tx.no_show, "1");
    ret = pub_ins_trace_log();
    if (ret) 
    {
        sprintf(acErrMsg, "记流水日志错");
        WRITEMSG
        goto ErrExit;
    }
    strcpy(g_pub_tx.no_show, "0");

    if (cIntst_type[0] == 'Y')
    {
        ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply, " bail_type='1' and pact_no='%s' and sts='1' ",cPact_no);
        if (ret) {
            sprintf(acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d][%s]", ret, cPact_no);
            WRITEMSG
            sprintf(g_pub_tx.reply, "B334");
            goto ErrExit;
        }
        ret = Mo_bail_rate_Fet_Upd(&sMoBailRate, g_pub_tx.reply);
        if (ret) {
            sprintf(acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]", ret, cPact_no);
            WRITEMSG
            sprintf(g_pub_tx.reply, "B334");
            goto ErrExit;
        }
        memcpy(&sMoBailRate1, &sMoBailRate, sizeof(struct mo_bail_rate_c));

        pub_base_strpack(sMoBailRate.ac_no);
        pub_base_strpack(cIntst_ac_no);
        if (strcmp(sMoBailRate.ac_no, cIntst_ac_no)) {
            sprintf(acErrMsg, "输入的收息帐号与输入的不一致![%s][%s]", sMoBailRate.ac_no, cIntst_ac_no);
            WRITEMSG
            sprintf(g_pub_tx.reply, "B343");
            goto ErrExit;
        }
        ret = pub_base_getllz(HQLLBH, "01", g_pub_tx.tx_date, &HQ_rate);
        if (ret) {
            sprintf(acErrMsg, "取利率错误!");
            WRITEMSG
            sprintf(g_pub_tx.reply, "O195");
            goto ErrExit;
        }
        /** 判断是否到期 **/
        if (g_pub_tx.tx_date < sMoBailRate.end_date)  /** 提前支取 **/
        {
            /** 计算天数 **/
            if (pub_base_CalDays(sMoBailRate.beg_date, g_pub_tx.tx_date, "0", &HQday)) {
                sprintf(acErrMsg, "计算天数错误!");
                WRITEMSG
                sprintf(g_pub_tx.reply, "O108");
                goto ErrExit;
            }
            vtcp_log("[%s][%d] HQday =[%ld]",__FILE__,__LINE__,HQday);
            day = 0;
        } 
        else 
        {
            /** 计算天数 **/
            if (pub_base_CalDays(sMoBailRate.beg_date, sMoBailRate.end_date, "0", &day)) {
                sprintf(acErrMsg, "计算天数错误!");
                WRITEMSG
                sprintf(g_pub_tx.reply, "O108");
                goto ErrExit;
            }
            if (pub_base_CalDays(sMoBailRate.end_date, g_pub_tx.tx_date, "0", &HQday)) {
                sprintf(acErrMsg, "计算天数错误!");
                WRITEMSG
                sprintf(g_pub_tx.reply, "O108");
                goto ErrExit;
            }
            vtcp_log("[%s][%d] HQday =[%ld]",__FILE__,__LINE__,HQday);
            vtcp_log("[%s][%d] day =[%ld]",__FILE__,__LINE__,day);
        }
        sprintf(acErrMsg, "HQday=[%ld]HQ_rate=[%lf],day=[%ld],ic_rate=[%lf]", HQday, HQ_rate,day,sMoBailRate.ic_rate);
        WRITEMSG
        vtcp_log("[%s][%d]sMoBailRate.amt=[%f] g_pub_intst.dealval=[%f]",__FILE__,__LINE__,sMoBailRate.amt,g_pub_intst.dealval);
        vtcp_log("[%s][%d] sMoBailRate.amt=[%f]",__FILE__,__LINE__,sMoBailRate.amt);
        /** 计算利息 **/
        if (pub_base_CompDblVal(dTempAmt, sMoBailRate.amt) > 0) {
            /***HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; del by chenchao 晋中不计算活期利息了 2011-8-9 16:31:38 **/
            vtcp_log("[%s][%d] HQdealval=[%f]",__FILE__,__LINE__,HQdealval);
            g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
            vtcp_log("[%s][%d] g_pub_intst.dealval=[%f]",__FILE__,__LINE__,g_pub_intst.dealval);
            g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
            vtcp_log("[%s][%d] g_pub_intst.dealval=[%f]",__FILE__,__LINE__,g_pub_intst.dealval);
            sMoBailRate1.amt = 0.00;
        } 
        else 
        {
            /***HQdealval = HQ_rate * dTempAmt * HQday / L360 / L100;**modify by chenchao 2011-8-9 16:30:50 只是计算输入的利率利息 ***/
            vtcp_log("[%s][%d] HQdealval=[%f]",__FILE__,__LINE__,HQdealval);
            g_pub_intst.dealval = sMoBailRate.ic_rate * dTempAmt * day / L360 / L100;
            vtcp_log("[%s][%d] g_pub_intst.dealval=[%f]",__FILE__,__LINE__,g_pub_intst.dealval);
            g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
            vtcp_log("[%s][%d] g_pub_intst.dealval=[%f]",__FILE__,__LINE__,g_pub_intst.dealval);
            sMoBailRate1.amt = sMoBailRate.amt - dTempAmt;
            sMoBailRate.amt = dTempAmt;
        }
        g_pub_intst.dealval = pub_base_PubDround_1(g_pub_intst.dealval);
        if (pub_base_CompDblVal(g_pub_intst.dealval,0.00) < 0)
        {
            sprintf(acErrMsg, "计算利息错误错误!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "D158");
            goto ErrExit;
        }
        /** 更新保证金计息金额 **/
        sMoBailRate.intst_date = g_pub_tx.tx_date;
        sMoBailRate.end_trace_no = g_pub_tx.trace_no;
        sMoBailRate.intst_amt = g_pub_intst.dealval;
        
        /**更新保证金状态**/
        strcpy(sMoBailRate.sts, "*");
        ret = Mo_bail_rate_Upd_Upd(sMoBailRate, g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg, "执行Mo_bail_rate_Upd_Upd错![%d][%s]", ret, cPact_no);
            WRITEMSG
            sprintf(g_pub_tx.reply, "B334");
            goto ErrExit;
        }
        Mo_bail_rate_Clo_Upd();
        dTotintst = g_pub_intst.dealval;
/*** 
        if(pub_base_CompDblVal(dYfintst,dTotintst) != 0 )
        {
            sprintf(acErrMsg, "传入的应付利息错啦!![%f][%f]!",dYfintst,dTotintst);
            WRITEMSG
            sprintf(g_pub_tx.reply, "A032");
            goto ErrExit;
        }
*****/

        set_zd_double("0410",dTotintst);
        vtcp_log("[%s][%d] 该保证金总共产生了[%f]利息",__FILE__,__LINE__,dTotintst);
        
        /**全部利息转给原收息帐户**/
        
        
        /** 多余资金插入新的保证金利息登记薄 **/
        if (pub_base_CompDblVal(sMoBailRate1.amt, 0.00) > 0) 
        {
            int iSeq=0;
            iSeq = sql_count("mo_bail_rate", " pact_no='%s' ", sMoBailRate1.pact_no);
            sMoBailRate1.seq = iSeq + 1;
            
            ret = Mo_bail_rate_Ins(sMoBailRate1, g_pub_tx.reply);
            if (ret) 
            {
                sprintf(acErrMsg, "执行Mo_bail_rate_Ins错![%d]", ret);
                WRITEMSG
                sprintf(g_pub_tx.reply, "B334");
                goto ErrExit;
            }
        }
        g_pub_intst.tax_intst = g_pub_intst.dealval;  /** 应税利息 **/
        g_pub_intst.factval = g_pub_intst.dealval;    /** 实付利息 **/
        g_pub_intst.val = 0.00;                       /** 利息税   **/
        g_pub_intst.keepval = 0.00;                   /** 保值利息 **/

        /**----------------检查保证金利息需要从哪个科目取值---------------------------**/
        vtcp_log("[%s][%d] cBail_ac_no =[%s]",__FILE__,__LINE__,cBail_ac_no);
        
        ret = iGetAccHrt(cBail_ac_no , cAcc_hrt);
        if (ret) 
        {
            sprintf(acErrMsg, "查找帐户[%s]的付息科目错误![%s]", cBail_ac_no,cAcc_hrt);
            WRITEMSG
            goto ErrExit;
        }
        
        vtcp_log("[%s][%d]cAcc_hrt =[%s]\n",__FILE__,__LINE__,cAcc_hrt);
        set_zd_data("0153", cAcc_hrt);       /** 保证金利息 **/
        set_zd_double("102P", g_pub_intst.dealval);
    }
    else
    {
        g_pub_intst.dealval   = 0.00;
        g_pub_intst.tax_intst = 0.00;      /** 应税利息 **/
        g_pub_intst.factval   = 0.00;      /** 实付利息 **/
        g_pub_intst.val       = 0.00;      /** 利息税   **/
        g_pub_intst.keepval   = 0.00;      /** 保值利息 **/
        
        vtcp_log("[%s][%d] cBail_ac_no =[%s]",__FILE__,__LINE__,cBail_ac_no);
        
        ret = iGetAccHrt(cBail_ac_no , cAcc_hrt);
        if (ret) 
        {
            sprintf(acErrMsg, "查找帐户[%s]的付息科目错误![%s]", cBail_ac_no,cAcc_hrt);
            WRITEMSG
            goto ErrExit;
        }
        set_zd_data("0153", cAcc_hrt);    /** 保证金利息 **/
        set_zd_double("102P", g_pub_intst.dealval);
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
int iGetAccHrt(char *ac_no ,char *acc_hrt)
{
    int  ret=0;
    char   cTmpAcno[25];
    
    struct dd_mst_c         sDd_mst;
    struct dd_parm_c        sDd_parm;
    struct dc_acc_rel_c     sDc_acc_rel;
    struct mdm_ac_rel_c     sMdm_ac_rel;
    
    memset(cTmpAcno     , 0 , sizeof(cTmpAcno));
    memset(&sDd_mst     , 0 , sizeof(struct dd_mst_c));
    memset(&sDd_parm    , 0 , sizeof(struct dd_parm_c));
    memset(&sDc_acc_rel , 0 , sizeof(struct dc_acc_rel_c));
    memset(&sMdm_ac_rel , 0 , sizeof(struct mdm_ac_rel_c));
    
    memcpy(cTmpAcno , ac_no , sizeof(cTmpAcno)-1);
    zip_space(cTmpAcno);
    pub_base_old_acno(cTmpAcno);
    vtcp_log("[%s][%d] 传入帐户的新账号为:[%s]",__FILE__,__LINE__,cTmpAcno);
    
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cTmpAcno);
    if( ret )
    {
        sprintf(acErrMsg,"查询介质表错误!!ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "O074");
        return ret;
    }
    vtcp_log("[%s][%d]ac_id =[%ld]  ac_seqn=[%ld]\n",__FILE__,__LINE__,sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
    
    memset(&sDd_mst, 0 , sizeof(sDd_mst));
    ret = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id='%ld' and ac_seqn='%ld' ", sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
    if (ret) 
    {
        sprintf(acErrMsg, "执行Dc_acc_rel_Sel错![%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "O074");
        return ret;
    }
    vtcp_log("[%s][%d]prdt_no=[%s]\n",__FILE__,__LINE__,sDd_mst.prdt_no);
    memset(&sDd_parm , 0 , sizeof(struct dd_parm_c));
    ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s' ",sDd_mst.prdt_no);
    if( ret )
    {
        sprintf(acErrMsg,"查询活期参数表异常!!ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "O074");
        return ret;
    }
    vtcp_log("[%s][%d]dc_code =[%s] \n",__FILE__,__LINE__,sDd_parm.dc_code);
    
    memset(&sDc_acc_rel, 0 , sizeof(sDc_acc_rel));
    ret = Dc_acc_rel_Sel(g_pub_tx.reply, &sDc_acc_rel, "dc_code='%s' and data_code='0153'", sDd_parm.dc_code);
    if (ret) 
    {
        sprintf(acErrMsg, "执行Dc_acc_rel_Sel错![%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "O074");
        return ret;
    }
    vtcp_log("[%s][%d] acc_hrt =[%s]",__FILE__,__LINE__,sDc_acc_rel.acc_hrt);
    memcpy(acc_hrt , sDc_acc_rel.acc_hrt, sizeof(sDc_acc_rel.acc_hrt)-1);
    return 0;
}
