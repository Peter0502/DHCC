/*********************************************************************
* 版 本 号 :  V1.0
* 程序作者 :  商永超
* 日    期 :  2011-09-14
* 所属模块 :  扣划交易
* 程序名称 :  spD120.c
* 程序作用 :  根据前台输入的扣划类型，冻结序号，扣划账号，收款账号，完成扣划操作。
* 函数列表 :
* 使用注意 : 本程序和spD214.c联合使用。
* 修改记录 :
*     修改作者 :
*     修改时间 :
*     修改内容 :
交易配置：
insert into tx_def values ('4713', '扣划交易', '10000000000010000000000000000000000000111110000000000000000000000001000000000000000000000000000000000100000000000000000000000100', '1', '3', '2', '3', null);

--------tx_flow_def ----------
insert into tx_flow_def values ('4713', 0, 'D120', '#=0001@5@#=0002@1@#$');
insert into tx_flow_def values ('4713', 1, 'D201', '#=0000@2@#$');
insert into tx_flow_def values ('4713', 2, 'D200', '#=0000@3@#$');
insert into tx_flow_def values ('4713', 3, 'D099', '#=0000@4@#$');
insert into tx_flow_def values ('4713', 4, 'D214', '#$');
insert into tx_flow_def values ('4713', 5, 'D003', '#=0000@6@#$');
insert into tx_flow_def values ('4713', 6, 'd099', '#=0000@4@#$');

-------tx_sub_def 已经有的就不用增加了-----------
insert into tx_sub_def values ('D120', '扣划交易', 'spD120', '0', '0');
-----下面这几个tx_sub_def应该有了---
insert into tx_sub_def values ('D201', '部提计息', 'spD201', '1', '0');
insert into tx_sub_def values ('D200', '部提子交易', 'spD200', '1', '0');
insert into tx_sub_def values ('D099', '存款子交易', 'spD099', '1', '0');
insert into tx_sub_def values ('D003', '取款子交易', 'spD003', '1', '0');
insert into tx_sub_def values ('d099', '保证金到期转应解汇款', 'spD099', '1', '0');

-----tx_sub_rel-----
insert into tx_sub_rel values ('4713', 'D003', '1', '1', '0191', '启用0190');
insert into tx_sub_rel values ('4713', 'D003', '1', '1001', '0192', '控制标志');
insert into tx_sub_rel values ('4713', 'D003', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel values ('4713', 'D099', '0', '102J', '101A', '币种');
insert into tx_sub_rel values ('4713', 'D099', '0', '102S+102P-102Q', '1013', '金额');
insert into tx_sub_rel values ('4713', 'D099', '1', '1', '0191', '启用0190');
insert into tx_sub_rel values ('4713', 'D099', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel values ('4713', 'D099', '1', '0001', '0192', '控制标志');
insert into tx_sub_rel values ('4713', 'd099', '1', '1', '0191', '启用0190');
insert into tx_sub_rel values ('4713', 'd099', '1', '0001', '0192', '控制标志');
insert into tx_sub_rel values ('4713', 'd099', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel values ('4713', 'd099', '0', '102J', '101A', '币种');
insert into tx_sub_rel values ('4713', 'd099', '0', '102S', '1013', '金额');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_hold_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "td_parm_c.h"
#include "dd_parm_c.h"
int spD120()
 {
    int ret = 0;
    int iHold_seqn = 0;/*冻结序号*/
    int iAc_seqn = 0;/*账号序号*/
    double dSum_plan_hold_amt = 0.00;/*部分冻结之前冻结金额之和*/
    double dSum_kh_sum_amt = 0.00;/*部分冻结之前已扣划金额之和*/
    double dSy_kh_amt = 0.00; /*部分冻结计划剩余金额*/
    double dMax_ky_amt = 0.00;/*部分冻结扣划的可用余额*/
    double dKh_amt = 0.00;/*扣划金额*/
    double dKy_amt = 0.00;/*可用金额*/
    char cKh_type[2];/*扣划种类 1直接扣划 2冻结扣划*/
    char cOut_ac_no[25];/*扣款账号*/
    char cIn_ac_no[25];/*收款账号*/
    
    struct mo_hold_c sMo_hold;
    struct mo_hold_c sMo_hold_tj;
    struct dd_mst_c sDd_mst;
    struct td_mst_c sTd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;
    struct td_parm_c sTd_parm;
    struct dd_parm_c sDd_parm;
    memset(cKh_type,0,sizeof(cKh_type));
    memset(cOut_ac_no,0,sizeof(cOut_ac_no));
    memset(cIn_ac_no,0,sizeof(cIn_ac_no));
    memset(&sMo_hold,0x0,sizeof(struct mo_hold_c));
    memset(&sMo_hold_tj,0x0,sizeof(struct mo_hold_c));
    memset(&sDd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&sTd_mst,0x0,sizeof(struct td_mst_c));
    memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
    memset(&sPrdt_ac_id,0x0,sizeof(struct prdt_ac_id_c));
    memset(&sTd_parm,0x0,sizeof(struct td_parm_c));
    memset(&sDd_parm,0x0,sizeof(struct dd_parm_c));
    /*初始化*/
    ret=pub_base_sysinit();
    if(ret)
    {
         sprintf(acErrMsg,"初始化公用结构错误!!");
         WRITEMSG
         goto ErrExit;
    }
    /*取值*/
    
    get_zd_data("1021",cOut_ac_no);
    vtcp_log("[%s][%d] 扣款账号：[%s]",__FILE__,__LINE__,cOut_ac_no);
    get_zd_int("1022",&iAc_seqn);
    vtcp_log("[%s][%d] 账号序号：[%d]",__FILE__,__LINE__,iAc_seqn);
    get_zd_data("0300",cIn_ac_no);
    vtcp_log("[%s][%d] 收款账号：[%s]",__FILE__,__LINE__,cIn_ac_no);
    get_zd_data("0700",cKh_type);
    vtcp_log("[%s][%d] 扣划类型：[%s]",__FILE__,__LINE__,cKh_type);
    get_zd_double("102F",&dKh_amt);
    vtcp_log("[%s][%d] 扣划金额：[%f]",__FILE__,__LINE__,dKh_amt);
    get_zd_int("0480",&iHold_seqn);
    vtcp_log("[%s][%d] 冻结序号：[%d]",__FILE__,__LINE__,iHold_seqn);

    /*根据账号找ac_id*/
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cOut_ac_no);
    if(ret !=0 && ret !=100)
    {
         sprintf(acErrMsg,"查介质账户关系表错误");
         WRITEMSG
         strcpy(g_pub_tx.reply,"L015");
         goto ErrExit;
    }
    else if(ret == 100)
    {
         sprintf(acErrMsg,"无此账号");
         WRITEMSG
         strcpy(g_pub_tx.reply,"LS53");
         goto ErrExit;
    }
    if(sMdm_ac_rel.note_sts[0] != '0' && sMdm_ac_rel.note_sts[0] != '1')
    {
        sprintf(acErrMsg,"该介质状态不正常");
        WRITEMSG
        strcpy(g_pub_tx.reply,"M013");
        goto ErrExit;
    }
    /*根据ac_id 和ac_seqn找账号类型*/
    ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
    if(ret)
    {
         sprintf(acErrMsg,"查找产品账号对照表错误");
         WRITEMSG
         strcpy(g_pub_tx.reply,"W013");
         goto ErrExit;
    }
    if(sPrdt_ac_id.ac_id_type[0] == '2') /*定期*/
    {
        ret=Td_mst_Sel(g_pub_tx.reply,&sTd_mst,"ac_id =%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
        if(ret)
        {
            sprintf(acErrMsg,"查找定期主文件失败");
            WRITEMSG
            strcpy(g_pub_tx.reply,"X004");
            goto ErrExit;
        }
        /*检查账号的状态*/
        if(sTd_mst.ac_sts[0] != '1')
        {
            sprintf(acErrMsg,"账户的状态不正常，请检查");
            WRITEMSG
            strcpy(g_pub_tx.reply,"L016");
            goto ErrExit;
        }
        /*如果账号出现部分止付和全部冻结、只进不出同时出现，或者全部止付和其他冻结同时出现，报错，理论上是不会出现这两种情况*/
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
        if((sTd_mst.hold_sts[0] == '1' || sTd_mst.hold_sts[0] == '2') && cKh_type[0] == '1')
        {
            sprintf(acErrMsg,"账号被全部冻结或者只进不出，不允许扣划，请检查！");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH02");
            goto ErrExit;
        }
        ret=Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no='%s'",sTd_mst.prdt_no);
        if(ret)
        {
            sprintf(acErrMsg,"查找定期产品参数表失败");
            WRITEMSG
            strcpy(g_pub_tx.reply,"W027");
            goto ErrExit;
        }
        /*计算可用金额*/
        dKy_amt = sTd_mst.bal - sTd_mst.hold_amt - sTd_mst.zf_amt - sTd_mst.ctl_amt -sTd_parm.min_bal;          
    }
    else if(sPrdt_ac_id.ac_id_type[0] == '1') /*活期*/
    {
        ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id =%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
        if(ret)
        {
            sprintf(acErrMsg,"查找活期主文件失败");
            WRITEMSG
            strcpy(g_pub_tx.reply,"X003");
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
        /*如果账号出现部分止付和全部冻结、只进不出同时出现，或者全部止付和其他冻结同时出现，报错，理论上是不会出现这两种情况*/
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
        if((sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2') && cKh_type[0] == '1')
        {
            sprintf(acErrMsg,"账户被全部冻结或者只进不出，不允许扣划，请检查");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH02");
            goto ErrExit;
        }
        ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
        if(ret)
        {
            sprintf(acErrMsg,"查找活期产品参数表失败");
            WRITEMSG
            strcpy(g_pub_tx.reply,"W027");
            goto ErrExit;
        }
        /*计算可用金额*/
        dKy_amt = sDd_mst.bal - sDd_mst.hold_amt - sDd_mst.zf_amt - sDd_mst.ctl_amt -sDd_parm.min_bal;
    }
    else
    {
        sprintf(acErrMsg,"账户非定期或者活期[%c]",sPrdt_ac_id.ac_id_type[0]);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O238");
        goto ErrExit;
    }
    /*扣划分为直接扣划和冻结扣划两种情况*/
    if(cKh_type[0] == '1')/*直接扣划*/
    {
        if(pub_base_CompDblVal(dKh_amt,dKy_amt) == 1)
        {
            sprintf(acErrMsg,"扣划金额[%f]大于可用金额[%f]",dKh_amt,dKy_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH03");
            goto ErrExit;
        }
        /*开始扣划*/
        if(sPrdt_ac_id.ac_id_type[0] == '2')
        {
            strcpy(g_pub_tx.reply,"0002");
        }
        else if(sPrdt_ac_id.ac_id_type[0] == '1')
        {
            strcpy(g_pub_tx.reply,"0001");
        }
        else
        {
            sprintf(acErrMsg,"账户非定期或者活期[%c]",sPrdt_ac_id.ac_id_type[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"O238");
            goto ErrExit;
        }
    }
    else if(cKh_type[0] == '2')/*冻结扣划*/
    {
        ret=Mo_hold_Dec_Upd(g_pub_tx.reply,"ac_id =%ld and ac_seqn=%d and hold_seqn= %d and hold_stop ='1'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
        if(ret)
        {
            sprintf(acErrMsg,"Mo_hold_dec_Upd出错！");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH04");
            goto ErrExit;
        }
        ret=Mo_hold_Fet_Upd(&sMo_hold,g_pub_tx.reply);
        if(ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"Mo_hold_Fet_Upd 出错！");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH04");
            goto ErrExit;
        }
        else if(ret == 100)
        {
            Mo_hold_Clo_Upd();
            sprintf(acErrMsg,"无此冻结序号!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH05");
            goto ErrExit;
        }
        if(sMo_hold.hold_sts[0] != '0')
        {
            Mo_hold_Clo_Upd();
            sprintf(acErrMsg,"该笔冻结已解除，不能扣划！sMo_hold.hold_sts=[%c]",sMo_hold.hold_sts[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH06");
            goto ErrExit;
        }
        
        /*判断该笔部分冻结是否生效,检查此笔冻结记录之前是否存在全部冻结，只进不出，全部止付*/
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
        /*该笔生效，开始处理,分为部分冻结扣划，全部冻结、只进不出扣划两种情况*/
        if(sMo_hold.hold_typ[0] == '3')/*部分冻结扣划*/
        {
            /*此笔冻结剩余可扣划金额 = 计划金额 - 已扣划金额*/
            dSy_kh_amt = sMo_hold.plan_hold_amt - sMo_hold.kh_sum_amt;
            if(pub_base_CompDblVal(dKh_amt,dSy_kh_amt) == 1)
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"扣划金额[%f]大于此笔冻结剩余可扣划金额[%f]",dKh_amt,dSy_kh_amt);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH08");
                goto ErrExit;
            }
            /*此笔冻结扣划可用金额 = bal - zf_amt - min_bal - ctl_amt - \
            (此笔部分冻结之前所有未解除的部分冻结金额 - 此笔部分冻结之前所有未解除的部分冻结已扣划金额)*/
            /*******
            ret=sql_sum_double( "mo_hold","plan_hold_amt",&dSum_plan_hold_amt,"ac_id=%ld and ac_seqn=%d and hold_seqn < %d and \
            hold_typ ='3'  and hold_stop ='1' and hold_sts ='0'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
            if(ret)
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"计算冻结金额错误！");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH09");
                goto ErrExit;
            }
            ret=sql_sum_double( "mo_hold","kh_sum_amt",&dSum_kh_sum_amt,"ac_id=%ld and ac_seqn =%d and hold_seqn < %d \
            and hold_typ ='3' and hold_stop ='1'and hold_sts ='0' ",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
            if(ret)
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"计算已扣划金额错误！");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH09");
                goto ErrExit;
            }
            **************/
             ret = Mo_hold_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and hold_seqn < %d and \
            hold_typ ='3'  and hold_stop ='1' and hold_sts ='0'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
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
                 /*如果是计划扣划的，不累计*/
                 if(memcmp(sMo_hold_tj.hold_knd,"JHKH",4) == 0)
                 {
                     vtcp_log("[%][%s] 计划扣划的冻结不累计 计划冻结金额和 已扣划金额",__FILE__,__LINE__);
                     continue;
                 }
                 dSum_plan_hold_amt += sMo_hold_tj.plan_hold_amt;
                 dSum_kh_sum_amt += sMo_hold_tj.kh_sum_amt;
             }
            vtcp_log("[%s][%d],dSum_plan_hold_amt=[%lf] dSum_kh_sum_amt=[%lf]",__FILE__,__LINE__,dSum_plan_hold_amt,dSum_kh_sum_amt);
            /*计算此笔冻结扣划的可用金额，够扣的话开始扣划*/
            if(sPrdt_ac_id.ac_id_type[0] == '2')/*定期*/
            {
                dMax_ky_amt =  sTd_mst.bal - sTd_mst.zf_amt - sTd_mst.ctl_amt -sTd_parm.min_bal - (dSum_plan_hold_amt -dSum_kh_sum_amt);
                if(pub_base_CompDblVal(dKh_amt,dMax_ky_amt) == 1)
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg,"扣划金额[%f]大于此笔冻结可用金额[%f]",dKh_amt,dMax_ky_amt);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH10");
                    goto ErrExit;
                }
                
                ret=sql_execute("update td_mst set hold_amt =%f   where ac_id = %ld and ac_seqn=%d",sTd_mst.hold_amt-dKh_amt,sMdm_ac_rel.ac_id,iAc_seqn);
                if(ret)
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg,"更新td_mst 失败！");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"O013");
                    goto ErrExit;
                }
                strcpy(g_pub_tx.reply,"0002");            
            }
            else if(sPrdt_ac_id.ac_id_type[0] == '1')/*活期*/
            {
                dMax_ky_amt =  sDd_mst.bal - sDd_mst.zf_amt - sDd_mst.ctl_amt -sDd_parm.min_bal - (dSum_plan_hold_amt -dSum_kh_sum_amt);
                if(pub_base_CompDblVal(dKh_amt,dMax_ky_amt) == 1)
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg,"扣划金额[%f]大于此笔冻结可用金额[%f]",dKh_amt,dMax_ky_amt);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH10");
                    goto ErrExit;
                }
                
                ret=sql_execute("update dd_mst set hold_amt = %f where ac_id = %ld and ac_seqn=%d",sDd_mst.hold_amt-dKh_amt,sMdm_ac_rel.ac_id,iAc_seqn);
                if(ret)
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg,"更新dd_mst 失败！");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"O012");
                    goto ErrExit;
                }
                strcpy(g_pub_tx.reply,"0001");            
            }
            else
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"账户非定期或者活期[%c]",sPrdt_ac_id.ac_id_type[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"O238");
                goto ErrExit;
            }
            /*修改冻结记录*/    
            sMo_hold.kh_sum_amt += dKh_amt;
            sMo_hold.kh_sum_cnt ++;
            /*判断该笔冻结是否已全部扣划完,已扣划完且为自动解冻的 做解冻结处理*/
            if(pub_base_CompDblVal(sMo_hold.kh_sum_amt,sMo_hold.plan_hold_amt) == 0 && sMo_hold.auto_unhold_ind[0] == 'Y')
            {
                sMo_hold.hold_sts[0] = '1';/*冻结解除*/
                /*如果此账号只有这一笔部分冻结，则更改主表状态hold_sts = 0 正常*/
                ret = sql_count("mo_hold", "ac_id= %ld and ac_seqn=%d and hold_seqn <> %d and hold_stop='1' and hold_sts='0'",\
                    sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
                if (ret == 0)
                {
                    if(sPrdt_ac_id.ac_id_type[0] == '2')/*定期*/
                    {
                        ret=sql_execute("update td_mst set hold_sts ='0'  where ac_id = %ld and ac_seqn=%d ",\
                        sMdm_ac_rel.ac_id,iAc_seqn);
                        if(ret)
                        {
                            Mo_hold_Clo_Upd();
                            sprintf(acErrMsg,"更新td_mst 失败！");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"O013");
                            goto ErrExit;
                        }            
                    }
                    else if(sPrdt_ac_id.ac_id_type[0] == '1')/*活期*/
                    {
                        ret=sql_execute("update dd_mst set hold_sts ='0' where ac_id = %ld and ac_seqn=%d ",\
                        sMdm_ac_rel.ac_id,iAc_seqn);
                        if(ret)
                        {
                            Mo_hold_Clo_Upd();
                            sprintf(acErrMsg,"更新dd_mst 失败！");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"O012");
                            goto ErrExit;
                        }      
                    }
                    else
                   {
                       Mo_hold_Clo_Upd();
                       sprintf(acErrMsg,"账户非定期或者活期[%c]",sPrdt_ac_id.ac_id_type[0]);
                       WRITEMSG
                       strcpy(g_pub_tx.reply,"O238");
                       goto ErrExit;
                   }
                } 
                else if (ret < 0) 
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg, "查询冻结登记簿错误");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "KH04");
                    goto ErrExit;
                }
                    
            }                               
        }
        else if(sMo_hold.hold_typ[0] == '1' || sMo_hold.hold_typ[0] == '2')/*全部冻结或者只进不出扣划*/
        {
            if(pub_base_CompDblVal(dKh_amt,dKy_amt) == 1)
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"扣划金额[%f]大于可用金额[%f]",dKh_amt,dKy_amt);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH03");
                goto ErrExit;
            }
            /*开始扣划*/
            if(sPrdt_ac_id.ac_id_type[0] == '2')/*定期*/
            {
                strcpy(g_pub_tx.reply,"0002");            
            }
            else if(sPrdt_ac_id.ac_id_type[0] == '1')/*活期*/
            {
                strcpy(g_pub_tx.reply,"0001");            
            }
            else
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"账户非定期或者活期[%c]",sPrdt_ac_id.ac_id_type[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"O238");
                goto ErrExit;
            }
            /*修改冻结记录*/    
            sMo_hold.kh_sum_amt += dKh_amt;
            sMo_hold.kh_sum_cnt ++;          
        }
        else
        {
            Mo_hold_Clo_Upd();
            sprintf(acErrMsg,"冻结类型错[%c]",sMo_hold.hold_typ[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH11");
            goto ErrExit;
        }
        ret=Mo_hold_Upd_Upd(sMo_hold,g_pub_tx.reply);
        if(ret)
       {
           Mo_hold_Clo_Upd();
           sprintf(acErrMsg,"更新mo_hold 失败！");
           WRITEMSG
           strcpy(g_pub_tx.reply,"O021");
           goto ErrExit;
       }
        Mo_hold_Clo_Upd();
    }
    else
    {
        sprintf(acErrMsg,"扣划类型错[%c]",cKh_type[0]);
        WRITEMSG
        strcpy(g_pub_tx.reply,"KH12");
        goto ErrExit;
    }   
  OkExit:
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
