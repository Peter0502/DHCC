/*************************************************************
* 文 件 名: gD157.c
* 功能描述：遂心存日终结息入账处理 达到配置天数后执行
*
* 作    者: zqbo
* 完成日期: 2014年11月21日
*
* 修改记录:
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "mo_sxc_c.h"
#include "mo_sxc_hst_c.h"
#include "com_item_c.h"

extern long pub_base_daynumSUB(long date, int day);
#define COUNT_COMMIT 10000

double dJz_amt = 0.00;

gD157()
{
    int  iRet     = 0;
    int  iDays    = 0;
    int  iCount   = 0;
    long lDate    = 0;
    long lAc_id   = 0;
    long lAc_seqn = 1;
    double dIntst_hst = 0.00;    /**每笔结出利息**/
    double dSum_intst = 0.00;
    
    struct mo_sxc_hst_c sMo_sxc_hst;
    
    memset(&sMo_sxc_hst,  0x00, sizeof(sMo_sxc_hst));
    
    nOpenBatchLog();
    /* 打开事务 */
    iRet = sql_begin();    
    if ( iRet )
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    strcpy(g_pub_tx.tx_code, "Z157");
    strcpy(g_pub_tx.sub_tx_code, "Z157");
    /**取流水号**/
    iRet = pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_PubQlsh取流水号出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    set_zd_long(DC_TRACE_NO, g_pub_tx.trace_no);
    
    /**取com_parm表里配置的参数阀值**/
    iRet = pub_base_GetParm_double("SXC", 4, &dJz_amt);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_GetParm_double4取最小入账利息参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    iRet = pub_base_GetParm_int("SXC", 5, &iDays);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_GetParm_double5取周期结息相隔天数参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }
    /***推算 iDays天前的日期***/
    lDate = pub_base_daynumSUB(g_pub_tx.tx_date, iDays);
    vtcp_log("iDays=[%d],lDate==[%ld]", iDays, lDate);

    iRet = Mo_sxc_hst_Dec_Sel(g_pub_tx.reply, "sts='1' and ic_date<=%ld order by ac_id", lDate); /**order by不能用for update**/
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
        memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
        
        iRet = Mo_sxc_hst_Fet_Sel(&sMo_sxc_hst, g_pub_tx.reply);
        if(iRet == 100)
        {
            if(lAc_id != 0)
            {
                /**---------退出循环时对最后一个账户的累加利息入账  并修改mo_sxc登记簿-------**/
                iRet = iAcct_sxc_sum_intst(lAc_id, lAc_seqn, dSum_intst);
                if(iRet)
                {
                    sprintf( acErrMsg, "iAcct_sxc_sum_intst出错!!!" );
                    WRITEMSG
                    Mo_sxc_hst_Clo_Sel();
                    goto ErrExit;
                }
            }
            break;
        }
        else if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mo_sxc_hst_Fet_Sel!!!" );
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("ac_id[%ld]", lAc_id);
        if(sMo_sxc_hst.ac_id != lAc_id && lAc_id != 0) /**-变换账户**/  
        {
            /**------对前一个账户累加的利息入账 并修改mo_sxc登记簿----------**/
            iRet = iAcct_sxc_sum_intst(lAc_id, lAc_seqn, dSum_intst);
            if(iRet)
            {
                sprintf( acErrMsg, "iAcct_sxc_sum_intst出错!!!" );
                WRITEMSG
                Mo_sxc_hst_Clo_Sel();
                goto ErrExit;
            }
            
            dSum_intst = 0.00; /**累加下一个账户时重新赋值**/
            
            
            if(iCount >= COUNT_COMMIT)
            {
                vtcp_log("开始分步提交iCount[%d]==================", iCount);
                sql_commit();
                iRet = sql_begin();    
                if ( iRet )
                {
                    sprintf( acErrMsg, "打开事务失败!!!" );
                    WRITEMSG
                    Mo_sxc_hst_Clo_Sel();
                    goto ErrExit;
                }
                /**重新取流水号**/
                iRet = pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply);
                if(iRet)
                {
                    sprintf( acErrMsg, "pub_base_PubQlsh取流水号出错!!!" );
                    WRITEMSG
                    Mo_sxc_hst_Clo_Sel();
                    goto ErrExit;
                }
                set_zd_long(DC_TRACE_NO, g_pub_tx.trace_no);
                iCount = 0;
            }
        }
        
        /****--------计算释放金额的利息 并登记结息明细-------***/
        iRet = iSxc_hst_intst(sMo_sxc_hst, sMo_sxc_hst.hst_bal, &dIntst_hst, '3');
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf(acErrMsg, "利息支出科目记账错误!!");
            WRITEMSG
            Mo_sxc_hst_Clo_Sel();
            goto ErrExit;
        }
        
        /****--------------更新登记明细-------------***/
        sMo_sxc_hst.intst += dIntst_hst;
        iRet = sql_execute("update mo_sxc_hst set intst=%lf,ic_date=%ld where ac_id=%ld and ac_seqn=%d and rate_date=%ld and sts='1'", \
                                       sMo_sxc_hst.intst, g_pub_tx.tx_date, sMo_sxc_hst.ac_id, sMo_sxc_hst.ac_seqn, sMo_sxc_hst.rate_date);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "sql_execute(update mo_sxc_hst...)出错!!!" );
            WRITEMSG
            Mo_sxc_hst_Clo_Sel();
            goto ErrExit;
        }
        
        dSum_intst += dIntst_hst;     /***累加本次ac_id结出的利息**/
        
        lAc_id   = sMo_sxc_hst.ac_id; /***记录本次ac_id**/
        lAc_seqn = sMo_sxc_hst.ac_seqn;
        
        iCount++;
    }
    Mo_sxc_hst_Clo_Sel();
    

