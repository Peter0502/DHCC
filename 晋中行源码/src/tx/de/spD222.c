/***************************************************************
* �� �� ��: spD222.c
* ��������: ���б�����ӡ(�Զ�)
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
#include "com_branch_c.h"

spD222()
{
    int ret;
    char cFileName[50];
    FILE *fp;
    int beg_no,end_no;
    int flag;

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;
	struct com_branch_c sCom_branch;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_int("0530",&beg_no);
    get_zd_int("0540",&end_no);
    strcpy(mo_city_c.in_out_ind,"1");
    strcpy(mo_city_c.sts,"6");
    
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

    if (end_no == 0 && beg_no != 0)
    {
        /* ��ӡ���� */
        sprintf(acErrMsg,"���Ŵ�ӡ");
        WRITEMSG
        ret = Mo_city_Dec_Sel(g_pub_tx.reply,"trans_mode='2' and \
			in_out_ind='1' and bat_no=%d  and sts='7' order by bat_no", beg_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
    }
    else if (end_no == 0 && beg_no == 0)
    {
        /* ��ӡȫ�� */
        sprintf(acErrMsg,"��ӡȫ��");
        WRITEMSG
        ret = Mo_city_Dec_Sel(g_pub_tx.reply,"trans_mode='2' and \
				in_out_ind='1' and sts='6' order by bat_no");
        if (ret != 0)
        {
            sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
    }
    else 
    {
        /* ��ӡһ�� */
        sprintf(acErrMsg,"��ӡһ�� [%d] [%d]", beg_no, end_no);
        WRITEMSG
        ret = Mo_city_Dec_Sel(g_pub_tx.reply,"bat_no >= %d and bat_no <= %d \
              and sts = '7' and trans_mode='2' and in_out_ind='1' order by \
			  bat_no", beg_no, end_no);
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
                sprintf(acErrMsg,"û�м�¼!![%d]",ret);
                WRITEMSG
				strcpy(g_pub_tx.reply, "D160");
                goto ErrExit;
            }
            break;
        }

        /* �ж��Ƿ��Ǹý��׻������ϼ����� */
        ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
              and br_no='%s'", mo_city_c.tx_br_no);
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

        if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
        {
            /* �û������Ǹñ�ҵ����ϼ��������ܴ�ӡ */
			if (beg_no != 0 && end_no == 0)
			{
            	sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",mo_city_c.bat_no,
                	    mo_city_c.tx_br_no);
            	WRITEMSG
            	strcpy(g_pub_tx.reply,"D144");
            	goto ErrExit;
			}
			continue;
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
		mo_city_c.pr_cnt = mo_city_c.pr_cnt + 1;
   
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
        
        strcpy(mo_city_c.sts,"7");
       	if (end_no == 0 && beg_no != 0)
    	{
        	sprintf(acErrMsg,"���µ��Ŵ�ӡ");
        	WRITEMSG
        	ret = sql_execute("update mo_city set sts='7', \
				pr_cnt = pr_cnt + 1 , tc_date=%ld where \
				trans_mode='2' and in_out_ind='1' and bat_no=%d  and \
				sts='7'", g_pub_tx.tx_date, beg_no);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"sql_execute ERROR !! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
    	}
    	else if (end_no == 0 && beg_no == 0)
    	{
        	sprintf(acErrMsg,"���´�ӡȫ��");
        	WRITEMSG
        	ret = sql_execute("update mo_city set sts='7', \
				pr_cnt = pr_cnt + 1 , tc_date=%ld where trans_mode='2' and \
				in_out_ind='1' and sts='6' and bat_no=%d", g_pub_tx.tx_date,
				mo_city_c.bat_no);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
            	WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
            	goto ErrExit;
        	}
    	}
    	else 
    	{
        	sprintf(acErrMsg,"����һ��");
        	WRITEMSG
        	ret = sql_execute("update mo_city set sts='7', \
				pr_cnt = pr_cnt + 1 , tc_date=%ld where \
				bat_no = %d and sts = '7' and \
				trans_mode='2' and in_out_ind='1'", g_pub_tx.tx_date,
				mo_city_c.bat_no);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
    	}
		/*****
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
	strcpy(g_pub_tx.brf, "�Զ������ӡ");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������Զ���ӡ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��������Զ���ӡ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

