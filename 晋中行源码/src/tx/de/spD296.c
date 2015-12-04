/***************************************************************
* �� �� ��:     spD296.c
* ����������	����ͬ�����뱨����ӡ
* ��    ��:     mike
* ������ڣ�    2004��1��26��
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
#include "com_branch_c.h"
#include "mo_city_c.h"

spD296()
{
	 double tmp_sum1,tmp_sum2;
	 int tmp_num1,tmp_num2;
	 long tmp_date;
	 FILE *fp;
	 char filename[50],tmpname[101];
	 char br_no[7];

	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct com_branch_c	g_com_branch;
	struct mo_city_c	g_mo_city;
	struct dd_parm_c	g_dd_parm;
	struct td_parm_c	g_td_parm;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));

	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0910",br_no);					/*�������*/
	get_zd_long("0440",&tmp_date);				/*����*/

 	g_reply_int=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",br_no);
	if (g_reply_int==100)
	{
	   	sprintf(acErrMsg,"�����������в����ڸü�¼!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}
	else if (g_reply_int)
	{
	  	sprintf(acErrMsg,"��ѯ�����������쳣!!");
	  	WRITEMSG
	  	strcpy(g_pub_tx.reply,"O099");
	  	goto ErrExit;
	}

	/*�跽����Ʊ�ݸ���*/
	tmp_num1=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and cd_ind='1' and in_out_ind='1'",
			br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (tmp_num1<0)
	{
		sprintf(acErrMsg,"ȡ�跽�Զ�����Ʊ�ݸ�������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	sprintf(acErrMsg,"OKOK!!br_no=[%s],g_pub_tx.tx_date=[%ld],no=[%d]",br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	WRITEMSG
TRACE
TRACE
	/*�跽����Ʊ�ݽ����*/
	sql_sum_double("mo_city","sttl_amt",&tmp_sum1,
	" tx_br_no='%s' and tx_date=%ld and no=%d and cd_ind='1' and in_out_ind='1' ",br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(tmp_sum1,0.00);
	if(g_reply_int==-1);
	{
		sprintf(acErrMsg,"ȡ�跽�Զ�����Ʊ�ݽ��������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*ȡ��������ܱ���*/
	tmp_num2=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='1' and cd_ind='2' ",
			br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (tmp_num2<0)
	{
		sprintf(acErrMsg,"ȡ�Զ���������ܱ�������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*ȡ��������ܽ��*/
	sql_sum_double("mo_city","sttl_amt",&tmp_sum2,
					" tx_br_no='%s' and tx_date=%ld and no=%d and cd_ind='2' \
					and in_out_ind='1' ",
					g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(tmp_sum2,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"ȡ�Զ���������ܽ�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	sprintf(acErrMsg,"OKOKOKOKOOOKOKOKKOKK");
	WRITEMSG

 	g_reply_int=Mo_city_Dec_Upd(g_pub_tx.reply,
								"tx_br_no='%s' and tx_date=%ld and no=%d",
								br_no,tmp_date,g_com_branch.city_no);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"DECLARE Mo_city_Dec_Upd �α����!!g_reply_int=[%d]",
				g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

 /*��ӡ��ʼ*/
 pub_base_GenDwnFilName(filename);
 pub_base_strpack(filename);
 sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
 fp=fopen(tmpname,"w");
 fprintf(fp,"`**********����ͬ��������ܱ���************|\n");
 fprintf(fp,"`��������:%s|������Ϣ¼�����:%s|����:%ld|����:%d|\n",
  g_pub_tx.tx_br_no,g_com_branch.br_name,g_pub_tx.tx_date,g_com_branch.city_no);
 fprintf(fp,"`----------------------------------------------------------|\n");
 fprintf(fp,"������| Ʊ������| Ʊ�ݺ���|	�������˻�|	��������| ������ȫ��| �������к�| ����������| �տ����˻�| �տ�������| �տ����к�| �տ�������|\n");

	while(1)
	{
 		g_reply_int=Mo_city_Fet_Upd(&g_mo_city,g_pub_tx.reply);
		if (g_reply_int==100)break;
		else if (g_reply_int)
		{
			sprintf(acErrMsg,"FETCH Mo_city_Dec_Sel ����!!g_reply_int=[%d]!!",
					g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O099");
			goto ErrExit;
		}

 fprintf(fp,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",g_mo_city.cd_ind,g_mo_city.bill_type,g_mo_city.bill_no,g_mo_city.payer_ac_no,g_mo_city.payer_name,g_mo_city.payer_br_no,g_mo_city.payer_br_name,g_mo_city.payee_ac_no,g_mo_city.payee_name,g_mo_city.payee_br_no,g_mo_city.payee_br_name);

	g_mo_city.sts[0]=='1';						/*1-����״̬*/

	g_reply_int=Mo_city_Upd_Upd(g_mo_city,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"UPDATE Mo_city_Upd_Upd ����!!g_reply_int=[%d]!!",
				g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	}	/*while����*/

 	fprintf(fp,"`--------------------------------------------------------|\n");
 	fprintf(fp,"`�ϼ�:����跽����|����跽�ܽ��|�����������|��������ܽ��|\n");
 	fprintf(fp,"`%d|%lf|%d|%lf|\n",tmp_num1,tmp_sum1,tmp_num2,tmp_sum2);
 	fprintf(fp,"`--------------------------------------------------------|\n");
 	fprintf(fp,"`����Ա:%s		   ��ӡ����:%ld|\n",g_pub_tx.tel,g_pub_tx.tx_date);

	sprintf(acErrMsg,"OKOKOKOKOKKKOKKKKOKOKKOKOK");
	WRITEMSG

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

	Mo_city_Clo_Sel ();

 	if( pub_ins_trace_log() )
 	{
 		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
  		WRITEMSG
    	goto ErrExit;
 	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ͬ������ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����ͬ�����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
