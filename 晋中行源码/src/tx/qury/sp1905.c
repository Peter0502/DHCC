/***************************************************************
* �� �� ��:     sp1905.c
* ����������	�ͻ���ʹ�õ����з���
* ��    ��:     peter
* ������ڣ�    2003��3��19��
*
* �޸ļ�¼��
*    ��    ��:	2004��06��08��
*    �� �� ��:	peter
*    �޸�����:	��ӽ������ͺͽ��ʺ���
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "mo_opn_cls_c.h"
#include "cif_basic_inf_c.h"
#include "mdm_attr_c.h"

sp1905()
{
	char filename[100];
	char prdt_name[25];  
	char tmpname[512],tmpstr[150];
	char flag[3],tmp_ind[9];
	int i=0;
	int ret=0;
	FILE *fp;
	int cnt=0;
	int aaa=0;
	char freenull[20];

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct cif_id_code_rel_c	g_cif_id_code_rel;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct mo_opn_cls_c	g_mo_opn_cls;
	struct cif_basic_inf_c	g_cif_basic_inf;
	struct mdm_attr_c	mdm_attr_tmp;
	
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_mo_opn_cls,0x00,sizeof(struct mo_opn_cls_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset( &g_cif_basic_inf,0,sizeof(struct cif_basic_inf_c));
	memset( &mdm_attr_tmp,0,sizeof(struct mdm_attr_c));
	
	g_reply_int=0;
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	
	strcpy(freenull,"");
	
	get_zd_long("0280",&g_pub_tx.cif_no);				/*�ͻ���*/	
	get_zd_data("0620",g_pub_tx.id_no);					/*֤������*/
	get_zd_data("0670",flag);							/*֤������*/
	vtcp_log("g_pub_tx.cif_no=[%ld]",g_pub_tx.cif_no);
	vtcp_log("g_pub_tx.id_no=[%s]",g_pub_tx.id_no);
	vtcp_log("flag=[%s]",flag);
	if(g_pub_tx.cif_no==0&&strlen(flag)!=0)				/*����֤�����Ͳ���*/
	{
		if(strlen(g_pub_tx.id_no)==0)
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
	 		sprintf(acErrMsg,"�ͻ�֤�����ձ����޴˼�¼!!");
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"C007");
	 		goto ErrExit;
		}
		else if(g_reply_int)
	 	{
	 		sprintf(acErrMsg,"��ѯ�ͻ�֤�����ձ��쳣!!g_reply_int=[%ld]", \
					g_reply_int);
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"D101");
	 		goto ErrExit;
		}

		g_pub_tx.cif_no=g_cif_id_code_rel.cif_no;		/*�ͻ�ID*/
	}
	else if(g_pub_tx.cif_no==0&&strlen(flag)==0)
	{
	 	sprintf(acErrMsg,"�ͻ��ź�֤�����Ͳ���ͬʱΪ��!!g_reply_int=[%ld]", \
				g_reply_int);
	 	WRITEMSG
	 	strcpy(g_pub_tx.reply,"C076");
	 	goto ErrExit;
	}

	/*���ÿͻ����Ƿ����*/
	g_reply_int=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf, \
								  "cif_no=%ld", g_pub_tx.cif_no);
	if (g_reply_int&&g_reply_int!=100)
	{
 		sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]�쳣!!");
 		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
 		goto ErrExit;
	}else if( g_reply_int==100 ){
 		sprintf(acErrMsg,"�ͻ��Ų�����,����!!");
 		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
 		goto ErrExit;
 	}		

	g_reply_int=Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "cif_no=%ld", \
								   g_pub_tx.cif_no);
	if (g_reply_int)
	{
 		sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]�쳣!!");
 		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
 		goto ErrExit;
	}

	strcpy(tmpstr,"ac_id=%ld and ac_seqn=%d");

	/*��ӡ��ʼ*/
	pub_base_GenDwnFilName(filename);
	pub_base_strpack(filename);
	sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
	fp=fopen(tmpname,"w");

	/**��ʾ�б���**/
 	fprintf(fp,"~�ͻ���|@�ͻ�����|@��Ʒ|@��Ʒ����|@ƾ֤����|@ƾ֤����|@��//����|@״̬|����ʱ��|@����|@��Ա|\n");

	while(1)
	{
		g_reply_int=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if (g_reply_int==100&&ret==0)
		{
			sprintf(acErrMsg,"�ÿͻ�û��ʹ�õ����з���!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C078");
			goto ErrExit;;
		}else if( g_reply_int==100&&ret!=0){
			break;
		}else if(g_reply_int){
			sprintf(acErrMsg,"FETCT ��Ʒ�ʺŶ��ձ��쳣!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}

		strcpy(g_pub_tx.prdt_code,g_prdt_ac_id.prdt_no);	/*��Ʒ���*/

			TRACE
			/* �����Լ���ֻ�ܲ��Լ��� modify by wudawei 20140711*/
			g_reply_int=Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and (ac_seqn=%d or ac_seqn=9999) and opn_br_no='%s'",g_prdt_ac_id.ac_id, g_prdt_ac_id.ac_seqn, g_pub_tx.tx_br_no);
			if(g_reply_int)
			{
				sprintf(acErrMsg,"Mdm_ac_rel_Dec_Selʧ��!!");
				WRITEMSG
				goto ErrExit;
			}
			while(1)
			{
				TRACE
				g_reply_int=Mdm_ac_rel_Fet_Sel(&g_mdm_ac_rel,g_pub_tx.reply);
				if(g_reply_int&&g_reply_int!=100)
				{
					sprintf(acErrMsg,"Mdm_ac_rel_Fet_Sel�쳣����!!");
					WRITEMSG
					goto ErrExit;
				}else if(g_reply_int==100){
					sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"N062");	
					break;
				}			
				TRACE
				strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);			/*�ͻ��ʺ�*/

				g_reply_int=Mo_opn_cls_Sel(g_pub_tx.reply,&g_mo_opn_cls, \
						"ac_id=%ld and ac_seqn=%d", \
						g_mdm_ac_rel.ac_id,g_prdt_ac_id.ac_seqn );
				if(g_reply_int==100)
				{
					sprintf(acErrMsg,"�������ǼǱ��в����ڸü�¼!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P230");
					break;
				}
				TRACE
				/* ����ac_id�ڽ����˺Ŷ��ձ��в���������ͺͽ��ʺ��� */	
				/* ��mdm_attr���в�ѯ�������� */
				g_reply_int = Mdm_attr_Sel(g_pub_tx.reply,&mdm_attr_tmp,"mdm_code='%s'",g_mdm_ac_rel.mdm_code);
				if (g_reply_int)	goto ErrExit;
				TRACE	
				g_pub_tx.tx_date=g_mo_opn_cls.tx_date;				/*��������*/
				strcpy(tmp_ind,g_mo_opn_cls.wrk_sts);				/*�˻�״̬*/
				pub_base_dic_show_str(tmp_ind,"opn_cls_sts",g_mo_opn_cls.wrk_sts);
				strcpy(g_pub_tx.name,g_mo_opn_cls.name);			/*����*/
				strcpy(prdt_name,g_dd_parm.title);					/*��Ʒ����*/
				strcpy(g_pub_tx.tx_br_no,g_mo_opn_cls.opn_br_no);	/*������*/
				strcpy(g_pub_tx.tel,g_mo_opn_cls.tel);				/*����Ա*/

				TRACE
				switch(g_prdt_ac_id.ac_id_type[0])
				{
					case '1':
						g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
									"prdt_no='%s'", g_pub_tx.prdt_code);
						if(g_reply_int==100)
						{
							sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
							WRITEMSG
							strcpy(g_pub_tx.reply,"N062");
							break;
						}
						strcpy(prdt_name,g_dd_parm.title);			/*��Ʒ����*/
						strcpy(flag,"OK");							
						break;
					case '2':
						g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
									"prdt_no='%s'",g_pub_tx.prdt_code);
						if(g_reply_int==100)
						{
							sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
							WRITEMSG
							strcpy(g_pub_tx.reply,"N062");
							break;
						}
						strcpy(prdt_name,g_td_parm.title);			/*��Ʒ����*/
						strcpy(flag,"OK");							
						break;
					case '3':
						g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
										"prdt_no='%s'",g_pub_tx.prdt_code);
						if(g_reply_int==100)
						{
							sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
							WRITEMSG
							strcpy(g_pub_tx.reply,"N062");
							break;
						}
						strcpy(prdt_name,g_ln_parm.title);			/*��Ʒ����*/
						strcpy(mdm_attr_tmp.mdm_name,"�����˻���ƾ֤");
						strcpy(flag,"OK");							
						break;
					default:
						break;
				}/*switch����*/

				if(!strcmp(flag,"OK"))
				{
 					if(!cnt)
 					{
 						fprintf(fp,"%ld|%s|%s|%s|%s|%s|%s|%s|%ld|%s|%s|\n", \
							g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.prdt_code, \
							prdt_name,mdm_attr_tmp.mdm_name,g_mdm_ac_rel.note_no,g_pub_tx.ac_no, \
							tmp_ind,g_pub_tx.tx_date,g_pub_tx.tx_br_no,g_pub_tx.tel);
					}else{
 						fprintf(fp,"%ld|%s|%s|%s|%s|%s|%s|%s|%ld|%s|%s|\n", \
							aaa,freenull,freenull, \
							freenull,mdm_attr_tmp.mdm_name,g_mdm_ac_rel.note_no,g_pub_tx.ac_no, \
							tmp_ind,g_pub_tx.tx_date,g_pub_tx.tx_br_no,g_pub_tx.tel);
					}					
					ret++;
					cnt++;
				}
				TRACE
			}			
			Mdm_ac_rel_Clo_Sel();
			cnt=0;						
		
	}/*while����*/

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

	Prdt_ac_id_Clo_Sel();

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ���ʹ�õ����з�����Ϣ��ѯ�ɹ�![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ͻ���ʹ�õ����з�����Ϣ��ѯʧ��![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
