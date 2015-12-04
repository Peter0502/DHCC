/***************************************************************
* �� �� ��: spD285.c
* ��������: ��ӡ������ˮ��ѯ
*
* ��    ��: jack
* �������: 2004��7��13��
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

spD285()
{
    int ret=0;
    long beg_no=0, end_no=0;
	int flag1=0;
	char wherelist[500];
    char cFileName[50];
    FILE *fp;
	char flag[2];
	char tmp[50];
	int t_amt=0.00;

	struct trace_log_c sTrace_log;

	memset(&sTrace_log, 0x0, sizeof(struct trace_log_c));
	memset(wherelist, 0x0, sizeof(wherelist));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(flag, 0x0, sizeof(flag));
	memset(tmp, 0x0, sizeof(tmp));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0670", flag);
	get_zd_long("0480", &beg_no);
	get_zd_long("0490", &end_no);
    
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

	strcpy(wherelist, "no_show='1' ");

	memset(tmp, 0x0, sizeof(tmp));
	sprintf(tmp," and tx_br_no='%s' ", g_pub_tx.tx_br_no);
	strcat(wherelist, tmp);

	if (flag[0] == '1')
	{
		strcat(wherelist, " and tx_code='4212' ");
	}
	else if (flag[0] == '2')
	{
		strcat(wherelist, " and tx_code='4213' ");
	}
	else if (flag[0] == '3')
	{
		strcat(wherelist, " and tx_code='4222' ");
	}
	else if (flag[0] == '4')
	{
		strcat(wherelist, " and tx_code='4232' ");
	}
	else if (flag[0] == '5')
	{
		strcat(wherelist, " and tx_code='4233' ");
	}
	else 
	{
		strcat(wherelist, " and tx_code='4242' ");
	}

	if (flag[0] != '3' && flag[0] != '6' && flag[0] != '4' && beg_no != 0)
	{
		memset(tmp, 0x0, sizeof(tmp));
		sprintf(tmp," and ac_seqn=%ld ", beg_no);
		strcat(wherelist, tmp);
	}

	if (flag[0] == '3' || flag[0] == '6' || flag[0] == '4')
	{
		if (beg_no!=0 && end_no == 0)
		{
			memset(tmp, 0x0, sizeof(tmp));
			sprintf(tmp, " and ac_seqn <= %d and svc_ind >= %d ", 
				beg_no, beg_no);
			strcat(wherelist, tmp);
		}
		else if (beg_no==0 && end_no != 0)
		{
			memset(tmp, 0x0, sizeof(tmp));
			sprintf(tmp, " and ac_seqn <= %d and svc_ind >= %d ", 
				end_no, end_no);
			strcat(wherelist, tmp);
		}
		else if (beg_no!=0 && end_no !=0)
		{
			memset(tmp, 0x0, sizeof(tmp));
			sprintf(tmp, " and ac_seqn <= %d and svc_ind >= %d ", 
				beg_no, end_no);
			strcat(wherelist, tmp);
		}
	}

	strcat(wherelist," order by trace_no");

	sprintf(acErrMsg,"wherelist = [%s]", wherelist);
	WRITEMSG

	if (flag[0] != '3' && flag[0] != '6' && flag[0] != '4')
	{
    	fprintf(fp,"~%s|%s|%s|%s|%s|\n","��ˮ��", "���κ�", "����Ա", 
			"ժҪ", "״̬");
	}
	else
	{
    	fprintf(fp,"~%s|%s|%s|%s|%s|%s|\n","��ˮ��", "��ʼ���", "��ֹ���", 
			"����Ա", "ժҪ", "״̬");
	}

    ret = Trace_log_Dec_Sel(g_pub_tx.reply,wherelist);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	flag1 = 0;
    while(1)
    {
        ret = Trace_log_Fet_Sel(&sTrace_log, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ������ˮ�����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
			if (flag1 == 0)
			{
				sprintf(acErrMsg,"û�и�����ˮ!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D214");
				goto ErrExit;
			}
            break;
        }

		if (flag[0] != '3' && flag[0] != '6' && flag[0] != '4')
		{
			/******
			if (flag[0] == '1' || flag[0] == '2')
			{
				t_amt = 0.00;
				ret = sql_sum_double("mo_city", "bill_amt", &t_amt, 
					"bat_no=%ld and in_out_ind='1' and trans_mode='1'", 
					sTrace_log.ac_seqn);
				if (ret != 0)
				{
					sprintf(acErrMsg, "ͳ��ͳ�ƵǼǲ�����! [%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "D101");
					goto ErrExit;
				}
			}
			else
			{
				t_amt = 0.00;
				ret = sql_sum_double("mo_city", "bill_amt", &t_amt, 
					"bat_no=%ld and in_out_ind='2' and trans_mode='1'", 
					sTrace_log.ac_seqn);
				if (ret != 0)
				{
					sprintf(acErrMsg, "ͳ��ͳ�ƵǼǲ�����! [%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "D101");
					goto ErrExit;
				}
			}
			*****/

			if (sTrace_log.sts[0] == '0')
			{
       			fprintf(fp,"%ld|%d|%s|%s|%s|\n", sTrace_log.trace_no,
						sTrace_log.ac_seqn, sTrace_log.tel,
						sTrace_log.brf, "����");
			}
			else
			{
       			fprintf(fp,"%ld|%d|%s|%s|%s|\n", sTrace_log.trace_no,
						sTrace_log.ac_seqn, sTrace_log.tel,
						sTrace_log.brf, "����");
			}
		}
		else
		{
			if (sTrace_log.sts[0] == '0')
			{
       			fprintf(fp,"%ld|%d|%d|%s|%s|%s|\n", sTrace_log.trace_no,
					sTrace_log.ac_seqn, sTrace_log.svc_ind, sTrace_log.tel,
					sTrace_log.brf, "����");
			}
			else
			{
       			fprintf(fp,"%ld|%d|%d|%s|%s|%s|\n", sTrace_log.trace_no,
					sTrace_log.ac_seqn, sTrace_log.svc_ind, sTrace_log.tel,
					sTrace_log.brf, "����");
			}
		}

		flag1 = flag1 + 1;
    }
	Trace_log_Clo_Sel();

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

