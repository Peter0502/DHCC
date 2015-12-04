/*****************************************************
* 文 件 名:  sp7025.c                              ***
* 功能描述： 校验授权柜员                          ***
* 作    者:  zhangtilei                            ***
* 完成日期： 2012-10-23 14:50:10                   ***
*                                                  ***
* 修改记录：                                       ***
*   日   期:                                       ***
*   修 改 人:                                      ***
*   修改内容:                                      ***
insert into tx_def values ('7025', '校验授权柜员', '10000000000000000000110001000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000', '0', '5', '2',NULL,NULL);
insert into tx_flow_def values ('7025', 0, '7025', '#$');
insert into tx_sub_def values ('7025', '校验授权柜员', 'sp7025', '0', '0');
*****************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int sp7025()
{
   int    iRet = 0;
   char   cTelj[TEL_LENGTH+1]; /*交易柜员*/
   char   cTels[TEL_LENGTH+1]; /*授权柜员*/
   char   cPwd[7];  /*密码*/
    
   struct com_tel_c sCom_tel;
   struct com_tel_c sCom_tel1;
   
   memset(&sCom_tel, 0x00, sizeof(sCom_tel));
   memset(&sCom_tel1, 0x00, sizeof(sCom_tel1));
   memset(cTelj, 0x00, sizeof(cTelj));
   memset(cTels, 0x00, sizeof(cTels));
   memset(cPwd, 0x00, sizeof(cPwd));
   
   get_zd_data("0070",cTelj);
   get_zd_data("0920",cTels);
   get_zd_data("0790",cPwd);
   
   vtcp_log("%s,%d,0070=[%s]",__FILE__,__LINE__,cTelj);
   vtcp_log("%s,%d,0920=[%s]",__FILE__,__LINE__,cTels);
   vtcp_log("%s,%d,0790=[%s]",__FILE__,__LINE__,cPwd);
   /*初始化全局变量*/
    iRet = pub_base_sysinit();
    if(iRet)
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
   
   /*查询授权柜员基本信息*/
   iRet=Com_tel_Sel(g_pub_tx.reply, &sCom_tel, "tel='%s'", cTels);
   if(iRet)
    {
       sprintf(acErrMsg, "执行Com_tel_Sel错误!ret=[%d]",iRet);
       WRITEMSG
       strcpy(g_pub_tx.reply, "S009");
       goto ErrExit;
    }
   if(strcmp(g_pub_tx.tx_br_no,sCom_tel.br_no))   /*所属机构是否相等*/
   {
      sprintf(acErrMsg,"授权柜员和交易柜员所属不同");
      WRITEMSG
      strcpy(g_pub_tx.reply,"S083");
      goto ErrExit;
   }
   pub_base_EnDes(g_pub_tx.tx_date, "",cPwd);   /*授权柜员密码是否正确*/
   /*if (strcmp(cPwd, sCom_tel.pwd) != 0) */
   if ( memcmp(cPwd, sCom_tel.pwd,strlen(sCom_tel.pwd)) ) 
   {
      sprintf(acErrMsg, "密码输入不正确!tel=[%s]", cTels);
      WRITEMSG
      strcpy(g_pub_tx.reply, "M005");
      goto ErrExit;
   }
   /*检查授权柜员状态*/
   /*if(sCom_tel.csts[0] != '0')                 
   {
      sprintf(acErrMsg, "授权柜员未签到!", cTels);
      WRITEMSG
      strcpy(g_pub_tx.reply, "S017");
      goto ErrExit;
   }*/
    
   memset(&sCom_tel1, 0x00, sizeof(sCom_tel1));
   iRet=Com_tel_Sel(g_pub_tx.reply, &sCom_tel1, "tel='%s'", cTelj);
   if(iRet)
   {
       printf(acErrMsg, "执行Com_tel_Sel错误!ret=[%d]",iRet);
       WRITEMSG
       strcpy(g_pub_tx.reply, "S009");
       goto ErrExit;
   }
   if(sCom_tel1.lvl[0] <= sCom_tel.lvl[0])            /*检查授权柜员级别*/
   {
      vtcp_log("%s,%d,交易柜员级别%s不能小于授权柜员%s",__FILE__,__LINE__,sCom_tel1.lvl , sCom_tel.lvl);
      sprintf(acErrMsg,"除了凭证管理员，不能查询比自己级别高的柜员的凭证");
      WRITEMSG
      strcpy(g_pub_tx.reply,"Z10K");
      goto ErrExit;     
   }
OkExit:
  strcpy(g_pub_tx.reply, "0000");
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