OkExit:
    /*--提交事务--*/
    sql_commit();        
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "遂心存日终理财处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    
    /*--事务回滚--*/
    sql_rollback();
    if (strcmp(g_pub_tx.reply, "0000") == 0 || strlen(g_pub_tx.reply) == 0)
    {
        strcpy(g_pub_tx.reply, "G009");
    }
    sprintf(acErrMsg, "遂心存日终理财处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}

int iAcct_sxc_sum_intst(long lAc_id, long lAc_seqn, double dSum_intst)
{
    int iRet = 0;

    struct mo_sxc_c sMo_sxc;
    
    memset(&sMo_sxc,      0x00, sizeof(sMo_sxc));
    memset(&g_dd_mst,     0x00, sizeof(g_dd_mst));
    memset(&g_dd_parm,    0x00, sizeof(g_dd_parm));
    memset(&g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel));
    
    /***查找活期账户**/
    iRet = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%ld", lAc_id, lAc_seqn);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Dd_mst_Sel出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    iRet = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Dd_parm_Sel出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    /***根据ac_id找到一个状态为正常/挂失状态的账号***/
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_id=%ld and note_sts in('0','1') and mdm_code<>'1010'", g_dd_mst.ac_id);
    if(iRet)
    {
    	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_id=%ld and note_sts in('0','1') and mdm_code='1010'", g_dd_mst.ac_id);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mdm_ac_rel_Sel出错!!!" );
            WRITEMSG
            goto ErrExit;
         }
    }
    
    if(pub_base_CompDblVal(dSum_intst, 0.00) > 0)
    {
    	/**找到随心存登记簿里的未入账金额**/
    	iRet = Mo_sxc_Sel(g_pub_tx.reply, &sMo_sxc, "ac_id=%ld and ac_seqn=%d and sts='1'", lAc_id, lAc_seqn);
    	if(iRet != 0 && iRet != 100)
    	{
    	    strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mo_sxc_Sel出错!!!" );
            WRITEMSG
            goto ErrExit;
    	}
    	
    	/*****-------对释放结出总利息入账---------****/
    	if(pub_base_CompDblVal(dSum_intst+sMo_sxc.un_intst, dJz_amt) >= 0) /**只有达到1元（配置）才入客户帐**/
    	{
    	    
            iRet = iAcct_sxc_intst(lAc_id, lAc_seqn, dSum_intst+sMo_sxc.un_intst);
            if(iRet)
            {
                sprintf( acErrMsg, "iAcct_intst出错!!!" );
                WRITEMSG
                goto ErrExit;
            }
            
            sMo_sxc.un_intst = 0.00;         /**入账后未入账利息清空**/
            sMo_sxc.flag[0] = '1';           /**1-发生业务**/
        }
        else
        {
             sMo_sxc.un_intst += dSum_intst;  /**累加未入账利息**/
        }
        
        /**--------更新登记簿的结出利息----------**/
        sMo_sxc.intst += dSum_intst;      /**累加结出利息**/
        iRet = sql_execute("update mo_sxc set intst=%lf, un_intst=%lf, flag='%s' where ac_id=%ld and ac_seqn=%d and sts='1'", \
                                                     sMo_sxc.intst, sMo_sxc.un_intst, sMo_sxc.flag, lAc_id, lAc_seqn);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "sql_execute(update mo_sxc...)出错!!!" );
            WRITEMSG
            goto ErrExit;
        }
    }
    
