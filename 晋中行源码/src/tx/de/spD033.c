/***************************************************************
* �� �� ��:     spD033.c
* ����������	���ʺŴ�ȡ���
*
* ��    ��:     jack
* ������ڣ�    2003��7��11��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD033()
{
	int             ret = 0;
	int             mcnt = 0, zcnt = 0;
	char            add_ind[2];
	long            sub_ac_seqn;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(add_ind,0x00,sizeof(add_ind));
	get_zd_data("0300", g_pub_tx.ac_no);	/*** ĸ�ʺ� ***/
	pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_long("0780",&sub_ac_seqn);
	get_zd_data("0620",g_pub_tx.brf);
	/* g_pub_tx.sub_ac_seqn=sub_ac_seqn; */


	get_zd_data("0250", g_pub_tx.name);	/*** ���ʺŻ��� ***/
	get_zd_data("0670", add_ind);	/*** ������־ ***/
	sprintf(acErrMsg, "[%s][%d]���˻�[%s] add_ind=%s sub_ac_seqn=%ld",__FILE__,__LINE__, g_pub_tx.ac_no,add_ind,sub_ac_seqn);
	WRITEMSG
	sprintf(g_pub_tx.ac_no1,"%s-%d",g_pub_tx.ac_no,sub_ac_seqn);
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
	get_zd_double("0390", &g_pub_tx.tx_amt1);
	vtcp_log("[%s][%d] zylsee017 amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	/*-- modify begin by wzs 20120215 reason:ͬspD034 44���Ѿ�����ȡ����˻���Ž��� --*/
	/* get_zd_long("0440", &g_pub_tx.erro_date); */ 	/* ĸ�ʺ��������� */
	get_zd_long("0450",&g_pub_tx.erro_date);
	/*-- modify end by wzs 20120215 --*/
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

	sprintf(acErrMsg, "---ac_no=[%s], amt=[%lf], add_ind=[%s], \
		ct_ind=[%s], tx_date=[%ld]", g_pub_tx.ac_no, g_pub_tx.tx_amt1,
		g_pub_tx.add_ind, g_pub_tx.ct_ind, g_pub_tx.erro_date);
	WRITEMSG

	ret = pub_base_sub_ac_trance(sub_ac_seqn);
	if (ret) {
		sprintf(acErrMsg, "���ʺŴ�ȡ�����ش���!");
		WRITEMSG
		goto ErrExit;
	}
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
