/******************************************************
* �� �� ��:  sp5580.c
* ��������:  ����ί��֧����֧ȡ�޶��ѯ
* ����Ŀ��:  
* ��    ��:  ����
insert into tx_def values ('5580', '����ί��֧����֧ȡ�޶�', '10000000000000000000000000000100000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000', '0', '4', '0', '3', null);
insert into tx_flow_def values ('5580', 0, '5580', '#$');
insert into tx_sub_def values ('5580', '����ί��֧����֧ȡ�޶�', 'sp5580', '0', '0');
*******************************************************/
#define EXTERN
#include "public.h"
#include "draw_loan_limit_c.h"
#include "draw_loan_hst_c.h"

sp5580()
{
	char   ac_no[20],filename[512],cFlag[2];
	double auth_amt;
	char   cKh_flag[2];   /*** �ۻ���־***/
	struct draw_loan_hst_c sDraw_hst;
	struct draw_loan_limit_c sDraw_limit;
	FILE   *fp=NULL;
	
	memset(&sDraw_hst,0x00,sizeof(sDraw_hst));
	memset(&sDraw_limit,0x00,sizeof(sDraw_limit));
	memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
	memset(filename,0x00,sizeof(filename));
	memset(cKh_flag,0x00,sizeof(cKh_flag));
	
	pub_base_sysinit();
	
	get_zd_data("0300",ac_no);
	get_zd_data("0690",cFlag);
	get_zd_data("0670",cKh_flag);
	
	pub_base_old_acno(ac_no);     /*add by CHENGBO �¾��˺�ת�� 20101007*/	
	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName(filename);
	
	fp = fopen(filename, "w");
	if (fp == NULL)
	{
		sprintf(acErrMsg, "���ļ� [%s] ���� ", filename);
		WRITEMSG
		strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	
	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' ",ac_no);
	if(g_reply_int)
	{
		sprintf(acErrMsg, "[%s][%d]�����˺�[%s]",__FILE__,__LINE__,ac_no );
		WRITEMSG
		strcpy(g_pub_tx.reply, "B117");
		goto ErrExit;
	}
	if(g_mdm_ac_rel.note_sts[0] == '1' && cKh_flag[0] != '1')  /*** ��ʧ���Կۻ� add by chenchao 20101114***/
	{
		/**�����жϸ��˻��Ƿ������� 20130226 �����ʧ��������ʾ��׼ȷ������**/
		g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
		if(g_reply_int)
		{
			sprintf(acErrMsg, "[%s][%d]δ�ҵ�ac_id[%ld]",__FILE__,__LINE__,g_mdm_ac_rel.ac_id );
			WRITEMSG
			strcpy(g_pub_tx.reply, "B117");
			goto ErrExit;
		}
		if(g_dd_mst.ac_sts[0] == '*')
		{
		sprintf(acErrMsg, "[%s][%d]������ac_sts[%s]",__FILE__,__LINE__,g_dd_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply, "P245");
		goto ErrExit;
		}
		/**�����жϸ��˻��Ƿ������� 20130226 �����ʧ��������ʾ��׼ȷ������**/
		sprintf(acErrMsg, "[%s][%d]�ѹ�ʧnote_sts[%s]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D140");
		goto ErrExit;
	}else if(g_mdm_ac_rel.note_sts[0] != '0' && g_mdm_ac_rel.note_sts[0] != '1')
	{
		sprintf(acErrMsg, "[%s][%d]״̬������note_sts[%s]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply, "O085");
		goto ErrExit;
	}	
	set_zd_data("0020",g_mdm_ac_rel.opn_br_no);
	g_reply_int=Draw_loan_hst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and sts='%s' ",g_mdm_ac_rel.ac_id,cFlag);
	if(g_reply_int)
	{
		sprintf(acErrMsg, "[%s][%d]��Ȩ֧����ϸ�����α����[%d]",__FILE__,__LINE__,g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	while(1)
	{
		g_reply_int=Draw_loan_hst_Fet_Sel(&sDraw_hst,g_pub_tx.reply);
		if(g_reply_int==100)
		{
			break;
		}
		else if(g_reply_int)
		{
			sprintf(acErrMsg, "��Ȩ֧����ϸ��ȡ�α����[%d]",g_reply_int );
			WRITEMSG
			strcpy(g_pub_tx.reply, "D102");
			goto ErrExit;
		}
		g_reply_int=Draw_loan_limit_Sel(g_pub_tx.reply,&sDraw_limit,"pact_no='%s'",sDraw_hst.pact_no);
		if(g_reply_int)
		{
			sprintf(acErrMsg, "[%s][%d]����ί��֧���޶���ѯ����[%d]",__FILE__,__LINE__,g_reply_int );
			WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}
		/**����Э��ţ���Ȩ֧��Э�飬֧ȡ������֧ȡ�޶ί��֧���޶�,֧����ʽ���տ��˺�**/
		fprintf(fp,"%20s|%24s|%14.2f|%14.2f|%14.2f|%1s|%s|\n",sDraw_hst.pact_no,sDraw_hst.draw_no,sDraw_hst.draw_amt,sDraw_limit.self_amt-sDraw_limit.self_draw_amt,sDraw_limit.trust_amt-sDraw_limit.trust_draw_amt,sDraw_hst.st_flag,sDraw_hst.trust_ac_no);
	set_zd_data(DC_FILE_SND,"1");
	}
	Draw_loan_hst_Clo_Sel();
OkExit:
	fclose(fp);
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	if(fp!=NULL)
	{
		fclose(fp);
	}
	Draw_loan_hst_Clo_Sel();
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
