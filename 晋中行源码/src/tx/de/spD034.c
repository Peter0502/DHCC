/***************************************************************
* �� �� ��:     spD034.c
* ����������	���ʺŴ�ȡ���
*
* ��    ��:     houyi
* ������ڣ�    20101019
*
*˵�����˽���ʱspD033.c�ĸ�������Ӧ�����Ķ����˻��Ĳ���
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
spD034()
{
	int             ret = 0;
	int             mcnt = 0, zcnt = 0;
	char            add_ind[2];
	char            opt_ind[2];
	long            sub_ac_seqn;

	memset(add_ind,0x00,sizeof(add_ind));
	memset(opt_ind,0x00,sizeof(opt_ind));
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	get_zd_data("0300", g_pub_tx.ac_no);	/*** ĸ�ʺ� ***/
	get_zd_data("0720",opt_ind);
	pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_long("0440",&sub_ac_seqn);
	get_zd_data("0630",g_pub_tx.brf);
	get_zd_data("0890",g_pub_tx.note_type);	
	get_zd_data("0580",g_pub_tx.beg_note_no);
	get_zd_data("0580",g_pub_tx.end_note_no);
	/* g_pub_tx.sub_ac_seqn=sub_ac_seqn; */

	sprintf(g_pub_tx.ac_no1,"%s-%d",g_pub_tx.ac_no,sub_ac_seqn);
	vtcp_log("���˻�: [%s] [%s] [%d]",g_pub_tx.ac_no1,__FILE__,__LINE__);
	sprintf(acErrMsg, "���˻�--[%s][%ld]", g_pub_tx.ac_no,sub_ac_seqn);
	WRITEMSG
	/* ��֤һ�����˻� */
	ret =Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if(ret)
	{
			sprintf(acErrMsg, "�������˻�--[%s][%ld]", g_pub_tx.ac_no,sub_ac_seqn);
			WRITEMSG
			goto ErrExit;
	}
	get_zd_data("0690", add_ind);	/*** ������־ ***/
	if (add_ind[0] == '1') {
		strcpy(g_pub_tx.add_ind, "1");
		g_pub_tx.svc_ind = 1201;
	} else if (add_ind[0] == '2') {
		strcpy(g_pub_tx.add_ind, "0");
		g_pub_tx.svc_ind = 1201;
	} else if (add_ind[0] == '3') {
		strcpy(g_pub_tx.add_ind, "3");
		strcpy(g_pub_tx.brf, "���ʺ�����");
		g_pub_tx.svc_ind = 1202;
	} else {
		sprintf(acErrMsg, "���ȡ���־����![%s]", add_ind);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P106");
		goto ErrExit;
	}
	get_zd_data("0680", g_pub_tx.ct_ind);	/*** ��ת��־ ***/
	if (g_pub_tx.ct_ind[0] != '1') {
		g_pub_tx.ct_ind[0] = '2';
	}
	get_zd_double("0400", &g_pub_tx.tx_amt1);
	/*-- modify begin reason: 44���Ѿ������˻���ŵĽ��� �������ٽ���ĸ�˺��������� by wzs 20120215 --*/
	/* get_zd_long("0440", &g_pub_tx.erro_date); */	/* ĸ�ʺ��������� */
	get_zd_long("0450",&g_pub_tx.erro_date); /* ����45����� ĸ�˺��������� */
	/*-- modify end by wzs 20110215 --*/
	if (g_pub_tx.erro_date == 0) {
		g_pub_tx.erro_date = g_pub_tx.tx_date;
	}
	strcpy(g_pub_tx.cur_no, "01");	/*** Ĭ������� ***/

	/* add by rob at 20051207 ���ĸ�ʺ��Ƿ����ñ�ҵ�� */
	memset(add_ind, 0x0, sizeof(add_ind));
	if (g_pub_tx.add_ind[0] == '3') {
		strcpy(add_ind, "0");	/* �� */
	} else {
		strcpy(add_ind, g_pub_tx.add_ind);
	}
	ret = pub_base_sub_ac_trance(sub_ac_seqn);
	if (ret) {
		sprintf(acErrMsg, "���ʺŴ�ȡ�����ش���!");
		WRITEMSG
			goto ErrExit;
	}
	/* by Wang Yongwei 20101026 ����ת��֧Ʊ������֧Ʊ */
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;
	ret=pub_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn, g_pub_tx.note_type,g_pub_tx.beg_note_no, g_pub_tx.end_note_no,g_pub_tx.tel);
	if(opt_ind[0]=='2')
	{
		goto OkExit;
	}
OkExit1:
	strcpy(g_pub_tx.reply, "0001");
	sprintf(acErrMsg, "���ʺŴ�ȡ��ɹ�![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "���ʺŴ�ȡ��ɹ�![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "���ʺŴ�ȡ��ʧ��![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
