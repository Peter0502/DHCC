/***********************************************************
* 文 件 名: rpt060.c
* 功能描述：柜员揽存报表
*
* 作    者: 武大为
* 完成日期: 2014年6月16日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
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

	/*打开事务*/
	g_reply_int = sql_begin();
	if(g_reply_int != 0)
	{
		sprintf(acErrMsg, "BEGIN WORK error!!!");
		WRITEMSG
		return 1;
	}

	/*初始化全局变量*/
	pub_base_sysinit();

	strcpy(fname, "RPT060");
	g_reply_int = pub_rpt_rmfile_all(fname);
	if(g_reply_int != 0)
	{
		sprintf(acErrMsg,"删除旧报表错误!!");
		WRITEMSG
		return 1;
	}

	/*g_reply_int = Com_branch_Dec_Sel(g_pub_tx.reply, " br_type not in('0','6') and wrk_sts!='*' ORDER BY br_no"); lwb 20150525  增加分行  */
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

		/*机构号变,下一个文件*/
		if(strcmp(sComBranch.br_no, pre_br_no))
		{
			/*不是第一个机构,打上个机构的结尾*/
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

			fprintf(fp, "日期 揽存人 客户号 账户 利率 余额 账户类型 产品\n");
			strcpy(pre_br_no, sComBranch.br_no);
		}

		ret = Tel_save_rel_Dec_Sel(g_pub_tx.reply, " br_no='%s' and sts='1' order by tel, tx_date", sComBranch.br_no);
		if (ret != 0)
		{
			sprintf(acErrMsg,"[DECLARE SELECT Tel_save_rel_Dec_Sel]异常!!");
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
				sprintf(acErrMsg,"FETCT Tel_save_rel_Fet_Sel异常!!");
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
					sprintf(acErrMsg,"无此帐号请检查!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"N062");
					continue;
				}
				memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
				ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld ", g_mdm_ac_rel.ac_id);
		  		if (ret != 0 && ret != 100)
		  		{
		 			sprintf(acErrMsg, "查询产品帐号对照表错误!! [%d]", ret);
		  			WRITEMSG
		  			goto ErrExit;
				}
		  		else if (ret == 100)
		  		{
		  			ret = pub_base_CurToSeqn(g_pub_tx.cur_no, &g_pub_tx.ac_seqn);
			  		if (ret != 0)
					{
						sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]", g_pub_tx.cur_no);
						WRITEMSG
						goto ErrExit;
					}
		 		}
				else
				{
					g_pub_tx.ac_seqn = sPrdt_ac_id.ac_seqn;
				}
				sprintf(acErrMsg, "帐号!! g_mdm_ac_rel.ac_no=[%s],sPrdt_ac_id.ac_id_type=[%c]", g_mdm_ac_rel.ac_no, sPrdt_ac_id.ac_id_type[0]);
				WRITEMSG

				switch(sPrdt_ac_id.ac_id_type[0])
				{
					case '1':
						memset(&g_dd_mst, 0x0, sizeof(struct dd_mst_c));
						ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, "ac_id=%ld and ac_seqn=%d and ac_sts='1' ", sPrdt_ac_id.ac_id, sPrdt_ac_id.ac_seqn);
						if(ret == 100)
						{
							sprintf(acErrMsg, "活期明细文件不存在该记录!!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "N062");
							break;
						}
						ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
						if(ret == 100)
						{
							sprintf(acErrMsg, "活期产品不存在\n");
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
							sprintf(acErrMsg, "定期文件不存在该记录!!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "N062");
							break;
						}
						ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'", g_td_mst.prdt_no);
						if(ret == 100)
						{
							sprintf(acErrMsg, "定期产品不存在\n");
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
							sprintf(acErrMsg, "贷款文件不存在该记录!!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "N062");
							break;
						}
						ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", g_ln_mst.prdt_no);
						if(ret == 100)
						{
							sprintf(acErrMsg, "贷款产品不存在\n");
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
				} /*switch结束*/

				/* fprintf(fp,"~日期|@揽存人|@客户号|@账户|$利率|@余额|@账户类型|@产品"); */
				if(is_print == 1)
				{
					fprintf(fp, "%s %4s %ld %s %.5lf %.2lf %d %s\n", pr_date, g_tel_save_rel.tel, cif_no_tmp, g_tel_save_rel.ac_no, rate, bal_tmp, ac_type, pr_prdt_no);
				}
			}

			/* 查询关联客户号的 */
			else if(g_tel_save_rel.tx_type[0] == '2')
			{
				ret = Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "cif_no=%ld", g_tel_save_rel.cif_no);
				if (ret != 0)
				{
					sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]异常!!");
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
						sprintf(acErrMsg, "FETCT 产品帐号对照表异常!!");
						WRITEMSG
						strcpy(g_pub_tx.reply, "D102");
						goto ErrExit;
					}

					memset(&g_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c)) ;
					ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_id=%ld and opn_br_no='%s' and note_sts not in ('2','3','4')", g_prdt_ac_id.ac_id, sComBranch.br_no );
					if(ret == 100)
					{
						sprintf(acErrMsg, "无此帐号请检查!!");
						WRITEMSG
						strcpy(g_pub_tx.reply, "N062");
						continue;
					}
					num = sql_count("tel_save_rel", " ac_no='%s' and sts='1' ", g_mdm_ac_rel.ac_no);
					sprintf(acErrMsg, "关联客户号的 如果账户在关联表里有 就跳过 ******* tel_save_rel_num = [%d]", num);
					WRITEMSG
					if(num > 0)
					{
						sprintf(acErrMsg,"此帐号已经做过关联，跳过!!");
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
								sprintf(acErrMsg, "活期明细文件不存在该记录!!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "N062");
								break;
							}
							ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
							if(ret == 100)
							{
								sprintf(acErrMsg, "活期产品不存在\n");
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
								sprintf(acErrMsg, "定期文件不存在该记录!!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "N062");
								break;
							}
							ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'", g_td_mst.prdt_no);
							if(ret == 100)
							{
								sprintf(acErrMsg, "定期产品不存在\n");
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
								sprintf(acErrMsg,"贷款文件不存在该记录!!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "N062");
								break;
							}
							ret = Td_parm_Sel(g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", g_ln_mst.prdt_no);
							if(ret == 100)
							{
								sprintf(acErrMsg, "贷款产品不存在\n");
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
					} /*switch结束*/
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
		sql_commit();	/*--提交事务--*/
		strcpy(g_pub_tx.reply,"0000");
		sprintf(acErrMsg,"柜员揽存关系表统计成功!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		set_zd_data("0120",g_pub_tx.reply);
		return 0;
	ErrExit:
		Trace_log_Clo_Sel();
		sql_rollback();	/*--事务回滚--*/
		sprintf(acErrMsg,"柜员揽存关系表统计失败!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		set_zd_data("0120",g_pub_tx.reply);
		return 1;
}
