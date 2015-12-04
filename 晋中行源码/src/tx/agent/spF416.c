/*************************************************
* �� �� ��: spF416.c
* ��������: �ɷѳ����Ǽǲ�
*
* ��    ��: mike
* �������: 2004��07��01��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "mo_finance_info_c.h"
#include "mo_finance_quit_c.h"

spF416()  
{ 	
	int ret=0;
	long trace_no=0;
	char flag[2];
	char pay_no[25];
	char ac_no[25];
	char flag_tmp[2];

	struct mo_finance_info_c sMo_finance_info;
	struct mo_finance_quit_c sMo_finance_quit;

	memset(pay_no, 0x0, sizeof(pay_no));
	memset(flag, 0x0, sizeof(flag));
	memset(&sMo_finance_info, 0x0, sizeof(struct mo_finance_info_c));
	memset(&sMo_finance_quit, 0x0, sizeof(struct mo_finance_quit_c));
	memset(ac_no, 0x0, sizeof(ac_no));
	memset(flag_tmp, 0x0, sizeof(flag_tmp));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if ( pub_base_sysinit() )
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	memset(flag, 0x0, sizeof(flag));
	memset(pay_no, 0x0, sizeof(pay_no));

	get_zd_long("0440", &trace_no);
	get_zd_data("0710", flag);
	get_zd_data("0330", pay_no);
	get_zd_data("0300", ac_no);

	if (flag[0] == '1' || flag[0] == '2' || flag[0] == '4')
	{
		ret = Mo_finance_info_Sel(g_pub_tx.reply, &sMo_finance_info, 
			"tx_date=%ld and trace_no=%ld", g_pub_tx.tx_date, trace_no);
		if ( ret == 100)
		{
			sprintf(acErrMsg, "û�и�Ʊ�ݺ���ļ�¼ !! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "H060");
			goto ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg, "SELECT MO_FINANCE_INFO ERROR !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		if (flag[0] != '4')
		{
			pub_base_strpack(sMo_finance_info.pay_no);
			if (strcmp(sMo_finance_info.pay_no, pay_no) != 0)
			{
				sprintf(acErrMsg, "�ɷ���������!! [%s],[%s]", pay_no,
					sMo_finance_info.pay_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "H022");
				goto ErrExit;
			}
		}

		memset(flag_tmp, 0x0, sizeof(flag_tmp));
		get_zd_data("0670", flag_tmp);
		sprintf(acErrMsg,"--------[%s], [%s], [%s], [%s]", flag_tmp, ac_no,
			sMo_finance_info.payee_acno, sMo_finance_info.payer_acno);
		WRITEMSG
		if (flag_tmp[0] == '0')
		{
			pub_base_strpack(sMo_finance_info.payee_acno);
			if (strcmp(sMo_finance_info.payee_acno, ac_no) != 0)
			{
				sprintf(acErrMsg, "�����˺��������!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "F034");
				goto ErrExit;
			}
		}
		else 
		{
			pub_base_strpack(sMo_finance_info.payer_acno);
			if (strcmp(sMo_finance_info.payer_acno, ac_no) != 0)
			{
				sprintf(acErrMsg, "�����˺��������!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "F034");
				goto ErrExit;
			}
		}
	}
	else if (flag[0] == '3')
	{
		ret = Mo_finance_quit_Sel(g_pub_tx.reply, &sMo_finance_quit, 
			"trace_no=%ld and tx_date=%ld", trace_no, g_pub_tx.tx_date);
		if ( ret )
		{
			sprintf(acErrMsg, "SELECT MO_FINANCE ERROR !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		pub_base_strpack(sMo_finance_quit.pay_no);
		if (strcmp(sMo_finance_quit.pay_no, pay_no) != 0)
		{
			sprintf(acErrMsg, "�ɷ���������!! [%s],[%s]", pay_no,
				sMo_finance_quit.pay_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "H022");
			goto ErrExit;
		}
	}

	ret = sql_execute("update mo_finance_info set sts='*' where \
			trace_no=%ld and tx_date=%ld", trace_no, g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "update mo_finance_info error!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute("delete from mo_finance_quit where trace_no=%ld and \
			tx_date=%ld", trace_no, g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "delete mo_finance_quit error!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0300", "");

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�޸Ĳ����Ǽǲ��ɹ�OKExit [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�޸Ĳ����Ǽǲ�ʧ��ErrExit [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
