/*********************************************************************
* 版 本 号 :  V1.0        
* 程序作者 :  商永超          
* 日    期 :  2011-07-18   
* 所属模块 :  POS预授权撤销           
* 程序名称 :  spF238.c
* 程序作用 : 
* 函数列表 :       
*         1、pub_base_sysinit();初始化函数
*         2、pub_base_CheckCrdNo();检查卡合法性函数
*         3、pubf_card_chk_msr();检查卡磁道信息函数
*         4、pub_base_DesChk();检查支付密码函数
*         5、pub_base_CompDblVal();数值比较函数
*         6、pub_acct_trance();记账函数                 
* 使用注意 :                                       
* 修改记录 :                                       
*      日    期:20110816
*      修 改 人：薛珂
*      修改内容:pub_com_test.c()控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
insert into tx_def values('6238','预授权撤销','10000000000010000000000010000100000000000000000000000000000000000000000000000000011000000000000000000100000000000000000000000100','1','1','0','3',null);
insert into tx_flow_def values('6238','0','F238','#$');
insert into tx_sub_def values('F238','预授权撤销','spF238','0','0');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_hold_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
int spF238()
{
          
    int ret=0;
    char cYsq_ind[2];/*预授权标志 2为预授权撤销*/
    /*char cDc_ind[2];借贷标志：1借，2贷*/
    char cMsr_str2[38];/*2磁道信息*/
    char cMsr_str3[142];/*3磁道信息*/
    char cCtrlcmd[11];/*控制标志本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
          检查标志(0不检查1检查)：
          [0]:密码检查标志0不查1查
          [1]:磁道信息检查标志0不查1查
          [2]:证件检查标志0不查1查
          [3]:户名检查标志0不查不查
          [4]:帐号检查标志0不查不查
          [5]:收款人挂帐标志0帐号必须相符不查户名1帐号户名必须都相符2帐号必须相符，户名不符挂帐3帐号或户名有一不符挂帐,9强制挂帐
          [6]:通存通兑标志0允许1不允许
          [7]:凭证检查标志0不查1查柜员凭证2查客户凭证*/
    char cBrf[51];/*摘要*/
    char cYsq_no[11];/*预授权号*/
    long lOld_pt_date=0;/*原平台日期*/
    char cOld_pt_trace_no[13];/*原平台流水*/
    long lPt_date=0;/*平台日期*/
    char cPt_time[7];/*平台时间*/
    char cPt_trace_no[13];/*平台流水*/
    char cDz_ref[77];/*对账参考数据*/
    char cHx_term[5];/*终端号*/
    double account_amt=0.0;/*账面余额*/
    double ky_bal=0.0;/*可用余额*/
    char tmpstr[41];
    /*调用pub_com_test函数时，接收返回值*/
    char cGz_flag[2];/*挂账标志*/
    struct chnl_single_hold_c sChnl_single_hold_old;
    struct chnl_single_hold_c sChnl_single_hold_new;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    struct dd_parm_c sDd_parm;

    memset(&sChnl_single_hold_old,0x0,sizeof(struct chnl_single_hold_c));
    memset(&sChnl_single_hold_new,0x0,sizeof(struct chnl_single_hold_c));
    memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
    memset(cYsq_ind,0,sizeof(cYsq_ind));
   /* memset(cDc_ind,0,sizeof(cDc_ind));*/ 
    memset(cMsr_str2,0,sizeof(cMsr_str2));
    memset(cMsr_str3,0,sizeof(cMsr_str3));
    memset(cCtrlcmd,0,sizeof(cCtrlcmd));
    memset(cBrf,0,sizeof(cBrf));
    memset(cYsq_no,0,sizeof(cYsq_no));
    memset(cPt_trace_no,0,sizeof(cPt_trace_no));
    memset(cPt_time,0,sizeof(cPt_time));
    memset(cOld_pt_trace_no,0,sizeof(cOld_pt_trace_no));
    memset(cDz_ref,0,sizeof(cDz_ref));
    memset(cHx_term,0,sizeof(cHx_term));
    memset(tmpstr,0,sizeof(tmpstr));
    /*调用pub_com_test时用到*/
    memset(cGz_flag,0x00,sizeof(cGz_flag));
    /*初始化*/
    if(pub_base_sysinit())
    {
       sprintf(acErrMsg,"初始化公用结构错误!!");
       WRITEMSG
       goto ErrExit;
    }
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    /*取数据*/
    get_zd_long("0440",&lPt_date);/*平台日期*/
    vtcp_log("%s,%d,平台日期=[%ld]",__FILE__,__LINE__,lPt_date);
    get_zd_data("0170",cPt_time);/*平台时间*/
    vtcp_log("%s,%d,平台时间=[%s]",__FILE__,__LINE__,cPt_time);
    get_zd_data("0520",cPt_trace_no);/*平台流水*/
    vtcp_log("%s,%d,平台流水=[%s]",__FILE__,__LINE__,cPt_trace_no);
    get_zd_data("0100",cHx_term);/*终端号*/
    vtcp_log("%s,%d,终端号=[%s]",__FILE__,__LINE__,cHx_term);
    get_zd_data("0700",cYsq_ind);/*预授权标志*/
    vtcp_log("%s,%d,预授权标志=[%c]",__FILE__,__LINE__,cYsq_ind[0]);
    /*get_zd_data("0660",cDc_ind);借贷标志：1借，2贷*/
    get_zd_data("0750",cMsr_str2);/*磁道2信息*/
    vtcp_log("%s,%d,2磁道信息=[%s]",__FILE__,__LINE__,cMsr_str2);
    get_zd_data("0760",cMsr_str3);/*磁道3信息*/
    vtcp_log("%s,%d,3磁道信息=[%s]",__FILE__,__LINE__,cMsr_str3);
    get_zd_data("0730",cCtrlcmd);/*控制标志*/
    vtcp_log("%s,%d,控制标志=[%s]",__FILE__,__LINE__,cCtrlcmd);
    get_zd_data("0810",cBrf);/*摘要*/
    vtcp_log("%s,%d,摘要=[%s]",__FILE__,__LINE__,cBrf);
    get_zd_long("0450",&lOld_pt_date);/*原平台日期*/
    vtcp_log("%s,%d,原平台日期=[%ld]",__FILE__,__LINE__,lOld_pt_date);
    get_zd_data("0780",cOld_pt_trace_no);/*原平台流水*/
    vtcp_log("%s,%d,原平台流水=[%s]",__FILE__,__LINE__,cOld_pt_trace_no);
    get_zd_data("0740",cDz_ref);/*对账参考*/
    vtcp_log("%s,%d,对账参考=[%s]",__FILE__,__LINE__,cDz_ref);
    get_zd_data("0870",cYsq_no);/*预授权号*/
    vtcp_log("%s,%d,预授权号=[%s]",__FILE__,__LINE__,cYsq_no);
    get_zd_data("0300",g_pub_tx.crd_no);/*卡号*/
    vtcp_log("%s,%d,卡号=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
    get_zd_double("0400",&g_pub_tx.tx_amt1);/*控制金额*/
    vtcp_log("%s,%d,控制金额=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    /*
    get_zd_data("0790",g_pub_tx.draw_pwd);交易密码
    vtcp_log("%s,%d,交易密码=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);
    */
    
    /*检查预授权标志是否为预授权撤销*/
    if(cYsq_ind[0] != '2')
    {
      sprintf(acErrMsg,"预授权标志错误");
      WRITEMSG
        goto ErrExit;
    }
    
    /*检查卡的合法性*/
    
    if(pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"检查卡的合法性错误！");
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
    /*查找原记录*/
    /*增加bbmk字段作为查询条件 shangyongchao 2012/6/7 10:48:00 改为游标更新
    ret = Chnl_single_hold_Sel(g_pub_tx.reply,&sChnl_single_hold_old,"ac_no='%s' and pt_date=%ld and pt_trace_no= %s \
        and premise_no='%s' ",g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no);*/
    ret = Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and pt_date=%ld and pt_trace_no= %s \
        and premise_no='%s' and bbmk='%s' ",g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no,cHx_term);
    if(ret)
    {
        sprintf(acErrMsg,"定义游标出错!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    ret = Chnl_single_hold_Fet_Upd(&sChnl_single_hold_old,g_pub_tx.reply);
    if(ret)
    {
        sprintf(acErrMsg,"取记录出错!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    vtcp_log("%s,%d,原sts=[%c]",__FILE__,__LINE__,sChnl_single_hold_old.sts[0]);
    
    /*检查原纪录状态*/
    if(sChnl_single_hold_old.sts[0] != '1')
    {
      sprintf(acErrMsg,"原记录状态不为预授权");
      WRITEMSG
      strcpy(g_pub_tx.reply,"CU06");
      goto ErrExit;
    } 
    /*检查金额*/
    vtcp_log("%s,%d,授权金额=[%f]",__FILE__,__LINE__,sChnl_single_hold_old.hold_amt);
    if(pub_base_CompDblVal(sChnl_single_hold_old.hold_amt,g_pub_tx.tx_amt1) != 0)
    {
      sprintf(acErrMsg,"撤销金额与预授权金额不符合!!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"P008");
      goto ErrExit;
    }
      
    /*更新要撤销的记录*/
    vtcp_log("%s,%d,授权号=[%s]",__FILE__,__LINE__,sChnl_single_hold_old.premise_no);
    /*更新原纪录*/
    /*改为游标更新
    ret=sql_execute2("update chnl_single_hold set sts='2' where ac_no='%s' \
  and pt_date=%ld and pt_trace_no= %s and premise_no='%s'",g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no);
  */
    sChnl_single_hold_old.sts[0] ='2';
    ret = Chnl_single_hold_Upd_Upd(sChnl_single_hold_old,g_pub_tx.reply);
    if(ret)
    {
        sprintf(acErrMsg,"更新撤销记录错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    Chnl_single_hold_Clo_Upd();
     /*给新纪录结构赋值*/    
    sChnl_single_hold_new.tx_date = g_pub_tx.tx_date;/*核心日期*/
    sprintf(sChnl_single_hold_new.tx_time,"%d",g_pub_tx.tx_time);/*核心交易时间*/
    sChnl_single_hold_new.trace_no = g_pub_tx.trace_no;/*核心流水号*/
    strcpy(sChnl_single_hold_new.cur_no,g_pub_tx.cur_no);/*币种，01*/
    strcpy(sChnl_single_hold_new.ac_no,g_pub_tx.crd_no);/*卡号*/
    strcpy(sChnl_single_hold_new.tx_code,g_pub_tx.tx_code);/*交易代码*/
    strcpy(sChnl_single_hold_new.tx_br_no,g_pub_tx.tx_br_no);/*交易机构*/
    strcpy(sChnl_single_hold_new.opn_br_no,g_pub_tx.opn_br_no);/*开户机构*/
    strcpy(sChnl_single_hold_new.tel,g_pub_tx.tel);/*柜员号*/
    /*strcpy(sChnl_single_hold_new.bbmk,"COMM");系统种类*/
    strcpy(sChnl_single_hold_new.bbmk,cHx_term);/*改为第10域传送的值 shangyongchao 2012/6/7 10:44:22*/
    strcpy(sChnl_single_hold_new.ttyn,cHx_term);/*终端号*/
    sChnl_single_hold_new.pt_date = lPt_date;/*平台日期*/
    strcpy(sChnl_single_hold_new.pt_time,cPt_time);/*平台时间*/
    strcpy(sChnl_single_hold_new.pt_trace_no,cPt_trace_no);/*平台流水*/
    strcpy(sChnl_single_hold_new.premise_no,sChnl_single_hold_old.premise_no);/*预授权号*/
    sChnl_single_hold_new.hold_amt = g_pub_tx.tx_amt1;/*控制金额*/
    sChnl_single_hold_new.tx_amt = sChnl_single_hold_old.tx_amt;/*交易金额*/
    strcpy(sChnl_single_hold_new.brf,cBrf);/*摘要*/
    strcpy(sChnl_single_hold_new.sts,"2");/*预授权状态*/
    sChnl_single_hold_new.o_tx_date = sChnl_single_hold_old.tx_date;/*预授权核心日期*/
    sprintf(sChnl_single_hold_new.o_trace_no,"%d",sChnl_single_hold_old.trace_no);/*预授权核心流水*/
    sChnl_single_hold_new.o_pt_date = sChnl_single_hold_old.pt_date;/*预授权平台时间*/
    strcpy(sChnl_single_hold_new.o_pt_trace_no,sChnl_single_hold_old.pt_trace_no);/*预授权平台流水*/
    strcpy(sChnl_single_hold_new.dz_ind,"0");/*对账标志*/
    strcpy(sChnl_single_hold_new.dz_ref,cDz_ref); /*对账参考*/   
    Chnl_single_hold_Debug(&sChnl_single_hold_new);
    /* 登记单笔预授权登记簿*/
    ret =Chnl_single_hold_Ins(sChnl_single_hold_new,g_pub_tx.reply);
    if (ret)
    {
        sprintf(acErrMsg,"登记单笔预授权登记簿错误!!");
        WRITEMSG
        goto ErrExit;
    }    
       
    /* 修改帐户的控制金额并对帐户的检查 */ 
    strcpy(g_pub_tx.cur_no,"01");/*币种设定为人民币*/
    set_zd_data("0210",g_pub_tx.cur_no);
    ret=pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);/*根据币种生成账户序号*/
    if(ret)
    {
        sprintf(acErrMsg,"根据币种生成账户序号错误 cur_no=[%s]",g_pub_tx.cur_no); 
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT08");
        goto ErrExit;
    }
    g_pub_tx.ac_id = 0;
    g_pub_tx.tx_amt1 = sChnl_single_hold_old.hold_amt; /*金额*/
    g_pub_tx.add_ind[0] = '1';  /*减少控制金额*/
    g_pub_tx.svc_ind = 1006;    /*预授权*/ 
    g_pub_tx.hst_ind[0] = '0'; /*不入明细*/
    memcpy(g_pub_tx.brf,cBrf,20);
         
   
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"预授权撤销--错误!!!");
        WRITEMSG
        goto ErrExit;
    }
    /* end of 修改帐户的控制金额并对帐户的检查 */
     
     /*返回余额*/
    ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&sDd_mst);/*根据币种，卡号查找存款信息*/
    if(ret)
    {
        sprintf(acErrMsg,"查询余额出错!!");
        WRITEMSG
        goto ErrExit;
    }
    account_amt = sDd_mst.bal; /* 账面余额*/
    vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
    ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
    if(ret)
    {
        sprintf(acErrMsg,"查询dd_parm错误!!!");
        strcpy(g_pub_tx.reply,"W015");
        WRITEMSG
        goto ErrExit;
    }
    ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
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
    set_zd_data("0820",tmpstr);
    set_zd_long("0040",sChnl_single_hold_new.trace_no);/*返回核心流水号*/
    /*返回余额结束*/
    
    
OkExit:
  strcpy( g_pub_tx.reply, "0000" );
  sprintf(acErrMsg,"success!!");
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 0;

 ErrExit:
  if(strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
  {
      strcpy(g_pub_tx.reply,"CU06");
  }
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 1;
}
    
  
