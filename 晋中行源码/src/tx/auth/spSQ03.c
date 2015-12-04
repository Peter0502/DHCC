/*************************************************
* 文 件 名：spSQ03.c
* 功能描述: 记录远程授权信息 
* 作    者: liuyong
* 完成日期: 2010年2月24日
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
#include "auth_remote_inf_c.h"

static struct auth_remote_inf_c auth_remote_inf;

spSQ03()
{
        int ret = 0;
        long trace_no = 0;
        char filename[200];
        char attachs[601];

        /** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&auth_remote_inf,0x00,sizeof(struct auth_remote_inf_c));
        memset(filename,0x00,sizeof(filename));
        memset(attachs,0x00,sizeof(attachs));
        
        sprintf(acErrMsg,"开始取值:");
        WRITEMSG

        /** 取值、赋值 **/
        get_zd_long("0530",&auth_remote_inf.trace_no);
        get_zd_data("0280",auth_remote_inf.br_no);
        get_zd_data("0340",auth_remote_inf.tel);
        get_zd_long("0440",&auth_remote_inf.tx_date);
        get_zd_data("0350",auth_remote_inf.tx_code);
        get_zd_data("0360",auth_remote_inf.auth_tel);
        get_zd_data("0760",filename);
        get_zd_data("0950",attachs);
        get_zd_data("0250",auth_remote_inf.title);
 
        sprintf(acErrMsg,"取值完成:[%ld]--[%s]--[%s]--[%ld]--[%s]--[%s]--[%s]",auth_remote_inf.trace_no,auth_remote_inf.br_no,auth_remote_inf.tel,auth_remote_inf.tx_date,auth_remote_inf.tx_code,auth_remote_inf.auth_tel,auth_remote_inf.title);
        WRITEMSG

        pub_base_strpack(filename);
        strcpy(auth_remote_inf.filename,filename);
        pub_base_strpack(attachs);
        strcpy(auth_remote_inf.attachments,attachs);

        sprintf(acErrMsg,"交易文件[%s],附件[%s]",filename,attachs);
        WRITEMSG
        if(auth_remote_inf.trace_no<0)
        {
            sprintf(acErrMsg,"交易流水号错误");
            WRITEMSG
            goto ErrExit;
        }
 
	/** 处理流程 **/
        ret = Auth_remote_inf_Ins(auth_remote_inf,g_pub_tx.reply);
        if(ret)
        {
            sprintf(acErrMsg,"登记远程信息失败");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D107");
            goto ErrExit;
        }
        
        sprintf(acErrMsg,"处理完毕");
        WRITEMSG 	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"登记远程授权信息成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"登记远程授权信息失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}



