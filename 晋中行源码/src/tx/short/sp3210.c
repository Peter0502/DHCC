/***************************************************************
* 文 件 名:     sp3210.c
* 功能描述：	  科目记账 只允许 科目记账 不允许记5打头科目
* 作    者:     jack
* 完成日期：    2004年1月10日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "dc_acc_rel_c.h"
#include "prdt_ac_id_c.h"
#include "in_parm_c.h"
/**#include "rpt_trace_log_c.h"**/
sp3210()
{
	int             ret = 0;
	char            acc_no1[20];
	char            acc_no2[ACC_NO_LVL3_LEN+1];
	char            dc_ind[2];
	char            trantyp[2];
	char            dscpts[11];
	char            acc_name1[60];
	char            acc_name2[60];
	char            df_br_no[6];
	char            br_no[6];
	double          tx_amt = 0.00;
	struct com_item_c com_item1;
	struct com_item_c com_item2;
	struct prdt_ac_id_c prdt_ac_id;
	
	memset(&prdt_ac_id, 0x00, sizeof(prdt_ac_id));
	memset(&com_item1, 0x00, sizeof(com_item1));
	memset(&com_item2, 0x00, sizeof(com_item2));
	memset(acc_no1, 0x00, sizeof(acc_no1));
	memset(acc_no2, 0x00, sizeof(acc_no2));
	memset(dc_ind, 0x00, sizeof(dc_ind));
	memset(trantyp, 0x00, sizeof(trantyp));
	memset(dscpts, 0x00, sizeof(dscpts));
	memset(acc_name1, 0x00, sizeof(acc_name1));
	memset(acc_name2, 0x00, sizeof(acc_name2));
	memset(br_no, 0x00, sizeof(br_no));
	memset(df_br_no, 0x00, sizeof(df_br_no));
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	get_zd_data("0030", br_no);
	get_zd_data("0250", acc_name1);
	get_zd_data("0260", acc_name2);
	get_zd_double("0400", &tx_amt);
	get_zd_data("0310", acc_no1);
	vtcp_log("%s,%d,%s\n", __FILE__, __LINE__, acc_no1);
	get_zd_data("0650", acc_no2);
	get_zd_data("0660", dc_ind);
	get_zd_data("0880", dscpts);
	get_zd_data("0910", df_br_no);
	pub_base_strpack(acc_no1);
	pub_base_strpack(acc_no2);
	pub_base_strpack(dc_ind);
	pub_base_strpack(dscpts);
	pub_base_strpack(acc_name1);
	pub_base_strpack(acc_name2);
	pub_base_strpack(br_no);
	pub_base_strpack(df_br_no);
	strcpy(g_pub_tx.brf, dscpts);
	int             i = 0;
	i = strlen(acc_no1);
	/*
	if (i == ACC_NO_LVL1_LEN || i == ACC_NO_LVL2_LEN || i == ACC_NO_LVL3_LEN) {
	*/
	if (i >= 3 &&  i <= ACC_NO_LVL3_LEN) {	/* 科目与内部帐号 */
		memset(&com_item1, 0x00, sizeof(com_item1));
		/* 根据科目号查询科目名称 */
		ret = Com_item_Sel(g_pub_tx.reply, &com_item1, "acc_no='%s'", acc_no1);
		if (ret == 100) {
			sprintf(acErrMsg, "该科目号不存在!!acc_hrt=[%s]", acc_no1);
			WRITEMSG
				strcpy(g_pub_tx.reply, "O074");
			goto ErrExit;
		} else if (ret) {
			sprintf(acErrMsg, "查询公共科目表异常!!ret=[%d]", ret);
			WRITEMSG
				goto ErrExit;
		}
		if (com_item1.acc_knd[0] != '3') {
			sprintf(acErrMsg, "该科目下有账户或有子科目!!acc_knd=[%s]", com_item1.acc_knd);
			WRITEMSG
				strcpy(g_pub_tx.reply, "P406");
			goto ErrExit;
		}
		if (dc_ind[0] == '1') {
			if (com_item1.amt_dc_ind[0] == '2') {
				if (memcmp(com_item1.acc_no, "529", 3) == 0) {
				} else {
					sprintf(acErrMsg, "此科目只能记贷方帐![%s]", com_item1.acc_no);
					WRITEMSG
						strcpy(g_pub_tx.reply, "A025");
					goto ErrExit;
				}
			}
		} else if (dc_ind[0] == '2') {
			if (com_item1.amt_dc_ind[0] == '1') {
				if (memcmp(com_item1.acc_no, "529", 3) == 0) {
				} else {
					sprintf(acErrMsg, "此科目只能记借方帐![%s]", com_item1.acc_no);
					WRITEMSG
						strcpy(g_pub_tx.reply, "A024");
					goto ErrExit;
				}
			}
		}
	}
	i = strlen(acc_no2);
	/*
	if (i == ACC_NO_LVL1_LEN || i == ACC_NO_LVL2_LEN || i == ACC_NO_LVL3_LEN) {
	*/
	if (i >= 3 && i <= ACC_NO_LVL3_LEN) {	/* 科目与内部帐号 */
		memset(&com_item2, 0x00, sizeof(com_item2));
		/* 根据科目号查询科目名称 */
		ret = Com_item_Sel(g_pub_tx.reply, &com_item2, "acc_no='%s'", acc_no2);
		if (ret == 100) {
			sprintf(acErrMsg, "该科目号不存在!!acc_hrt=[%s]", acc_no2);
			WRITEMSG
				strcpy(g_pub_tx.reply, "O074");
			goto ErrExit;
		} else if (ret) {
			sprintf(acErrMsg, "查询公共科目表异常!!ret=[%d]", ret);
			WRITEMSG
				goto ErrExit;
		}
		if (com_item2.acc_knd[0] != '3') {
			sprintf(acErrMsg, "该科目下有账户或有子科目!!acc_knd=[%s]", com_item2.acc_knd);
			WRITEMSG
				strcpy(g_pub_tx.reply, "P406");
			goto ErrExit;
		}
		if (dc_ind[0] == '1') {
			if (com_item2.amt_dc_ind[0] == '1') {
				if (memcmp(com_item2.acc_no, "529", 3) == 0) {
				} if(com_item2.acc_no[0]=='5'){
					sprintf(acErrMsg, "此科目只能记借方帐![%s]", com_item2.acc_no);
					WRITEMSG
						strcpy(g_pub_tx.reply, "P404");
					goto ErrExit;
				}
			}
			if (com_item2.amt_dc_ind[0] == '2') {
				/**
				if (memcmp(com_item2.acc_no, "529", 3) == 0 || memcmp(com_item2.acc_no,"511",3)==0) {
				} else {
					sprintf(acErrMsg, "此科目只能记贷方帐![%s]", com_item2.acc_no);
					WRITEMSG
					strcpy(g_pub_tx.reply, "P404");
					goto ErrExit;
				}
				**/
			}
		} else if (dc_ind[0] == '2') {
			if (com_item2.amt_dc_ind[0] == '1') {
				/**
				if (memcmp(com_item2.acc_no, "529", 3) == 0) {
				} else {
					sprintf(acErrMsg, "此科目只能记借方帐![%s]", com_item2.acc_no);
					WRITEMSG
						strcpy(g_pub_tx.reply, "P404");
					goto ErrExit;
				}
				**/
			}
			if (com_item2.amt_dc_ind[0] == '2') {
				if (memcmp(com_item2.acc_no, "529", 3) == 0) {
				} if(com_item2.acc_no[0]=='5'){
					sprintf(acErrMsg, "此科目只能记贷方帐![%s]", com_item2.acc_no);
					WRITEMSG
						strcpy(g_pub_tx.reply, "P404");
					goto ErrExit;
				}
			}
		}
	}
	strcpy(g_pub_tx.ac_id_type, "9");	/* 账户类型为内部 */
	strcpy(g_pub_tx.ac_get_ind, "00");	/* 本程序未读取分户 */
	strcpy(g_pub_tx.ac_wrk_ind, "000");	/* 不核对凭证号，零金额不计流水、明细 */
	strcpy(g_pub_tx.end_note_no, g_pub_tx.beg_note_no);
	strcpy(g_pub_tx.prt_ind, "0");	/* 不登折 */
	g_pub_tx.svc_ind = 9001;/* 内部帐存取 */
	strcpy(g_pub_tx.prt_ind, "0");
	strcpy(g_pub_tx.hst_ind, "1");	/* 日间入明细 */
	set_zd_data("0151", "");
	strcpy(g_pub_tx.prdt_code, "");
	strcpy(g_pub_tx.ac_no, "");
	g_pub_tx.ac_id = 0;
	if (dc_ind[0] == '1') {
		if (strlen(acc_no1) <= ACC_NO_LVL3_LEN) {
			memcpy(g_pub_tx.opn_br_no, br_no, 5);
			g_pub_tx.tx_amt1 = tx_amt;	/* 支行借方 */
			g_pub_tx.ct_ind[0] = '2';
			g_pub_tx.add_ind[0] = '0';	/* 借 */
			memcpy(g_pub_tx.cur_no, "01", 2);
			set_zd_data("1201", acc_no1);
			set_zd_double("1208", g_pub_tx.tx_amt1);
			set_zd_data("120A", g_pub_tx.brf);
			set_zd_data("1205", g_pub_tx.ct_ind);
			set_zd_data("1204", g_pub_tx.cur_no);
			set_zd_data("0152", com_item1.acc_hrt);
			ret = pubf_acct_proc("A016");
			if (ret != 0) {
				sprintf(acErrMsg, "记机构%s的%s科目会计帐错误!! ret=[%d]", br_no,acc_no1,ret);
				WRITEMSG
					goto ErrExit;
			}
			strcpy(g_pub_tx.tx_br_no, br_no);	/**交易流水为财务的**/
			strcpy(g_pub_tx.ac_no, acc_no1);
			strcpy(g_pub_tx.sub_tx_code, "A016");
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 begin****/
            g_pub_tx.trace_cnt--;
            set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);   
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 end****/
			ret = pub_ins_trace_log();
			if (ret) {
				sprintf(acErrMsg, "记日志错误!! ret=[%d]", ret);
				WRITEMSG
					goto ErrExit;
			}
			memset(&com_item2, 0x00, sizeof(com_item2));
			ret = Com_item_Sel(g_pub_tx.reply, &com_item2, "acc_no='%s'", acc_no2);
			if (ret == 100) {
				sprintf(acErrMsg, "该科目号不存在!!acc_hrt=[%s]", acc_no2);
				WRITEMSG
					strcpy(g_pub_tx.reply, "O074");
				goto ErrExit;
			} else if (ret) {
				sprintf(acErrMsg, "查询公共科目表异常!!ret=[%d]", ret);
				WRITEMSG
					goto ErrExit;
			}
			if (com_item2.acc_knd[0] != '3') {
				sprintf(acErrMsg, "该科目下有账户或有子科目!!acc_knd=[%s]", com_item2.acc_knd);
				WRITEMSG
					strcpy(g_pub_tx.reply, "P406");
				goto ErrExit;
			}
			memcpy(g_pub_tx.opn_br_no, df_br_no, 5);
			g_pub_tx.tx_amt1 = tx_amt;	/* 支行贷方负 */
			g_pub_tx.ct_ind[0] = '2';
			g_pub_tx.add_ind[0] = '1';
			memcpy(g_pub_tx.cur_no, "01", 2);
			set_zd_data("1211", acc_no2);
			set_zd_double("1218", g_pub_tx.tx_amt1);
			set_zd_data("121A", g_pub_tx.brf);
			set_zd_data("1215", g_pub_tx.ct_ind);
			set_zd_data("1214", g_pub_tx.cur_no);
			set_zd_data("0152", com_item2.acc_hrt);
			ret = pubf_acct_proc("A017");
			if (ret != 0) {
				sprintf(acErrMsg, "记机构%s的%s科目会计帐错误!! ret=[%d]", df_br_no,acc_no2,ret);
				WRITEMSG
					goto ErrExit;
			}
			strcpy(g_pub_tx.tx_br_no, br_no);	/**交易流水为财务的**/
			strcpy(g_pub_tx.ac_no, acc_no2);
			strcpy(g_pub_tx.sub_tx_code, "A017");
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 begin****/
            g_pub_tx.trace_cnt--;
            set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);   
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 end****/
			ret = pub_ins_trace_log();
			if (ret) {
				sprintf(acErrMsg, "记日志错误!! ret=[%d]", ret);
				WRITEMSG
					goto ErrExit;
			}
		} else {
			memset(&g_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
			memset(&g_in_mst, 0x0, sizeof(struct in_mst_c));
			memset(&g_in_parm, 0x0, sizeof(struct in_parm_c));
			vtcp_log("%s,%d,%s\n", __FILE__, __LINE__, acc_no1);
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", acc_no1);
			if (ret)
				goto ErrExit;
			memcpy(g_pub_tx.ac_no, g_mdm_ac_rel.ac_no, sizeof(g_pub_tx.ac_no));
			g_pub_tx.ac_id = g_mdm_ac_rel.ac_id;
			g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn;
			ret = Prdt_ac_id_Sel(g_pub_tx.reply, &prdt_ac_id, "ac_id=%ld", g_mdm_ac_rel.ac_id);
			if (ret)
				goto ErrExit;
			if (prdt_ac_id.ac_id_type[0] == '9') {
				ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
				if (ret)
					goto ErrExit;
				ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm, "prdt_no='%s'", g_in_mst.prdt_no);
				if (ret)
					goto ErrExit;
				memcpy(g_pub_tx.opn_br_no, g_in_mst.opn_br_no, 5);
				g_pub_tx.tx_amt1 = tx_amt;
				g_pub_tx.ct_ind[0] = '2';
				g_pub_tx.add_ind[0] = '0';
				memcpy(g_pub_tx.cur_no, "01", 2);
				set_zd_data("1201", g_pub_tx.ac_no);
				set_zd_double("1208", g_pub_tx.tx_amt1);
				set_zd_data("120A", g_pub_tx.brf);
				set_zd_data("1205", g_pub_tx.ct_ind);
				set_zd_data("1204", g_pub_tx.cur_no);
				set_zd_data("0152", g_in_parm.acc_hrt);
				strcpy(g_pub_tx.sub_tx_code, "A016");
				if (pub_acct_trance()) {
					vtcp_log("[%s][%d]--------------------------!!", __FILE__, __LINE__);
					sprintf(acErrMsg, "交易记帐处理错误!");
					WRITEMSG
					goto ErrExit;
				}
				ret = pubf_acct_proc("A016");
				if (ret != 0) {
					sprintf(acErrMsg, "记机构%s的%s科目会计帐错误!! ret=[%d]",g_pub_tx.opn_br_no,g_pub_tx.ac_no, ret);
					WRITEMSG
					goto ErrExit;
				}
                /****解决trace_cnt在两个流水表中不一致的问题   20111215 begin****/
                g_pub_tx.trace_cnt++;
                set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);   
                /****解决trace_cnt在两个流水表中不一致的问题   20111215 end****/
			}
			set_zd_data("1201", "");
			set_zd_double("1208", 0);
			set_zd_data("120A", "");
			set_zd_data("1205", "");
			set_zd_data("1204", "");
			set_zd_data("0152", "");
			set_zd_data("0151", "");
			memcpy(g_pub_tx.opn_br_no, df_br_no, 5);
			g_pub_tx.tx_amt1 = tx_amt;	/* 支行贷方负 */
			g_pub_tx.ct_ind[0] = '2';
			g_pub_tx.add_ind[0] = '1';
			memcpy(g_pub_tx.cur_no, "01", 2);
			set_zd_data("1211", acc_no2);
			set_zd_double("1218", g_pub_tx.tx_amt1);
			set_zd_data("121A", g_pub_tx.brf);
			set_zd_data("1215", g_pub_tx.ct_ind);
			set_zd_data("1214", g_pub_tx.cur_no);
			set_zd_data("0152", com_item2.acc_hrt);
			ret = pubf_acct_proc("A017");
			if (ret != 0) {
				sprintf(acErrMsg, "记机构的科目会计帐错误!! ret=[%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(g_pub_tx.tx_br_no, br_no);	/**交易流水为财务的**/
			strcpy(g_pub_tx.ac_no, acc_no2);
			strcpy(g_pub_tx.sub_tx_code, "A017");
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 begin****/
            g_pub_tx.trace_cnt--;
            set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);   
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 end****/
			ret = pub_ins_trace_log();
			if (ret) {
				sprintf(acErrMsg, "记日志错误!! ret=[%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
		}
	} else {
		if (strlen(acc_no1) <= ACC_NO_LVL3_LEN) {
			g_pub_tx.tx_amt1 = tx_amt;	/* 贷方 */
			g_pub_tx.ct_ind[0] = '2';
			g_pub_tx.add_ind[0] = '1';	/* 贷 */
			memcpy(g_pub_tx.cur_no, "01", 2);
			set_zd_data("1211", acc_no1);
			set_zd_double("1218", g_pub_tx.tx_amt1);
			set_zd_data("121A", g_pub_tx.brf);
			set_zd_data("1215", g_pub_tx.ct_ind);
			set_zd_data("1214", g_pub_tx.cur_no);
			set_zd_data("0152", com_item1.acc_hrt);
			ret = pubf_acct_proc("A017");
			if (ret != 0) {
				sprintf(acErrMsg, "记机构的%s科目会计帐错误!! ret=[%d]",acc_no1, ret);
				WRITEMSG
					goto ErrExit;
			}
			strcpy(g_pub_tx.tx_br_no, br_no);	/**交易流水为财务的**/
			strcpy(g_pub_tx.ac_no, acc_no1);
			strcpy(g_pub_tx.sub_tx_code, "A017");
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 begin****/
            g_pub_tx.trace_cnt--;
            set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);   
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 end****/
			ret = pub_ins_trace_log();
			vtcp_log("[%s][%d]aaaaaaa[%d]\n", __FILE__, __LINE__, ret);
			if (ret) {
				sprintf(acErrMsg, "记日志错误!! ret=[%d]", ret);
				WRITEMSG
					goto ErrExit;
			}
			memset(&com_item2, 0x00, sizeof(com_item2));
			ret = Com_item_Sel(g_pub_tx.reply, &com_item2, "acc_no='%s'", acc_no2);
			if (ret == 100) {
				sprintf(acErrMsg, "该科目号不存在!!acc_hrt=[%s]", acc_no2);
				WRITEMSG
					strcpy(g_pub_tx.reply, "O074");
				goto ErrExit;
			} else if (ret) {
				sprintf(acErrMsg, "查询公共科目表异常!!ret=[%d]", ret);
				WRITEMSG
					goto ErrExit;
			}
			if (com_item2.acc_knd[0] != '3') {
				sprintf(acErrMsg, "该科目下有账户或有子科目!!acc_knd=[%s]", com_item2.acc_knd);
				WRITEMSG
					strcpy(g_pub_tx.reply, "P406");
				goto ErrExit;
			}
			memcpy(g_pub_tx.opn_br_no, df_br_no, 5);
			g_pub_tx.tx_amt1 = tx_amt;
			g_pub_tx.ct_ind[0] = '2';
			g_pub_tx.add_ind[0] = '0';
			memcpy(g_pub_tx.cur_no, "01", 2);
			set_zd_data("1201", acc_no2);
			set_zd_double("1208", g_pub_tx.tx_amt1);
			set_zd_data("120A", g_pub_tx.brf);
			set_zd_data("1205", g_pub_tx.ct_ind);
			set_zd_data("1204", g_pub_tx.cur_no);
			set_zd_data("0152", com_item2.acc_hrt);
			ret = pubf_acct_proc("A016");
			if (ret != 0) {
				sprintf(acErrMsg, "记机构的%s科目会计帐错误!! ret=[%d]",acc_no2,ret);
				WRITEMSG
					goto ErrExit;
			}
			strcpy(g_pub_tx.tx_br_no, br_no);	/**财务记账交易流水为财务的**/
			strcpy(g_pub_tx.ac_no, acc_no2);
			strcpy(g_pub_tx.sub_tx_code, "A016");
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 begin****/
            g_pub_tx.trace_cnt--;
            set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);   
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 end****/
			ret = pub_ins_trace_log();
			vtcp_log("[%s][%d]aaaaaaa[%d]\n", __FILE__, __LINE__, ret);
			if (ret) {
				sprintf(acErrMsg, "记日志错误!! ret=[%d]", ret);
				WRITEMSG
					goto ErrExit;
			}
		} else {
			memset(&g_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
			memset(&g_in_mst, 0x0, sizeof(struct in_mst_c));
			memset(&g_in_parm, 0x0, sizeof(struct in_parm_c));
			vtcp_log("%s,%d,%s\n", __FILE__, __LINE__, acc_no1);
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", acc_no1);
			if (ret)
				goto ErrExit;
			memcpy(g_pub_tx.ac_no, g_mdm_ac_rel.ac_no, sizeof(g_pub_tx.ac_no));
			g_pub_tx.ac_id = g_mdm_ac_rel.ac_id;
			g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn;
			ret = Prdt_ac_id_Sel(g_pub_tx.reply, &prdt_ac_id, "ac_id=%ld", g_mdm_ac_rel.ac_id);
			if (ret)
				goto ErrExit;
			if (prdt_ac_id.ac_id_type[0] == '9') {
				ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
				if (ret)
					goto ErrExit;
				ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm, "prdt_no='%s'", g_in_mst.prdt_no);
				if (ret)
					goto ErrExit;
				memcpy(g_pub_tx.opn_br_no, g_in_mst.opn_br_no, 5);
				g_pub_tx.tx_amt1 = tx_amt;	/* 支行贷方负 */
				g_pub_tx.ct_ind[0] = '2';
				g_pub_tx.add_ind[0] = '1';
				memcpy(g_pub_tx.cur_no, "01", 2);
				strcpy(g_pub_tx.ac_id_type, "9");
				strcpy(g_pub_tx.ac_get_ind, "000");
				strcpy(g_pub_tx.ac_wrk_ind, "000");
				strcpy(g_pub_tx.end_note_no, g_pub_tx.beg_note_no);
				strcpy(g_pub_tx.prt_ind, "0");
				g_pub_tx.svc_ind = 9001;
				strcpy(g_pub_tx.prt_ind, "0");
				strcpy(g_pub_tx.hst_ind, "1");
				set_zd_data("1211", g_pub_tx.ac_no);
				set_zd_double("1218", g_pub_tx.tx_amt1);
				set_zd_data("121A", g_pub_tx.brf);
				set_zd_data("1215", g_pub_tx.ct_ind);
				set_zd_data("1214", g_pub_tx.cur_no);
				set_zd_data("0152", g_in_parm.acc_hrt);
				strcpy(g_pub_tx.sub_tx_code, "A017");
				if (pub_acct_trance()) {
					vtcp_log("[%s][%d]--------------------------!!", __FILE__, __LINE__);
					sprintf(acErrMsg, "交易记帐处理错误!");
					WRITEMSG
						goto ErrExit;
				}
				ret = pubf_acct_proc("A017");
				if (ret != 0) {
					sprintf(acErrMsg, "[%d]记机构的科目会计帐错误!! ret=[%d]", __LINE__,ret);
					WRITEMSG
						goto ErrExit;
				}
                /****解决trace_cnt在两个流水表中不一致的问题   20111215 begin****/
                g_pub_tx.trace_cnt++;
                set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);   
                /****解决trace_cnt在两个流水表中不一致的问题   20111215 end****/
			}
			set_zd_data("0151", "");
			set_zd_data("1211", "");
			set_zd_double("1218", 0);
			set_zd_data("121A", "");
			set_zd_data("1215", "");
			set_zd_data("1214", "");
			set_zd_data("0152", "");
			memcpy(g_pub_tx.opn_br_no, df_br_no, 5);
			g_pub_tx.tx_amt1 = tx_amt;
			g_pub_tx.ct_ind[0] = '2';
			g_pub_tx.add_ind[0] = '0';
			memcpy(g_pub_tx.cur_no, "01", 2);
			set_zd_data("1201", acc_no2);
			set_zd_double("1208", g_pub_tx.tx_amt1);
			set_zd_data("120A", g_pub_tx.brf);
			set_zd_data("1205", g_pub_tx.ct_ind);
			set_zd_data("1204", g_pub_tx.cur_no);
			set_zd_data("0152", com_item2.acc_hrt);
			ret = pubf_acct_proc("A016");
			if (ret != 0) {
				sprintf(acErrMsg, "[%d]记机构的科目会计帐错误!! ret=[%d]", __LINE__,ret);
				WRITEMSG
					goto ErrExit;
			}
			strcpy(g_pub_tx.tx_br_no, br_no);	/**财务记账交易流水为财务的,20090328 wyw 修改机构**/
			strcpy(g_pub_tx.ac_no, acc_no2);
			strcpy(g_pub_tx.sub_tx_code, "A016");
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 begin****/
            g_pub_tx.trace_cnt--;
            set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);   
            /****解决trace_cnt在两个流水表中不一致的问题   20111215 end****/
			ret = pub_ins_trace_log();
			vtcp_log("[%s][%d]aaaaaaa[%d]\n", __FILE__, __LINE__, ret);
			if (ret) {
				sprintf(acErrMsg, "记日志错误!! ret=[%d]", ret);
				WRITEMSG
					goto ErrExit;
			}
		}
	}
	/****
  ret=pub_rpt_trace_log();
	if(ret)
	{
	   goto ErrExit;
	}
	****/
OKExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "判断现转标志成功![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0  || strlen(g_pub_tx.reply)==0)
		strcpy(g_pub_tx.reply, "P156");
	sprintf(acErrMsg, "判断现转标志失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
ErrExit2:
	strcpy(g_pub_tx.reply, "0002");
	sprintf(acErrMsg, "判断现转标志成功![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit3:
	strcpy(g_pub_tx.reply, "0003");
	sprintf(acErrMsg, "判断现转标志成功![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
}
/**
int
pub_rpt_trace_log()
{
	*************登记报表流水****************
	char            rpt_ct_ind[2];
	char            rpt_cash_ac_no[25];
	char            rpt_cash_ac_name[61];
	char            rpt_pay_ac_no[25];
	char            rpt_pay_ac_name[61];
	char            rpt_cash_br_no[13];
	char            rpt_cash_br_name[61];
	char            rpt_pay_br_no[13];
	char            rpt_pay_br_name[61];
	double          rpt_tx_amt = 0.00;
	char            rpt_note_type[4];
	char            rpt_note_no[17];
	char            rpt_tel[TEL_LENGTH+1];
	char            br_no[6];
	char            rpt_sts[2];
	char            rpt_brf[21];
	char            rpt_dc_ind[2];
	long            rpt_trace_no = 0;
	long            rpt_tx_date = 0;
	long            rpt_tx_time = 0;
	int             rpt_pay_ac_seqn = 0;
	int             rpt_cash_ac_seqn = 0;
	struct mdm_ac_rel_c rpt_mdm_ac_rel;
	struct dd_mst_c rpt_dd_mst;
	struct mdm_ac_rel_c rpt_mdm_ac_rel1;
	struct dd_mst_c rpt_dd_mst1;
	struct in_mst_c rpt_in_mst1;
	struct com_branch_c rpt_com_branch;
	struct com_item_c rpt_com_item;
	struct com_item_c rpt_com_item1;
	struct com_branch_c rpt_com_branch1;
	struct rpt_trace_log_c rpt_trace_log;
	int             ret = 0;
	memset(rpt_ct_ind, 0x00, sizeof(rpt_ct_ind));
	memset(rpt_cash_ac_no, 0x00, sizeof(rpt_cash_ac_no));
	memset(rpt_cash_ac_name, 0x00, sizeof(rpt_cash_ac_name));
	memset(rpt_pay_ac_no, 0x00, sizeof(rpt_pay_ac_no));
	memset(rpt_pay_ac_name, 0x00, sizeof(rpt_pay_ac_name));
	memset(rpt_cash_br_no, 0x00, sizeof(rpt_cash_br_no));
	memset(rpt_cash_br_name, 0x00, sizeof(rpt_cash_br_name));
	memset(rpt_pay_br_no, 0x00, sizeof(rpt_pay_br_no));
	memset(rpt_pay_br_name, 0x00, sizeof(rpt_pay_br_name));
	memset(rpt_note_type, 0x00, sizeof(rpt_note_type));
	memset(rpt_note_no, 0x00, sizeof(rpt_note_no));
	memset(rpt_tel, 0x00, sizeof(rpt_tel));
	memset(br_no, 0x00, sizeof(br_no));
	memset(rpt_sts, 0x00, sizeof(rpt_sts));
	memset(rpt_brf, 0x00, sizeof(rpt_brf));
	memset(rpt_dc_ind, 0x00, sizeof(rpt_dc_ind));
	memset(rpt_ct_ind, 0x00, sizeof(rpt_ct_ind));
	memset(&rpt_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&rpt_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&rpt_mdm_ac_rel1, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&rpt_dd_mst1, 0x00, sizeof(struct dd_mst_c));
	memset(&rpt_in_mst1, 0x00, sizeof(struct in_mst_c));
	memset(&rpt_com_branch, 0x00, sizeof(struct com_branch_c));
	memset(&rpt_com_item, 0x00, sizeof(struct com_item_c));
	memset(&rpt_com_item1, 0x00, sizeof(struct com_item_c));
	memset(&rpt_com_branch1, 0x00, sizeof(struct com_branch_c));
	memset(&rpt_trace_log, 0x00, sizeof(struct rpt_trace_log_c));
	vtcp_log("%s,%d Gethere\n", __FILE__, __LINE__);
	get_zd_data("0660", rpt_dc_ind);
	if (rpt_dc_ind[0] == '1') {
		get_zd_data("0640", rpt_pay_ac_no);
		pub_base_strpack(rpt_pay_ac_no);
		get_zd_double("0400", &rpt_tx_amt);
		get_zd_data("0880", rpt_brf);
		get_zd_data("0650", rpt_cash_ac_no);
		get_zd_data("0910", br_no);
		pub_base_strpack(rpt_cash_ac_no);
		strcpy(rpt_dc_ind, "0");
		*******开始组建表结构******

		ret = Com_item_Sel(g_pub_tx.reply, &rpt_com_item, "acc_no='%s' ", rpt_pay_ac_no);
		if (ret) {
			sprintf(acErrMsg, "帐号不存在!");
			WRITEMSG
				goto ErrExit;
		}
		ret = Com_branch_Sel(g_pub_tx.reply, &rpt_com_branch, "br_no='%s'", g_pub_tx.tx_br_no);
		if (ret) {
			sprintf(acErrMsg, "机构不存在!");
			WRITEMSG
				goto ErrExit;
		}
		*
		if (strlen(rpt_cash_ac_no) == ACC_NO_LVL1_LEN || strlen(rpt_cash_ac_no) == ACC_NO_LVL2_LEN || strlen(rpt_cash_ac_no) == ACC_NO_LVL3_LEN) {
		*
		if (strlen(rpt_cash_ac_no) >= 3 && strlen(rpt_cash_ac_no) <= ACC_NO_LVL3_LEN) {
			ret = Com_item_Sel(g_pub_tx.reply, &rpt_com_item1, "acc_no='%s' ", rpt_cash_ac_no);
			if (ret) {
				sprintf(acErrMsg, "帐号不存在!");
				WRITEMSG
					goto ErrExit;
			}
			ret = Com_branch_Sel(g_pub_tx.reply, &rpt_com_branch1, "br_no='%s'", br_no);
			if (ret) {
				sprintf(acErrMsg, "机构不存在!");
				WRITEMSG
					goto ErrExit;
			}
		}
		strcpy(rpt_trace_log.cash_ac_no, rpt_cash_ac_no);
		*
		if (strlen(rpt_cash_ac_no) == ACC_NO_LVL1_LEN || strlen(rpt_cash_ac_no) == ACC_NO_LVL2_LEN || strlen(rpt_cash_ac_no) == ACC_NO_LVL3_LEN) {
		*
		if (strlen(rpt_cash_ac_no) == ACC_NO_LVL1_LEN || strlen(rpt_cash_ac_no) == ACC_NO_LVL2_LEN || strlen(rpt_cash_ac_no) == ACC_NO_LVL3_LEN) {
			strcpy(rpt_trace_log.cash_br_no, rpt_com_branch1.br_no);
			strcpy(rpt_trace_log.cash_ac_name, rpt_com_item1.acc_name);
			strcpy(rpt_trace_log.cash_br_name, rpt_com_branch1.br_name);
		}
		strcpy(rpt_trace_log.pay_ac_no, rpt_pay_ac_no);
		strcpy(rpt_trace_log.pay_br_no, rpt_com_branch.br_no);
		strcpy(rpt_trace_log.pay_ac_name, rpt_com_item.acc_name);
		strcpy(rpt_trace_log.pay_br_name, rpt_com_branch.br_name);
		strcpy(rpt_trace_log.note_type, rpt_note_type);
		strcpy(rpt_trace_log.note_no, rpt_note_no);
		strcpy(rpt_trace_log.tel, g_pub_tx.tel);
		strcpy(rpt_trace_log.dc_ind, rpt_dc_ind);
		strcpy(rpt_trace_log.ct_ind, rpt_ct_ind);
		strcpy(rpt_trace_log.brf, rpt_brf);
		strcpy(rpt_trace_log.sts, "0");
		strcpy(rpt_trace_log.tx_br_no, g_pub_tx.tx_br_no);
		strcpy(rpt_trace_log.tx_code, g_pub_tx.tx_code);
		rpt_trace_log.trace_no = g_pub_tx.trace_no;
		rpt_trace_log.tx_date = g_pub_tx.tx_date;
		rpt_trace_log.tx_time = g_pub_tx.tx_time;
		rpt_trace_log.tx_amt = rpt_tx_amt;
		get_zd_long("1261",&rpt_trace_log.tt_no);  ***add by zhanghongbo 20090819***
		ret = Rpt_trace_log_Ins(rpt_trace_log, g_pub_tx.reply);
		if (ret) {
			sprintf(acErrMsg, "登记流水错误!");
			WRITEMSG
				goto ErrExit;
		}
	} else {
		get_zd_data("0650", rpt_cash_ac_no);
		pub_base_strpack(rpt_cash_ac_no);
		get_zd_double("0400", &rpt_tx_amt);
		get_zd_data("0880", rpt_brf);
		get_zd_data("0640", rpt_pay_ac_no);
		get_zd_data("0910", br_no);
		pub_base_strpack(rpt_pay_ac_no);
		strcpy(rpt_dc_ind, "1");
		*******开始组建表结构******
		ret = Com_item_Sel(g_pub_tx.reply, &rpt_com_item, "acc_no='%s' ", rpt_cash_ac_no);
		if (ret) {
			sprintf(acErrMsg, "帐号不存在!");
			WRITEMSG
				goto ErrExit;
		}
		ret = Com_branch_Sel(g_pub_tx.reply, &rpt_com_branch, "br_no='%s'", br_no);
		if (ret) {
			sprintf(acErrMsg, "机构不存在!");
			WRITEMSG
				goto ErrExit;
		}
		*
		if (strlen(rpt_pay_ac_no) == ACC_NO_LVL1_LEN || strlen(rpt_pay_ac_no) == ACC_NO_LVL2_LEN || strlen(rpt_pay_ac_no) == ACC_NO_LVL3_LEN) {
		*
		if (strlen(rpt_pay_ac_no) >= 3 && strlen(rpt_pay_ac_no) <= ACC_NO_LVL3_LEN) {
			ret = Com_item_Sel(g_pub_tx.reply, &rpt_com_item1, "acc_no='%s' ", rpt_pay_ac_no);
			if (ret) {
				sprintf(acErrMsg, "帐号不存在!");
				WRITEMSG
					goto ErrExit;
			}
			ret = Com_branch_Sel(g_pub_tx.reply, &rpt_com_branch1, "br_no='%s'", g_pub_tx.tx_br_no);
			if (ret) {
				sprintf(acErrMsg, "机构不存在!");
				WRITEMSG
					goto ErrExit;
			}
		}
		strcpy(rpt_trace_log.pay_ac_no, rpt_pay_ac_no);
		*
		if (strlen(rpt_pay_ac_no) == ACC_NO_LVL1_LEN || strlen(rpt_pay_ac_no) == ACC_NO_LVL2_LEN || strlen(rpt_pay_ac_no) == ACC_NO_LVL3_LEN) {
		*
		if (strlen(rpt_pay_ac_no) >= 3 && strlen(rpt_pay_ac_no) <= ACC_NO_LVL3_LEN) {
			strcpy(rpt_trace_log.pay_br_no, rpt_com_branch1.br_no);
			strcpy(rpt_trace_log.pay_ac_name, rpt_com_item1.acc_name);
			strcpy(rpt_trace_log.pay_br_name, rpt_com_branch1.br_name);
		}
		strcpy(rpt_trace_log.cash_ac_no, rpt_cash_ac_no);
		strcpy(rpt_trace_log.cash_br_no, rpt_com_branch.br_no);
		strcpy(rpt_trace_log.cash_ac_name, rpt_com_item.acc_name);
		strcpy(rpt_trace_log.cash_br_name, rpt_com_branch.br_name);
		strcpy(rpt_trace_log.note_type, rpt_note_type);
		strcpy(rpt_trace_log.note_no, rpt_note_no);
		strcpy(rpt_trace_log.tel, g_pub_tx.tel);
		strcpy(rpt_trace_log.dc_ind, rpt_dc_ind);
		strcpy(rpt_trace_log.ct_ind, rpt_ct_ind);
		strcpy(rpt_trace_log.brf, rpt_brf);
		strcpy(rpt_trace_log.sts, "0");
		strcpy(rpt_trace_log.tx_br_no, g_pub_tx.tx_br_no);
		strcpy(rpt_trace_log.tx_code, g_pub_tx.tx_code);
		rpt_trace_log.trace_no = g_pub_tx.trace_no;
		rpt_trace_log.tx_date = g_pub_tx.tx_date;
		rpt_trace_log.tx_time = g_pub_tx.tx_time;
		rpt_trace_log.tx_amt = rpt_tx_amt;
		get_zd_long("1261",&rpt_trace_log.tt_no);  ***add by zhanghongbo 20090819***
		ret = Rpt_trace_log_Ins(rpt_trace_log, g_pub_tx.reply);
		if (ret) {
			sprintf(acErrMsg, "登记流水错误!");
			WRITEMSG
				goto ErrExit;
		}
	}
	*****************end************************
OkExit:
	return 0;
ErrExit:
	return 1;
}
jk20111208  end**/
