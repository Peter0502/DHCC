/***************************************************************
* �� �� ��: spD242.c
* ��������: ���㱨����ӡ(�Զ�����)
*
* ��    ��: jack
* �������: 2004��3��31��
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

spD242()
{
    int ret;
	int flag;
    char cFileName[50];
    FILE *fp;
    int beg_no,end_no;

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 and \
			br_no='%s'", g_pub_tx.tx_br_no);	
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"��ѯ���׻��������!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D167");
		goto ErrExit;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg,"���׻�����ϵ����û��Ҫ��ļ�¼!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D168");
		goto ErrExit;
	}

	/***** test *
	if (strcmp(com_tx_br_rel_c.up_br_no, g_pub_tx.tx_br_no) == 0)
	{
		sprintf(acErrMsg,"�Լ�����Լ����ô�ӡ!!");
		WRITEMSG
		ret = sql_execute("update mo_city set sts='I' where sts='H'");
		if (ret != 0)
		{
			sprintf(acErrMsg,"����ͬ�ǵǼǲ�����!![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D171");
			goto ErrExit;
		}
		else
		{
			set_zd_data("0130","�Լ�����Լ����ô�ӡ!!");
		}
		goto OkExit;
	}
	else
	{
    	set_zd_data( DC_FILE_SND, "1" );        * �������ͱ�־ *
	}
	*****/


    get_zd_int("0480",&beg_no);
    get_zd_int("0490",&end_no);
    
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

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","���","�������˺�",
            "����������", "����������","�տ����˺�","�տ�������","�տ�������",
            "���", "�������","��ӡ����","ժҪ","��ע");

    if (beg_no != 0 && end_no == 0)
	{
		/* ��ӡ���� */
    	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"bat_no=%d  and (sts='H' or \
				sts='I')", beg_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
	}
	else if (beg_no == 0 && end_no == 0)
	{
		/* ��ӡȫ�� */
    	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"sts='H'");
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
	}
	else
	{
		ret = Mo_city_Dec_Sel(g_pub_tx.reply, "bat_no >= %ld and \
				bat_no <= %ld and (sts='H' or sts='I')", beg_no, end_no);
		if (ret != 0)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

	}
  
	flag = 0;
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
			if (flag == 0)
			{
				sprintf(acErrMsg,"�����ڴ�Ʊ��!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D160");
				goto ErrExit;
			}
			break;
		}


    	if (strcmp(mo_city_c.payee_br_no,g_pub_tx.tx_br_no) != 0)
    	{
			if (beg_no != 0 && end_no == 0)
			{
        		sprintf(acErrMsg,"��Ʊ�������ڱ������ӡ!!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"D149");
        		goto ErrExit;
			}

			sprintf(acErrMsg,"�տ����к�:[%s], ���׻�����:[%s]",
						mo_city_c.payee_br_no, g_pub_tx.tx_br_no);
			WRITEMSG
			continue;
    	}

		if ( flag == 0 )
		{
		}

		flag = flag + 1;
        mo_city_c.pr_cnt = mo_city_c.pr_cnt + 1;
 
		if (mo_city_c.bill_type[0] == '1')
		{
    		fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
					mo_city_c.bat_no,
          			mo_city_c.payer_ac_no, mo_city_c.payer_name,
					mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
					mo_city_c.payee_name,mo_city_c.payee_br_name, 
					mo_city_c.bill_amt, "ͬ��", mo_city_c.pr_cnt,
					mo_city_c.brf, mo_city_c.remark);
		}
		else if (mo_city_c.bill_type[0] == '2')
		{
    		fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
					mo_city_c.bat_no,
          			mo_city_c.payer_ac_no, mo_city_c.payer_name,
					mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
					mo_city_c.payee_name,mo_city_c.payee_br_name, 
					mo_city_c.bill_amt, "��ת",mo_city_c.pr_cnt,
					mo_city_c.brf, mo_city_c.remark);
		}
		else if (mo_city_c.bill_type[0] == '3')
		{
    		fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
					mo_city_c.bat_no,
          			mo_city_c.payer_ac_no, mo_city_c.payer_name,
					mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
					mo_city_c.payee_name,mo_city_c.payee_br_name, 
					mo_city_c.bill_amt, "��Ʊ",mo_city_c.pr_cnt,
					mo_city_c.brf, mo_city_c.remark);
		}
		else 
		{
    		fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
					mo_city_c.bat_no,
          			mo_city_c.payer_ac_no, mo_city_c.payer_name,
					mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
					mo_city_c.payee_name,mo_city_c.payee_br_name, 
					mo_city_c.bill_amt, "����",mo_city_c.pr_cnt,
					mo_city_c.brf, mo_city_c.remark);
		}

    	strcpy(mo_city_c.sts,"I");
    	if (beg_no != 0 && end_no == 0)
		{
			/*���µ��� */
    		ret = sql_execute("update mo_city set sts='I', \
				pr_cnt = pr_cnt + 1 where bat_no=%d  and (sts='H' or \
				sts='I')", mo_city_c.bat_no);
    		if (ret != 0)
			{
			   	sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
			   	WRITEMSG
			   	goto ErrExit;
			}
		}
		else if (beg_no == 0 && end_no == 0)
		{
			/* ����ȫ�� */
			ret = sql_execute("update mo_city set sts='I', \
				pr_cnt = pr_cnt + 1 where bat_no=%d and sts='H'",
				mo_city_c.bat_no);
			if (ret != 0)
			{
			   	sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
			   	WRITEMSG
			   	goto ErrExit;
			}
		}
		else
		{
			ret = sql_execute("update mo_city set sts='I' ,\
				pr_cnt = pr_cnt + 1 where bat_no = %ld and \
				(sts='H' or sts='I')", mo_city_c.bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
  
		/**** 
    	ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
		*****/
	}
	Mo_city_Clo_Sel();
    fclose(fp);

    set_zd_data( DC_FILE_SND, "1" );        /* �������ͱ�־ */

	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "�Զ�����Ʊ�������ӡ");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���б�����ӡ(�Զ�)����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���б�����ӡ(�Զ�)����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

