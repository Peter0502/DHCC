/***************************************************************
* �� �� ��: spD281.c
* ��������: ��������ֹ�Ʊ�ݲ�ѯ
*
* ��    ��: jack
* �������: 2004��5��23��
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

spD281()
{
    int ret;
    int bat_no=0;
	char flag[2];
	int flag1=0;
	char wherelist[1024];
	char stat[50];
	char tmp[500];
    char cFileName[50];
    FILE *fp;

	double amt=0.00;
	long t_cnt = 0;
	double t_amt = 0.00;
	int i=0;

    struct mo_city_c mo_city_c;
	struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
	memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(wherelist, 0x0, sizeof(wherelist));
	memset(tmp, 0x0, sizeof(tmp));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(flag, 0x0, sizeof(flag));
	memset(stat, 0x0, sizeof(stat));

	flag1 = 0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0670",flag);
	get_zd_data("0680",mo_city_c.trans_mode);
	get_zd_double("0390", &amt);
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


	sprintf(wherelist,"in_out_ind='1'");

	if (strcmp(mo_city_c.trans_mode,"") != 0)
	{
		memset(tmp, 0x0, sizeof(tmp));
		sprintf(tmp," and trans_mode='%s'", mo_city_c.trans_mode);
		strcat(wherelist, tmp);
	}

	if (mo_city_c.tx_date != 0)
	{
		memset(tmp, 0x0, sizeof(tmp));
		sprintf(tmp, " and tx_date=%ld ", mo_city_c.tx_date);
		strcat(wherelist, tmp);
	}

	if (strcmp(flag,"") != 0)
	{
		if (flag[0] == '2')
		{
			/* �Ѿ���ӡ */
			if (mo_city_c.trans_mode[0] == '1')
			{
				strcat(wherelist," and sts='2'");
			}
			else if (mo_city_c.trans_mode[0] == '2')
			{
				strcat(wherelist," and sts='7'");
			}
			else
			{
				strcat(wherelist," and (sts='2' or sts='7')");
			}
		}
		else if (flag[0] == '1')
		{
			/* δ��ӡ */
			if (mo_city_c.trans_mode[0] == '1')
			{
				strcat(wherelist," and sts='1'");
			}
			else if (mo_city_c.trans_mode[0] == '2')
			{
				strcat(wherelist," and sts='6'");
			}
			else
			{
				strcat(wherelist," and (sts='1' or sts='6')");
			}
		}
		else if (flag[0] == '3')
		{
			/* δ���� */
			strcat(wherelist," and (sts='1' or sts='2')");
		}
		else
		{
			/* �Ѻ��� */
			strcat(wherelist, " and sts='3'");
		}
	}

	if ( pub_base_CompDblVal(amt, 0.00) > 0 )
	{
		memset(tmp, 0x0, sizeof(tmp));
		sprintf(tmp," and bill_amt = %lf",amt);
		strcat(wherelist,tmp);
	}

	ret = sql_count("com_tx_br_rel", "rel_type=3 and up_br_no='%s'", \
		g_pub_tx.tx_br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "��ѯ���׻�����ϵ�����!!");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret > 0)
	{
		memset(tmp, 0x0, sizeof(tmp));
		sprintf(tmp, " and tx_br_no in (select br_no from com_tx_br_rel \
			where rel_type=3 and up_br_no='%s')", g_pub_tx.tx_br_no);
		strcat(wherelist, tmp);
	}
	else
	{
		ret = sql_count("com_tx_br_rel", "rel_type=3 and br_no='%s'", \
			g_pub_tx.tx_br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "��ѯ���׻�����ϵ�����!!");
			WRITEMSG
			goto ErrExit;
		}
		else if (ret > 0)
		{
			memset(tmp, 0x0, sizeof(tmp));
			sprintf(tmp, " and tx_br_no = '%s'", g_pub_tx.tx_br_no);
			strcat(wherelist, tmp);
		}
		else
		{
			sprintf(acErrMsg, "���׻�����ϵ��û��û�ж���û�����Ʊ�ݴ��ݹ�ϵ");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D238");
			goto ErrExit;
		}
	}

	strcat(wherelist," order by tx_date,tx_no, bat_no, in_out_ind, trans_mode, cd_ind");

	sprintf(acErrMsg,"wherelist = [%s]", wherelist);
	WRITEMSG

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","��ˮ��", 
			"¼������", "���κ�", "�����",
			"����Ա","�������˺�","���","Ʊ������","�տ����˺�",
			"�տ�������", "�տ�������", "�������",
			"��ӡ����","���״̬","Ʊ�ݴ��ݷ�ʽ");

    ret = Mo_city_Dec_Sel(g_pub_tx.reply,wherelist);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

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
 
		flag1 = flag1 + 1;

		if (strcmp(g_pub_tx.tx_br_no, mo_city_c.tx_br_no) != 0)
		{
			ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,
				"rel_type=3 and br_no='%s'",g_pub_tx.tx_br_no);
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
		}

 if (mo_city_c.trans_mode[0] == '1')
 {
		if (mo_city_c.sts[0] == '1')
		{
			strcpy(stat,"�ֹ����¼��");
		}
		else if (mo_city_c.sts[0] == '2')
		{
			strcpy(stat,"�ֹ���������Ѿ���ӡ");
		}
		else if (mo_city_c.sts[0] == '3')
		{
			strcpy(stat,"�ֹ�Ʊ�������Ѿ����");
		}
		else if (mo_city_c.sts[0] == '0')
		{
			strcpy(stat,"����Ǽ�");
		}
		else
		{
			sprintf(acErrMsg,"���Ʊ�ݵ�״̬����!! [%s]",mo_city_c.sts);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D176");
			goto ErrExit;
		}

		switch(mo_city_c.bill_type[0])
		{
			case '1':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "ͬ��",
						mo_city_c.payee_ac_no,mo_city_c.payee_name,
						mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						stat,"�ֹ�");
				break;
			case '2':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
             			mo_city_c.payer_ac_no, mo_city_c.bill_amt, "�й�˰Ʊ",
						mo_city_c.payee_ac_no, mo_city_c.payee_name,
						mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						stat,"�ֹ�");
				break;
			case '3':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, "�ع�˰Ʊ",
						mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						stat,"�ֹ�");
				break;
			case '4':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"��ͬ�ǽ��ʵ�", mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������", mo_city_c.pr_cnt,stat,"�ֹ�");
				break;
			case '5':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"ί���տ�",mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������", mo_city_c.pr_cnt,stat,"�ֹ�");
				break;
			case '6':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"����",mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������", mo_city_c.pr_cnt,stat,"�ֹ�");
				break;
			default :
				sprintf(acErrMsg,"Ʊ���������!![%s]",mo_city_c.bill_type);
				WRITEMSG
				break;
		}
 }
 		else if (mo_city_c.trans_mode[0] == '2')
 		{
			if (mo_city_c.sts[0] == '0')
			{
				strcpy(stat,"����Ǽ�");
			}
			else if (mo_city_c.sts[0] == '6')
			{
				strcpy(stat,"�Զ����¼��");
			}
			else if (mo_city_c.sts[0] == '7')
			{
				strcpy(stat,"�Զ���������Ѿ���ӡ");
			}
			else
			{
				sprintf(acErrMsg,"���Ʊ�ݵ�״̬����!! [%s]",mo_city_c.sts);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D176");
				goto ErrExit;
			}

			switch(mo_city_c.bill_type[0])
			{
				case '1':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "ͬ��",
						mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						stat,"�Զ�");
					break;
				case '2':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "��ת",
						mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						stat,"�Զ�");
					break;
				case '3':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "��Ʊ",
						mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						stat,"�Զ�");
					break;
				/***
				case '4':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "��Ʊ",
						mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						stat,"�Զ�");
					break;
				****/
				case '5':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "����",
						mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						stat,"�Զ�");
					break;
				default:
					sprintf(acErrMsg,"Ʊ���������!![%s]",mo_city_c.bill_type);
					WRITEMSG
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "",
						mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						stat,"�Զ�");
					break;
			}
 		}
 		else
 		{
			if (mo_city_c.sts[0] != '0')
			{
				sprintf(acErrMsg,"��Ʊ�ݵĴ��ݷ�ʽ����!! [%s]",
					mo_city_c.trans_mode);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D177");
				goto ErrExit;
			}
			else
			{
				if (mo_city_c.cd_ind[0] == '1')
				{
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "",
						mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"����跽",mo_city_c.pr_cnt,
						"����Ǽ�","");
				}
				else if (mo_city_c.cd_ind[0] == '2')
				{
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.tx_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "",
						mo_city_c.payee_ac_no,
						mo_city_c.payee_name, mo_city_c.payee_br_name,
						"�������",mo_city_c.pr_cnt,
						"����Ǽ�","");
				}
				else
				{
					sprintf(acErrMsg,"��������־����!! [%s]",
						mo_city_c.cd_ind);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D178");
					goto ErrExit;
				}
			}
 		}
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

