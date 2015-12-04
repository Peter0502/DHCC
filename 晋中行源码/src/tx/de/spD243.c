/***************************************************************
* �� �� ��: spD243.c
* ��������: �Զ�����Ʊ���嵥��ӡ
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

spD243()
{
    int ret=0;
	int flag=0;
    char cFileName[50];
	char stat[50];
    FILE *fp;

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(stat, 0x0, sizeof(stat));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	/******
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
	*****/

	get_zd_data("0910", mo_city_c.tx_br_no);
	get_zd_long("0440", &mo_city_c.tx_date);

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

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","���","�������˺�",
            "����������", "����������","�տ����˺�","�տ�������","�տ�������",
            "���", "Ʊ��״̬");

	ret = sql_count("com_tx_br_rel", "rel_type=3 and up_br_no='%s'",  
		g_pub_tx.tx_br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ�ƽ��׻�����ϵ���쳣!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	else if ( ret )
	{
		/* ������������ */
		ret = sql_count("com_tx_br_rel", "rel_type=3 and up_br_no='%s' \
			and br_no='%s'",  g_pub_tx.tx_br_no, mo_city_c.tx_br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "ͳ�ƽ��׻�����ϵ���쳣!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D101");
			goto ErrExit;
		}
		else if (ret == 0)
		{
			sprintf(acErrMsg, "�����㲻�Ǳ��������¼�����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D185");
			goto ErrExit;
		}

  		ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tx_date = %ld and \
			tx_br_no='%s' and in_out_ind='2' and \
			trans_mode='2'", mo_city_c.tx_date, mo_city_c.tx_br_no);
		TRACE
	}
	else 
	{
		/* ��ͨ���� */
		if (strcmp(g_pub_tx.tx_br_no, mo_city_c.tx_br_no) != 0)
		{
			sprintf(acErrMsg, "��ͨ����ֻ�ܴ�ӡ�Լ���Ʊ��!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D257");
			goto ErrExit;
		}
  		ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tx_date = %ld and \
			(payer_br_no='%s' or payee_br_no='%s') and in_out_ind='2' and \
			trans_mode='2'", mo_city_c.tx_date, mo_city_c.tx_br_no, 
			mo_city_c.tx_br_no);
		TRACE
	}

	/******
  	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tx_date = %ld and \
		(payer_br_no='%s' or payee_br_no='%s') and in_out_ind='2' and \
		trans_mode='2'", mo_city_c.tx_date, mo_city_c.tx_br_no, 
		mo_city_c.tx_br_no);
	******/
   	if (ret != 0)
   	{
       	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
       	WRITEMSG
       	goto ErrExit;
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



		flag = flag + 1;
		if (mo_city_c.sts[0] == 'H')
		{
			strcpy(stat, "�����Ѿ�¼��");
		}
		else if (mo_city_c.sts[0] == 'I')
		{
			strcpy(stat, "�����Ѿ���ӡ");
		}
		else if (mo_city_c.sts[0] == 'O')
		{
			strcpy(stat, "�����Ѿ�����");
		}
		else if (mo_city_c.sts[0] == 'P')
		{
			strcpy(stat, "�����Ѿ�ͨ��");
		}
		else if (mo_city_c.sts[0] == 'Q')
		{
			strcpy(stat, "�����Ѿ���Ʊ");
		}
		else
		{
			strcpy(stat, "״̬����");
		}
 
    	fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|\n",
			mo_city_c.bat_no,
       		mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name, 
			mo_city_c.bill_amt, stat);
	}
	Mo_city_Clo_Sel();
    fclose(fp);

    set_zd_data( DC_FILE_SND, "1" );        /* �������ͱ�־ */

	/*** �Ǽǽ�����ˮ 
	strcpy(g_pub_tx.brf, "�Զ�����Ʊ�������ӡ");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�Զ�����Ʊ���嵥��ӡ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�Զ�����Ʊ���嵥��ӡ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

