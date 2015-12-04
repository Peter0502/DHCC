/***************************************************************
* �� �� ��: spD284.c
* ��������: Ʊ�ݺ�����ѯ
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
#include "mo_city_c.h"
#include "com_tx_br_rel_c.h"

spD284()
{
    int ret=0;
    int bat_no=0;
	char flag[2];
	int flag1=0;
	char wherelist[500];
	char stat[50];
	char tmp[50];
    char cFileName[50];
    FILE *fp;

	double amt=0.00, d_amt=0.00, c_amt=0.00;
	int num=0;
	int i=0;
	char br_no[6];

    struct mo_city_c mo_city_c;
	struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
	memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(wherelist, 0x0, sizeof(wherelist));
	memset(flag, 0x0, sizeof(flag));
	memset(stat, 0x0, sizeof(stat));
	memset(tmp, 0x0, sizeof(tmp));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(br_no, 0x0, sizeof(br_no));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0670",flag);
	get_zd_long("0480",&mo_city_c.bat_no);
	get_zd_data("0680", mo_city_c.in_out_ind);
	/* get_zd_data("0910", br_no); */
	get_zd_long("0440", &mo_city_c.tx_date);
    
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

	/*****
 	if (strcmp(br_no, "") != 0)
	{
		ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,
				"rel_type=3 and br_no='%s'",br_no);
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
			strcpy(g_pub_tx.reply,"D168");
			goto ErrExit;
		}

		if (strcmp(g_pub_tx.tx_br_no, com_tx_br_rel_c.up_br_no) != 0)
		{
			sprintf(acErrMsg, "�ý��׻������Ǹû������ϼ�����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D144");
			goto ErrExit;
		}
	}
	********/

	sprintf(wherelist,"trans_mode='1'");

	if (strcmp(mo_city_c.in_out_ind,"") != 0)
	{
		sprintf(tmp," and in_out_ind='%s'", mo_city_c.in_out_ind);
		strcat(wherelist, tmp);
	}

	if (mo_city_c.bat_no != 0)
	{
		sprintf(tmp," and bat_no=%ld ", mo_city_c.bat_no);
		strcat(wherelist, tmp);
	}

	if (strcmp(flag,"") != 0)
	{
		if (flag[0] == 'N')
		{
			/* δ���� */
			strcat(wherelist, " and (sts='2' or sts='B')");
		}
		else
		{
			/* �Ѻ��� */
			strcat(wherelist, " and sts in ('3', 'C', 'O', 'Q')");

			if ( mo_city_c.in_out_ind[0] == '1')
			{
				if (mo_city_c.tx_date > 0)
				{
					sprintf(tmp," and tc_date=%ld ", mo_city_c.tx_date);
					strcat(wherelist, tmp);
				}
			}
			else 
			{
				if (mo_city_c.tx_date > 0)
				{
					sprintf(tmp," and tx_date=%ld ", mo_city_c.tx_date);
					strcat(wherelist, tmp);
				}
			}
		}
	}
	else
	{
		strcat(wherelist, " and sts in ('2', '3', 'B', 'C', 'O', 'Q')");
	}

	strcat(wherelist," order by in_out_ind, bat_no");

	sprintf(acErrMsg,"wherelist = [%s]", wherelist);
	WRITEMSG

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|\n","���κ�","�����",
			"����Ա","����", "���","������־","��������");

    ret = Mo_city_Dec_Sel(g_pub_tx.reply,wherelist);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	flag1 = 0;
	bat_no = 0;
	TRACE
    while(1)
    {
        ret = Mo_city_Fet_Sel(&mo_city_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
			if (flag1 == 0)
			{
				sprintf(acErrMsg,"û�и���Ʊ��!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D160");
				goto ErrExit;
			}
            break;
        }

		if (mo_city_c.bat_no == 0)
		{
			continue;
		}
 
		if (mo_city_c.bat_no != bat_no)
		{
			bat_no = mo_city_c.bat_no;
		}
		else
		{
			continue;
		}

		if (mo_city_c.in_out_ind[0] == '1')
		{
			/* ��� */
			ret = sql_count("com_tx_br_rel", "rel_type=3 and br_no='%s' \
				and up_br_no='%s'", mo_city_c.tx_br_no, 
				g_pub_tx.tx_br_no);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ�ƽ��׻�����ϵ�����! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			else if (ret == 0)
			{
				ret = sql_count("com_tx_br_rel", "rel_type=3 and \
					br_no='%s' and up_br_no='%s'", mo_city_c.tx_br_no, 
					g_pub_tx.tx_br_no);
				if (ret < 0)
				{
					sprintf(acErrMsg, "ͳ�ƽ��׻�����ϵ�����! [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				else if (ret == 0)
				{
					ret = sql_count("com_tx_br_rel", "rel_type=3 and \
						br_no='%s'", g_pub_tx.tx_br_no);
					if (ret < 0)
					{
						sprintf(acErrMsg, "ͳ�ƽ��׻�����ϵ�����! [%d]", ret);
						WRITEMSG
						goto ErrExit;
					}
					else if (ret == 0)
					{
						sprintf(acErrMsg, "û�ж���û�����Ʊ�ݴ��ݹ�ϵ!!");
						WRITEMSG
						strcpy(g_pub_tx.reply, "D221");
						goto ErrExit;
					}
					else 
					{
						if (strcmp(mo_city_c.tx_br_no, g_pub_tx.tx_br_no) != 0)
						{
							continue;
						}
					}
				}
			}

			ret = sql_count("mo_city", "bat_no = %ld and tx_br_no='%s' and \
				tel='%s' and trans_mode='1' and in_out_ind='1'",
				mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ����������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D104");
				goto ErrExit;
			}

			num = ret;

			ret = sql_sum_double("mo_city", "bill_amt", &c_amt, "bat_no=%ld \
				and tx_br_no='%s' and tel='%s' and trans_mode='1' and \
				in_out_ind='1' and cd_ind='1'", mo_city_c.bat_no,
				mo_city_c.tx_br_no, mo_city_c.tel);
			if (ret != 0)
			{
				sprintf(acErrMsg, "ͳ�ƽ跽������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D104");
				goto ErrExit;
			}

			ret = sql_sum_double("mo_city", "bill_amt", &d_amt, "bat_no=%ld \
				and tx_br_no='%s' and tel='%s' and trans_mode='1' and \
				in_out_ind='1' and cd_ind='2'", mo_city_c.bat_no,
				mo_city_c.tx_br_no, mo_city_c.tel);
			if (ret != 0)
			{
				sprintf(acErrMsg, "ͳ�ƴ���������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D104");
				goto ErrExit;
			}

			if (pub_base_CompDblVal(d_amt, c_amt) == 1)
			{
				amt = d_amt - c_amt;
			}
			else
			{
				amt = c_amt - d_amt;
			}

			if (mo_city_c.sts[0] == '2')
			{
       			fprintf(fp,"%ld|%s|%s|%ld|%16.2lf|%s|%s|\n",
					mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
					num, amt, "δ����", "���");
			}
			else
			{
       			fprintf(fp,"%ld|%s|%s|%d|%16.2lf|%s|%s|\n",
					mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
					num, amt, "�Ѻ���", "���");
			}
		}
		else
		{
			/* ���� */
			ret = sql_count("com_tx_br_rel", "rel_type=3 and up_br_no='%s'",
				g_pub_tx.tx_br_no);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ�ƽ��׻�����ϵ�����! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			else if (ret >0)
			{
				if (strcmp(mo_city_c.tx_br_no, g_pub_tx.tx_br_no) != 0)
				{
					continue;
				}
			}
			else
			{
				ret = sql_count("com_tx_br_rel", "rel_type=3 and \
					br_no='%s'", g_pub_tx.tx_br_no);
				if (ret < 0)
				{
					sprintf(acErrMsg, "ͳ�ƽ��׻�����ϵ�����! [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				else if (ret >0)
				{
					if (strcmp(mo_city_c.payee_br_no, g_pub_tx.tx_br_no) != 0 \
						&& strcmp(mo_city_c.payer_br_no, g_pub_tx.tx_br_no) !=0)
					{
						continue;
					}
				}
				else
				{
					sprintf(acErrMsg, "û�ж���û�����Ʊ�ݴ��ݹ�ϵ!!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "D238");
					goto ErrExit;
				}
			}

			/* ���� */
			ret = sql_count("mo_city", "bat_no = %ld and trans_mode='1' \
				and in_out_ind='2'", mo_city_c.bat_no);
			if (ret < 0)
			{
				sprintf(acErrMsg, "ͳ����������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D104");
				goto ErrExit;
			}

			num = ret;

			ret = sql_sum_double("mo_city", "bill_amt", &c_amt, "bat_no=%ld \
				and trans_mode='1' and in_out_ind='2' and cd_ind='1'", 
				mo_city_c.bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "ͳ�ƽ跽������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D104");
				goto ErrExit;
			}

			ret = sql_sum_double("mo_city", "bill_amt", &d_amt, "bat_no=%ld \
				and trans_mode='1' and in_out_ind='2' and cd_ind='2'", 
				mo_city_c.bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "ͳ�ƴ���������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D104");
				goto ErrExit;
			}

			if (pub_base_CompDblVal(d_amt, c_amt) == 1)
			{
				amt = d_amt - c_amt;
			}
			else
			{
				amt = c_amt - d_amt;
			}

			if (mo_city_c.cd_ind[0] == '1')
			{
				strcpy(br_no, mo_city_c.payer_br_no);
			}
			else
			{
				strcpy(br_no, mo_city_c.payee_br_no);
			}

			if (mo_city_c.sts[0] == 'B')
			{
       			fprintf(fp,"%ld|%s|%s|%d|%16.2lf|%s|%s|\n",
					mo_city_c.bat_no, br_no, "", num, amt, "δ����","����");
			}
			else
			{
       			fprintf(fp,"%ld|%s|%s|%d|%16.2lf|%s|%s|\n",
					mo_city_c.bat_no, br_no, "", num, amt, "�Ѻ���","����");
			}
		}

		flag1 = flag1 + 1;
    }
	Mo_city_Clo_Sel();

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

