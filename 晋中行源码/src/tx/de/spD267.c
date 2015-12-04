/***************************************************************
* �� �� ��: spD267.c
* ��������: ֧�ж���������
*
* ��    ��: jack
* �������: 2004��4��04��
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
#include "com_parm_c.h"
#include "com_tx_br_rel_c.h"

spD267()
{
    int ret;
    char flag[2];
	double tc_d_amt=0.00, tc_c_amt=0.00, ye_d_amt=0.00, ye_c_amt=0.00;
	double tc_d_amt_z=0.00, tc_c_amt_z=0.00;
	char err_msg[61];
    struct com_parm_c com_parm_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(err_msg, 0x0, sizeof(err_msg));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_data("0670",flag);

    /* ����Ƿ���֧�ж����� */
    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=4 and \
          br_no='%s'", g_pub_tx.tx_br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ���׻�����ϵ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"�û�������ִ�д˽���!! ",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D156");
        goto ErrExit;
    }

	/* 408���Ŀ */
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	pub_base_strpack(com_parm_c.val); /* 408�� */

	ret = sql_count("mo_city", "tx_date=%ld and \
		sts = '2' and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s')",
		g_pub_tx.tx_date, g_pub_tx.tx_br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ��ͬ�ǵǼǲ��쳣!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret > 0)
	{
		sprintf(acErrMsg, "�����Ļ����ֹ���ӡƱ��û�к���[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D221");
		goto ErrExit;
	}

	/* ͳ�Ʒ�����û�������� */
	tc_d_amt = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &tc_d_amt, "tx_date=%ld and \
		sts in ('1', '6') and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s') and cd_ind='1'", 
		g_pub_tx.tx_date, g_pub_tx.tx_br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��ͬ�ǵǼǲ��е��������쳣!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** ����δ����������Ϊ: [%lf]", tc_d_amt);
	WRITEMSG

	tc_c_amt = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &tc_c_amt, "tx_date=%ld and \
		sts in ('1', '6') and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s') and cd_ind='2'", 
		g_pub_tx.tx_date, g_pub_tx.tx_br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��ͬ�ǵǼǲ��е��������쳣!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** ����δ����跽���Ϊ: [%lf]", tc_c_amt);
	WRITEMSG

	tc_c_amt_z = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &tc_c_amt_z, "tx_date!=%ld and \
		sts in ('3', '7') and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s') and cd_ind='2' \
		and tc_date=%ld", 
		g_pub_tx.tx_date, g_pub_tx.tx_br_no, g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��ͬ�ǵǼǲ��е��������쳣!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** �����������Ʊ�ݽ跽���Ϊ: [%lf]", 
		tc_c_amt_z);
	WRITEMSG

	tc_d_amt_z = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &tc_d_amt_z, "tx_date!=%ld and \
		sts in ('3', '7') and tx_br_no in (select br_no from \
		com_tx_br_rel where rel_type=3 and up_br_no='%s') and cd_ind='1' \
		and tc_date=%ld", 
		g_pub_tx.tx_date, g_pub_tx.tx_br_no, g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��ͬ�ǵǼǲ��е��������쳣!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** �����������Ʊ�ݽ跽���Ϊ: [%lf]", 
		tc_d_amt_z);
	WRITEMSG

	/* ͳ�Ʒ����������� */

	/* ͳ�Ʒ����ĵ�408����� */
	/* ��408�� */
	ye_d_amt = 0.00;
	ret = sql_sum_double("dc_log", "amt", &ye_d_amt, "tx_date=%ld and \
		tx_tx_br_no in (select br_no from com_tx_br_rel where rel_type=3 and \
		up_br_no='%s') and sts='0' and \
		dc_ind='1' and acc_hrt='%s'", g_pub_tx.tx_date, g_pub_tx.tx_br_no,
		com_parm_c.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��408��跽������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** 408��跽���Ϊ [%lf]", ye_d_amt);
	WRITEMSG

	ye_c_amt = 0.00;
	ret = sql_sum_double("dc_log", "amt", &ye_c_amt, "tx_date=%ld and \
		tx_tx_br_no in (select br_no from com_tx_br_rel where rel_type=3 and \
		up_br_no='%s') and sts='0' and \
		dc_ind='2' and acc_hrt='%s'", g_pub_tx.tx_date, g_pub_tx.tx_br_no,
		com_parm_c.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��408�����������! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg, "CGH****** 408��������: [%lf]", ye_c_amt);
	WRITEMSG

	sprintf(acErrMsg, "CGH****** 408�����Ϊ: [%lf]", 
		ye_c_amt - ye_d_amt);
	WRITEMSG

	if (flag[0] == '1')
	{
		g_pub_tx.tx_amt1 = ye_c_amt - ye_d_amt - g_pub_tx.tx_amt1;
	}
	else 
	{
		g_pub_tx.tx_amt1 = ye_c_amt - ye_d_amt + g_pub_tx.tx_amt1;
	}

	sprintf(acErrMsg, "CGH****** 408�����Ϊ: [%lf]", g_pub_tx.tx_amt1);
	WRITEMSG
	g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 + tc_c_amt_z - tc_d_amt_z;

	if (pub_base_CompDblVal(g_pub_tx.tx_amt1, tc_c_amt - tc_d_amt) != 0)
	{
		/*
		sprintf(acErrMsg, "�����Ŀ����ȷ!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D223");
		*/
		sprintf(acErrMsg, "ͬ�ǵǼǲ��еĽ��Ϊ:[%lf],408�ⲿ�����Ŀ:[%lf]", 
			g_pub_tx.tx_amt1, tc_c_amt - tc_d_amt); 
		WRITEMSG 

		if (pub_base_CompDblVal(g_pub_tx.tx_amt1, tc_c_amt - tc_d_amt) < 0)
		{
			sprintf(err_msg, "�˶Բ�ƽ:40801�����ټǽ��:[%.2lf]", 
				tc_c_amt-tc_d_amt-g_pub_tx.tx_amt1); 
		}
		else
		{
			sprintf(err_msg, "�˶Բ�ƽ:40801�跽�ټǽ��:[%.2lf]", 
				g_pub_tx.tx_amt1 - tc_c_amt + tc_d_amt); 
		}
		set_zd_data("0130", err_msg); 
		strcpy(g_pub_tx.reply, "HDBP"); 
		goto ErrExit;
	}

	/****** 
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (flag[0] == '2')
    {
        * ��� ��408�� *
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","֧�ж������������");
    }
    else
    {
        * ���� ��408�� *
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        set_zd_data("1215","2");
        set_zd_data("121A","֧�ж������������");
    }

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=3");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    if (flag[0] == '2')
    {
        * ��� ���ڲ������Ŀ *
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        set_zd_data("1215","2");
        set_zd_data("121A","֧�ж������������");
    }
    else
    {
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","֧�ж������������");
    }
	**/
    
	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "֧�ж���������");
    get_zd_double("0390",&g_pub_tx.tx_amt1);
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"֧�ж��������㴦��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"֧�ж��������㴦��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

