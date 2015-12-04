/***************************************************************
* �� �� ��: sp1907.c
* ����������VTM��ѯ�ͻ����п�
* ��    ��: �����
* ������ڣ�2014��7��24��
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
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"

sp1907()
{
	char filename[100];
	char tmpname[512],tmpstr[150];
	char flag[3],tmp_ind[21];
	char sNoteSts[31]={0};
	long open_date = 0; /**add zgf 20131031 ���ӿ�������**/
	int ret=0;
	FILE *fp;

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct cif_id_code_rel_c	g_cif_id_code_rel;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct cif_basic_inf_c	g_cif_basic_inf;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));

	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_long("0280",&g_pub_tx.cif_no);				/*�ͻ���*/
	get_zd_data("0670",flag);							/*֤������*/
	get_zd_data("0620",g_pub_tx.id_no);					/*֤������*/

	if(g_pub_tx.cif_no==0&&strlen(flag)!=0)				/*����֤�����Ͳ���*/
	{
 		if (strlen(g_pub_tx.id_no)==0)
 		{
 			sprintf(acErrMsg,"������ͻ�֤������!!");
 			WRITEMSG
 			strcpy(g_pub_tx.reply,"C079");
 			goto ErrExit;
		}

		g_reply_int=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
					"id_type='%s' and id_no='%s'",flag,g_pub_tx.id_no); 
	 	if (g_reply_int==100)
	 	{
	 		sprintf(acErrMsg,"�ͻ�������Ϣ�����޴˼�¼!!");
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"C007");
	 		goto ErrExit;
		}
		else if(g_reply_int)
	 	{
	 		sprintf(acErrMsg,"��ѯ�ͻ�������Ϣ���쳣!!g_reply_int=[%ld]", \
					g_reply_int);
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"D101");
	 		goto ErrExit;
		}

		g_pub_tx.cif_no=g_cif_id_code_rel.cif_no;
	}
	else if(g_pub_tx.cif_no==0&&strlen(flag)==0)
	{
		sprintf(acErrMsg,"�ͻ��ź�֤��������ͬʱΪ��");
	 	WRITEMSG
	 	strcpy(g_pub_tx.reply,"C076");
	 	goto ErrExit;
	}

	/*���ͻ����Ƿ����*/
	g_reply_int=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf, \
								  "cif_no=%ld", g_pub_tx.cif_no); 
 	if (g_reply_int==100)
 	{
 		sprintf(acErrMsg,"�ͻ�������Ϣ����δ�ҵ��˿ͻ���!!");
 		WRITEMSG
 		strcpy(g_pub_tx.reply,"C007");
 		goto ErrExit;
	}

	strcpy(tmpstr,"ac_id=%ld and ac_seqn=%d");

	g_reply_int=Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "cif_no=%ld", \
								   g_pub_tx.cif_no);
	if (g_reply_int)
	{
 		sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]�쳣!!");
 		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
 		goto ErrExit;
	}

	/*��ӡ��ʼ*/
	pub_base_GenDwnFilName(filename);
	pub_base_strpack(filename);
	sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
	fp=fopen(tmpname,"w");

	/**��ʾ�б���**/
 	fprintf(fp,"~�ͻ�ID|@�ͻ�����|@��//����|$�˻����|@�˻�״̬|@��������|����״̬|\n");/**add zgf 20131031 ���ӿ�������**/

					vtcp_log("XXXXXXXXXXXXXX");
	while(1)
	{
		g_reply_int=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if (g_reply_int==100&&ret==0)
		{
			sprintf(acErrMsg,"�ÿͻ�û����ص�������Ϣ!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C077");
			goto ErrExit;
		}
		else if(g_reply_int==100)break;
		else if(g_reply_int)
		{
			sprintf(acErrMsg,"FETCT ��Ʒ�ʺŶ��ձ��쳣!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}
		/** ������ ������ ����� ֻ�ܲ鵽�ɽ��� �鲻���µ� ���˺� ֱ�Ӳ��µ� modify by wudawei 20130923 
		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_id=%ld",g_prdt_ac_id.ac_id);
		modify by wudawei 20130923 **/
		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_id=%ld and note_sts not in ('2','3','4')",g_prdt_ac_id.ac_id);
		if(g_reply_int==100)
		{
			sprintf(acErrMsg,"�޴��ʺű�����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"N062");
			/*wudawei 20140212 
			break;
			*/
			continue;
		}

		strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);			/*�ͻ��˺�*/

		switch(g_prdt_ac_id.ac_id_type[0])
		{
			case '1':
					g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
						tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
					if(g_reply_int==100)
					{
						sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
						break;
					}
	
					g_pub_tx.tx_amt1=g_dd_mst.bal;
					pub_base_dic_show_str(tmp_ind,"dd_ac_sts",g_dd_mst.ac_sts);
					strcpy(g_pub_tx.name,g_dd_mst.name);
					open_date = g_dd_mst.opn_date;/**add zgf 20131031 ���ӿ�������**/
					strcpy(flag,"OK");
					break;
			case '2':
					g_reply_int=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
						tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
					if(g_reply_int==100)
					{
						sprintf(acErrMsg,"�����ļ������ڸü�¼!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
						break;
					}
					g_pub_tx.tx_amt1=g_td_mst.bal;
					pub_base_dic_show_str(tmp_ind,"td_ac_sts",g_td_mst.ac_sts);
					strcpy(g_pub_tx.name,g_td_mst.name);
					open_date = g_td_mst.opn_date;/**add zgf 20131031 ���ӿ�������**/
					strcpy(flag,"OK");
					break;
			case '3':
					g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, \
						tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
					if(g_reply_int==100)
					{
						sprintf(acErrMsg,"�����ļ������ڸü�¼!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
						break;
					}
					g_pub_tx.tx_amt1=g_ln_mst.bal;
					pub_base_dic_show_str(tmp_ind,"ln_ac_sts",g_ln_mst.ac_sts);
					strcpy(g_pub_tx.name,g_ln_mst.name);
					open_date = g_ln_mst.opn_date;/**add zgf 20131031 ���ӿ�������**/
					strcpy(flag,"OK");
					break;
			default:
					break;
		}/*switch����*/

		if(!strcmp(flag,"OK"))
		{/**add zgf 20131031 ���ӿ�������**/
                        memset(sNoteSts,0,sizeof(sNoteSts));
                        pub_base_dic_show_str(sNoteSts,"mdm_sts",g_mdm_ac_rel.note_sts);
 			fprintf(fp,"%ld|%s|%s|%.2lf|%s|%ld|%s|\n", \
					g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.ac_no, \
					g_pub_tx.tx_amt1,tmp_ind,open_date,sNoteSts);
			ret++;
		}
	}/*while����*/

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

	Prdt_ac_id_Clo_Sel();

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ��˻���ϸ��Ϣ��ѯ�ɹ�![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ͻ��˻���ϸ��Ϣ��ѯʧ��![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
