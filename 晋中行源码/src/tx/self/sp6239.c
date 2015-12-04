/*************************************************
* 文 件 名:  sp6239.c
* 功能描述： POS 预授权完成(处理卡帐户)(本行卡)
*
* 作    者:  dongxiaoxi 
* 完成日期： 2011年7月18日

insert into tx_def values ('6239','预授权完成','10000000001000000000000000000100000100010000100000000000000000000101010010110110100000100000000000000000000000000000000000000000','1','3','0','3',null)

insert into tx_flow_def  values ('6239', 0, '6239', '#=0001@1@#=0002@2@#=0003@3@#=0004@4@#$');
insert into tx_flow_def  values ('6239', 1, 'D003', '#=0000@5@#$');
insert into tx_flow_def  values ('6239', 2, 'D099', '#=0000@5@#$');
insert into tx_flow_def  values ('6239', 3, 'A016', '#=0000@5@#$');
insert into tx_flow_def  values ('6239', 4, 'A017', '#=0000@5@#$');
insert into tx_flow_def  values ('6239', 5, 'NULL', '#=0006@7@#=0007@6@#=0008@9@#=0009@8@#$');
insert into tx_flow_def  values ('6239', 6, 'D003', '#=0000@10@#$');
insert into tx_flow_def  values ('6239', 7, 'D099', '#=0000@10@#$');
insert into tx_flow_def  values ('6239', 8, 'A016', '#=0000@10@#$');
insert into tx_flow_def  values ('6239', 9, 'A017', '#=0000@10@#$');
insert into tx_flow_def  values ('6239', 10, 'F000', '#$');

insert into tx_sub_def values ('6239','预授权完成','sp6239','0','0');

insert into tx_sub_rel  values ('6239', 'NULL', '0', '0720', '0120','配置分支');

insert into tx_sub_rel  values ('6239', 'D003', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel  values ('6239', 'D003', '1', '0000', '0192', '控制标志');
insert into tx_sub_rel  values ('6239', 'D003', '1', '1', '0191', '控制标志');
insert into tx_sub_rel  values ('6239', 'D003', '1', '1', '1022', '账户序号');
insert into tx_sub_rel  values ('6239', 'D003', '0', '0400', '102F', '金额');
insert into tx_sub_rel  values ('6239', 'D003', '1', '01', '102J', '币种');
insert into tx_sub_rel  values ('6239', 'D003', '1', '2', '102K', '现转标志');
insert into tx_sub_rel  values ('6239', 'D003', '0', '0810', '102Y', '摘要');

insert into tx_sub_rel  values ('6239', 'D099', '1', '01', '101A', '币种');
insert into tx_sub_rel  values ('6239', 'D099', '1', '2', '101B', '现转');
insert into tx_sub_rel  values ('6239', 'D099', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel  values ('6239', 'D099', '1', '0000', '0192', '控制标志');
insert into tx_sub_rel  values ('6239', 'D099', '1', '1', '0191', '控制标志');
insert into tx_sub_rel  values ('6239', 'D099', '0', '0810', '1016', '摘要');
insert into tx_sub_rel  values ('6239', 'D099', '1', '1', '1012', '序号');
insert into tx_sub_rel  values ('6239', 'D099', '0', '0400', '1013', '金额');

insert into tx_sub_rel  values ('6239', 'A016', '1', '01', '1204', '币种');
insert into tx_sub_rel  values ('6239', 'A016', '1', '2', '1205', '转账');
insert into tx_sub_rel  values ('6239', 'A016', '0', '0400', '1208', '金额');
insert into tx_sub_rel  values ('6239', 'A016', '0', '0810', '120A', '摘要');

insert into tx_sub_rel  values ('6239', 'A017', '1', '2', '1215', '现转标志');
insert into tx_sub_rel  values ('6239', 'A017', '0', '0400', '1218', '金额');
insert into tx_sub_rel  values ('6239', 'A017', '0', '0810', '121A', '摘要');
insert into tx_sub_rel  values ('6239', 'A017', '1', '01', '1214', '币种');
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_hold_c.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h" 
#include "prdt_ac_id_c.h"   
#include "in_mst_c.h"
#include "dd_mst_c.h"
#include "com_item_c.h"
#include "dd_mst_hst_c.h"
#include "card.h"
#include "card_reg_c.h"


int sp6239()
{
  
  int iRet;
  char flag;
  double dCtl_amt = 0.00;
  double ysqwc_rate = 0.00;
  double dMax_amt =0.00;
  long hx_old_trace,  hx_old_date,  trace_no;
  long lFlag;/*接收POS标志0是本行POS，1是他行POS*/
  char ac_no[25], ctrlcmd[10], march_type[3], tmpstr[51];
  char cDc_ind[2];/*借贷标志*/
  char ct_ind[2]; /*现转标识*/
  char ysq_no[8];/*预授权码*/
  char ysq_ind[2];/*预授权标志位*/
  char cCom_acno[25];/*商户帐号或待清算帐号*/
  char cGz_flag[2];
  char cBbmk[5];/*系统种类*/
  int atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */
  int pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */
  int pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */
  int atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */
  int dscpt = 0;
  struct chnl_single_hold_c mChnl_single_hold;
  struct mdm_ac_rel_c sMdm_ac_rel;
  struct prdt_ac_id_c sPrdt_ac_id; 
  struct dd_mst_c sDd_mst;
  struct dd_mst_c mDd_mst;
  struct in_mst_c sIn_mst ;
  struct com_item_c sCom_item;
  struct dd_mst_hst_c sDd_mst_hst;
  
  /*初始化变量*/
  memset(&mChnl_single_hold,0x00,sizeof(struct chnl_single_hold_c));
  memset(ctrlcmd,0x00,sizeof(ctrlcmd));
  memset(march_type,0x00,sizeof(march_type));
  memset(ysq_no,0x00,sizeof(ysq_no));
  memset(ac_no,0x00,sizeof(ac_no));
  memset(tmpstr,0x00,sizeof(tmpstr));
  memset(cDc_ind,0x00,sizeof(cDc_ind));
  memset(ct_ind,0x00,sizeof(ct_ind));
  memset(ysq_ind,0x00,sizeof(ysq_ind));
  memset(cCom_acno,0,sizeof(cCom_acno));
  memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
  memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
  memset(&mDd_mst,0x00,sizeof(struct dd_mst_c));
  memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
  memset(&sCom_item,0x00,sizeof(struct com_item_c));
  memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
  memset(cGz_flag,0x00,sizeof(cGz_flag));
  memset(cBbmk,0x00,sizeof(cBbmk));
  
  /*初始化系统变量*/
  if (pub_base_sysinit())
  {
    sprintf(acErrMsg,"初始化公用结构错误!!");
    WRITEMSG
      goto ErrExit;
  }
    
  vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
  set_zd_long("0460",g_pub_tx.tx_date);
  
  get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */
  get_zd_data("0810",tmpstr);               /*备注*/
  get_zd_long("0040",&trace_no) ;           /*核心流水号*/  
  get_zd_data("0680",ct_ind);               /*现转标志*/
  get_zd_data("0700",ysq_ind);              /*预授权标志位*/
  get_zd_data("0730",ctrlcmd);              /*控制标志位*/
  get_zd_data("0360",march_type);           /*商户类型*/
  get_zd_data("0660",cDc_ind);               /* 借贷标志 */ 
  /**
  get_zd_data("0790",g_pub_tx.draw_pwd);    支取密码   
  ***/
  get_zd_long("0450",&hx_old_date);         /* 原平台日期*/ 
  get_zd_long("0780",&hx_old_trace);        /* 原平台流水 */
  get_zd_data("0300",g_pub_tx.crd_no);      /* 卡号 */
  get_zd_data("0870",ysq_no);               /* 预授权码*/
  get_zd_data("0310",cCom_acno);            /*商户账号或待清算账号*/
  get_zd_long("0690",&lFlag);/*接收前台POS 标记0是本行1是他行*/
  get_zd_data("0100",cBbmk);
 /*73域控制标志检查*/
  iRet=pub_com_test(ctrlcmd,"","",g_pub_tx.crd_no,"","","",cGz_flag);
  if (iRet)
  {
    sprintf(acErrMsg,"调用公共函数检查错误");
    WRITEMSG
    goto ErrExit;
  }
  
 
  /* 检查卡的合法性 
  if (pub_base_CheckCrdNo())
  {
    sprintf(acErrMsg,"检查卡的合法性错误!!");
    WRITEMSG
    goto ErrExit;
  }*/
  /*查找预授权记录*/ 
  /*增加bbmk 字段 shangyongchao 2012/6/7 13:31:09
  iRet=Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"pt_date='%ld' and pt_trace_no='%ld'",hx_old_date,hx_old_trace);
  */
  iRet=Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"pt_date='%ld' and pt_trace_no='%ld' and bbmk='%s'",\
  hx_old_date,hx_old_trace,cBbmk);
  if (iRet)
  {
    vtcp_log("%s,%d,定义游标错",__FILE__,__LINE__); 
    strcpy(g_pub_tx.reply,"CU25");
    goto ErrExit;
  }
  iRet = Chnl_single_hold_Fet_Upd(&mChnl_single_hold,g_pub_tx.reply);
  if(iRet)
  {
    vtcp_log("%s,%d,需要完成的于授权交易不存在",__FILE__,__LINE__); 
    strcpy(g_pub_tx.reply,"CU25");
    goto ErrExit;
  }
  dCtl_amt = mChnl_single_hold.hold_amt;   
  vtcp_log("[%s][%d],原控制金额 = 【%lf】",__FILE__,__LINE__,dCtl_amt); 
  /* 判断金额是否超出预授权金额 * 银联测试是超过15％以上不可以 modified by liuxuan*/
  iRet = pub_base_GetParm_double("YSQWC",1,&ysqwc_rate);
  if(iRet)
  {
  	strcpy(g_pub_tx.reply,"CD06");
  	vtcp_log("%s,%d,取预授权完成上浮比率失败!",__FILE__,__LINE__);
  	goto ErrExit;
  }
  vtcp_log("%s,%d,上浮比率[%lf]",__FILE__,__LINE__,ysqwc_rate);
  dMax_amt = mChnl_single_hold.hold_amt * (1+ysqwc_rate);
  vtcp_log("%s,%d,最大完成金额[%lf]",__FILE__,__LINE__,dMax_amt);
  if (pub_base_CompDblVal(dMax_amt,g_pub_tx.tx_amt1) < 0)
  {
    sprintf(acErrMsg,"金额超出了预授权金额!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU61");
    goto ErrExit;
  }
  vtcp_log("%s,%d,完成金额[%lf],控制金额[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1,mChnl_single_hold.hold_amt);
  
  /*判断预授权状态*/
  if (mChnl_single_hold.sts[0] != '1')
  {
    vtcp_log("%s,%d,授权号状态不正常",__FILE__,__LINE__); 
    strcpy(g_pub_tx.reply,"CU25");
    goto ErrExit;
  }
  
  /*更新预授权状态*/
  mChnl_single_hold.tx_amt = g_pub_tx.tx_amt1;
  strcpy(mChnl_single_hold.sts,"3");
  Chnl_single_hold_Debug(&mChnl_single_hold);
  iRet = Chnl_single_hold_Upd_Upd(mChnl_single_hold,g_pub_tx.reply);
  if (iRet)
  {
    sprintf(acErrMsg,"更新预授权记录出错");
    WRITEMSG
    strcpy(g_pub_tx.reply,"0041");
    goto ErrExit;
  }
  Chnl_single_hold_Clo_Upd();
 
 /*在单笔预授权登记簿中插入一条新记录*/
  mChnl_single_hold.o_tx_date=mChnl_single_hold.tx_date;
  sprintf(mChnl_single_hold.o_trace_no,"%ld",mChnl_single_hold.trace_no);
  mChnl_single_hold.o_pt_date=mChnl_single_hold.pt_date;
  strcpy(mChnl_single_hold.o_pt_trace_no,mChnl_single_hold.pt_trace_no);
  mChnl_single_hold.tx_amt=g_pub_tx.tx_amt1;
  mChnl_single_hold.hold_amt=mChnl_single_hold.hold_amt;
  strcpy(mChnl_single_hold.brf,"预授权结束");
  strcpy(mChnl_single_hold.sts,"3");
  mChnl_single_hold.tx_date=g_pub_tx.tx_date;
  sprintf(mChnl_single_hold.tx_time,"%d",g_pub_tx.tx_time);
  mChnl_single_hold.trace_no=g_pub_tx.trace_no;
  get_zd_data("0520",mChnl_single_hold.pt_trace_no);
  get_zd_long("0440",&mChnl_single_hold.pt_date);
  get_zd_data("0170",mChnl_single_hold.pt_time);
  get_zd_data("0100",mChnl_single_hold.bbmk);
  iRet=Chnl_single_hold_Ins( mChnl_single_hold , g_pub_tx.reply );
  if (iRet)
  {
    sprintf(acErrMsg,"插入预授权记录出错，POS机对应账户没有实现贷方记账!!");
    WRITEMSG
    goto ErrExit;
  } 
  /*更新卡登记簿*/
  if (cDc_ind[0] == '1')
  {
    g_pub_tx.add_ind[0]='0';
  }
  else if (cDc_ind[0]=='2')
  {
    g_pub_tx.add_ind[0]='1';
  }
  dscpt = CARD_TRADE_TRAN;
  iRet=pub_card_reg(g_pub_tx.tx_amt1,"POS",g_pub_tx.add_ind[0],&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,lFlag,dscpt);
  if (iRet)
  {
    sprintf(acErrMsg,"插入卡登记簿错误");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
   /*更新用户控制金额*/
  iRet=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s'" ,g_pub_tx.crd_no);
  if (iRet)
  {
    vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.crd_no);
    strcpy(g_pub_tx.reply,"P102");
    goto ErrExit;
  }
  
  iRet=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id);
  if (iRet)
  {
    sprintf(acErrMsg,"定义游标错误!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU25");
    goto ErrExit;
  }
  iRet=Dd_mst_Fet_Upd(&mDd_mst,g_pub_tx.reply);
  if(iRet)
  {
    sprintf(acErrMsg,"未找到记录!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU25");
    Dd_mst_Clo_Upd();
    goto ErrExit;
  } 
  /**
  mDd_mst.ctl_amt=mDd_mst.ctl_amt-g_pub_tx.tx_amt1;
  ***/
  mDd_mst.ctl_amt =mDd_mst.ctl_amt - dCtl_amt;
  iRet=Dd_mst_Upd_Upd(mDd_mst,g_pub_tx.reply);
  if(iRet)
   {
    sprintf(acErrMsg,"更新原记录错误!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU25");
    Dd_mst_Clo_Upd();
    goto ErrExit;
   }
  Dd_mst_Clo_Upd();    

  
  /*判断用户帐号类型*/
  if(strlen(g_pub_tx.crd_no) > 10)   /**账号*/
  {
    iRet= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",g_pub_tx.crd_no);
    if( iRet )
    {
      vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.crd_no);
      strcpy(g_pub_tx.reply,"P102");
      goto ErrExit;
    }
    iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
    if (iRet)
    {
      vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,g_pub_tx.crd_no);
      strcpy(g_pub_tx.reply,"K106");
      goto ErrExit;
    }
    if(sPrdt_ac_id.ac_id_type[0] == '1')  /*活期*/
    {
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
      if(cDc_ind[0] == '1')   /*1-借*/
      {
        set_zd_data("1021",mChnl_single_hold.ac_no);
        set_zd_data("0193","1007");
        strcpy(g_pub_tx.reply,"0001");
      }
      else if(cDc_ind[0] == '2')   /*2-贷*/
      {
        set_zd_data("1011",mChnl_single_hold.ac_no);
        set_zd_data("0193","1007");
        strcpy(g_pub_tx.reply,"0002");
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
      iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
      if (iRet)
      {
        vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.crd_no);
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
      }
      if(sIn_mst.sts[0] != '1')
      {
        vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,g_pub_tx.crd_no);
        strcpy(g_pub_tx.reply,"O085");
        goto ErrExit;
      }
      
      if(cDc_ind[0] == '1')   /*1-借*/
      {
        set_zd_data("1201",mChnl_single_hold.ac_no);
        set_zd_data("0193","1007");
        strcpy(g_pub_tx.reply,"0003");
      }
      else if(cDc_ind[0] == '2')   /*2-贷*/
      {
        set_zd_data("1211",mChnl_single_hold.ac_no);
        set_zd_data("0193","1007");
        strcpy(g_pub_tx.reply,"0004");
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
  else if( strlen(g_pub_tx.crd_no) <= 10)
  {
    iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",g_pub_tx.crd_no);
    if(iRet)
    {
      sprintf(acErrMsg,"查询科目失败!!!");
      WRITEMSG
      strcpy(g_pub_tx.reply, "B172");
      goto ErrExit;
    }
    
    if(cDc_ind[0] == '1')   /*1-借*/
    {
      set_zd_data("1201",mChnl_single_hold.ac_no);
      set_zd_data("0193","1007");
      
      strcpy(g_pub_tx.reply,"0003");
    }
    else if(cDc_ind[0] == '2')   /*2-贷*/
    {
      set_zd_data("1211",mChnl_single_hold.ac_no);
      set_zd_data("0193","1007");
      strcpy(g_pub_tx.reply,"0004");
    }
    else
    {
      sprintf(acErrMsg,"借贷标志错误!!!");
      WRITEMSG
      strcpy(g_pub_tx.reply, "P404");
      goto ErrExit;
    }
  }
  else if( strlen(g_pub_tx.crd_no)==0)
  {
    sprintf(acErrMsg,"未传入主账号!");
    WRITEMSG
    strcpy(g_pub_tx.reply, "AT08");
    goto ErrExit;
  }
  
  if(strlen(cCom_acno) > 10)   /**商户账号*/
  {
    iRet= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",cCom_acno);
    if( iRet )
    {
      vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,cCom_acno);
      strcpy(g_pub_tx.reply,"P102");
      goto ErrExit;
    }
    iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
    if (iRet)
    {
      vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cCom_acno);
      strcpy(g_pub_tx.reply,"K106");
      goto ErrExit;
    }
    if(sPrdt_ac_id.ac_id_type[0] == '1')  /*活期*/
    {
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
      
      if(cDc_ind[0] == '1')         /*1-借*/
      {
              
        set_zd_data("1011",cCom_acno);
        set_zd_data("0193","1007");
        set_zd_data("0720","6");
        
      }
      else if(cDc_ind[0] == '2')   /*2-贷*/
      {
        set_zd_data("1021",cCom_acno);
        set_zd_data("0193","1007");
        set_zd_data("0720","7");
        
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
      iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
      if (iRet)
      {
        vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cCom_acno);
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
      }
      if(sIn_mst.sts[0] != '1')
      {
        vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cCom_acno);
        strcpy(g_pub_tx.reply,"O085");
        goto ErrExit;
      }
      if(cDc_ind[0] == '1')   /*1-借*/
      {
        set_zd_data("1211",cCom_acno);
        set_zd_data("0193","1007");
        set_zd_data("0720","8");
      }
      else if(cDc_ind[0] == '2')   /*2-贷*/
      {
        set_zd_data("1201",cCom_acno);
        set_zd_data("0193","1007");
        set_zd_data("0720","9");
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
  else if(strlen(cCom_acno)<=10)
  {
    iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cCom_acno);
    if(iRet)
    {
      sprintf(acErrMsg,"查询科目失败!!!");
      WRITEMSG
      strcpy(g_pub_tx.reply, "B172");
      goto ErrExit;
    }
    if(cDc_ind[0] == '1')         /*1-借*/
    {
      set_zd_data("1211",cCom_acno);
      set_zd_data("0193","1007");
      set_zd_data("0720","8");
    }
    else if(cDc_ind[0] == '2')   /*2-贷,分支走借*/
    {
      set_zd_data("1201",cCom_acno);
      set_zd_data("0193","1007");
      set_zd_data("0720","9");
    }
    else
    {
      sprintf(acErrMsg,"借贷标志错误!!!");
      WRITEMSG
      strcpy(g_pub_tx.reply, "P404");
      goto ErrExit;
    }
  }
  else if( strlen(cCom_acno)==0)
  {
    sprintf(acErrMsg,"未传入商户账号或待清算账号!");
    WRITEMSG
    strcpy(g_pub_tx.reply, "AT08");
    goto ErrExit;
  }
  /********下面的程序是关于把“被冻结”的账户转走钱的*******************/
 
 /*set_zd_data("1021",mChnl_single_hold.ac_no);
 set_zd_data("102K","2");
 set_zd_long("102F",mChnl_single_hold.tx_amt);
 set_zd_data("0193","1007");*/

OkExit:
  /*strcpy( g_pub_tx.reply, "0000" );*/
  sprintf(acErrMsg,"success!!");
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 0;

ErrExit:
  if (strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
  {
    strcpy(g_pub_tx.reply,"CU06");
  }
  sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
  WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 1;
}
