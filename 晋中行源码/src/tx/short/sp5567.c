/******************************************************
* 文 件 名:  sp5567.c
* 功能描述:  查询输入机构的上级单位（上级机构，上级清算机构，上级凭证调拨单位）
* 交易目标: 
* 作    者:  侯毅
* 完成日期： 2009-9-21
insert into tx_def values('5567','上级机构查询','10000000000000000000000000000000000000000000000000000000011100000100000000000000000000000010000000000000000000000000000000000000','0','4','0','3',0)
insert into tx_flow_def values('5567',0,'5567','#$')
insert into tx_sub_def values('5567','上级机构查询','sp5567','0','0')
*******************************************************/
#include "public.h"
#include "com_branch_c.h"
#include "com_tx_br_rel_c.h"

sp5567()
{
	char cBrno[BRNO_LEN+1];
	struct com_branch_c     sBranch;
	struct com_tx_br_rel_c  sRel;
	
	memset(cBrno,0x00,sizeof(cBrno));
	memset(&sBranch,0x00,sizeof(sBranch));
	memset(&sRel,0x00,sizeof(sRel));
	
	pub_base_sysinit();
	get_zd_data("0910",cBrno);
	
	g_reply_int=Com_branch_Sel(g_pub_tx.reply,&sBranch,"br_no='%s'",cBrno);
	if(g_reply_int==100)
	{
		vtcp_log("[%s][%d]公共机构码表中已经存在该记录\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"O125");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		vtcp_log("[%s][%d]查询公共机构码表错误[%d]\n",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"O070");
		goto ErrExit;
	}
	set_zd_data("0580",sBranch.up_br_no);
	set_zd_data("0660",sBranch.br_type);
	set_zd_data("0250",sBranch.br_name);
	g_reply_int=Com_tx_br_rel_Sel(g_pub_tx.reply,&sRel,"br_no='%s' and rel_type=1",cBrno);
	if(g_reply_int==100)
	{
		vtcp_log("[%s][%d]该机构没有上级清算单位\n",__FILE__,__LINE__);
	}
	else if(g_reply_int)
	{
		vtcp_log("[%s][%d]查询交易机构关系表错误\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D167");
		goto ErrExit;
	}
	else
		set_zd_data("0590",sRel.up_br_no);

	g_reply_int=Com_tx_br_rel_Sel(g_pub_tx.reply,&sRel,"br_no='%s' and rel_type=2",cBrno);
	if(g_reply_int==100)
	{
		vtcp_log("[%s][%d]该机构没有上级凭证调拨单位\n",__FILE__,__LINE__);
	}
	else if(g_reply_int)
	{
		vtcp_log("[%s][%d]查询交易机构关系表错误\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D167");
		goto ErrExit;
	}
	else
		set_zd_data("0600",sRel.up_br_no);
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data("0120",g_pub_tx.reply);
	return g_reply_int;
}
