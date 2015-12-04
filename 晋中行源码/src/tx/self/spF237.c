/*************************************************
* 文 件 名:  spF237.c
* 功能描述： 预授权（冻结）交易
*
* 作    者:  史立芝
* 完成日期： 2011年7月日

*--预授权交易，bit_map尚未修改，log_ind=1 :记录通讯日志，tx_type=4:其他，wrk_sts=0:不限制签到，
insert into tx_def
values ('6237', '预授权交易', '10000000000000000000000000000000000000001000000010000000000000000000000000000000000000000000000000000000000000000000000000000100', '1', '6', '0', '3', null);

--保留原来的6203交易，添加新交易流程6237
insert into tx_flow_def 
values ('6237', 0, 'F237', '#$');


--添加子交易,acct_ind,chnl_ind 同F036子交易
insert into tx_sub_def 
values ('F237', '预授权交易', 'spF237', '0', '0');

*  修改记录：
*   日   期:2011年8月2日
*   修 改 人:史立芝
*   修改内容:预授权号由原来的平台流水号FD52 改为 FD87 
*   日   期:2011年8月4日
*   修 改 人:史立芝
*   修改内容:增加check_ky_bal() 检查可用余额 
*      日    期:20110816
*      修 改 人：薛珂
*      修改内容:pub_com_test.c()控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_hold_c.h"
#include "card.h"
#include "card_reg_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"

#define DEBUG_spF237 1
/* 注意：
 因为无法测试卡账户检查，所以暂时封住了，
 在有卡账户的系统，将DEBUG_spF237 宏定义为 1
 */
int Return_bal(char *dc_ind); /*查询活期余额*/
int check_ky_bal();/*检查可用余额是否足够*/

