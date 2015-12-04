/*************************************************
* 文 件 名:  spB432.c
* 功能描述： 完成登记委托收款清单录入功能。
*
* 作    者:  jane
* 完成日期： 2003年2月2日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int spB432()
{
	char	cLoadName[50];			/* 上传文件名 */
	char	cOperCode[6];			/* 业务代码 */
	int	ret;
	
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	strcpy( cOperCode, "10006" );
	
	/* 检查交易机构是否具有权限 */
	ret = pub_sttl_ChkInputBrno( cOperCode, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查交易机构权限错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 从文件导入临时表mo_trust_list中 */
	Pubscname( cLoadName );
	ret = tab_load( cLoadName, "mo_trust_list", "turst_pay_no,payer_seqn,payer_ac_no,\
			payer_name,payer_br_no,payer_br_name,amt,'1'" );
		
	/* 记流水日志 */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
