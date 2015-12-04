/*************************************************
* 文 件 名:    sp9662.c
* 功能描述：   
*              根据存款账号取出账户序号和对应产品信息
*
* 作    者:    LiuHuafeng 2008-7-14 17:25:56
* 完成日期：   2008-7-14 17:26:02
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
TX_CODE	TX_NAME	BIT_MAP	LOG_IND	TX_TYPE	WRK_STS	YW_CNT_IND	YW_PERCENT
9662	存款账号取序号和产品	10000000000000000000001111100000001000000000010000000000000000000111110000000000000000000000000000000000000000000000000000000000	0	4	0	3	
TX_CODE	FLOW_CODE	SUB_TX_CODE	SUB_TX_RSLT
9662	0	9662	#$
SUB_TX_CODE	SUB_TX_NAME	TX_FUNC	ACCT_IND	CHNL_IND
9662	存款户取序号产品信息	sp9662	0	0

*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"

sp9662()
{
	int             ret = 0;
	char            ac_no[20];
	int             ac_seqn, back_ac_seqn;
	int             flag = 0, redflag = 0;
	char            tx_code[8];


	struct dd_mst_c dd_mst_tmp;
	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;

	/** 数据初始化 **/
	memset(&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	memset(&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset(&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 取值、赋值 **/

	get_zd_data("0310", ac_no);	/* 存款帐号 */
	pub_base_old_acno(ac_no);	/** 对旧帐号的处理 **/
	set_zd_data("0310", ac_no);	/* 处理旧帐号 */

	get_zd_data("0650", tx_code);	/* 交易代码 */

	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, " ac_no='%s' ", ac_no);
	if (ret == 100) {
		sprintf(acErrMsg, "凭证(介质)与账户关系表无此记录 erro code=[%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "L113");
		ERR_DEAL
	} else if (ret != 0) {
		sprintf(acErrMsg, "取凭证(介质)与账户关系表异常erro code=[%s]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		ERR_DEAL
	}
	sprintf(acErrMsg, "取出的账户ID为[%ld]", g_mdm_ac_rel.ac_id);
	WRITEMSG
	set_zd_data("0910",g_mdm_ac_rel.opn_br_no);
	/* 根据贷款账户ID查询贷款主文件个数 */
	flag = sql_count("dd_mst", " ac_id=%ld ", g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg, "根据贷款账户ID查询贷款主文件个数[%d]", flag);
	WRITEMSG

	if (flag == 0) {
		set_zd_int("0660", redflag);	/* 是否输入序号标志 */
		sprintf(acErrMsg, "贷款主文件表中无此记录[%d]", flag);
		WRITEMSG
			strcpy(g_pub_tx.reply, "L114");
		goto ErrExit;
	}
	if (flag == 1) {
		ret = Dd_mst_Sel(g_pub_tx.reply, &dd_mst_tmp, " ac_id=%ld ", g_mdm_ac_rel.ac_id);
		if (ret == 100) {
			sprintf(acErrMsg, "存款主文件表中无此记录 erro code=[%s]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "L002");
			ERR_DEAL

		} else if (ret != 0) {
			sprintf(acErrMsg, "取贷款主文件异常 erro code=[%s]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
			ERR_DEAL
		}
		sprintf(acErrMsg, "取贷款主文件PASS");
		WRITEMSG


		/* 非卡上开户专用通道 */


		/* 调用根据存款显示账号和账号序号取存款主文件函数 */
			ret = pub_dd_dddis_file(ac_no, dd_mst_tmp.ac_seqn,
				      &g_mdm_ac_rel, &g_dd_mst, &g_dd_parm);	/* 读取存款户信息 */
		if (ret) {
			sprintf(acErrMsg, "读取贷款户信息error");
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("aaaaaaaaaaaaaaa[%ld]\n", g_dd_mst.cif_no);
		long            tmpcif_no = 0;
		tmpcif_no = g_dd_mst.cif_no;
		vtcp_log("bbbbbbbbbbbbbbbb[%ld]\n", tmpcif_no);
			sprintf(acErrMsg, "交易3312!");
			WRITEMSG

			/* 输出变量 */
			set_zd_data("0250", g_dd_mst.name);	/* 存款户名 */
			set_zd_data("0230", g_dd_mst.prdt_no);	/* 产品编号 */
			set_zd_data("0260", g_dd_parm.title);	/* 产品名称 */
			set_zd_data("0680", g_dd_parm.cif_type);	/* 客户类型 */
			set_zd_long("0460", g_dd_mst.opn_date);	/* 开户日期 */
		/* 输出变量 */
		sprintf(acErrMsg, "进入输出变量");
		WRITEMSG
			redflag = 1;
		set_zd_long("0350", dd_mst_tmp.ac_seqn);	/* 贷款账户序号 */

		set_zd_int("0660", redflag);	/* 是否输入序号标志
						 * 1-唯一记录 */
		goto OkExit;
	}
	redflag = 9;
	set_zd_int("0660", redflag);	/* 是否输入序号标志 9-多条记录 */

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
