/*************************************************************
* 文 件 名: gD030.c
* 功能描述：对帐务的平衡检查
*
* 作    者: jack
* 完成日期: 2003年3月12日
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

gD030()
{
	int ret = 0;
    struct dd_mst_c dd_mst_c;
    struct dd_parm_c dd_parm_c;
    struct com_sys_parm_c com_sys_parm_c;

    memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));
    memset(&dd_parm_c,0x0,sizeof(struct dd_parm_c));
    memset(&com_sys_parm_c,0x0,sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();

	/***xxx 20031208***
    ret = pub_eod_checktrace_mst();
    if (ret != 0)
    {
        sprintf(acErrMsg,"业务流水和主文件检查平衡错误!!");
        WRITEMSG
        goto ErrExit;
    }
	**************/

	/********xxx 20031307
    ret = pub_eod_checkmst_trace();
    if (ret != 0)
    {
        sprintf(acErrMsg,"主文件和业务流水检查平衡错误!!");
        WRITEMSG
        goto ErrExit;
    }
	*********/

	/**------ xxx --------**/
	ret = pub_eod_check_dc_log();
    if (ret != 0)
    {
        sprintf(acErrMsg,"会计流水检查错误!!");
        WRITEMSG
        goto ErrExit;
    }
	ret = pub_eod_check_gl_mst();
	if( ret )
	{
        sprintf(acErrMsg,"总分平衡检查错误!");
        WRITEMSG
        goto ErrExit;
	}

    /* 修改系统状态为联机 * 不能在这里改等系统稳定后的
    ret = Com_sys_parm_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
    }

    ret = Com_sys_parm_Fet_Upd(&com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	com_sys_parm_c.sys_sts = 0;

    ret = Com_sys_parm_Upd_Upd(com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Com_sys_parm_Clo_Upd();
	*/

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"对帐务平衡的检查成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"对帐务平衡的检查失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
