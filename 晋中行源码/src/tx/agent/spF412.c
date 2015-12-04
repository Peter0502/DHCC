/*************************************************
* 文 件 名: spF412.c
* 功能描述：登记财政代缴登记簿
*
* 作    者: mike
* 完成日期: 2004年03月29日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "mo_finance_info_c.h"

spF412()  
{ 	
	int ret;
	char flag[2];
	char ac_no[20];

	struct mo_finance_info_c s_mo_finance_info;
	struct mdm_ac_rel_c sMdm_ac_rel;

	memset(&s_mo_finance_info, 0x0, sizeof(struct mo_finance_info_c));
	memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));

	pub_base_sysinit();

	get_zd_data("0330", ac_no);
	sprintf(acErrMsg, "------------->0330=[%s]", ac_no);
	WRITEMSG

	s_mo_finance_info.trace_no = g_pub_tx.trace_no;
	s_mo_finance_info.trace_cnt = g_pub_tx.trace_cnt;
	get_zd_long("0780",&s_mo_finance_info.mid_trace_no);
	strcpy(s_mo_finance_info.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(s_mo_finance_info.tx_code,g_pub_tx.tx_code);
	s_mo_finance_info.tx_date = g_pub_tx.tx_date;
	s_mo_finance_info.tx_time = g_pub_tx.tx_time;
	get_zd_data("0730",s_mo_finance_info.pay_no);
	get_zd_data("0240", s_mo_finance_info.agent_bank);
	get_zd_data("0230",s_mo_finance_info.hall_code);
	get_zd_data("0370",s_mo_finance_info.unit_code);
	get_zd_long("0440",&s_mo_finance_info.ticket_date);

	get_zd_data("0710", s_mo_finance_info.ct_ind);

	get_zd_data("0690",flag);
	if (flag[0] == '1')
		strcpy(s_mo_finance_info.pay_sign,"100");
	else if (flag[0] == '2')
		strcpy(s_mo_finance_info.pay_sign,"200");
	else 
		strcpy(s_mo_finance_info.pay_sign,"300");

	get_zd_data("0250",s_mo_finance_info.payer_name);
	get_zd_data("0300",s_mo_finance_info.payer_acno);
	get_zd_data("0270",s_mo_finance_info.payer_brno);
	get_zd_data("0680",s_mo_finance_info.payer_sort);

	if (s_mo_finance_info.payer_sort[0] == '0')
	{
		get_zd_data("0310", ac_no);
	}
	else
	{
		get_zd_data("0300", ac_no);
	}

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", ac_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询介质账号关系表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	strcpy(s_mo_finance_info.agent_br_no, sMdm_ac_rel.opn_br_no);

	get_zd_data("0260",s_mo_finance_info.payee_name);
	get_zd_data("0310",s_mo_finance_info.payee_acno);
	get_zd_data("0810",s_mo_finance_info.payee_brno);
	get_zd_double("0420",&s_mo_finance_info.amt);
	get_zd_data("0640",s_mo_finance_info.validate_code);

    if (strcmp(g_pub_tx.tx_code,"6401") == 0)
	{
		strcpy(s_mo_finance_info.sts,"1");
		strcpy(s_mo_finance_info.brf,"机制票据缴费");
	}
    else if (strcmp(g_pub_tx.tx_code,"6402") == 0)
	{
		strcpy(s_mo_finance_info.sts,"2");
		strcpy(s_mo_finance_info.brf,"手工票据缴费");
	}
	else if (strcmp(g_pub_tx.tx_code,"6404") == 0)
	{
		strcpy(s_mo_finance_info.sts,"3");
		strcpy(s_mo_finance_info.brf,"待查票据缴费");
	}
	else if (strcmp(g_pub_tx.tx_code,"6405") == 0)
	{
		strcpy(s_mo_finance_info.sts,"4");
		strcpy(s_mo_finance_info.brf,"补录缴款书");
	}

	strcpy(s_mo_finance_info.tel,g_pub_tx.tel);
	strcpy(s_mo_finance_info.chk,g_pub_tx.chk);
	strcpy(s_mo_finance_info.auth,g_pub_tx.auth);

	ret = Mo_finance_info_Ins(s_mo_finance_info, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"登记财政缴费登记簿错误!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"登记财政登记簿处理成功OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"登记财政登记簿处理失败!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
