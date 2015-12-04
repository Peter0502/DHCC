/*****************************************************
* 文 件 名:  sp9403.c
* 功能描述： 小额往包信息查询
* 作    者:  chenhw
* 完成日期： 2006-9-13 13:41 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_wbctl_c.h"
int sp9403()
{
	int ret=0,i;
	char cSts[2];
	char cPackday[9];
	char cPackid[9];
	
	struct lv_wbctl_c  g_lv_wbctl;
	memset(cSts,0,sizeof(cSts));
	memset(cPackday,'\0',sizeof(cPackday));
	memset(cPackid,'\0',sizeof(cPackid));
	memset(&g_lv_wbctl,0x00,sizeof(struct lv_wbctl_c));
	
	vtcp_log("sp9403 开始！[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_fd_data("0690",cSts);
	get_fd_data("0280",cPackday);
	get_fd_data("0290",cPackid);
	vtcp_log("[%s,%d]cPackday=[%s],packid=[%s]",__FILE__,__LINE__,cPackday,cPackid);
	
	g_lv_wbctl.pack_date = apatoi(cPackday,8);
	memcpy(g_lv_wbctl.packid,cPackid,sizeof(g_lv_wbctl.packid)-1);
	
	ret=Lv_wbctl_Sel(g_pub_tx.reply,&g_lv_wbctl,"pack_date='%s' and packid=%s",cPackday,cPackid);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此小额往包交易");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询小额往包交易基本信息出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__); 
	set_zd_data("0520",g_lv_wbctl.cash_qs_no);
	set_zd_data("0690",g_lv_wbctl.stat);
	set_zd_long("0540",g_lv_wbctl.dtlcnt);
	set_zd_double("0430",g_lv_wbctl.totamt);
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
