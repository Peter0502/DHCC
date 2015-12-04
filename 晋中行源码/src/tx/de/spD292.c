/***************************************************************
* �� �� ��:     spD292.c
* ����������	��ӡ����������ܱ���
* ��    ��:     jack
* ������ڣ�    2004��1��24��
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

spD292()
{
	 double sum1,sum2,sum3,sum4;
	 int count1,count2,count3,count4,tmp_int;
	 FILE *fp;
	 char filename[50];
	 char tmpname[101];

	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct com_branch_c	g_com_branch;
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

	sprintf(acErrMsg,"g_pub_tx.tx_br_no=[%s]!!",g_pub_tx.tx_br_no);
	WRITEMSG

	g_reply_int=Com_branch_Dec_Upd(g_pub_tx.reply,"br_no='%s'",
									g_pub_tx.tx_br_no);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"���幫���������α��쳣!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O090");
		goto ErrExit;
	}

	g_reply_int=Com_branch_Fet_Upd(&g_com_branch,g_pub_tx.reply);
	if (g_reply_int==100)
	{
		sprintf(acErrMsg,"�����������в����ڸü�¼!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O093");
		goto ErrExit;
	}
	else if (g_reply_int)
	{
		sprintf(acErrMsg,"FETCH��ѯ�����������쳣!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O089");
		goto ErrExit;
	}

	sprintf(acErrMsg,"OKOKOKOKOKKKOKKKKOKOKKOKOK");
	WRITEMSG

	/****************************�ֹ�����Ʊ�ݻ���*************************/
	/*ȡ����跽�ܱ���*/
	count1=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='1' and trans_mode='1' ",
			g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (count1<0)
	{
		sprintf(acErrMsg,"ȡ����跽�ܱ�������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*ȡ����跽�ܽ��*/
	sql_sum_double("mo_city","stll_amt",&sum1,
			" tx_br_no='%s' and tx_date=%ld and no=%d and in_out_ind='1' \
			and trans_mode='1' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,
			g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(sum1,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"ȡ����跽�ܽ�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*ȡ��������ܱ���*/
	count2=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='2' and trans_mode='1' ",
			g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (count2<0)
	{
		sprintf(acErrMsg,"ȡ��������ܱ�������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*ȡ��������ܽ��*/
	sql_sum_double("mo_city","stll_amt",&sum2,
			" tx_br_no='%s' and tx_date=%ld and no=%d and in_out_ind='2' \
			and trans_mode='1' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,
			g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(sum2,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"ȡ��������ܽ�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	sprintf(acErrMsg,"OKOKOKOKOOOKOKOKKOKK");
	WRITEMSG

	/****************************�Զ�����Ʊ�ݻ���*************************/
	/*ȡ����跽�ܱ���*/
	count3=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='1' and trans_mode='2' ",
			g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (count1<0)
	{
		sprintf(acErrMsg,"ȡ����跽�ܱ�������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*ȡ����跽�ܽ��*/
	sql_sum_double("mo_city","stll_amt",&sum3,
			" tx_br_no='%s' and tx_date=%ld and no=%d and in_out_ind='2' \
			and trans_mode='1' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,
			g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(sum3,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"ȡ����跽�ܽ�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*ȡ��������ܱ���*/
	count4=sql_count("mo_city","tx_br_no='%s' \
			and tx_date=%ld and no=%d and in_out_ind='2' and trans_mode='2' ",
			g_pub_tx.tx_br_no,g_pub_tx.tx_date,g_com_branch.city_no);
	if (count2<0)
	{
		sprintf(acErrMsg,"ȡ��������ܱ�������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	/*ȡ��������ܽ��*/
	sql_sum_double("mo_city","stll_amt",&sum4,
			" tx_br_no='%s' and tx_date=%ld and no=%d and in_out_ind='2' \
			and trans_mode='2' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,
			g_com_branch.city_no);
	g_reply_int=pub_base_CompDblVal(sum4,0.00);
	if(g_reply_int==-1)
	{
		sprintf(acErrMsg,"ȡ��������ܽ�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

 /*��ӡ��ʾ����*/
 pub_base_GenDwnFilName(filename);
 pub_base_strpack(filename);
 sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
 fp=fopen(tmpname,"w");

 fprintf(fp,"*************************ͬ��������ܱ���************************|\n");
 fprintf(fp,"��������:%s		��������:%s				����:%ld		����:%d|\n",
  g_pub_tx.tx_br_no,g_com_branch.br_name,g_pub_tx.tx_date,g_com_branch.city_no);
 fprintf(fp,"-----------------------------------------------------------------|\n");
 fprintf(fp,"------------------------�˹�����Ʊ�ݻ���-------------------------|\n");
 fprintf(fp,"����跽����    ����跽�ܽ��	    �����������    ��������ܽ��|\n");
 fprintf(fp,"%d				  %lf				  %d			 %lf 		  |\n",
			count1,sum1,count2,sum2);

 fprintf(fp,"-----------------------------------------------------------------|\n|\n");
 fprintf(fp,"------------------------�Զ�����Ʊ�ݻ���-------------------------|\n");
 fprintf(fp,"����跽����	  ����跽�ܽ��	   �����������	   ��������ܽ��\n");
 fprintf(fp,"%d				%lf				%d				%lf|\n",
			count3,sum3,count4,sum4);
 fprintf(fp,"-----------------------------------------------------------------|\n");
 fprintf(fp,"����Ա:%s									          ��ӡ����:%ld|\n",
			g_pub_tx.tel,g_pub_tx.tx_date);

	g_com_branch.city_no++;						/*ͬ�����μ�һ*/

	g_reply_int=Com_branch_Upd_Upd(g_com_branch,g_pub_tx.reply);				
	if (g_reply_int)
	{
		sprintf(acErrMsg,"UPDATE��������������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	sprintf(acErrMsg,"OKOKOKOKOKKKOKKKKOKOKKOKOK");
	WRITEMSG

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

	Com_branch_Clo_Upd();

 	if( pub_ins_trace_log() )
 	{
 		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
  		WRITEMSG
    	goto ErrExit;
 	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ӡ����������ܱ����ɹ�![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ӡ����������ܱ���ʧ��![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
