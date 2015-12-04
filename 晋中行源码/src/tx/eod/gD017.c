/*************************************************************
* 文 件 名: gD017.c
* 功能描述：卡限额管理文件维护
*
* 作    者: jack
* 完成日期: 2003年3月14日
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
#include "chnl_tradeadd_c.h"
#include "com_parm_c.h"

gD017()
{
	int ret = 0;
	int days = 0;
	long clr_date = 0;

	struct chnl_tradeadd_c sChnl_tradeadd;
	struct com_parm_c sCom_parm;

	memset(&sChnl_tradeadd, 0x00, sizeof(sChnl_tradeadd) );
	memset(&sCom_parm, 0x00, sizeof(sCom_parm) );

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	ret = Com_parm_Sel( g_pub_tx.reply , sCom_parm, "parm_code = 'KXEQ' and parm_seqn = 1");
	{
		sprintf( acErrMsg, "查询卡限额记录保留天数出错!!!" );
		WRITEMSG
		goto ErrExit;
	}

	days = atoi( sCom_parm.val );

	clr_date = pub_base_daynumSUB(g_pub_tx.tx_date, days );

	ret = sql_execute("delete from chnl_tradeadd where \
			tx_date <= %ld", clr_date);
	if (ret != 0)
	{
		sprintf( acErrMsg, "删除卡限额记录出错!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"卡限额管理文件维护通过!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"卡限额管理文件维护失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
