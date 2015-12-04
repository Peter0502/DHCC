/***************************************************************
* �� �� ��: spD212.c
* ��������: �����ӡ����(�ֹ�)
*
* ��    ��: jack
* �������: 2004��3��30��
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

spD212()
{
    int ret=0;
    int bat_no=0;
	int flag=0;
    char cFileName[50];
    FILE *fp;

	char bill_name[20];
	char dc_ind[14];
	double amt=0.00, d_amt=0.00, c_amt=0.00;
	double totalamt=0.00;
	int sum=0;
	int totalsum=0;
	int i=0;

	long i_bat_no=0;

    struct mo_city_c mo_city_c;
	struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
	memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(bill_name, 0x0, sizeof(bill_name));
	memset(dc_ind, 0x0, sizeof(dc_ind));

	flag = 0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    /* get_zd_data("0670",mo_city_c.bill_type); */
	get_zd_long("0530", &i_bat_no);

    strcpy(mo_city_c.in_out_ind,"1");
    strcpy(mo_city_c.tel,g_pub_tx.tel);
    strcpy(mo_city_c.sts,"1");
    mo_city_c.tx_date = g_pub_tx.tx_date;
    
	ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 and \
			br_no='%s'",g_pub_tx.tx_br_no);
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

	/*********
	if (strcmp(g_pub_tx.tx_br_no,com_tx_br_rel_c.up_br_no) == 0)
	{
		sprintf(acErrMsg,"�Լ�����Լ����ô�ӡ!!");
		WRITEMSG
		* ������Щ��¼��״̬ *
		ret = sql_execute("update mo_city set sts='2' where tel='%s' and \
			tx_date=%ld and sts='1'", mo_city_c.bill_type, g_pub_tx.tel, 
			g_pub_tx.tx_date);
		if (ret != 0)
		{
			sprintf(acErrMsg,"����ͬ�ǵǼǲ��еļ�¼����!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D171");
			goto ErrExit;
		}
		else
		{
			* strcpy(g_pub_tx.reply,"ZZZZ"); *
			set_zd_data("0130","�Լ�����Լ����ô�ӡ!!");
		}
		goto OkExit;
	}
	else
	{
	}
	*****/

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

    /* �������κ� */
    ret = sql_max_int("mo_city","bat_no","trans_mode='1' and in_out_ind='1'");
    if (ret < 0)
    {
        sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    bat_no = ret + 1;

    /* �������κ��Ƿ��Ѵ��ڲ���״̬Ϊ2�� delete by rob at 20050712
    while(1)
    {
        ret = sql_count("mo_city","trans_mode='1' and bat_no=%d and sts='2'",
			bat_no);
        if (ret < 0)
        {
            sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret > 0)
        {
            bat_no += 1;
            continue;
        }
        else 
        {
            break;
        }
    }*/

 	sum = 0;
fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|\n","���κ�","�������˺�","���",
           "Ʊ������","�տ����˺�","�������","�������","��ӡ����");

	i = 1;
	flag = 0;
	while(1)
	/* for(i=1;i<=5;i++) */
	{
		if (i_bat_no != 0)
		{
			/* �ش� */
			TRACE
			ret = Mo_city_Dec_Upd(g_pub_tx.reply, "bat_no=%d and sts='2' and \
					bill_type='%d' and in_out_ind='1' and trans_mode='1' and \
					tel='%s'", i_bat_no, i, g_pub_tx.tel);
			if (ret != 0)
			{
				sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		else
		{
			TRACE
    		ret = Mo_city_Dec_Upd(g_pub_tx.reply,"bill_type='%d' and \
					tel='%s' and sts='1' and trans_mode='1'", i, g_pub_tx.tel);
    		if (ret != 0)
    		{
        		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        		WRITEMSG
        		goto ErrExit;
    		}	
		}

    	while(1)
    	{
        	ret = Mo_city_Fet_Upd(&mo_city_c,g_pub_tx.reply);
        	if (ret != 0 && ret != 100)
        	{
            	sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�����!! [%d]",ret);
            	WRITEMSG
           	 	goto ErrExit;
        	}
        	else if (ret == 100)
        	{
				/**
				if (flag == 0 && i == 6) 
				{
					sprintf(acErrMsg,"û�и���Ʊ��!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D160");
					goto ErrExit;
				}
				*****/
            	break;
        	}
 
			flag = flag + 1;
			if (i_bat_no == 0)
			{
        		mo_city_c.bat_no = bat_no;
			}
        	mo_city_c.pr_cnt = mo_city_c.pr_cnt + 1;

TRACE
			switch(mo_city_c.bill_type[0])
			{
				case '1':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no,mo_city_c.bill_amt, "ͬ��",
						mo_city_c.payee_ac_no,"�������", "2",mo_city_c.pr_cnt);
					break;
				case '2':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, "�й���˰Ʊ",
						mo_city_c.payee_ac_no,"�������", "2",mo_city_c.pr_cnt);
					break;
				case '3':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, "�ع���˰Ʊ",
						mo_city_c.payee_ac_no,"�������", "2",mo_city_c.pr_cnt);
					break;
				case '4':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"��ͬ�ǽ��ʵ�", mo_city_c.payee_ac_no,"�������", "2",
						mo_city_c.pr_cnt);
					break;
				case '5':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"ί���տ�", mo_city_c.payee_ac_no,"�������","2",
						mo_city_c.pr_cnt);
					break;
				case '6':
					if (mo_city_c.cd_ind[0] == '2')
					{
            			fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",
						mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"����Ʊ��", mo_city_c.payee_ac_no,"�������","2",
						mo_city_c.pr_cnt);
					}
					else
					{
            			fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",
						mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"����Ʊ��", mo_city_c.payee_ac_no,"����跽","1",
						mo_city_c.pr_cnt);
					}

					break;
				default :
					sprintf(acErrMsg,"Ʊ���������!![%s]",mo_city_c.bill_type);
					WRITEMSG
					break;
			}

			sum = sum + 1;
        	strcpy(mo_city_c.sts,"2");
        	ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
    	}
    	Mo_city_Clo_Upd();
		i = i + 1;
		if (i > 6)
		{
			if (flag == 0) 
			{
				sprintf(acErrMsg,"û�и���Ʊ��!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D160");
				goto ErrExit;
			}
			break;
		}
	}


	/* ���ܱ��� */

   	fprintf(fp,"%s|\n","����");
   	fprintf(fp,"%s|%s|%s|\n","Ʊ������","����","���");
				
	totalamt = 0.00;
	totalsum = 0;
	for(i=1;i<=6;i++)
	{
		/*** delete by rob at 20050711
		if (i_bat_no == 0)
		{
		***/

		if (i_bat_no != 0)
		{
			bat_no = i_bat_no;
		}

			/* add by rob at 20050818** for ����跽��Ʊ */
			amt = 0.00;
			d_amt = 0.00;
			c_amt = 0.00;
			ret = sql_sum_double("mo_city","bill_amt",&d_amt,"bill_type='%d' \
				and tel='%s' and bat_no=%d  and in_out_ind ='1' and \
				trans_mode='1' and cd_ind='2'", i, g_pub_tx.tel,bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg,"����Ʊ�ݽ�����!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D179");
				goto ErrExit;
			}

			ret = sql_sum_double("mo_city","bill_amt",&c_amt,"bill_type='%d' \
				and tel='%s' and bat_no=%d  and in_out_ind ='1' and \
				trans_mode='1' and cd_ind='1'", i, g_pub_tx.tel,bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg,"����Ʊ�ݽ�����!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D179");
				goto ErrExit;
			}
			amt = d_amt - c_amt;

		/***** delete by rob at 20050711 
			ret = sql_sum_double("mo_city","bill_amt",&amt,"bill_type='%d' \
				and tel='%s' and bat_no=%d  and in_out_ind ='1' and \
				trans_mode='1'", i, g_pub_tx.tel,bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg,"����Ʊ�ݽ�����!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D179");
				goto ErrExit;
			}

		}
		else
		{
			ret = sql_sum_double("mo_city", "bill_amt", &amt, "bat_no=%d and \
				bill_type='%d' and in_out_ind='1' and trans_mode='1'", 
				i_bat_no, i);
			if (ret != 0)
			{
				sprintf(acErrMsg,"����Ʊ�ݽ�����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D179");
				goto ErrExit;
			}
		}

		if (i_bat_no == 0)
		{
		*****/

			ret = sql_count("mo_city","bill_type='%d' and tel='%s' and \
					bat_no = %d and in_out_ind='1' and trans_mode='1'", 
					i, g_pub_tx.tel, bat_no);
			if (ret < 0)
			{
				sprintf(acErrMsg,"����Ʊ����������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D180");
				goto ErrExit;
			}

		/*** delete by rob at 20050711
		}
		else
		{
			ret = sql_count("mo_city", "bill_type='%d' and bat_no=%d and \
					in_out_ind = '1' and trans_mode='1'", i, bat_no);
			if (ret < 0)
			{
				sprintf(acErrMsg,"����Ʊ����������!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D180");
				goto ErrExit;
			}
		}
		********/

		totalsum = ret;

		/***** delete by rob**
		if (pub_base_CompDblVal(amt,0.00) != 1)
		{
			continue;
		}
		****/

		switch(i)
		{
			case 1:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","ͬ��",totalsum,amt);
				break;
			case 2:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","�й���˰Ʊ",totalsum,amt);
				break;
			case 3:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","�ع���˰Ʊ",totalsum,amt);
				break;
			case 4:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","��ͬ�ǽ��ʵ�",totalsum,amt);
				break;
			case 5:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","ί���տ�",totalsum,amt);
				break;
			case 6:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","����Ʊ��",totalsum,amt);
				break;
			default:
				sprintf(acErrMsg,"Ʊ���������!!");
				WRITEMSG
				goto ErrExit;
		}
	}


		if (pub_base_CompDblVal(totalamt,0.00) == -1)
		{
           	fprintf(fp,"%s|%s|%lf|\n","����","����跽",-totalamt);
		}
		else if (pub_base_CompDblVal(totalamt,0.00) == 1)
		{
           	fprintf(fp,"%s|%s|%lf|\n","����","�������",totalamt);
		}

    fclose(fp);

   	set_zd_data( DC_FILE_SND, "1" );        /* �������ͱ�־ */
	if (i_bat_no == 0)
	{
		set_zd_long("0530",bat_no);
	}
	else
	{
		set_zd_long("0530",i_bat_no);
	}
	set_zd_long("0480",sum);

	/*** �Ǽǽ�����ˮ ***/
	if (i_bat_no == 0)
	{
		strcpy(g_pub_tx.brf, "�ֹ������ӡ");
	}
	else
	{
		strcpy(g_pub_tx.brf, "�ֹ�����ش�");
	}
	strcpy(g_pub_tx.no_show, "1");
	g_pub_tx.ac_seqn = bat_no;
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
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

