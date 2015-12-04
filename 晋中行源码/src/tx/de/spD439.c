/***************************************************************
* 文 件 名: spD438.c
* 功能描述：七天乐解约功能
* 作    者: 邵理广
* 完成日期：2011年4月8日
 insert into tx_def values('0013','七天乐解约交易','10000000000000000000000110000100000000011100000000000000000001100001000000000010110000000000000000000000000000000000000000000100','1','6','0');
 insert into tx_flow_def values('0013','0','D439','#=0001@1@#$');
 insert into tx_flow_def values('0013','1','D202','#=0000@2@#$');
 insert into tx_flow_def values('0013','2','D203','#=0000@3@#$');
 insert into tx_flow_def values('0013','3','D205','#=0001@4@#$');
 insert into tx_flow_def values('0013','4','NULL','#=0001@5@#$');
 insert into tx_flow_def values('0013','5','D099','#$');
 insert into tx_sub_def values('D439','七天乐解约交易','spD439','0','0');
 
 insert into tx_sub_def values('D205','销户转开','spD205','0','0');
 
 insert into tx_sub_rel values('0013','D099','1','1','1012','账户序号');  --- by zyl 20110514
 insert into tx_sub_rel values('0013','D099','1','1','0191','启用0190');
 insert into tx_sub_rel values('0013','D099','1','0001','0192','控制标志');
 insert into tx_sub_rel values('0013','D099','1','1001','0193','普通存取');
 insert into tx_sub_rel values('0013','D099','0','102S+102P-102Q-1037+0410','1013','金额');
 insert into tx_sub_rel values('0013','D099','0','102J','101A','币种');
*修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
**************************************************************/
#define MYERR if( ret ) {\
    sprintf(acErrMsg,"SQLERR [%d]",ret);\
    WRITEMSG\
    goto ErrExit;\
}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "prdt_prot_parm_c.h"
#include "mo_prot_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "intst_hst_c.h"
#include "mdm_attr_c.h"
#include "intst_hst_c.h"
#include "prdt_ac_id_c.h"

