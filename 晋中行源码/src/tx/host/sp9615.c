/*************************************************
* 文 件 名: sp9615.c
* 功能描述：商户信息修改
*
* 作    者: rob
* 完成日期：20030921
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "march_info_c.h"

int sp9615( )
{
    int ret;
	struct march_info_c sMarch_info, sMarch_info1;

	memset(&sMarch_info, 0x0, sizeof(struct march_info_c));
	memset(&sMarch_info1, 0x0, sizeof(struct march_info_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

	get_zd_data("0580", sMarch_info.mrchno);
	get_zd_data("0250", sMarch_info.mrchnm);
	get_zd_data("0910", sMarch_info.br_no);
	get_zd_data("0210", sMarch_info.mrchtype);
	get_zd_data("0670", sMarch_info.mrchstat);
	get_zd_data("0300", sMarch_info.mrchacno);
	get_zd_data("0260", sMarch_info.mrchaddr);
	get_zd_data("0590", sMarch_info.mrchtel);
	get_zd_data("0730", sMarch_info.mrchman);
	get_zd_double("0840", &sMarch_info.chrg);
	get_zd_long("0440", &sMarch_info.reg_date);
	get_zd_long("0450", &sMarch_info.beg_date);
	get_zd_long("0460", &sMarch_info.end_date);

	ret = March_info_Dec_Upd(g_pub_tx.reply, "mrchno='%s'", sMarch_info.mrchno);
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE FOR UPDATE ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = March_info_Fet_Upd(&sMarch_info1, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "FETCH FOR UPDATE ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = March_info_Upd_Upd(sMarch_info, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "UPDATE ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	March_info_Clo_Upd();

	/****
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }
	****/

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"修改商户信息成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"修改商户信息失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
