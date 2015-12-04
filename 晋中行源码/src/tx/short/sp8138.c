/*******************************************************************************
 * 程序名称: sp8138.c
 * 程序描述: 加卡校验
 * 开发人员: 孙朋君
 * 创建时间: 2014-06-07
 ******************************************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int sp8138()
{
	int iRet = 0;
	char cTwTel[5];
	
	memset(cTwTel, 0x00, sizeof(cTwTel));
	
	/**初始化g_pub_tx结构**/
	pub_base_sysinit();
	
	get_zd_data("0920", cTwTel);/**对方柜员**/
	
	iRet = sql_count("note_mst", "tel='%s' and note_type in ('020', '024') and sts='0'", cTwTel);
	if (0 > iRet)
	{
		sprintf(acErrMsg, "操作数据库[note_mst]错误[%d]!", iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT03");
		goto ErrExit;
	}
	else if (0 < iRet)
	{
		sprintf(acErrMsg, "加卡时自助机具没有清卡!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "A097");
		goto ErrExit;
	}
	
	iRet = sql_count("vtm_note_mst", "tw_tel='%s' and ind='2'", cTwTel);
	if (0 > iRet)
	{
		sprintf(acErrMsg, "操作数据库[vtm_note_mst]错误[%d]!", iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT03");
		goto ErrExit;
	}
	else if (0 < iRet)
	{
		sprintf(acErrMsg, "VTM柜员存在未接收的卡!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "A098");
		goto ErrExit;
	}
	
	iRet = sql_count("vtm_note_mst", "tel='%s' and tw_tel='%s' and ind='2'", cTwTel, g_pub_tx.tel);
	if (0 > iRet)
	{
		sprintf(acErrMsg, "操作数据库[vtm_note_mst]错误[%d]!", iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT03");
		goto ErrExit;
	}
	else if (0 < iRet)
	{
		sprintf(acErrMsg, "该柜员存在未接收的卡!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "A096");
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}