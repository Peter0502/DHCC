/***************************************************************
* �� �� ��: spD283.c
* ��������: ͬ�ǲ�ѯ��ӡ
*
* ��    ��: jack
* �������: 2005��6��20��
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
#include "com_tx_br_rel_c.h"
#include "com_branch_c.h"

spD283()
{
    int ret;
	int flag;
	char br_no[6];
	char wherelist[500], wherelist1[500];
	char tmp[50];
    char cFileName[50];
    FILE *fp;

	long beg_date,end_date, beg_date1;
	double amt1=0.00, amt2=0.00, amt3=0.00, amt4=0.00, amt5=0.00;
	double dr_amt=0.00, cr_amt=0.00, sum1=0.00, sum2=0.00;

	double t_amt1=0.00, t_amt2=0.00, t_amt3=0.00, t_amt4=0.00;
	double t_sum1=0.00, t_sum2=0.00;

	int num1=0, num2=0, num3=0, num4=0;
	int t_num1=0, t_num2=0, t_num3=0, t_num4=0;


	struct com_tx_br_rel_c sCom_tx_br_rel;
	struct com_branch_c sCom_branch;

	memset(&sCom_tx_br_rel, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&wherelist, 0x0, sizeof(wherelist));
	memset(&wherelist1, 0x0, sizeof(wherelist1));


	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	get_zd_data("0910", br_no);
	get_zd_long("0440", &beg_date);
	get_zd_long("0450", &end_date);
    
	if (beg_date == 0)
	{
		beg_date = g_pub_tx.tx_date;
		if (end_date == 0)
		{
			end_date = g_pub_tx.tx_date;
		}
	}
	else
	{
		if (end_date == 0)
		{
			end_date = beg_date;
		}
	}
	beg_date1 = beg_date;

	sprintf(acErrMsg, "��ʼ����: [%ld], ��ֹ����: [%ld]", beg_date, end_date);
	WRITEMSG

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

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
			"�����", "����",
			"�ֹ�������", "�ֹ��������", "�Զ�������",
			"�Զ��������", "������ܽ��", "�����������", 
			"�ֹ�������", "�ֹ���������", "�Զ�������",
			"�Զ���������", "������ܽ��", "�����������");

	sprintf(wherelist,"1=1 ");
	if (strcmp(br_no, "") != 0)
	{
		sprintf(tmp, "and br_no = '%s'", br_no);
		strcat(wherelist, tmp);
	}

	strcat(wherelist," order by br_no");

	sprintf(acErrMsg,"wherelist = [%s]", wherelist);
	WRITEMSG

	while(1)
	{
    	ret = Com_branch_Dec_Sel(g_pub_tx.reply,wherelist);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}

		flag = 0;

		t_amt1 = 0.00;
		t_amt2 = 0.00;
		t_amt3 = 0.00;
		t_amt4 = 0.00;
		t_sum1 = 0.00;
		t_sum2 = 0.00;
		t_num1 = 0;
		t_num2 = 0;
		t_num3 = 0;
		t_num4 = 0;

   	 	while(1)
    	{
        	ret = Com_branch_Fet_Sel(&sCom_branch, g_pub_tx.reply);
        	if (ret != 0 && ret != 100)
        	{
            	sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�����!! [%d]",ret);
            	WRITEMSG
				strcpy(g_pub_tx.reply,"D172");
            	goto ErrExit;
        	}
        	else if (ret == 100)
        	{
				if (flag == 0) 
				{
					sprintf(acErrMsg,"û�иû�����!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D173");
					goto ErrExit;
				}
            	break;
        	}
 

			if (strcmp(br_no, "") != 0)
			{
				ret = Com_tx_br_rel_Sel(g_pub_tx.reply, &sCom_tx_br_rel,
					"rel_type = 3 and br_no = '%s' and up_br_no = '%s'",
					br_no, g_pub_tx.tx_br_no);
    			if (ret != 0 && ret != 100)
				{
					sprintf(acErrMsg,"��ѯ���׻�����ϵ�����!! [%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D167");
					goto ErrExit;
				}
				else if (ret == 100)
				{
					sprintf(acErrMsg,"���׻�����ϵ����û�иü�¼!!");
					WRITEMSG
					if (strcmp(br_no, g_pub_tx.tx_br_no) != 0)
					{
						sprintf(acErrMsg,"����ֻ�ܴ�ӡ�Լ������Ʊ�ݻ���\
									�Ĵ�ӡ���¼�������Ʊ��");
						WRITEMSG
						strcpy(g_pub_tx.reply, "D185");
						goto ErrExit;
					}
				}
			}
			else
			{
				ret = Com_tx_br_rel_Sel(g_pub_tx.reply, &sCom_tx_br_rel,
					"rel_type = 3 and br_no = '%s' and up_br_no = '%s'",
					sCom_branch.br_no, g_pub_tx.tx_br_no);
    			if (ret != 0 && ret != 100)
				{
					sprintf(acErrMsg,"��ѯ���׻�����ϵ�����!! [%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D167");
					goto ErrExit;
				}
				else if (ret == 100)
				{
					sprintf(acErrMsg,"���� [%s] ���� ���� [%s] ���ϼ�����!!",
						g_pub_tx.tx_br_no, sCom_branch.br_no);
					WRITEMSG
					continue;
				}
			}

			flag++;
			sum1 = 0.00;
			sum2 = 0.00;
			amt1 = 0.00;
			amt2 = 0.00;
			amt3 = 0.00;
			amt4 = 0.00;
			cr_amt = 0.00;
			dr_amt = 0.00;

			num1 = 0;
			num2 = 0;
			num3 = 0;
			num4 = 0;

			/* �ֹ�����������ͳ�� */
			ret = sql_sum_double("mo_city", "bill_amt", &cr_amt,
				"in_out_ind = '1' and cd_ind = '2' and trans_mode = '1' and \
				sts = '3' and tx_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg," ͳ���ֹ��������������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			/**
			ret = sql_count("mo_city", "in_out_ind = '1' and cd_ind = '2' \
				and trans_mode = '1' and sts = '3' and tx_br_no = '%s' and \
				tx_date = %ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ�������������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			num1 = ret;
			***/

			/* �ֹ�����跽���ͳ�� */
			ret = sql_sum_double("mo_city", "bill_amt", &dr_amt,
				"in_out_ind = '1' and cd_ind = '1' and trans_mode = '1' and \
				sts = '3' and tx_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg," ͳ���ֹ��������������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			amt1 = (cr_amt - dr_amt);
			sprintf(acErrMsg, "�ֹ����ͳ�ƽ��Ϊ: [%lf]", amt1);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind = '1' \
				and trans_mode = '1' and sts = '3' and tx_br_no = '%s' and \
				tx_date = %ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ�������������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			num1 = ret;

			sprintf(acErrMsg, "�ֹ����ͳ�ƽ��Ϊ: [%ld]", num1);
			WRITEMSG

			/* �Զ�������ͳ�� */
			ret = sql_sum_double("mo_city", "bill_amt", &amt2, 
				"in_out_ind = '1' and cd_ind = '2' and trans_mode = '2' and \
				sts = '7' and tx_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "ͳ���Զ����������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			sprintf(acErrMsg, "�Զ����ͳ�ƽ��Ϊ: [%lf]", amt2);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind = '1' and cd_ind = '2' \
				and trans_mode = '2' and sts = '7' and tx_br_no = '%s' and \
				tx_date = %ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ�������������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			num2 = ret;
			sprintf(acErrMsg, "�������Ϊ: [%ld]", num2);
			WRITEMSG

			/* �ֹ�����跽���ͳ�� */
			ret = sql_sum_double("mo_city", "bill_amt", &dr_amt,
				"in_out_ind = '2' and cd_ind = '1' and trans_mode = '1' and \
				sts != 'A' and payer_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "ͳ���ֹ�����跽������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			sprintf(acErrMsg, "�ֹ�����跽���Ϊ: [%lf]", dr_amt);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind='2' and cd_ind='1' and \
				trans_mode='1' and sts != 'A' and payer_br_no='%s' and \
				tx_date=%ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ���ֹ�����跽��������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			num3 = ret;

			/* �ֹ�����������ͳ�� */
			ret = sql_sum_double("mo_city", "bill_amt", &cr_amt,
				"in_out_ind = '2' and cd_ind = '2' and trans_mode = '1' and \
				sts != 'A' and payee_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "ͳ���ֹ�����跽������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			sprintf(acErrMsg, "�ֹ�����������Ϊ: [%lf]", cr_amt);
			WRITEMSG

			amt3 = (cr_amt - dr_amt);

			sprintf(acErrMsg, "ͳ���ֹ�������Ϊ: [%lf]", amt3);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind='2' and trans_mode='1' and \
				cd_ind='2' and sts != 'A' and payee_br_no='%s' and \
				tx_date=%ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ���ֹ�������������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			num3 += ret;

			/* ͳ���Զ������� */
			ret = sql_sum_double("mo_city", "bill_amt", &amt4,
				"in_out_ind = '2' and cd_ind = '2' and trans_mode = '2' and \
				sts != 'H' and payee_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "ͳ���Զ����������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			sprintf(acErrMsg, "ͳ���Զ�������Ϊ: [%lf]", amt4);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind='2' and cd_ind='2' and \
				trans_mode='2' and sts != 'H' and payee_br_no='%s' and \
				tx_date=%ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ���Զ�������������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			num4 = ret;

			/* ͳ������ͨ���� *
			ret = sql_sum_double("mo_city", "bill_amt", &amt5, 
				"sts = 'P' and tx_date = %ld", beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "ͳ������ͨ�������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			ret = sql_count("mo_city", "sts='P' and tx_date=%ld", beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ������ͨ����������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			num2 += ret;
			***/

			sum1 += amt1 + amt2;
			sum2 += amt3 + amt4;

			/* ���� */
			t_num1 += num1;
			t_num2 += num2;
			t_num3 += num3;
			t_num4 += num4;
			t_amt1 += amt1;
			t_amt2 += amt2;
			t_amt3 += amt3;
			t_amt4 += amt4;

			amt1 = amt1 / 10000;
			amt2 = amt2 / 10000;
			amt3 = amt3 / 10000;
			amt4 = amt4 / 10000;
			sum1 = sum1 / 10000;
			sum2 = sum2 / 10000;


           	fprintf(fp,"%s|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|\n", 
			sCom_branch.br_no, beg_date, amt1, num1, amt2, num2, 
			sum1, num1+num2, amt3, num3, amt4, num4, sum2, num3+num4);

    	}

		t_sum1 = t_amt1 + t_amt2;
		t_sum2 = t_amt3 + t_amt4;

		t_sum1 = t_sum1 / 10000;
		t_sum2 = t_sum2 / 10000;
		t_amt1 = t_amt1 / 10000;
		t_amt2 = t_amt2 / 10000;
		t_amt3 = t_amt3 / 10000;
		t_amt4 = t_amt4 / 10000;

        fprintf(fp,"%s|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|\n", 
			"����", beg_date, t_amt1, t_num1, t_amt2, t_num2, 
			t_sum1, t_num1 + t_num2, t_amt3, t_num3, t_amt4, t_num4, 
			t_sum2, t_num3 + t_num4);

		ret = pub_base_deadlineD(beg_date, 1, &beg_date);
		if (ret != 0)
		{
			sprintf(acErrMsg,"ȡ��һ�����ڴ���!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D186");
			goto ErrExit;
		}
		if (beg_date > end_date)
		{
			/******
			t_sum1 += sum1;
			t_sum2 += sum2;
			t_num1 += total_num1;
			t_num2 += total_num2;
			sum1 = sum1 / 10000;
			sum2 = sum2 / 10000;
			fprintf(fp, "%s|%s|%s|%lf|%s|%ld|%s|%lf|%s|%ld|\n",
					"�������", "", "������", sum1, "�������", 
					total_num1, "������", sum2, "��������", total_num2);

			beg_date = beg_date1;
			******/
			break;
		}

	}

    fclose(fp);

   	set_zd_data( DC_FILE_SND, "1" );        /* �������ͱ�־ */

	/*** �Ǽǽ�����ˮ 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������ֹ���ӡ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
   	set_zd_data( DC_FILE_SND, "0" );
	sprintf(acErrMsg,"��������ֹ���ӡ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