spD439()
{
  int      ret=0;
  int      iRet=0;
  char     cOptype[2];          /*解约类型*/
  char     ac_no[20];
  char     cTx_br_no[8];        /*用于处理交易机构*/

  memset(cTx_br_no,0x00,sizeof(cTx_br_no));

  struct dd_mst_c          sApp_dd_mst,sMain_dd_mst;
  struct dd_parm_c         sApp_dd_parm,sMain_dd_parm;
  struct mdm_ac_rel_c      sMain_mdm_ac_rel,sApp_mdm_ac_rel;
  struct mo_prdt_prot_c    sMo_prdt_prot;
  struct prdt_prot_parm_c  sPrdt_prot_parm;
  struct mo_prot_ac_rel_c  sMo_prot_ac_rel;
  struct mdm_attr_c        sMain_mdm_attr,sApp_mdm_attr;
  struct S_pub_intst       sum_intst;

  memset(&sMain_dd_mst,    0x00, sizeof(struct dd_mst_c));
  memset(&sApp_dd_mst,    0x00, sizeof(struct dd_mst_c));
  memset(&sMain_dd_parm,   0x00, sizeof(struct dd_parm_c));
  memset(&sApp_dd_parm,    0x00, sizeof(struct dd_parm_c));

  memset(&sMain_mdm_ac_rel,    0x00, sizeof(struct mdm_ac_rel_c));
  memset(&sApp_mdm_ac_rel,    0x00, sizeof(struct mdm_ac_rel_c));
  memset(&sMo_prdt_prot,   0x00, sizeof(struct mo_prdt_prot_c));
  memset(&sMo_prot_ac_rel, 0x00, sizeof(struct mo_prot_ac_rel_c));
  memset(&sPrdt_prot_parm, 0x00, sizeof(struct prdt_prot_parm_c));
  memset(&sMain_mdm_attr,0x00,  sizeof(struct mdm_attr_c));
  memset(&sApp_mdm_attr,0x00, sizeof(struct mdm_attr_c));

  /*** 初始化全局变量 ***/
  pub_base_sysinit();

  /** 取值、赋值 **/
  get_zd_data("0300", ac_no);   /* 帐号 */
  pub_base_strpack(ac_no);
  pub_base_old_acno(ac_no);     /* 对旧帐号的处理 */

  get_zd_data("0680",cOptype);  /*解约类型*/

  /* 根据主帐号查询主账户序号和账户ID */
  ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMain_mdm_ac_rel, "ac_no='%s'", ac_no);
  if(ret == 100)
  {
    sprintf(acErrMsg, "不存在该帐号!!ac_no=[%s]", ac_no);
    WRITEMSG
    strcpy(g_pub_tx.reply, "AT01");
    goto ErrExit;
  }
  else if (ret)
  {
    sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错误!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  ret= Dd_mst_Sel(g_pub_tx.reply,&sMain_dd_mst,"ac_id=%ld",sMain_mdm_ac_rel.ac_id);
  if (ret == 100)
  {
    sprintf(acErrMsg, "不存在该帐号!!ac_id=[%s]", sMain_mdm_ac_rel.ac_id);
    WRITEMSG
    strcpy(g_pub_tx.reply, "AT01");
    goto ErrExit;
  }
  else if (ret)
  {
    sprintf(acErrMsg, "执行dd_mst_Sel错误!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  /*查询划款协议登记簿*/
  /*查询划款协议登记簿*/
  ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot,"prot_type='TK7' and sts in('1','2') and main_ac_id=%ld",sMain_mdm_ac_rel.ac_id);
  if (ret)
  {
    sprintf(acErrMsg, "查询划款协议登记簿出错!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"P176");
    goto ErrExit;
  }
 
  vtcp_log("留存余额=【%.2f】",sMo_prdt_prot.leave_amt);  /*前台写的是可用金额*/
  vtcp_log("化转金额=【%.2f】",sMo_prdt_prot.turn_amt);   /*前台写的是备用金额*/
  
  /*判断是否已经解约
  if(sMo_prdt_prot.sts[0]=='*'){
     sprintf(acErrMsg, "该协议已经解约！");
     WRITEMSG
     strcpy(g_pub_tx.reply,"B370");
     goto ErrExit;
  }*/
  /*查询账户对照表取出虚拟账户*/
  ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld and sts in('1','2') ",sMain_mdm_ac_rel.ac_id);
  if(ret)
  {
     sprintf(acErrMsg, "查询划款协议账户对照表出错[%d]", ret);
     WRITEMSG
     strcpy(g_pub_tx.reply, "X013");
     goto ErrExit;
  }
  vtcp_log("虚拟账户app_ac_id=【%d】",sMo_prot_ac_rel.app_ac_id);
    
  /*取虚拟账户*/
  ret=Dd_mst_Sel(g_pub_tx.reply,&sApp_dd_mst,"ac_id=%ld and ac_seqn=%d",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
  if(ret)
  {
     sprintf(acErrMsg, "查询活期主表出错[%d]", ret);
     WRITEMSG
     strcpy(g_pub_tx.reply, "B114");
     goto ErrExit;
  }
  vtcp_log("虚拟账户ac_id=【%d】",sApp_dd_mst.ac_id);
  vtcp_log("虚拟账户余额=【%.2f】",sApp_dd_mst.bal);
  vtcp_log("虚拟账户=【%d】",sApp_dd_mst.ac_id);

  memset(&sApp_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  ret=Mdm_ac_rel_Sel1(g_pub_tx.reply,&sApp_mdm_ac_rel,"ac_id=%ld",sApp_dd_mst.ac_id);
  if (ret && ret != 100)
  {
     sprintf(acErrMsg, "查询介质与账户关系表出错[%d]", ret);
     WRITEMSG
     strcpy(g_pub_tx.reply, "W011");
     goto ErrExit;
  }
  vtcp_log("虚拟账号ac_no=【%s】",sApp_mdm_ac_rel.ac_no);
  /*根据虚拟账户产品编号查询产品参数表取出利率编号*/
  memset(&sApp_dd_parm,0x00,sizeof(struct dd_parm_c));
  ret = Dd_parm_Sel(g_pub_tx.reply, &sApp_dd_parm, "prdt_no='%s'",sApp_dd_mst.prdt_no);
  if (ret)
  {
      sprintf(acErrMsg, "查询活期产品表错误ret=[%d]prdt_no=[%s]!!!",ret,sApp_dd_mst.prdt_no);
      WRITEMSG
      strcpy(g_pub_tx.reply, "W015");
      goto ErrExit;
  }
  vtcp_log("[%s][%d]虚拟账户利率编号=[%s],币种=[%s]",__FILE__,__LINE__,sApp_dd_parm.rate_no,sApp_dd_parm.cur_no);

  vtcp_log("cOptype=【%s】",cOptype);
  /*判断解约类型*/
  if(cOptype[0] == '0')/*正式解约 根据流程配置.向下执行.*/
  {
    /*更改协议登记薄状态*/      
    memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
    ret=Mo_prdt_prot_Dec_Upd(g_pub_tx.reply,"prot_type='TK7' and main_ac_id=%ld and sts in('1','2') ",sMain_mdm_ac_rel.ac_id);      
    if (ret)
    {
			sprintf(acErrMsg, "查询划款协议登记簿出错!");
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"P176");
	    goto ErrExit;
    }
    while(1)
    { 
      ret=Mo_prdt_prot_Fet_Upd(&sMo_prdt_prot,g_pub_tx.reply);
      if ( ret == 100 )
       break;
		  else if ( ret )
		  {
         sprintf(acErrMsg, "游标定义错误!");
         WRITEMSG
         strcpy(g_pub_tx.reply, "D101");
         goto ErrExit;
		  }
      
      sMo_prdt_prot.sts[0]='*';
      sMo_prdt_prot.tx_date=g_pub_tx.tx_date;
      ret=Mo_prdt_prot_Upd_Upd(sMo_prdt_prot,g_pub_tx.reply);
      if(ret)
      {
          sprintf(acErrMsg, "取主文件错误[%d]", ret);
          WRITEMSG
          strcpy(g_pub_tx.reply, "P200");
          goto ErrExit;
      }
    }
    Mo_prdt_prot_Clo_Upd();

    /*更新mo_prot_ac_rel状态*/
    memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
    ret=Mo_prot_ac_rel_Dec_Upd(g_pub_tx.reply,"prot_type='TK7' and main_ac_id=%ld and sts in('1','2') ",sMain_mdm_ac_rel.ac_id);
    if(ret)
    {
        sprintf(acErrMsg, "游标定义错误!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D101");
        goto ErrExit;
    }
    while(1)
    {
      ret=Mo_prot_ac_rel_Fet_Upd(&sMo_prot_ac_rel,g_pub_tx.reply);
      if ( ret == 100 )
       break;
		  else if ( ret )
		  {
         sprintf(acErrMsg, "游标定义错误!");
         WRITEMSG
         strcpy(g_pub_tx.reply, "D101");
         goto ErrExit;
		  }
      sMo_prot_ac_rel.sts[0]='*';
      sMo_prot_ac_rel.tx_date=g_pub_tx.tx_date;
      ret=Mo_prot_ac_rel_Upd_Upd(sMo_prot_ac_rel,g_pub_tx.reply);
      if (ret)
      {
          sprintf(acErrMsg, "取主文件错误[%d]", ret);
          WRITEMSG
          strcpy(g_pub_tx.reply, "P200");
          goto ErrExit;
      }
    }
    Mo_prot_ac_rel_Clo_Upd();
    /*查询虚拟账户介质属性表*/

    memset(&sApp_mdm_attr,0x00, sizeof(struct mdm_attr_c));
    ret=Mdm_attr_Sel(g_pub_tx.reply,&sApp_mdm_attr,"mdm_code=%s",sApp_mdm_ac_rel.mdm_code);
    if (ret && ret != 100)
    {
        sprintf(acErrMsg, "查询介质属性表出错[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N056");
        goto ErrExit;
    }

    /*查询主账户介质属性表*/
    memset(&sMain_mdm_attr,0x00,  sizeof(struct mdm_attr_c));
    ret=Mdm_attr_Sel(g_pub_tx.reply,&sMain_mdm_attr,"mdm_code=%s",sMain_mdm_ac_rel.mdm_code);
    if (ret && ret != 100)
    {
        sprintf(acErrMsg, "查询介质属性表出错[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N056");
        goto ErrExit;
    }

    /*为销户计息做准备*/
    set_zd_data("101A", g_pub_tx.cur_no);             /*币种*/
    set_zd_data("1021", sApp_mdm_ac_rel.ac_no);       /*从账号*/
    set_zd_int("1022", sMo_prot_ac_rel.app_ac_seqn);  /*从账户序号*/
    set_zd_data("1025", sApp_mdm_attr.draw_uncon_yn);
    set_zd_data("1026", sApp_mdm_ac_rel.draw_pwd_yn);
    set_zd_data("1027", sApp_mdm_ac_rel.qry_pwd);

    set_zd_data("1028", sApp_mdm_ac_rel.draw_pwd);
    set_zd_data("1029", sApp_mdm_ac_rel.draw_id_yn);
    set_zd_data("102G", sApp_mdm_ac_rel.id_type);         /*证件类型*/
    set_zd_data("102A", sApp_mdm_ac_rel.id_no);
    set_zd_data("102B", sApp_mdm_ac_rel.draw_seal_yn);
    set_zd_data("102C", sApp_mdm_ac_rel.pwd_mach_yn);
    /*set_zd_data("102D", g_pub_tx.draw_mach);
    set_zd_long("102E", &g_pub_tx.mach_date);*/
    set_zd_data("0191","0");                          /*0191结构*/
    set_zd_data("0192","0"); 
    set_zd_double("102I",sApp_dd_mst.bal);            /*余额*/
    set_zd_data("102X", "0");                         /**教育证明 1有0没有**/
    set_zd_data("102J", sApp_dd_parm.cur_no);
    set_zd_data("0030", sApp_dd_mst.opn_br_no);
    /*为销户准备*/
    set_zd_data("102K","2");                          /*转账销户*/
    set_zd_data("1011",sMain_mdm_ac_rel.ac_no);       /*销户时候要的转账账号为主账号*/
    set_zd_int("1012", sMo_prdt_prot.main_ac_seqn);
    set_zd_data("1014", sMain_mdm_attr.note_type);
    set_zd_data("1015", sMain_mdm_ac_rel.note_no);
    /*set_zd_double("1013",g_pub_tx.tx_amt1);*/
    set_zd_data("101B", "2");                         /*转账标志*/
    
    /*pub_mob_sendmail("七天通知存款账户解约", g_pub_tx.tx_code,sMain_mdm_ac_rel.ac_no,g_pub_tx.add_ind,0.0, 0.0 );*/
    pub_mob_sendmail("七天通知存款账户解约", g_pub_tx.tx_code,sMain_mdm_ac_rel.ac_no,"0",0.0, 0.0 );
    
    goto OkExit;
  }
  else if(cOptype[0] == '1')/*临时解约 在本程序中完成所有处理.*/
  {
      /*计算虚拟帐户的利息*/
      iRet=app_cal_intst(sMo_prdt_prot.sts,sApp_dd_mst,sApp_dd_parm);
      if (iRet)
      {
          sprintf(acErrMsg, "计算虚拟帐户利息错误ret=[%d]!!!", iRet);
          WRITEMSG
          goto ErrExit;
      }
      sum_intst.dealval=g_pub_intst.dealval;/*应付利息*/
      sum_intst.val=g_pub_intst.val;/*利息税*/

      vtcp_log("[%s][%d]g_pub_intst.dealval=[%.2lf],g_pub_intst.val=[%.2lf]",__FILE__,__LINE__,g_pub_intst.dealval,g_pub_intst.val);
      vtcp_log("[%s][%d]  sum_intst.dealval=[%.2lf],sum_intst.val=[%.2lf]",__FILE__,__LINE__,sum_intst.dealval,sum_intst.val);
      
      /*查询活期产品表*/
      memset(&sMain_dd_parm ,0x00,sizeof(struct dd_parm_c));
      iRet = Dd_parm_Sel(g_pub_tx.reply, &sMain_dd_parm, "prdt_no='%s'",sMain_dd_mst.prdt_no);
      if (iRet)
      {
          sprintf(acErrMsg, "查询活期产品表错误iRet=[%d]prdt_no=[%s]!!!",iRet,sApp_dd_mst.prdt_no);
          WRITEMSG
          goto ErrExit;
      }
      iRet=main_intst_acct(sMain_dd_mst,sMain_dd_parm,sum_intst);
      if (iRet)
      {
          sprintf(acErrMsg, "主账号记账处理失败iRet=[%d]!",iRet);
          WRITEMSG
          goto ErrExit;
      }
      /*****1111111进入从虚拟账户取款--划转本金******/
      vtcp_log("1111111进入取款");
      g_pub_tx.tx_amt1=sApp_dd_mst.bal;                /*虚拟账户余额*/
      g_pub_tx.ac_id=sApp_dd_mst.ac_id;                /*账户ID*/
      g_pub_tx.ac_seqn = 1;          /*账户序号*/
      strcpy(g_pub_tx.ac_no, sApp_mdm_ac_rel.ac_no);   /*账号*/
      strcpy(cTx_br_no,sApp_dd_mst.opn_br_no);         /*用于存款时的交易机构*/
      strcpy(g_pub_tx.tx_br_no,sApp_dd_mst.opn_br_no); /*交易机构*/
      strcpy(g_pub_tx.opn_br_no,sApp_dd_mst.opn_br_no);/*开户机构*/
      strcpy( g_pub_tx.cur_no,"01");
      strcpy(g_pub_tx.ac_get_ind, "000");              /*本程序未读取分户 */
      strcpy( g_pub_tx.ac_id_type,"1");                /*1存取款*/
      vtcp_log("赋值之后ac_id_type2=[%s]",g_pub_tx.ac_id_type);
      strcpy( g_pub_tx.add_ind,"0" );      /* 增加1，减0 */
      strcpy( g_pub_tx.ct_ind,"2" );       /* 现转:1现金2转帐 */
      strcpy( g_pub_tx.prt_ind,"0" );      /* 是否打印存折(有折无折标志):1是0否 */
      strcpy( g_pub_tx.hst_ind,"1" );      /* 入明细帐标志:1日间入2日终单笔入3日终汇总入*/
      strcpy( g_pub_tx.intst_type , "1" ); /*计息类型 0不计息1清理积数*/
      strcpy ( g_pub_tx.brf,"七天乐临时解约取款");
      vtcp_log("g_pub_tx.ac_id_type=【%s】",g_pub_tx.ac_id_type);
      g_pub_tx.svc_ind = 1001;
      /*** 交易记帐处理 ***/
      ret = pub_acct_trance();
      if (ret)
      {
        sprintf(acErrMsg, "交易记帐处理错误!调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
      }
      /* 取款方------会计记帐 */
      vtcp_log("序号是=【%ld】",g_pub_tx.ac_seqn);
      set_zd_data("102J", g_pub_tx.cur_no);    /*币种*/
      set_zd_data("1021",g_pub_tx.ac_no);      /*账号*/
      set_zd_int("1024",g_pub_tx.ac_seqn);     /*账户序号*/
      set_zd_data("102K", g_pub_tx.ct_ind);    /*现转标志*/
      set_zd_double("102F", g_pub_tx.tx_amt1); /*金额*/
      ret = pubf_acct_proc("D003");            /*取款记账*/
      if (ret!=0)
      {
        vtcp_log("%s,%d,记账不成功!ret[%ld]",__FILE__,__LINE__,ret);
        WRITEMSG
        goto ErrExit;
      }
      /*开始进入主账户存款记账*/
      vtcp_log("------22222开始进入存款记账");
      vtcp_log("[%d][%s]sApp_dd_mst.bal=[%.2lf]",__FILE__,__LINE__,sApp_dd_mst.bal);
      g_pub_tx.tx_amt1=sApp_dd_mst.bal;                     /*存款金额是虚拟账户里的余额*/
      g_pub_tx.ac_seqn=1;                /*主账户序号*/
      g_pub_tx.ac_id=sMain_dd_mst.ac_id;
      strcpy(g_pub_tx.ac_no, sMain_mdm_ac_rel.ac_no);       /*账号*/
      strcpy(g_pub_tx.tx_br_no,cTx_br_no);                  /*交易行号*/
      strcpy(g_pub_tx.opn_br_no,sMain_dd_mst.opn_br_no);    /*开户行号*/
      strcpy( g_pub_tx.cur_no,"01");
      strcpy(g_pub_tx.ac_get_ind, "00");                    /*本程序未读取分户*/ 
      strcpy( g_pub_tx.ac_id_type,"1" );                    /*1存取款*/
      vtcp_log("赋值之后ac_id_type2=[%s]",g_pub_tx.ac_id_type);
      strcpy( g_pub_tx.add_ind,"1" );       /* 增加1，减0 */
      strcpy( g_pub_tx.ct_ind,"2" );        /* 现转:1现金2转帐 */
      strcpy( g_pub_tx.prt_ind,"0" );       /* 是否打印存折(有折无折标志):1是0否 */
      strcpy( g_pub_tx.hst_ind,"1" );       /* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
      strcpy( g_pub_tx.intst_type , "1" );  /*计息类型 0不计息1清理积数*/
      strcpy ( g_pub_tx.brf,"七天乐临时解约存款");
      /* 存款方------会计记帐 */  
      g_pub_tx.svc_ind = 1001;
      /*** 交易记帐处理 ***/
      ret = pub_acct_trance();
      if (ret)
      {
        sprintf(acErrMsg, "交易记帐处理错误!调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
      }
      set_zd_data("101A", g_pub_tx.cur_no);             /*币种*/
      set_zd_data("1011",g_pub_tx.ac_no);               /*账号*/
      set_zd_int("1012",sMo_prot_ac_rel.main_ac_seqn);  /*账户序号*/
      set_zd_data("101B", g_pub_tx.ct_ind);             /*现转标志*/
      /**set_zd_double("102F",g_pub_tx.tx_amt1);   jk20121121        *金额*/
      set_zd_double("1013",g_pub_tx.tx_amt1);           /*金额*/
      ret = pubf_acct_proc("D099");                     /*存款记账*/
      if (ret!=0)
      {
        vtcp_log("%s,%d,记账不成功!ret[%ld]",__FILE__,__LINE__,ret);
        WRITEMSG
        goto ErrExit;
      }
      /* 登记交易流水 
      if( pub_ins_trace_log() )
      {
          sprintf(acErrMsg,"登记交易流水错误!");
          WRITEMSG
          goto ErrExit;
      }
      sprintf(acErrMsg,"登记交易流水 PASS!");
      WRITEMSG */
      
      /*更改协议登记薄状态为2*/
      memset(&sMo_prdt_prot,   0x00, sizeof(struct mo_prdt_prot_c));
      ret=Mo_prdt_prot_Dec_Upd(g_pub_tx.reply,"prot_type='TK7' and main_ac_id=%ld and sts='1'",sMain_mdm_ac_rel.ac_id);
      if (ret) {
         sprintf(acErrMsg, "游标定义错误!");
         WRITEMSG
         strcpy(g_pub_tx.reply, "D101");
         goto ErrExit;
      }
      ret=Mo_prdt_prot_Fet_Upd(&sMo_prdt_prot,g_pub_tx.reply);
      if (ret == 100) {
         sprintf(acErrMsg, "记录不存在!");
         WRITEMSG
         strcpy(g_pub_tx.reply, "D104");
         goto ErrExit;
      } else if (ret) {
          sprintf(acErrMsg, "查找记录出错[%d]", ret);
          WRITEMSG
          strcpy(g_pub_tx.reply, "D103");
          goto ErrExit;
      }
      sMo_prdt_prot.sts[0]='2';/*2临时解约*/
      sMo_prdt_prot.tx_date=g_pub_tx.tx_date;
      ret=Mo_prdt_prot_Upd_Upd(sMo_prdt_prot,g_pub_tx.reply);
      if (ret) {
        sprintf(acErrMsg, "取主文件错误[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "P200");
        goto ErrExit;
       }
      Mo_prdt_prot_Clo_Upd();
      
			pub_mob_sendmail( "七天通知存款账户临时解约", g_pub_tx.tx_code,sMain_mdm_ac_rel.ac_no,"0",0.0, 0.0 );

      goto OkExit1;
  }
  else
  {
      sprintf(acErrMsg, "类型错误!");
      WRITEMSG
      strcpy(g_pub_tx.reply, "C108");
      goto ErrExit;
  }


OkExit:
  strcpy(g_pub_tx.reply, "0001");
  sprintf(acErrMsg, "七天乐解约成功![%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
  return 0;
OkExit1:
  strcpy(g_pub_tx.reply, "0000");
  sprintf(acErrMsg, "七天乐临时解约成功![%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
  return 0;
ErrExit:
  sprintf(acErrMsg, "七天乐解约失败![%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
  return 1;
}

/*用到的函数*/
int app_cal_intst(cSts, dd_mst_c, dd_parm_c)
char   *cSts;
struct dd_mst_c  dd_mst_c;
struct dd_parm_c dd_parm_c;
{
    vtcp_log("[%s][%d],签约状态cSts=[%s]\n",__FILE__,__LINE__,cSts);
    int    ret=0;
    long   lDays=0;
    double dLx=0.00;       /*应付利息*/
    double dLxacm=0.00;    /*利息积数*/
    double dHq_rate=0.00; /*利率*/
    double dLxs=0.00;      /*利息税*/
    double dLxsl=0.00;     /*利息税税率*/
    double dSflx=0.00;     /*实付利息*/
    
    struct intst_hst_c T_intst_hst;
    struct dd_mst_c tmp_dd_mst;
    
    memset(&T_intst_hst, 0x00, sizeof(struct intst_hst_c));
    memset(&g_pub_intst, 0x0, sizeof(g_pub_intst));
    
    memcpy(&g_dd_mst, &dd_mst_c, sizeof(g_dd_mst));
    memcpy(&g_dd_parm, &dd_parm_c, sizeof(g_dd_parm));
    
    /*状态为1.签约的时候才可以临时解约*/
    {
        /*临时解约的要取当前的活期利率*/
        ret = pub_base_getllz("100","01",g_pub_tx.tx_date,&dHq_rate);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从公共利率表中取利率值错误 [%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"W110");
            goto ErrExit;
        }
        vtcp_log("[%s][%d],活期利率dHq_rate=[%.2lf]\n",__FILE__,__LINE__,&dHq_rate);
        /**滚积数结息**/
        vtcp_log("[%s][%d],cSts=[%s],dHq_rate=[%.2lf]\n",__FILE__,__LINE__,cSts,dHq_rate);
        dLx = (g_dd_mst.intst_acm * dHq_rate)/L360/L100;/**应付利息**/
        vtcp_log("[%s][%d],dLx=[%.2lf]\n",__FILE__,__LINE__,dLx);
        dLx = pub_base_PubDround_1(dLx);
        /*如果有利息税，是按活期利息税收还是按七天乐这种产品的利息税来收?*/
        if(strcmp(g_dd_parm.tax_no,SLBH) == 0)
        {
            ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &dLxsl);
            if ((ret != 0) && (ret == 1))
            {
                sprintf(acErrMsg,"利息税税率不存在 ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"W110");
                goto ErrExit;
            }
            dLxs=dLx*dLxsl;
            dLxs=pub_base_PubDround_1(dLxs) ;/**利息税**/
            vtcp_log("[%s][%d]汇总记帐dLx=[%.2lf]dLxs=[%.2lf]dLxsl=[%.2lf]",__FILE__,__LINE__,dLx,dLxs,dLxsl);
            dSflx=dLx-dLxs;
        }
        g_pub_intst.dealval=dLx;
        g_pub_intst.val=dLxs;
        vtcp_log("[%s][%d] sts=[%s],dealval=[%.2f],val=[%.2lf]",__FILE__,__LINE__,cSts,g_pub_intst.dealval,g_pub_intst.val);     
    }
    vtcp_log("[%s][%d],准备登记结息登记簿\n",__FILE__,__LINE__);
    strcpy(T_intst_hst.br_no, g_dd_mst.opn_br_no);
    T_intst_hst.ac_id = g_dd_mst.ac_id;
    T_intst_hst.ac_seqn = g_dd_mst.ac_seqn;
    strcpy(T_intst_hst.prdt_no, g_dd_mst.prdt_no);
    strcpy(T_intst_hst.cur_no, g_dd_parm.cur_no);
    T_intst_hst.bic_bal = g_dd_mst.bal;
    T_intst_hst.bic_acm = g_dd_mst.intst_acm;
    T_intst_hst.fct_intst = g_pub_intst.dealval;
    T_intst_hst.intst_tax = g_pub_intst.val;
    T_intst_hst.aic_bal = g_dd_mst.bal;
    T_intst_hst.aic_acm = 0.00;
    T_intst_hst.ic_date = g_dd_mst.ic_date;    
    T_intst_hst.intst_date = g_pub_tx.tx_date;
    strcpy(T_intst_hst.ac_ind, "1");
    strcpy(T_intst_hst.pt_ind, "1");
    T_intst_hst.rate = dHq_rate;
    T_intst_hst.intst_date=g_pub_tx.tx_date;
    ret = Intst_hst_Ins(T_intst_hst, g_pub_tx.reply);
    MYERR
    /*更新活期主文件*/
    ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%d and ac_seqn=%d",g_dd_mst.ac_id, g_dd_mst.ac_seqn);
    MYERR
    ret = Dd_mst_Fet_Upd(&tmp_dd_mst, g_pub_tx.reply);
    MYERR
    
    g_dd_mst.intst_acm = 0.00;/**应付利息积数**/
    g_dd_mst.lst_date=g_pub_tx.tx_date;
    g_dd_mst.ic_date=g_pub_tx.tx_date;
    
    ret = Dd_mst_Upd_Upd(g_dd_mst, g_pub_tx.reply);
    MYERR
    vtcp_log("[%s][%d],更新活期主表结束\n",__FILE__,__LINE__);
    Dd_mst_Clo_Upd();
    return 0;
ErrExit:
    return 1;
} 
   
int main_intst_acct(dd_mst_c, dd_parm_c, sum_intst)
struct dd_mst_c dd_mst_c;
struct dd_parm_c dd_parm_c;
struct S_pub_intst sum_intst;
{
    int  ret = 0;
    /**add by zyl 20110511**/
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id = %ld ",dd_mst_c.ac_id);
    if(ret)
    {
        sprintf(acErrMsg, "Mdm_ac_rel_Sel error! ret=[%ld]",ret);
        WRITEMSG
        goto ErrExit;
    }
    ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id=%ld ",dd_mst_c.ac_id);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在![%ld][%d]",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    strcpy(g_pub_tx.prdt_code,sPrdt_ac_id.prdt_no);
    strcpy(g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type);
    vtcp_log("zylsee134  ac_id_type=[%s]",sPrdt_ac_id.ac_id_type);
    /**end by zyl 20110511**/    
    strcpy(g_pub_tx.tx_br_no, dd_mst_c.opn_br_no);
    strcpy(g_pub_tx.opn_br_no, dd_mst_c.opn_br_no);
    strcpy(g_pub_tx.cur_no, dd_parm_c.cur_no);
    strcpy(g_pub_tx.ac_wrk_ind, "0000000");
    g_pub_tx.svc_ind = 1005;
    strcpy(g_pub_tx.ct_ind, "2");
    strcpy(g_pub_tx.add_ind, "1");
    strcpy(g_pub_tx.prdt_code, dd_parm_c.prdt_no);
    g_pub_tx.ac_id = dd_mst_c.ac_id;
    vtcp_log("zylsee12331 g_pub_tx.ac_id=[%ld]",g_pub_tx.ac_id);    
    g_pub_tx.ac_seqn = sMdm_ac_rel.ac_seqn;/**mod by zyl 20110512**/
    strcpy(g_pub_tx.note_type, "");
    strcpy(g_pub_tx.beg_note_no, "");
    g_pub_tx.tx_amt1 = sum_intst.dealval;
    strcpy(g_pub_tx.brf, "七天乐结息");
    strcpy(g_pub_tx.hst_ind, "1"); /*少一条明细20121210 by jk*/
    /* 进行利息的交易记帐处理 */
    ret = pub_acct_trance();
    if (ret)
    {
        sprintf(acErrMsg, "记账错误!!");
        WRITEMSG
        goto ErrExit;
    }    
    /* 进行利息税的交易记帐处理 */
    strcpy(g_pub_tx.brf, "利息税");
    strcpy(g_pub_tx.no_show, "0");
    strcpy(g_pub_tx.add_ind, "0");
    strcpy(g_pub_tx.ac_wrk_ind, "0000000");
    strcpy(g_pub_tx.hst_ind, "1");
    g_pub_tx.tx_amt1 = sum_intst.val;
    ret = pub_acct_trance();
    if (ret)
    {
        sprintf(acErrMsg, "记账错误!!");
        WRITEMSG
        goto ErrExit;
    }   
    /* 进行会计记帐 */
    set_zd_data(DC_CODE, dd_parm_c.dc_code);
    set_zd_double("102P", sum_intst.dealval-sum_intst.val);
    set_zd_double("102Q", sum_intst.val);
    set_zd_data("102J", dd_parm_c.cur_no);
    strcpy(g_pub_tx.prdt_code, dd_mst_c.prdt_no);

    strcpy(g_pub_tx.sub_tx_code,"Z032");/**临时解约状态.**/

    ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
    if (ret != 0)
    {
        sprintf(acErrMsg, "会计记帐不成功!!");
        WRITEMSG
        goto ErrExit;
    }
    return 0;
ErrExit:
    return 1;
}
