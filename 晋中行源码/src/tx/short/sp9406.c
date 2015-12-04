/*****************************************************
* 文 件 名:  sp9406.c
* 功能描述： 查询大小额专用内部帐户
* 作    者:  xyy
* 完成日期： 2006-10-18 20:07
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lv_fd123.h"
#include "com_parm_c.h"
int sp9406()
{
	int ret=0,i;
	char cActno[21];
	char cSeqn[5];
	char cCode[6];
	struct com_parm_c  com_parm;
	memset(cCode ,0,sizeof(cCode));
	memset(cActno,0,sizeof(cActno));
	memset(cSeqn,0,sizeof(cSeqn));
	memset(&com_parm,0x00,sizeof(struct com_parm_c));
	vtcp_log("sp9406 开始！[%s][%d]",__FILE__,__LINE__);
 	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	get_zd_data("0910",cCode);
	get_zd_data("0480",cSeqn);
	pub_base_strpack(cCode);
	pub_base_strpack(cSeqn);
	if(strlen(cCode)==0)
		get_fd_data(DC_TX_BR_NO,cCode);
	ret = Com_parm_Sel(g_pub_tx.reply,&com_parm, "parm_code='%s' and parm_seqn=%d ",cCode,atoi(cSeqn));
	vtcp_log("sp9406 开始！[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"没有该机构的大小额专用内部帐户");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O043" );
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询该机构的大小额专用内部帐户错误[%d]",ret);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("[%s][%d]查询成功!ac_seqn=[%d]",__FILE__,__LINE__,com_parm.parm_seqn);
  vtcp_log("[%s][%d]val====[%s]",__FILE__,__LINE__,com_parm.val);
  memcpy(cActno,com_parm.val,sizeof(cActno)-1);
  set_zd_data("1201",cActno);
  vtcp_log("[%s][%d]返回内部帐号的帐号为[%s]",__FILE__,__LINE__,cActno);

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

