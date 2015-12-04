/*******************************************
版本号：    V1.0
日期：      20111024
程序作者：   shangyongchao
程序名称：  spF250.c
程序功能：  从平台接收账号、查询密码、支付密码和校验类型，对客户的密码和支取方式进行检查
            成功返回 0000 失败返回相应错误码
交易配置：
  insert into tx_def values('6250','客户密码检查','10000000000000000000000010000000000000000000000000000000000000000011100000000000000000000001000000000000000000000000000000000100','0','6','0','3',null);
  insert into tx_flow_def  values ('6250', 0, 'F250', '#$');
  insert into tx_sub_def  values ('F250', '客户密码检查', 'spF250', '0', '0');

程序说明：本程序是按照平台传来的密码为密文且为8位长处理的，
          如果为不加密的6位长的密码，请将解密的部分删除并将相应变量的前2位置其他字符，本程序只截取相应变量后6位

******************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"

int spF250()
{
    int ret = 0;
    long lKeylen = 0;/*密钥长度*/
    long lQrypwd = 0;/*查询密码长度*/
    long lDrawpwd = 0;/*支取密码长度*/
    char cVal[17];/*存放十六进制的密钥*/
    char cTmp[17];
    char cQry_pwd[9]; /*查询密码*/
    char cDraw_pwd[9];/*支取密码*/
    char cCmp_type[2]; /*校验类型1校验支取密码 2 校验查询密码 3全部校验*/
    char cAc_no[25];   /*账号*/
    struct com_parm_c sCom_parm;
    
    memset(cQry_pwd,0x00,sizeof(cQry_pwd));
    memset(cDraw_pwd,0x00,sizeof(cDraw_pwd));
    memset(cCmp_type,0x00,sizeof(cCmp_type));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
    /*初始化数据*/
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    /*从平台取值*/
    get_zd_data("0300",cAc_no);
    vtcp_log("[%s][%d],账号=【%s】",__FILE__,__LINE__,cAc_no);
    get_zd_data("0700",cCmp_type);
    vtcp_log("[%s][%d],校验类型=【%s】",__FILE__,__LINE__,cCmp_type);
    get_fd_data("0800",cTmp);
    vtcp_log("[%s][%d],80域值=【%s】",__FILE__,__LINE__,cTmp);
    
    memcpy(cDraw_pwd,cTmp,8);
    /*vtcp_log("[%s][%d],支取密码=【%s】",__FILE__,__LINE__,cDraw_pwd);*/
    memcpy(cQry_pwd,cTmp+8,8);
    /*vtcp_log("[%s][%d],查询密码=【%s】",__FILE__,__LINE__,cQry_pwd);*/
    
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no);
    if(ret)
    {
        sprintf(acErrMsg,"查询mdm_ac_rel失败！！");
        strcpy(g_pub_tx.reply,"W011");
        WRITEMSG
        goto ErrExit;
    }
    ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='PTMY' and parm_seqn=1");
    if( ret )
    {
         sprintf(acErrMsg,"查询密钥失败!!!");
         WRITEMSG
         strcpy(g_pub_tx.reply, "G001");
         goto ErrExit;
    }
    lKeylen = strlen(sCom_parm.val);
    vtcp_log("[%s][%d] lKeylen=[%ld]",__FILE__,__LINE__,lKeylen);
    lDrawpwd = strlen(cDraw_pwd);
   /* vtcp_log("[%s][%d] lDrawpwd=[%ld]",__FILE__,__LINE__,lDrawpwd);*/
    lQrypwd = strlen(cQry_pwd);
    /*vtcp_log("[%s][%d],lQrypwd=[%ld]",__FILE__,__LINE__,lQrypwd);*/
    vtcp_log("[%s][%d] val=[%s]",__FILE__,__LINE__,sCom_parm.val);
    /*转十六进制*/
    pubf_str_hex2str(sCom_parm.val,cVal,lKeylen/2);
    
    if(cCmp_type[0] == '1')
    {
       /* vtcp_log("[%s][%d],支取密码校验=[%s]",__FILE__,__LINE__,cCmp_type);*/
        /*解3des密码 支取密码*/
        ret = pubf_3des_dec(cVal,lKeylen/2,cDraw_pwd,cDraw_pwd,8); 
        if( ret )
        {
             sprintf(acErrMsg,"解密失败!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
        }
       /* vtcp_log("[%s][%d],解密完cDraw_pwd=[%s]",__FILE__,__LINE__,cDraw_pwd);*/
        memcpy(g_pub_tx.draw_pwd,cDraw_pwd+2,sizeof(g_pub_tx.draw_pwd)-1);
        
        /*vtcp_log("[%s][%d],g_pub_tx.draw_pwd=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd); */
        ret = pub_base_check_draw("001");
        if(ret)
        {
            sprintf(acErrMsg,"支取密码检验错误!");
            WRITEMSG
            goto ErrExit;
        }
    }
    else if(cCmp_type[0] == '2')
    {
        vtcp_log("[%s][%d],插叙密码校验=[%s]",__FILE__,__LINE__,cCmp_type);
        /*解3des密码 查询密码*/
        ret = pubf_3des_dec(cVal,lKeylen/2,cQry_pwd,cQry_pwd,8); 
        if( ret )
        {
             sprintf(acErrMsg,"解密失败!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
            
        }
        /*vtcp_log("[%s][%d],解密完cQry_pwd=[%s]",__FILE__,__LINE__,cQry_pwd);*/
        memcpy(g_pub_tx.qry_pwd,cQry_pwd+2,sizeof(g_pub_tx.qry_pwd)-1);
        /*vtcp_log("[%s][%d],g_pub_tx.qry_pwd=[%s]",__FILE__,__LINE__,g_pub_tx.qry_pwd);*/
        ret = pub_base_check_draw("011");
        if(ret)
        {
            sprintf(acErrMsg,"查询密码检验错误!");
            WRITEMSG
            goto ErrExit;
        }
    }
    else if(cCmp_type[0] == '3')
    {
        vtcp_log("[%s][%d],全部校验=[%s]",__FILE__,__LINE__,cCmp_type);
        ret = pubf_3des_dec(cVal,lKeylen/2,cDraw_pwd,cDraw_pwd,8); 
        if( ret )
        {
             sprintf(acErrMsg,"解密失败!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
        }
        /*vtcp_log("[%s][%d],解密完cDraw_pwd=[%s]",__FILE__,__LINE__,cDraw_pwd);*/
        memcpy(g_pub_tx.draw_pwd,cDraw_pwd+2,sizeof(g_pub_tx.draw_pwd)-1);
        
        /*vtcp_log("[%s][%d],g_pub_tx.draw_pwd=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
        ret = pub_base_check_draw("001");
        if(ret)
        {
            sprintf(acErrMsg,"支取密码检验错误!");
            WRITEMSG
            goto ErrExit;
        }
        /*解3des密码 查询密码*/
        ret = pubf_3des_dec(cVal,lKeylen/2,cQry_pwd,cQry_pwd,8); 
        if( ret )
        {
             sprintf(acErrMsg,"解密失败!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
            
        }
        /*vtcp_log("[%s][%d],解密完cQry_pwd=[%s]",__FILE__,__LINE__,cQry_pwd);*/
        memcpy(g_pub_tx.qry_pwd,cQry_pwd+2,sizeof(g_pub_tx.qry_pwd)-1);
        /*vtcp_log("[%s][%d],g_pub_tx.qry_pwd=[%s]",__FILE__,__LINE__,g_pub_tx.qry_pwd);*/
        ret = pub_base_check_draw("011");
        if(ret)
        {
            sprintf(acErrMsg,"查询密码检验错误!");
            WRITEMSG
            goto ErrExit;
        }
    }
    else
    {
        sprintf(acErrMsg,"校验标志错误!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "O006");
        goto ErrExit;
    }
    
  OkExit:
      strcpy( g_pub_tx.reply, "0000" );
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