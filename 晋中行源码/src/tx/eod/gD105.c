/*************************************************************
* 文 件 名: gD105.c
* 功能描述：同城登记簿中录入金额的统计
*
* 作    者: rob
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

gD105()
{
    int ret=0;
	double d_amt=0.00, c_amt=0.00;


	ret=sql_begin(); /*打开事务*/
	if(ret != 0) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	/* 同城参数错误 add by rob at 20030816 */
	ret = sql_sum_double("mo_city", "bill_amt", &d_amt, 
		"sts in ('1', '6') and cd_ind='2'");
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计金额错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	ret = sql_sum_double("mo_city", "bill_amt", &c_amt, 
		"sts in ('1', '6') and cd_ind='1'");
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计金额错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
		
	/******
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='GZJE' and \
		parm_seqn=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询COM_PARM异常!![%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	******/
	ret = sql_execute("update com_parm set val='%.2lf' where parm_code='GZJE' \
		and parm_seqn=1", d_amt - c_amt);
	if (ret != 0)
	{
		sprintf(acErrMsg, "更新COM_PARM异常 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute("update com_parm set val='%.2lf' where parm_code='GZJE' \
		and parm_seqn=2", d_amt - c_amt);
	if (ret != 0)
	{
		sprintf(acErrMsg, "更新COM_PARM异常 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	/* end at 20050816 **/


OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"同城登记簿中录入金额统计成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	if (strcmp(g_pub_tx.reply,"0000") == 0)
		strcpy(g_pub_tx.reply,"G108");
	sprintf(acErrMsg,"同城登记簿中录入金额统计失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
