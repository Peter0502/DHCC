/***************************************************************
* �� �� ��: spD223.c
* ��������: ��������嵥��ӡ(�Զ�)
*
* ��    ��: jack
* �������: 2004��4��04��
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
#include "com_branch_c.h"

spD223()
{
    int ret=0;
    char cFileName[50];
    FILE *fp;
    int beg_no=0, end_no=0;
    int flag=0;
	int zx_flag=0;
	long total_cnt=0;
	double total_amt = 0.00;
	char tmp_amt[20];
	char tmp_cnt[10];

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;
	struct com_branch_c sCom_branch;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(cFileName, 0x0, sizeof(cFileName));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	get_zd_long("0440", &g_pub_tx.tx_date);
    strcpy(mo_city_c.in_out_ind,"1");
    strcpy(mo_city_c.sts,"6");

	sprintf(acErrMsg, "--------->[%ld]", g_pub_tx.tx_date);
	WRITEMSG
    
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

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","���","���׻���",
			"���׻�������", "�������˺�", "����������", "����������",
			"�տ����˺�","�տ�������","�տ�������", "���", "�������",
			"��ӡ����","ժҪ");

	ret = sql_count("com_tx_br_rel", "up_br_no='%s' and rel_type=3", 
		g_pub_tx.tx_br_no);
	if ( ret < 0 )
	{
		sprintf(acErrMsg, "ͳ�ƽ��׻�����ϵ�����!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret == 0 )
	{
		sprintf(acErrMsg, "�����ӡ!!");
		WRITEMSG
    	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and tel='%s' and \
			in_out_ind='1' and trans_mode='2' order by bat_no", 
			g_pub_tx.tx_date, g_pub_tx.tel);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
		zx_flag = 0;
  	}
	else if ( ret > 0)
	{
		sprintf(acErrMsg, "���Ĵ�ӡ!!");
		WRITEMSG
    	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tc_date=%ld and in_out_ind='1' \
			and trans_mode='2' and tx_br_no in \
			(select br_no from com_tx_br_rel where up_br_no='%s' ) \
			order by bat_no", g_pub_tx.tx_date, g_pub_tx.tx_br_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
		zx_flag = 1;
	}

    flag = 0;
	total_cnt = 0;
	total_amt = 0.00;
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
                sprintf(acErrMsg,"û�м�¼!![%d]",ret);
                WRITEMSG
				strcpy(g_pub_tx.reply, "D160");
                goto ErrExit;
            }
            break;
        }

		ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
				mo_city_c.tx_br_no);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"��ѯ������������!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D172");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg,"��Ʊ�ݵĽ��׻���������!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D173");
			goto ErrExit;
		}

        flag = flag + 1;
   		total_cnt = total_cnt + 1;
		total_amt = total_amt + mo_city_c.bill_amt;
		if (mo_city_c.bill_type[0] == '1')
		{
        	fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
			mo_city_c.bat_no, mo_city_c.tx_br_no, sCom_branch.br_name, 
			mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name,
          	mo_city_c.bill_amt, "ͬ��",mo_city_c.pr_cnt,mo_city_c.brf,
			mo_city_c.remark);
		}
		else if (mo_city_c.bill_type[0] == '2')
		{
        	fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
			mo_city_c.bat_no, mo_city_c.tx_br_no, sCom_branch.br_name,
			mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name,
          	mo_city_c.bill_amt, "��ת",mo_city_c.pr_cnt,mo_city_c.brf,
			mo_city_c.remark);
		}
		else if (mo_city_c.bill_type[0] == '3')
		{
        	fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
			mo_city_c.bat_no, mo_city_c.tx_br_no, sCom_branch.br_name,
			mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name,
          	mo_city_c.bill_amt, "��Ʊ",mo_city_c.pr_cnt,mo_city_c.brf,
			mo_city_c.remark);
		}
		else 
		{ 
			fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
			mo_city_c.bat_no, mo_city_c.tx_br_no, sCom_branch.br_name,
			mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name,
          	mo_city_c.bill_amt, "����",mo_city_c.pr_cnt,mo_city_c.brf, 
			mo_city_c.remark);
		}
    }
    Mo_city_Clo_Sel();

	memset(tmp_amt, 0x0, sizeof(tmp_amt));
	memset(tmp_cnt, 0x0, sizeof(tmp_cnt));
	sprintf(tmp_amt, "%16.2lf", total_amt);
	sprintf(tmp_cnt, "%ld", total_cnt);
	fprintf(fp,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
     	"����", "", "", "", "", "", "", "", "���", total_amt, 
		"����", total_cnt, "", "");
    fclose(fp);
    set_zd_data( DC_FILE_SND, "1" );        /* �������ͱ�־ */


	/*** �Ǽǽ�����ˮ 
	strcpy(g_pub_tx.brf, "�Զ������ӡ");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������嵥��ӡ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��������嵥��ӡ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

