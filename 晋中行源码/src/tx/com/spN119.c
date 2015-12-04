/*******************************************************************************
 * ��������: spN119.c
 * ��������: VTM������
 * ������Ա: �����
 * ����ʱ��: 2014-06-10
 ******************************************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "note_mst_c.h"
#include "note_parm_c.h"
#include "note_mst_hst_c.h"

#define NOTESECTIONNUM 3/**֧�ֵ�ƾ֤�Ŷ���**/

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
	
	/**��ʼ��g_pub_tx�ṹ**/
	pub_base_sysinit();
	
	get_zd_data("0920", cTwTel);/*�Է���Ա*/
	get_zd_data("0890", g_pub_tx.note_type);/**ƾ֤����**/
	get_zd_data("1230", cAllNoteNo);/**ƾ֤����**/
	
	vtcp_log("%s,%d �Է���Ա cTwTelel=[%s]\n", __FILE__, __LINE__, cTwTel);
	vtcp_log("%s,%d ƾ֤���� note_type=[%s]\n", __FILE__, __LINE__, g_pub_tx.note_type);
	vtcp_log("%s,%d ƾ֤���� cAllNoteNo=[%s]\n", __FILE__, __LINE__, cAllNoteNo);
	
	iRet = Note_parm_Sel(g_pub_tx.reply, &stNote_parm, "note_type='%s'", g_pub_tx.note_type);
	if (iRet)
	{
		vtcp_log("%s,%d ��ѯƾ֤��Ϣ���� note_type=[%s]\n", __FILE__, __LINE__, g_pub_tx.note_type);
		goto ErrExit;
	}
	
	iRet = pub_base_item_inf(stNote_parm.acc_hrt, &stCom_item);
	if(iRet)
	{
		vtcp_log("%s,%d ����ƾ֤ȡ��Ӧ��Ŀ��Ϣ���� note_type=[%s],acc_hrt=[%s],acc_no=[%s]\n",
							__FILE__, __LINE__, g_pub_tx.note_type, stNote_parm.acc_hrt, stCom_item.acc_no);
		goto ErrExit;
	}
	strcpy(g_pub_tx.ac_no, stCom_item.acc_no);
	
	/**����Ȩ�޼��**/
	if (pub_com_NoteAuthChk(g_pub_tx.tx_br_no, g_pub_tx.tel, 301, 0))
	{
		vtcp_log("������ԱȨ�޴���!");
		goto ErrExit;
	}
	
	for (int i = 0; i < NOTESECTIONNUM; i++)
	{
		memcpy(cBegNoteNo, cAllNoteNo + 2 * i * 16, 16);
		memcpy(cEndNoteNo, cAllNoteNo + (2 * i + 1) * 16, 16);
		
		/**ѹ���ո�**/
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
					sprintf(acErrMsg, "ƾ֤���ճ���");
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
		sprintf(acErrMsg, "�޸ĵǼǱ�״̬ʧ��!", iRet);
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
	char cNoteAct[2];/**ƾ֤��Ϊ:2-����;3-����*/
	struct note_mst_c stNote_mst;
	struct note_mst_c stNote_mst_o;
	struct note_mst_hst_c stNote_mst_hst;
	
	memset(cNoteAct, 0x00, sizeof(cNoteAct));
	memset(&stNote_mst, 0x00, sizeof(struct note_mst_c));
	memset(&stNote_mst_o, 0x00, sizeof(struct note_mst_c));
	memset(&stNote_mst_hst, 0x00, sizeof(struct note_mst_hst_c));
	
	/**ƾ֤��Ч�Լ��:�Ƿ����,�Ƿ���ʹ��״̬**/
	iRet = pub_com_ChkNoteUse(g_pub_tx.note_type);
	if (iRet)
	{
		sprintf(acErrMsg, "���ƾ֤�Ƿ���ڣ��Ƿ����ô�[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**ƾ֤��ʼ���롢��ֹ�������ͷ���**/
	iRet = pub_com_ChkNoteHead(g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel);
	if (iRet)
	{
		sprintf(acErrMsg, "���ƾ֤��ʼ�������ͷ��[%d]", iRet);
		WRITEMSG
		return -1;
	}
	iRet = pub_com_ChkNoteHead(g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel);
	if (iRet)
	{
		sprintf(acErrMsg, "���ƾ֤��ֹ�������ͷ��[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	strcpy(cNoteAct, "2");
	strcpy(g_pub_tx.brf, "ƾ֤����");
	strcpy(g_pub_tx.add_ind, "1");
	
	/**���ҹ�Աƾ֤��ϸ ���ݽ�������.ƾ֤����.��ʼƾ֤.��ֹƾ֤.�Է���Ա.ƾ֤��Ϊ**/
	vtcp_log("[%s][%d]--[%d][%s][%s][%s][%s][%s]\n", __FILE__, __LINE__, g_pub_tx.tx_date, g_pub_tx.note_type,
						g_pub_tx.beg_note_no, g_pub_tx.end_note_no, g_pub_tx.tx_br_no, cTwTel);
	iRet = pub_com_GetNoteHst(g_pub_tx.tx_date, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no,
					g_pub_tx.tx_br_no, cTwTel, cNoteAct, &stNote_mst_hst);
	if (iRet)
	{
		sprintf(acErrMsg, "��ѯ��Աƾ֤��ϸ��[%d]", iRet);
		WRITEMSG
		iRet = Note_mst_hst_Sel(g_pub_tx.reply, &stNote_mst_hst,
						"tx_date=%d and note_type='%s' and beg_note_no='%s' and end_note_no='%s' and br_no='%s' and sts_ind='%s'",
						g_pub_tx.tx_date, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no, g_pub_tx.tx_br_no, cNoteAct);
		if (iRet)
		{
			sprintf(acErrMsg, "��ѯ��Աƾ֤��ϸ��[%d]", iRet);
			WRITEMSG
			return -1;
		}
		vtcp_log("[%s][%d] stNote_mst_hst_tel[%s] stNote_mst_hst_tw_tel[%s] g_pub_tx.tel[%s]\n",
							__FILE__, __LINE__, stNote_mst_hst.tel, stNote_mst_hst.tw_tel, g_pub_tx.tel);
		if (0 != memcmp(stNote_mst_hst.tel, g_pub_tx.tel, sizeof(stNote_mst_hst.tel) - 1)
			&& 0 != memcmp(stNote_mst_hst.tw_tel, g_pub_tx.tel, sizeof(stNote_mst_hst.tw_tel) - 1))
		{
			sprintf(acErrMsg, "��ѯ��Աƾ֤��ϸ��");
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
				sprintf(acErrMsg, "��ѯ��Աƾ֤��ϸ��[%d]", iRet);
				WRITEMSG
				return -1;
			}
		}
		else
		{
			sprintf(acErrMsg, "��ѯ��Աƾ֤��ϸ��[%d]", iCnt);
			WRITEMSG
			return -1;
		}
	}
	vtcp_log("[%s][%d] stNote_mst_hst_tel[%s] stNote_mst_hst_tw_tel[%s] g_pub_tx.tel[%s]\n",
						__FILE__, __LINE__, stNote_mst_hst.tel, stNote_mst_hst.tw_tel, g_pub_tx.tel);
	
	strcpy(cNoteAct, "3");
	
	/**���Է�����Ա�Ƿ�ӵ������ƾ֤**/
	strcpy(stNote_mst_o.tel , stNote_mst_hst.tel);
	strcpy(stNote_mst_o.br_no , stNote_mst_hst.br_no);
	strcpy(stNote_mst_o.note_type , stNote_mst_hst.note_type);
	strcpy(stNote_mst_o.beg_note_no , stNote_mst_hst.beg_note_no);
	strcpy(stNote_mst_o.end_note_no , stNote_mst_hst.end_note_no);
	iRet = pub_com_ChkNote(cNoteAct, stNote_mst_o);
	if (iRet)
	{
		sprintf(acErrMsg, "����Աƾ֤��[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**�Ǽǹ�Աƾ̨֤�ʣ����δ���**/
	iRet = pub_com_RegTelNote(g_pub_tx.tel, g_pub_tx.tx_br_no,
					cNoteAct, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no);
	if (iRet)
	{
		sprintf(acErrMsg, "�Ǽǹ�Աƾ̨֤�ʣ����δ�����[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**ɾ��/�޸��·���Ա��ƾ֤**/
	iRet = pub_com_NoteDel("2", stNote_mst_o);
	if (iRet)
	{
		sprintf(acErrMsg, "ɾ��/�޸��·���Ա��ƾ֤��[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**�Ǽǹ�Աƾ֤��ϸ**/
	strcpy(stNote_mst.tel, g_pub_tx.tel);
	strcpy(stNote_mst.br_no, g_pub_tx.tx_br_no);
	strcpy(stNote_mst.note_type, g_pub_tx.note_type);
	strcpy(stNote_mst.beg_note_no, g_pub_tx.beg_note_no);
	strcpy(stNote_mst.end_note_no, g_pub_tx.end_note_no);
	stNote_mst.cnt = (long)g_pub_tx.tx_amt1;
	iRet = pub_com_RegNoteHst(cNoteAct, stNote_mst, stNote_mst_hst.br_no, stNote_mst_hst.tel);
	if (-1 == iRet)
	{
		sprintf(acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	/**�Ǽ�ƾ֤��С��**/
	iRet = pub_com_RegNoteMin(g_pub_tx.tx_br_no, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel, "I");
	if (iRet)
	{
		sprintf(acErrMsg, "�Ǽ�ƾ֤��С�Ŵ�[%d]", iRet);
		WRITEMSG
		return -1;
	}

	/**д������־8*/
	iRet = pub_ins_trace_log();
	if (iRet)
	{
		sprintf(acErrMsg, "����ˮ��־��[%d]", iRet);
		WRITEMSG
		return -1;
	}
	
	return 0;
}