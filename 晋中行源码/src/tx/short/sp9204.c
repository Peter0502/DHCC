/*************************************************
* 文 件 名:  sp9204.c
* 功能描述： 计算凭证数量
* 作    者:  YW
* 完成日期：
*
* 修改记录：
* 日   期:   20111206
* 修 改 人:
* 修改内容:
* 交易表数据：
------------------------------
insert into tx_def values('9204', '计算凭证数量','10000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000','0', '4', '0', '3', '');
insert into tx_flow_def values('9204', '0', '9204', '#$');
insert into tx_sub_def values('9204','计算凭证数量', 'sp9204','0','0');
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"


int sp9204()
{
	long lBeg_note_no=0.00;
	long lEnd_note_no=0.00;
	long lSum_cnt=0.00;

	vtcp_log("进入9204交易\n");
	get_zd_long("0580", &lBeg_note_no);
	get_zd_long("0590", &lEnd_note_no);

	lSum_cnt=lEnd_note_no-lBeg_note_no+1;
	vtcp_log("起始凭证号[%ld],终止凭证号[%ld],凭证总数:[%ld]\n",lBeg_note_no,lEnd_note_no,lSum_cnt);

	set_zd_long("0600", lSum_cnt);

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
