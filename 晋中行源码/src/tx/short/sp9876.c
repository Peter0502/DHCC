/*************************************************
* 文 件 名:  sp9876.c
* 功能描述： 缴费号码信息管理
*
* 作    者:  jack
* 完成日期： 2003年03月28日
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_peritemmap_c.h"
#include "ag_itemlist_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_ac_rel_c.h"

int sp9876()
{
	struct ag_peritemmap_c sAg_peritemmap;	/* 缴费号码信息表 */
	struct prdt_ac_id_c z_prdt_ac_id;
	struct mdm_ac_rel_c z_mdm_ac_rel;
	int             ret = 0;
	char            cItem_no[26];
	char cUnit_no[9];
	char            cFlag[2];
	double          dTram = 0.00;

	memset(cItem_no, 0, sizeof(cItem_no));
	memset(cUnit_no, 0, sizeof(cUnit_no));
	memset(&z_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&z_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sAg_peritemmap,0x00,sizeof(struct ag_peritemmap_c));
	memset(cFlag, 0, sizeof(cFlag));
	
	pub_base_sysinit();
	get_zd_data("0920",cUnit_no);
	get_zd_data("0630", cItem_no);
	/*****
	vtcp_log("%s,%d cItem_no=[%s]\n",__FILE__,__LINE__,cItem_no);
	sprintf(cItem_no,"%010d",atol(cItem_no));
	vtcp_log("%s,%d cItem_no=[%s]\n",__FILE__,__LINE__,cItem_no);
	*****/
	get_zd_data("0700", cFlag);

	ret = Ag_peritemmap_Sel(g_pub_tx.reply, &sAg_peritemmap, \
		       "unit_no = '%s' and item_no = '%s' and stat = '0'", \
				cUnit_no, cItem_no);
	if (ret == 100) {
		sprintf(acErrMsg, "没有查询到该缴费号码编号对应记录!\
			unit_no =[%s],item_no=[%s]", cUnit_no, cItem_no);
		WRITEMSG
			if (cFlag[0] == '0') {
			goto OkExit;
		}
		strcpy(g_pub_tx.reply, "H010");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "执行Ag_peritemmap错误!ret=[%d]", ret);
		WRITEMSG
			goto ErrExit;
	}
	if (cFlag[0] == '0') {
		sprintf(acErrMsg, "缴费号码对应记录已经存在,不能再增加!!!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "H011");
		goto ErrExit;
	}
	ret = sql_sum_double("ag_itemlist", "am06", &dTram, "item_no = '%s' \
			and unit_no = '%s' and stcd in('0','2')", cItem_no, cUnit_no);
	if (ret) {
		sprintf(acErrMsg, "执行sql_sum_double错误!ret=[%d]", ret);
		WRITEMSG
			goto ErrExit;
	}
	/****add 20110804 begin****/
	{
		/**新增加把帐户类型传到前台 add by zhgf 0516***/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&z_mdm_ac_rel,"ac_no='%s'",sAg_peritemmap.ac_no);
		if(ret)
		{
			sprintf(acErrMsg, "执行mdm_ac_rel_sel错误!ret=[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&z_prdt_ac_id,"ac_id=%ld",z_mdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg, "执行mdm_ac_rel_sel错误!ret=[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0670",z_prdt_ac_id.ac_id_type);
	}
	/**** add 20110804 over****/
	set_zd_data("0260", sAg_peritemmap.remark);
	set_zd_data("0270", sAg_peritemmap.name);
	set_zd_data("0310", sAg_peritemmap.ac_no);
	set_zd_double("0390", dTram);
	set_zd_data("0720", sAg_peritemmap.bat_flag);

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
