/*********************************************************************
* 版 本 号 :  V1.0        
* 程序作者 :  CHENGBO          
* 日    期 :  2011-07-13   
* 所属模块 :  与平台的接口程序  (联机单笔退回交易)            
* 程序名称 :  spF236.c
* 程序作用 : 
*             (1)联机单笔撤销主要用于隔日交易，只处理转帐不处理现金。  
*             (2)核心需要检查收付款帐号与原交易收付款帐号是否一致，退回交易金额�*                超限或者撤销金额是否与原交易金额一致等。
*             (3)帐务处理由核心根据登记簿记录自动组织，手续费是否退回由相应控制�*                指定，若需退回则直接退回到付款帐户上，不考虑退现金
* 函数列表 :       
* 使用注意 :                                       
* 修改记录 :                                       
*      日    期:20110816
*      修 改 人：xueke
*      修改内容:pub_com_test.c()控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
*     <EYY83 DESC="原交易已做退回"/>
*     <EYY03 DESC="退回金额超出原交易金额!"/>
*     <EYY04 DESC="不能做现金记账的退回!"/>
      <EYY06 DESC="退回手续费超出原手续费!"/>

insert into tx_def  values ('6236', '联机单笔转账退回', '10000000000000000000000000000110000000010000000000000000000001000100000000000000100000000000000000000000000000000000000000000000', '1', '1', '0', '3', null);
insert into tx_flow_def  values ('6236', 0, 'F236', '#=0001@1@#=0002@2@#$');
insert into tx_flow_def  values ('6236', 1, 'D003', '#=0000@3@#$');
insert into tx_flow_def  values ('6236', 2, 'A016', '#=0000@3@#$');
insert into tx_flow_def  values ('6236', 3, 'HPDF', '#=0000@4@#$');
insert into tx_flow_def  values ('6236', 4, 'F000', '#$');

insert into tx_sub_def  values ('F236', '联机单笔转账退回', 'spF236', '0', '0');

insert into tx_sub_rel  values ('6236', 'D003', '0', '0310', '1021', '账号');
insert into tx_sub_rel  values ('6236', 'D003', '1', '0000', '0192', '控制标志');
insert into tx_sub_rel  values ('6236', 'D003', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel  values ('6236', 'D003', '1', '1', '0191', '控制标志');
insert into tx_sub_rel  values ('6236', 'D003', '1', '1', '1022', '账户序号');
insert into tx_sub_rel  values ('6236', 'D003', '0', '0400', '102F', '金额');
insert into tx_sub_rel  values ('6236', 'D003', '1', '01', '102J', '币种');
insert into tx_sub_rel  values ('6236', 'D003', '1', '2', '102K', '现转标志');
insert into tx_sub_rel  values ('6236', 'D003', '0', '0810', '102Y', '摘要');

insert into tx_sub_rel  values ('6236', 'A016', '0', '0310', '1201', '账号');
insert into tx_sub_rel  values ('6236', 'A016', '1', '01', '1204', '币种');
insert into tx_sub_rel  values ('6236', 'A016', '1', '2', '1205', '转账');
insert into tx_sub_rel  values ('6236', 'A016', '0', '0400', '1208', '金额');
insert into tx_sub_rel  values ('6236', 'A016', '0', '0810', '120A', '摘要');

insert into tx_sub_rel  values ('6236', 'HPDF', '1', '2', '0660', '借贷标志');
insert into tx_sub_rel  values ('6236', 'HPDF', '1', '01', '0210', '币种');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0300', '0300', '账号');
insert into tx_sub_rel  values ('6236', 'HPDF', '1', '2', '0670', '现转标志');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0400', '0400', '金额');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0810', '0880', '摘要');

******************************************************************/

#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "chnl_single_acct_c.h"
#include "com_item_c.h"
#include "in_mst_c.h"
#include "dd_mst_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_ac_rel_c.h"