int spF237()
{
    int ret=0;
    char cDc_ind[2];/*借贷标志：1借，2贷*/
    char cMsr_str2[38];/*2磁道信息*/
    char cMsr_str3[142];/*3磁道信息*/
    char cCtrlcmd[11];/*控制标志*/
    char cTtyn[21]; /*终端号*/
    char cTel[7];
    char cBrf[51];/*摘要*/
    long lPt_date=0;/*平台日期*/
    char cPt_time[7];/*平台时间*/
    char lPt_trace_no[13];/*平台流水*/
    char cYsq_no[11];/*预授权号 */
    /*调用pub_com_test函数时，接收返回值*/
    char cGz_flag[2];/*挂账标志*/
    struct chnl_single_hold_c sChnl_single_hold;    
    memset(&sChnl_single_hold,0x0,sizeof(struct chnl_single_hold_c));
    memset(cDc_ind,0,sizeof(cDc_ind)); 
    memset(cMsr_str2,0,sizeof(cMsr_str2));
    memset(cMsr_str3,0,sizeof(cMsr_str3));
    memset(cCtrlcmd,0,sizeof(cCtrlcmd));
    memset(cBrf,0,sizeof(cBrf));
    memset(cTel,0,sizeof(cTel));
    /*调用pub_com_test时用到*/
    memset(cGz_flag,0x00,sizeof(cGz_flag));
    strcpy(g_pub_tx.cur_no,"01"); /*币种*/
    
    if(pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    
    get_zd_data("0070",cTel);/*操作员号*/
    get_zd_data("0100",cTtyn); /*终端号*/
    get_zd_data("0660",cDc_ind);/*借贷标志：1借，2贷*/
    get_zd_data("0750",cMsr_str2);/*磁道2信息*/
    get_zd_data("0760",cMsr_str3);/*磁道3信息*/
    get_zd_data("0730",cCtrlcmd);/*控制标志*/
    get_zd_data("0810",cBrf);/*摘要*/
    get_zd_long("0440",&lPt_date);/*平台日期*/
    get_zd_data("0170",cPt_time); /*平台时间*/
    get_zd_data("0520",lPt_trace_no);/*平台流水*/
    get_zd_data("0870",cYsq_no);/*预授权号*/
    get_zd_data("0300",g_pub_tx.crd_no);/*卡号*/
    get_zd_double("0400",&g_pub_tx.tx_amt1);/*控制金额*/
    /*
    get_zd_data("0790",g_pub_tx.draw_pwd);交易密码
    */
    get_zd_data("0730",cCtrlcmd);
    
    vtcp_log("[%s],[%d]slzh123",__FILE__,__LINE__);
    vtcp_log("[%s][%d]借贷标志=[%s]",__FILE__,__LINE__,cDc_ind);
    vtcp_log("[%s][%d]终端号=[%s]",__FILE__,__LINE__,cTtyn);
    vtcp_log("[%s][%d]2磁道=[%s]",__FILE__,__LINE__,cMsr_str2);
    vtcp_log("[%s][%d]3磁道=[%s]",__FILE__,__LINE__,cMsr_str3);
    vtcp_log("[%s][%d]控制标志=[%s]",__FILE__,__LINE__,cCtrlcmd);
    vtcp_log("[%s][%d]摘要=[%s]",__FILE__,__LINE__,cBrf);
    vtcp_log("[%s][%d]平台日期=[%ld]",__FILE__,__LINE__,lPt_date);
    vtcp_log("[%s][%d]平台流水=[%s]",__FILE__,__LINE__,lPt_trace_no);
    vtcp_log("[%s][%d]预授权号=[%s]",__FILE__,__LINE__,cYsq_no);
    vtcp_log("[%s][%d]卡号=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
    vtcp_log("[%s][%d]金额=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    /*vtcp_log("[%s][%d]交易密码=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
    vtcp_log("%s,%d,磁道检查控制位[%c]",__FILE__,__LINE__,cCtrlcmd[1]);
    vtcp_log("%s,%d,流水号[%d]",__FILE__,__LINE__,g_pub_tx.trace_no);
  
   
    /* 检查卡的合法性 */
  #if DEBUG_spF237
  
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"检查卡的合法性错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /*73域控制标志检查*/
    ret=pub_com_test(cCtrlcmd,"","",g_pub_tx.crd_no,"","","",cGz_flag);
    vtcp_log("%s,%d,cCtrlcmd=[%s],g_pub_tx.crd_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,g_pub_tx.crd_no,cGz_flag);
    if (ret)
    {
        sprintf(acErrMsg,"%s,%d,公共函数pub_com_test()调用失败",__FILE__,__LINE__);
        WRITEMSG
        goto ErrExit;	
    }
    #endif
    ret=check_ky_bal();
    if(ret)
    {
        strcpy( g_pub_tx.reply, "A030" );
        sprintf(acErrMsg,"可用余额小于控制金额");
        WRITEMSG
        goto ErrExit;
    }
  /*给预授权状态表赋值*/
  
    vtcp_log("[%s],[%d]slzh:insert into chnl_single_hold ",__FILE__,__LINE__);
    sChnl_single_hold.tx_date=g_pub_tx.tx_date; /*核心日期*/
    sprintf(sChnl_single_hold.tx_time,"%d",g_pub_tx.tx_time); /*核心时间，long -> char[6] */
    sChnl_single_hold.trace_no=g_pub_tx.trace_no; /*核心流水*/
    memcpy(sChnl_single_hold.cur_no,g_pub_tx.cur_no,strlen(g_pub_tx.cur_no)); /*币种*/
    memcpy(sChnl_single_hold.ac_no,g_pub_tx.crd_no,strlen(g_pub_tx.crd_no)); /*主账号*/
    memcpy(sChnl_single_hold.tx_code,g_pub_tx.tx_code,strlen(g_pub_tx.tx_code)); /*交易代码*/
    
    memcpy(sChnl_single_hold.tx_br_no,g_pub_tx.tx_br_no,strlen(g_pub_tx.tx_br_no)); /*交易机构*/
    memcpy(sChnl_single_hold.opn_br_no,g_pub_tx.opn_br_no,strlen(g_pub_tx.opn_br_no)); /*开户机构*/
    memcpy(sChnl_single_hold.tel,cTel,strlen(cTel)); /*操作员*/
    memcpy(sChnl_single_hold.ttyn,cTtyn,strlen(cTtyn));/*终端号ttyn */
    
    sChnl_single_hold.pt_date=lPt_date; /*平台日期*/
    memcpy(sChnl_single_hold.pt_time,cPt_time,strlen(cPt_time));/*平台时间*/
    
    memcpy(sChnl_single_hold.pt_trace_no,lPt_trace_no,strlen(lPt_trace_no)); /*平台流水号*/
    memcpy(sChnl_single_hold.premise_no,cYsq_no,strlen(cYsq_no)); /*预授权号*/
    memcpy(sChnl_single_hold.brf,cBrf,strlen(cBrf)); /*摘要*/
    sChnl_single_hold.sts[0]='1'; /*预授权冻结*/
    sChnl_single_hold.dz_ind[0]='0'; /*对账标志*/ 
    /*strcpy(sChnl_single_hold.bbmk,"COMM"); 系统种类 COMM公共 */
    strcpy(sChnl_single_hold.bbmk,cTtyn); /*系统种类 COMM公共  不能写死 第10域传送 shangyongchao 2012/6/7 10:42:31*/
    sChnl_single_hold.hold_amt = g_pub_tx.tx_amt1; /*预授权金额*/
    sChnl_single_hold.tx_amt = 0.00; /*完成金额，预授权完成时，改为再修改为预授权金额*/
    Chnl_single_hold_Debug(&sChnl_single_hold);
    ret = Chnl_single_hold_Ins(sChnl_single_hold,g_pub_tx.reply);   
    if(ret)
    {
        sprintf(acErrMsg,"pos 插入登记簿错误!!");
          WRITEMSG
          strcpy(g_pub_tx.reply,"CU06");
          goto ErrExit;
    }
        
    /* 修改帐户的控制金额并对帐户的检查 */
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]",
        g_pub_tx.cur_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    
       g_pub_tx.ac_id = 0; /**/
       g_pub_tx.add_ind[0] = '0'; /* 增加控制金额 ,减客户*/
    
    
    memcpy(g_pub_tx.brf,cBrf,20);
    
    g_pub_tx.svc_ind = 1006;   /*  预授权  */
    g_pub_tx.hst_ind[0] = '0'; /* 不入明细 */
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"记分户账失败!!!");
        WRITEMSG
        goto ErrExit;
    }
    ret = Return_bal(cDc_ind);  /*余额*/
    if(ret)
    {
        sprintf(acErrMsg,"查询余额错误!!!");
        WRITEMSG
        goto ErrExit;    
    }
          
OkExit:
  strcpy( g_pub_tx.reply, "0000" );
  sprintf(acErrMsg,"slzh 预授权success!!");
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  set_zd_long("0040",g_pub_tx.trace_no);/*核心流水*/
  return 0;

ErrExit:
  if(strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
    {
       strcpy(g_pub_tx.reply,"CU06");
    }
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  vtcp_log("[%s][%d]ErrExit:reply=%s",__FILE__,__LINE__,g_pub_tx.reply);
  return 1;
  
}
/**********************************************************************
* 函 数 名：  Return_bal
* 函数功能：  返回活期账户金额，只供本文件使用
* 作    者：  史立芝
* 完成时间：  2011年7月日
*
* 参    数：
*     输  入：无
*      
*     输  出：0820域赋值 金额银联格式组54域
*
*     返回值：0：正确执行
*
* 修改历史：
*                   
********************************************************************/
int Return_bal(char *dc_ind)
{
  int ret=0;
  double today_amt=0.0;
  double account_amt=0.0,limit_amt=0.0;
  double ky_bal=0.0;
  char tmpstr[41];
  
  struct dd_mst_c sDd_mst;
  struct dd_parm_c sDd_parm;
  
  memset(tmpstr,0x00,sizeof(tmpstr));

  strcpy(g_pub_tx.cur_no,"01");
  
  memset(&sDd_mst,0x00,sizeof(sDd_mst));
  memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
  
  ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&sDd_mst);
  account_amt = sDd_mst.bal; /* 账面余额*/
  vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
  /*增加对留存金额的计算*/
  ret= Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no ='%s'",sDd_mst.prdt_no);
  if(ret)
  {
      sprintf(acErrMsg,"查询dd_parm错误!!!");
      strcpy(g_pub_tx.reply,"W015");
      WRITEMSG
      return 1;
  }
  
  ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt - sDd_parm.min_bal;
  
  if (pub_base_CompDblVal(sDd_mst.bal,ky_bal)<0)
  {
      ky_bal = sDd_mst.bal;
  }
  if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '0')
  {
      ky_bal = 0.00;
  }
  vtcp_log("%s,%d,可用金额[%lf],余额[%lf]",__FILE__,__LINE__,ky_bal,account_amt);
  memcpy(tmpstr,"1001156C",8);
  sprintf(tmpstr+8,"%012.0f",account_amt*100);
  memcpy(tmpstr+20,"1002156C",8);	/**modify 2012-04-12**/
  sprintf(tmpstr+28,"%012.0f",ky_bal*100);
  vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
  vtcp_log("%s,%d,金额银联格式组54域[%s]",__FILE__,__LINE__,tmpstr);
  set_zd_data("0820",tmpstr);
  
  
  return(0);
}
/**********************************************************************
* 函 数 名：  check_ky_bal
* 函数功能：  检查可用余额是否大于冻结金额，只供本文件使用
* 作    者：  史立芝
* 完成时间：  2011年8月4日
*
* 参    数：
*     输  入：无
*      
*     输  出：
*
*     返回值：0：足额，1：不足额
*
* 修改历史：
*                   
********************************************************************/

int check_ky_bal()
{
  int ret=0;
  double account_amt=0.0;
  double ky_bal=0.0;
  struct dd_mst_c sDd_mst;
  struct dd_parm_c sDd_parm;

  strcpy(g_pub_tx.cur_no,"01");
  memset(&sDd_mst,0,sizeof(sDd_mst));
  memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
  
  ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&sDd_mst);
  account_amt = sDd_mst.bal; /* 账面余额*/
  vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
  ret= Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no ='%s'",sDd_mst.prdt_no);
  if(ret)
  {
      sprintf(acErrMsg,"查询dd_parm错误!!!");
      strcpy(g_pub_tx.reply,"W015");
      WRITEMSG
      return 1;
  } 
  ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
    
  if (pub_base_CompDblVal(ky_bal,g_pub_tx.tx_amt1)<0)
  {
    return 1;
  }
   return 0;
}
