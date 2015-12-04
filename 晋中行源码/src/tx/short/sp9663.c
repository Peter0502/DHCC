/*************************************************
* 文  件名: sp9663.c
* 功能描述: 检测账号或卡号是否存在
* 作    者: liuyong
* 完成日期: 2009年10月11日
* 修改记录:   
*     1.日    期:
*       修 改 人:
*       修改内容:
*************************************************/

#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "stdlib.h" 
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#define  EXTERN
#include "public.h"
#include "pub_tx.h"
#include "mdm_ac_rel_c.h"

static struct mdm_ac_rel_c mdm_ac_rel;

sp9663()
{
        int ret = 0;

        /** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
        memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

        /** 取值、赋值 **/
        get_zd_data("0300",mdm_ac_rel.ac_no);	/* 账号或卡号 */	
        /** 处理流程 **/
        sprintf(acErrMsg,"账号：[%s]",mdm_ac_rel.ac_no);
        WRITEMSG

        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,
              "ac_no='%s'",mdm_ac_rel.ac_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"账号或卡号不存在![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"M002");
            ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"读取介质与账户关系表错误！[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"W011");
            ERR_DEAL;
        }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"查找账卡号成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"查找账卡号失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}