spF236()
{
    int iRet=0;
    char cAc_no[25];   /*交易账号*/
    /*char cHx_cert[21];    证件类型、证件号码*/
    /*char cHx_ctrlcmd[11]; 控制标志*/
    char  cCert_no[21];  /**证件类型/号码**/
    char  cCtrlcmd[11];  /**控制标志 **/
    char  cFee_flag[2]; /*收费标志*/
    char cHx_msr2[28];    /*二磁道信息*/
    char cHx_msr3[105];   /*三磁道信息*/
    /*char cHx_pwd[7];      客户密码*/
    char  cPwd[17];  /** 客户密码 **/
    char cHx_brf[51];     /*摘要*/
    char cBbmk[5];        /*系统种类*/
    char cHx_trace_no[13];/*核心流水*/
    char cPt_trace_no[13];/*原平台流水*/
    char cPt_trano[13];   /*平台流水*/
    double dHx_amt=0.00;  /*交易金额*/
    double dBj_fee=0.00;  /*比较手续费使用*/
    long lHx_old_date=0;  /*原平台日期*/
    long lHx_old_trace=0; /*原平台流水*/
    long lPt_trace_no=0;  /*平台流水*/
    long lPt_date=0;
    long lHx_trace=0;  /*核心流水*/
    
    int i;/*循环时使用*/
    long   lFee_cnt=0;     /*收费条数*/
    char   cFee_cnt[3];    /*收费条数*/
    char   cFd123[1000];   /*接收123域内容*/
    char   cFee_amt[13];   /*退回收费金额*/
    double dFee_amt=0.00;  /*退回收费金额*/
    char   cFee_dc_ind[2]; /*收费 借贷标志 1 借2 贷*/
    /*调用pub_com_test函数时，接收返回值*/
    char cGz_flag[2];/*挂账标志*/
    double dTh_amt=0.00; /*累计退回金额20111011 xk*/
   
    struct chnl_single_acct_c sChnl_single_acct;
    struct chnl_single_acct_c sChnl_single_acct_1;
    struct chnl_single_acct_c sChnl_single_acct_2;
    struct com_item_c   sCom_item;
    struct com_item_c   sCom_item_1;
    struct dd_mst_c     sDd_mst;
    struct dd_mst_c     sDd_mst_1;
    struct in_mst_c     sIn_mst;
    struct in_mst_c     sIn_mst_1;
    struct prdt_ac_id_c sPrdt_ac_id;
    struct prdt_ac_id_c sPrdt_ac_id_1;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct mdm_ac_rel_c sMdm_ac_rel_1;

    memset(cAc_no,0x00,sizeof(cAc_no)); 
    /*memset(cHx_cert, 0x00,sizeof(cHx_cert)); */
    memset(cCert_no, 0x00, sizeof(cCert_no));
    memset(cCtrlcmd,0x00,sizeof(cCtrlcmd)); 
    memset(cFee_flag,0x00,sizeof(cFee_flag)); 
    memset(cHx_msr2,0x00,sizeof(cHx_msr2)); 
    memset(cHx_msr3,0x00,sizeof(cHx_msr3)); 
    /*memset(cHx_pwd,0x00,sizeof(cHx_pwd));*/ 
    memset(cPwd, 0x00, sizeof(cPwd));
    memset(cHx_brf,0x00,sizeof(cHx_brf));
    memset(cBbmk,0x00,sizeof(cBbmk));
    memset(cHx_trace_no,0x00,sizeof(cHx_trace_no));
    memset(cPt_trace_no,0x00,sizeof(cPt_trace_no));
    memset(cPt_trano,0x00,sizeof(cPt_trano));   /*平台流水*/
    memset(&sChnl_single_acct,0x00,sizeof(struct chnl_single_acct_c));
    memset(&sChnl_single_acct_1,0x00,sizeof(struct chnl_single_acct_c));
    memset(&sChnl_single_acct_2,0x00,sizeof(struct chnl_single_acct_c));
    memset(&sCom_item,0x00,sizeof(struct com_item_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sDd_mst_1,0x00,sizeof(struct dd_mst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sIn_mst_1,0x00,sizeof(struct in_mst_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sPrdt_ac_id_1,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sMdm_ac_rel_1,0x00,sizeof(struct mdm_ac_rel_c));
    /*调用pub_com_test时用到*/
    memset(cGz_flag,0x00,sizeof(cGz_flag));
    memset(cFd123,0x00,sizeof(cFd123));
    memset(cFee_amt,0x00,sizeof(cFee_amt));
    memset(cFee_cnt,0x00,sizeof(cFee_cnt));
    memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind));
    
    /*初始化变量*/
    pub_base_sysinit();
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    get_zd_data("0300",cAc_no);
    get_zd_data("0620",cCert_no);
    get_zd_data("0730",cCtrlcmd);
    get_zd_data("0710",cFee_flag);
    get_zd_data("0750",cHx_msr2);
    get_zd_data("0760",cHx_msr3);
/*****
    get_zd_data("0790",cPwd);
******/
    get_zd_data("0810",cHx_brf);
    get_zd_data("0100",cBbmk);
    get_zd_long("0450",&lHx_old_date);
    get_zd_long("0780",&lHx_old_trace);
    get_zd_double("0400",&dHx_amt);
    get_zd_long("0440",&lPt_date);
    get_zd_long("0520",&lPt_trace_no); 
    /*平台流水数据类型转换*/
    sprintf(cPt_trace_no,"%d",lHx_old_trace);
    
    /*新旧账号转换*/ 
    pub_base_old_acno(cAc_no);
    memcpy(g_pub_tx.draw_pwd,cPwd,sizeof(g_pub_tx.draw_pwd)-1);
    /**增加bbmk 字段 shangyongchao 2012/6/7 11:21:31
    iRet = Chnl_single_acct_Sel(g_pub_tx.reply,&sChnl_single_acct," pt_date=%ld and pt_trace_no='%s' ",lHx_old_date,cPt_trace_no);
    **/
    iRet = Chnl_single_acct_Sel(g_pub_tx.reply,&sChnl_single_acct," pt_date=%ld and pt_trace_no='%s' and bbmk='%s' ",\
    lHx_old_date,cPt_trace_no,cBbmk);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D103");
        sprintf(acErrMsg, "查询要被退回的交易记录错误!");
        WRITEMSG
        goto ErrExit;
    }
    
    if(sChnl_single_acct.ct_ind[0]=='1')
    {
        strcpy(g_pub_tx.reply,"YY04");
        sprintf(acErrMsg,"不能做现金记账的退回!");
        WRITEMSG
        goto ErrExit;
    } 
    if(sChnl_single_acct.sts[0]=='5')
    {
        strcpy(g_pub_tx.reply,"YY83");
        sprintf(acErrMsg, "该转账已被退回");
        WRITEMSG
        goto ErrExit;
    }
    
    if( strcmp(sChnl_single_acct.ac_no,cAc_no) != 0)
    {
        strcpy(g_pub_tx.reply, "YY01");
        sprintf(acErrMsg,"该账号与原账号不符!");
        WRITEMSG
        goto ErrExit;
    } 
    vtcp_log("%s,%d,dHx_amt=[%f],th_amt=[%f],tx_amt=[%f]",__FILE__,__LINE__,dHx_amt,sChnl_single_acct.th_amt,sChnl_single_acct.tx_amt);
    if( pub_base_CompDblVal(sChnl_single_acct.tx_amt,sChnl_single_acct.th_amt+dHx_amt) < 0)
    {
        strcpy(g_pub_tx.reply,"YY03");
        sprintf(acErrMsg,"退回金额超出原交易金额!");
        WRITEMSG
        goto ErrExit;
    }
    /*检查退回手续费 slz*/
    get_fd_data("1230",cFd123);
    memcpy(cFee_cnt,cFd123+1,2);
    vtcp_log("[%s][%d],cFee_cnt=[%s]",__FILE__,__LINE__,cFee_cnt);
    lFee_cnt=atoi(cFee_cnt);
    vtcp_log("[%s][%d],lFee_cnt=[%d]",__FILE__,__LINE__,lFee_cnt);
    dFee_amt=0.00;
    for(i=0;lFee_cnt > i; i++)
    {   
        memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind)); 
        memset(cFee_amt,0x00,sizeof(cFee_amt));
        memcpy(cFee_dc_ind,cFd123+27+48*i,1);
        memcpy(cFee_amt,cFd123+29 + 48 * i,12);
        vtcp_log("[%s][%d],cFee_dc_ind=[%s]",__FILE__,__LINE__,cFee_dc_ind);
        vtcp_log("[%s][%d],cFee_amt=[%s]",__FILE__,__LINE__,cFee_amt);
        dBj_fee=atof(cFee_amt) / 100;
        if(cFee_dc_ind[0] == '2')
        {
            dFee_amt+=dBj_fee;
        }
    }
    /**add by chenggx 20120106 手续费标志2贷记调整使用不检查**/
    if(cFee_flag[0]=='2'){
        vtcp_log("%s,%d,贷记调整不检查0710[%s]!",__FILE__,__LINE__,cFee_flag);
    }else{
        if( pub_base_CompDblVal(sChnl_single_acct.fee_amt,dFee_amt) < 0)
        {
            strcpy(g_pub_tx.reply,"YY06");
            sprintf(acErrMsg,"退回手续费超出原手续费!");
            WRITEMSG
            goto ErrExit;
        }
    }
     /* end 检查退回手续费 slz*/
    if( strlen ( sChnl_single_acct.ac_no2 ) >10) /*** 此种情况下,只能是账号 ***/
    {
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",sChnl_single_acct.ac_no2);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        if(sMdm_ac_rel.note_sts[0] != '0')
        {
            vtcp_log("%s,%d,ac_no[%s]介质状态不正常!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
            strcpy(g_pub_tx.reply,"D140");
            goto ErrExit;
        }
        iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",sMdm_ac_rel.ac_id);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_id错误!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if(sPrdt_ac_id.ac_id_type[0] == '1')
        {
            iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sDd_mst.ac_sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            strcpy(g_pub_tx.reply, "0001");
        }
        else if(sPrdt_ac_id.ac_id_type[0] == '9')
        {
            iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sIn_mst.sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            strcpy(g_pub_tx.reply, "0002");
        }
               
    }
    else
    {
        /**** 否则是科目  **/
        iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",sChnl_single_acct.ac_no2);
        if(iRet)
        {
            sprintf(acErrMsg,"查询科目失败!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
        /*** 开始配置记帐要素 **/
        strcpy(g_pub_tx.reply, "0002");
    }
    
    if( strlen ( sChnl_single_acct.ac_no1 ) >10) /*** 此种情况下,只能是账号 ***/
    {
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_1,"ac_no='%s'",sChnl_single_acct.ac_no1);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        if(sMdm_ac_rel_1.note_sts[0] != '0')
        {
            vtcp_log("%s,%d,ac_no[%s]介质状态不正常!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
            strcpy(g_pub_tx.reply,"D140");
            goto ErrExit;
        }
        iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id_1,"ac_id=%ld ",sMdm_ac_rel_1.ac_id);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_id错误!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if(sPrdt_ac_id_1.ac_id_type[0] == '1')
        {
            iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_1,"ac_id=%ld",sMdm_ac_rel_1.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sDd_mst_1.ac_sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("0680","1");/** 活期帐户 **/
        }
        else if(sPrdt_ac_id_1.ac_id_type[0] == '9')
        {
            iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst_1,"ac_id=%ld",sMdm_ac_rel_1.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sIn_mst_1.sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("0680","9");/** 内部帐户 **/
        }
        
             /*73域控制标志检查*/
        iRet=pub_com_test(cCtrlcmd,cCert_no,"",cAc_no,"","","",cGz_flag);
        vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_no,cGz_flag); 
        if (iRet)
        {
            sprintf(acErrMsg,"%s,%d,公共函数pub_com_test()调用失败",__FILE__,__LINE__);
            WRITEMSG
            goto ErrExit;	
        }
       
    }
    else
    {
        /**** 否则存在该科目  **/
        iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item_1,"acc_no = '%s'",sChnl_single_acct.ac_no1);
        if(iRet)
        {
            sprintf(acErrMsg,"查询科目失败!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
    }
    
    /*准备更新原纪录*/
    memset(&sChnl_single_acct_2,0x00,sizeof(struct chnl_single_acct_c));
    /**增加 bbmk字段 shangyongchao 2012/6/7 11:22:53
    iRet= Chnl_single_acct_Dec_Upd(g_pub_tx.reply,"pt_date=%ld and pt_trace_no='%s' ",lHx_old_date,cPt_trace_no);
    **/
    iRet= Chnl_single_acct_Dec_Upd(g_pub_tx.reply,"pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",\
            lHx_old_date,cPt_trace_no,cBbmk);
    if( iRet)
    {
        sprintf(acErrMsg,"更新单笔记账登记簿错误! iRet=[%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }
    
    iRet= Chnl_single_acct_Fet_Upd(&sChnl_single_acct_2,g_pub_tx.reply);
    if( iRet )
    {
        sprintf(acErrMsg,"得到单笔记账登记簿纪录错误!");
        WRITEMSG
        goto ErrExit;
    }
    /*判断累计金额与原金额，如果相等，退回成功；如果小于，继续退货。20111011 xk*/
     dTh_amt=sChnl_single_acct_2.th_amt;
     
     iRet=pub_base_CompDblVal(sChnl_single_acct.tx_amt,dTh_amt + dHx_amt);
     if( iRet == 0 )
     {
        sChnl_single_acct_2.sts[0]='5';  
                                   /*
                                   0:交易无效(不参与对账)
                                   1:记账成功(参与对账)
                                   2：挂帐成功(参与对账)
                                   3：挂后入账成功(参与对账)
                                   4：冲正成功（不参与对账）
                                   5：退回成功(参与对账)
                                   6: 退货标志(参与对账)
                                   */
    }
    else if ( iRet == 1 )
    {
       sChnl_single_acct_2.sts[0]='6';
       sChnl_single_acct_2.th_amt+=dHx_amt;
    }
   
    sChnl_single_acct_2.o_tx_date=g_pub_tx.tx_date;
    sprintf(cHx_trace_no,"%12ld",sChnl_single_acct.trace_no);
    strcpy(sChnl_single_acct_2.o_trace_no,cHx_trace_no); 
    sChnl_single_acct_2.o_pt_date=sChnl_single_acct.pt_date;
    
    strcpy(sChnl_single_acct_2.o_pt_trace_no,sChnl_single_acct.pt_trace_no);
    
    
    iRet = Chnl_single_acct_Upd_Upd(sChnl_single_acct_2,g_pub_tx.reply);
    if(iRet)
    {
         sprintf(acErrMsg, "修改单笔记账登记簿错误![%d]",iRet);
         WRITEMSG
         goto ErrExit;
    }
    
    Chnl_single_acct_Clo_Upd();
    /*****在记账交易里登记
    if (pub_ins_trace_log())
    {
         sprintf(acErrMsg, "登记交易流水错误!");
         WRITEMSG
         goto ErrExit;
    }
   ****/ 
    /*将这笔退回纪录登记到单笔记账登记簿中*/
    
    memset(&sChnl_single_acct_1,0x00,sizeof(struct chnl_single_acct_c));
    
    sChnl_single_acct_1.tx_date=g_pub_tx.tx_date;
    sprintf(sChnl_single_acct_1.tx_time,"%6d",g_pub_tx.tx_time);
    /*sChnl_single_acct_1.trace_no=g_pub_tx.ejfno;登记的非柜员流水号，有些商行没有柜员流水号这一概念，改为核心交易流水号 20110727 Mod by CHENGBO*/
    sChnl_single_acct_1.trace_no=g_pub_tx.trace_no;
    memcpy(sChnl_single_acct_1.cur_no,"01",2);
    memcpy(sChnl_single_acct_1.ac_no,sChnl_single_acct.ac_no,sizeof(sChnl_single_acct_1.ac_no)-1);
    memcpy(sChnl_single_acct_1.ac_no1,sChnl_single_acct.ac_no2,sizeof(sChnl_single_acct_1.ac_no1)-1);
    memcpy(sChnl_single_acct_1.ac_no2,sChnl_single_acct.ac_no1,sizeof(sChnl_single_acct_1.ac_no2)-1);
    memcpy(sChnl_single_acct_1.new_ac_no2,"",sizeof(sChnl_single_acct_1.new_ac_no2)-1);
    memcpy(sChnl_single_acct_1.new_ac_name2,"",sizeof(sChnl_single_acct_1.new_ac_name2)-1);
    memcpy(sChnl_single_acct_1.tx_code,g_pub_tx.tx_code,sizeof(sChnl_single_acct_1.tx_code)-1);
    memcpy(sChnl_single_acct_1.g_ac_no2,"",sizeof(sChnl_single_acct_1.g_ac_no2)-1);
    memcpy(sChnl_single_acct_1.tx_code,g_pub_tx.tx_code,sizeof(sChnl_single_acct_1.tx_code)-1);
    memcpy(sChnl_single_acct_1.tx_br_no,g_pub_tx.tx_br_no,sizeof(sChnl_single_acct_1.tx_br_no)-1);
    memcpy(sChnl_single_acct_1.tel,g_pub_tx.tel,sizeof(sChnl_single_acct_1.tel)-1);
    memcpy(sChnl_single_acct_1.bbmk,cBbmk,sizeof(sChnl_single_acct_1.bbmk)-1);
    memcpy(sChnl_single_acct_1.ttyn,g_pub_tx.tty,sizeof(sChnl_single_acct_1.ttyn)-1);
    sChnl_single_acct_1.pt_date=lPt_date;
    sprintf(sChnl_single_acct_1.pt_time,"%6d",g_pub_tx.tx_time);
    sprintf(cPt_trano,"%ld",lPt_trace_no); /*平台流水号*/
    memcpy(sChnl_single_acct_1.pt_trace_no,cPt_trano,sizeof(sChnl_single_acct_1.pt_trace_no)-1);
    sChnl_single_acct_1.tx_amt=dHx_amt;
    sChnl_single_acct_1.ct_ind[0]='2';
    memcpy(sChnl_single_acct_1.brf,cHx_brf,sizeof(sChnl_single_acct_1.brf)-1); 
    sChnl_single_acct_1.fee_amt=dFee_amt;
    sChnl_single_acct_1.sts[0]='1';
    sChnl_single_acct_1.dz_ind[0]='0';
    sChnl_single_acct_1.o_pt_date = sChnl_single_acct.pt_date;
    strcpy(sChnl_single_acct_1.o_pt_trace_no,sChnl_single_acct.pt_trace_no);
    
    iRet = Chnl_single_acct_Ins(sChnl_single_acct_1,g_pub_tx.reply);
    if( iRet )
    {
        sprintf(acErrMsg,"插入单笔记账登记簿错误!");
        WRITEMSG
        goto ErrExit;
    } 
    
    set_zd_data("0310",sChnl_single_acct.ac_no2);   /*借方账号*/
    set_zd_data("0300",sChnl_single_acct.ac_no1);   /*贷方账号*/
    
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