OkExit:
    sprintf(acErrMsg, "记账处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
    
    sprintf(acErrMsg, "记账处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}

int iAcct_sxc_intst(long ac_id, long ac_seqn, double intst)
{
    int iRet = 0;
    
    struct com_item_c sCom_item;
    memset(&sCom_item,    0x00, sizeof(sCom_item));
    
    /***贷 储蓄活期户 ***/
    strcpy(g_pub_tx.ac_no, g_mdm_ac_rel.ac_no);
    vtcp_log("KKKKK,g_pub_tx.ac_no[%s]-----------------", g_pub_tx.ac_no);
    g_pub_tx.ac_id_type[0] = '1';
    strcpy(g_pub_tx.tx_br_no, g_dd_mst.opn_br_no);
    strcpy(g_pub_tx.opn_br_no, g_dd_mst.opn_br_no);
    g_pub_tx.ac_id = g_dd_mst.ac_id;
    g_pub_tx.ac_seqn = g_dd_mst.ac_seqn;
    strcpy(g_pub_tx.cur_no, g_dd_parm.cur_no);
    
    strcpy(g_pub_tx.add_ind, "1");
    g_pub_tx.tx_amt1 = intst;
    strcpy(g_pub_tx.ct_ind, "2");
    strcpy(g_pub_tx.prt_ind, "0");
    strcpy(g_pub_tx.hst_ind, "1");
    strcpy(g_pub_tx.ac_wrk_ind, "0000000J1");
    g_pub_tx.svc_ind = 1005;
    strcpy(g_pub_tx.brf, "遂心存周期结息");
    
    iRet = pub_acct_trance();
    if(iRet)
    {
        sprintf( acErrMsg, "pub_acct_trance_jx出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    set_zd_data("101A", g_pub_tx.cur_no);
    set_zd_data("101B", g_pub_tx.ct_ind);
    set_zd_double("1013", g_pub_tx.tx_amt1);
    iRet = pubf_acct_proc("D099");
    if(iRet)
    {
        sprintf( acErrMsg, "pubf_acct_proc出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    /***借活期利息支出***/
    get_zd_data("0153", g_pub_tx.ac_no);
    vtcp_log("KKKKK,g_pub_tx.ac_no[%s]-----------------", g_pub_tx.ac_no);
    iRet=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt='%s'",g_pub_tx.ac_no);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf(acErrMsg, "Com_item_Sel利息支出科目错误!!");
        WRITEMSG
        goto ErrExit;
    }
    memset(g_pub_tx.ac_no, 0x00,sizeof(g_pub_tx.ac_no));
    strcpy(g_pub_tx.ac_no, sCom_item.acc_no);
    strcpy(g_pub_tx.tx_br_no, g_dd_mst.opn_br_no);
    strcpy(g_pub_tx.opn_br_no, g_dd_mst.opn_br_no);
    g_pub_tx.ac_id_type[0] = '9';
    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.ac_id_type, "9");
    strcpy(g_pub_tx.add_ind, "0");    /**借方 **/
    g_pub_tx.tx_amt1 = intst;
    strcpy(g_pub_tx.hst_ind, "1");
    strcpy(g_pub_tx.ac_wrk_ind, "0000000J1");
    g_pub_tx.svc_ind = 9001;

    iRet = pub_acct_trance();
    if (iRet)
    {
        sprintf(acErrMsg, "利息支出科目记账错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    set_zd_data("1204", g_pub_tx.cur_no);
    set_zd_data("1205", g_pub_tx.ct_ind);
    set_zd_double("1208", g_pub_tx.tx_amt1);
    iRet = pubf_acct_proc("A016");
    if (iRet != 0)
    {
        sprintf(acErrMsg, "pubf_acct_proc会计记帐不成功!!");
        WRITEMSG
        goto ErrExit;
    }
    
    
OkExit:
    sprintf(acErrMsg, "记账处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
    
    sprintf(acErrMsg, "记账处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}