/*******************************************************************************
 * ��������: sp8138.c
 * ��������: �ӿ�У��
 * ������Ա: �����
 * ����ʱ��: 2014-06-07
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
	
	/**��ʼ��g_pub_tx�ṹ**/
	pub_base_sysinit();
	
	get_zd_data("0920", cTwTel);/**�Է���Ա**/
	
	iRet = sql_count("note_mst", "tel='%s' and note_type in ('020', '024') and sts='0'", cTwTel);
	if (0 > iRet)
	{
		sprintf(acErrMsg, "�������ݿ�[note_mst]����[%d]!", iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT03");
		goto ErrExit;
	}
	else if (0 < iRet)
	{
		sprintf(acErrMsg, "�ӿ�ʱ��������û���忨!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "A097");
		goto ErrExit;
	}
	
	iRet = sql_count("vtm_note_mst", "tw_tel='%s' and ind='2'", cTwTel);
	if (0 > iRet)
	{
		sprintf(acErrMsg, "�������ݿ�[vtm_note_mst]����[%d]!", iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT03");
		goto ErrExit;
	}
	else if (0 < iRet)
	{
		sprintf(acErrMsg, "VTM��Ա����δ���յĿ�!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "A098");
		goto ErrExit;
	}
	
	iRet = sql_count("vtm_note_mst", "tel='%s' and tw_tel='%s' and ind='2'", cTwTel, g_pub_tx.tel);
	if (0 > iRet)
	{
		sprintf(acErrMsg, "�������ݿ�[vtm_note_mst]����[%d]!", iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT03");
		goto ErrExit;
	}
	else if (0 < iRet)
	{
		sprintf(acErrMsg, "�ù�Ա����δ���յĿ�!");
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