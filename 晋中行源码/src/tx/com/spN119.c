/*******************************************************************************
 * 程序名称: spN119.c
 * 程序描述: VTM卡回收
 * 开发人员: 孙朋君
 * 创建时间: 2014-06-10
 ******************************************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "note_mst_c.h"
#include "note_parm_c.h"
#include "note_mst_hst_c.h"

#define NOTESECTIONNUM 3/**支持的凭证号段数**/

int spN119()
{
	int iRet = 0;
	long lBegNoteNo = 0;
	long lEndNoteNo = 0;
	long lNoteCnt = 0;
	char cTwTel[5];
	char cAllNoteNo[1024];
	char cBegNoteNo[17];
	char cEndNoteNo[17];
	struct com_item_c stCom_item;
	struct note_parm_c stNote_parm;
	
	memset(cTwTel, 0x00, sizeof(cTwTel));
	memset(cAllNoteNo, 0x00, sizeof(cAllNoteNo));
	memset(cBegNoteNo, 0x00, sizeof(cBegNoteNo));
	memset(cEndNoteNo, 0x00, sizeof(cEndNoteNo));
	memset(&stCom_item, 0x00, sizeof(struct com_item_c));
	memset(&stNote_parm, 0x00, sizeof(struct note_parm_c));
	
	/**初始化g_pub_tx结构**/
	pub_base_sysinit();
	
	get_zd_data("0920", cTwTel);/*对方柜员*/
	get_zd_data("0890", g_pub_tx.note_type);/**凭证类型**/
	get_zd_data("1230", cAllNoteNo);/**凭证号码**/
	
	vtcp_log("%s,%d 对方柜员 cTwTelel=[%s]\n", __FILE__, __LINE__, cTwTel);
	vtcp_log("%s,%d 凭证类型 note_type=[%s]\n", __FILE__, __LINE__, g_pub_tx.note_type);
	vtcp_log("%s,%d 凭证号码 cAllNoteNo=[%s]\n", __FILE__, __LINE__, cAllNoteNo);
	
	iRet = Note_parm_Sel(g_pub_tx.reply, &stNote_parm, "note_type='%s'", g_pub_tx.note_type);
	if (iRet)
	{
		vtcp_log("%s,%d 查询凭证信息错误 note_type=[%s]\n", __FILE__, __LINE__, g_pub_tx.note_type);
		goto ErrExit;
	}
	
	iRet = pub_base_item_inf(stNote_parm.acc_hrt, &stCom_item);
	if(iRet)
	{
		vtcp_log("%s,%d 根据凭证取对应科目信息错误 note_type=[%s],acc_hrt=[%s],acc_no=[%s]\n",
							__FILE__, __LINE__, g_pub_tx.note_type, stNote_parm.acc_hrt, stCom_item.acc_no);
		goto ErrExit;
	}
	strcpy(g_pub_tx.ac_no, stCom_item.acc_no);
	
	/**本方权限检查**/
	if (pub_com_NoteAuthChk(g_pub_tx.tx_br_no, g_pub_tx.tel, 301, 0))
	{
		vtcp_log("本方柜员权限错误!");
		goto ErrExit;
	}
	
	for (int i = 0; i < NOTESECTIONNUM; i++)
	{
		memcpy(cBegNoteNo, cAllNoteNo + 2 * i * 16, 16);
		memcpy(cEndNoteNo, cAllNoteNo + (2 * i + 1) * 16, 16);
		
		/**压缩空格**/
		zip_space(cBegNoteNo);
		zip_space(cEndNoteNo);
		
		if (0 != strlen(cBegNoteNo) && 0 != strlen(cEndNoteNo))
		{
			set_zd_data("0580", cBegNoteNo);
			set_zd_data("0590", cEndNoteNo);
			get_zd_long("0580", &lBegNoteNo);
			get_zd_long("0590", &lEndNoteNo);
			lNoteCnt = lEndNoteNo - lBegNoteNo + 1;
			if(1 <= lNoteCnt && 0 < atoi(cBegNoteNo))
			{
				strcpy(g_pub_tx.beg_note_no, cBegNoteNo);
				strcpy(g_pub_tx.end_note_no, cEndNoteNo);
				g_pub_tx.tx_amt1 = lNoteCnt;
				if (-1 == note_back(cTwTel))
				{
					sprintf(acErrMsg, "凭证回收出错");
					WRITEMSG
					goto ErrExit;
				}
				vtcp_log("%s,%d lNoteCnt [%d][%ld][%s][%s]\n", __FILE__, __LINE__, i, lNoteCnt, cBegNoteNo, cEndNoteNo);
			}
		}
	}
	
	iRet = sql_execute("update vtm_note_mst set ind='A' where tel='%s' and tw_tel='%s' and ind='2'", g_pub_tx.tel, cTwTel);
	if (iRet)
	{
		sprintf(acErrMsg, "修改登记薄状态失败!", iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D105");
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
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

int note_back(char *cTwTel)
{
	int iRet = 0;
	int iCnt = 0;
	char cNoteAct[2];/**凭证行为:2-付出;3-收入*/
	struct note_mst_c stNote_mst;
	struct note_mst_c stNote_mst_o;
	struct note_mst_hst_c stNote_mst_hst;
	
	memset(cNoteAct, 0x00, sizeof(cNoteAct));
	memset(&stNote_mst, 0x00, sizeof(struct note_mst_c));
	memset(&stNote_mst_o, 0x00, sizeof(struct note_mst_c));
	memset(&stNote_mst_hst, 0x00, sizeof(struct note_mst_hst_c));
	
	/**凭证有效性检查:是否存在,是否处于使用状态**/
	iRet = pub_com_ChkNoteUse(g_pub_tx.note_type);
	if (iRet)
	{
		sprintf(acErrMsg, "检查凭证是否存在，是否启用错[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**凭证起始号码、终止号码冠子头检查**/
	iRet = pub_com_ChkNoteHead(g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel);
	if (iRet)
	{
		sprintf(acErrMsg, "检查凭证起始号码冠子头错[%d]", iRet);
		WRITEMSG
		return -1;
	}
	iRet = pub_com_ChkNoteHead(g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel);
	if (iRet)
	{
		sprintf(acErrMsg, "检查凭证终止号码冠子头错[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	strcpy(cNoteAct, "2");
	strcpy(g_pub_tx.brf, "凭证回收");
	strcpy(g_pub_tx.add_ind, "1");
	
	/**查找柜员凭证明细 根据交易日期.凭证种类.起始凭证.终止凭证.对方柜员.凭证行为**/
	vtcp_log("[%s][%d]--[%d][%s][%s][%s][%s][%s]\n", __FILE__, __LINE__, g_pub_tx.tx_date, g_pub_tx.note_type,
						g_pub_tx.beg_note_no, g_pub_tx.end_note_no, g_pub_tx.tx_br_no, cTwTel);
	iRet = pub_com_GetNoteHst(g_pub_tx.tx_date, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no,
					g_pub_tx.tx_br_no, cTwTel, cNoteAct, &stNote_mst_hst);
	if (iRet)
	{
		sprintf(acErrMsg, "查询柜员凭证明细错[%d]", iRet);
		WRITEMSG
		iRet = Note_mst_hst_Sel(g_pub_tx.reply, &stNote_mst_hst,
						"tx_date=%d and note_type='%s' and beg_note_no='%s' and end_note_no='%s' and br_no='%s' and sts_ind='%s'",
						g_pub_tx.tx_date, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no, g_pub_tx.tx_br_no, cNoteAct);
		if (iRet)
		{
			sprintf(acErrMsg, "查询柜员凭证明细错[%d]", iRet);
			WRITEMSG
			return -1;
		}
		vtcp_log("[%s][%d] stNote_mst_hst_tel[%s] stNote_mst_hst_tw_tel[%s] g_pub_tx.tel[%s]\n",
							__FILE__, __LINE__, stNote_mst_hst.tel, stNote_mst_hst.tw_tel, g_pub_tx.tel);
		if (0 != memcmp(stNote_mst_hst.tel, g_pub_tx.tel, sizeof(stNote_mst_hst.tel) - 1)
			&& 0 != memcmp(stNote_mst_hst.tw_tel, g_pub_tx.tel, sizeof(stNote_mst_hst.tw_tel) - 1))
		{
			sprintf(acErrMsg, "查询柜员凭证明细错");
			WRITEMSG
			return -1;
		}
		iCnt = sql_count("note_mst_hst", "tx_date=%d and note_type='%s' and beg_note_no='%s' and end_note_no='%s' and br_no='%s' and sts_ind='%s'",
						g_pub_tx.tx_date, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no, g_pub_tx.tx_br_no, cNoteAct);
		if (1 == iCnt)
		{
			iRet = pub_com_GetNoteHst(g_pub_tx.tx_date, g_pub_tx.note_type, g_pub_tx.beg_note_no,
							g_pub_tx.end_note_no, g_pub_tx.tx_br_no, stNote_mst_hst.tw_tel, cNoteAct, &stNote_mst_hst);
			if (iRet)
			{
				sprintf(acErrMsg, "查询柜员凭证明细错[%d]", iRet);
				WRITEMSG
				return -1;
			}
		}
		else
		{
			sprintf(acErrMsg, "查询柜员凭证明细错[%d]", iCnt);
			WRITEMSG
			return -1;
		}
	}
	vtcp_log("[%s][%d] stNote_mst_hst_tel[%s] stNote_mst_hst_tw_tel[%s] g_pub_tx.tel[%s]\n",
						__FILE__, __LINE__, stNote_mst_hst.tel, stNote_mst_hst.tw_tel, g_pub_tx.tel);
	
	strcpy(cNoteAct, "3");
	
	/**检查对方操作员是否拥有这批凭证**/
	strcpy(stNote_mst_o.tel , stNote_mst_hst.tel);
	strcpy(stNote_mst_o.br_no , stNote_mst_hst.br_no);
	strcpy(stNote_mst_o.note_type , stNote_mst_hst.note_type);
	strcpy(stNote_mst_o.beg_note_no , stNote_mst_hst.beg_note_no);
	strcpy(stNote_mst_o.end_note_no , stNote_mst_hst.end_note_no);
	iRet = pub_com_ChkNote(cNoteAct, stNote_mst_o);
	if (iRet)
	{
		sprintf(acErrMsg, "检查柜员凭证错[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**登记柜员凭证台帐（并段处理）**/
	iRet = pub_com_RegTelNote(g_pub_tx.tel, g_pub_tx.tx_br_no,
					cNoteAct, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no);
	if (iRet)
	{
		sprintf(acErrMsg, "登记柜员凭证台帐（并段处理）错[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**删除/修改下发柜员的凭证**/
	iRet = pub_com_NoteDel("2", stNote_mst_o);
	if (iRet)
	{
		sprintf(acErrMsg, "删除/修改下发柜员的凭证错[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**登记柜员凭证明细**/
	strcpy(stNote_mst.tel, g_pub_tx.tel);
	strcpy(stNote_mst.br_no, g_pub_tx.tx_br_no);
	strcpy(stNote_mst.note_type, g_pub_tx.note_type);
	strcpy(stNote_mst.beg_note_no, g_pub_tx.beg_note_no);
	strcpy(stNote_mst.end_note_no, g_pub_tx.end_note_no);
	stNote_mst.cnt = (long)g_pub_tx.tx_amt1;
	iRet = pub_com_RegNoteHst(cNoteAct, stNote_mst, stNote_mst_hst.br_no, stNote_mst_hst.tel);
	if (-1 == iRet)
	{
		sprintf(acErrMsg, "登记柜员凭证明细错[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**登记凭证最小号**/
	iRet = pub_com_RegNoteMin(g_pub_tx.tx_br_no, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel, "I");
	if (iRet)
	{
		sprintf(acErrMsg, "登记凭证最小号错[%d]", iRet);
		WRITEMSG
		return -1;
	}

	/**写交易日志8*/
	iRet = pub_ins_trace_log();
	if (iRet)
	{
		sprintf(acErrMsg, "记流水日志错[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	return 0;
}