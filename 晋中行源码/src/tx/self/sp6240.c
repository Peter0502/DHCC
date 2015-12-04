/*************************************************
* 版 本 号 :  V1.0        
* 程序作者 :  程令营         
* 日    期 :  2011-07-12   
* 所属模块 :  与平台的接口程序 (预授权类冲正交易)
* 程序名称 :  sp6240.c
* 程序作用 : 预授权类冲正交易
*             （1）预授权冲正
*             （2）预授权撤消冲正
*             （3）预授权完成冲正
*函数列表：（1）pub_base_sysinit()
*          （2）pub_acct_trance()
*           (3) pubf_acct_proc()
*使用注意：程序根据平台70域取值，判断冲正类型，进行相应类型的冲正
*          使用程序要修改pubf_acct_trance.c 第5784行处添加如下代码：
*          strcmp(g_pub_tx.tx_code,"6240") &&保存，重新编译，并重启
*          服务。
*修改作者 :                           
*修改时间 :                                    
*修改内容 :
*交易定义表配置如下：
*insert into tx_def values('6240'，'预授权类冲正交易','10000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100','1','1','0','3',null)
*insert into tx_flow_def values('6240','0','6240','#=0001@1@#=0002@2@#$');
*insert into tx_flow_def values('6240',1,'D099','#=0000@3@#$');
*insert into tx_flow_def values('6240','2','A017','#=0000@3@#$');
*insert into tx_flow_def values('6240','3','F000','#$');
*insert into tx_sub_def values('6240','预授权类冲正交易','sp6240','0','0');
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_hold_c.h"
#include "card.h"
#include "card_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h" 
#include "com_item_c.h"
#include "dd_parm_c.h"
int Return_bal(); /*查询活期余额*/
int sp6240()
{
 int ret;
 long old_date;/*原平台日期*/
 char old_trace_no[13];/*原平台流水号*/
 char cFlag[2];/*判断是否是本行POS,0是本行，1是他行，由平台数据判断*/
 double tx_amt2=0.00;/*用来插入卡登记簿*/
 char brf[51];/*记帐摘要*/
 char ct_ind[2];/*现转标识*/
 char dc_ind[2];/*借贷标记*/
 char tmpstr[51];/*备注*/
 char tmp_str[51];/*开户机构*/
 char ysq_ind[2];/*预授权标志*/
 char msr_str2[38];/*2磁道信息*/
 char msr_str3[142];/*3磁道信息*/
 char ac_no[25];/*卡号*/
 char cCom_acno[25];/*商户帐号或待清算帐号*/
 char cCtrlcmd[10];/*控制标志*/
 char cGz_flag[2];/*挂账标志*/
 char cAc_no[25];
 char cBbmk[5];/*系统种类*/
 struct chnl_single_hold_c sChnl_single_hold;
 struct chnl_single_hold_c mChnl_single_hold;
 struct mdm_ac_rel_c sMdm_ac_rel;
 struct prdt_ac_id_c sPrdt_ac_id; 
 struct dd_mst_c sDd_mst;
 struct in_mst_c sIn_mst;
 struct com_item_c sCom_item;
 
 memset(old_trace_no,0,sizeof(old_trace_no));
 memset(brf,0,sizeof(brf));
 memset(cFlag,0x00,sizeof(cFlag));
 memset(ct_ind,0,sizeof(ct_ind));
 memset(tmpstr,0,sizeof(tmpstr));
 memset(msr_str2,0,sizeof(msr_str2));
 memset(dc_ind,0,sizeof(dc_ind));
 memset(ac_no,0,sizeof(ac_no));
 memset(ysq_ind,0,sizeof(ysq_ind));
 memset(tmp_str,0,sizeof(tmp_str));
 memset(cCom_acno,0x00,sizeof(cCom_acno));
 memset(msr_str3,0,sizeof(msr_str3));
 memset(&sChnl_single_hold,0x00,sizeof(struct chnl_single_hold_c));
 memset(&mChnl_single_hold,0x00,sizeof(struct chnl_single_hold_c));
 memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
 memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
 memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
 memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
 memset(&sCom_item,0x00,sizeof(struct com_item_c));
 memset(cCtrlcmd,0x00,sizeof(cCtrlcmd));
 memset(cGz_flag,0x00,sizeof(cGz_flag));
 memset(cAc_no,0x00,sizeof(cAc_no));
 memset(cBbmk,0x00,sizeof(cBbmk));
 /*初始化公用结构*/
 if (pub_base_sysinit())
  {
     sprintf(acErrMsg,"初始化公用结构失败!!");
     WRITEMSG
     goto ErrExit;
  }
  vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
  set_zd_long("0460",g_pub_tx.tx_date);
 /*取得域值*/
 get_zd_data("0300",g_pub_tx.crd_no);/*取得卡号*/
 get_zd_data("0310",cCom_acno);/*取得商户帐号*/
 vtcp_log("[%s][%d],商户帐号1=[%s]",__FILE__,__LINE__,cCom_acno);
 get_zd_double("0400",&g_pub_tx.tx_amt1);/*取得消费金额*/
 get_zd_long("0450",&old_date);/*取得原平台日期*/
 get_zd_data("0660",dc_ind);/*取得借贷标记*/
 get_zd_data("0690",cFlag);/*POS标识*/
 get_zd_data("0700",ysq_ind);/*取得预授权标志*/
 get_zd_data("0730",cCtrlcmd);/*控制项*/
 get_zd_data("0750",msr_str2);/*2磁道信息*/
 get_zd_data("0760",msr_str3);
 get_zd_data("0780",old_trace_no);/*取得原平台流水号*/
 get_zd_data("0100",cBbmk);/*系统种类*/
 /*
 get_zd_data("0790",g_pub_tx.draw_pwd);  支取密码 
 */
 get_zd_data("0810",tmpstr);/*摘要*/
 strcpy(cAc_no,g_pub_tx.crd_no);
 vtcp_log("[%s][%d],商户帐号4=[%s]",__FILE__,__LINE__,cCom_acno);
 vtcp_log("%s,%d,原平台流水号==g_pub_tx.old_trace_no==[%s]",__FILE__,__LINE__,old_trace_no);
 vtcp_log("%s,%d,预授权标志==0700==[%s]",__FILE__,__LINE__,ysq_ind) ;
 vtcp_log("%s,%d,取得卡号=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no) ;
 vtcp_log("%s,%d,cCom_acno=[%s]",__FILE__,__LINE__,cCom_acno) ;
 
 ret=pub_com_test(cCtrlcmd,"","",g_pub_tx.crd_no,"","","",cGz_flag);
 if (ret)
  {
    sprintf(acErrMsg,"调用检查控制项出错");
    WRITEMSG
    goto ErrExit;
  }
  
  /*定义登记预授权状态的游标*/
  /*增加查询条件 shangyongchao 2012/6/7 10:58:39
  ret=Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"pt_date=%ld and pt_trace_no='%s'",old_date,old_trace_no);*/
  ret=Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",old_date,old_trace_no,cBbmk);
  if(ret)
   {
      sprintf(acErrMsg,"定义游标错误");
      strcpy(g_pub_tx.reply,"CU25");
      WRITEMSG
      goto ErrExit;
   }
    
  /*查询预授权记录*/
  ret=Chnl_single_hold_Fet_Upd(&mChnl_single_hold,g_pub_tx.reply);
  if(ret)
  {
     sprintf(acErrMsg,"未找到记录");
     WRITEMSG
     strcpy(g_pub_tx.reply,"CU25");
     Chnl_single_hold_Clo_Upd(); 
     goto ErrExit;
  }
   
  vtcp_log("%s,%d,hold_amt==[%.2f]",__FILE__,__LINE__,mChnl_single_hold.hold_amt);
  vtcp_log("%s,%d,tx_amt==[%.2f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
  
  if(memcmp(mChnl_single_hold.ac_no,g_pub_tx.crd_no,13)!=0)
   {
      sprintf(acErrMsg,"要冲正的卡号不一致!mac_no is [%s],crd_no is [%s]",mChnl_single_hold.ac_no,g_pub_tx.crd_no);
      WRITEMSG
      strcpy(g_pub_tx.reply,"CU06");
      goto ErrExit;
   }
  
  if(mChnl_single_hold.sts[0]=='0')
   {
      sprintf(acErrMsg,"交易无效");
      WRITEMSG
      Chnl_single_hold_Clo_Upd();
      strcpy(g_pub_tx.reply,"CU64");
      goto ErrExit;
   }
   
  if (ysq_ind[0]=='1')/*如果是预授权冲正*/
  {
     if (mChnl_single_hold.sts[0]!='1')
      {
         sprintf(acErrMsg,"要冲正的交易非预授权");
         WRITEMSG
         Chnl_single_hold_Clo_Upd();
         strcpy(g_pub_tx.reply,"CU64");
         goto ErrExit;
      }
     vtcp_log("%s,%d",__FILE__,__LINE__) ;
     if(pub_base_CompDblVal(mChnl_single_hold.hold_amt,g_pub_tx.tx_amt1)==0)
      {
         mChnl_single_hold.sts[0]='0';
         mChnl_single_hold.hold_amt=0.00;
      }
      else if(pub_base_CompDblVal(mChnl_single_hold.hold_amt,g_pub_tx.tx_amt1)>0)
     {
        mChnl_single_hold.hold_amt=mChnl_single_hold.hold_amt-g_pub_tx.tx_amt1;
        memcpy(mChnl_single_hold.brf,tmpstr,30);
        mChnl_single_hold.sts[0]='0';
        vtcp_log("%s,%d",__FILE__,__LINE__) ;
     }
     else
     {
        vtcp_log("%s,%d,冲正金额大于预授权金额",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"CU64");
        WRITEMSG
        Chnl_single_hold_Clo_Upd();
        goto ErrExit;
     }
     ret=Chnl_single_hold_Upd_Upd(mChnl_single_hold,g_pub_tx.reply);
     vtcp_log("%s,%d,hold_amt=[%.2f]",__FILE__,__LINE__,mChnl_single_hold.hold_amt) ;
     if(ret)
     {
        sprintf(acErrMsg,"更新记录出错");
        strcpy(g_pub_tx.reply,"CU25");
        Chnl_single_hold_Clo_Upd();
        WRITEMSG
        goto ErrExit;
     }
     Chnl_single_hold_Clo_Upd();
     vtcp_log("%s,%d,hold_amt=[%.2f]",__FILE__,__LINE__,mChnl_single_hold.hold_amt) ;
   
       /*冲正前初始化*/
     g_pub_tx.tx_amt1=0-g_pub_tx.tx_amt1;
     g_pub_tx.add_ind[0]='0';/* 增减控制金额0：减少；1：增加 */
     g_pub_tx.svc_ind=1006;/*  预授权  */
     g_pub_tx.hst_ind[0]='0';/* 不入明细 */
     memcpy(g_pub_tx.brf,tmpstr,20);
     if (pub_acct_trance())
     {
        sprintf(acErrMsg,"pos 预授权--错误!!!");
        WRITEMSG
        goto ErrExit;
     }
     strcpy(g_pub_tx.reply,"0000");
  } 
  
  if(ysq_ind[0]=='2') /*预授权撤销冲正*/
  {
      if (mChnl_single_hold.sts[0]!='2')
      {
         sprintf(acErrMsg,"要冲正的交易非预授权撤销");
         WRITEMSG
         Chnl_single_hold_Clo_Upd();
         strcpy(g_pub_tx.reply,"CU64");
         goto ErrExit;
      }
      vtcp_log("%s,%d,ac_no=%s,pt_date=%ld,pt_trace_no=%s",__FILE__,__LINE__, g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no);
      /*增加 bbmk 字段作为查询条件 shangyongchao 2012/6/7 11:03:38
      ret=sql_execute("update chnl_single_hold set sts='1' where ac_no='%s'  and pt_date=%ld and pt_trace_no='%s'",\
                       g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no);
      */
      ret=sql_execute("update chnl_single_hold set sts='1' where ac_no='%s'  and pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",\
                       g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no,mChnl_single_hold.bbmk);
      vtcp_log("%s,%d,预授权撤消冲正ac_no=[%s],pt_date=[%ld],pt_trace_no=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no,old_date,old_trace_no) ; 
      if (ret)
      {
        sprintf(acErrMsg,"更新表chnl_single_hold出错");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        Chnl_single_hold_Clo_Upd();
        goto ErrExit;
      }
    
     vtcp_log("%s,%d,预授权撤消冲正",__FILE__,__LINE__) ; 
     /* 冲正前的初始化 */
     strcpy(g_pub_tx.cur_no,"01");/*币种为人民币*/
     set_zd_data("0210",g_pub_tx.cur_no);
     strcpy(g_pub_tx.ct_ind,"2");
     /*g_pub_tx.tx_amt1 = 0-mChnl_single_hold.tx_amt;*/
     g_pub_tx.tx_amt1 = 0-mChnl_single_hold.hold_amt;
     g_pub_tx.ac_id = 0;
     g_pub_tx.add_ind[0] = '1'; /* 增加控制金额 */
     g_pub_tx.svc_ind = 1006;   /*  预授权完成  */
     g_pub_tx.hst_ind[0] = '0'; /* 不入明细 */
     memcpy(g_pub_tx.brf,tmpstr,20);
     if (pub_acct_trance())
     {
        sprintf(acErrMsg,"pos 预授权--错误!!!");
        Chnl_single_hold_Clo_Upd();
        WRITEMSG
        goto ErrExit;
     }
     Chnl_single_hold_Clo_Upd();
     strcpy(g_pub_tx.reply,"0000");
  }
  if(ysq_ind[0]=='3')/*预授权完成冲正*/
  {
      if (mChnl_single_hold.sts[0]!='3')
      {
         sprintf(acErrMsg,"要冲正的交易非预授权完成");
         WRITEMSG
         Chnl_single_hold_Clo_Upd();
         strcpy(g_pub_tx.reply,"CU64");
         goto ErrExit;
      }
      memcpy(mChnl_single_hold.brf,tmpstr,30);
      
      /*判断冲正金额与完成金额是否一致*/
     if (pub_base_CompDblVal(mChnl_single_hold.tx_amt,g_pub_tx.tx_amt1)!=0)
     {
         sprintf(acErrMsg,"冲正金额与完成金额不一致");
         WRITEMSG
         strcpy(g_pub_tx.reply,"CU06");
         goto ErrExit;
      }
      vtcp_log("[%s][%d],pt_date=[%ld],pt_trace=[%s]",__FILE__,__LINE__,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no);  
      /**增加bbmk 字段 shangyongchao 2012/6/7 11:05:06
      ret = sql_execute("update chnl_single_hold set sts='1', brf='%s' where ac_no='%s' and pt_date=%ld and pt_trace_no='%s'",\
      mChnl_single_hold.brf,g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no);
      */
      ret = sql_execute("update chnl_single_hold set sts='1', brf='%s' where ac_no='%s' and pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",\
      mChnl_single_hold.brf,g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no,mChnl_single_hold.bbmk);
      if(ret)
      {
        sprintf(acErrMsg,"更新原交易状态错!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU25");
        Chnl_single_hold_Clo_Upd();
        goto ErrExit; 
      }
      /* 冲正前的初始化 */
      /* 冲回帐户主文件的控制金额 */
      strcpy(g_pub_tx.cur_no,"01");
      set_zd_data("0210",g_pub_tx.cur_no);
      g_pub_tx.ac_id = 0;
     /*g_pub_tx.add_ind[0] = '1';*/ /* 取款 */
      g_pub_tx.add_ind[0] = '0';   /*同方向按负数记帐*/
      g_pub_tx.svc_ind = 1007;   /*  预授权完成  */
      g_pub_tx.hst_ind[0] = '1';
      g_pub_tx.tx_amt1 =0-g_pub_tx.tx_amt1;
      g_pub_tx.tx_amt2=0-mChnl_single_hold.hold_amt; /*恢复原控制金额*/
      memcpy(g_pub_tx.brf,tmpstr,20);
      if (pub_acct_trance())
      {
        sprintf(acErrMsg,"pos 预授权完成冲正--错误!!!");
        Chnl_single_hold_Clo_Upd();
        WRITEMSG
        goto ErrExit;
      }
     /*借方记帐*/
     set_zd_data("1021",g_pub_tx.crd_no);/*卡号*/
     set_zd_data("102J","01");/*币种*/
     set_zd_double("102F",g_pub_tx.tx_amt1);/*交易金额*/
     set_zd_data("102K","2");/*现转标记*/
     ret=pubf_acct_proc("D003");
     if (ret)
     {
       sprintf(acErrMsg,"调用D003借方记帐错误");
       WRITEMSG
       vtcp_log("%s,%d调用记帐程序错误",__FILE__,__LINE__);
       strcpy(g_pub_tx.reply,"T999");
       goto ErrExit;
     }
     vtcp_log("%s,%d,预授权完成冲正",__FILE__,__LINE__);
     Chnl_single_hold_Clo_Upd();
     
     /*插入卡登记簿*/
     tx_amt2=0-g_pub_tx.tx_amt1;/*完成金额冲正记负金额*/
     int atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */
     int pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */
     int pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */
     int atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */
     int dscpt = 0;
     dscpt = CARD_TRADE_TRAN;
     
     ret= pub_card_reg(tx_amt2,"POS",g_pub_tx.add_ind[0],&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,"",dscpt);
     if (ret)
     { 
       sprintf(acErrMsg,"更新卡登记簿错误");
       WRITEMSG
       strcpy(g_pub_tx.reply,"CU06");
       goto ErrExit;
     }
    
     /*判断商户帐号是否存在,状态是否正常*/
     if (strlen(cCom_acno)==0)
     {
        sprintf(acErrMsg,"未传入商户帐号");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT08");
        goto ErrExit;
      }
     if(strlen(cCom_acno)>10)
      {
        ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",cCom_acno);
        if (ret)
          {
            vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,cCom_acno);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
          }
        ret= Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld",g_mdm_ac_rel.ac_id);
        if (ret)
          {
             vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cCom_acno);
             strcpy(g_pub_tx.reply,"K106");
             goto ErrExit;
          }
         if (sPrdt_ac_id.ac_id_type[0]=='1')/*商户帐号如果是活期*/
          {
            /*查询商户帐号是否存在*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if (ret)
            {
              vtcp_log("%s,%d,ac_id[%s]查找错误!",__FILE__,__LINE__,sDd_mst.ac_id);
              strcpy(g_pub_tx.reply,"P102");
              goto ErrExit;
            }
            /*查询商户帐号状态是否正常*/
            if (sDd_mst.ac_sts[0]!='1')
            {
              vtcp_log("%s,%d,ac_no该账户不正常!",__FILE__,__LINE__);
              strcpy(g_pub_tx.reply,"O085");
              goto ErrExit;
            }
            set_zd_data("101A","01");/*币种*/
            set_zd_data("101B","2");/*现转标记贷方*/
            set_zd_double("1013",g_pub_tx.tx_amt1);/*交易金额记为负数*/
            set_zd_data("1011",cCom_acno);/*商户帐号*/
            strcpy(g_pub_tx.reply,"0001");/*调用D099活期贷方记帐*/
            
             
          }
         else if (sPrdt_ac_id.ac_id_type[0]=='9')/*如果商户帐号是内部帐号*/
          {
            ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if (ret)
              {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cCom_acno);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
              }
            if (sIn_mst.sts[0]!='1')
              {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cCom_acno);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
              }
            set_zd_data("1214","01");/*币种*/
            set_zd_data("1215","2");/*现转标志*/
            set_zd_double("1218",g_pub_tx.tx_amt1);/*交易金额*/
            set_zd_data("1211",cCom_acno);/*商户帐号*/
            strcpy(g_pub_tx.reply,"0002");/*调用A017内部帐号贷方记帐*/
            
            
          }
      }
     else if (strlen(cCom_acno)<=10)
     {
        ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cCom_acno);
        if(ret)
       {
          sprintf(acErrMsg,"查询科目失败!!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "B172");
          goto ErrExit;
       }
       set_zd_data("1214","01");/*币种*/
       set_zd_data("1215","2");/*现转标志*/
       set_zd_double("1218",g_pub_tx.tx_amt1);
       set_zd_data("1211",cCom_acno);/*科目号*/
       strcpy(g_pub_tx.reply,"0002");/*调用A017内部帐号贷方记帐*/
       
     }
    
  }
  ret = Return_bal();/*返回余额*/
  if(ret)
  {
          sprintf(acErrMsg,"返回余额错误!!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "D103");
          goto ErrExit;
  }
OkExit:
 /*strcpy(g_pub_tx.reply,"0000");*/
 sprintf(acErrMsg,"Success!!");
 WRITEMSG
 set_zd_data(DC_REPLY,g_pub_tx.reply); 
 return 0;
ErrExit:
 sprintf(acErrMsg,"failed!,reply is [%s]",g_pub_tx.reply);
 set_zd_data(DC_REPLY,g_pub_tx.reply);
 return 1;
 
}
/*返回余额的函数*/
int Return_bal()
{
  int ret=0;
  double today_amt=0.0;
  double account_amt=0.0,limit_amt=0.0;
  double ky_bal=0.0;
  char tmpstr[41];
  
  struct dd_mst_c sDd_mst;
  struct dd_parm_c sDd_parm;
  
  memset(tmpstr,0,sizeof(tmpstr));

  strcpy(g_pub_tx.cur_no,"01");
  
  memset(&sDd_mst,0,sizeof(sDd_mst));
  memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
  
  ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&sDd_mst);
  account_amt = sDd_mst.bal; /* 账面余额*/
  vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
  
  ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
  if(ret)
  {
      sprintf(acErrMsg,"查找活期产品参数表错误!!!");
      WRITEMSG
      strcpy(g_pub_tx.reply, "W015");
      return 1;
  }
  ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt - sDd_parm.min_bal;
  
  if (pub_base_CompDblVal(sDd_mst.bal,ky_bal)<0)
  {
    ky_bal = sDd_mst.bal;
  }
  if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2')
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
