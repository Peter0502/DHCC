/***************************************************************
* 文 件 名: spD205.c
* 功能描述：销户转开
* 作    者: Gujy
* 完成日期：2007年7月2日
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
#include "card.h"
#include "cif_id_code_rel_c.h"
#include "cif_mger_rel_c.h"

spD205()
{
	FILE           *fp;
	int             ret = 0;
	char            tmpname[100], intst_type[17], ct_name[5], term[3], term_type[3], memo[5], id_name[21];
	char            br_name[31], br_tele[31], filename[25];
	char            acc_no[28];
	double          tval = 0.00, val = 0.00, dealval = 0.00, avl = 0.00, oavl = 0.00;
	char            kinbr[6], reply[8];
	char            clty, cashflg;
	struct cif_mger_rel_c cif_mger_rel_t;

	memset(tmpname,0x00,sizeof(tmpname));
        memset(ct_name,0x00,sizeof(ct_name));
        memset(term,0x00,sizeof(term));
        memset(term_type,0x00,sizeof(term_type));
        memset(id_name,0x00,sizeof(id_name));
        memset(intst_type,0x00,sizeof(intst_type));
        memset(memo,0x00,sizeof(memo));
	memset(br_name,0x00,sizeof(br_name));
        memset(br_tele,0x00,sizeof(br_tele));
	memset(acc_no,0x00,sizeof(acc_no));
	memset(kinbr,0x0,sizeof(kinbr));
	memset(reply,0x0,sizeof(reply));
	get_zd_double("102P", &dealval);
	get_zd_double("102Q", &val);
	get_zd_double("102S", &avl);
	get_zd_double("102I", &tval);
	vtcp_log("[%s] [%d] 102S[%.2lf] 102I[%.2lf]\n", __FILE__, __LINE__, avl, tval);
	get_zd_data("1021", acc_no);
	vtcp_log("[%s] [%d] 102S[%.2lf] 102I[%.2lf]\n", __FILE__, __LINE__, avl, tval);


	get_zd_data("0670", &clty);
	switch (clty) {
	case '8':
		oavl = avl;
		break;
	case '9':
		oavl = avl + dealval - val;
		break;

	default:
		oavl = 0;
		strcpy(g_pub_tx.reply, "0001");
	}
	vtcp_log("%d@%s   oavl[%.2lf] = avl[%.2lf]+dealval[%.2lf]-val[%.2lf] \n", __LINE__, __FILE__, oavl, avl, dealval, val);
	get_zd_data("0680", &cashflg);
	set_zd_data("102K", &cashflg);
	set_zd_double("1037", oavl);
	if ('8' == clty || '9' == clty) {
		struct mdm_ac_rel_c t_mdm_ac_rel;

		strcpy(g_pub_tx.reply, "0002");
		ret = Cif_mger_rel_Sel(reply, &cif_mger_rel_t, "ac_no = '%s'", acc_no);
		vtcp_log("%d@%s , ret [%d]\n", __LINE__, __FILE__, ret);
		if (!ret) {
			set_zd_data("0920", cif_mger_rel_t.mang);
			set_zd_data("0700", cif_mger_rel_t.mang_type);
		} else {
			if (100 != ret) {
				strcpy(g_pub_tx.reply, reply);
				vtcp_log("%d@%s , ret [%d]\n", __LINE__, __FILE__, ret);
				goto ErrExit;
			}
		}

		memset(&t_mdm_ac_rel, 0x0, sizeof(t_mdm_ac_rel));
		pub_base_old_acno(acc_no); /***add by xyy 2009-05-11增加新旧账户转换**/
		if (ret = Mdm_ac_rel_Sel(reply, &t_mdm_ac_rel, "ac_no = '%s'", acc_no)) {
			strcpy(g_pub_tx.reply, reply);
			vtcp_log("%d@%s  ret = [%d] \n", __LINE__, __FILE__, ret);
			goto ErrExit;
		}
		set_zd_data("102G", t_mdm_ac_rel.id_type);
		set_zd_data("102A", t_mdm_ac_rel.id_no);
		set_zd_data("1162", g_pub_tx.crd_no);
	}
OkExit:
	sprintf(acErrMsg, "销户转开配置结束![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 0;
ErrExit:
	/* strcpy(g_pub_tx.reply, reply); */
	sprintf(acErrMsg, "销户失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 1;
}
