/*************************************************
* 文 件 名:    sp9677.c
* 功能描述：   
*              根据字母帐号返回账户序号
*
* 作    者:    rob
* 完成日期：   20033307月11日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "sub_dd_mst_c.h"

sp9677()
{
	int             ret = 0;
	int             i = 0;
	struct sub_dd_mst_c sSub_dd_mst;
	struct dic_data_c vdic_data;
	char            filename[100], tmpname[100];
	char            tmp_memo[100], prdt_name[31];
	long            sub_ac_seqn;
	double          acm=0.0,lx=0.0,sRate=0.0;
	memset(&sSub_dd_mst, 0x00, sizeof(struct sub_dd_mst_c));
	FILE           *fp = NULL;
	pub_base_sysinit();
	/** 取值、赋值 **/
	get_zd_data("1011", g_pub_tx.ac_no1);	/* 母帐号 */
	pub_base_old_acno(g_pub_tx.ac_no1);	/** 对旧帐号的处理 **/
	set_zd_data("0300", g_pub_tx.ac_no1);	/* 处理旧帐号 */
	get_zd_long("0440", &sub_ac_seqn);	/* 子帐号 */
	
	/* 判断是否开通子账户业务 */
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'" ,g_pub_tx.ac_no1);
	if(ret)
	{
		vtcp_log("%s,%d,该账号不存在!ac_no1=[%s]",__FILE__,__LINE__, g_pub_tx.ac_no1);
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}	
	/* 二级账户标志 20120213 by Wang Yongwei */
	if(g_mdm_ac_rel.draw_pwd_chg[0]!='1')
	{
		vtcp_log("%s,%d,该账号没有开通二级账户业务!ac_no1=[%s]",__FILE__,__LINE__, g_pub_tx.ac_no1);
		strcpy(g_pub_tx.reply,"SN10");
		goto ErrExit;
	}
	vtcp_log("ac_no1=[%s],ac_no=[%s]", g_pub_tx.ac_no1, g_pub_tx.ac_no);
	ret = Sub_dd_mst_Sel(g_pub_tx.reply, &sSub_dd_mst, "ac_no='%s' and sub_ac_seqn=%ld ", g_pub_tx.ac_no1, sub_ac_seqn);
	if (ret == 100)
	{
		sprintf(acErrMsg, "ac_no=[%s],sub_ac_no=[%s]!!ret=[%d]", g_pub_tx.ac_no1, g_pub_tx.ac_no, ret);
		WRITEMSG
		set_zd_data(g_pub_tx.reply,"D104");
			goto ErrExit;
	}
	else if (ret)
	{
		sprintf(acErrMsg, "ac_no=[%s],sub_ac_no=[%s]!!ret=[%d]", g_pub_tx.ac_no1, g_pub_tx.ac_no, ret);
		WRITEMSG
			goto ErrExit;
	}
	/****计息子户利息****/
	g_reply_int=pub_base_getllz("100","01",g_pub_tx.tx_date, &sRate);
	if(g_reply_int)
	{
       vtcp_log("[%s][%d] pub_base_getllz %d",__FILE__,__LINE__,g_reply_int);
       strcpy(g_pub_tx.reply,"L088");
       goto ErrExit;
	}
	if(sSub_dd_mst.rate_type[0]=='1')
	{
		sRate=sSub_dd_mst.rate;
	}
	acm=sSub_dd_mst.bal*pub_base_CalTrueDays(sSub_dd_mst.lst_date,g_pub_tx.tx_date)+sSub_dd_mst.intst_acm;
	if(sSub_dd_mst.intst_type[0]!='0') /* 20120109 by Wang Yongwei */
	{
		lx=sRate*acm/L360/L100;
	}else{
		lx=0.00;
	}
	/* 输出变量 */
	pub_base_strpack(sSub_dd_mst.name);
	vtcp_log("name=[%s]", sSub_dd_mst.name);
	set_zd_data("0250", sSub_dd_mst.name);
	set_zd_data("0310",sSub_dd_mst.sub_ac_no);
	set_zd_double("0400", sSub_dd_mst.bal);
	set_zd_double("0410",lx);
	set_zd_long("0470",sSub_dd_mst.opn_date);
	set_zd_data("0700",sSub_dd_mst.ac_sts);
	set_zd_double("0850",sSub_dd_mst.rate);

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
