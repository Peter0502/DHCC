/*******************************************************************************
* 版 本 号 :  V1.0        
* 程序作者 :  CHENGBO          
* 日    期 :  2011-07-12   
* 所属模块 :  与平台的接口程序 (联机单笔冲正交易)==二代支付专用交易
* 程序名称 :  sp7023.c
* 程序作用 :  (1)联机单笔冲正相当于行内的取消交易，不允许隔日操作。
*             (2)核心根据原平台日期和原平台流水查找并更新登记簿状
*             态，单笔冲正不新生成记录，不参与核心与平台对账。
*             (3)核心系统需要检查主帐号、交易金额等相关信息是否与
*             原交易一致。
* 函数列表 :       
*         1、pub_acct_rollback();
*         2、pub_base_sysinit();                              
* 使用注意 :                                       
* 修改记录 :                                       
*     修改作者 :                           
*     修改时间 :                                    
*     修改内容 :
*<EYY01 DESC="该账号与原账号不符!"/>
*<EYY02 DESC="冲正金额与原金额不符!"/>

insert into TX_DEF  values ('7023', '联机单笔冲正', '10000000000000000000000000000100000000010000010000000010000001000100000000000000100000000000000000000000000000000000000000000100', '1', '3', '0', '3', null);

insert into tx_sub_def  values ('7023', '联机单笔冲正', 'sp7023', '0', '0');

insert into TX_FLOW_DEF values ('7023', 0, '7023', '#=0001@1@#=0002@7@#=0003@3@#=0004@8@#=0005@5@#=0006@9@#=0007@6@#=0008@10@#$');
insert into TX_FLOW_DEF values ('7023', 1, 'D003', '#=0000@2@#$');
insert into TX_FLOW_DEF values ('7023', 2, 'M002', '#=0000@16@#$');
insert into TX_FLOW_DEF values ('7023', 3, 'D099', '#=0000@4@#$');
insert into TX_FLOW_DEF values ('7023', 4, 'M001', '#=0000@16@#$');
insert into TX_FLOW_DEF values ('7023', 5, 'A016', '#=0000@2@#$');
insert into TX_FLOW_DEF values ('7023', 6, 'A017', '#=0000@4@#$');
insert into TX_FLOW_DEF values ('7023', 7, 'D003', '#=0000@11@#$');
insert into TX_FLOW_DEF values ('7023', 8, 'D099', '#=0000@11@#$');
insert into TX_FLOW_DEF values ('7023', 9, 'A016', '#=0000@11@#$');
insert into TX_FLOW_DEF values ('7023', 10, 'A017', '#=0000@11@#$');
insert into TX_FLOW_DEF values ('7023', 11, 'NULL', '#=0006@13@#=0007@12@#=0008@15@#=0009@14@#$');
insert into TX_FLOW_DEF values ('7023', 12, 'D003', '#=0000@16@#$');
insert into TX_FLOW_DEF values ('7023', 13, 'D099', '#=0000@16@#$');
insert into TX_FLOW_DEF values ('7023', 14, 'A016', '#=0000@16@#$');
insert into TX_FLOW_DEF values ('7023', 15, 'A017', '#=0000@16@#$');
insert into TX_FLOW_DEF values ('7023', 16, 'F059', '#=0000@17@#$');
insert into TX_FLOW_DEF values ('7023', 17, 'F000', '#$');

insert into TX_SUB_REL values ('7023', 'A016', '1', '01', '1204', '币种');
insert into TX_SUB_REL values ('7023', 'A016', '0', '0680', '1205', '转账');
insert into TX_SUB_REL values ('7023', 'A016', '0', '0810', '120A', '摘要');
insert into TX_SUB_REL values ('7023', 'A017', '1', '01', '1214', '币种');
insert into TX_SUB_REL values ('7023', 'A017', '0', '0680', '1215', '现转标志');
insert into TX_SUB_REL values ('7023', 'A017', '0', '0810', '121A', '摘要');
insert into TX_SUB_REL values ('7023', 'D003', '1', '1', '0191', '控制标志');
insert into TX_SUB_REL values ('7023', 'D003', '1', '0000', '0192', '控制标志');
insert into TX_SUB_REL values ('7023', 'D003', '1', '1001', '0193', '控制标志');
insert into TX_SUB_REL values ('7023', 'D003', '1', '1', '1022', '账户序号');
insert into TX_SUB_REL values ('7023', 'D003', '1', '01', '102J', '币种');
insert into TX_SUB_REL values ('7023', 'D003', '0', '0680', '102K', '现转标志');
insert into TX_SUB_REL values ('7023', 'D003', '0', '0810', '102Y', '摘要');
insert into TX_SUB_REL values ('7023', 'D099', '1', '1', '0191', '控制标志');
insert into TX_SUB_REL values ('7023', 'D099', '1', '0000', '0192', '控制标志');
insert into TX_SUB_REL values ('7023', 'D099', '1', '1001', '0193', '普通存取');
insert into TX_SUB_REL values ('7023', 'D099', '1', '1', '1012', '序号');
insert into TX_SUB_REL values ('7023', 'D099', '0', '0810', '1016', '摘要');
insert into TX_SUB_REL values ('7023', 'D099', '1', '01', '101A', '币种');
insert into TX_SUB_REL values ('7023', 'D099', '0', '0680', '101B', '现转');
insert into TX_SUB_REL values ('7023', 'M001', '1', '01', '1181', '币种');
insert into TX_SUB_REL values ('7023', 'M001', '0', '0810', '1187', '摘要');
insert into TX_SUB_REL values ('7023', 'M001', '0', '0300', '1188', '对方账号');
insert into TX_SUB_REL values ('7023', 'M002', '1', '01', '1191', '币种');
insert into TX_SUB_REL values ('7023', 'M002', '0', '0810', '1197', '摘要');
insert into TX_SUB_REL values ('7023', 'M002', '0', '0300', '1198', '对方账号');
insert into TX_SUB_REL values ('7023', 'NULL', '0', '0700', '0120', '配置分支');

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "chnl_single_cnaps_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "in_mst_c.h"
#include "dd_mst_c.h"
#include "com_item_c.h"

sp7023()
{
    int iRet=0;
    int i=0;
    long lPt_old_trace=0;  /*原平台流水 long型*/
    long lPt_old_date=0;   /*原平台交易日期*/
    long lPt_cz_date=0;    /*平台冲正日期*/
    long lPt_cz_trace=0;   /*平台冲正流水 long型*/
    char cHx_ac_no[25];    /*主账号*/
    char cHx_ac_no1[25];   /*第二账号*/
    char cHx_dc_ind[2];    /*借贷标志：1-借，2-贷*/
    char cHx_ct_ind[2];    /*现转标志：1-现金，2-转账*/
    char cHx_sjct_ind[2];  /*实际现转标志：1-现金，2-转账*/
    char cHx_msr2[38];     /*二磁道信息*/
    char cHx_msr3[105];    /*三磁道信息*/
    char cHx_brf[51];      /*记账摘要*/
    char cPt_old_trace[13]; /*原平台流水 char型*/
    char cPt_cz_trace[13];  /*平台冲正流水 char型*/
    char cO_pt_trace_no[13];/*平台流水*/
    char cHx_tel[7];       /*操作员*/
    char cFee_flag[2];     /*收费标志 0-不收 1-收费*/
    char cFd123[1000];     /*接收123域里的收费信息*/
    long lFee_cnt=0;     /*收费条数*/
    char cFee_cnt[3];    /*收费条数*/
    char cFee_amt[13];   /*收费金额*/
    char cFee_dc_ind[2]; /*收费借贷标志 1 借2 贷*/
    double dFee_amt = 0.00; /*手续费金额*/
    double dFee_amt1=0.00;
    double dHx_amt=0.00;   /*交易金额*/
    double dCif_fee=0.0;
    char cCz_flag[2]; /*冲正种类 1 撤销冲正 其他消费冲正*/
    char sFee_acno[32]={0};
    char cBbmk[5];/*系统种类*/
    struct trace_log_c         sTrace_log;
    struct chnl_single_cnaps_c  stChnlCnaps;
    struct mdm_ac_rel_c        g_mdm_ac_rel;
    struct prdt_ac_id_c        sPrdt_ac_id;
    struct in_mst_c            sIn_mst;
    struct dd_mst_c            sDd_mst;
    struct com_item_c          sCom_item;
    
    memset(cHx_ac_no,0,sizeof(cHx_ac_no));
    memset(cHx_ac_no1,0,sizeof(cHx_ac_no1));
    memset(cHx_dc_ind,0,sizeof(cHx_dc_ind));
    memset(cHx_ct_ind,0,sizeof(cHx_ct_ind));
    memset(cHx_sjct_ind,0,sizeof(cHx_sjct_ind));
    memset(cFee_flag,0,sizeof(cFee_flag));
    memset(cFd123,0,sizeof(cFd123));
    memset(cFee_cnt,0,sizeof(cFee_cnt));
    memset(cFee_amt,0,sizeof(cFee_amt));
    memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind));
    memset(cHx_msr2,0,sizeof(cHx_msr2));
    memset(cHx_msr3,0,sizeof(cHx_msr3));
    memset(cHx_brf,0,sizeof(cHx_brf));
    memset(cPt_old_trace,0,sizeof(cPt_old_trace));
    memset(cHx_tel,0,sizeof(cHx_tel));
    memset(cO_pt_trace_no,0,sizeof(cO_pt_trace_no));
    memset(&sTrace_log,0,sizeof(struct trace_log_c));
    memset(&stChnlCnaps,0,sizeof(struct chnl_single_cnaps_c));
    memset(&g_mdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
    memset(&sIn_mst,0,sizeof(struct in_mst_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sCom_item,0x00,sizeof(struct com_item_c));
    memset(cCz_flag,0x00,sizeof(cCz_flag));
    memset(cBbmk,0x00,sizeof(cBbmk));
    /* 初始化结构 */
    pub_base_sysinit();
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    
    get_zd_data("0070",cHx_tel); 
    get_zd_long("0780",&lPt_old_trace);
    get_zd_long("0450",&lPt_old_date);
    get_zd_long("0440",&lPt_cz_date);
    get_zd_long("0520",&lPt_cz_trace);
    get_zd_data("0300",cHx_ac_no);
    get_zd_data("0310",cHx_ac_no1);
    get_zd_data("0660",cHx_dc_ind);
    get_zd_data("0680",cHx_ct_ind);
    get_zd_data("0710",cFee_flag);
    get_zd_data("0720",cHx_sjct_ind);
    get_zd_data("0750",cHx_msr2);
    get_zd_data("0760",cHx_msr3);
    get_zd_data("0810",cHx_brf);
    get_zd_double("0400",&dHx_amt);
    get_fd_data("1230",cFd123);
    get_zd_data("0100",cBbmk);
    get_zd_data("0700",cCz_flag);
    char sys73[16] = {0};
    get_zd_data("0730",sys73);
    
    /*新旧账号转换*/
    pub_base_old_acno(cHx_ac_no);
    pub_base_old_acno(cHx_ac_no1); 
    vtcp_log("[%s][%d],lPt_old_trace=[%ld]",__FILE__,__LINE__,lPt_old_trace);
    sprintf(cPt_old_trace,"%d",lPt_old_trace);  /*变量类型转换,long型--->char型*/
    vtcp_log("[%s][%d],cPt_old_trace=[%s]",__FILE__,__LINE__,cPt_old_trace);
    vtcp_log("[%s][%d],现转标志为=[%s]",__FILE__,__LINE__,cHx_ct_ind);
    
    memset(&stChnlCnaps,0x00,sizeof(struct chnl_single_cnaps_c));
    /**增加bbmk 字段 shangyongchao  2012/6/7 11:14:54
    iRet=Chnl_single_cnaps_Sel(g_pub_tx.reply,&stChnlCnaps,"pt_date=%ld and pt_trace_no='%s'",lPt_old_date,cPt_old_trace);
    Modefied by YangMeng 2014/7/18 9:51:36 修改查询条件**/
    
    if ( sys73[9] == '3' ) /**二代特有的选择项 Modefied by liuzengying 2014/7/18 10:54:46 修改查询条件**/
    {
		iRet=Chnl_single_cnaps_Sel(g_pub_tx.reply,&stChnlCnaps,"chnl_type='%c' and bbmk = '%s' and pt_date=%ld and pt_trace_no='%s'",sys73[9], cBbmk, lPt_old_date, cPt_old_trace);
	}
	else
	{
		iRet=Chnl_single_cnaps_Sel(g_pub_tx.reply,&stChnlCnaps,"bbmk = '%s' and pt_date=%ld and pt_trace_no='%s'", cBbmk, lPt_old_date, cPt_old_trace);
	}
		
    if (iRet)
    {
        strcpy(g_pub_tx.reply, "D103");
        sprintf(acErrMsg, "查询要被撤销的交易记录错误!");
        WRITEMSG
        goto ErrExit;
    }
    
    if (stChnlCnaps.sts[0] == '4')  /*4-已做冲正*/
    {
        strcpy(g_pub_tx.reply, "CU22");
        sprintf(acErrMsg, "该记录已做冲正!");
        WRITEMSG
        goto ErrExit;
    }
    
    if (stChnlCnaps.sts[0] == '0')  /*0-交易无效*/
    {
        strcpy(g_pub_tx.reply, "LS12");
        sprintf(acErrMsg, "该记录为无效交易!");
        WRITEMSG
        goto ErrExit;    
    }
    vtcp_log("[%s][%d],stChnlCnaps.ac_no=[%s]",__FILE__,__LINE__,stChnlCnaps.ac_no);
    vtcp_log("[%s][%d],cHx_ac_no=[%s]",__FILE__,__LINE__,cHx_ac_no);
    /*
        if (strcmp(stChnlCnaps.ac_no,cHx_ac_no) != 0)
        {
            strcpy(g_pub_tx.reply, "YY01");
            sprintf(acErrMsg, "该账号与原账号不符!");
            WRITEMSG
            goto ErrExit;
        }Deleted by YangMeng 2014/7/18 9:55:20 如果是现金汇款冲账30域的账号和记账时登记的账号不相符，不做检查*/
    if (pub_base_CompDblVal(stChnlCnaps.tx_amt,dHx_amt) !=0 )
    {
        strcpy(g_pub_tx.reply, "YY02");
        sprintf(acErrMsg, "冲正金额与原金额不符!");
        WRITEMSG
        vtcp_log("[%s][%d],stChnlCnaps.tx_amt=[%lf] dHx_amt=[%lf]",__FILE__,__LINE__,stChnlCnaps.tx_amt,dHx_amt);
        goto ErrExit;
    }
    /*判断收费金额是否正确    开始*/
    vtcp_log("[%s][%d],收费标志=[%s]",__FILE__,__LINE__,cFee_flag);
    vtcp_log("[%s][%d],cFd123=[%s]",__FILE__,__LINE__,cFd123);
    if(cFee_flag[0] == '0')
    {
      stChnlCnaps.fee_amt=0.00;
    }
    else if( cFee_flag[0] == '1')
    {
        memcpy(cFee_cnt,cFd123+1,2);
        vtcp_log("[%s][%d],cFee_cnt=[%s]",__FILE__,__LINE__,cFee_cnt);
        lFee_cnt=atoi(cFee_cnt);
        vtcp_log("[%s][%d],lFee_cnt=[%d]",__FILE__,__LINE__,lFee_cnt);
        i=0;
        dFee_amt1=0.00;
        for(i=0;lFee_cnt > i; i++)
        {
             memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind));
             memset(cFee_amt,0x00,sizeof(cFee_amt));
             memset(sFee_acno,0x00,sizeof(sFee_acno));
             memcpy(sFee_acno,cFd123+3 + 48 * i,24);
             memcpy(cFee_dc_ind,cFd123+27+48*i,1);
             memcpy(cFee_amt,cFd123+29 + 48 * i,12);
             vtcp_log("[%s][%d],cFee_dc_ind=[%s]",__FILE__,__LINE__,cFee_dc_ind);
             vtcp_log("[%s][%d],cFee_amt=[%s]",__FILE__,__LINE__,cFee_amt);
             dFee_amt1=atof(cFee_amt) / 100.00;	/**modify 20110822: 金额以分为单位**/
             vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt1);
             if(cFee_dc_ind[0] == '2')
             {
                dFee_amt=dFee_amt + dFee_amt1;
             }
             
             vtcp_log("[%s][%d],sFee_acno=[%s] and cHx_ac_no=[%s] ",__FILE__,__LINE__,sFee_acno,cHx_ac_no); 
             zip_space(sFee_acno);
             if(!memcmp(sFee_acno,cHx_ac_no,strlen(cHx_ac_no)))
             {
             		dCif_fee=dFee_amt1;
             }
             /*现金手续费不冲账*/
             /**if(cFd123[0] == '1')
             {
             		dCif_fee=dFee_amt1;
             }*/
             
        }
        vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);             
    }
    else
    {
        sprintf(acErrMsg, "收费标志错误!");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
    vtcp_log("[%s][%d],dCif_fee=[%lf]",__FILE__,__LINE__,dCif_fee);
    vtcp_log("[%s][%d],stChnlCnaps.fee_amt=[%lf]",__FILE__,__LINE__,stChnlCnaps.fee_amt);
    if (pub_base_CompDblVal(stChnlCnaps.fee_amt,dCif_fee) !=0 )
    {
        strcpy(g_pub_tx.reply, "B296");
        sprintf(acErrMsg, "手续费金额不符!");
        WRITEMSG
        goto ErrExit;
    }
    
    vtcp_log("[%s][%d] tx_date=[%d],Pt_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date,lPt_old_date);
    zip_space(cHx_ac_no);
    zip_space(stChnlCnaps.ac_no2);
    
    if(strlen(cHx_ac_no) > 10)   /**账号*/
    {
         memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
         iRet= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",cHx_ac_no);
         if( iRet )
         {
              vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,stChnlCnaps.ac_no1);
              strcpy(g_pub_tx.reply,"P102");
              goto ErrExit;
         }
         memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
         iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
         if (iRet)
         {
              vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,stChnlCnaps.ac_no1);
              strcpy(g_pub_tx.reply,"K106");
              goto ErrExit;
         }
         dHx_amt = -1 * dHx_amt;   /*冲回记负数回去*/
         
         if(sPrdt_ac_id.ac_id_type[0] == '1')  /*活期*/
         {    
              memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
              iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
              if (iRet)
              {
                vtcp_log("%s,%d,ac_id[%s]查找错误!",__FILE__,__LINE__,sDd_mst.ac_id);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
              }
              if(sDd_mst.ac_sts[0] != '1')
              {
                vtcp_log("%s,%d,ac_no该账户不正常!",__FILE__,__LINE__);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
              }
               
              if(cHx_dc_ind[0] == '1')   /*1-借*/
              {
                  if( cHx_ct_ind[0] == '1')   /*现金*/ 
                  {                                    
                      set_zd_data("1021",cHx_ac_no);
                      set_zd_double("102F",dHx_amt);
                      strcpy(g_pub_tx.reply,"0001");
                  }
                  else if( cHx_ct_ind[0] == '2')  /*转账*/
                  {
                     set_zd_data("1021",cHx_ac_no);
                     set_zd_double("102F",dHx_amt);
                     strcpy(g_pub_tx.reply,"0002");
                  }
                  else
                  {
                      sprintf(acErrMsg,"现转标志错误!!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply, "B099");
                      goto ErrExit;
                  }
              }
              else if(cHx_dc_ind[0] == '2')   /*2-贷*/
              {
                  if( cHx_ct_ind[0] == '1')   /*现金*/ 
                  {                                    
                      set_zd_data("1011",cHx_ac_no);
                      set_zd_double("1013",dHx_amt);
                      strcpy(g_pub_tx.reply,"0003");
                  }
                  else if( cHx_ct_ind[0] == '2')  /*转账*/
                  {
                     set_zd_data("1011",cHx_ac_no);
                     set_zd_double("1013",dHx_amt);
                     strcpy(g_pub_tx.reply,"0004");
                  }
                  else
                  {
                      sprintf(acErrMsg,"现转标志错误!!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply, "B099");
                      goto ErrExit;
                  }
              }
              else
              {
                sprintf(acErrMsg,"借贷标志错误!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "P404");
                goto ErrExit;
              }
         }
         else if(sPrdt_ac_id.ac_id_type[0] == '9')  /*内部*/
         {
              vtcp_log("[%s][%d] 跟踪看看1",__FILE__,__LINE__);
              memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
              iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
              if (iRet)
              {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cHx_ac_no);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
              }
              if(sIn_mst.sts[0] != '1')
              {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cHx_ac_no);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
              }
              
              if(cHx_dc_ind[0] == '1')   /*1-借*/
              {
                vtcp_log("[%s][%d] 跟踪看看2",__FILE__,__LINE__);
                if( cHx_ct_ind[0] == '1')   /*现金*/ 
                {                                    
                      set_zd_data("1201",cHx_ac_no);
                      set_zd_double("1208",dHx_amt);
                      strcpy(g_pub_tx.reply,"0005");
                }
                else if( cHx_ct_ind[0] == '2')  /*转账*/
                {
                     vtcp_log("[%s][%d] 跟踪看看3",__FILE__,__LINE__);
                     set_zd_data("1201",cHx_ac_no);
                     set_zd_double("1208",dHx_amt);
                     strcpy(g_pub_tx.reply,"0006");
                }
                else
                {
                      sprintf(acErrMsg,"现转标志错误!!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply, "B099");
                      goto ErrExit;
                } 
              }
              else if(cHx_dc_ind[0] == '2')   /*2-贷*/
              {
                if( cHx_ct_ind[0] == '1')   /*现金*/ 
                {                                    
                      set_zd_data("1211",cHx_ac_no);
                      set_zd_double("1218",dHx_amt);
                      strcpy(g_pub_tx.reply,"0007");
                }
                else if( cHx_ct_ind[0] == '2')  /*转账*/
                {
                     set_zd_data("1211",cHx_ac_no);
                     set_zd_double("1218",dHx_amt);
                     strcpy(g_pub_tx.reply,"0008");
                }
                else
                {
                      sprintf(acErrMsg,"现转标志错误!!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply, "B099");
                      goto ErrExit;
                }
              }
              else
              {
                sprintf(acErrMsg,"借贷标志错误!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "P404");
                goto ErrExit;
              }
         }
    }
    else if( strlen(cHx_ac_no)  <= 10)
    {    
         memset(&sCom_item,0x00,sizeof(struct com_item_c));
         iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cHx_ac_no);
         if(iRet)
         {
           sprintf(acErrMsg,"查询科目失败!!!");
           WRITEMSG
           strcpy(g_pub_tx.reply, "B172");
           goto ErrExit;
         }
            /**add by lw 20110920**/
            dHx_amt = -1 * dHx_amt;   /*冲回记负数回去*/
            /**end by lw 20110920**/
         
         if(cHx_dc_ind[0] == '1')   /*1-借*/
         {
              if( cHx_ct_ind[0] == '1')   /*现金*/ 
              {                                    
                    set_zd_data("1201",cHx_ac_no);
                    set_zd_double("1208",dHx_amt);
                    strcpy(g_pub_tx.reply,"0005");
              }
              else if( cHx_ct_ind[0] == '2')  /*转账*/
              {
                   set_zd_data("1201",cHx_ac_no);
                   set_zd_double("1208",dHx_amt);
                   strcpy(g_pub_tx.reply,"0006");
              }
              else
              {
                  sprintf(acErrMsg,"现转标志错误!!!");
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "B099");
                  goto ErrExit;
              } 
         }
         else if(cHx_dc_ind[0] == '2')   /*2-贷*/
         {
              if( cHx_ct_ind[0] == '1')   /*现金*/ 
              {                                    
                    set_zd_data("1211",cHx_ac_no);
                    set_zd_double("1218",dHx_amt);
                    strcpy(g_pub_tx.reply,"0007");
              }
              else if( cHx_ct_ind[0] == '2')  /*转账*/
              {
                   set_zd_data("1211",cHx_ac_no);
                   set_zd_double("1218",dHx_amt);
                   strcpy(g_pub_tx.reply,"0008");
              }
              else
              {
                  sprintf(acErrMsg,"现转标志错误!!!");
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "B099");
                  goto ErrExit;
              }
         }
         else
         {
           sprintf(acErrMsg,"借贷标志错误!!!");
           WRITEMSG
           strcpy(g_pub_tx.reply, "P404");
           goto ErrExit;
         }
    }
    else if( strlen(cHx_ac_no)==0)
    {
         sprintf(acErrMsg,"未传入主账号!");
         WRITEMSG
         strcpy(g_pub_tx.reply, "AT08");
         goto ErrExit;
    }
    
    if( cHx_ct_ind[0] == '1' )      /*现金记账*/
    {
        if(cHx_dc_ind[0] == '1')         /*1-借*/
        {
    
          set_zd_double("1194",dHx_amt);
        }
        else if(cHx_dc_ind[0] == '2')   /*2-贷*/
        {
    
          set_zd_double("1184",dHx_amt);
        }
        else
        {
          sprintf(acErrMsg,"借贷标志错误!!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "P404");
          goto ErrExit;
        }
    }
    else if( cHx_ct_ind[0] == '2' )      /*转账记账*/
    {
        zip_space(cHx_ac_no1);
        if(strlen(cHx_ac_no1) > 10)     /**第二账号作为对方账号 20110803*/
        {    
             memset( &g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
             iRet= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",cHx_ac_no1);
             if( iRet )
             {
                  vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,cHx_ac_no1);
                  strcpy(g_pub_tx.reply,"P102");
                  goto ErrExit;
             }
             memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
             iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
             if (iRet)
             {
                  vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cHx_ac_no1);
                  strcpy(g_pub_tx.reply,"K106");
                  goto ErrExit;
             }
             if(sPrdt_ac_id.ac_id_type[0] == '1')  /*活期*/
             {
                  memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
                  iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
                  if (iRet)
                  {
                       vtcp_log("%s,%d,ac_id[%s]查找错误!",__FILE__,__LINE__,sDd_mst.ac_id);
                       strcpy(g_pub_tx.reply,"P102");
                       goto ErrExit;
                  }
                  if(sDd_mst.ac_sts[0] != '1')
                  {
                       vtcp_log("%s,%d,ac_no该账户不正常!",__FILE__,__LINE__);
                       strcpy(g_pub_tx.reply,"O085");
                       goto ErrExit;
                  }
                  
                  if(cHx_dc_ind[0] == '1')         /*1-借*/
                  {    
                       
                       set_zd_data("1011",cHx_ac_no1);
                       set_zd_double("1013",dHx_amt);
                       set_zd_data("0700","6");     /*D099*/
                  }
                  else if(cHx_dc_ind[0] == '2')   /*2-贷*/
                  {
                       set_zd_data("1021",cHx_ac_no1);
                       set_zd_double("102F",dHx_amt);
                       set_zd_data("0700","7");     /*D003*/
                  }
                  else
                  {
                       sprintf(acErrMsg,"借贷标志错误!!!");
                       WRITEMSG
                       strcpy(g_pub_tx.reply, "P404");
                       goto ErrExit;
                  }
             }
             else if(sPrdt_ac_id.ac_id_type[0] == '9')  /*内部*/
             {    
                  memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
                  iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
                  if (iRet)
                  {
                       vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cHx_ac_no1);
                       strcpy(g_pub_tx.reply,"P102");
                       goto ErrExit;
                  }
                  if(sIn_mst.sts[0] != '1')
                  {
                       vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cHx_ac_no1);
                       strcpy(g_pub_tx.reply,"O085");
                       goto ErrExit;
                  }
                  if(cHx_dc_ind[0] == '1')   /*1-借*/
                  {
                       set_zd_data("1211",cHx_ac_no1);
                       set_zd_double("1218",dHx_amt);
                       set_zd_data("0700","8");    /*A017*/
                  }
                  else if(cHx_dc_ind[0] == '2')   /*2-贷*/
                  {
                       set_zd_data("1201",cHx_ac_no1);
                       set_zd_double("1208",dHx_amt);
                       set_zd_data("0700","9");   /*A016*/
                  }
                  else
                  {
                       sprintf(acErrMsg,"借贷标志错误!!!");
                       WRITEMSG
                       strcpy(g_pub_tx.reply, "P404");
                       goto ErrExit;
                  }
             }
        }
        else if(strlen(cHx_ac_no1) <= 10)
        {     
              memset(&sCom_item,0x00,sizeof(struct com_item_c));
              iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cHx_ac_no1);
              if(iRet)
              {
                    sprintf(acErrMsg,"查询科目失败!!!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "B172");
                    goto ErrExit;
              }
              if(cHx_dc_ind[0] == '1')   /*1-借*/
              {
                   set_zd_data("1211",cHx_ac_no1);
                   set_zd_double("1218",dHx_amt);
                   set_zd_data("0700","8");    /*A017*/
              }
              else if(cHx_dc_ind[0] == '2')   /*2-贷*/
              {
                   set_zd_data("1201",cHx_ac_no1);
                   set_zd_double("1208",dHx_amt);
                   set_zd_data("0700","9");   /*A016*/
              }
              else
              {
                   sprintf(acErrMsg,"借贷标志错误!!!");
                   WRITEMSG
                   strcpy(g_pub_tx.reply, "P404");
                   goto ErrExit;
              }
         }  
         else if( strlen(cHx_ac_no1)==0)
         {  
              sprintf(acErrMsg,"未传入商户账号或待清算账号!");
              WRITEMSG
              strcpy(g_pub_tx.reply, "AT08");
              goto ErrExit;
         }
    }
    /*开始更新单笔记账登记簿（chnl_single_cnaps）*/
    
     memset(&stChnlCnaps,0,sizeof(struct chnl_single_cnaps_c));
     /**增加bbmk 字段 shangyongchao 2012/6/7 11:17:07
     iRet=Chnl_single_cnaps_Dec_Upd(g_pub_tx.reply," pt_date = %ld and pt_trace_no= '%s'",lPt_old_date,cPt_old_trace);
     **/
     iRet=Chnl_single_cnaps_Dec_Upd(g_pub_tx.reply," pt_date = %ld and pt_trace_no= '%s' and bbmk='%s'",\
        lPt_old_date,cPt_old_trace,cBbmk);
     if (iRet) 
     {
         sprintf(acErrMsg, "执行Chnl_single_cnaps_Dec_Upd错[%d]", iRet);
         WRITEMSG
         goto ErrExit;
     }
     
     
     iRet = Chnl_single_cnaps_Fet_Upd(&stChnlCnaps,g_pub_tx.reply);
     if (iRet) 
     {
         sprintf(acErrMsg, "执行Chnl_single_cnaps_Fet_Upd 错[%d]", iRet);
         WRITEMSG
         goto ErrExit;
     }
     stChnlCnaps.sts[0]='4'; /*
                                     0:交易无效(不参与对账)
                                     1:记账成功(参与对账)
                                     2：挂帐成功(参与对账)
                                     3：挂后入账成功(参与对账)
                                     4：冲正成功（不参与对账）
                                     5：退回成功(参与对账)
                                   */
     /*stChnlCnaps.o_pt_date=lPt_cz_date;   平台日期
     sprintf(cO_pt_trace_no,"%12ld",lPt_cz_trace);
     strcpy(stChnlCnaps.o_pt_trace_no,cO_pt_trace_no); 平台流水*/
     iRet=Chnl_single_cnaps_Upd_Upd(stChnlCnaps,g_pub_tx.reply);
     if (iRet)
     {
           sprintf(acErrMsg, "修改单笔记账登记簿错误![%d]",iRet);
           WRITEMSG
           goto ErrExit;
     }
     Chnl_single_cnaps_Clo_Upd();
     
     if(cCz_flag[0] == '1')
     {
        /**增加bbmk 字段 shangyongchao 2012/6/7 11:19:32
        iRet = sql_execute("update chnl_single_cnaps set sts ='1' where pt_date =%ld and pt_trace_no ='%s'",\
        stChnlCnaps.o_pt_date,stChnlCnaps.o_pt_trace_no);
        **/
        iRet = sql_execute("update chnl_single_cnaps set sts ='1' where pt_date =%ld and pt_trace_no ='%s' and bbmk='%s'",\
			stChnlCnaps.o_pt_date,stChnlCnaps.o_pt_trace_no,stChnlCnaps.bbmk);
        if(iRet)
        {
            sprintf(acErrMsg, "修改单笔记账登记簿错误![%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
     }
    set_zd_long("0040",g_pub_tx.trace_no); 	/*将核心流水传给平台*/ 
	set_zd_long("0460",g_pub_tx.tx_date);	/*Added by YangMeng 2014/7/18 9:58:37 将核心日期传给平台*/
   
    
OkExit:
       sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
       WRITEMSG
               set_zd_data(DC_REPLY, g_pub_tx.reply);
       return 0;
ErrExit:
       if(memcmp(g_pub_tx.reply,"0000",4) == 0)
       {
           strcpy(g_pub_tx.reply, "H034");
       }
       sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
       WRITEMSG
               set_zd_data(DC_REPLY, g_pub_tx.reply);
       return 1;
}

