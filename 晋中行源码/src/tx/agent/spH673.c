/*************************************************
* 文 件 名: spH673.c
* 功能描述：代缴(话费)撤销前处理
*
* 作    者: mike
* 完成日期：2004年06月13日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "ag_paytlcs_c.h"
#include "ag_itemlist_c.h"

int spH673()
{
	int ret = 0;
	int i = 0;
	char tmpstr[32];

	char cUnit_no[5];
	char cItem_no[21];
	char cTrace_no[16];
	double dTram = 0.00;

	struct ag_paytlcs_c sAg_paytlcs;

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	memset( cItem_no, 0x0, sizeof(cItem_no) );
	memset( cTrace_no, 0x0, sizeof(cTrace_no) );

	memset( &sAg_paytlcs, 0x00, sizeof(sAg_paytlcs) );

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0610", cTrace_no );
	get_zd_data( "0630", cItem_no );
	get_zd_double( "0400", &dTram );
	
	sprintf( acErrMsg,"---->撤销条件为unit_no = '%s' and bank_tlcs = '%s' and item_no = '%s' \
			and trn_amt = %f", cUnit_no, cTrace_no, cItem_no, dTram );
	WRITEMSG

	ret = Ag_paytlcs_Sel( g_pub_tx.reply, &sAg_paytlcs, \
			"unit_no = '%s' and bank_tlcs = '%s' and item_no = '%s' \
			and trn_amt = %f", cUnit_no, cTrace_no, cItem_no, dTram );
	if ( ret == 100 )
	{
		sprintf( acErrMsg,"要被撤销的记录不存在!!!" );
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"查找要被撤销的记录出错!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update ag_paytlcs set stat = '1' where \
			unit_no = '%s' and bank_tlcs = '%s' and item_no = '%s' \
			and trn_amt = %f", cUnit_no, cTrace_no, cItem_no, dTram );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "H020" );
		sprintf( acErrMsg,"修改被撤销的记录状态错!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "delete from ag_itemlist where \
			unit_no = '%s' and bank_tlcs = '%s' and item_no = '%s' \
			and am30 = %f", cUnit_no, cTrace_no, cItem_no, dTram );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "H020" );
		sprintf( acErrMsg, "删除缴费明细出错!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf( acErrMsg,"缴费撤销前处理成功!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"缴费撤销前处理失败[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
