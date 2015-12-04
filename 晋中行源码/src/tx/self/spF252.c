/***************************
*程序名称：spF252.c
*程序功能: 查询活期账户或内部帐的
           某一日期的借方发生额、
           贷方发生额、昨日余额、
           本日余额(本程序用于承德支付宝业务，其他地方要根据实际情况修改)
*完成人：CHENGBO
*完成时间： 2011-11-23
insert into tx_def values('6252','余额查询','10000000000000000000000000000000000000011110000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','4','0','3',null);
insert into tx_flow_def  values ('6252', 0, 'F252', '#$');
insert into tx_sub_def  values ('F252', '金额查询', 'spF252', '0', '0');
*
<EO099 DESC="查询日期不得大于当前日期!!"/>
***************************/
#include <stdio.h>
#include "public.h"
#include "in_mst_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_hst_c.h"
#include "prdt_ac_id_c.h"

spF252()
{
    int iRet=0;
    long lTx_date=0;
    long lMax_date=0;          /*最大交易日期-20120618*/
    long lBr_max_trace=0;
    long lBr_min_trace=0;
    long lAc_max_trace=0;      /*帐户对应的最大流水号*/
    long lAc_min_trace=0;      /*账户最小流水号*/
    char cAc_no[25];
    double dTot_dr_amt=0.00;   /*借方总金额*/
    double dTot_cr_amt=0.00;   /*贷方总金额*/
    double dBr_bal=0.00;       /*本日余额*/
    double dZr_bal=0.00;       /*昨日余额*/

    struct in_mst_hst_c sIn_mst_hst;
    struct dd_mst_hst_c sDd_mst_hst;
    struct in_mst_c sIn_mst;
    struct dd_mst_c sDd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;

    memset(&sIn_mst_hst,0x00,sizeof(struct in_mst_hst_c));
    memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(cAc_no,0x00,sizeof(cAc_no));

    pub_base_sysinit();

    get_zd_data("0300",cAc_no);
    get_zd_long("0460",&lTx_date);

    pub_base_old_acno(cAc_no);  /*新旧账号转换*/
    
    /*根据承德需求，如果传入的日期大于当前日期直接报错  20120618 */
    if( g_pub_tx.tx_date < lTx_date )
    {
        sprintf(acErrMsg,"查询日期大于当前系统日期!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D244");
        goto ErrExit;
    }
    /*END BY 20120618*/
    iRet=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no = '%s' ",cAc_no);
    if(iRet)
    {
         sprintf(acErrMsg,"查询介质账号对照表错误!");
         WRITEMSG
         strcpy(g_pub_tx.reply,"W011");
         goto ErrExit;
    }

    iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id=%ld ",sMdm_ac_rel.ac_id);
    if(iRet)
    {
         sprintf(acErrMsg,"查询产品账号对照表错误!");
         WRITEMSG
         strcpy(g_pub_tx.reply,"W013");
         goto ErrExit;
    }
    if( sPrdt_ac_id.ac_id_type[0] == '1' )
    {
         iRet=sql_max_long("dd_mst_hst","trace_no",&lBr_max_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"取最大流水号错误iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
         iRet=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lBr_max_trace,lTx_date);
         if( iRet == 100 )
         {
             memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
             /*如果没有流水就去查找交易明细最大日期的最大流水号对应的余额，并返回-20120618*/
             iRet=sql_max_long("dd_mst_hst","tx_date",&lMax_date," ac_id=%ld and tx_date <= %ld ",sMdm_ac_rel.ac_id,lTx_date);
             if( iRet )
             {
                 sprintf(acErrMsg,"取明细表最大交易日期错误iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=sql_max_long("dd_mst_hst","trace_no",&lAc_max_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lMax_date);
             if(iRet)
             {
                 sprintf(acErrMsg,"取明细表最大流水号错误iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lAc_max_trace,lMax_date);
             if( iRet == 100 )
             {
                 dBr_bal=0.00;
             }
             else if( iRet == 0 )
             {
                 dBr_bal=sDd_mst_hst.bal;
             }
             else
             {
                 sprintf(acErrMsg,"查询活期明细表错误!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"W050");
                 goto ErrExit;
             }
             /*ADD END 20120618*/
         }
         else if( iRet == 0 )
         {
             dBr_bal=sDd_mst_hst.bal;
         }
         else
         {
             sprintf(acErrMsg,"查询活期明细表错误!");
             WRITEMSG
             strcpy(g_pub_tx.reply,"W050");
             goto ErrExit;
         }

         iRet=sql_min_long("dd_mst_hst","trace_no",&lBr_min_trace,"ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"取最小流水号错误iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
         iRet=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst,"ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lBr_min_trace,lTx_date);
         if( iRet == 100 )
         {
             /*
             sDd_mst_hst.tx_amt=0.00;
             memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
             iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld ",sMdm_ac_rel.ac_id);
             if( iRet )
             {
                 sprintf(acErrMsg,"查询活期主表错误!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"K107");
                 goto ErrExit;
             }
             dZr_bal= sDd_mst.bal; 如果没有交易明细就把主表余额传过去
             */
             memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
             /*如果没有流水就去查找交易明细最大日期的最小流水号对应的交易额，并返回-20120618*/
             iRet=sql_max_long("dd_mst_hst","tx_date",&lMax_date," ac_id=%ld and tx_date <= %ld ",sMdm_ac_rel.ac_id,lTx_date);
             if( iRet )
             {
                 sprintf(acErrMsg,"取明细表最大交易日期错误iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=sql_min_long("dd_mst_hst","trace_no",&lAc_min_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lMax_date);
             if(iRet)
             {
                 sprintf(acErrMsg,"取明细表最小流水号错误iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lAc_min_trace,lMax_date);
             if( iRet == 100 )
             {
                 dZr_bal=0.00;
             }
             else if( iRet == 0 )
             {
                 if( sDd_mst_hst.add_ind[0] == '0')
                 {
                    dZr_bal=sDd_mst_hst.bal + sDd_mst_hst.tx_amt;
                 }
                 else if(sDd_mst_hst.add_ind[0] == '1')
                 {
                    dZr_bal=sDd_mst_hst.bal - sDd_mst_hst.tx_amt;
                 }
             }
             else
             {
                 sprintf(acErrMsg,"查询活期明细表错误!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"W050");
                 goto ErrExit;
             }
             /*ADD END 20120618*/
         }
         else if( iRet == 0 )
         {
             if( sDd_mst_hst.add_ind[0] == '0')
             {
                dZr_bal=sDd_mst_hst.bal + sDd_mst_hst.tx_amt;
             }
             else if(sDd_mst_hst.add_ind[0] == '1')
             {
                dZr_bal=sDd_mst_hst.bal - sDd_mst_hst.tx_amt;
             }
         }
         else
         {
             sprintf(acErrMsg,"查询活期明细表错误!");
             WRITEMSG
             strcpy(g_pub_tx.reply,"W050");
             goto ErrExit;
         }
         iRet=sql_sum_double("dd_mst_hst","tx_amt",&dTot_dr_amt,"ac_id=%ld and tx_date=%ld and add_ind='0' ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"统计借方发生额错误iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         iRet=sql_sum_double("dd_mst_hst","tx_amt",&dTot_cr_amt,"ac_id=%ld and tx_date=%ld and add_ind='1' ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"统计贷方发生额错误iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
    }
    else if( sPrdt_ac_id.ac_id_type[0] == '9' )
    {
         iRet=sql_max_long("in_mst_hst","trace_no",&lBr_max_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"取最大流水号错误iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         memset(&sIn_mst_hst,0x00,sizeof(struct in_mst_hst_c));
         iRet=In_mst_hst_Sel(g_pub_tx.reply,&sIn_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lBr_max_trace,lTx_date);
         if( iRet == 100 )
         {
             memset(&sIn_mst_hst,0x00,sizeof(struct in_mst_hst_c));
             /*如果没有流水就去查找交易明细最大日期的最大流水号对应的余额，并返回-20120618*/
             iRet=sql_max_long("in_mst_hst","tx_date",&lMax_date," ac_id=%ld and tx_date <= %ld ",sMdm_ac_rel.ac_id,lTx_date);
             if( iRet )
             {
                 sprintf(acErrMsg,"取明细表最大交易日期错误iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=sql_max_long("in_mst_hst","trace_no",&lAc_max_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lMax_date);
             if(iRet)
             {
                 sprintf(acErrMsg,"取明细表最大流水号错误iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=In_mst_hst_Sel(g_pub_tx.reply,&sIn_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lAc_max_trace,lMax_date);
             if( iRet == 100 )
             {
                 dBr_bal=0.00;
             }
             else if( iRet == 0 )
             {
                 dBr_bal=sIn_mst_hst.bal;
             }
             else
             {
                 sprintf(acErrMsg,"查询内部帐明细表错误!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"W054");
                 goto ErrExit;
             }
             /*ADD END 20120618*/
         }
         else if( iRet == 0 )
         {
             dBr_bal=sIn_mst_hst.bal;
         }
         else
         {
             sprintf(acErrMsg,"查询内部帐明细表错误!");
             WRITEMSG
             strcpy(g_pub_tx.reply,"W054");
             goto ErrExit;
         }

         iRet=sql_min_long("in_mst_hst","trace_no",&lBr_min_trace,"ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"取最小流水号错误iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         memset(&sIn_mst_hst,0x00,sizeof(struct in_mst_hst_c));
         iRet=In_mst_hst_Sel(g_pub_tx.reply,&sIn_mst_hst,"ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lBr_min_trace,lTx_date);
         if( iRet == 100 )
         {
             /*
             sIn_mst_hst.tx_amt=0.00;
             memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
             iRet=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
             if( iRet )
             {
                 sprintf(acErrMsg,"查询内部帐主表错误!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"K110");
                 goto ErrExit;
             }
             dZr_bal=sIn_mst.bal; 如果没有交易明细就把主表余额取出来
             */
             memset(&sIn_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
             /*如果没有流水就去查找交易明细最大日期的最小流水号对应的交易额，并返回-20120618*/
             iRet=sql_max_long("in_mst_hst","tx_date",&lMax_date," ac_id=%ld and tx_date <= %ld ",sMdm_ac_rel.ac_id,lTx_date);
             if( iRet )
             {
                 sprintf(acErrMsg,"取明细表最大交易日期错误iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=sql_min_long("in_mst_hst","trace_no",&lAc_min_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lMax_date);
             if(iRet)
             {
                 sprintf(acErrMsg,"取明细表最小流水号错误iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=In_mst_hst_Sel(g_pub_tx.reply,&sIn_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lAc_min_trace,lMax_date);
             if( iRet == 100 )
             {
                 dZr_bal=0.00;
             }
             else if( iRet == 0 )
             {
                 if( sIn_mst_hst.add_ind[0] == '0')
                 {
                    dZr_bal=sIn_mst_hst.bal + sIn_mst_hst.tx_amt;
                 }
                 else if(sIn_mst_hst.add_ind[0] == '1')
                 {
                    dZr_bal=sIn_mst_hst.bal - sIn_mst_hst.tx_amt;
                 }
             }
             else
             {
                 sprintf(acErrMsg,"查询内部帐明细表错误!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"W050");
                 goto ErrExit;
             }
             /*ADD END 20120618*/
         }
         else if( iRet == 0 )
         {
             if( sIn_mst_hst.add_ind[0] == '0')
             {
                 dZr_bal=sIn_mst_hst.bal + sIn_mst_hst.tx_amt;
             }
             else if(sIn_mst_hst.add_ind[0] == '1')
             {
                 dZr_bal=sIn_mst_hst.bal - sIn_mst_hst.tx_amt;
             }
         }
         else
         {
             sprintf(acErrMsg,"查询内部帐明细表错误!");
             WRITEMSG
             strcpy(g_pub_tx.reply,"W054");
             goto ErrExit;
         }

         iRet=sql_sum_double("in_mst_hst","tx_amt",&dTot_dr_amt,"ac_id=%ld and tx_date=%ld and add_ind='0' ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"统计借方发生额错误iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         iRet=sql_sum_double("in_mst_hst","tx_amt",&dTot_cr_amt,"ac_id=%ld and tx_date=%ld and add_ind='1' ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"统计贷方发生额错误iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
    }
    else
    {
         vtcp_log("[%s][%d],ac_id_type=[%c] 非内部帐和活期账户",__FILE__,__LINE__,sPrdt_ac_id.ac_id_type[0]);
         sprintf(acErrMsg,"非活期和内部帐账户");
         WRITEMSG
         goto ErrExit;
    }
    set_zd_double("0400",dBr_bal);
    set_zd_double("0410",dZr_bal);
    set_zd_double("0420",dTot_dr_amt);
    set_zd_double("0430",dTot_cr_amt);
    /*承德要求查询登记流水  20120516 chengbo*/
    strcpy(g_pub_tx.ac_no,cAc_no);
    g_pub_tx.ac_id=sMdm_ac_rel.ac_id;
    g_pub_tx.ac_seqn=1;
    strcpy(g_pub_tx.brf,"账户余额查询");
    if (pub_ins_trace_log())
    {
       sprintf(acErrMsg, "登记交易流水错误!");
       WRITEMSG
       goto ErrExit;
    }
    /*返回核心流水给平台*/
    set_zd_long("0040",g_pub_tx.trace_no);

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"查询交易状态成功!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    if(strcmp(g_pub_tx.reply,"0000")==0 || strcmp(g_pub_tx.reply,"")==0)
    {
        strcpy(g_pub_tx.reply,"D104");
    }
    sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
