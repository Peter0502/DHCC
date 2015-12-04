/*********************************************************************
* 版 本 号 :  V1.0        
* 程序作者 :  XUEKE          
* 日    期 :  2011-07-14   
* 所属模块 :  与平台的接口程序            
* 程序名称 :  spF230.c
* 程序作用 :  测试核心系统是否能够正常接收平台交易
* 函数列表 :                                   
* 使用注意 :                                       
* 修改记录 :                                       
*     修改作者 :                           
*     修改时间 :                                    
*     修改内容 :
insert into tx_def  values ('6230', '连通性测试交易', '10000000000000010000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000100100', '1', '6', '0', '3', null);
insert into tx_flow_def  values ('6230', 0, 'F230', '#$');
insert into tx_sub_def  values ('F230','连通性测试交易','spF230','0','0');
******************************************************************/

#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"

spF230()
{
    int iRet=0;
    char cHx_tx_code[5];    /*交易代码*/
    char cHx_brf[50];       /*摘要*/
    
    /** 初始化各变量 **/
    memset(cHx_tx_code,0,sizeof(cHx_tx_code));
    memset(cHx_brf,0,sizeof(cHx_brf));
    
    /*初始化结构*/
    pub_base_sysinit();
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    
    /*** 取值 **/
    get_zd_data("0160",cHx_tx_code);
    get_zd_data("0810",cHx_brf);
    
    /*判断平台发送报文的是否成功*/
    if(strcmp(cHx_tx_code,"6230") != 0 )
    {
        sprintf(acErrMsg, "报文发送失败!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "P064");
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
