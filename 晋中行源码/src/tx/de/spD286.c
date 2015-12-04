/***************************************************************
* �� �� ��: spD286.c
* ��������: ����ͳ�Ʋ�ѯ
*
* ��    ��: jack
* �������: 2004��4��13��
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
#include "trace_log_c.h"
#include "com_parm_c.h"

spD286()
{
    int ret=0;
    FILE *fp;
	char cFileName[50];
	double amt=0.00, c_amt=0.00, d_amt=0.00, amt1=0.00;
	int cnt=0;

	struct trace_log_c sTrace_log;
	struct com_parm_c sCom_parm;

	memset(&sTrace_log, 0x0, sizeof(struct trace_log_c));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    ret = pub_base_AllDwnFilName( cFileName );
    if (ret != 0)
    {
        sprintf( acErrMsg, "�����ļ��������!!" );
        WRITEMSG
        strcpy(g_pub_tx.reply,"D126");
        goto ErrExit;
    }

    fp = fopen( cFileName, "w" );
    if( fp==NULL )
    {
        sprintf( acErrMsg, "��ʱ�ļ�����!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B080" );
        goto ErrExit;
    }

    fprintf(fp,"~%s|%s|%s|%s|\n","����", "����", "���", "40801��������");

	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4231' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ���ֹ�����跽������![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4231' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ���ֹ�����跽������![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;

    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "�ֹ�����跽", cnt, amt, "����");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4231' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ���ֹ��������������![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4231' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ���ֹ��������������![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "�ֹ��������", cnt, amt, "�跽");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4241' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ���Զ��������������![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4241' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ���ֹ�����跽������![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "�Զ��������", cnt, amt, "�跽");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4252' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ������ͨ�����������![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4252' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ������ͨ�����Ʊ������![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "����ͨ�����", cnt, amt, "�跽");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4261' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "���Ĺ���(����跽)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4261' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "���Ĺ���(����跽)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "������������", cnt, amt, "����");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4261' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "���Ĺ���(�������)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4261' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "���Ĺ���(�������)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "�������������", cnt, amt, "�跽");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4262' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "��������(����跽)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4262' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "��������(����跽)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "�������������", cnt, amt, "�跽");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4262' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "��������(�������)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4262' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "��������(�������)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "�������������", cnt, amt, "����");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4265' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͬ������(���)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4265' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͬ������(���)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "ͬ������(���)", cnt, amt, "�跽");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4265' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͬ������(����)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4265' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͬ������(����)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "ͬ������(����)", cnt, amt, "����");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4268' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ͬ�ǹ���(���)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4268' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "��ͬ�ǹ���(���)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "��ͬ�ǹ���(���)", cnt, amt, "�跽");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4268' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ͬ�ǹ���(����)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4268' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "��ͬ�ǹ���(����)������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "��ͬ�ǹ���(����)", cnt, amt, "����");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &amt, "sts in ('1',  '6')", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "�������������������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log", "tx_code='4269' and sts='0'");
	if (ret < 0)
	{
		sprintf(acErrMsg, "��ѯ�Ƿ��Ѿ��˶Գɹ����� [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	else if (ret > 0)
	{
		ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='GZJE' and \
			parm_seqn = 2");
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
			parm_seqn = 1");
		if (ret != 0)
		{
			sprintf(acErrMsg, "��ѯCOM_PARM�쳣!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	amt1 = amt;

    fprintf(fp,"%s|%s|%16.2lf|%s|\n", "40801���ӦΪ", "", amt,"����");

	cnt = 0;
	amt = 0.00;
	c_amt = 0.00;
	d_amt = 0.00;
	ret = sql_sum_double("dc_log", "amt", &c_amt, "tx_date=%ld and \
		acc_hrt='40801' and sts='0' and dc_ind='1'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "����������δ���������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_sum_double("dc_log", "amt", &d_amt, "tx_date=%ld and \
		acc_hrt='40801' and sts='0' and dc_ind='2'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "����������δ���������[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	pub_base_strpack(sCom_parm.val);
	amt = d_amt - c_amt + atof(sCom_parm.val);

	if (pub_base_CompDblVal(amt, 0.00) == 1)
	{
    	fprintf(fp,"%s|%s|%16.2lf|%s|\n", "40801ʵ�����Ϊ:", "", amt, "����");
	}
	else
	{
    	fprintf(fp,"%s|%s|%16.2lf|%s|\n", "40801ʵ�����Ϊ:", "", -amt, "�跽");
	}
   	fprintf(fp,"%s|%s|%16.2lf|%s|\n", "40801�����Ϊ:", "", amt1-amt, "����");

    fclose(fp);

   	set_zd_data( DC_FILE_SND, "1" );        /* �������ͱ�־ */

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ӡ������ˮ��ѯ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
   	set_zd_data( DC_FILE_SND, "0" );
	sprintf(acErrMsg,"��ӡ������ˮ��ѯ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

