/***********************************************************
* �� �� ��: spF707.c
* ������������Ա��ؽ������һ����ˮ��ѯ
* ��    ��: jack
* ������ڣ�2003��2��22��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "four_tradelog_c.h"

spF707()
{
	int ret = 0;
	char cTel[5];

	struct four_tradelog_c sFour_Tradelog;
	
	memset(&sFour_Tradelog,0x0,sizeof(sFour_Tradelog));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	memset(cTel, 0x0, sizeof(cTel));
	get_zd_data("0360", cTel);
	get_zd_data("0920",sFour_Tradelog.tx_tel);
	sprintf(acErrMsg,"------------> ��Ա��Ϊ[%s]", sFour_Tradelog.tx_tel );
	WRITEMSG
	sprintf(acErrMsg,"------------> ��Ա��Ϊ[%s]", cTel );
	WRITEMSG
    	

	ret = sql_count("four_tradelog","tx_date = %ld and tx_tel = '%s'", \
		g_pub_tx.tx_date, sFour_Tradelog.tx_tel);
	if (ret == 0)
	{
		sprintf(acErrMsg,"��Ҫ�ҵĽ�����ˮ![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	if (ret >0)
	{
		ret = 0;
		ret = sql_max_long( "four_tradelog","trace_no",&sFour_Tradelog.trace_no, \
			"tx_date = %ld and tx_tel = '%s'",g_pub_tx.tx_date, sFour_Tradelog.tx_tel);
		if (ret != 0)
		{
			sprintf(acErrMsg,"ȡ���й�Ա��ؽ��������ˮ�ų���![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"F002");
			goto ErrExit;
		}
	}

	ret = Four_tradelog_Sel( g_pub_tx.reply, &sFour_Tradelog, "tx_date = %ld and trace_no = %ld", \
		g_pub_tx.tx_date, sFour_Tradelog.trace_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"��ѯҪ�ҵ���ˮ��¼ʧ��![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("0610",sFour_Tradelog.tx_code);
	set_zd_long("0530",sFour_Tradelog.tx_date);
	set_zd_long("0540",sFour_Tradelog.tx_time);
	set_zd_data("0300",sFour_Tradelog.ac_no);
	set_zd_double("0400",sFour_Tradelog.tx_amt);
	set_zd_double("0850",sFour_Tradelog.sx_rate);
	set_zd_long("0550",sFour_Tradelog.trace_no);
	set_zd_data("0210",sFour_Tradelog.cur_no);
	if (strcmp(sFour_Tradelog.tx_stat,"0") == 0)
	{
		set_zd_data("0650","����");
	}
	else
	{
		set_zd_data("0650","����");
	}


OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ѯ��ˮ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ѯ��ˮ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
