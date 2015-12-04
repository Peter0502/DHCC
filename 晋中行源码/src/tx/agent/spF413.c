/*************************************************
* �� �� ��: spF413.c
* �����������Ǽǲ����˸���Ǽǲ�
*
* ��    ��: mike
* �������: 2004��03��29��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "mo_finance_quit_c.h"

spF413()  
{ 	
	int ret;
	struct mo_finance_quit_c s_mo_finance_quit;

	memset(&s_mo_finance_quit, 0x0, sizeof(struct mo_finance_quit_c));

	pub_base_sysinit();

	s_mo_finance_quit.trace_no = g_pub_tx.trace_no;
	s_mo_finance_quit.trace_cnt = g_pub_tx.trace_cnt;
	get_zd_long("0780",&s_mo_finance_quit.mid_trace_no);
    strcpy(s_mo_finance_quit.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(s_mo_finance_quit.tx_code, g_pub_tx.tx_code);
	s_mo_finance_quit.tx_date = g_pub_tx.tx_date;
	s_mo_finance_quit.tx_time = g_pub_tx.tx_time;
	get_zd_data("0330",s_mo_finance_quit.untread_no);
	get_zd_data("0880",s_mo_finance_quit.untread_stat);
	get_zd_data("0730",s_mo_finance_quit.pay_no);
	get_zd_long("0440",&s_mo_finance_quit.untread_date);
	get_zd_data("0250",s_mo_finance_quit.payee_name);
	get_zd_data("0300",s_mo_finance_quit.payee_acno);
	get_zd_data("0270",s_mo_finance_quit.payee_brno);
	get_zd_data("0260",s_mo_finance_quit.payer_name);
	get_zd_data("0310",s_mo_finance_quit.payer_acno);
	get_zd_data("0810",s_mo_finance_quit.payer_brno);
	get_zd_data("0860",s_mo_finance_quit.item_code);
	get_zd_data("0600",s_mo_finance_quit.unit_code);
	get_zd_double("0420",&s_mo_finance_quit.amt);
	get_zd_data("0760",s_mo_finance_quit.untread_cause);
	strcpy(s_mo_finance_quit.brf,"�˸����˸�");
	strcpy(s_mo_finance_quit.tel,g_pub_tx.tel);
	strcpy(s_mo_finance_quit.chk,g_pub_tx.chk);
	strcpy(s_mo_finance_quit.auth,g_pub_tx.auth);

	ret = Mo_finance_quit_Ins(s_mo_finance_quit, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"�Ǽǲ����˸���Ǽǲ�!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�Ǽǲ����˸���Ǽǲ�����ɹ�OKExit [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�Ǽǲ����˸���Ǽǲ�����ʧ��ErrExit [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
