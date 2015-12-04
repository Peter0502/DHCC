/*************************************************
* �� �� ��: spF414.c
* ��������: ������Ϣ��ӡ
*
* ��    ��: mike
* �������: 2004��05��20��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "mo_finance_info_c.h"

spF414()  
{ 	
	int ret=0;
	int flag=0;
	char cFileName[50];
    FILE *fp;
	long date=0;
	double amt1=0.00, amt2=0.00, amt3=0.00, amt4=0.00;
	int sum1=0, sum2=0, sum3=0, sum4=0;
	char c_flag[2];
    
	struct mo_finance_info_c sMo_finance_info;

	memset(&sMo_finance_info, 0x0, sizeof(struct mo_finance_info_c));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(c_flag, 0x0, sizeof(c_flag));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if ( pub_base_sysinit() )
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	get_zd_long("0440", &date);
	get_zd_data("0670", c_flag);

	if (date == 0)
	{
		date = g_pub_tx.tx_date;
	}

    ret = pub_base_AllDwnFilName( cFileName );
    if (ret != 0)
    {
        sprintf( acErrMsg, "�����ļ��������!!" );
        WRITEMSG
        strcpy(g_pub_tx.reply,"D126");
        goto ErrExit;
    }
 
    sprintf(acErrMsg,"filename=[%s]",cFileName);
    WRITEMSG

    fp = fopen( cFileName, "w" );
    if( fp==NULL )
    {
        sprintf( acErrMsg, "��ʱ�ļ�����!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B080" );
        goto ErrExit;
    }

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","ժҪ","Ʊ�ݺ���",
			"ִ�յ�λ", "������","�˺�", "���", "����", "��Ա", "��ˮ��");
	
	sprintf(acErrMsg, "-------->��������Ϊ: [%s]", g_pub_tx.tx_br_no);
	WRITEMSG

	if (c_flag[0] == 'Y')
	{
		ret = Mo_finance_info_Dec_Sel(g_pub_tx.reply, "agent_br_no = '%s' and \
				tx_date = %ld and sts in ('1', '2', '3') order by sts, \
				payer_sort", g_pub_tx.tx_br_no, date);
		if (ret != 0)
		{
			sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		ret = Mo_finance_info_Dec_Sel(g_pub_tx.reply, "agent_br_no = '%s' and \
				tx_date = %ld and sts='3'   order by sts, \
				payer_sort", g_pub_tx.tx_br_no, date);
		if (ret != 0)
		{
			sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}

	flag = 0;
	while(1)
	{
		ret = Mo_finance_info_Fet_Sel(&sMo_finance_info, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH FOR SELECT ERROR [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			if (flag == 0)
			{
				sprintf(acErrMsg, "û��Ҫ��ӡ�ļ�¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "H024");
				goto ErrExit;
			}
			break;
		}

		flag ++;

		if (sMo_finance_info.sts[0] == '1' || sMo_finance_info.sts[0] == '2')
		{
			if (sMo_finance_info.payer_sort[0] == '0')
			{
				fprintf(fp,"%s|%s|%s|%s|%s|%16.2lf|%s|%s|%ld|\n", "��������", 
					sMo_finance_info.pay_no, sMo_finance_info.unit_code,
					sMo_finance_info.payer_name, sMo_finance_info.payer_acno,
					sMo_finance_info.amt, sMo_finance_info.tx_br_no,
					sMo_finance_info.tel, sMo_finance_info.trace_no);
			}
			else
			{
				fprintf(fp,"%s|%s|%s|%s|%s|%16.2lf|%s|%s|%ld|\n", "����֧��", 
					sMo_finance_info.pay_no, sMo_finance_info.unit_code,
					sMo_finance_info.payer_name, sMo_finance_info.payer_acno,
					sMo_finance_info.amt, sMo_finance_info.tx_br_no,
					sMo_finance_info.tel, sMo_finance_info.trace_no);
			}
		}
		else
		{
			if (sMo_finance_info.payer_sort[0] == '0')
			{
				fprintf(fp,"%s|%s|%s|%s|%s|%16.2lf|%s|%s|%ld|\n", "��������", 
					sMo_finance_info.pay_no, sMo_finance_info.unit_code,
					sMo_finance_info.payer_name, sMo_finance_info.payer_acno,
					sMo_finance_info.amt, sMo_finance_info.tx_br_no,
					sMo_finance_info.tel, sMo_finance_info.trace_no);
			}
			else
			{
				fprintf(fp,"%s|%s|%s|%s|%s|%16.2lf|%s|%s|%ld|\n", "����֧��", 
					sMo_finance_info.pay_no, sMo_finance_info.unit_code,
					sMo_finance_info.payer_name, sMo_finance_info.payer_acno,
					sMo_finance_info.amt, sMo_finance_info.tx_br_no,
					sMo_finance_info.tel, sMo_finance_info.trace_no);
			}
		}
	}
	Mo_finance_info_Clo_Sel();

	ret = sql_sum_double("mo_finance_info", "amt", &amt1, "agent_br_no = '%s' \
			and payer_sort = '0' and tx_date = %ld and sts in ('1', '2')", 
			g_pub_tx.tx_br_no, date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ���������������!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	ret = sql_count("mo_finance_info", "agent_br_no = '%s' \
			and payer_sort = '0' and tx_date = %ld and sts in ('1', '2')", 
			g_pub_tx.tx_br_no, date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ������������Ŀ����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	sum1 = ret;
			
	ret = sql_sum_double("mo_finance_info", "amt", &amt2, "agent_br_no = '%s' \
			and payer_sort = '0' and tx_date = %ld and sts = '3'", 
			g_pub_tx.tx_br_no, date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ�ƴ������������!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	ret = sql_count("mo_finance_info", "agent_br_no = '%s' \
			and payer_sort = '0' and tx_date = %ld and sts = '3'", 
			g_pub_tx.tx_br_no, date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ������������Ŀ����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	sum2 = ret;

	ret = sql_sum_double("mo_finance_info", "amt", &amt3, "agent_br_no = '%s' \
			and payer_sort = '1' and tx_date = %ld and sts in ('1', '2')", 
			g_pub_tx.tx_br_no, date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ�ƴ������������!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	ret = sql_count("mo_finance_info", "agent_br_no = '%s' \
			and payer_sort = '1' and tx_date = %ld and sts in ('1', '2')", 
			g_pub_tx.tx_br_no, date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ������������Ŀ����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	sum3 = ret;

	ret = sql_sum_double("mo_finance_info", "amt", &amt4, "agent_br_no = '%s' \
			and payer_sort = '1' and tx_date = %ld and sts ='3'", 
			g_pub_tx.tx_br_no, date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ�ƴ������������!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	ret = sql_count("mo_finance_info", "agent_br_no = '%s' \
			and payer_sort = '1' and tx_date = %ld and sts = '3'", 
			g_pub_tx.tx_br_no, date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ������������Ŀ����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}

	sum4 = ret;

  	fclose(fp);
    set_zd_data( DC_FILE_SND, "1" );        /* �������ͱ�־ */
	set_zd_long("0440", date);
	set_zd_double("0390", amt1);
	set_zd_double("0400", amt2);
	set_zd_double("0410", amt3);
	set_zd_double("0420", amt4);
	set_zd_int("0340", sum1);
	set_zd_int("0350", sum2);
	set_zd_int("0360", sum3);
	set_zd_int("0480", sum4);

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ɷ���Ϣ��ӡ����ɹ�OKExit [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�ɷ���Ϣ��ӡ����ʧ��ErrExit [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
