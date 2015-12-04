/*********************************************************************
* 版 本 号 :  V1.0        
* 程序作者 :  商永超          
* 日    期 :  2011-07-20   
* 所属模块 :  POS预授权完成撤销（只处理本行卡）           
* 程序名称 :  spF245.c
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
*insert into tx_def values('6245','预授权完成撤销','10000000000010000000000010000100000000000000000000000000000000000000000000000000011000000000000000000100000000000000000000000100','1','1','0','3',null);
*insert into tx_flow_def values('6245','0','F245','#=0000@1@#=0001@2@#$');
*insert into tx_flow_def values('6245','1','A016','#=0000@3@#$');
*insert into tx_flow_def values('6245','2','D003','#=0000@3@#$');
*insert into tx_flow_def values('6245','3','F000','#$');
*insert into tx_sub_def values('F245','预授权完成撤销','spF245','1','0');
*insert into tx_dc_rel values('F245','0000','F04200');
*insert into tx_sub_rel values('6245','A017','0','0020','0020','开户机构');
*insert into tx_sub_rel values('6245','A016','0','0020','0020','开户机构');
*insert into tx_sub_rel values('6245','D003','1','1001','0193','普通存取');
*insert into tx_sub_rel values('6245','D003','1','000110','0192','控制标志');
*insert into tx_sub_rel values('6245','D003','1','1','0191','启用0190');
下面这个有了就不用加了，F04200 用的原来的
*insert into dc_entry values('F04200','1','0210','@0152','0930','1','1','2','0420','POS预授权完成撤销');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "chnl_single_hold_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "in_mst_c.h"
int spF245()
{
  int ret;
  char flag;
  long lOld_pt_date;/*原平台日期*/
  char cOld_pt_trace_no[13]/*原平台流水*/;
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
  
  char cYsq_ind[2];/*预授权标志*/
  char cYsq_no[11];/*预授权号*/
  /*char cDc_ind[2];借贷标志*/
  char cFee_ind[2];/*POS标志，0为本行POS，1为他行POS*/
  /*double dFee_amt=0.00;暂时没用到*/
  long lPt_date=0;/*平台日期*/
  char cPt_time[7];/*平台时间*/
  char cPt_trace_no[13];/*平台流水号*/  
  char cBrf[51];/*摘要*/
  char cDz_ref[77];/*对账参考数据*/
  char cHx_term[5];/*终端号*/    
  char cMsr_str2[38];/*2磁道信息*/
  char cMsr_str3[142];/*3磁道信息*/
  char cTmp_str[51];
  double dTx_amt2=0.00;
  char cAc_no[25];
  char cTmp_brf[21];
  char cLs_ac_no[25];
  /*调用pub_com_test函数时，接收返回值*/
  char cGz_flag[2];/*挂账标志*/
  
  double account_amt=0.0;/*账面余额*/
  double ky_bal=0.0;/*可用余额*/
  char tmpstr[41];
  struct chnl_single_hold_c sChnl_single_hold_old;/*预授权完成的记录*/
  struct chnl_single_hold_c sChnl_single_hold_ysq;/*预授权记录*/
  struct chnl_single_hold_c sChnl_single_hold_new;/*新的记录*/
  struct mdm_ac_rel_c  sMdm_ac_rel;
  struct dd_mst_c sDd_mst;
  struct mdm_ac_rel_c sMdm_ac_rel_sh;
  struct prdt_ac_id_c sPrdt_ac_id;
  struct com_item_c sCom_item;
  struct in_mst_c sIn_mst;
  
  /*
  struct march_info_c sMarch_info;
  struct march_mst_c sMarch_mst;
  
  memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
  memset(&sMarch_info,0x00,sizeof(sMarch_info));
  */
  memset(cOld_pt_trace_no,0,sizeof(cOld_pt_trace_no));
  memset(cYsq_no,0,sizeof(cYsq_no));
  memset(cYsq_ind,0,sizeof(cYsq_ind));
  /*memset(cDc_ind,0,sizeof(cDc_ind));*/
  memset(cFee_ind,0,sizeof(cFee_ind));
  memset(cPt_time,0,sizeof(cPt_time));
  memset(cPt_trace_no,0,sizeof(cPt_trace_no));
  memset(cBrf,0,sizeof(cBrf));
  memset(cDz_ref,0,sizeof(cDz_ref));
  memset(cTmp_str,0,sizeof(cTmp_str));
  memset(cHx_term,0,sizeof(cHx_term));
  memset(cMsr_str2,0,sizeof(cMsr_str2));
  memset(cMsr_str3,0,sizeof(cMsr_str3));  
  memset(cCtrlcmd,0,sizeof(cCtrlcmd));
  memset(cAc_no,0,sizeof(cAc_no));
  memset(cLs_ac_no,0x00,sizeof(cLs_ac_no)); 
  memset(cTmp_brf,0,sizeof(cTmp_brf));
  memset(tmpstr,0,sizeof(tmpstr));
  memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
  memset(&sChnl_single_hold_new,0x0,sizeof(struct chnl_single_hold_c));
  memset(&sChnl_single_hold_ysq,0x0,sizeof(struct chnl_single_hold_c));
  memset(&sChnl_single_hold_old,0x0,sizeof(struct chnl_single_hold_c));
  memset(&sDd_mst,0x0,sizeof(struct dd_mst_c));
  memset(&sMdm_ac_rel_sh,0x0,sizeof(struct mdm_ac_rel_c));
  memset(&sPrdt_ac_id,0x0,sizeof(struct prdt_ac_id_c));
  memset(&sCom_item,0x0,sizeof(struct com_item_c));
  memset(&sIn_mst,0x0,sizeof(struct in_mst_c));
  /*调用pub_com_test时用到*/
  memset(cGz_flag,0x00,sizeof(cGz_flag));
  /*
  memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
  */
  
  /*初始化*/
  if (pub_base_sysinit())
  {
    sprintf(acErrMsg,"初始化公用结构错误!!");
    WRITEMSG
    goto ErrExit;
  }
  vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
  set_zd_long("0460",g_pub_tx.tx_date);
    
  get_zd_long("0440",&lPt_date);/*平台日期*/
  vtcp_log("%s,%d,平台日期=[%ld]",__FILE__,__LINE__,lPt_date);
  get_zd_data("0170",cPt_time);/*平台时间*/
  vtcp_log("%s,%d,平台时间=[%s]",__FILE__,__LINE__,cPt_time);
  get_zd_data("0520",cPt_trace_no);/*平台流水*/
  vtcp_log("%s,%d,平台流水=[%s]",__FILE__,__LINE__,cPt_trace_no);
  get_zd_data("0100",cHx_term);/*终端号*/
  vtcp_log("%s,%d,终端号=[%s]",__FILE__,__LINE__,cHx_term);
  get_zd_data("0700",cYsq_ind);/*预授权标志 4位预授权完成撤销*/
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
  get_zd_data("0810",cTmp_str);
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

  vtcp_log("%s,%d,撤销金额[%.2f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
 /********   
  get_zd_data("0790",g_pub_tx.draw_pwd);交易密码
******/
  /*vtcp_log("%s,%d,交易密码=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
  /*get_zd_double("0410",&dFee_amt);*/
  get_zd_data("0310",cAc_no);/*账号*/
  vtcp_log("%s,%d,cAc_no[%s]",__FILE__,__LINE__,cAc_no);
  
  get_zd_data("0690",cFee_ind);/*本行他行POS标志，0为本行POS,1为他行POS*/
  vtcp_log("%s,%d,POS标志=[%c]",__FILE__,__LINE__,cFee_ind[0]);
  memcpy(g_pub_tx.ac_no1,cAc_no,strlen(cAc_no)); /*第二个帐号*/
  /* get_zd_data("0730",chnl_marchinfo.mrchno); 商户号 */
  memcpy(cTmp_brf,cTmp_str,20);   
  

  /*检查预授权标志是否为预授权完成撤销*/
  vtcp_log("%s,%d,预授权标志【%c】",__FILE__,__LINE__,cYsq_ind[0]);
  if(cYsq_ind[0] != '4')
  {
        sprintf(acErrMsg,"预授权标志错误");
      WRITEMSG
      goto ErrExit;
  }
  /* 检查卡的合法性 */  
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
  /* 查询预授权完成的纪录 */
  /*增加bbmk 字段 商永超2012/6/7 11:30:32
  ret = Chnl_single_hold_Sel(g_pub_tx.reply,&sChnl_single_hold_old,"ac_no='%s' and pt_date=%ld and pt_trace_no= '%s' and premise_no='%s'",\
  g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no);
  **/
  ret = Chnl_single_hold_Sel(g_pub_tx.reply,&sChnl_single_hold_old,\
        "ac_no='%s' and pt_date=%ld and pt_trace_no= '%s' and premise_no='%s' and bbmk='%s'",\
        g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no,cHx_term);
  if (ret)
  { 
    sprintf(acErrMsg,"预授权完成的记录不存在!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
  
  /*检查原纪录状态是否为完成状态*/
  if (sChnl_single_hold_old.sts[0]!='3')
  {
      sprintf(acErrMsg,"记录状态不正常!!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"CU06");
      goto ErrExit;
  }
  /*检查金额*/
  if(pub_base_CompDblVal(sChnl_single_hold_old.tx_amt, g_pub_tx.tx_amt1)!=0)
  {
    sprintf(acErrMsg,"撤销金额与完成金额不一直!!");
    vtcp_log("%s,%d,完成金额=[%f]",__FILE__,__LINE__,sChnl_single_hold_old.tx_amt);
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
  /*按预授权完成记录的 预授权平台日期、预授权平台流水、预授权号查找原预授权记录*/
  /*增加bbmk字段 shangyongchao 2012/6/7 11:34:09 改为游标更新
  ret = Chnl_single_hold_Sel(g_pub_tx.reply,&sChnl_single_hold_ysq,"ac_no='%s' and pt_date =%ld and pt_trace_no = '%s' and premise_no='%s'",\
      g_pub_tx.crd_no,sChnl_single_hold_old.o_pt_date,sChnl_single_hold_old.o_pt_trace_no,cYsq_no);*/ 
  ret = Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and pt_date =%ld and pt_trace_no = '%s' and premise_no='%s' and bbmk='%s'",\
      g_pub_tx.crd_no,sChnl_single_hold_old.o_pt_date,sChnl_single_hold_old.o_pt_trace_no,cYsq_no,sChnl_single_hold_old.bbmk);
  if (ret)
  {
    sprintf(acErrMsg,"定义游标出错!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
  ret = Chnl_single_hold_Fet_Upd(&sChnl_single_hold_ysq,g_pub_tx.reply);
  if (ret)
  {
    sprintf(acErrMsg,"预授权的记录不存在!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
  vtcp_log("%s,%d,查到原预授权的记录了平台日期【%ld】平台流水为[%s]",__FILE__,__LINE__,sChnl_single_hold_ysq.pt_date,sChnl_single_hold_ysq.pt_trace_no);
  
  sChnl_single_hold_ysq.tx_amt=0.00;
  sChnl_single_hold_ysq.sts[0]='1';
  
  /*更新预授权记录的状态为1,完成金额为0*/
  /*改为游标更新
  ret = sql_execute("update chnl_single_hold set sts='1',tx_amt=0 where ac_no='%s' and pt_date=%ld and pt_trace_no= '%s' and premise_no='%s'\
            ",g_pub_tx.crd_no,sChnl_single_hold_old.o_pt_date,sChnl_single_hold_old.o_pt_trace_no,cYsq_no);
  */
  ret = Chnl_single_hold_Upd_Upd(sChnl_single_hold_ysq,g_pub_tx.reply);
  if (ret)
  {
     sprintf(acErrMsg,"更新预授权纪录错误!!");
     WRITEMSG
     strcpy(g_pub_tx.reply,"CU06");
     goto ErrExit;
    
  }
  Chnl_single_hold_Clo_Upd();
  vtcp_log("%s,%d,更新完原预授权记录的状态了（状态置为1，预授权状态）",__FILE__,__LINE__);
  /* 给单笔预授权新纪录结构赋值 */  
  sChnl_single_hold_new.tx_date = g_pub_tx.tx_date;/*核心日期*/
  sprintf(sChnl_single_hold_new.tx_time,"%d",g_pub_tx.tx_time);/*核心交易时间*/
  sChnl_single_hold_new.trace_no = g_pub_tx.trace_no;/*核心流水号*/
  strcpy(sChnl_single_hold_new.cur_no,g_pub_tx.cur_no);/*币种，01*/
  strcpy(sChnl_single_hold_new.ac_no,g_pub_tx.crd_no);/*卡号*/
  strcpy(sChnl_single_hold_new.tx_code,g_pub_tx.tx_code);/*交易代码*/
  strcpy(sChnl_single_hold_new.tx_br_no,g_pub_tx.tx_br_no);/*交易机构*/
  strcpy(sChnl_single_hold_new.opn_br_no,g_pub_tx.tx_br_no);/*开户机构*/
  strncpy(sChnl_single_hold_new.tel,g_pub_tx.tel,6);/*柜员号*/
  /*strcpy(sChnl_single_hold_new.bbmk,"COMM");系统种类*/
  strcpy(sChnl_single_hold_new.bbmk,cHx_term);/*系统种类 改为从10 域传送 shangyongchao 2012/6/7 11:29:25*/
  strcpy(sChnl_single_hold_new.ttyn,cHx_term);/*终端号*/
  sChnl_single_hold_new.pt_date = lPt_date;/*平台日期*/
  strcpy(sChnl_single_hold_new.pt_time,cPt_time);/*平台时间*/
  strcpy(sChnl_single_hold_new.pt_trace_no,cPt_trace_no);/*平台流水*/
  strcpy(sChnl_single_hold_new.premise_no,sChnl_single_hold_old.premise_no);/*预授权号*/
  sChnl_single_hold_new.hold_amt = sChnl_single_hold_old.hold_amt;/*控制金额*/
  sChnl_single_hold_new.tx_amt = g_pub_tx.tx_amt1;/*交易金额*/
  strcpy(sChnl_single_hold_new.brf,cBrf);/*摘要*/
  strcpy(sChnl_single_hold_new.sts,"4");/*预授权完成撤销状态*/
  sChnl_single_hold_new.o_tx_date = sChnl_single_hold_ysq.tx_date;/*预授权核心日期*/
  sprintf(sChnl_single_hold_new.o_trace_no,"%d",sChnl_single_hold_ysq.trace_no);/*预授权核心流水*/
  sChnl_single_hold_new.o_pt_date = sChnl_single_hold_ysq.pt_date;/*预授权平台时间*/
  strcpy(sChnl_single_hold_new.o_pt_trace_no,sChnl_single_hold_ysq.pt_trace_no);/*预授权平台流水*/
  strcpy(sChnl_single_hold_new.dz_ind,"0");/*对账标志*/
  strcpy(sChnl_single_hold_new.dz_ref,cDz_ref); /*对账参考*/   
      
  Chnl_single_hold_Debug(&sChnl_single_hold_new);
  ret = Chnl_single_hold_Ins(sChnl_single_hold_new,g_pub_tx.reply);
  if (ret)
  {
    sprintf(acErrMsg,"pos 插入单笔预授权登记簿失败!!");
    WRITEMSG
    goto ErrExit;
  }
  
  /* 更改卡金额 存款 */
  strcpy(g_pub_tx.cur_no,"01");
  set_zd_data("0210",g_pub_tx.cur_no);
  /*根据币种生成账户序号*/
  ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
  if ( ret != 0)
  {
    sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]",g_pub_tx.cur_no);
    WRITEMSG
    goto ErrExit;
  }
  g_pub_tx.tx_amt1=sChnl_single_hold_old.tx_amt; /*交易金额*/
  g_pub_tx.tx_amt2= sChnl_single_hold_old.hold_amt; /*恢复控制金额*/
  g_pub_tx.svc_ind = 1007;   /*  预授权完成  */ 
  g_pub_tx.add_ind[0] = '1';
  /*g_pub_tx.add_ind[0] = '0';*/

  g_pub_tx.hst_ind[0] = '1'; /* 日间入明细 */
  g_pub_tx.ac_id = 0;
  strcpy(g_pub_tx.ct_ind,"2"); /* 转帐 */
  memcpy(g_pub_tx.brf,cTmp_str,20);
  if (pub_acct_trance())
  {
    sprintf(acErrMsg,"卡用户存款错误!!!");
    WRITEMSG
    goto ErrExit;
  }
  /*此处给分录entry_code=F0420赋值*/
  set_zd_double("0420",g_pub_tx.tx_amt1);
  set_zd_data("0930","2");
  /*********************
  set_zd_data("1201",cAc_no);    

  set_zd_data("120A",cTmp_brf);
  set_zd_data("1205","2");
  set_zd_data("1204","01");
  set_zd_double("1208",g_pub_tx.tx_amt1);
  ******************/
/***#endif****/
    /*对传来的账号不同做相应的处理流程*/
    if(strlen(cAc_no) >= 13)   /**商户账号*/
    {
        ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_sh," ac_no= '%s'",cAc_no);
        if( ret )
         {
             vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,cAc_no);
             strcpy(g_pub_tx.reply,"P102");
             goto ErrExit;
         }
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",sMdm_ac_rel_sh.ac_id);
        if (ret)
         {
             vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_id错误!",__FILE__,__LINE__,cAc_no);
             strcpy(g_pub_tx.reply,"K106");
             goto ErrExit;
         }
         if(sPrdt_ac_id.ac_id_type[0] == '1')  /*活期*/
          {
              ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel_sh.ac_id);
              if(ret)
                {
                       vtcp_log("%s,%d,ac_id[%s]查找错误!",__FILE__,__LINE__,sMdm_ac_rel_sh.ac_id);
                       strcpy(g_pub_tx.reply,"P102");
                       goto ErrExit;
                   }
              if(sDd_mst.ac_sts[0] != '1')
                {
                       vtcp_log("%s,%d,ac_no该账户不正常!",__FILE__,__LINE__);
                       strcpy(g_pub_tx.reply,"O085");
                       goto ErrExit;
                  }
              set_zd_data("1021",cAc_no);
              set_zd_data("102K","2");
              set_zd_double("102F",g_pub_tx.tx_amt1);
              strcpy(g_pub_tx.reply,"0001");
          }
         else if(sPrdt_ac_id.ac_id_type[0] == '9') /*内部账号*/
          {
            ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",sMdm_ac_rel_sh.ac_id);
              if(ret)
                {
                       vtcp_log("%s,%d,ac_id[%s]查找错误!",__FILE__,__LINE__,sMdm_ac_rel_sh.ac_id);
                       strcpy(g_pub_tx.reply,"P102");
                       goto ErrExit;
                   }
              if(sIn_mst.sts[0] != '1')
                {
                       vtcp_log("%s,%d,ac_no该账户不正常!",__FILE__,__LINE__);
                       strcpy(g_pub_tx.reply,"O085");
                       goto ErrExit;
                  }
              set_zd_data("1201",cAc_no);    
              set_zd_data("120A",cTmp_brf);
              set_zd_data("1205","2");
              set_zd_data("1204","01");
              set_zd_double("1208",g_pub_tx.tx_amt1);
              strcpy(g_pub_tx.reply,"0000");
          }
         else
          {
              sprintf(acErrMsg,"卡账户类型错误!!!");
              WRITEMSG
              goto ErrExit;
          }
    }
    else if(strlen(cAc_no)<13)
    {
        ret= Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no='%s'",cAc_no);
        if(ret)
        {
            sprintf(acErrMsg,"查询科目失败!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
        set_zd_data("1201",cAc_no);
        set_zd_data("120A",cTmp_brf);
        set_zd_data("1205","2");
        set_zd_data("1204","01");
        set_zd_double("1208",g_pub_tx.tx_amt1);
        strcpy(g_pub_tx.reply,"0000");
    }
    else
    {
        sprintf(acErrMsg,"未传入商户账号或待清算账号!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }   
         

  int atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */
  int pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */
  int pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */
  int atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */
  int dscpt = 0;
  int iselflag=0;
  dscpt = CARD_TRADE_TRAN;/*01*/
  vtcp_log("%s,%d,POS标志=[%c]",__FILE__,__LINE__,cFee_ind[0]); 
  if(cFee_ind[0] == '0')
  {
    iselflag = 0;
  }
  else 
  {
    iselflag = 1;
  }
  vtcp_log("%s,%d,参数iselflag====%d",__FILE__,__LINE__,iselflag);
 
  
  /*撤销交易更新card_reg，时借方记负数*/
  
  ret = pub_card_reg(-1 * g_pub_tx.tx_amt1,"POS",'0',&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,iselflag,dscpt);
  if (ret)
  {
    sprintf(acErrMsg,"插入卡登记簿失败!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
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
  ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
  if (pub_base_CompDblVal(sDd_mst.bal,ky_bal)<0)
  {
    ky_bal = sDd_mst.bal;
  }
  vtcp_log("%s,%d,可用金额[%lf],余额[%lf]",__FILE__,__LINE__,ky_bal,account_amt);
  memcpy(tmpstr,0,sizeof(tmpstr));
  memcpy(tmpstr,"1001156C",8);
  sprintf(tmpstr+8,"%012.0f",account_amt*100);
  memcpy(tmpstr+20,"1002156C",8);	/**modify 2012-04-12**/
  sprintf(tmpstr+28,"%012.0f",ky_bal*100);
  vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
  set_zd_data("0820",tmpstr);
  set_zd_long("0040",sChnl_single_hold_new.trace_no);/*返回核心流水号*/
    /*返回余额结束*/
    
OkExit:
    /*strcpy( g_pub_tx.reply, "0000" ); 如果返回0000 就走不到记分户账的分支了 2011/12/1 16:18:52*/
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
