/*************************************************
* 文 件 名:  spW002.c
* 功能描述： 账户信息查询
*
* 作    者:  chengbo 
* 完成日期： 2009年3月25日
*
* 修改记录： 
* 日   期:	2009年3月31日
* 修 改 人:	gujingyu *Lsy20100928*
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"

spW002()
{
	int             ret, i = 0, vcnt;
	char            id_no[21];
	char            ac_no[21], filename[300];
	char            id_type[2];
	char            ac_std[10];
	int             ttlnum;
	int             cunqi;
	FILE           *fp;
	double          ttbal = 0;
	long             dBeg_num, dEnd_num;
	int             date;

	struct prdt_ac_id_c sPrdt_ac_id;	/* 产品账户结构 */
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_attr_c sMdm_attr;
	struct td_mst_c sTd_mst;
	struct dd_mst_c sDd_mst;
	struct dd_parm_c sDd_parm;
	struct td_parm_c sTd_parm;
	/* 清空 */
	memset(&sMdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&sPrdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
	memset(&sTd_mst, 0x00, sizeof(struct td_mst_c));
	memset(&sDd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&sMdm_attr, 0x00, sizeof(struct mdm_attr_c));
	memset(&sTd_parm, 0x00, sizeof(struct td_parm_c));
	memset(&sDd_parm, 0x00, sizeof(struct dd_parm_c));
	memset(ac_no, 0x00, sizeof(ac_no));
	memset(filename, 0x00, sizeof(filename));
	memset(ac_std, 0x00, sizeof(ac_std));
	memset(id_type, 0x00, sizeof(id_type));
	memset(id_no, 0x00, sizeof(id_no));
	/* 取值 、赋值 */
	pub_base_sysinit();
	get_zd_data("0300", ac_no);	/* 帐/卡号 */
	vtcp_log("[%s][%d]得到的帐号ac_no=[%s]\n", __FILE__, __LINE__, ac_no);
	pub_base_old_acno(ac_no);	/**对旧帐号的处理**/
	get_zd_data("0680", id_type);	/**证件类型**/
	get_zd_data("0630", id_no);	/**证件号码**/
	vtcp_log("[%s][%d]得到的帐号ac_no=[%s],id_type[%s],id_no[%s]\n", __FILE__, __LINE__, ac_no, id_type, id_no);
	get_zd_long("0540", &dBeg_num);	/* 开始的条数 */
	dBeg_num = dBeg_num - 1;
	dEnd_num = dBeg_num + 10 - 1;	/* 结束的条数 */
	vtcp_log("dBeg_num=[%d]dEnd_num=[%d]",dBeg_num,dEnd_num);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", ac_no);
	if (ret == 100) {
		sprintf(acErrMsg, "无此卡号，请检查!ac_no=[%s]", ac_no);
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "读取介质账户对照表异常![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P147");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]得到的sMdm_ac_rel.opn_br_no=[%s],sMdm_ac_rel.beg_date=[%ld],sMdm_ac_rel.note_sts=[%s]\n", __FILE__, __LINE__, sMdm_ac_rel.opn_br_no, sMdm_ac_rel.beg_date, sMdm_ac_rel.note_sts);
  set_zd_data("0770", sMdm_ac_rel.opn_br_no);
  set_zd_long("0450", sMdm_ac_rel.beg_date);
  set_zd_data("0670", sMdm_ac_rel.note_sts);
	vtcp_log("[%s][%d]得到的sMdm_ac_rel.opn_br_no=[%s],sMdm_ac_rel.beg_date=[%ld],sMdm_ac_rel.note_sts=[%s]\n", __FILE__, __LINE__, sMdm_ac_rel.opn_br_no, sMdm_ac_rel.beg_date, sMdm_ac_rel.note_sts);
  
	ret = Mdm_attr_Sel(g_pub_tx.reply, &sMdm_attr, "mdm_code='%s'", sMdm_ac_rel.mdm_code);
	if (ret == 100) {
		sprintf(acErrMsg, "不存在此介质!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "读取mdm_attr表异常![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "N056");
		goto ErrExit;
	}
	ret = Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "ac_id=%ld", sMdm_ac_rel.ac_id);
	ERR_DEAL

		ttlnum = 0;

	while (1) {
		ret = Prdt_ac_id_Fet_Sel(&sPrdt_ac_id, g_pub_tx.reply);
		if (ret == 100)
			break;
		ERR_DEAL

			if (!ttlnum) {
			/* 生成下传全路经文件名(批量) */
			pub_base_AllDwnFilName(filename);

			fp = fopen(filename, "w");
			if (fp == NULL) {
				sprintf(acErrMsg, "open file error [%s]", filename);
				WRITEMSG
					strcpy(g_pub_tx.reply, "S047");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]下传文件为%.30s",__FILE__,__LINE__,filename);
			/**fprintf(fp, "~账号|账户序号|当前余额|储种|存期|开户日期|到期日|销户日期|账户状态|开户网点|\n");**/
			fprintf(fp, "~账号|当前余额|储种|存期|开户日期|到期日|销户日期|账户状态|开户网点|\n");
			vtcp_log("%s,%d开始逐条查询帐号信息", __FILE__, __LINE__);
		}
		if (sPrdt_ac_id.ac_id_type[0] == '1') {
			/**开始查询活期产品!***/
			vtcp_log("[%s][%d]查询活期产品",__FILE__,__LINE__);
			ret = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id=%ld and ac_seqn=%d", sPrdt_ac_id.ac_id, sPrdt_ac_id.ac_seqn);
			if (ret == 100) {
				sprintf(acErrMsg, "该账户ID不存在!ac_no=[%s]", ac_no);
				WRITEMSG
					strcpy(g_pub_tx.reply, "M003");
				goto ErrExit;
			} else if (ret) {
				sprintf(acErrMsg, "读取活期主文件异常![%d]", ret);
				WRITEMSG
					strcpy(g_pub_tx.reply, "W015");
				goto ErrExit;
			}
			ttbal += sDd_mst.bal;

			ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm, "prdt_no='%s'", \
					  sPrdt_ac_id.prdt_no);
			if (ret == 100) {
				sprintf(acErrMsg, "该产品不存在!prdt_no=[%s]",
					sPrdt_ac_id.prdt_no);
				WRITEMSG
					strcpy(g_pub_tx.reply, "W025");
				goto ErrExit;
			}
			memset(ac_std, 0x00, sizeof(ac_std));
			if (sDd_mst.ac_sts[0] == '1') {
				strcpy(ac_std, "正常");
				date = 99999999;
			} else if (sDd_mst.ac_sts[0] == '3') {
				strcpy(ac_std, "挂失");
				date = sDd_mst.lst_date;
			} else if (sDd_mst.ac_sts[0] == '*') {
				strcpy(ac_std, "销户");
				date = sDd_mst.lst_date;
			} else
			{
				strcpy(ac_std, "异常");
				date = sDd_mst.lst_date;
			}
			/**开始将帐号信息写入文件**/
			/*** 生成产品列表 ***/
			ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);		
			if(ret)
			{
				sprintf(acErrMsg, "读取活期主文件异常![%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "W015");
				goto ErrExit;
			}
				vtcp_log("[%s][%d],dBeg_num=[%d]dEnd_num=[%d]", __FILE__, __LINE__,dBeg_num,dEnd_num);
			if (ttlnum >= dBeg_num && ttlnum <= dEnd_num)	/* 将符合条件的记录返回 */
				fprintf(fp, "%20s|%15.2f|%20s|%4d|%8d|%8d|%8d|%6s|%5s|\n", ac_no, sDd_mst.bal, sDd_parm.title, 0000, sDd_mst.opn_date, 99999999, date, ac_std, sDd_mst.opn_br_no);
		}
		if (sPrdt_ac_id.ac_id_type[0] == '2') {
			/**查询定期产品**/
			vtcp_log("[%s][%d]查询定期产品",__FILE__,__LINE__);
			ret = Td_mst_Sel(g_pub_tx.reply, &sTd_mst, "ac_id=%ld and ac_seqn=%d", sPrdt_ac_id.ac_id, sPrdt_ac_id.ac_seqn);
			if (ret == 100) {
				sprintf(acErrMsg, "该账户不存在定期产品!");
				WRITEMSG
					strcpy(g_pub_tx.reply, "W016");
				goto ErrExit;
			} else if (ret) {
				sprintf(acErrMsg, "读取定期主文件异常![%d]", ret);
				WRITEMSG
					strcpy(g_pub_tx.reply, "W017");
				goto ErrExit;
			}
			ttbal += sTd_mst.bal;
			ret = Td_parm_Sel(g_pub_tx.reply, &sTd_parm, "prdt_no='%s'", \
					  sPrdt_ac_id.prdt_no);
			if (ret) {
				goto ErrExit;
			}
			memset(ac_std, 0x00, sizeof(ac_std));
			if (sTd_mst.ac_sts[0] == '1') {
				date = 99999999;
				strcpy(ac_std, "正常");
			} else if (sTd_mst.ac_sts[0] == '3') {
				date = sTd_mst.lst_date;
				strcpy(ac_std, "挂失");
			} else if (sTd_mst.ac_sts[0] == '*') {
				strcpy(ac_std, "销户");
				date = sTd_mst.lst_date;
			} else
			{
				strcpy(ac_std, "异常");
				date = sTd_mst.lst_date;
			}
			if ('Y' == sTd_parm.term_type[0])
				cunqi = sTd_parm.term * 12;
			else
				cunqi = sTd_parm.term;
				vtcp_log("[%s][%d],dBeg_num=[%d]dEnd_num=[%d]", __FILE__, __LINE__,dBeg_num,dEnd_num);
			if (ttlnum >= dBeg_num && ttlnum <= dEnd_num)	/* 将符合条件的记录返回 */
				fprintf(fp, "%20s|%15.2f|%20s|%4d|%8d|%8d|%8d|%6s|%5s|\n", ac_no, sTd_mst.bal, sTd_parm.title, cunqi, sTd_mst.opn_date, sTd_mst.mtr_date, date, ac_std, sTd_mst.opn_br_no);
		}
		ttlnum++;
	}
	if(sDd_mst.name[0]!='\0')
		{
			vtcp_log("[%s][%d],sDd_mst.name=[%s]", __FILE__, __LINE__,sDd_mst.name);
			set_zd_data("0250", sDd_mst.name);
		}
	else
		{
			vtcp_log("[%s][%d],sTd_mst.name=[%s]", __FILE__, __LINE__,sTd_mst.name);
			set_zd_data("0250", sTd_mst.name);
		}
	ret = Prdt_ac_id_Clo_Sel();
	ERR_DEAL

	vtcp_log("[%s][%d]", __FILE__, __LINE__);

	fclose(fp);
	set_zd_data(DC_FILE_SND, "1");
	set_zd_int("0490", ttlnum);
	vtcp_log("[%s][%d],ttlnum=[%d]", __FILE__, __LINE__, ttlnum);
	set_zd_double("0410", ttbal);
	vtcp_log("[%s][%d],ttbal=[%f]", __FILE__, __LINE__, ttbal);

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
