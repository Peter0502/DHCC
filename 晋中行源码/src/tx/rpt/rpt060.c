/***********************************************************
* �� �� ��: rpt060.c
* ������������Ա���汨��
*
* ��    ��: ���Ϊ
* �������: 2014��6��16��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
***********************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "find_debug.h"
#include "com_branch_c.h"
#include "tel_save_rel_c.h"
#include "com_rate_c.h"
struct tel_save_rel_c g_tel_save_rel;
struct com_branch_c sComBranch;

int rpt060()
{
	int ret      = 0;
	int num      = 0;
	int ac_type  = 1;
	int is_print = 0;

	long cif_no_tmp = 0;

	double rate    = 0.00;
	double bal_tmp = 0.00;

	char fname[100];
	char pr_date[12];
	char pre_br_no[6];
	char pr_prdt_no[4];

	FILE *fp;

	struct prdt_ac_id_c sPrdt_ac_id;
	struct com_rate_c   g_com_rate;
	struct prdt_ac_id_c	g_prdt_ac_id;

	memset(&g_com_rate,     0x0,  sizeof(struct com_rate_c));
	memset(&sPrdt_ac_id,    0x0,  sizeof(struct prdt_ac_id_c));
	memset(&g_prdt_ac_id,   0x0,  sizeof(struct prdt_ac_id_c));
  	memset(&g_tel_save_rel, 0x0,  sizeof(struct tel_save_rel_c));
	memset(fname,           0x00, sizeof(fname));
	memset(pr_date,         0x00, sizeof(pr_date));
	memset(pre_br_no,       0x00, sizeof(pre_br_no));
	memset(pr_prdt_no,      0x00, sizeof(pr_prdt_no));

	/*������*/
	g_reply_int = sql_begin();
	if(g_reply_int != 0)
	{
		sprintf(acErrMsg, "BEGIN WORK error!!!");
		WRITEMSG
		return 1;
	}

	/*��ʼ��ȫ�ֱ���*/
	pub_base_sysinit();

	strcpy(fname, "RPT060");
	g_reply_int = pub_rpt_rmfile_all(fname);
	if(g_reply_int != 0)
	{
		sprintf(acErrMsg,"ɾ���ɱ������!!");
		WRITEMSG
		return 1;
	}

	/*g_reply_int = Com_branch_Dec_Sel(g_pub_tx.reply, " br_type not in('0','6') and wrk_sts!='*' ORDER BY br_no"); lwb 20150525  ���ӷ���  */
	g_reply_int = Com_branch_Dec_Sel(g_pub_tx.reply, " br_type not in('0','6','7') and wrk_sts!='*' ORDER BY br_no");
	if(g_reply_int != 0)
	{
		sprintf(acErrMsg, "DECLARE com_branch CUR error![%d]", g_reply_int );
		WRITEMSG
		return 1;
	}

	sprintf(pr_date, "%d/%d/%d", g_pub_tx.tx_date/10000, g_pub_tx.tx_date/100%100, g_pub_tx.tx_date%100);
	while(1)
	{
		g_reply_int = Com_branch_Fet_Sel(&sComBranch, g_pub_tx.reply);
		if(g_reply_int != 0 && g_reply_int != 100)
		{
			sprintf(acErrMsg, "FETCH com_branch CUR error![%d]", g_reply_int);
			WRITEMSG
			return 1;
		}
		else if(g_reply_int == 100)
		{
			fclose(fp);
			break;
		}

		vtcp_log("sComBranch.br_no===[%s]",sComBranch.br_no);
		vtcp_log("sComBranch.br_no===[%s]",sComBranch.br_name);
		vtcp_log("pre_br_no==[%s]",pre_br_no);

		/*�����ű�,��һ���ļ�*/
		if(strcmp(sComBranch.br_no, pre_br_no))
		{
			/*���ǵ�һ������,���ϸ������Ľ�β*/
			if(strlen(pre_br_no))
			{
				fclose(fp);
			}

			sprintf(fname, "%s/report/%s/RPT060.txt", getenv("HOME"), sComBranch.br_no);

			fp = fopen(fname,"w");
			if(fp==NULL)
			{
				sprintf(acErrMsg, " open file [%s] error ", fname);
				WRITEMSG
				strcpy(g_pub_tx.reply, "S047");
				set_zd_data(DC_REPLY, g_pub_tx.reply);
				return 1;
			}

			fprintf(fp, "���� ������ �ͻ��� �˻� ���� ��� �˻����� ��Ʒ\n");
			strcpy(pre_br_no, sComBranch.br_no);
		}

		ret = Tel_save_rel_Dec_Sel(g_pub_tx.reply, " br_no='%s' and sts='1' order by tel, tx_date", sComBranch.br_no);
		if (ret != 0)
		{
			sprintf(acErrMsg,"[DECLARE SELECT Tel_save_rel_Dec_Sel]�쳣!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
		 	goto ErrExit;
		}

		while(1)
		{
			memset(&g_tel_save_rel, 0x00, sizeof(struct tel_save_rel_c));
			ret = Tel_save_rel_Fet_Sel(&g_tel_save_rel , g_pub_tx.reply);
			if(ret==100)
			{
				break;
			}
			else if(ret != 0)
			{
				sprintf(acErrMsg,"FETCT Tel_save_rel_Fet_Sel�쳣!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D102");
				goto ErrExit;
			}

			if(g_tel_save_rel.tx_type[0] == '1')
			{
				is_print = 0;
				memset(&g_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s' ",g_tel_save_rel.ac_no);
				if(ret==100)
				{
					sprintf(acErrMsg,"�޴��ʺ�����!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"N062");
					continue;
				}
				memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
				ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld ", g_mdm_ac_rel.ac_id);
		  		if (ret != 0 && ret != 100)
		  		{
		 			sprintf(acErrMsg, "��ѯ��Ʒ�ʺŶ��ձ����!! [%d]", ret);
		  			WRITEMSG
		  			goto ErrExit;
				}
		  		else if (ret == 100)
		  		{
		  			ret = pub_base_CurToSeqn(g_pub_tx.cur_no, &g_pub_tx.ac_seqn);
			  		if (ret != 0)
					{
						sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]", g_pub_tx.cur_no);
						WRITEMSG
						goto ErrExit;
					}
		 		}
				else
				{
					g_pub_tx.ac_seqn = sPrdt_ac_id.ac_seqn;
				}
				sprintf(acErrMsg, "�ʺ�!! g_mdm_ac_rel.ac_no=[%s],sPrdt_ac_id.ac_id_type=[%c]", g_mdm_ac_rel.ac_no, sPrdt_ac_id.ac_id_type[0]);
				WRITEMSG

				switch(sPrdt_ac_id.ac_id_type[0])
				{
					case '1':
						memset(&g_dd_mst, 0x0, sizeof(struct dd_mst_c));
						ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, "ac_id=%ld and ac_seqn=%d and ac_sts='1' ", sPrdt_ac_id.ac_id, sPrdt_ac_id.ac_seqn);
						if(ret == 100)
						{
							sprintf(acErrMsg, "������ϸ�ļ������ڸü�¼!!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "N062");
							break;
						}
						ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
						if(ret == 100)
						{
							sprintf(acErrMsg, "���ڲ�Ʒ������\n");
							WRITEMSG
							strcpy(g_pub_tx.reply, "N062");

						}
						if(g_dd_mst.ac_type[0] != '6' && strcmp(g_dd_mst.prdt_no, "142") != 0)
						{
							memset(&g_com_rate, '\0', sizeof(g_com_rate));
							if(strcmp(g_dd_mst.prdt_no, "107") == 0)
							{
								ret = Com_rate_Sel(g_pub_tx.reply, &g_com_rate, "cur_no='01' and rate_code='B00' and end_date>=99999999");
							}
							else
							{
								ret = Com_rate_Sel(g_pub_tx.reply, &g_com_rate, "cur_no='01' and rate_code='%s' and end_date>=99999999", g_dd_parm.rate_no);
							}
							 rate = (1 + g_dd_mst.flt_ratio) * g_com_rate.rate_val;
						}
						else
						{
							rate = g_dd_mst.rate;
						}
						cif_no_tmp = g_dd_mst.cif_no;
						bal_tmp = g_dd_mst.bal;
						ac_type = 1;
						sprintf(pr_prdt_no, "%s", g_dd_mst.prdt_no);
						is_print = 1;
						break;

					case '2':
						memset(&g_td_mst, 0x0, sizeof(struct td_mst_c));
						ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst, "ac_id=%ld and ac_seqn=%d and ac_sts='1'", sPrdt_ac_id.ac_id, sPrdt_ac_id.ac_seqn);
						if(ret == 100)
						{
							sprintf(acErrMsg, "�����ļ������ڸü�¼!!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "N062");
							break;
						}
						ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'", g_td_mst.prdt_no);
						if(ret == 100)
						{
							sprintf(acErrMsg, "���ڲ�Ʒ������\n");
							WRITEMSG
							strcpy(g_pub_tx.reply, "N062");

						}
						cif_no_tmp = g_td_mst.cif_no;
						bal_tmp = g_td_mst.bal;
						rate = g_td_mst.rate;
						ac_type = 1;
						sprintf(pr_prdt_no, "%s", g_td_mst.prdt_no);
						is_print = 1;
						break;

					case '3':
						memset(&g_ln_mst, 0x0, sizeof(struct ln_mst_c));
						ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, "ac_id=%ld and ac_seqn=%d and ac_sts='1' ", sPrdt_ac_id.ac_id, sPrdt_ac_id.ac_seqn);
						if(ret == 100)
						{
							sprintf(acErrMsg, "�����ļ������ڸü�¼!!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "N062");
							break;
						}
						ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", g_ln_mst.prdt_no);
						if(ret == 100)
						{
							sprintf(acErrMsg, "�����Ʒ������\n");
							WRITEMSG
							strcpy(g_pub_tx.reply, "N062");

						}
						cif_no_tmp = g_ln_mst.cif_no;
						bal_tmp = g_ln_mst.bal;
						rate = g_ln_mst.rate;
						ac_type = 2;
						sprintf(pr_prdt_no, "%s", g_ln_mst.prdt_no);
						is_print = 1;
						break;
					default:
						break;
				} /*switch����*/

				/* fprintf(fp,"~����|@������|@�ͻ���|@�˻�|$����|@���|@�˻�����|@��Ʒ"); */
				if(is_print == 1)
				{
					fprintf(fp, "%s %4s %ld %s %.5lf %.2lf %d %s\n", pr_date, g_tel_save_rel.tel, cif_no_tmp, g_tel_save_rel.ac_no, rate, bal_tmp, ac_type, pr_prdt_no);
				}
			}

			/* ��ѯ�����ͻ��ŵ� */
			else if(g_tel_save_rel.tx_type[0] == '2')
			{
				ret = Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "cif_no=%ld", g_tel_save_rel.cif_no);
				if (ret != 0)
				{
					sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]�쳣!!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "D101");
					goto ErrExit;
				}
				while(1)
				{
					is_print = 0;
					memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
					ret = Prdt_ac_id_Fet_Sel(&g_prdt_ac_id, g_pub_tx.reply);
					if(ret == 100)
					{
						break;
					}
					else if(ret != 0)
					{
						sprintf(acErrMsg, "FETCT ��Ʒ�ʺŶ��ձ��쳣!!");
						WRITEMSG
						strcpy(g_pub_tx.reply, "D102");
						goto ErrExit;
					}

					memset(&g_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c)) ;
					ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_id=%ld and opn_br_no='%s' and note_sts not in ('2','3','4')", g_prdt_ac_id.ac_id, sComBranch.br_no );
					if(ret == 100)
					{
						sprintf(acErrMsg, "�޴��ʺ�����!!");
						WRITEMSG
						strcpy(g_pub_tx.reply, "N062");
						continue;
					}
					num = sql_count("tel_save_rel", " ac_no='%s' and sts='1' ", g_mdm_ac_rel.ac_no);
					sprintf(acErrMsg, "�����ͻ��ŵ� ����˻��ڹ��������� ������ ******* tel_save_rel_num = [%d]", num);
					WRITEMSG
					if(num > 0)
					{
						sprintf(acErrMsg,"���ʺ��Ѿ���������������!!");
						WRITEMSG
						continue;
					}

					switch(g_prdt_ac_id.ac_id_type[0])
					{
						case '1':
							memset(&g_dd_mst, 0x0, sizeof(struct dd_mst_c));
							ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, "ac_id=%ld and ac_seqn=%d and ac_sts='1' ", g_prdt_ac_id.ac_id, g_prdt_ac_id.ac_seqn);
							if(ret == 100)
							{
								sprintf(acErrMsg, "������ϸ�ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "N062");
								break;
							}
							ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
							if(ret == 100)
							{
								sprintf(acErrMsg, "���ڲ�Ʒ������\n");
								WRITEMSG
								strcpy(g_pub_tx.reply, "N062");
							}
							if(g_dd_mst.ac_type[0] != '6' && strcmp(g_dd_mst.prdt_no,"142") != 0)
							{
								memset(&g_com_rate, '\0', sizeof(g_com_rate));
								if(strcmp(g_dd_mst.prdt_no,"107") == 0)
								{
									ret = Com_rate_Sel(g_pub_tx.reply,&g_com_rate,"cur_no='01' and rate_code='B00' and end_date>=99999999");
								}
								else
								{
									ret = Com_rate_Sel(g_pub_tx.reply, &g_com_rate, "cur_no='01' and rate_code='%s' and end_date>=99999999", g_dd_parm.rate_no);
								}
								rate = (1 + g_dd_mst.flt_ratio) * g_com_rate.rate_val;
							}
							else
							{
								rate = g_dd_mst.rate;
							}
							cif_no_tmp = g_dd_mst.cif_no;
							bal_tmp = g_dd_mst.bal;
							ac_type = 1;
							sprintf(pr_prdt_no, "%s", g_dd_mst.prdt_no);
							is_print = 1;
							break;

						case '2':
							memset(&g_td_mst, 0x0, sizeof(struct td_mst_c));
							ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst, "ac_id=%ld and ac_seqn=%d and ac_sts='1' ", g_prdt_ac_id.ac_id, g_prdt_ac_id.ac_seqn);
							if(ret == 100)
							{
								sprintf(acErrMsg, "�����ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "N062");
								break;
							}
							ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'", g_td_mst.prdt_no);
							if(ret == 100)
							{
								sprintf(acErrMsg, "���ڲ�Ʒ������\n");
								WRITEMSG
								strcpy(g_pub_tx.reply, "N062");

							}
							cif_no_tmp = g_td_mst.cif_no;
							bal_tmp = g_td_mst.bal;
							rate = g_td_mst.rate;
							ac_type = 1;
							sprintf(pr_prdt_no, "%s", g_td_mst.prdt_no);
							is_print = 1;
							break;

						case '3':
							memset(&g_ln_mst, 0x0, sizeof(struct ln_mst_c));
							ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, "ac_id=%ld and ac_seqn=%d and ac_sts='1' ", g_prdt_ac_id.ac_id, g_prdt_ac_id.ac_seqn);
							if(ret == 100)
							{
								sprintf(acErrMsg,"�����ļ������ڸü�¼!!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "N062");
								break;
							}
							ret = Td_parm_Sel(g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", g_ln_mst.prdt_no);
							if(ret == 100)
							{
								sprintf(acErrMsg, "�����Ʒ������\n");
								WRITEMSG
								strcpy(g_pub_tx.reply, "N062");

							}
							cif_no_tmp = g_ln_mst.cif_no;
							bal_tmp = g_ln_mst.bal;
							rate = g_ln_mst.rate;
							ac_type = 2;
							sprintf(pr_prdt_no, "%s", g_ln_mst.prdt_no);
							is_print = 1;
							break;
						default:
							break;
					} /*switch����*/
					if(is_print == 1)
					{
						fprintf(fp, "%s %4s %ld %s %.5lf %.2lf %d %s\n", pr_date, g_tel_save_rel.tel, cif_no_tmp, g_mdm_ac_rel.ac_no, rate, bal_tmp, ac_type, pr_prdt_no);
					}
				}
				Prdt_ac_id_Clo_Sel ();
			}
		}
		Tel_save_rel_Clo_Sel ();
	}

	OkExit:
		sql_commit();	/*--�ύ����--*/
		strcpy(g_pub_tx.reply,"0000");
		sprintf(acErrMsg,"��Ա�����ϵ��ͳ�Ƴɹ�!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		set_zd_data("0120",g_pub_tx.reply);
		return 0;
	ErrExit:
		Trace_log_Clo_Sel();
		sql_rollback();	/*--����ع�--*/
		sprintf(acErrMsg,"��Ա�����ϵ��ͳ��ʧ��!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		set_zd_data("0120",g_pub_tx.reply);
		return 1;
}
