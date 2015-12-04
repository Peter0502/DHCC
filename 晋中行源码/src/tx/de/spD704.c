/********************************************************************
* 文 件 名: spD704.c
* 功能描述: 存单转证实书交易
* 作    者: jack
* 完成日期: 2004年1月11日
* 说    明: 根据衡水需求依照老系统将转存单交易改为两个交易，此为其二
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**********************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"
#include "cif_email_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "mo_rpl_note_c.h"
#include "td_mst_hst_c.h"

spD704()
{
	char            ac_no[25];
	char            new_note_no[17];	/****原记录中的新凭证号****/
	char            br_name[31];
	long            old_rpl_date;	/****原置换日期****/
	int             ret = 0;
	long            ac_id;
	char            mdm_code[5];

	struct mdm_attr_c mdm_attr_c;
	struct mdm_ac_rel_c f_mdm_ac_rel;
	struct mo_rpl_note_c g_mo_rpl_note;
	memset(ac_no,0x00,sizeof(ac_no));
	memset(mdm_code,0x00,sizeof(mdm_code));
	memset(new_note_no,0x00,sizeof(new_note_no));
	memset(br_name,0x00,sizeof(br_name));
	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&g_mo_rpl_note, 0x00, sizeof(struct mo_rpl_note_c));

	/*** 初始化全局变量***/
	pub_base_sysinit();

	get_zd_data("0300", ac_no);
	get_zd_data("1161", g_pub_tx.mdm_code);
	get_zd_long("0450", &old_rpl_date);
	get_zd_data("0580", new_note_no);	/****原存单号码****/
	get_zd_data("1168", g_pub_tx.beg_note_no);	/****原证实书号码****/
	pub_base_old_acno(ac_no);

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
	if (ret == 100) {
		sprintf(acErrMsg, "不存在该账户!!ac_no=[%s]", ac_no);
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "查找记录出错[%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	if (g_mdm_ac_rel.note_sts[0] == '1') {
		sprintf(acErrMsg, "该介质已挂失!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "P170");
		goto ErrExit;
	} else if (g_mdm_ac_rel.note_sts[0] == '2') {
		sprintf(acErrMsg, "该介质已挂失换证!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "P171");
		goto ErrExit;
	} else if (g_mdm_ac_rel.note_sts[0] == '3') {
		sprintf(acErrMsg, "该介质已正常换证!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "M022");
		goto ErrExit;
	} else if (g_mdm_ac_rel.note_sts[0] == '*') {
		sprintf(acErrMsg, "该介质已销户!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "P172");
		goto ErrExit;
	} else if (g_mdm_ac_rel.note_sts[0] == '4') {
		sprintf(acErrMsg, "该介质已做部提，请输入新帐号!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "O217");
		goto ErrExit;
	}
	
	ac_id = g_mdm_ac_rel.ac_id;							/* 原帐号ID */
	strcpy( mdm_code,g_mdm_ac_rel.mdm_code );			/* 原介质 */
	
	/****检验原存单号码正确性****/
	vtcp_log("[%s][%d] g_mdm_ac_rel.note_no = %s ,new_note_no =  %s \n", __FILE__, __LINE__, g_mdm_ac_rel.note_no, new_note_no);
	if (strcmp(g_mdm_ac_rel.note_no, new_note_no) != 0) {
		sprintf(acErrMsg, "原存单号不符 g_mdm_ac_rel.note_no = %s ,new_note_no = %s ", g_mdm_ac_rel.note_no, new_note_no);
		WRITEMSG
			strcpy(g_pub_tx.reply, "LS78");
		goto ErrExit;
	}
	/***  转存单必须在开户机构办理 ***/
	if (strncmp(g_mdm_ac_rel.opn_br_no, g_pub_tx.tx_br_no, 5)) {
		if (pub_base_getbrname(g_mdm_ac_rel.opn_br_no, br_name)) {
			sprintf(acErrMsg, "取行名函数错!");
			WRITEMSG
				goto ErrExit;
		}
		pub_base_strpack(br_name);
		strcpy(g_pub_tx.reply, "D211");
		sprintf(acErrMsg, "开户机构为:(%s).", br_name);
		set_zd_data(DC_GET_MSG, acErrMsg);
		WRITEMSG
			goto ErrExit;
	}
	/*** 取介质属性 ***/
	ret = Mdm_attr_Sel(g_pub_tx.reply, &mdm_attr_c, "mdm_code='%s'", \
			   g_pub_tx.mdm_code);
	if (ret == 100) {
		sprintf(acErrMsg, "不存在该介质!!mdm_code=[%s]", g_mdm_ac_rel.mdm_code);
		WRITEMSG
			strcpy(g_pub_tx.reply, "N055");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "查找记录出错[%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	/*** 取户名 ***/
	ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and ac_seqn=%d", \
			 g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn);
	if (ret == 100) {
		sprintf(acErrMsg, "不存在该账户!!ac_id=[%ld]", g_mdm_ac_rel.ac_id);
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "查找记录出错[%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	if (g_td_mst.ac_sts[0] == '*') {
		sprintf(acErrMsg, "该帐户已销");
		WRITEMSG
			strcpy(g_pub_tx.reply, "M012");
		goto ErrExit;
	}
	ret = Td_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d", \
			     g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn);
	if (ret) {
		sprintf(acErrMsg, "定义游标错误!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "P162");
		goto ErrExit;
	}
	ret = Td_mst_Fet_Upd(&g_td_mst, g_pub_tx.reply);
	if (ret == 100) {
		sprintf(acErrMsg, "账户不存在!ac_id=[%ld]", g_mdm_ac_rel.ac_id);
		WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "查找记录出错[%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	/****完全冻结解冻****/
	g_td_mst.hold_sts[0] = '0';
	g_td_mst.hold_amt = 0.00;
	g_td_mst.ctl_amt = 0.00;

	ret = Td_mst_Upd_Upd(g_td_mst, g_pub_tx.reply);
	if (ret) {
		sprintf(acErrMsg, "修改记录错误!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "P162");
		goto ErrExit;
	}
	Td_mst_Clo_Upd();

	/*** 检查产品-介质-介质关系是否匹配 ***/
	if (pub_base_check_promedi(g_td_mst.prdt_no, g_pub_tx.mdm_code, \
				   NULL_MDM, NULL_MDM)) {
		sprintf(acErrMsg, "该介质不能此产品[%s][%s]!", g_pub_tx.mdm_code, \
			g_td_mst.prdt_no);
		WRITEMSG
			goto ErrExit;
	}
	ret = Mo_rpl_note_Sel(g_pub_tx.reply, &g_mo_rpl_note, "old_ac_no = %s and mdm_code = %s and rpl_date = %ld ", \
			      ac_no, g_pub_tx.mdm_code, old_rpl_date);
	if (ret == 100) {
		sprintf(acErrMsg, "换证登记表中无此记录 ac_no = %s ,mdm_code =%s ,rpl_date = %ld ", \
			ac_no, g_pub_tx.mdm_code, old_rpl_date);
		WRITEMSG
			strcpy(g_pub_tx.reply, "CD23");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "查找记录出错[%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	/****检验原证实书号码 ****/
	vtcp_log("[%s][%d] g_pub_tx.beg_note_no = %s ,g_mo_rpl_note.old_note_no = %s \n", __FILE__, __LINE__, g_pub_tx.beg_note_no, g_mo_rpl_note.old_note_no);
	if (strcmp(g_pub_tx.beg_note_no, g_mo_rpl_note.old_note_no) != 0) {
		sprintf(acErrMsg, "原证实书号码不符 g_pub_tx.beg_note_no = %s ,g_mo_rpl_note.old_note_no = %s ", g_pub_tx.beg_note_no, g_mo_rpl_note.old_note_no);
		WRITEMSG
			strcpy(g_pub_tx.reply, "LS78");
		goto ErrExit;
	}
	
	/*更改介质帐户表信息*/
		memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and mdm_code='%s'",ac_id,g_mdm_ac_rel.ac_seqn,mdm_code);
		if( ret )
		{
			sprintf(acErrMsg,"定义定期主文件游标异常!!ret=[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O087");
			goto ErrExit;
		}

		ret = Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"定期主文件不存在该账户!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(f_mdm_ac_rel.mdm_code , g_pub_tx.mdm_code);
		strcpy(f_mdm_ac_rel.note_no , g_pub_tx.beg_note_no);

		ret = Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"更新活期主文件异常!!ret=[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O087");
			goto ErrExit;
		}
		Mdm_ac_rel_Clo_Upd( );
	
	/*** 登记换证登记薄 ***/
	strcpy(g_mo_rpl_note.old_ac_no, ac_no);
	strcpy(g_mo_rpl_note.br_no, g_pub_tx.tx_br_no);
	strcpy(g_mo_rpl_note.mdm_code, "0008");	/****登记介质为存单****/
	strcpy(g_mo_rpl_note.old_note_no, new_note_no);
	strcpy(g_mo_rpl_note.new_ac_no, ac_no);
	strcpy(g_mo_rpl_note.new_note_no, g_pub_tx.beg_note_no);
	g_mo_rpl_note.rpl_date = g_pub_tx.tx_date;
	strcpy(g_mo_rpl_note.tel, g_pub_tx.tel);
	g_mo_rpl_note.trace_no = g_pub_tx.trace_no;
	/*get_zd_long("1261",&g_mo_rpl_note.tt_no);add by cgx 20090818 nxhhyh */
	strcpy(g_mo_rpl_note.rmk, "3");

	ret = Mo_rpl_note_Ins(g_mo_rpl_note, g_pub_tx.reply);
	if (ret) {
		sprintf(acErrMsg, "登记换证登记薄出错[%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P157");
		goto ErrExit;
	}
	/** 开户回显 **/
	set_zd_data("1167", g_pub_tx.ac_no);
	set_zd_int("103D", g_pub_tx.ac_seqn);

	if (pub_ins_trace_log()) {
		sprintf(acErrMsg, "登记交易流水错误!");
		WRITEMSG
			goto ErrExit;
	}
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "存单转证实书交易成功![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "存单转证实书交易失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
