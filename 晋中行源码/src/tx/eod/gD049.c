/*************************************************************
* 文 件 名: gD049.c
* 功能描述：登记大额提现登记薄
*
* 作    者: mike
* 完成日期: 2003年3月16日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"


gD049()
{
    int ret = 0;
    struct com_sys_parm_c com_sys_parm_c;

    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
   
    /* 查询系统参数表 */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询系统参数表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    g_pub_tx.tx_date = com_sys_parm_c.lst_date; /* 翻盘后处理 */

    ret = pub_eod_ins_mofetcash();
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记大额提现登记薄错误~!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"登记大额提现登记薄成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"登记大额提现登记薄失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
