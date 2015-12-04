/*************************************************************
* 文 件 名: gD025A.c
* 功能描述：为汇总账准备流水dc_log->dc_log_bk
*
* 作    者: jack
* 完成日期: 2003年3月8日
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

gD025A()
{
    int ret=0;
	double d_amt=0.00, c_amt=0.00;
    struct com_sys_parm_c com_sys_parm_c;

    sprintf(acErrMsg,"翻盘开始!!!!");
    WRITEMSG

	ret=sql_begin(); /*打开事务*/
	if(ret != 0) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

    /* 删除临时表 增加了处理几个周期表的语句 gl_mst_hst*/
    ret = pub_pubdb_delcretable();
    if (ret != 0)
    {
        sprintf(acErrMsg,"删除建立备份表错误!!");
        WRITEMSG
        goto ErrExit;
    }
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"翻盘准备汇总账流水成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	if (strcmp(g_pub_tx.reply,"0000") == 0)
		strcpy(g_pub_tx.reply,"G108");
	sprintf(acErrMsg,"翻盘准备汇总账流水失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
