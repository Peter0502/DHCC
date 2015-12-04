/*************************************************
* 文 件 名: sp9993.c
* 功能描述：查询柜员尾箱状态是否正常
*
* 作    者: jack
* 完成日期：20031114 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"

int sp9993()
{
	int ret=0;

	struct cash_mst_c sCash_mst;

	pub_base_sysinit();

	ret = Cash_mst_Sel( g_pub_tx.reply, &sCash_mst, \
			"br_no = %s and tel = %s and sts = '0'", \
			g_pub_tx.tx_br_no, g_pub_tx.tel );
	if( ret )
	{
		strcpy( g_pub_tx.reply, "D189" );
		goto ErrExit;
	}	

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf( acErrMsg, "---->查询柜员尾箱出错[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
