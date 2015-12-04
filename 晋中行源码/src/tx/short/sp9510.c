/*****************************************************
* 文 件 名:  sp9510.c
* 功能描述： TIPS同步核心状态
* 作    者:  xyy
* 完成日期： 2009-6-29 23:32:05
insert into tx_def values ('9510','TIPS同步核心状态','10000000000100010000000000000000000000000000001000000000000000000000000100000000000000000000000000000000000000000000000000000100','1','6','0','','');
insert into tx_flow_def values ('9510','0','9510','#$');
insert into tx_sub_def values ('9510','TIPS同步核心状态','sp9510','0','0');
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "com_sys_parm_c.h"
int sp9510()
{
    int iRc = 0;
    char cSys_sts[2];
    memset(cSys_sts , 0 , sizeof(cSys_sts));
    struct com_sys_parm_c sCom_sys_parm;
    
    memset(&sCom_sys_parm , 0 , sizeof(sCom_sys_parm));
    /* 初始化全局变量 */
    pub_base_sysinit();
    vtcp_log("[%s][%d]TIPS系统调用行内日期,需要判断行内状态\n",__FILE__,__LINE__);
    iRc = Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
    if (iRc)
    {
        vtcp_log("声明表com_sys_parm的游标出错！\n");
        sprintf(acErrMsg,"声明表com_sys_parm的游标出错![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply, "1888");
        goto ErrExit;
    }
    vtcp_log("[%s][%d] sys_date=[%ld] sys_sts=[%d]",__FILE__,__LINE__,sCom_sys_parm.sys_date,sCom_sys_parm.sys_sts);
    set_zd_long("0470",sCom_sys_parm.sys_date);
    apitoa(sCom_sys_parm.sys_sts,sizeof(cSys_sts)-1,cSys_sts);

    set_zd_data("1260",cSys_sts);
OkExit:
 strcpy( g_pub_tx.reply, "0000" );
 sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
 WRITEMSG
 set_zd_data(DC_REPLY,g_pub_tx.reply);
 return 0;

ErrExit:
 sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
 WRITEMSG
 set_zd_data(DC_REPLY,g_pub_tx.reply);
 return 1;
}
