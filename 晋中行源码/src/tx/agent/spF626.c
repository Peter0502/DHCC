/***************************************************************
* 文 件 名: spF626.c
* 功能描述：代理缴税业务撤销记录处理
*
* 作    者: jack
* 完成日期：2004年06月26日
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
#include "ag_paytlcs_c.h"
#include "ag_unitinfo_c.h"

spF626()
{
	int ret = 0;

	char cItem_no[21];
	char cUnit_no[5];
	char cTrace_no[13];
	double dTram = 0.00;

	struct ag_unitinfo_c sAg_unitinfo;	/*单位信息*/
	struct ag_paytlcs_c sAg_paytlcs;   /*缴费流水表*/

	memset(cItem_no,0x0,sizeof(cItem_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset( cTrace_no, 0x0, sizeof(cTrace_no) );

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );
	get_zd_data( "0610", cTrace_no );
	get_zd_double( "0400", &dTram );

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sAg_unitinfo, \
			"unit_no = '%s' and unit_stat = '0'", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "查询代理单位信息出错[%s]!!!", cUnit_no );
		WRITEMSG
		goto ErrExit;
	}

	memset( &sAg_paytlcs, 0x0, sizeof(sAg_paytlcs) );
	ret = Ag_paytlcs_Sel( g_pub_tx.reply, &sAg_paytlcs, \
			"unit_no = '%s' and item_no = '%s' and bank_tlcs = '%s' \
			and trn_amt = %f and stat = '0'", cUnit_no, cItem_no, \
			cTrace_no, dTram );
	if ( ret )
	{
		sprintf( acErrMsg, "--------->要撤销的记录出错!!![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update pay_agent set sata = '1', result = '1' \
			where unit_no = '%s' and item_no = '%s' and \
			result = '0'", cUnit_no, cItem_no );
	if ( ret )
	{
		sprintf(acErrMsg,"----->修改未缴记录状态失败![%s]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update ag_paytlcs set stat = '1' where \
			unit_no = '%s' and item_no = '%s' and bank_tlcs = '%s' \
			and trn_amt = %f and stat = '0'", cUnit_no, cItem_no, \
			cTrace_no, dTram );
	if ( ret )
	{
		sprintf(acErrMsg,"---->修改代缴银行业务流水状态失败![%s]", ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf( acErrMsg, "------>撤销缴费记录处理成功!!!!" );
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"------>撤销缴费记录处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
