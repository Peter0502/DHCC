/*************************************************************
* 文 件 名: gD156.c
* 功能描述：遂心存日终处理
*
* 作    者: zqbo
* 完成日期: 2014年11月19日
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
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "mo_sxc_c.h"
#include "mo_sxc_hst_c.h"

extern double pub_base_PubDround_1(double amt);
#define COUNT_COMMIT 5000   /**5000笔提交一次数据库**/

gD156()
{
    int    iRet = 0;
    int    iCount = 0;
    double dLc_amt = 0.00;   /**com_parm表里配置的最小留存金额**/
    double dSx_amt = 0.00;   /**com_parm表里配置的生效触发阀值**/
    double dBs_amt = 0.00;   /**com_parm表里配置的登记/释放单位倍数**/
    double dJz_amt = 0.00;   /**com_parm表里配置的最小入账金额**/
    double dGc_amt = 0.00;       /**轧差金额**/
    double dGc_down_amt = 0.00;  /**每次存入金额 向下取整**/
    double dGc_up_amt = 0.00;    /**每次释放金额 向上取整**/
    double dIntst = 0.00;        /**释放结出利息**/
    
    struct mo_sxc_c        sMo_sxc;
    struct mo_sxc_hst_c    sMo_sxc_hst;
    
    memset(&sMo_sxc,       0x00, sizeof(sMo_sxc));
    memset(&sMo_sxc_hst,   0x00, sizeof(sMo_sxc_hst));
    
    
    nOpenBatchLog();
    
    /* 打开事务 */
    iRet = sql_begin();    
    if ( iRet )
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    /**-------初始化全局变量--------**/
    pub_base_sysinit();
    strcpy(g_pub_tx.tx_code, "Z156");
    strcpy(g_pub_tx.sub_tx_code, "Z156");
    
    /**取流水号**/
    iRet = pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_PubQlsh取流水号出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    set_zd_long(DC_TRACE_NO, g_pub_tx.trace_no);
    
    /**取com_parm表里配置的参数**/
    iRet = pub_base_GetParm_double("SXC", 1, &dLc_amt);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_GetParm_double1取参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }
    iRet = pub_base_GetParm_double("SXC", 2, &dSx_amt);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_GetParm_double2取参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }
    iRet = pub_base_GetParm_double("SXC", 3, &dBs_amt);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_GetParm_double3取参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }
    iRet = pub_base_GetParm_double("SXC", 4, &dJz_amt);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_GetParm_double4取参数配置表错误!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    /***查找遂心存登记簿**/
    iRet = Mo_sxc_Dec_Sel(g_pub_tx.reply, "sts='1' and flag='1' ");
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D101");
        sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    while(1)
    {
        /**每次循环初始化**/
        dGc_amt      = 0.00;
        dGc_down_amt = 0.00;
        dGc_up_amt   = 0.00;
        dIntst = 0.00;
        /***
        memset(&sMo_sxc,     0x00, sizeof(sMo_sxc));
        ***/
        memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
        memset(&g_dd_mst,  0x00, sizeof(g_dd_mst));
        
        iRet = Mo_sxc_Fet_Sel(&sMo_sxc, g_pub_tx.reply);
        if(iRet == 100)
        {
            break;
        }
        else if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!![%d]", iRet );
            WRITEMSG
            goto ErrExit;
        }
        /***查找遂心存的活期账户**/
        iRet = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%d", sMo_sxc.ac_id, sMo_sxc.ac_seqn);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Dd_mst_Sel出错!!![%d]", iRet );
            WRITEMSG
            Mo_sxc_Clo_Sel();
            goto ErrExit;
        }
        
        dGc_amt = g_dd_mst.bal - sMo_sxc.bal - dLc_amt; /**计算扎差金额=活期余额-遂心存余额-留存金额 目前账户留存金额为0**/
        iRet = pub_base_CompDblVal(dGc_amt, 0.00);
        if(iRet < 0) /***轧差金额为负、释放***/
        {
            dGc_up_amt = dBs_amt * ( -1*(int)(dGc_amt/dBs_amt) + 1); /***计算释放金额=dBs_amt的整数倍（向上取整）***/
            if(pub_base_CompDblVal(dGc_amt-(int)(dGc_amt/dBs_amt)*dBs_amt, 0.00) == 0)
            {
                dGc_up_amt -= dBs_amt;   /**如果为整数倍，上面计算时多加了1，在此减去**/
            }
            
            vtcp_log("dGc_up_amt需要释放金额为[%lf]", dGc_up_amt);
            if(pub_base_CompDblVal(dGc_up_amt,0.00) > 0 )
            {
                /***----------释放算息并登记遂心存结息明细--------***/
                iRet = iSxc_intst(sMo_sxc.ac_id, sMo_sxc.ac_seqn, dGc_up_amt, &dIntst, '2');
                if(iRet)
                {
                    sprintf( acErrMsg, "iSxc_intst出错!!!" );
                    WRITEMSG
                    Mo_sxc_Clo_Sel();
                    goto ErrExit;
                }
                
                /*****----------结出利息入客户账----------****/
                if(pub_base_CompDblVal(dIntst+sMo_sxc.un_intst, dJz_amt) >= 0)  
                {
                    /**只有达到1元（配置）才入客户帐**/
                    iRet = iAcct_sxc_intst(sMo_sxc.ac_id, sMo_sxc.ac_seqn, dIntst+sMo_sxc.un_intst);
                    if(iRet)
                    {
                        sprintf( acErrMsg, "iAcct_intst出错!!!" );
                        WRITEMSG
                        Mo_sxc_Clo_Sel();
                        goto ErrExit;
                    }
                    sMo_sxc.un_intst = 0.00;  /**入账后未入账利息清空**/
                    sMo_sxc.flag[0] = '1';    /**1-发生业务**/
                }
                else
                {
                    sMo_sxc.un_intst += dIntst;  /**累加未入账利息**/
                    sMo_sxc.flag[0] = '0';       /**0-未发生业务**/
                }
                
                /**----------------更新登记簿-------------**/
                sMo_sxc.bal -= dGc_up_amt;    /**修改余额**/
                if(pub_base_CompDblVal(sMo_sxc.bal, 0.00) < 0)
                {
                    strcpy(g_pub_tx.reply, "S035");
                    sprintf( acErrMsg, "sMo_sxc.bal[%lf]不允许为负数!dGc_up_amt=[%lf]", sMo_sxc.bal, dGc_up_amt);
                    WRITEMSG
                    Mo_sxc_Clo_Sel();
                    goto ErrExit;
                }
                sMo_sxc.intst += dIntst;      /**累加结出利息**/
                /**sMo_sxc.flag[0] = '0';加上利息后可能满足生效金额、下次需要登记明细，不能修改flag值**/
                
                /**轧差金额为负时，计算出的利息倘若大于“随心存触发生效阀值”，也需要转入随心存
                （由于晋中设定的阀值为0.01，即只要账户中有钱，即转入随心存中，故暂且这样添加）add by lq 20151015**/
                if(pub_base_CompDblVal(dIntst, dSx_amt) > 0)
                	{
                	/*将结出的利息直接转入随心存 add by wp 20151127*/
                				sMo_sxc_hst.ac_id     = sMo_sxc.ac_id;
                        sMo_sxc_hst.ac_seqn   = sMo_sxc.ac_seqn;
                        sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                        sMo_sxc_hst.ic_date   = g_pub_tx.tx_date;
                        sMo_sxc_hst.tx_date   = g_pub_tx.tx_date;
                        sMo_sxc_hst.trace_no  = g_pub_tx.trace_no;
                        sMo_sxc_hst.tx_amt    = dIntst;
                        sMo_sxc_hst.hst_bal   = dIntst;
                        strcpy(sMo_sxc_hst.tx_br_no,  g_pub_tx.tx_br_no);
                        strcpy(sMo_sxc_hst.opn_br_no, sMo_sxc.opn_br_no);
                        strcpy(sMo_sxc_hst.tx_tel,    g_pub_tx.tel);
                        sMo_sxc_hst.type[0]   = '1';  /**1-登记**/
                        sMo_sxc_hst.sts[0]    = '1';  /**1-正常**/
                        iRet = Mo_sxc_hst_Ins(sMo_sxc_hst, g_pub_tx.reply);
                        if(iRet)
                        {
                            strcpy(g_pub_tx.reply, "D102");
                            sprintf( acErrMsg, "结出利息转存随心存Mo_sxc_hst_Ins出错!!![%d]", iRet );
                            WRITEMSG
                            goto ErrExit;
                        }
                        sMo_sxc.bal += dIntst;  /**修改登记簿余额**/
                  }
                /**end by lq 20151015**/
                
                iRet = sql_execute("update mo_sxc set bal=%lf, intst=%lf, un_intst=%lf, flag='%s' where ac_id=%ld and ac_seqn=%d and sts='1'", \
                                                  sMo_sxc.bal, sMo_sxc.intst, sMo_sxc.un_intst, sMo_sxc.flag, sMo_sxc.ac_id, sMo_sxc.ac_seqn);
                if(iRet)
                {
                    strcpy(g_pub_tx.reply, "D102");
                    sprintf( acErrMsg, "sql_execute(update...)出错!!iRet[%d]", iRet );
                    WRITEMSG
                    Mo_sxc_Clo_Sel();
                    goto ErrExit;
                }
            }
            else
            {
                strcpy(g_pub_tx.reply, "S035");
                sprintf( acErrMsg, "计算释放金额[%d]出错,必须释放、释放金额必大于0", dGc_up_amt);
                WRITEMSG
                Mo_sxc_Clo_Sel();
                goto ErrExit;	
            }
        }
        else if(iRet > 0) /***轧差金额为正、登记明细***/
        {
            if(pub_base_CompDblVal(g_dd_mst.bal, dSx_amt) >= 0) /****账户余额达到生效阀值，才登记遂心存登记簿***/
            {
                dGc_down_amt = dBs_amt * (int)(dGc_amt/dBs_amt); /**计算登记金额=dBs_amt的整数倍（向下取整)**/
                
                vtcp_log("dGc_down_amt生效金额为[%lf]", dGc_down_amt);
                if(pub_base_CompDblVal(dGc_down_amt,0.00) > 0 )
                {
                    /**---------------登记明细---------------**/
                    iRet = sql_count("mo_sxc_hst", "ac_id=%ld and ac_seqn=%d and rate_date=%ld", sMo_sxc.ac_id, sMo_sxc.ac_seqn, g_pub_tx.tx_date);
                    if(iRet > 0) /**如果存在直接更新,保证同一个rate_date只存在一条记录**/
                    {
                        iRet = sql_execute("update mo_sxc_hst set tx_amt=tx_amt+%lf, hst_bal=hst_bal+%lf, sts='1' where ac_id=%ld and ac_seqn=%d and rate_date=%ld", \
                                                       dGc_down_amt, dGc_down_amt, sMo_sxc.ac_id, sMo_sxc.ac_seqn, g_pub_tx.tx_date);
                        if(iRet)
                        {
                            strcpy(g_pub_tx.reply, "D102");
                            sprintf( acErrMsg, "sql_execute(update mo_sxc_hst...)出错!!iRet[%d]", iRet );
                            WRITEMSG
                            Mo_sxc_Clo_Sel();
                            goto ErrExit;
                        }
                        vtcp_log("竟然走到这里啦!=======");
                    }
                    else /**正常走此分支，直接插入**/
                    {
                        sMo_sxc_hst.ac_id     = sMo_sxc.ac_id;
                        sMo_sxc_hst.ac_seqn   = sMo_sxc.ac_seqn;
                        sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                        sMo_sxc_hst.ic_date   = g_pub_tx.tx_date;
                        sMo_sxc_hst.tx_date   = g_pub_tx.tx_date;
                        sMo_sxc_hst.trace_no  = g_pub_tx.trace_no;
                        sMo_sxc_hst.tx_amt    = dGc_down_amt;
                        sMo_sxc_hst.hst_bal   = dGc_down_amt;
                        strcpy(sMo_sxc_hst.tx_br_no,  g_pub_tx.tx_br_no);
                        strcpy(sMo_sxc_hst.opn_br_no, sMo_sxc.opn_br_no);
                        strcpy(sMo_sxc_hst.tx_tel,    g_pub_tx.tel);
                        sMo_sxc_hst.type[0]   = '1';  /**1-登记**/
                        sMo_sxc_hst.sts[0]    = '1';  /**1-正常**/
                        iRet = Mo_sxc_hst_Ins(sMo_sxc_hst, g_pub_tx.reply);
                        if(iRet)
                        {
                            strcpy(g_pub_tx.reply, "D102");
                            sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!![%d]", iRet );
                            WRITEMSG
                            Mo_sxc_Clo_Sel();
                            goto ErrExit;
                        }
                    }
                    
                    sMo_sxc.bal += dGc_down_amt;  /**修改登记簿余额**/
                }
                else
                {
                    vtcp_log("账户[%ld]生效金额为0、不需登记", g_dd_mst.ac_id);	
                }
            }
            else
            {
                vtcp_log("账户[%ld]未达到生效阀值、不处理", g_dd_mst.ac_id);
            }
            /**--------------更新登记簿----------------**/
            iRet = sql_execute("update mo_sxc set bal=%lf, flag='0' where ac_id=%ld and ac_seqn=%d and sts='1'", \
                                                       sMo_sxc.bal, sMo_sxc.ac_id, sMo_sxc.ac_seqn);
            if(iRet)
            {
                strcpy(g_pub_tx.reply, "D102");
                sprintf( acErrMsg, "sql_execute(update...)出错!!iRet[%d]", iRet );
                WRITEMSG
                Mo_sxc_Clo_Sel();
                goto ErrExit;
            }
            
        }
        else
        {
            vtcp_log("账户[%ld]不需登记、也不需释放结息", g_dd_mst.ac_id);	
        }
        
        iCount++;
        if(iCount == COUNT_COMMIT)
        {
            vtcp_log("开始分步提交iCount[%d]==================", iCount);
            sql_commit();
            iRet = sql_begin();    
            if ( iRet )
            {
                sprintf( acErrMsg, "打开事务失败!!!" );
                WRITEMSG
                Mo_sxc_Clo_Sel();
                goto ErrExit;
            }
            /**重新取流水号**/
            iRet = pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply);
            if(iRet)
            {
                sprintf( acErrMsg, "pub_base_PubQlsh取流水号出错!!!" );
                WRITEMSG
                Mo_sxc_Clo_Sel();
                goto ErrExit;
            }
            set_zd_long(DC_TRACE_NO, g_pub_tx.trace_no);
            
            iCount = 0;
        }
    }
    Mo_sxc_Clo_Sel();

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

