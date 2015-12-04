/*************************************************
* 文 件 名:  sp9237.c
* 功能描述： 最小凭证查询
* 作    者:
* 完成日期：
* insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('9237', '查询最小介质号', '10000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '2');
* insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('9237', 0, '9237', '#$');
* insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('9237', '查询最小介质号', 'sp9237', '0', '0'); 
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "note_parm_c.h"
#include "note_mst_c.h"

int sp9237()
{
	int ret = 0;
	/* 初始化g_pub_tx结构 */
	pub_base_sysinit();
	get_zd_data( "0230", g_pub_tx.note_type );
	get_zd_data( "0590", g_pub_tx.new_note_no );

	ret = pub_com_GetNoteMin(g_pub_tx.note_type, g_pub_tx.new_note_no, g_pub_tx.tel);
	if (ret != 0)
	{
		sprintf( acErrMsg, "执行pub_com_GetNoteMin错误!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "note_no=[%s]", g_pub_tx.new_note_no);
	WRITEMSG
	zip_space(g_pub_tx.new_note_no);
	if (0 == strlen(g_pub_tx.new_note_no))
	{
		strcpy( g_pub_tx.reply, "N075" );
		goto ErrExit;
	}
	set_zd_data( "0580", g_pub_tx.new_note_no);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}