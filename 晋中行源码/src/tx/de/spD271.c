/***************************************************************
* �� �� ��: spD271.c
* ��������: ��ѯ��¼��
*
* ��    ��: jack
* �������: 2004��4��14��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_city_query_c.h"

spD271()
{
    int ret;

    struct mo_city_query_c sMo_city_query;

    memset(&sMo_city_query, 0x0, sizeof(struct mo_city_query_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0910",sMo_city_query.rec_br_no);

	ret = sql_count("com_branch", "br_no='%s' and wrk_sts!= '*'", 
		sMo_city_query.rec_br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "��ѯ��������������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 0)
	{
		sprintf(acErrMsg, "û�иý��ܻ���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D193");
		goto ErrExit;
	}

	ret = sql_count("com_tx_br_rel", "br_no='%s' and up_br_no='%s' and \
		rel_type=3", g_pub_tx.tx_br_no, sMo_city_query.rec_br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "��ѯ���׻�����ϵ�����!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 0)
	{
		ret = sql_count("com_tx_br_rel", "br_no='%s' and up_br_no='%s' and \
			rel_type=3", sMo_city_query.rec_br_no, g_pub_tx.tx_br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "��ѯ���׻����ù�ϵ�����!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 0)
		{
			sprintf(acErrMsg, "���׻����ͽ��ܻ��������໥��ѯ!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D192");
			goto ErrExit;
		}
	}

    /* �Ǽǲ�ѯ�鸴�Ǽǲ� */
    sMo_city_query.tx_date = g_pub_tx.tx_date;
    sMo_city_query.tx_no = g_pub_tx.trace_no;
    strcpy(sMo_city_query.tx_br_no, g_pub_tx.tx_br_no);
    strcpy(sMo_city_query.tel, g_pub_tx.tel);

	get_zd_data("0670", sMo_city_query.type);
	get_zd_long("0440", &sMo_city_query.note_date);
	get_zd_data("0680", sMo_city_query.note_type);
	get_zd_double("0390", &sMo_city_query.tx_amt);
	get_zd_data("0300", sMo_city_query.payer_ac_no);
	get_zd_data("0250", sMo_city_query.payer_name);
	get_zd_data("0270", sMo_city_query.payer_br_name);
	get_zd_data("0310", sMo_city_query.payee_ac_no);
	get_zd_data("0260", sMo_city_query.payee_name);
	get_zd_data("0830", sMo_city_query.payee_br_name);
	get_zd_data("0760", sMo_city_query.content);
	strcpy(sMo_city_query.sts, "0");

    ret = Mo_city_query_Ins(sMo_city_query, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�Ǽǲ�ѯ�鸴�Ǽǲ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }


	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "��ѯ��¼��");
	strcpy(g_pub_tx.no_show, "1");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ѯ�鸴̨�ʹ�����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ѯ�鸴̨�ʹ�����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