int iAcct_sxc_intst(long ac_id, long ac_seqn, double intst)
{
    int iRet = 0;
    
    struct com_item_c sCom_item;
    memset(&sCom_item,    0x00, sizeof(sCom_item));
    memset(&g_dd_parm,    0x00, sizeof(g_dd_parm));
    memset(&g_mdm_ac_rel, 0x00,sizeof(g_mdm_ac_rel));
    
    /***贷 储蓄活期户 ***/
    
    iRet = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Dd_parm_Sel出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    /***根据ac_id找到一个状态为正常/挂失状态的卡号***/
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_id=%ld and note_sts in('0','1') and mdm_code<>'1010'", g_dd_mst.ac_id);
    if(iRet)
    {
    	/**找不到卡号，再找活折号，实在找不到报错**/
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_id=%ld and note_sts in('0','1') and mdm_code='1010'", g_dd_mst.ac_id);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mdm_ac_rel_Sel出错!!!" );
            WRITEMSG
            goto ErrExit;
         }
    }
    strcpy(g_pub_tx.ac_no, g_mdm_ac_rel.ac_no);
    g_pub_tx.ac_id_type[0] = '1';
    vtcp_log("KKKKKKK,g_pub_tx.ac_no[%s]-----------------", g_pub_tx.ac_no);
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
    strcpy(g_pub_tx.brf, "结息");
    
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
     strcpy(g_pub_tx.tx_br_no, g_dd_mst.opn_br_no); strcpy(g_pub_tx.tx_br_no, g_dd_mst.opn_br_no); strcpy(g_pub_tx.tx_br_no, g_dd_mst.opn_br_no);
    /***借活期利息支出***/
    memset(g_pub_tx.ac_no, 0x00, sizeof(g_pub_tx.ac_no));
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