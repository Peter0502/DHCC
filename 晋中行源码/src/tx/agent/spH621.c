/***************************************************************
* 文 件 名: spH621.c
* 功能描述：税务欠费信息查询
*
* 作    者: jack
* 完成日期：2004年6月24日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "pay_agent_c.h"

spH621()
{
	int ret = 0;
	int num = 0;
	char cItem_no[21];
	char cUnit_no[5];

	struct pay_agent_c sPay_agent; /*缴费信息明细表*/

	memset(cItem_no,0x0,sizeof(cItem_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );

	/* 数据初始化 */
	pub_base_sysinit();

	ret = Pay_agent_Sel( g_pub_tx.reply, &sPay_agent, \
			"item_no = '%s' and unit_no = '%s' and result= '1'", \
			cItem_no, cUnit_no);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Pay_agent_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0260", sPay_agent.remark );	/* 纳税人名称 */
	set_zd_double( "0400", sPay_agent.tram );	/* 应缴金额 */
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"缴税欠费信息查询成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"缴税欠费信息查询失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
