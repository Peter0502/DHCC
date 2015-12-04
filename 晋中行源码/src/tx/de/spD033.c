/***************************************************************
* 文 件 名:     spD033.c
* 功能描述：	子帐号存取款交易
*
* 作    者:     jack
* 完成日期：    2003年7月11日
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

spD033()
{
	int             ret = 0;
	int             mcnt = 0, zcnt = 0;
	char            add_ind[2];
	long            sub_ac_seqn;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(add_ind,0x00,sizeof(add_ind));
	get_zd_data("0300", g_pub_tx.ac_no);	/*** 母帐号 ***/
	pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_long("0780",&sub_ac_seqn);
	get_zd_data("0620",g_pub_tx.brf);
	/* g_pub_tx.sub_ac_seqn=sub_ac_seqn; */


	get_zd_data("0250", g_pub_tx.name);	/*** 子帐号户名 ***/
	get_zd_data("0670", add_ind);	/*** 增减标志 ***/
	sprintf(acErrMsg, "[%s][%d]主账户[%s] add_ind=%s sub_ac_seqn=%ld",__FILE__,__LINE__, g_pub_tx.ac_no,add_ind,sub_ac_seqn);
	WRITEMSG
	sprintf(g_pub_tx.ac_no1,"%s-%d",g_pub_tx.ac_no,sub_ac_seqn);
	if (add_ind[0] == '1') {
		strcpy(g_pub_tx.add_ind, "1");
		g_pub_tx.svc_ind = 1201;
	} else if (add_ind[0] == '2') {
		strcpy(g_pub_tx.add_ind, "0");
		g_pub_tx.svc_ind = 1201;
	} else if (add_ind[0] == '3') {
		strcpy(g_pub_tx.add_ind, "3");
		strcpy(g_pub_tx.brf, "子帐号销户");
		g_pub_tx.svc_ind = 1202;
	} else {
		sprintf(acErrMsg, "存款取款标志错误![%s]", add_ind);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P106");
		goto ErrExit;
	}
	get_zd_data("0680", g_pub_tx.ct_ind);	/*** 现转标志 ***/
	if (g_pub_tx.ct_ind[0] != '1') {
		g_pub_tx.ct_ind[0] = '2';
	}
	get_zd_double("0390", &g_pub_tx.tx_amt1);
	vtcp_log("[%s][%d] zylsee017 amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	/*-- modify begin by wzs 20120215 reason:同spD034 44域已经用作取款的账户序号接收 --*/
	/* get_zd_long("0440", &g_pub_tx.erro_date); */ 	/* 母帐号入帐日期 */
	get_zd_long("0450",&g_pub_tx.erro_date);
	/*-- modify end by wzs 20120215 --*/
	if (g_pub_tx.erro_date == 0) {
		g_pub_tx.erro_date = g_pub_tx.tx_date;
	}
	strcpy(g_pub_tx.cur_no, "01");	/*** 默认人民币 ***/

	/* add by rob at 20051207 检查母帐号是否发生该笔业务 */
	memset(add_ind, 0x0, sizeof(add_ind));
	if (g_pub_tx.add_ind[0] == '3') {
		strcpy(add_ind, "0");	/* 减 */
	} else {
		strcpy(add_ind, g_pub_tx.add_ind);
	}

	sprintf(acErrMsg, "---ac_no=[%s], amt=[%lf], add_ind=[%s], \
		ct_ind=[%s], tx_date=[%ld]", g_pub_tx.ac_no, g_pub_tx.tx_amt1,
		g_pub_tx.add_ind, g_pub_tx.ct_ind, g_pub_tx.erro_date);
	WRITEMSG

	ret = pub_base_sub_ac_trance(sub_ac_seqn);
	if (ret) {
		sprintf(acErrMsg, "子帐号存取款主控错误!");
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "子帐号存取款成功![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "子帐号存取款失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
