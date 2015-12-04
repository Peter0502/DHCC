/*************************************************
* 文 件 名:  spB508.c
* 功能描述： 拆借资金提出交易(贷408外)
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
#include "mo_city_c.h"
#include "com_parm_c.h"

int spB508()
{
    int ret;
	char flag[2];

	struct mo_city_c sMo_city;
	struct com_parm_c sCom_parm;

	memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

	sMo_city.tx_date = g_pub_tx.tx_date;
	sMo_city.tx_no = g_pub_tx.trace_no;
	strcpy(sMo_city.tx_br_no, g_pub_tx.tx_br_no);
	strcpy(sMo_city.tel, g_pub_tx.tel);
	strcpy(sMo_city.trans_mode, "1");
	strcpy(sMo_city.in_out_ind, "1");
	strcpy(sMo_city.cd_ind, "2"); /* 提出贷 */

	get_zd_data("0310", sMo_city.payer_ac_no); /* 拆借账号 */
	get_zd_data("0250", sMo_city.payer_name);
	strcpy(sMo_city.payer_br_no, g_pub_tx.tx_br_no);

	get_zd_data("0300", sMo_city.payee_ac_no);
	get_zd_data("0250", sMo_city.payee_name);
	get_zd_long("0440", &sMo_city.matr_date);
	get_zd_double("0400", &sMo_city.bill_amt);
	strcpy(sMo_city.sts, "3");
	strcpy(sMo_city.brf, "同业拆借拆出资金");

	ret = Mo_city_Ins(sMo_city, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "登记同城登记簿错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	/* 贷408外 */
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='TCKM' and \
			parm_seqn = 2");
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询公共参数表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	pub_base_strpack(sCom_parm.val);
	set_zd_data("1211", sCom_parm.val);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg," success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

