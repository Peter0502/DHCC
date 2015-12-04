/*************************************************
* �� �� ��:  sp1714.c
* ���������� �����˻����޸�
* ��    ��:  zyhli0
* ������ڣ� 2010-11-8 9:34:51
*
*insert into tx_def values ('1714','�����˻����޸�','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100','0','5','0','3',null);
*insert into tx_flow_def values('1714',0,'1714','#$');
*insert into tx_sub_def values('1714','�����˻����޸�','sp1714','0','0');
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
        sprintf( acErrMsg, "sql error" ); \
        WRITEMSG \
        goto ErrExit; \
        }
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "sub_dd_mst_c.h"

int sp1714()
{
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct sub_dd_mst_c sSub_dd_mst;
	char cAc_no[25];
	char cName[61];
	char cFlag[2];
	char cAc_seqn[9];
	char cPaper_type[1+1];
	char cPaper_no[20+1];
	char cTelephone[20+1];
	int ret=0;
	/* add begin by wzs 20120510 reason:����С���ֶ� --*/
	long xq_num=0;
	long lp_num=0;
	long dy_num=0;
	long fj_num=0;
	/*-- add end by wzs 20120510 --*/

	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sSub_dd_mst,0x00,sizeof(sSub_dd_mst));
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(cAc_seqn,0x00,sizeof(cAc_seqn));
	memset(cName,0x00,sizeof(cName));
	memset(cFlag,0x00,sizeof(cFlag));
	/*-- add begin by wzs 20120410 reason:�����ֶ� --*/
	memset(cPaper_type,0x00,sizeof(cPaper_type));
	memset(cPaper_no,0x00,sizeof(cPaper_no));
	memset(cTelephone,0x00,sizeof(cTelephone));
	/*-- add end by wzs 20120410 --*/

	get_zd_data("0300",cAc_no);
	pub_base_old_acno(cAc_no);
	get_zd_data("0780",cAc_seqn);
	get_zd_data("0250",cName);
	get_zd_data("0690",cFlag);
	/*-- add begin by wzs 20120410 reason:�����ֶ� --*/
	get_zd_data("0680",cPaper_type);
	get_zd_data("0620",cPaper_no);
	get_zd_data("0270",cTelephone);
	/*-- add end by wzs 20120410 --*/

	/* add begin by wzs 20120510 reason:����С���ֶ� --*/
	get_zd_long("0530",&xq_num);
	get_zd_long("0540",&lp_num);
	get_zd_long("0550",&dy_num);
	get_zd_long("0470",&fj_num);
	/*-- add end by wzs 20120510 --*/

	pub_base_sysinit();

	vtcp_log(" %s, %d, ac_no[%s] ac_seqn[%s] name[%s] flag[%s] ",__FILE__,__LINE__,cAc_no,cAc_seqn,cName,cFlag);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no='%s' ",cAc_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�������˻���ϵ���ձ����޴��˺�[%s]��¼!",cAc_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B117");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"��ѯ�������˻���ϵ���ձ��쳣[ret=%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L182");
		goto ErrExit;
	}
	/* ����к� */
	if(memcmp(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no,sizeof(g_pub_tx.tx_br_no)))
	{
		sprintf(acErrMsg,"�����뱾�������˻������ܿ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"A039");
		goto ErrExit;
	}
	/* �Ƿ�ͨ������ҵ�� */
	/* if(sMdm_ac_rel.kt_flag[0]!='1') */
	if(strcmp(sMdm_ac_rel.draw_pwd_chg,"1"))
	{
		sprintf(acErrMsg,"���˻�û�п�ͨ�����˻�ҵ��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"SN19");
		goto ErrExit;
	}

	if(cFlag[0]=='1')		/* ��ѯʱֻ�������˺Ż��� */
	{
		ret=Sub_dd_mst_Sel(g_pub_tx.reply,&sSub_dd_mst," ac_no='%s' and sub_ac_seqn=%ld ",cAc_no,atoi(cAc_seqn));
		if(ret==100)
		{
			sprintf(acErrMsg,"�������ʺ�̨�ʱ����޴��˺�[ac_no=%s sub_ac_seqn=%ld]��¼!",cAc_no,atoi(cAc_seqn));
			WRITEMSG
			strcpy(g_pub_tx.reply,"SN12");
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"��ѯ�������ʺ�̨�ʱ��쳣!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		set_zd_data("0250",sSub_dd_mst.name);
		goto OkExit;
	}

	ret=Sub_dd_mst_Dec_Upd(g_pub_tx.reply," ac_no='%s' and sub_ac_seqn=%ld ",cAc_no,atoi(cAc_seqn));
	if(ret)
	{
		sprintf(acErrMsg,"DECLARE sub_dd_mst �쳣[ret=%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}

	memset(&sSub_dd_mst,0x00,sizeof(sSub_dd_mst));
	ret=Sub_dd_mst_Fet_Upd(&sSub_dd_mst,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"FETCH sub_dd_mst �쳣[ret=%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;
	}

	if(sSub_dd_mst.ac_sts[0]=='*')
	{
		sprintf(acErrMsg,"�����˺Ŵ�������״̬���������޸Ļ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"SN11");
		goto ErrExit;
	}

	pub_base_strpack(cName);
	memcpy(sSub_dd_mst.name,cName,sizeof(sSub_dd_mst.name));
	/*-- add begin by wzs 20120410 reason:�����ֶ� --*/
	strncpy(sSub_dd_mst.paper_type,cPaper_type,sizeof(sSub_dd_mst.paper_type));
	strncpy(sSub_dd_mst.paper_no,cPaper_no,sizeof(sSub_dd_mst.paper_no));
	strncpy(sSub_dd_mst.telephone,cTelephone,sizeof(sSub_dd_mst.telephone));
	/*-- add end by wzs 20120410 --*/
	/* add begin by wzs 20120510 reason:����С���ֶ� --*/
	sSub_dd_mst.xq_num=xq_num;
	sSub_dd_mst.lp_num=lp_num;
	sSub_dd_mst.dy_num=dy_num;
	sSub_dd_mst.fj_num=fj_num;
	/*-- add end by wzs 20120510 --*/
	Sub_dd_mst_Debug(&sSub_dd_mst);

	ret=Sub_dd_mst_Upd_Upd(sSub_dd_mst,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"Update sub_dd_mst �쳣[ret=%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}

	vtcp_log(" %s, %d, ���³ɹ����Ǽ���ˮ",__FILE__,__LINE__);
	strcpy(g_pub_tx.ac_no,sSub_dd_mst.ac_no);
	g_pub_tx.ac_id=sSub_dd_mst.sub_ac_seqn;
   	/* �Ǽǽ�����ˮ */
   	if(pub_ins_trace_log())
   	{
   	  sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
   	  WRITEMSG
   	  strcpy(g_pub_tx.reply,"P126");
   	  goto ErrExit;
   	}

	set_zd_data("0250",sSub_dd_mst.name);
	set_zd_data("0320",sSub_dd_mst.ac_no);
	set_zd_data("0330",sSub_dd_mst.sub_ac_no);
	set_zd_int("0780",sSub_dd_mst.sub_ac_seqn);

	Sub_dd_mst_Clo_Upd();

OkExit:
  sprintf(acErrMsg,"���׳ɹ�!");
  WRITEMSG
 	strcpy(g_pub_tx.reply,"0000");
  set_zd_data("0120",g_pub_tx.reply);
  return 0;

ErrExit:
  sprintf(acErrMsg,"����ʧ��![ %s ]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data("0120",g_pub_tx.reply);
  return 1;
}
