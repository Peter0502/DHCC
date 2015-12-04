/*******************************************************************************
 * 程序名称: sp8137.c
 * 程序描述: VTM清卡信息回显
 * 开发人员: 孙朋君
 * 创建时间: 2014-06-07
 ******************************************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "vtm_note_mst_c.h"

int sp8137()
{
	int iRet = 0;
	int iCnt = 0;
	long iNoteCnt = 0;
	long lBeg_note_no = 0;
	long lEnd_note_no = 0;
	char cFlag[2];
	char cTwTel[5];
	char cTemp[512];
	struct vtm_note_mst_c stVtm_note_mst;
	
	memset(cFlag, 0x00, sizeof(cFlag));
	memset(cTwTel, 0x00, sizeof(cTwTel));
	memset(cTemp, 0x00, sizeof(cTemp));
	memset(&stVtm_note_mst, 0x00, sizeof(struct vtm_note_mst_c));
	
	/**初始化g_pub_tx结构**/
	pub_base_sysinit();
	
	get_zd_data("0670", cFlag);
	get_zd_data("0920", cTwTel);/**对方柜员**/
	get_zd_data("0890", g_pub_tx.note_type);/**凭证类型**/
	
	if (0 == memcmp(cFlag, "3", 1))
	{
		iRet = Vtm_note_mst_Sel(g_pub_tx.reply, &stVtm_note_mst, "tel='%s' and tw_tel='%s' and ind='2'", g_pub_tx.tel, cTwTel);
	}
	else
	{
		iRet = Vtm_note_mst_Sel(g_pub_tx.reply, &stVtm_note_mst, "tel='%s' and tw_tel='%s' and ind='2'", cTwTel, g_pub_tx.tel);
	}
	if (iRet && 100 != iRet)
	{
		sprintf(acErrMsg, "操作数据库出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	else if (100 == iRet)
	{
		sprintf(acErrMsg, "没有待清的卡!!");
		WRITEMSG
		if (0 == memcmp(cFlag, "3", 1))
		{
			strcpy(g_pub_tx.reply, "A095");
		}
		else
		{
			strcpy(g_pub_tx.reply, "A099");
		}
		goto ErrExit;
	}
	
	sprintf(cTemp, "%1d", 0);
	if (0 != strlen(stVtm_note_mst.beg_note_no1) && 0 != strlen(stVtm_note_mst.end_note_no1))
	{
		iCnt++;
		strcat(cTemp, stVtm_note_mst.beg_note_no1);
		strcat(cTemp, stVtm_note_mst.end_note_no1);
		set_zd_data("0580", stVtm_note_mst.beg_note_no1);
		set_zd_data("0590", stVtm_note_mst.end_note_no1);
		get_zd_long("0580", &lBeg_note_no);
		get_zd_long("0590", &lEnd_note_no);
		iNoteCnt = iNoteCnt + (lEnd_note_no - lBeg_note_no + 1);
	}
	if (0 != strlen(stVtm_note_mst.beg_note_no2) && 0 != strlen(stVtm_note_mst.end_note_no2))
	{
		iCnt++;
		strcat(cTemp, stVtm_note_mst.beg_note_no2);
		strcat(cTemp, stVtm_note_mst.end_note_no2);
		set_zd_data("0580", stVtm_note_mst.beg_note_no2);
		set_zd_data("0590", stVtm_note_mst.end_note_no2);
		get_zd_long("0580", &lBeg_note_no);
		get_zd_long("0590", &lEnd_note_no);
		iNoteCnt = iNoteCnt + (lEnd_note_no - lBeg_note_no + 1);
	}
	if (0 != strlen(stVtm_note_mst.beg_note_no3) && 0 != strlen(stVtm_note_mst.end_note_no3))
	{
		iCnt++;
		strcat(cTemp, stVtm_note_mst.beg_note_no3);
		strcat(cTemp, stVtm_note_mst.end_note_no3);
		set_zd_data("0580", stVtm_note_mst.beg_note_no3);
		set_zd_data("0590", stVtm_note_mst.end_note_no3);
		get_zd_long("0580", &lBeg_note_no);
		get_zd_long("0590", &lEnd_note_no);
		iNoteCnt = iNoteCnt + (lEnd_note_no - lBeg_note_no + 1);
	}
	
	set_zd_data("1230", cTemp);
	set_zd_long("0600", iNoteCnt);
	
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