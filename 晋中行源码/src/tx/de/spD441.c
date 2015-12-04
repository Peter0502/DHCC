/***************************************************************
* 文 件 名: spD441.c
* 功能描述：晋中商行业务七天乐签约插入对照表
* 作    者: 薛珂
* 完成日期：2011年4月8日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:

insert into tx_sub_def values('D441','七天乐签约插入对照表','spD441','0','0');


**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "dd_mst_c.h"
#include "mo_prot_ac_rel_c.h"

spD441()
{
   int ret=0;
   char app_ac_no[21];

   struct dd_mst_c  sDd_mst;                /*活期主张户表*/
   struct mdm_ac_rel_c  sMdm_ac_rel;        /*介质与账户关系*/
   struct mo_prdt_prot_c  sMo_prdt_prot;    /*划款协议登记簿*/
   struct mo_prot_ac_rel_c  sMo_prot_ac_rel;/*划款协议账户对照表*/

   memset(&sDd_mst,         0x00 , sizeof(struct dd_mst_c));
   memset(&sMdm_ac_rel,     0x00 , sizeof(struct mdm_ac_rel_c));
   memset(&sMo_prdt_prot,   0x00 , sizeof(struct mo_prdt_prot_c));
   memset(&sMo_prot_ac_rel, 0x00 , sizeof(struct mo_prot_ac_rel_c));
   memset(app_ac_no,0x0,sizeof (app_ac_no));
  
   /*** 初始化全局变量 ***/
   pub_base_sysinit();
   
   /*取值、赋值*/
   get_zd_data("0300", g_pub_tx.ac_no);     /*主帐号*/
   get_zd_data("1039", app_ac_no);          /*从账号*/
   
   pub_base_strpack(app_ac_no);
   pub_base_strpack(g_pub_tx.ac_no);
   pub_base_old_acno(g_pub_tx.ac_no); /* 对旧帐号的处理 */
   
    /* 根据帐号查询账户序号和账户ID */
   ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
   if(ret)
    {
      sprintf(acErrMsg, "查询mdm_ac_rel错误!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"W011");
        goto ErrExit;
    }
   strcpy(sMo_prot_ac_rel.tx_br_no,g_pub_tx.tx_br_no);/*登记机构编码*/
   strcpy(sMo_prot_ac_rel.prot_type,"TK7");           /*协议类型*/
   sMo_prot_ac_rel.main_ac_id=sMdm_ac_rel.ac_id;      /*主账号*/
   sMo_prot_ac_rel.main_ac_seqn=sMdm_ac_rel.ac_seqn;  /*主账号序号*/
    
   /* 根据帐号查询虚拟账户序号和账户ID */
   memset(&sMdm_ac_rel,     0x00 , sizeof(struct mdm_ac_rel_c));
   ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",app_ac_no);
   if(ret)
    {
      sprintf(acErrMsg, "查询mdm_ac_rel错误!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"W011");
        goto ErrExit;
    }
   sMo_prot_ac_rel.app_ac_id=sMdm_ac_rel.ac_id;       /*从账号*/
   sMo_prot_ac_rel.app_ac_seqn=sMdm_ac_rel.ac_seqn;   /*从账号序号*/
   sMo_prot_ac_rel.tx_date=g_pub_tx.tx_date;          /*登记日期*/
   sMo_prot_ac_rel.trace_no=g_pub_tx.trace_no;        /*主机流水号*/
   strcpy(sMo_prot_ac_rel.tel,g_pub_tx.tel);          /*登记柜员*/
   strcpy(sMo_prot_ac_rel.sts,"1");                   /*状态: 1、正常；*、取消*/

   Mo_prot_ac_rel_Debug(&sMo_prot_ac_rel);

   ret = Mo_prot_ac_rel_Ins(sMo_prot_ac_rel,g_pub_tx.reply);
   if( ret )
    {  
      sprintf(acErrMsg, "插入mo_prot_ac_rel错误[%d]!!",ret);
      WRITEMSG
      strcpy(g_pub_tx.reply,"D105");
        goto ErrExit;
    }
       
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "7天乐签约插入对照表成功![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "7天乐签约插入对照表失败![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
    
}
