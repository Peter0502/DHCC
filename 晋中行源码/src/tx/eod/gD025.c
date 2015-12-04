/*************************************************************
* 文 件 名: gD025.c
* 功能描述：日终翻盘处理
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

gD025()
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
TRACE

    /* 修改系统日期、状态、初始化流水、更改网点状态 */
    ret = Com_sys_parm_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

TRACE
    ret = Com_sys_parm_Fet_Upd(&com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

TRACE
/*	 modified by liuxuan 2006-11-03 改到帐务平衡检查gD030中
    com_sys_parm_c.sys_sts = 0;
*/
    com_sys_parm_c.lst_date=com_sys_parm_c.sys_date;
   
    ret = pub_base_end_month(com_sys_parm_c.sys_date);
    if (ret == 0)
    {
        if (pub_base_end_year(com_sys_parm_c.sys_date) == 0)
        {
            com_sys_parm_c.sys_date = \
                  (com_sys_parm_c.sys_date /10000 + 1) * 10000 + 101;
        }
        else
        {
            com_sys_parm_c.sys_date = \
                        (com_sys_parm_c.sys_date / 100 + 1 ) * 100 + 1;
        }
    }
    else
    {
        com_sys_parm_c.sys_date = com_sys_parm_c.sys_date + 1;
    }

TRACE
    ret = Com_sys_parm_Upd_Upd(com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Com_sys_parm_Clo_Upd();

TRACE
    /* 初始化流水 */
    ret= set_tlsrno(0);
    if (ret < 0)
    {
        sprintf(acErrMsg,"初始化流水错误!!");
        WRITEMSG
        goto ErrExit;
    }

TRACE
    /* 更改网点状态 */
    ret = sql_execute("update com_branch set wrk_sts='2' where wrk_sts!='*' and br_type not in('0') ");
    if (ret != 0)
    {
        sprintf(acErrMsg,"修改网点状态错误!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"翻盘成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	if (strcmp(g_pub_tx.reply,"0000") == 0)
		strcpy(g_pub_tx.reply,"G108");
	sprintf(acErrMsg,"翻盘失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
