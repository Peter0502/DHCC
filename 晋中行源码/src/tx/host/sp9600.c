/*************************************************
* 文 件 名: sp9600.c
* 功能描述：管理平台的柜员登录
*
* 作    者: rob
* 完成日期：20030314
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int sp9600( )
{
    int ret;
    struct com_tel_c com_tel_c,com_tel;

    memset(&com_tel_c,0x0,sizeof(struct com_tel_c));
    memset(&com_tel,0x0,sizeof(struct com_tel_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0920",com_tel_c.tel);
    get_zd_data("0790",com_tel_c.pwd);

    pub_base_strpack(com_tel_c.tel);
    pub_base_strpack(com_tel_c.pwd);

    /* 检查柜员号是否唯一 */
    ret = Com_tel_Sel(g_pub_tx.reply,&com_tel,"tel='%s'",com_tel_c.tel);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询柜员信息表错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"该柜员号不存在!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O120");
        goto ErrExit;
    }
	
    /* 检查该柜员是否签到??? */
    /* 检查密码 */
    ret = pub_base_DesChk(g_pub_tx.tx_date,"",com_tel_c.pwd,com_tel.pwd);
	/*vtcp_log("JJJ com_tel_c.pwd=[%d][%s]",strlen(com_tel_c.pwd),com_tel_c.pwd);*/
    if (ret != 0)
    {
        sprintf(acErrMsg,"密码错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O123");
        goto ErrExit;
    }
       
    strcpy(g_pub_tx.ac_no,com_tel_c.tel);
    strcpy(g_pub_tx.brf,"管理平台柜员登录");

	/****
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }
	****/

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"管理平台柜员登录成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"管理平台柜员登录失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
