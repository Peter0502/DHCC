/***************************************************************************/
/* �ļ�����: spJ042.c                                                   */
/* ��������: ��ͨ����ҵ�񸴺�                                             */
/* ��������: 2005.08.01                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "lv_define.h"
#include "hv_define.h"
#include "mdm_ac_rel_c.h"
extern char * pcLvGetDay();
static PAY_IN_AREA payin;
int spJ042()
{
	/***����ƾ֤����add by martin start***/
	char cVtype[4];    /*ƾ֤����*/
	char cVocnum[17];  /*ƾ֤����*/
	char cAcno[25];
	struct mdm_ac_rel_c s_mdm_ac_rel;
	memset(cVtype     ,0, sizeof(cVtype));
	memset(cVocnum    ,0, sizeof(cVocnum));
	memset(cAcno, 0 , sizeof(cAcno));
	memset(&s_mdm_ac_rel, 0 , sizeof(s_mdm_ac_rel));
	/***����ƾ֤����add by martin end***/
	char cCur_no[3];
	memset(cCur_no,0 , sizeof(cCur_no));
  vtcp_log("AP MSG: Begin of dh65100_Work\n");
	memset(&payin,'\0',sizeof(payin));
	pub_base_sysinit();
	/****���payin*********/
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = iLvTisToPayin(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d error in ",__FL__);
		sprintf(acErrMsg,"%s,%d iLvTisToPayinʧ��",__FL__);
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
  vtcp_log("%s,%d,txamt[%.15s][%.15s]",__FL__,payin.F_txamt,payin.T_txamt);

	g_reply_int = iLvFetchRecToPayin(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d error in ",__FL__);
		sprintf(acErrMsg,"%s,%d iLvFetchRecToPayinʧ��",__FL__);	
		if(memcmp(g_pub_tx.reply,"0000",4)==0)
		{
			strncpy(g_pub_tx.reply,"P046",4);
			set_zd_data("0130",acErrMsg);
			sprintf(acErrMsg,"%s,%d iLvFetchRecToPayinʧ��",__FL__);
		}
		else
			set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
  vtcp_log("%s,%d,txamt[%.15s][%.15s]",__FL__,payin.F_txamt,payin.T_txamt);

	memcpy(payin.F_packday,payin.T_packday,sizeof(payin.F_packday));
	memcpy(payin.F_packid,payin.T_packid,sizeof(payin.F_packid));
	memcpy(payin.F_ywtype,payin.T_ywtype,sizeof(payin.F_ywtype));
  vtcp_log("%s,%d,txamt[%.15s][%.15s]",__FL__,payin.F_txamt,payin.T_txamt);
	
  vtcp_log("[%s][%d]\n", __FL__);
	/**********************************/
	/* verify data for basic checking */
	/**********************************/
	/****�������****/
  vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
  vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
	g_reply_int=iLvPayinCheck0(&payin);
	if(g_reply_int){
		vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,g_reply_int);
		strncpy(g_pub_tx.reply,"P046",4);
		pub_base_strpack(acErrMsg);
		if(strlen(acErrMsg)==0)
				sprintf(acErrMsg,"%s,%d iLvPayinCheck0ʧ��",__FL__);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
  vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
	/****Ҫ�ؼ��****/
  vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	g_reply_int=iLvPayinCheck4(&payin);
	if(g_reply_int){
		vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,g_reply_int);
		pub_base_strpack(acErrMsg);
		if(strlen(acErrMsg)==0)
				sprintf(acErrMsg,"%s,%d iLvPayinCheck4ʧ��",__FL__);
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
  vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
  vtcp_log("[%s][%d]\n", __FL__);
	/****�кż��****/
  vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	g_reply_int = iLvPayinCheck1(&payin);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
		pub_base_strpack(acErrMsg);
		if(strlen(acErrMsg)==0)
			sprintf(acErrMsg,"%s,%d iLvPayinCheck1ʧ��",__FL__);
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		goto ErrExit;
	}
  vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
  vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
  vtcp_log("%s,%d,txamt[%.15s][%.15s]",__FL__,payin.F_txamt,payin.T_txamt);

	/***����һ���Լ��****/
	g_reply_int=iLvPayinCheck6(&payin);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]check6 error sqlcode=[%d]", __FL__,g_reply_int);
		pub_base_strpack(acErrMsg);
		if(strlen(acErrMsg)==0)
				sprintf(acErrMsg,"%s,%d iLvPayinCheck6ʧ��",__FL__);
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	/********************************************************/
	/* Process -- 1.Initial PBMR record to opening stauts   */
	/*            2.Insert PBMR record/Delete(reversal)     */
	/********************************************************/
	int iOrderno;
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	/****������****/

	get_fd_data(DC_TEL,payin.T_checkno);
	get_fd_data(DC_TRACE_NO,payin.T_checkwssrno);
	get_fd_data(DC_DATE,payin.T_jzday);
	memcpy(payin.T_jzday,pcLvGetDay(),sizeof(payin.T_jzday));

	vtcp_log("[%s][%d]payin.T_jzday=[%s]\n",__FILE__,__LINE__,payin.T_jzday);

	payin.T_stat[0]=STAT_WZCHECK2;
	g_reply_int=iLvUpdRecFromPayin(&payin,0);
	if(g_reply_int){
		vtcp_log("%s,%d,���µǼǲ�ʧ��!sqlcode=[%d]", __FILE__,__LINE__,g_reply_int);
		pub_base_strpack(acErrMsg);
		if(strlen(acErrMsg)==0)
				sprintf(acErrMsg,"%s,%d ���µǼǲ�ʧ��",__FL__);
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	
	g_reply_int=ilv11_PreTos();
	if(g_reply_int)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}

  g_reply_int =pub_ins_trace_log();
  if(g_reply_int)
  {
      sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
      strncpy(g_pub_tx.reply,"S023",4);
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      goto ErrExit;
  }
  /** ƾ֤����ʼ 20071113 by martin **/
	get_zd_data("0900",cVtype);
  get_zd_data("0590",cVocnum);
  get_zd_data("0590",cVocnum);
  get_zd_data("0330",cAcno);
  vtcp_log("sssssssspayin.F_pkgno[%s],cVtype=[%s]\n",payin.F_pkgno,cVtype);
   if((memcmp(payin.F_pkgno,"001",3)==0)&&(memcmp(cVtype,"005",3)==0))
	  {
	  	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no='%s'",cAcno);
	  	if(g_reply_int)
	  	{
	  		sprintf(acErrMsg,"�ʺŲ�ѯ����!");
	  		WRITEMSG
	  		goto ErrExit;
	  	}
       vtcp_log("Mdm_ac_rel_Sel s_mdm_ac_rel.ac_id=[%ld],cVocnum=[%s][%s][%d]\n",s_mdm_ac_rel.ac_id,cVocnum,__FILE__,__LINE__);
       vtcp_log("��ʼƾ֤���� type[%s][%s][%s][%d]\n",cVtype,cVocnum,__FILE__,__LINE__);
	  	g_reply_int=pub_com_NoteUse(s_mdm_ac_rel.ac_id,s_mdm_ac_rel.ac_seqn,cVtype,cVocnum,cVocnum,g_pub_tx.tel);
	  	if(g_reply_int)
	  	{
	  		sprintf(acErrMsg,"����ƾ֤����!");
	  		WRITEMSG
	  		goto ErrExit;
	  	}
	  }
	/** ƾ֤������� 20071108 by martin **/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	get_zd_data("0210",cCur_no);
	vtcp_log("%s,%d cur_no==[%s]",__FL__,cCur_no);
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

/******************************************************************************
*  																			  *
*	FUNCTION:		   lv11_PreTos()										  *
*   DESCRIPTION:	   Ϊ���ذ���ֵ											  *
******************************************************************************/
ilv11_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
