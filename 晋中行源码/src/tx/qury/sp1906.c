/***************************************************************
* �� �� ��: sp1906.c
* ���������������˻���ϸ��Ϣ��ѯ
* ��    ��: ���Ϊ
* ������ڣ�2014��6��14��
*
* �޸ļ�¼��
*	��    ��:
*	�� �� ��:
*	�޸�����:
*
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('1906', '��Ա�����ϵ��ѯ', '00000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');

insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('1906', 0, '1906', '#$');

insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('1906', '��Ա�����ϵ��ѯ', 'sp1906', '0', '0');
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "tel_save_rel_c.h"

sp1906()
{
	int count = 0;
	char filename[100];
	char tmpname[512],tmpstr[150];
	char flag[3],tmp_ind[21];
	char sNoteSts[31]={0};
	long open_date = 0; /**add zgf 20131031 ���ӿ�������**/
	int ret=0;
	FILE *fp;
	char ac_no[20];
	char tel[5];
	long cif_no=0;
	char q_type[2];
	
	char ac_name_tmp[61];
	char note_sts_tmp[10];
	char ac_sts_tmp[16];
	char prdt_name_tmp[30];
	long cif_no_tmp = 0;
	double bal_tmp = 0.00;
	char rel_type_tmp[12];
	char br_no[6];
	int num = 0;
	
	struct prdt_ac_id_c sPrdt_ac_id;
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct cif_id_code_rel_c	g_cif_id_code_rel;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct cif_basic_inf_c	g_cif_basic_inf;
	struct tel_save_rel_c	g_tel_save_rel;


	memset(br_no,0x00,sizeof(br_no));
	memset(q_type,0x00,sizeof(q_type));
	memset(ac_no,0x00,sizeof(ac_no));
	memset(tel,0x00,sizeof(tel));
	memset(ac_name_tmp,0x00,sizeof(ac_name_tmp));
	memset(note_sts_tmp,0x00,sizeof(note_sts_tmp));
	memset(ac_sts_tmp,0x00,sizeof(ac_sts_tmp));
	memset(rel_type_tmp,0x00,sizeof(rel_type_tmp));	
	
	memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
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

	get_zd_data( "0370", ac_no );		/* �����˺� */
	get_zd_data( "0920", tel );		/* ��Ա���� */
	get_zd_long( "0290", &cif_no ); 
	get_zd_data( "0690", q_type );

	if(q_type[0] == '2')
	{
			ret = Tel_save_rel_Dec_Sel( g_pub_tx.reply , "ac_no='%s' and br_no='%s' and sts='1'" , ac_no ,g_pub_tx.tx_br_no );
			if (ret)
			{
		 		sprintf(acErrMsg,"[DECLARE SELECT Tel_save_rel_Dec_Sel]�쳣!!");
		 		WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
		 		goto ErrExit;
			}
	}else if(q_type[0] == '3')
	{
			ret = Tel_save_rel_Dec_Sel( g_pub_tx.reply ,  "cif_no='%ld' and br_no='%s' and sts='1'" , cif_no ,g_pub_tx.tx_br_no  );
			if (ret)
			{
		 		sprintf(acErrMsg,"[DECLARE SELECT Tel_save_rel_Dec_Sel]�쳣!!");
		 		WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
		 		goto ErrExit;
			}
	}else if(q_type[0] == '1')
	{
			ret = Tel_save_rel_Dec_Sel( g_pub_tx.reply , "tel='%s'  and br_no='%s' and sts='1'" , tel ,g_pub_tx.tx_br_no );
			if (ret)
			{
		 		sprintf(acErrMsg,"[DECLARE SELECT Tel_save_rel_Dec_Sel]�쳣!!");
		 		WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
		 		goto ErrExit;
			}
	}

	/*��ӡ��ʼ*/
	pub_base_GenDwnFilName(filename);
	pub_base_strpack(filename);
	sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
	fp=fopen(tmpname,"w");

	/**��ʾ�б���**/
 	fprintf(fp,"~�����Ա|@�����˻�|@����|@�ͻ���|$�˻����|@��Ʒ|@���濪ʼ����|@��������|�ʻ�״̬|����״̬|\n");
 	
	while(1)
	{
		
			memset(&g_tel_save_rel,0x00,sizeof(struct tel_save_rel_c));
			ret = Tel_save_rel_Fet_Sel(&g_tel_save_rel , g_pub_tx.reply  );
			if( ret==100 && count==0)
			{
				sprintf(acErrMsg,"û���ҵ�ƥ��ļ�¼!!");
		 		WRITEMSG
				strcpy(g_pub_tx.reply,"O043");
		 		goto ErrExit;							
			}
			else if(ret==100){
				break;
			}else if(ret){
				sprintf(acErrMsg,"FETCT Tel_save_rel_Fet_Sel�쳣!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D102");
				goto ErrExit;
			}
			/* ��������Լ��������Ȩ����ѯ */
			if(strcmp(g_tel_save_rel.tel ,g_pub_tx.tel ) != 0)
			{
				sprintf(acErrMsg,"�����Աg_tel_save_rel.tel=[%s],��ѯ��Ա��g_pub_tx.tel=[%s],����ͬһ��Ա���������ѯ!",g_tel_save_rel.tel ,g_pub_tx.tel );
				WRITEMSG
				strcpy(g_pub_tx.reply,"Z10B");
				goto ErrExit;
			}
			/* 20140620 wudawei end */
			if(g_tel_save_rel.tx_type[0] == '1')
			{
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_no='%s' ",g_tel_save_rel.ac_no);
				if(ret==100)
				{
					sprintf(acErrMsg,"�޴��ʺ�����!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"N062");
					continue;
				}
				ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ", g_mdm_ac_rel.ac_id);
        if (ret != 0 && ret != 100)
        {
                sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ����!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
        }
        else if (ret == 100)
        {
                ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
                if ( ret != 0)
                {
                        sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",
                                g_pub_tx.cur_no);
                        WRITEMSG
                        goto ErrExit;
                }
        }
        else
        {
                g_pub_tx.ac_seqn = sPrdt_ac_id.ac_seqn;
        }

				/*������ʾ�ʺź��˺���ŷ����˻����ͺ����ļ�*/
				ret = pub_base_disac_file( g_mdm_ac_rel.ac_no, sPrdt_ac_id.ac_seqn, g_pub_tx.ac_type); 
				if (ret != 0) 
				{ 
					sprintf(acErrMsg,"��ѯ�˺Ŵ���!! "); 
					WRITEMSG 
					strcpy(g_pub_tx.reply,"D169"); 
					goto ErrExit; 
				}
				
				memset(tel,0x00,sizeof(tel));
				memset(ac_name_tmp,0x00,sizeof(ac_name_tmp));
				memset(note_sts_tmp,0x00,sizeof(note_sts_tmp));
				memset(ac_sts_tmp,0x00,sizeof(ac_sts_tmp));
				memset(rel_type_tmp,0x00,sizeof(rel_type_tmp));	
				sprintf(acErrMsg,"��ѯ���!! prdt_ac_id.ac_id=[%ld],prdt_ac_id.ac_seqn=[%d],g_pub_tx.ac_type=[%s]",
                                sPrdt_ac_id.ac_id,sPrdt_ac_id.ac_seqn,g_pub_tx.ac_type);
        WRITEMSG
				strcpy(rel_type_tmp, "�����ʺ�");
				/* note_sts	char(1)	����״̬	mdm_sts	0��������1����ʧ��2����ʧ��֤��3��������֤��4���Ѿ����᣻5-������*������ */
				if(g_mdm_ac_rel.note_sts[0] == '0')
				{
					strcpy(note_sts_tmp, "����");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '1')
				{
					strcpy(note_sts_tmp, "��ʧ");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '2')
				{
					strcpy(note_sts_tmp, "��ʧ��֤");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '3')
				{
					strcpy(note_sts_tmp, "������֤");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '4')
				{
					strcpy(note_sts_tmp, "�Ѿ�����");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '5')
				{
					strcpy(note_sts_tmp, "����");
				}
				else if(g_mdm_ac_rel.note_sts[0] == '*')
				{
					strcpy(note_sts_tmp, "����");
				}
				/* ����״̬ */
				if (atoi(g_pub_tx.ac_type) == 1) 
				{ 
					strcpy(ac_name_tmp ,g_dd_mst.name); 
					strcpy(br_no, g_dd_mst.opn_br_no);
					ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
					if(ret==100)
					{
						sprintf(acErrMsg,"���ڲ�Ʒ������\n");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
			
					}
					strcpy(prdt_name_tmp, g_dd_parm.title);
					cif_no_tmp = g_dd_mst.cif_no;
					bal_tmp = g_dd_mst.bal;
					if(g_dd_mst.ac_sts[0] == '1')
					{
						strcpy(ac_sts_tmp,"����");
					}
					else if(g_dd_mst.ac_sts[0] == '*')
					{
						strcpy(ac_sts_tmp,"����");
					}
				} 
				else if (atoi(g_pub_tx.ac_type) == 2) 
				{ 
					strcpy(ac_name_tmp ,g_td_mst.name); 
					strcpy(br_no, g_td_mst.opn_br_no);
					ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, "prdt_no='%s'", g_td_mst.prdt_no);
					if(ret==100)
					{
						sprintf(acErrMsg,"���ڲ�Ʒ������\n");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
			
					}
					strcpy(prdt_name_tmp, g_td_parm.title);
					cif_no_tmp = g_td_mst.cif_no;
					bal_tmp = g_td_mst.bal;
					if(g_td_mst.ac_sts[0] == '1')
					{
						strcpy(ac_sts_tmp,"����");
					}
					else if(g_td_mst.ac_sts[0] == '*')
					{
						strcpy(ac_sts_tmp,"����");
					}
				} 
				else if (atoi(g_pub_tx.ac_type) == 3) 
				{ 
					strcpy(ac_name_tmp ,g_ln_mst.name); 
					strcpy(br_no, g_ln_mst.opn_br_no);
					ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, "prdt_no='%s'", g_ln_mst.prdt_no);
					if(ret==100)
					{
						sprintf(acErrMsg,"�����Ʒ������\n");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
			
					}
					strcpy(prdt_name_tmp, g_ln_parm.title);
					cif_no_tmp = g_ln_mst.cif_no;
					bal_tmp = g_ln_mst.bal;
					if(g_ln_mst.ac_sts[0] == '1')
					{
						strcpy(ac_sts_tmp,"����");
					}
					else if(g_ln_mst.ac_sts[0] == '*')
					{
						strcpy(ac_sts_tmp,"����");
					}
					else if(g_ln_mst.ac_sts[0] == '2')
					{
						strcpy(ac_sts_tmp,"����90����");
					}
					else if(g_ln_mst.ac_sts[0] == '3')
					{
						strcpy(ac_sts_tmp,"����");
					}
					else if(g_ln_mst.ac_sts[0] == '4')
					{
						strcpy(ac_sts_tmp,"����");
					}
					else if(g_ln_mst.ac_sts[0] == '5')
					{
						strcpy(ac_sts_tmp,"����90����");
					}
				} 
				/* fprintf(fp,"~�����Ա|@�����˻�|@����|@�ͻ���|$�˻����|@��Ʒ|@���濪ʼ����|@��������|�ʻ�״̬|����״̬|\n"); */
				fprintf( fp, "%4s|%19s|%60s|%ld|%.2lf|%s|%ld|%s|%s|%s|\n", g_tel_save_rel.tel,g_tel_save_rel.ac_no,ac_name_tmp,cif_no_tmp,bal_tmp,prdt_name_tmp,g_tel_save_rel.tx_date,rel_type_tmp,ac_sts_tmp,note_sts_tmp );
			}
			/* ��ѯ�����ͻ��ŵ� */
			else if(g_tel_save_rel.tx_type[0] == '2')
			{
					ret=Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "cif_no=%ld", \
								   g_tel_save_rel.cif_no);
					if (ret)
					{
					 		sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]�쳣!!");
					 		WRITEMSG
							strcpy(g_pub_tx.reply,"D101");
					 		goto ErrExit;
					}
					while(1)
					{
							ret=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
							if(ret==100)break;
							else if(ret)
							{
									sprintf(acErrMsg,"FETCT ��Ʒ�ʺŶ��ձ��쳣!!");
									WRITEMSG
									strcpy(g_pub_tx.reply,"D102");
									goto ErrExit;
							}
							
							memset(tel,0x00,sizeof(tel));
							memset(ac_name_tmp,0x00,sizeof(ac_name_tmp));
							memset(note_sts_tmp,0x00,sizeof(note_sts_tmp));
							memset(ac_sts_tmp,0x00,sizeof(ac_sts_tmp));
							memset(rel_type_tmp,0x00,sizeof(rel_type_tmp));	
							
							ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_id=%ld and opn_br_no='%s' and note_sts not in ('2','3','4')",g_prdt_ac_id.ac_id,g_pub_tx.tx_br_no);
							if(ret==100)
							{
								sprintf(acErrMsg,"�޴��ʺ�����!!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"N062");
								continue;
							}
							num = sql_count( "tel_save_rel" , " ac_no='%s' and sts='1' " , g_mdm_ac_rel.ac_no );
							sprintf(acErrMsg, "�����ͻ��ŵ� ����˻��ڹ��������� ������ ******* tel_save_rel_num = [%d]", num);
							WRITEMSG
							if(num > 0)
							{
								sprintf(acErrMsg,"���ʺ��Ѿ���������������!!");
								WRITEMSG
								continue;
							}
							strcpy(rel_type_tmp, "�����ͻ���");
							/* note_sts	char(1)	����״̬	mdm_sts	0��������1����ʧ��2����ʧ��֤��3��������֤��4���Ѿ����᣻5-������*������ */
							if(g_mdm_ac_rel.note_sts[0] == '0')
							{
								strcpy(note_sts_tmp, "����");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '1')
							{
								strcpy(note_sts_tmp, "��ʧ");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '2')
							{
								strcpy(note_sts_tmp, "��ʧ��֤");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '3')
							{
								strcpy(note_sts_tmp, "������֤");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '4')
							{
								strcpy(note_sts_tmp, "�Ѿ�����");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '5')
							{
								strcpy(note_sts_tmp, "����");
							}
							else if(g_mdm_ac_rel.note_sts[0] == '*')
							{
								strcpy(note_sts_tmp, "����");
							}
							/* ����״̬ */
							switch(g_prdt_ac_id.ac_id_type[0])
							{
								case '1':
										ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
											"ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
										if(ret==100)
										{
											sprintf(acErrMsg,"������ϸ�ļ������ڸü�¼!!");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
											break;
										}
						
										strcpy(ac_name_tmp ,g_dd_mst.name); 
										strcpy(br_no, g_dd_mst.opn_br_no);
										ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
										if(ret==100)
										{
											sprintf(acErrMsg,"���ڲ�Ʒ������\n");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
								
										}
										strcpy(prdt_name_tmp, g_dd_parm.title);
										cif_no_tmp = g_dd_mst.cif_no;
										bal_tmp = g_dd_mst.bal;
										if(g_dd_mst.ac_sts[0] == '1')
										{
											strcpy(ac_sts_tmp,"����");
										}
										else if(g_dd_mst.ac_sts[0] == '*')
										{
											strcpy(ac_sts_tmp,"����");
										}
										break;
								case '2':
										ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
											"ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
										if(ret==100)
										{
											sprintf(acErrMsg,"�����ļ������ڸü�¼!!");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
											break;
										}
										strcpy(ac_name_tmp ,g_td_mst.name); 
										strcpy(br_no, g_td_mst.opn_br_no);
										ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, "prdt_no='%s'", g_td_mst.prdt_no);
										if(ret==100)
										{
											sprintf(acErrMsg,"���ڲ�Ʒ������\n");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
								
										}
										strcpy(prdt_name_tmp, g_td_parm.title);
										cif_no_tmp = g_td_mst.cif_no;
										bal_tmp = g_td_mst.bal;
										if(g_td_mst.ac_sts[0] == '1')
										{
											strcpy(ac_sts_tmp,"����");
										}
										else if(g_td_mst.ac_sts[0] == '*')
										{
											strcpy(ac_sts_tmp,"����");
										}
										break;
								case '3':
										ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, \
											"ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
										if(ret==100)
										{
											sprintf(acErrMsg,"�����ļ������ڸü�¼!!");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
											break;
										}
										strcpy(ac_name_tmp ,g_ln_mst.name); 
										strcpy(br_no, g_ln_mst.opn_br_no);
										ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, "prdt_no='%s'", g_ln_mst.prdt_no);
										if(ret==100)
										{
											sprintf(acErrMsg,"�����Ʒ������\n");
											WRITEMSG
											strcpy(g_pub_tx.reply,"N062");
								
										}
										strcpy(prdt_name_tmp, g_ln_parm.title);
										cif_no_tmp = g_ln_mst.cif_no;
										bal_tmp = g_ln_mst.bal;
										if(g_ln_mst.ac_sts[0] == '1')
										{
											strcpy(ac_sts_tmp,"����");
										}
										else if(g_ln_mst.ac_sts[0] == '*')
										{
											strcpy(ac_sts_tmp,"����");
										}
										else if(g_ln_mst.ac_sts[0] == '2')
										{
											strcpy(ac_sts_tmp,"����90����");
										}
										else if(g_ln_mst.ac_sts[0] == '3')
										{
											strcpy(ac_sts_tmp,"����");
										}
										else if(g_ln_mst.ac_sts[0] == '4')
										{
											strcpy(ac_sts_tmp,"����");
										}
										else if(g_ln_mst.ac_sts[0] == '5')
										{
											strcpy(ac_sts_tmp,"����90����");
										}
										break;
								default:
										break;
							}/*switch����*/
							fprintf( fp, "%4s|%19s|%60s|%ld|%.2lf|%s|%ld|%s|%s|%s|\n", g_tel_save_rel.tel,g_mdm_ac_rel.ac_no ,ac_name_tmp,cif_no_tmp,bal_tmp,prdt_name_tmp,g_tel_save_rel.tx_date,rel_type_tmp,ac_sts_tmp,note_sts_tmp );
					}
					Prdt_ac_id_Clo_Sel ();
			}
			count++;
	}

	
	fclose(fp);
	Tel_save_rel_Clo_Sel ();
	
	set_zd_data(DC_FILE_SND,"1");

	

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ѯ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ѯʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	fclose(fp);
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
