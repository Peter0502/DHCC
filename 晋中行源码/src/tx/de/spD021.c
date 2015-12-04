/***************************************************************
* �� �� ��:     spD021.c
* ����������	����ת����
*
* ��    ��:     LiuHuafeng
* ������ڣ�    2006-10-26 16:00
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
#include "com_item_c.h"
#include "com_item_add_c.h"
#include "zjgl_mst_c.h"

spD021()
{
	int	ret=0;
	int mcnt=0, zcnt=0;
	char  cBr_no[BRNO_LEN+1];
	char  cAcc_no[8];
	double dTxamt=0.0,dRelamt=0.0;
	struct com_item_c sCom_item;
	int iProcType=0; /* 0ת�� 1�ۻ� */
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&sCom_item, 0 , sizeof(sCom_item));
	memset(cBr_no, 0 , sizeof(cBr_no));
	memset(cAcc_no, 0 ,sizeof(cAcc_no));
	get_zd_data("0300",g_pub_tx.ac_no1);		/*** ĸ�ʺ� ***/
	pub_base_old_acno( g_pub_tx.ac_no1 );
	if(memcmp(g_pub_tx.tx_br_no,QS_BR_NO,BRNO_LEN) && memcmp(g_pub_tx.tx_br_no,CW_BR_NO, BRNO_LEN))
	{
		vtcp_log("%s %d ���׻����������������Ļ����ǲ�������[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no);
		strncpy(g_pub_tx.reply,"S043",4);
		goto ErrExit;
	}
	get_zd_data("0910",cBr_no);
	if(memcmp(cBr_no,g_pub_tx.tx_br_no,BRNO_LEN)==0 && memcmp(cBr_no,QS_BR_NO,BRNO_LEN)==0)
	{
		vtcp_log("%s %d ���׻�������������[%s]���Է������������������[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,cBr_no);
		strncpy(g_pub_tx.reply,"S043",4);
		goto ErrExit;
	}
	get_zd_data("0640",cAcc_no);
	/* �õ���Ŀ������Ϣ */
	{
		memset(&sCom_item, 0 , sizeof(sCom_item));
		ret = Com_item_Sel(g_pub_tx.reply,&sCom_item, "acc_no ='%s' ",cAcc_no);
		if ( ret==100 )
		{
			strncpy(g_pub_tx.reply,"S033",4);
			sprintf(acErrMsg,"����Ŀ�Ŀ������!!");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"���ݿ����ʧ��[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		if(sCom_item.sub_acc_yn[0]=='Y')
		{
			strncpy(g_pub_tx.reply,"P301",4);
			sprintf(acErrMsg,"����Ŀ�Ŀ������ײ��Ŀ");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto	ErrExit;
		}
		if(sCom_item.dc_ind[0]=='0')
		{
			strncpy(g_pub_tx.reply,"T059",4);
			sprintf(acErrMsg,"����Ŀ�Ŀ�����Կ�Ŀ");
			WRITEMSG
			goto	ErrExit;
		}
	}
	if(memcmp(cBr_no,QS_BR_NO, BRNO_LEN)==0)
	{
		/* ������ת������ */
		iProcType=0;/* ת�� */
	}
	else
	{
		double dFd42=0.0;
		get_zd_double("0420",&dFd42);
		dFd42=0-dFd42;
		set_zd_double("0420",dFd42);
		set_zd_double("0430",dFd42);/*���ڻ��� add by LiuHuafeng 2006-12-26 10:48 */
		/* ���ݿ�Ŀ�Ľ�������ж��ǿۻ����ǻ��� */

		if(sCom_item.dc_ind[0]=='1')
			iProcType=1; /* �ۻ� */
		else
			iProcType=2; /* ���� */

		vtcp_log("%s,%d ------------- look iProcType==[%d] ",__FILE__,__LINE__,iProcType);
	}
	/* ���Է������Ƿ���Խ��пۻ����� */
	{
		struct com_item_add_c sCom_item_add;
		memset(&sCom_item_add, 0 , sizeof(sCom_item_add));
		ret = Com_item_add_Sel(g_pub_tx.reply,&sCom_item_add, "acc_no ='%s' ",cAcc_no);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"����Ŀ�Ŀ����Ҫ�����ʽ����!!");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"���ݿ����ʧ��[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		/*if(sCom_item_add.add_ind[0]=='0')  ����˵�жϵ���λ*/
		if(sCom_item_add.add_ind[2]=='0')
		{
			strncpy(g_pub_tx.reply,"T058",4);
			sprintf(acErrMsg,"����Ŀ�Ŀ����Ҫ�����ʽ����!!");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto	ErrExit;
		}
	}
	dTxamt=0.0,dRelamt=0.0;
	get_zd_double("0390",&dTxamt);
	vtcp_log("%s,%d dTxamt===[%f]",__FILE__,__LINE__,dTxamt);
	get_zd_double("0430",&dRelamt);
	/* �Ƚ������������� */
	{
		if(dTxamt-dRelamt<0)
		{
			strncpy(g_pub_tx.reply,"T058",4);
			sprintf(acErrMsg,"����Ŀۻ����С��ʵ��ת�����");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto	ErrExit;
		}
	}
/*
	dTxamt=0-dTxamt;
	set_zd_double("0390",dTxamt);
*/
	/* ���ҹ��ܵı�,�ж����ҵ������-- �ⲿ���ɹ�����������  */
	{
		double dTmpbal=0;
		struct zjgl_mst_c sZjgl_mst;
		memset(&sZjgl_mst, 0 , sizeof(sZjgl_mst));
		memcpy(sZjgl_mst.opn_br_no,cBr_no,BRNO_LEN);
		get_zd_data("0210",sZjgl_mst.cur_no);
		memcpy(sZjgl_mst.acc_no,cAcc_no,sizeof(sZjgl_mst.acc_no)-1);
		ret = Zjgl_mst_Sel(g_pub_tx.reply,&sZjgl_mst, 
			"opn_br_no='%s' and cur_no='%s' and acc_no ='%s' ", cBr_no,sZjgl_mst.cur_no,cAcc_no);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"����Ŀ�Ŀ����Ҫ�����ʽ����!!");
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"���ݿ����ʧ��[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("%s,%d into ipubf_acct_zjgl_trance",__FILE__,__LINE__);
		/*�ⲿ���ɹ��������Զ�����
		 *ret=ipubf_acct_zjgl_trance(cBr_no,sZjgl_mst.cur_no,cAcc_no,&dTxamt,&dTmpbal);
		 *if(ret)
		 *{
		 *	strncpy(g_pub_tx.reply,"T058",4);
		 *	vtcp_log("%s,%d ���¹����ʲ�����Ǽǲ�ʧ��",__FILE__,__LINE__);
		 *	sprintf(acErrMsg,"�����ʲ�����Ǽǲ���ʧ��[%d]",ret);
		 *	WRITEMSG
		 *	goto ErrExit;
		 *}
		 *vtcp_log("%s,%d ���¹����ʲ�����Ǽǲ��ɹ�",__FILE__,__LINE__);
		 *****************************************/
	}
	get_zd_data("0680",g_pub_tx.ct_ind);		/*** ��ת��־ ***/
	get_zd_double("0390",&g_pub_tx.tx_amt1);

	sprintf(acErrMsg, "---ac_no=[%s], amt=[%lf], ct_ind=[%s]", 
		g_pub_tx.ac_no1, g_pub_tx.tx_amt1,g_pub_tx.ct_ind);
	WRITEMSG

OkExit:
	if(iProcType==1)
		strcpy(g_pub_tx.reply,"0001");
	else if(iProcType==2)
		strcpy(g_pub_tx.reply,"0002");
	else
		strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ʽ����Ǽǲ��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�����ʽ����Ǽǲ�zjgl_mstʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
