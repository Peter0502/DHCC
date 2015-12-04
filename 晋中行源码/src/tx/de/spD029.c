/***************************************************************
* �� �� ��:     spD029.c
* ����������	�������ʺŽ���
*
* ��    ��:     jack
* ������ڣ�    2004��7��6��
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
#include "dd_mst_c.h"
#include "sub_dd_mst_c.h"

spD029()
{
	int ret = 0,sub_ac_seqn=0;
	char intst_type[2];
	/*-- add begin by wzs 20120410 reason:�����ֶβ��ҿ��Դ�Ǯ���� --*/
	char cPaper_type[2]; /* ֤������ */
	char cPaper_no[21];  /* ֤������ */
	char cTelephone[21]; /* ��ϵ�绰 */
	char cBar_code[17];  /* ������   */
	char cCash_flag[2];  /* ��ת��־ */
	double dTx_amt=0.0;  /* ���׽�� */
	/*-- add end by wzs --*/
	double			dRate_val=0.00;
	struct sub_dd_mst_c sSub_dd_mst;
	struct dd_mst_c sDd_mst;

	memset(intst_type,0x00,sizeof(intst_type)-1);
	long xq_num,lp_num,dy_num,fj_num;
	/*-- add begin by wzs 20120301 reason:����ûȡ�������� ����Ҫ�ð� - -! --*/
	char rate_type[2];
	memset(rate_type,0x00,sizeof(rate_type));
	/*-- add end by wzs 20120301 --*/
	/*-- add begin by wzs 20120410 reason:�����ֶ� --*/
	memset(cPaper_type,0x00,sizeof(cPaper_type));
	memset(cPaper_no,0x00,sizeof(cPaper_no));
	memset(cTelephone,0x00,sizeof(cTelephone));
	memset(cBar_code,0x00,sizeof(cBar_code));
	memset(cCash_flag,0x00,sizeof(cCash_flag));
	memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
	/*-- add end by wzs --*/

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	get_zd_data("0300", g_pub_tx.ac_no);	/*** ���ʺ� ***/
	pub_base_old_acno(g_pub_tx.ac_no);	/** �Ծ��ʺŵĴ��� **/
	get_zd_data("0250", g_pub_tx.name);	/*** ���ʺŻ��� ***/
	get_zd_data("0670", intst_type);	/*** ��Ϣ���� ***/
	get_zd_int("0440", &sub_ac_seqn);	/*** ���˻���� ***/
	vtcp_log("[%s][%d]ac_seqn[%d]",__FILE__,__LINE__,sub_ac_seqn);
	get_zd_double("0840",&dRate_val);	/*** ���� ***/
	vtcp_log("ȡ��������Ϊ:[%lf] [%s] [%d]",dRate_val,__FILE__,__LINE__);
	get_zd_long("0530",&xq_num);
	get_zd_long("0540",&lp_num);
	get_zd_long("0550",&dy_num);
	get_zd_long("0470",&fj_num);
	/*-- add begin by wzs 20120410 reason:�����ֶβ��ҿ��Դ�Ǯ���� --*/
	get_zd_data("0690", cPaper_type);
	get_zd_data("0620", cPaper_no);
	get_zd_data("0630", cTelephone);
	get_zd_data("0610", cBar_code);
	get_zd_data("0700", cCash_flag);
	get_zd_double("0400",&dTx_amt);
	/*-- add end by wzs 20120410--*/

	strcpy(g_pub_tx.brf, "�������ʺ�");
	strcpy(g_pub_tx.cur_no, "01");	/*** Ĭ������� ***/
	g_pub_tx.trace_cnt = 0;
	strcpy(g_pub_tx.add_ind, "1");
	strcpy(g_pub_tx.ct_ind, "2");
	g_pub_tx.tx_amt1 = 0.00;
	/*-- add begin by wzs 20120301 reason:����ûȡ�������� ����Ҫ�� --*/
	get_zd_data("0680",rate_type);
	vtcp_log("ȡ������������Ϊ:[%s] [%s] [%d]",rate_type,__FILE__,__LINE__);
	/*-- add end by wzs 20120301 --*/
	ret=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.ac_no);
	if(ret)
	{
		vtcp_log("[%s][%d] Mdm_ac_rel_Dec_Upd �α��������� ,ret=%d",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}
	ret=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if(ret)
	{
		vtcp_log("[%s][%d] Mdm_ac_rel_Fet_Upd ȡ�α���� ,ret=%d",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;
	}
	if(g_mdm_ac_rel.draw_pwd_chg[0]!='1')
	{
		vtcp_log("%s,%d,���˻�û�п�ͨ�����˻����ܽ������˻�!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"SN10");
		goto ErrExit;
	}
	ret=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if(ret)
	{
		vtcp_log("[%s][%d] Mdm_ac_rel_Upd_Upd ���� ,ret=%d",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	Mdm_ac_rel_Clo_Upd();

	ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
	if(ret)
	{
		vtcp_log("[%s][%d] Dd_mst Sel ���� ,ret=%d",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}

	/* �ѵǼ�������˺�  del by chenchao  ��Ϊǰ̨����
	sub_ac_seqn = sql_max_int("sub_dd_mst","sub_ac_seqn","ac_no='%s' ",g_pub_tx.ac_no);
	if(sub_ac_seqn<0)
		goto ErrExit;

	ret = pub_base_opn_sub_ac(sub_ac_seqn+1,intst_type,dRate_val);
	**/
	ret = pub_base_opn_sub_ac(sub_ac_seqn,intst_type,dRate_val);
	if (ret) {
		sprintf(acErrMsg, "�������ʺ����ش���!");
		WRITEMSG
			goto ErrExit;
	}
	char tmp_seqn[9];
	sprintf(tmp_seqn,"%d",sub_ac_seqn);
	set_zd_data("1221",tmp_seqn);
	/**С�������˻�����**/
	memset(&sSub_dd_mst,0x00,sizeof(sSub_dd_mst));
	g_reply_int=Sub_dd_mst_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and sub_ac_seqn=%ld",g_pub_tx.ac_no,sub_ac_seqn);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"���˻����ѯ�����α����");
		strcpy(g_pub_tx.reply,"D101");
		WRITEMSG
			goto ErrExit;
	}
	g_reply_int=Sub_dd_mst_Fet_Upd(&sSub_dd_mst,g_pub_tx.reply);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"���˻���ȡ�α����");
		strcpy(g_pub_tx.reply,"D102");
		WRITEMSG
			goto ErrExit;
	}
	/*-- modify begin by wzs 20120301 reason:�����ĳ���Ĭ�ϴ�0�� ������Ҫ���Ż����� �����Ϣ���� --*/
	/* strcpy(sSub_dd_mst.rate_type,"0"); */
	strncpy(sSub_dd_mst.rate_type,rate_type,strlen(rate_type));
	/*-- modify end by wzs 20120301 --*/
	/*-- add begin by wzs 20120410 reason:�����ֶ� --*/
	strcpy(sSub_dd_mst.paper_type,cPaper_type);
	strcpy(sSub_dd_mst.paper_no,cPaper_no);
	strcpy(sSub_dd_mst.telephone,cTelephone);
	strcpy(sSub_dd_mst.bar_code,cBar_code);
	/*-- add end by wzs 20120410--*/
	sSub_dd_mst.xq_num=xq_num;
	sSub_dd_mst.lp_num=lp_num;
	sSub_dd_mst.dy_num=dy_num;
	sSub_dd_mst.fj_num=fj_num;
	g_reply_int=Sub_dd_mst_Upd_Upd(sSub_dd_mst,g_pub_tx.reply);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"���˻����´���");
		strcpy(g_pub_tx.reply,"D105");
		WRITEMSG
		goto ErrExit;
	}
	Sub_dd_mst_Clo_Upd();
	/*-- add begin by wzs 20120410 reason:�������˺Żش� --*/
	if(dTx_amt > 0.0)
	{
		g_pub_tx.tx_amt1=dTx_amt;
		strcpy(g_pub_tx.ac_no1,sSub_dd_mst.sub_ac_no);
		g_pub_tx.add_ind[0]='1';
		g_pub_tx.svc_ind=1201;
		strcpy(g_pub_tx.ct_ind,cCash_flag);
		strcpy(g_pub_tx.brf,"���˻�����");
		strcpy(g_pub_tx.cur_no,"01");
		ret=pub_base_sub_ac_trance(sub_ac_seqn);
		if(ret)
		{
			vtcp_log("���˻�����ʧ��,[%s] [%d] ",__FILE__,__LINE__);
			goto ErrExit;
		}

		strcpy(g_pub_tx.ac_wrk_ind,"00000000");
		g_pub_tx.svc_ind=1001;
		strcpy(g_pub_tx.ct_ind,cCash_flag);
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.prdt_code,sDd_mst.prdt_no );
		g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
		g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"" );
		strcpy(g_pub_tx.no_show,"0");
		strcpy(g_pub_tx.ac_get_ind,"00");
		strcpy(g_pub_tx.ac_id_type,"1");
		strcpy(g_pub_tx.hst_ind,"1");
		strcpy(g_pub_tx.brf,"���˻����");
		strcpy(g_pub_tx.tx_code,"2109");
		strcpy(g_pub_tx.sub_tx_code,"D099");

		ret=pub_acct_trance();
		if(ret)
		{
			vtcp_log("�Ǽǽ�����ˮʧ�� [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}

		set_zd_data("1011", g_pub_tx.ac_no);
		set_zd_int("1012", &g_pub_tx.ac_seqn);
		set_zd_data("101A", g_pub_tx.cur_no);
		set_zd_data("1014", g_pub_tx.note_type);
		set_zd_data("1015", g_pub_tx.beg_note_no);
		set_zd_data("1015", g_pub_tx.end_note_no);
		set_zd_data("101B", g_pub_tx.ct_ind);
		set_zd_data("1016", g_pub_tx.brf);
		set_zd_double("1013", dTx_amt);

		ret = pubf_acct_proc("D099");
		if(ret)
		{
			vtcp_log("�������˻�ʧ�� [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}
	}
	set_zd_data("0330",sSub_dd_mst.sub_ac_no);
	set_zd_data("1181","01");
	set_zd_double("1184",dTx_amt);
	/*-- add end by wzs 20120410--*/
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "�����ʺųɹ�![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "�����ʺ�ʧ��![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
