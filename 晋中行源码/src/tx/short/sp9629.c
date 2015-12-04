/*******************************************************************************
 * ��������: sp9629.c
 * ��������: VTM��������ʱ�ж϶����˺���ת���˺��Ƿ�Ϊͬһ�ͻ�����
 * ������Ա: �����
 * ����ʱ��: 2013-12-25
 * insert into tx_def values('9629', 'VTM���������˺ż��', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000', '0', '4', '0');
 * insert into tx_flow_def values('9629', '0', '9629', '#$');
 * insert into tx_sub_def values('9629', 'VTM���������˺ż��', 'sp9629', '0', '0');
 ******************************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"

sp9629()
{
	int iRet = 0;
	char cAcNo[25];
	char cAcNoOth[25];
	struct mdm_ac_rel_c stMdm_ac_rel;
	struct mdm_ac_rel_c stMdm_ac_rel_oth;
	
	memset(cAcNo, 0x00, sizeof(cAcNo));
	memset(cAcNoOth, 0x00, sizeof(cAcNoOth));
	memset(&stMdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&stMdm_ac_rel_oth, 0x00, sizeof(struct mdm_ac_rel_c));
	
	/**������ʼ��**/
	get_zd_data("0300", cAcNo);
	pub_base_strpack(cAcNo);
	get_zd_data("0310", cAcNoOth);
	pub_base_strpack(cAcNoOth);
	
	/**���˺Ž����˻����ѯ**/
	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &stMdm_ac_rel, "ac_no='%s'", cAcNo);
	if (100 == iRet)
	{
		sprintf(acErrMsg, "�����˺Ų�����[%s]", cAcNo);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT01");
		goto ErrExit;
	}
	else if (iRet)
	{
		sprintf(acErrMsg, "SELECT mdm_ac_rel error,error code=[%d]", iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "W011");
		goto ErrExit;
	}
	if (0 == strcmp(stMdm_ac_rel.note_sts, "*"))
	{
		sprintf(acErrMsg, "�����˺��Ѿ�����!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "M012");
		goto ErrExit;
	}
	else if (0 == strcmp(stMdm_ac_rel.note_sts, "2"))
	{
		sprintf(acErrMsg, "�����˺��ѹ�ʧ��֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "P171");
		goto ErrExit;
	}
	else if (0 == strcmp(stMdm_ac_rel.note_sts, "3"))
	{
		sprintf(acErrMsg, "�����˺��ѻ�֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "M011");
		goto ErrExit;
	}
	else if (0 == strcmp(stMdm_ac_rel.note_sts, "4"))
	{
		sprintf(acErrMsg, "�����˺���������!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "M017");
		goto ErrExit;
	}
	else if (0 == strcmp(stMdm_ac_rel.note_sts, "5"))
	{
		sprintf(acErrMsg, "�����˺���������!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "M019");
		goto ErrExit;
	}
	else if (0 != strcmp(stMdm_ac_rel.note_sts, "0") && 0 != strcmp(stMdm_ac_rel.note_sts, "1"))
	{
		sprintf(acErrMsg, "note_sts=[%s],�����˺ŵĽ���״̬�쳣!", stMdm_ac_rel.note_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply, "M013");
		goto ErrExit;
	}
	
	/**ת���˺Ž����˻����ѯ**/
	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &stMdm_ac_rel_oth, "ac_no='%s'", cAcNoOth);
	if (100 == iRet)
	{
		sprintf(acErrMsg, "ת���˺Ų�����[%s]", cAcNoOth);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT21");
		goto ErrExit;
	}
	else if (iRet)
	{
		sprintf(acErrMsg, "SELECT mdm_ac_rel error,error code=[%d]", iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "W011");
		goto ErrExit;
	}
	if (0 == strcmp(stMdm_ac_rel_oth.note_sts, "*"))
	{
		sprintf(acErrMsg, "ת���˺��Ѿ�����!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "M112");
		goto ErrExit;
	}
	else if (0 == strcmp(stMdm_ac_rel_oth.note_sts, "2"))
	{
		sprintf(acErrMsg, "ת���˺��ѹ�ʧ��֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "P271");
		goto ErrExit;
	}
	else if (0 == strcmp(stMdm_ac_rel_oth.note_sts, "3"))
	{
		sprintf(acErrMsg, "ת���˺��ѻ�֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "M111");
		goto ErrExit;
	}
	else if (0 == strcmp(stMdm_ac_rel_oth.note_sts, "4"))
	{
		sprintf(acErrMsg, "ת���˺���������!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "M117");
		goto ErrExit;
	}
	else if (0 == strcmp(stMdm_ac_rel_oth.note_sts, "5"))
	{
		sprintf(acErrMsg, "ת���˺���������!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "M119");
		goto ErrExit;
	}
	else if (0 != strcmp(stMdm_ac_rel_oth.note_sts, "0") && 0 != strcmp(stMdm_ac_rel_oth.note_sts, "1"))
	{
		sprintf(acErrMsg, "note_sts=[%s],ת���˺ŵĽ���״̬�쳣!", stMdm_ac_rel_oth.note_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply, "M113");
		goto ErrExit;
	}
	
	/**�Ƚ����˺���ת���˺ŵ�֤��������֤�������Ƿ�һ��**/
	if ((0 != memcmp(stMdm_ac_rel.id_type, stMdm_ac_rel_oth.id_type, strlen(stMdm_ac_rel.id_type)))
		|| (0 != memcmp(stMdm_ac_rel.id_no, stMdm_ac_rel_oth.id_no, strlen(stMdm_ac_rel.id_no))))
	{
		sprintf(acErrMsg, "�˻�[%s]���˻�[%s]��֤��������֤�����벻һ��!", cAcNo, cAcNoOth);
		WRITEMSG
		strcpy(g_pub_tx.reply, "M120");
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}