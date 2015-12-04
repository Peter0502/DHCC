/*************************************************
* 文 件 名:  sp4999.c
*所属模块 :  与平台的接口程序 
* 功能描述： 柜员密码检查
*
* 作    者:  shaoliguang
* 完成日期： 
* 程序作用：用于需要检查柜员密码的场合，如二次登录或一些复核交易的联机检查柜员。在行内不记流水，不记录登记簿。
* 
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
  insert into tx_def values('6231','平台柜员密码检查','10000000000000000000000010000000000000000000000000000000000000000011100000000000000000000001000000000000000000000000000000000100','0','6','0','3',null);
  insert into tx_flow_def  values ('6231', 0, '4999', '#$');
  insert into tx_sub_def  values ('4999', '平台柜员密码检查', 'sp4999', '0', '0');
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

sp4999()
{
    int   iRet=0;
    char  cTel[7];  /*柜员号*/
    char  cPsw[17]; /*柜员密码*/
    char  cBrf[51]; /*摘要*/
    
    memset( cTel, 0x00 ,sizeof( cTel ));
    memset( cPsw, 0x00 ,sizeof( cPsw ));
    memset( cBrf, 0x00 ,sizeof( cBrf ));
    
    struct com_tel_c sCom_tel;
    memset(&sCom_tel,0x00,sizeof(struct com_tel_c));
    
    /*初始化变量*/
    pub_base_sysinit();
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    /*获取从平台传来的值*/
    get_zd_data("0070",cTel);
    get_zd_data("0790",cPsw);
    get_zd_data("0810",cBrf);
    
    /*vtcp_log("%s,%d 密码=[%s]\n",__FILE__,__LINE__,cPsw);*/

    vtcp_log("%s,%d 柜员号=[%s]\n",__FILE__,__LINE__,cTel);
    WRITEMSG

    iRet = Com_tel_Dec_Sel(g_pub_tx.reply, " tel='%s' ", cTel);
    if (iRet)
    {
       sprintf(acErrMsg, "柜员不存在[%s]", cTel);
       WRITEMSG
       strcpy(g_pub_tx.reply, "O120");
       goto ErrExit;
    }
    iRet = Com_tel_Fet_Sel(&sCom_tel,g_pub_tx.reply);
    if (iRet)
    {
       sprintf(acErrMsg, "柜员不存在[%s]", cTel);
       WRITEMSG
       strcpy(g_pub_tx.reply, "O120");
       goto ErrExit;
    }

    /*柜员密码出现空值,此处控制 */
    if( !strcmp(pub_base_strpack(sCom_tel.pwd),'') )
    {
      sprintf(acErrMsg, "口令为空,请与科技科联系!![%s][%s]", cPsw,sCom_tel.pwd);
      WRITEMSG
      strcpy(g_pub_tx.reply, "O270");
           goto ErrExit;
    }
    /*调用密码检查函数*/
    if (pub_base_DesChk(g_pub_tx.tx_date, " ",cPsw,sCom_tel.pwd)) 
    {
         strcpy(g_pub_tx.reply, "AT04");
         sprintf(acErrMsg, "密码不正确!!");
         WRITEMSG
         goto ErrExit;
    }
    
    Com_tel_Clo_Sel();

OkExit:
     strcpy(g_pub_tx.reply, "0000");
     sprintf(acErrMsg, "密码验证成功:[%s]", g_pub_tx.reply);
     WRITEMSG
          set_zd_data("0120", g_pub_tx.reply);
     return 0;
ErrExit:
     sprintf(acErrMsg, "密码验证失败：[%s]", g_pub_tx.reply);
     WRITEMSG
          set_zd_data("0120", g_pub_tx.reply);
     return 1;
}

