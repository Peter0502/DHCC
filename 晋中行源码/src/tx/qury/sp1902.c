/***************************************************************
* �� �� ��: sp1902.c
* �����������ͻ���ʷ���׼�¼��ѯ
* ��    ��: peter
* ������ڣ�2003��3��19��
*
* �޸ļ�¼��
*    ��    ��:	2003��06��08��
*    �� �� ��:	peter
*    �޸�����:	�������
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "prdt_ac_id_c.h"
#include "cif_basic_inf_c.h"
#include "tx_def_c.h"

sp1902()
{
	long tmp_date1,tmp_date2;
	char f_dc_ind[5];
	char f_ct_ind[5];
	char tx_name[21];
	char tmp_title[41];
	char filename[100];
	char wherelist[1024];  	/**��ѯ����**/
	char tmpname[512],tmpstr[150];
	char flag[3];
	int ttlnum=0;			/**��ȡ��������**/
	int i=0;
	int ret=0;
	FILE *fp;


	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct dd_mst_hst_c	g_dd_mst_hst;
	struct td_mst_hst_c	g_td_mst_hst;
	struct ln_mst_hst_c	g_ln_mst_hst;
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct cif_basic_inf_c	g_cif_basic_inf;
	struct tx_def_c		tx_def_tmp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
	memset(&g_td_mst_hst,0x00,sizeof(struct td_mst_hst_c));
	memset(&g_ln_mst_hst,0x00,sizeof(struct ln_mst_hst_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&tx_def_tmp,0x00,sizeof(struct tx_def_c));
	
	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_long("0280",&g_pub_tx.cif_no);				/*�ͻ���*/
	get_zd_long("0440",&tmp_date1);						/*��ʼ����*/
	get_zd_long("0450",&tmp_date2);						/*��ֹ����*/

	if(tmp_date1==0&&tmp_date2==0)
	{
		strcpy(flag,"00");
		strcpy(tmpstr,"ac_id=%ld and ac_seqn=%d");
	}
	else if(tmp_date1&&tmp_date2==0)
	{
		strcpy(flag,"10");
		strcpy(tmpstr,"ac_id=%ld and ac_seqn=%d and tx_date>=%ld");
	}
	else if(tmp_date1==0&&tmp_date2)
	{
		strcpy(flag,"01");
		strcpy(tmpstr,"ac_id=%ld and ac_seqn=%d and tx_date<=%ld");
	}
	else if(tmp_date1&&tmp_date2)
	{
		strcpy(flag,"11");
		strcpy(tmpstr,"ac_id=%ld and ac_seqn=%d and tx_date>=%ld and \
			   tx_date<=%ld");
	}

	if(g_pub_tx.cif_no)							/*�ͻ���NOT NULL�����*/
	{
		/* ȡ�ͻ����� */
		g_reply_int=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf, \
									"cif_no='%ld'", g_pub_tx.cif_no);
		if (g_reply_int)
		{
			sprintf(acErrMsg,"SELECT �ͻ�������Ϣ���쳣!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C007");
			goto ErrExit;
		}

		strcpy(g_pub_tx.name,g_cif_basic_inf.name);

		g_reply_int=Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "cif_no='%ld'", \
										g_pub_tx.cif_no);
		if (g_reply_int)
		{
			sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id] �쳣!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}
	}
	else {
		sprintf(acErrMsg,"������ͻ���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C075");
		goto ErrExit;
	}

	/*��ӡ��ʼ*/
	pub_base_GenDwnFilName(filename);
	pub_base_strpack(filename);
	sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
	fp=fopen(tmpname,"w");

	/**��ʾ�б���**/
 	fprintf(fp,"~�ͻ���|@�ͻ�����|��������|@���׻���|@��Ʒ����|@���״���|@��������|@��//����|@����|$���׽��|@��ת��־|\n");
  vtcp_log("[%s][%d]I am here00999999999999999999999999999999999999995!\n",__FILE__,__LINE__);	
	while(1)
	{
		g_reply_int=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if (g_reply_int==100)break;
		else if(g_reply_int)
		{
			sprintf(acErrMsg,"FETCT ��Ʒ�ʺŶ��ձ��쳣!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}

		sprintf(acErrMsg,"XXXXXXg_prdt_ac_id.ac_id=[%ld], \
				g_prdt_ac_id.ac_seqn=[%d]", \
				g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn);
		WRITEMSG
		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_id=%ld",g_prdt_ac_id.ac_id);
		if(g_reply_int==100)
		{
			sprintf(acErrMsg,"�޴��ʺű�����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"N062");
			break;
		}
		/* �ͻ��� */
		g_pub_tx.cif_no=g_prdt_ac_id.cif_no;
		/* �˺� */
		strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);

		sprintf(acErrMsg,"XXXXXXXg_pub_tx.cif_no=[%ld], \
				g_prdt_ac_id.ac_id_type=[%s]", \
				g_pub_tx.cif_no,g_prdt_ac_id.ac_id_type);
		WRITEMSG
		strcpy(g_pub_tx.name,g_cif_basic_inf.name);

vtcp_log("rowid11111111111111111111111111111==[%20s]\n",g_pub_tx.name);
vtcp_log("rowid22222222222222222222222222222==[%20s]\n",g_cif_basic_inf.name);


		switch(g_prdt_ac_id.prdt_no[0])
		{
			case '1':
					ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
					 	"prdt_no='%s'",g_prdt_ac_id.prdt_no);
					if(ret) goto ErrExit;
					
					/* ��Ʒ���� */
					strcpy(tmp_title,g_dd_parm.title);

						sprintf(acErrMsg,"XXXXg_dd_parm.title=[%s]!!", \
								g_dd_parm.title);
						WRITEMSG
					if(!strcmp(flag,"00"))
					{
						ret=Dd_mst_hst_Dec_Sel(g_pub_tx.reply, \
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );						
						if(ret)
						{
							sprintf(acErrMsg,"Dd_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Dd_mst_hst_Fet_Sel(&g_dd_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Dd_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							TRACE
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_dd_mst_hst.tx_code);
							if( ret ) goto ErrExit;
							TRACE
							
							strcpy(g_pub_tx.tx_br_no,g_dd_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_dd_mst_hst.tx_code);
							g_pub_tx.tx_date=g_dd_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_dd_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_dd_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_dd_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 							
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_dd_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
						
							
						}
						Dd_mst_hst_Clo_Sel();
					}			
					else if(!strcmp(flag,"10"))
					{
						ret=Dd_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn, 
							tmp_date1);						
						if(ret)
						{
							sprintf(acErrMsg,"Dd_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Dd_mst_hst_Fet_Sel(&g_dd_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Dd_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_dd_mst_hst.tx_code);
							if( ret ) goto ErrExit;						
							strcpy(g_pub_tx.tx_br_no,g_dd_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_dd_mst_hst.tx_code);
							g_pub_tx.tx_date=g_dd_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_dd_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_dd_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_dd_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_dd_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
						
							
						}
						Dd_mst_hst_Clo_Sel();
					}
					else if(!strcmp(flag,"01"))
					{
						ret=Dd_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn, 
							tmp_date2);						
						if(ret)
						{
							sprintf(acErrMsg,"Dd_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Dd_mst_hst_Fet_Sel(&g_dd_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Dd_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_dd_mst_hst.tx_code);
							if( ret ) goto ErrExit;						
							strcpy(g_pub_tx.tx_br_no,g_dd_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_dd_mst_hst.tx_code);
							g_pub_tx.tx_date=g_dd_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_dd_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_dd_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_dd_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_dd_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
						
							
						}
						Dd_mst_hst_Clo_Sel();
					}
					else if(!strcmp(flag,"11"))
					{
						ret=Dd_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn, 
							tmp_date1,tmp_date2);						
						if(ret)
						{
							sprintf(acErrMsg,"Dd_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Dd_mst_hst_Fet_Sel(&g_dd_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Dd_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_dd_mst_hst.tx_code);
							if( ret ) goto ErrExit;						
							strcpy(g_pub_tx.tx_br_no,g_dd_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_dd_mst_hst.tx_code);
							g_pub_tx.tx_date=g_dd_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_dd_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_dd_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_dd_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_dd_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
						
							
						}
						Dd_mst_hst_Clo_Sel();
					}

					break;
			case '2':
					TRACE
					TRACE
					TRACE
					ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, 
					 	"prdt_no='%s'",g_prdt_ac_id.prdt_no);
					if( ret ) goto ErrExit;
					TRACE
					strcpy(tmp_title,g_td_parm.title);

					if(!strcmp(flag,"00"))
					{
					TRACE
						ret=Td_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );						
						if(ret)
						{
							sprintf(acErrMsg,"Td_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							TRACE
							ret=Td_mst_hst_Fet_Sel(&g_td_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Td_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							TRACE
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_td_mst_hst.tx_code);
							if( ret ) goto ErrExit;	
							TRACE					
							strcpy(g_pub_tx.tx_br_no,g_td_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_td_mst_hst.tx_code);
							g_pub_tx.tx_date=g_td_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_td_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_td_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_td_mst_hst.add_ind);
							TRACE
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_td_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
							TRACE					
						}
						Td_mst_hst_Clo_Sel();
					}
					else if(!strcmp(flag,"10"))
					{
						TRACE
						ret=Td_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn, 
							tmp_date1);						
						if(ret)
						{
							sprintf(acErrMsg,"Td_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Td_mst_hst_Fet_Sel(&g_td_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Td_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_td_mst_hst.tx_code);
							if( ret ) goto ErrExit;						
							strcpy(g_pub_tx.tx_br_no,g_td_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_td_mst_hst.tx_code);
							g_pub_tx.tx_date=g_td_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_td_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_td_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_td_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_td_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
												
						}
						Td_mst_hst_Clo_Sel();
					}
					else if(!strcmp(flag,"01"))
					{
						TRACE
						ret=Td_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn, 
							tmp_date2);						
						if(ret)
						{
							sprintf(acErrMsg,"Td_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Td_mst_hst_Fet_Sel(&g_td_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Td_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_td_mst_hst.tx_code);
							if( ret ) goto ErrExit;						
							strcpy(g_pub_tx.tx_br_no,g_td_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_td_mst_hst.tx_code);
							g_pub_tx.tx_date=g_td_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_td_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_td_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_td_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_td_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
												
						}
						Td_mst_hst_Clo_Sel();
					}
					else if(!strcmp(flag,"11"))
					{
						TRACE
						ret=Td_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn, 
							tmp_date1,tmp_date2);						
						if(ret)
						{
							sprintf(acErrMsg,"Td_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Td_mst_hst_Fet_Sel(&g_td_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Td_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_td_mst_hst.tx_code);
							if( ret ) goto ErrExit;						
							strcpy(g_pub_tx.tx_br_no,g_td_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_td_mst_hst.tx_code);
							g_pub_tx.tx_date=g_td_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_td_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_td_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_td_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_td_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
												
						}
						Td_mst_hst_Clo_Sel();
					}
					break;
			case '3':
					ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, 
					 	"prdt_no='%s'",g_prdt_ac_id.prdt_no);
					strcpy(tmp_title,g_ln_parm.title);

					if(!strcmp(flag,"00"))
					{
						ret=Ln_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );						
						if(ret)
						{
							sprintf(acErrMsg,"Ln_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Ln_mst_hst_Fet_Sel(&g_ln_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Ln_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_ln_mst_hst.tx_code);
							if( ret ) goto ErrExit;						
							strcpy(g_pub_tx.tx_br_no,g_ln_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_ln_mst_hst.tx_code);
							g_pub_tx.tx_date=g_ln_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_ln_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_ln_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_ln_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_ln_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
												
						}
						Ln_mst_hst_Clo_Sel();
					}
					else if(!strcmp(flag,"10"))
					{
						ret=Ln_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn, 
							tmp_date1);						
						if(ret)
						{
							sprintf(acErrMsg,"Ln_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Ln_mst_hst_Fet_Sel(&g_ln_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Ln_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_ln_mst_hst.tx_code);
							if( ret ) goto ErrExit;					
							strcpy(g_pub_tx.tx_br_no,g_ln_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_ln_mst_hst.tx_code);
							g_pub_tx.tx_date=g_ln_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_ln_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_ln_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_ln_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_ln_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
												
						}
						Ln_mst_hst_Clo_Sel();
					}
					else if(!strcmp(flag,"01"))
					{
						ret=Ln_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn, 
							tmp_date2);						
						if(ret)
						{
							sprintf(acErrMsg,"Ln_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Ln_mst_hst_Fet_Sel(&g_ln_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Ln_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_ln_mst_hst.tx_code);
							if( ret ) goto ErrExit;						
							strcpy(g_pub_tx.tx_br_no,g_ln_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_ln_mst_hst.tx_code);
							g_pub_tx.tx_date=g_ln_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_ln_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_ln_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_ln_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_ln_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
												
						}
						Ln_mst_hst_Clo_Sel();
					}
					else if(!strcmp(flag,"11"))
					{
						ret=Ln_mst_hst_Dec_Sel(g_pub_tx.reply, 
							tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn, 
							tmp_date1,tmp_date2);						
						if(ret)
						{
							sprintf(acErrMsg,"Ln_mst_hst_Dec_Selʧ��!!");
							WRITEMSG
							goto ErrExit;
						}
						while(1)
						{
							ret=Ln_mst_hst_Fet_Sel(&g_ln_mst_hst , g_pub_tx.reply);
							if(ret&&ret!=100)
							{
								sprintf(acErrMsg,"Ln_mst_hst_Fet_Sel�쳣����!!");
								WRITEMSG
								goto ErrExit;
							}else if(ret==100){
								sprintf(acErrMsg,"����ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");								
								break;
							}
							ret=Tx_def_Sel(g_pub_tx.reply,&tx_def_tmp,"tx_code='%s'",g_ln_mst_hst.tx_code);
							if( ret ) goto ErrExit;						
							strcpy(g_pub_tx.tx_br_no,g_ln_mst_hst.tx_br_no);
							strcpy(g_pub_tx.tx_code,g_ln_mst_hst.tx_code);
							g_pub_tx.tx_date=g_ln_mst_hst.tx_date;
							g_pub_tx.tx_amt1=g_ln_mst_hst.tx_amt;
							strcpy(g_pub_tx.ct_ind,g_ln_mst_hst.ct_ind);
							strcpy(g_pub_tx.add_ind,g_ln_mst_hst.add_ind);
						
							pub_base_dic_show_str(f_ct_ind,"ct_ind",g_pub_tx.ct_ind);
							pub_base_dic_show_str(f_dc_ind,"add_ind",g_pub_tx.add_ind);					
							fprintf(fp,"%ld|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|\n", 
								g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.tx_date, 
								g_pub_tx.tx_br_no,g_ln_parm.title,g_pub_tx.tx_code, 
								tx_def_tmp.tx_name,g_pub_tx.ac_no,f_dc_ind,g_pub_tx.tx_amt1,f_ct_ind);						
												
						}
						Ln_mst_hst_Clo_Sel();
					}

					strcpy(g_pub_tx.tx_br_no,g_ln_mst_hst.tx_br_no);
					strcpy(g_pub_tx.tx_code,g_ln_mst_hst.tx_code);
					
					
					vtcp_log("rowid==44444444444444444444444444444444[%20s]\n",g_pub_tx.tx_code);
					
					
					g_pub_tx.tx_date=g_ln_mst_hst.tx_date;
					g_pub_tx.tx_amt1=g_ln_mst_hst.tx_amt;
					strcpy(g_pub_tx.ct_ind,g_ln_mst_hst.ct_ind);
					strcpy(g_pub_tx.add_ind,g_ln_mst_hst.add_ind);
					break;
			default:
					break;
		}/*switch����*/
		
		vtcp_log("[%s][%d]I am herer5555555555555555555555555!\n",__FILE__,__LINE__);			
vtcp_log("[%s][%d]I am herer5555555555555555555555555!\n",__FILE__,__LINE__);	


	}/*while����*/
vtcp_log("[%s][%d]I am herer7777777777777777777777777777777777!\n",__FILE__,__LINE__);	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

	Prdt_ac_id_Clo_Sel();

	/*** hao *** ��ѯ���ײ��ü���ˮ *****
 	if( pub_ins_trace_log() )
 	{
 		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
  		WRITEMSG
    	goto ErrExit;
 	}
	************************************/
vtcp_log("[%s][%d]I am herer8888888888888888888888888888888888888888!\n",__FILE__,__LINE__);	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ���ʷ���׼�¼��ѯ�ɹ�![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ͻ���ʷ���׼�¼��ѯʧ��![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
