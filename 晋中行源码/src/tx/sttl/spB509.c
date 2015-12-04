/*************************************************
* 文 件 名:  spB509.c
* 功能描述： 拆入还款信息维护
*
* 作    者:  rob
* 完成日期： 2003年7月06日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_ibl_ret_c.h"

int spB509()
{
    int ret;
	struct mo_ibl_ret_c sMo_ibl_ret;

	memset(&sMo_ibl_ret, 0x0, sizeof(struct mo_ibl_ret_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
	
	get_zd_data("0730", sMo_ibl_ret.pact_no);
	strcpy(sMo_ibl_ret.tx_br_no, g_pub_tx.tx_br_no);
	get_zd_double("0420", &sMo_ibl_ret.ret_amt);
	get_zd_double("0430", &sMo_ibl_ret.ret_rate);
	sMo_ibl_ret.tx_date = g_pub_tx.tx_date;
	sMo_ibl_ret.trace_no = g_pub_tx.trace_no;
	strcpy(sMo_ibl_ret.sts, "1");
	strcpy(sMo_ibl_ret.tel, g_pub_tx.tel);
	strcpy(sMo_ibl_ret.chk, g_pub_tx.chk);
	strcpy(sMo_ibl_ret.auth, g_pub_tx.auth);

	ret = Mo_ibl_ret_Ins(sMo_ibl_ret, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "insert into mo_ibl_ret error !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	strcpy(g_pub_tx.brf, "拆入还款信息录入");

	if (pub_ins_trace_log()) 
	{ 
		sprintf(acErrMsg,"登记交易流水错误!!"); 
		WRITEMSG 
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

