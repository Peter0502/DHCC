/***************************************************************
* �� �� ��: spD269.c
* ��������: ����˶�
*
* ��    ��: jack
* �������: 2004��6��20��
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


spD269()
{
	double dr_amt=0.00, cr_amt=0.00, amt1=0.00, amt2=0.00, tc_amt=0.00;
	char err_msg[51];
	int ret=0;

	struct com_parm_c sCom_parm;

	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
	memset(err_msg, 0x0, sizeof(err_msg));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_count("mo_city", "sts='2'");
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ���ֹ���ӡƱ�ݴ���!!");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret > 0)
	{
		sprintf(acErrMsg, "�����ֹ���ӡƱ��û�к���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D221");
		goto ErrExit;
	}

	ret = sql_count("trace_log", "tx_date=%ld and tx_br_no='%s' and \
		tx_code='4269'", g_pub_tx.tx_date, g_pub_tx.tx_br_no);
	if (ret < 0) 
	{ 
		sprintf(acErrMsg, "ͳ����ˮ���쳣 [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	}
	else if (ret == 0)
	{
		ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='GZJE' and \
			parm_seqn=1");
		if (ret != 0)
		{
			sprintf(acErrMsg, "��ѯCOM_PARM�쳣!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	else 
	{
		ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='GZJE' and \
			parm_seqn=2");
		if (ret != 0)
		{
			sprintf(acErrMsg, "��ѯCOM_PARM�쳣!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}

	pub_base_strpack(sCom_parm.val);
	tc_amt = atof(sCom_parm.val);
	sprintf(acErrMsg, "����408�������Ϊ:[%s],[%lf]", sCom_parm.val, tc_amt);
	WRITEMSG

	/* ����ͬ��δ���¼���� */
	/* ͳ��δ����������跽��� */
	ret = sql_sum_double("mo_city", "bill_amt", &dr_amt, "in_out_ind='1' and \
		cd_ind = '1' and sts in ('1', '2', '6')");
	if (ret != 0)
	{
		sprintf(acErrMsg, " ͳ��δ����跽Ʊ�ݽ�����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	/* ͳ��δ���������������� */
	ret = sql_sum_double("mo_city", "bill_amt", &cr_amt, "in_out_ind='1' and \
		cd_ind = '2' and sts in ('1', '2', '6') ");
	if (ret != 0)
	{
		sprintf(acErrMsg, " ͳ��δ�������Ʊ�ݽ�����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	/* δ���Ʊ�ݽ�� */
	amt1 = cr_amt - dr_amt;

	sprintf(acErrMsg, "δ���Ʊ�ݽ��Ϊ : [%lf]", amt1);
	WRITEMSG

	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code = 'TCKM' and \
			parm_seqn = 2");
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ѯ�������������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(sCom_parm.val);

	/* ͳ��408���Ŀ�Ľ跽��� */
	ret = sql_sum_double("dc_log", "amt", &dr_amt, "acc_hrt = '%s' and \
			dc_ind = '1' and sts = '0'", sCom_parm.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��408���Ŀ�Ľ跽������!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	sprintf(acErrMsg, "408�ⲿ��Ŀ�Ľ跽���Ϊ: [%lf]", dr_amt);
	WRITEMSG

	/* ͳ��408���Ŀ�Ĵ������ */
	ret = sql_sum_double("dc_log", "amt", &cr_amt, "acc_hrt = '%s' and \
			dc_ind = '2' and sts = '0'", sCom_parm.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��408���Ŀ�Ĵ���������!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	sprintf(acErrMsg, "ͳ��408���Ŀ�Ĵ������Ϊ: [%lf]", cr_amt);
	WRITEMSG

	amt2 = cr_amt + tc_amt - dr_amt;
	sprintf(acErrMsg, "ͳ��408�ⲿ�����Ŀ�����Ϊ: [%lf]", amt2);
	WRITEMSG

	if (pub_base_CompDblVal(amt1, amt2) != 0)
	{
		sprintf(acErrMsg, "ͬ�ǵǼǲ���δ���������408�ⲿ�����Ŀ����");
		WRITEMSG
		sprintf(acErrMsg, "ͬ�ǵǼǲ��еĽ��Ϊ:[%lf],408�ⲿ�����Ŀ:[%lf]",
			amt1, amt2);
		WRITEMSG
		sprintf(err_msg, "�˶Բ�ƽ:��:[%.2lf], ��:[%.2lf]", 
			cr_amt+tc_amt-amt1, dr_amt);
		set_zd_data("0130", err_msg);
		strcpy(g_pub_tx.reply, "HDBP");
		goto ErrExit;
	}

	ret = Com_parm_Dec_Upd(g_pub_tx.reply, "parm_code='GZJE' and parm_seqn=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ѯCOM_PARM�쳣!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
	ret = Com_parm_Fet_Upd(&sCom_parm, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "FETCH ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(sCom_parm.val, "%lf", amt1);
	sprintf(acErrMsg, "����δ������:[%s], [%lf]", sCom_parm.val, amt1);
	ret = Com_parm_Upd_Upd(sCom_parm, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "UPDATE ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	Com_parm_Clo_Upd();

	/*** �Ǽǽ�����ˮ ***/ 
	strcpy(g_pub_tx.brf, "����˶�"); 
	if( pub_ins_trace_log() ) 
	{ 
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!"); 
		WRITEMSG 
		goto ErrExit; 
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����˶Գɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����˶�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

