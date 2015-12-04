/*************************************************************
* 文 件 名: gD001.c
* 功能描述：批前检查部分
*
* 作    者: jack
* 完成日期: 2003年3月5日
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
#include "mbfe_sys_ctl_c.h"

gD001()
{
	int ret = 0;
	int br_type;

	struct com_sys_parm_c sCom_sys_parm;
	struct mbfe_sys_ctl_c sMbfe_sys_ctl;

	memset(&sCom_sys_parm,0x0,sizeof(sCom_sys_parm));
	memset(&sMbfe_sys_ctl,0x0,sizeof(sMbfe_sys_ctl));

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	/* 检查交易机构是否是科技部0 */
	br_type = pub_base_ChkBrno( g_pub_tx.tx_br_no );
	if ( br_type < 0 )
	{
		sprintf( acErrMsg, "检查日终执行机构错" );
		WRITEMSG
		goto ErrExit;
	}
	if ( br_type!='0' )
	{
		sprintf( acErrMsg, "此机构非科技部无权执行日终处理!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S043" );
		goto ErrExit;
	}
	/*-------------------------------------------------*/

	/* 根据公共系统参数表，判断状态是否是1 */
	ret = pub_base_GetSysparm( &sCom_sys_parm );
	if ( ret )
	{
		sprintf( acErrMsg, "取公共系统参数表错!" );
		WRITEMSG
		goto ErrExit;			
	}
	/* 判断是否做了备份 */
	if ( sCom_sys_parm.sys_sts != 1 )
	{
		sprintf( acErrMsg, "请首先做数据备份!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "G002" );
		goto ErrExit;
	}
	
	/* 取出所有的机构数据,判断是否都已经签退,科技部不签退 */
	ret = sql_count( "com_branch", "wrk_sts = '1' and br_type != '0'" );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "执行sql_count 错误,[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret > 0 )
	{
		sprintf( acErrMsg, "还有机构没有签退" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "G003" );
		goto ErrExit;
	}
	/*-----------------------------------------*/

	/* 判断大额支付系统是否已经日切 JYW*/
/*****JYW
	ret = Mbfe_sys_ctl_Sel( g_pub_tx.reply, &sMbfe_sys_ctl, "par_code='05'" );
	if( ret )
	{
		sprintf( acErrMsg, "检查大额支付系统是否日切出错!");
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(sMbfe_sys_ctl.par_value);
	if( strcmp( sMbfe_sys_ctl.par_value, "00" ) )
	{
		sprintf( acErrMsg, "检查大额支付系统尚未日切!");
		strcpy( g_pub_tx.reply, "G012" );
		WRITEMSG
		goto ErrExit;
	}
****JYW*****/
	/*-----------------------------------------*/

	/*-------- 修改系统状态为日终状态 ---------*/
	ret = sql_execute("update com_sys_parm set sys_sts = 2 ");
	if ( ret )
	{
		sprintf( acErrMsg, "修改系统状态为日终时出错!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "G005" );
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终前检查通过!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终前检查失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
