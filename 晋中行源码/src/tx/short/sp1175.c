/*********************************************************************
* 版 本 号 :  V1.0
* 程序作者 :  商永超
* 日    期 :  2011-09-19
* 所属模块 :  判断该笔扣划是否能执行
* 程序名称 :  sp1175.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
* 修改记录 :
*     修改作者 :
*     修改时间 :
*     修改内容 :
交易配置：
insert into tx_def values ('1175', '扣划查询', '10000000000000000001111010000001000000101110000000010000000001000011111110000000100000010011100101100100000000000000000100000100', '0', '4', '2', '3', null);
insert into tx_flow_def values ('1175', 0, '1175', '#$');
insert into tx_sub_def values ('1175', '账号查询', 'sp1175', '0', '0');
***********************************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_ac_rel_c.h"
#include "mo_hold_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"

int sp1175()
{
        int  ret = 0;
        int  iAc_seqn = 0; /*账号序号*/
        int  iHold_seqn = 0;/*冻结序号*/
        double dKh_amt =0.00;/*扣划金额*/
        double dSy_kh_amt = 0.00;/*该笔冻结序号已扣划金额*/
        double dSum_plan_hold_amt = 0.00;/*部分冻结之前冻结金额之和*/
        double dSum_kh_sum_amt = 0.00;/*部分冻结之前已扣划金额之和*/
        double dMax_ky_amt = 0.00;/*部分冻结扣划的可用余额*/
        double dKy_amt = 0.00;/*可用金额*/
        double dXh_amt = 0.00;/*销户金额 等于账面余额*/
        char cKh_type[2];/*扣划类型*/
        char cXh_flag[2];/*销户标志 1 是 0 否*/
        struct prdt_ac_id_c sPrdt_ac_id;
        struct mdm_ac_rel_c sMdm_ac_rel;
        struct mo_hold_c    sMo_hold;
        struct mo_hold_c    sMo_hold_tj;
        struct dd_mst_c sDd_mst;
        struct td_mst_c sTd_mst;
        struct dd_parm_c sDd_parm;
        struct td_parm_c sTd_parm;
        memset(cKh_type,0x00,sizeof(cKh_type));
        memset(cXh_flag,0x00,sizeof(cXh_flag));
        memset(&sDd_mst, 0x00, sizeof(struct dd_mst_c));
        memset(&sTd_mst, 0x00, sizeof(struct td_mst_c));
        memset(&sMdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
        memset(&sPrdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
        memset(&sDd_parm, 0x00, sizeof(struct dd_parm_c));
        memset(&sTd_parm, 0x00, sizeof(struct td_parm_c));        
        memset(&sMo_hold, 0x00, sizeof(struct mo_hold_c));
        memset(&sMo_hold_tj, 0x00, sizeof(struct mo_hold_c));
        /*初始化*/
        ret=pub_base_sysinit();
        if(ret)
        {
             sprintf(acErrMsg,"初始化公用结构错误!!");
             WRITEMSG
             goto ErrExit;
        }
        get_zd_data("0690",cXh_flag);
        get_zd_data("0700",cKh_type);
        get_zd_data("1021", sMdm_ac_rel.ac_no);
        get_zd_int("1022", &iAc_seqn);
        get_zd_double("102F",&dKh_amt);
        get_zd_int("0480",&iHold_seqn);
        pub_base_old_acno(sMdm_ac_rel.ac_no);
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel,"ac_no='%s'",sMdm_ac_rel.ac_no);
        if (ret != 0 && ret != 100)
        {
             sprintf(acErrMsg, "查询mdm_ac_rel错误!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "W011");
             goto ErrExit;
        }
        else if(ret == 100)
        {
             sprintf(acErrMsg, "无此账号!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "W010");
             goto ErrExit;
        }
        if(memcmp(sMdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1)!=0) /***只能开户做扣划***/
        {
            sprintf(acErrMsg,"开户机构与交易机构不符[%s]", sMdm_ac_rel.opn_br_no);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D245");
            goto ErrExit;
        }
        if(sMdm_ac_rel.note_sts[0] != '0' && sMdm_ac_rel.note_sts[0] != '1')
        {
            sprintf(acErrMsg,"该介质状态不正常");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M013");
            goto ErrExit;
        }

        
        ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
        if (ret)
        {
             sprintf(acErrMsg, "查询prdt_ac_rel错误!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "W013");
             goto ErrExit;
        }
        if(sPrdt_ac_id.ac_id_type[0] == '1')/*活期*/
        {
            ret = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
            if (ret)
            {
                sprintf(acErrMsg, "查询dd_mst错误!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "K107");
                goto ErrExit;
            }
            /*检查账号的状态*/
            if(sDd_mst.ac_sts[0] != '1')
            {
                sprintf(acErrMsg,"账户的状态不正常，请检查");
                WRITEMSG
                strcpy(g_pub_tx.reply,"L016");
                goto ErrExit;
            }
            /*如果账号出现部分止付和全部冻结、只进不出同时出现，或者全部止付和其他冻结同时出现，报错，理论上是不会出现这两种情况
            if((sDd_mst.zf_sts[0] == '3' && (sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'))\
                || (sDd_mst.zf_sts[0] == '1' && sDd_mst.hold_sts[0] != '0' ))
            {
                sprintf(acErrMsg,"账户的冻结和止付状态非常不正常，请检查");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH01");
                goto ErrExit;
                
            }
            if(sDd_mst.zf_sts[0] == '1')
            {
                sprintf(acErrMsg,"账户被全部止付，请检查");
                WRITEMSG
                strcpy(g_pub_tx.reply,"SN32");
                goto ErrExit;
            }
            */
            if((sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2') && cKh_type[0] == '1')
            {
                sprintf(acErrMsg,"账户被全部冻结或者只进不出，不允许扣划，请检查");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH02");
                goto ErrExit;
            }          
            ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
            if (ret)
            {
                sprintf(acErrMsg, "查询dd_parm错误!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "W015");
                goto ErrExit;
            }
            dXh_amt = sDd_mst.bal;
            dKy_amt = sDd_mst.bal - sDd_mst.hold_amt  - sDd_mst.ctl_amt -sDd_parm.min_bal;
        }
        else if(sPrdt_ac_id.ac_id_type[0] == '2')/*定期*/
        {
            ret = Td_mst_Sel(g_pub_tx.reply, &sTd_mst,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
            if (ret)
            {
                sprintf(acErrMsg, "查询td_mst错误!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "K108");
                goto ErrExit;
            }
            if(sTd_mst.ac_sts[0] != '1')
            {
                sprintf(acErrMsg,"账户的状态不正常，请检查");
                WRITEMSG
                strcpy(g_pub_tx.reply,"L016");
                goto ErrExit;
            }
            /*如果账号出现部分止付和全部冻结、只进不出同时出现，或者全部止付和其他冻结同时出现，报错，理论上是不会出现这两种情况
            if((sTd_mst.zf_sts[0] == '3' && (sTd_mst.hold_sts[0] == '1' || sTd_mst.hold_sts[0] == '2'))\
                || (sTd_mst.zf_sts[0] == '1' && sTd_mst.hold_sts[0] != '0' ))
            {
                sprintf(acErrMsg,"账户的冻结和止付状态非常不正常，请检查");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH01");
                goto ErrExit;
                
            }
            if(sTd_mst.zf_sts[0] == '1')
            {
                sprintf(acErrMsg,"账户被全部止付，请检查！");
                WRITEMSG
                strcpy(g_pub_tx.reply,"SN32");
                goto ErrExit;
            }
            */
            if((sTd_mst.hold_sts[0] == '1' || sTd_mst.hold_sts[0] == '2') && cKh_type[0] == '1')
            {
                sprintf(acErrMsg,"账号被全部冻结或者只进不出，不允许扣划，请检查！");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH02");
                goto ErrExit;
            }            
            ret = Td_parm_Sel(g_pub_tx.reply, &sTd_parm,"prdt_no='%s'",sTd_mst.prdt_no);
            if (ret)
            {
                sprintf(acErrMsg, "查询td_parm错误!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "W017");
                goto ErrExit;
            }
            dXh_amt = sTd_mst.bal;
            dKy_amt = sTd_mst.bal - sTd_mst.hold_amt  - sTd_mst.ctl_amt -sTd_parm.min_bal; 
        }
        else
        {
            sprintf(acErrMsg, "账户非定期或 活期 !! [%c]",sPrdt_ac_id.ac_id_type[0] );
            WRITEMSG
            strcpy(g_pub_tx.reply, "O238");
            goto ErrExit;
        }
        if(cKh_type[0] == '1')
        {
            if(cXh_flag[0] == '1')
            {
                /*选择直接扣划并销户时，不允许存在其他的冻结、止付*/
                if(sPrdt_ac_id.ac_id_type[0] == '2')
                {
                    if(sTd_mst.hold_sts[0] != '0' )
                    {
                        sprintf(acErrMsg,"该账户已经被冻结或者止付，不许直接扣划并销户！！");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH16");
                        goto ErrExit;
                    }
                    if(pub_base_CompDblVal(sTd_mst.ctl_amt,0.005) > 0)
                    {
                        sprintf(acErrMsg,"该账户存在控制金额，不允许扣划并销户！！，请检查");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH18");
                        goto ErrExit;
                    }
                }
                else if(sPrdt_ac_id.ac_id_type[0] == '1')
                {
                    if(sDd_mst.hold_sts[0] != '0' )
                    {
                        sprintf(acErrMsg,"该账户已经被冻结或者止付，不许直接扣划并销户！！");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH16");
                        goto ErrExit;
                    }
                    if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.005) > 0)
                    {
                        sprintf(acErrMsg,"该账户存在控制金额，不允许扣划并销户！！，请检查");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH18");
                        goto ErrExit;
                    }
                }
                else
                {
                    sprintf(acErrMsg,"账户非定期或者活期[%c]",sPrdt_ac_id.ac_id_type[0]);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"O238");
                    goto ErrExit;
                }
            }
            else if(cXh_flag[0] == '0')
            {
                if(pub_base_CompDblVal(dKh_amt,dKy_amt) > 0)
                {
                     sprintf(acErrMsg,"扣划金额[%f]大于可用金额[%f]",dKh_amt,dKy_amt);
                     WRITEMSG
                     strcpy(g_pub_tx.reply,"KH03");
                     goto ErrExit;
                }
            }
            else
            {
                sprintf(acErrMsg,"销户标志错误[%c]",cXh_flag[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH15");
                goto ErrExit;
            }
        }
        else if(cKh_type[0] == '2')
        {
            ret = Mo_hold_Sel(g_pub_tx.reply, &sMo_hold,"ac_id =%ld and ac_seqn=%d and hold_seqn= %d ",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
            if (ret != 0 && ret != 100)
            {
                 sprintf(acErrMsg, "查询mo_hold错误!! [%d]", ret);
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "KH04");
                 goto ErrExit;
            }
            else if(ret == 100)
            {
                 sprintf(acErrMsg, "此账号没有该笔冻结序号!! [%d]", ret);
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "KH05");
                 goto ErrExit;
            }
            if(sMo_hold.hold_sts[0] != '0')
            {
                sprintf(acErrMsg,"该笔冻结已解除，不能扣划！sMo_hold.hold_sts=[%c]",sMo_hold.hold_sts[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH06");
                goto ErrExit;
            }
            /*断该笔冻结序号是否生效*/
            ret = sql_count("mo_hold", "ac_id= %ld and ac_seqn =%d and hold_seqn < %d and hold_typ in('1','2','4')\
             and hold_sts='0'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
            if(ret > 0)
            {
                sprintf(acErrMsg,"该笔冻结未生效！");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH07");
                goto ErrExit;
            }
            else if(ret<0)
            {
                sprintf(acErrMsg,"查找冻结记录失败！");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH04");
                goto ErrExit;
            }
            
            /*如果要做销户，不允许存在其他的冻结和止付*/
            if(cXh_flag[0] == '1')
            {
                ret = sql_count("mo_hold", "ac_id= %ld and ac_seqn =%d and hold_seqn <> %d and hold_sts='0'",\
                                sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
                if(ret > 0)
                {
                    sprintf(acErrMsg,"该账户存在其他冻结或止付，不允许扣划并销户！！");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH16");
                    goto ErrExit;
                }
                else if(ret<0)
                {
                    sprintf(acErrMsg,"查找冻结记录失败！");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH04");
                    goto ErrExit;
                }
                /*选择销户时，不允许存在控制金额*/
                if(sPrdt_ac_id.ac_id_type[0] == '2')
                {
                    if(pub_base_CompDblVal(sTd_mst.ctl_amt,0.005) > 0)
                    {
                        sprintf(acErrMsg,"该账户存在控制金额，不允许扣划并销户！！，请检查");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH18");
                        goto ErrExit;
                    }
                }
                else if(sPrdt_ac_id.ac_id_type[0] == '1')
                {
                    if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.005) > 0)
                    {
                        sprintf(acErrMsg,"该账户存在控制金额，不允许扣划并销户！！，请检查");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH18");
                        goto ErrExit;
                    }
                }
                else
                {
                    sprintf(acErrMsg,"账户非定期或者活期[%c]",sPrdt_ac_id.ac_id_type[0]);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"O238");
                    goto ErrExit;
                }
            }
            else if(cXh_flag[0] == '0')
            {
                /*检查金额*/
                if(sMo_hold.hold_typ[0] == '3')/*部分冻结扣划*/
                {
                    /*晋中无累计扣划金额
                    此笔冻结剩余可扣划金额 = 计划金额 - 已扣划金额
                    dSy_kh_amt = sMo_hold.plan_hold_amt - sMo_hold.kh_sum_amt;
                    */
                    if(pub_base_CompDblVal(dKh_amt,sMo_hold.plan_hold_amt) != 0)
                    {
                        sprintf(acErrMsg,"扣划金额[%f]与该笔部分冻结金额[%f]不符",dKh_amt,sMo_hold.plan_hold_amt);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH19");
                        goto ErrExit;
                    }
#if 0
                    /*此笔冻结扣划可用金额 = bal - zf_amt - min_bal - ctl_amt - \
                    (此笔部分冻结之前所有未解除的部分冻结金额 - 此笔部分冻结之前所有未解除的部分冻结已扣划金额)*/
                    ret = Mo_hold_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and hold_seqn < %d and \
                hold_typ ='3'  and hold_sts ='0'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
                    if(ret)
                    {
                        sprintf(acErrMsg,"查询冻结登记簿错误");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH04");
                        goto ErrExit;
                    }
                    while(1)
                    {
                        memset(&sMo_hold_tj,0x00,sizeof(struct mo_hold_c));
                        ret = Mo_hold_Fet_Sel(&sMo_hold_tj,g_pub_tx.reply);
                        if(ret != 0 && ret != 100)
                        {
                            sprintf(acErrMsg,"查询mo_hold错误!!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"KH04");
                            goto ErrExit;
                        }
                        else if(ret == 100)
                        {
                            break;
                        }
                        dSum_plan_hold_amt += sMo_hold_tj.plan_hold_amt;
                    }
                    vtcp_log("[%s][%d],dSum_plan_hold_amt=[%lf] dSum_kh_sum_amt=[%lf]",__FILE__,__LINE__,dSum_plan_hold_amt,dSum_kh_sum_amt);
#endif
                    /*计算此笔冻结扣划的可用金额，够扣的话开始扣划*/
                    if(sPrdt_ac_id.ac_id_type[0] == '2')/*定期*/
                    {
                        dMax_ky_amt =  dKy_amt + sMo_hold.plan_hold_amt;
                        if(pub_base_CompDblVal(dKh_amt,dMax_ky_amt) > 0)
                        {
                            sprintf(acErrMsg,"扣划金额[%f]大于此笔冻结可用金额[%f]",dKh_amt,dMax_ky_amt);
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"KH10");
                            goto ErrExit;
                        }            
                    }
                    else if(sPrdt_ac_id.ac_id_type[0] == '1')/*活期*/
                    {
                        dMax_ky_amt =  dKy_amt + sMo_hold.plan_hold_amt;
                        if(pub_base_CompDblVal(dKh_amt,dMax_ky_amt) > 0)
                        {
                            Mo_hold_Clo_Upd();
                            sprintf(acErrMsg,"扣划金额[%f]大于此笔冻结可用金额[%f]",dKh_amt,dMax_ky_amt);
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"KH10");
                            goto ErrExit;
                        }
                    }
                    else
                    {
                        sprintf(acErrMsg,"账户非定期或者活期[%c]",sPrdt_ac_id.ac_id_type[0]);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"O238");
                        goto ErrExit;
                    }
                   vtcp_log("[%s][%d],dKy_amt=[%lf] dMax_ky_amt=[%lf],plan_hold_amt=[%f]",__FILE__,__LINE__,dKy_amt,dMax_ky_amt,sMo_hold.plan_hold_amt);

                }
                else if(sMo_hold.hold_typ[0] == '1' || sMo_hold.hold_typ[0] == '2')/*全部冻结或者只进不出扣划*/
                {
                    if(pub_base_CompDblVal(dKh_amt,dKy_amt) > 0)
                    {
                        sprintf(acErrMsg,"扣划金额[%f]大于可用金额[%f]",dKh_amt,dKy_amt);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH03");
                        goto ErrExit;
                    }         
                }
                else
                {
                    sprintf(acErrMsg,"冻结类型错[%c]",sMo_hold.hold_typ[0]);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH11");
                    goto ErrExit;
                }
            }
            else
            {
                sprintf(acErrMsg,"销户标志错误[%c]",cXh_flag[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH15");
                goto ErrExit;
            }
        }
        else
        {
            sprintf(acErrMsg,"扣划类型错[%c]",cKh_type[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH12");
            goto ErrExit;
        }
OkExit:
        strcpy(g_pub_tx.reply, "0000");
        sprintf(acErrMsg, "success !!: reply is[%s]", g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg, "fail !!: reply is[%s]", g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 1;
}


