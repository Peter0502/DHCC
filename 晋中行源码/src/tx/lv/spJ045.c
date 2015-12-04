/***************************************************************************/
/* �ļ�����: dh651000.pc                                                   */
/* ��������: ʵʱ����ҵ��¼��                                              */
/* ��������: 2005.08.01                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lv_wbctl_c.h"

static PAY_IN_AREA payin;
static struct lv_wbctl_c wd_lv_wbctl;
/******************************************************************************
*                                                                             *
*   FUNCTIONS:         dh65100_Work                                           *
*                                                                             *
*   DESCRIPTION:       1.��ʼ��������                                         *
*                      2.ִ�м��                                             *
*                      3.����(����)�Ǽǲ�                                     *
*                      4.׼���������   ?                                    *
******************************************************************************/
int spJ045()
{
	int iRc=0;
	/*********************/
	/* Initial variables */
	/*********************/
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	iRc=iLv18_Initial();
	if (iRc) 
	{
		sprintf(acErrMsg,"%s,%d init error!",__FL__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
vtcp_log("[%s][%d]\n", __FL__);

	/**********************************/
	/* verify data for basic checking */
	/**********************************/
	iRc=iLv18_Verify();
vtcp_log("[%s][%d]\n", __FL__);
	/********************************/
	if (iRc) 
	{
		sprintf(acErrMsg,"%s,%d init error!",__FL__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/********************************************************/
	/* Process -- 1.Initial PBMR record to opening stauts   */
	/*            2.Insert PBMR record/Delete(reversal)     */
	/********************************************************/
	iRc=iLv18_Process();
	if (iRc) 
	{
		sprintf(acErrMsg,"%s,%d init error!",__FL__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/******************************/
	/*       Prepare TOS          */
	/******************************/
	iRc=iLv18_PreTos();
	if (iRc)
	{
		sprintf(acErrMsg,"%s,%d init error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		WRITEMSG
		goto ErrExit;
	}
  iRc=pub_ins_trace_log();
  if(iRc)
  {
      sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
      strncpy(g_pub_tx.reply,"S023",4);
      WRITEMSG
      goto ErrExit;
  }

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
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
*                                                                             *
*   FUNCTION:          a10_Initial()                                         *
*                                                                             *
*   DESCRIPTION:       1.Initial all variables.                               *
******************************************************************************/
int iLv18_Initial()
{
	char ttmp[20] = {0};
	memset(&payin,'\0',sizeof(payin));
	/**************/
	/* Initial WC */
	/**************/
	memset(&wd_lv_wbctl,'\0',sizeof(wd_lv_wbctl));

	/**************/
	/* Initial WD */
	/**************/
	pub_base_sysinit();
	/****���payin*********/
	if(iLvTisToPayin(&payin)){
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return(-1);
	}
	vtcp_log("[%s][%d]=========wp_payin->A_tbsdy=[%s]\n",__FL__,payin.A_tbsdy);
	return(0);
}
/******************************************************************************
*                                                                             *
*   FUNCTION:                            iLv18_Verify()                        *
*                                                                             *
*   DESCRIPTION:                         1.Initial all variables.             *
******************************************************************************/
int iLv18_Verify()
{
	int iRc;
	/****�������****/
	vtcp_log("[%s][%d]=========wp_payin->A_tbsdy=[%s]\n",__FL__,payin.A_tbsdy);
	iRc=iLvPayinCheck0(&payin);
	if(iRc){
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]\n", __FL__);
vtcp_log("[%s][%d]=========wp_payin->A_tbsdy=[%s]\n",__FL__,payin.A_tbsdy);
	/****Ҫ�ؼ��****/
	iRc=iLvPayinCheck4(&payin);
	if(iRc){
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return(-1);
	}
	vtcp_log("[%s][%d]=========wp_payin->A_tbsdy=[%s]\n",__FL__,payin.A_tbsdy);
vtcp_log("[%s][%d]\n", __FL__);
	/****�кż��****/
	iRc=iLvPayinCheck1(&payin);
	if(iRc){
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return(-1);
	}
	vtcp_log("[%s][%d]=========wp_payin->A_tbsdy=[%s]\n",__FL__,payin.A_tbsdy);
	return(0);	
}
/******************************************************************************
*   FUNCTION:          iLv18_Process()                                         *
*   DESCRIPTION:  1.Initial PBMR record to opening stauts                     *
*                 2.Insert PBMR record/Delete(reversal)                       *
******************************************************************************/
int iLv18_Process()
{
	int iOrderno;
	int iRc;
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0){
	vtcp_log("%s,%d,�õ�֧��������Ŵ���!\n",__FL__);
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]\n", __FL__);
vtcp_log("[%s][%d]=========wp_payin->A_tbsdy=[%s]\n",__FL__,payin.A_tbsdy);
	apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
	/***����������������һ���µļ�¼****/
	iRc=iLvNewRecFromPayin(&payin);
	if(iRc){
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]\n", __FL__);
vtcp_log("[%s][%d]=========wp_payin->A_tbsdy=[%s]\n",__FL__,payin.A_tbsdy);
 /**NEWTCTD 20070827 ��״̬�óɸ���״̬***
  payin.T_stat[0]=STAT_WZCHECK2;
  get_fd_data(DC_DATE,payin.T_jzday);
  **NEWTCTD 20070827 ��״̬�óɸ���״̬***/
	/***�����¼�¼****/
	iRc=iLvUpdRecFromPayin(&payin,1);
	if(iRc){
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return(-1);
	}
	vtcp_log("[%s][%d]=========wp_payin->A_tbsdy=[%s]\n",__FL__,payin.A_tbsdy);
vtcp_log("[%s][%d]=========wp_payin->F_orderno=[%s]\n",__FL__,payin.F_orderno);
	if(iSetSSDJ_Addit())
	{
	vtcp_log("%s,%d,���¸�����Ǽǲ�ʧ��!\n", __FL__);
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]=========wp_payin->F_packid=[%.8s]\n",__FL__,payin.F_packid);
vtcp_log("[%s][%d] =========payin.F_orderno====[%.8s]\n",__FL__,payin.F_orderno);
vtcp_log("[%s][%d]\n", __FL__);

	payin.F_packend[0]='1';
	vtcp_log("[%s][%d]=========wp_payin->A_tbsdy=[%s]\n",__FL__,payin.A_tbsdy);
vtcp_log("[%s][%d]=========wp_payin->F_packend=[%c]\n",__FL__,payin.F_packend[0]);
vtcp_log("[%s][%d]=========wp_payin->F_orderno=[%s]\n",__FL__,payin.F_orderno);
vtcp_log("[%s][%d]=====��ʼ���� �����ˣ���������\n",__FL__);
	iRc=iLvPayinCheck8(&payin,&wd_lv_wbctl,NULL);
	if(iRc!=PCHK_UPDATE && iRc!=PCHK_NEW){
	vtcp_log("[%s][%d]======0�°�,1�Ѵ���,2�Ѹ���===iRc=[%d]\n",__FL__,iRc);
		strncpy(acErrMsg,"�޷�ƴ��ָ����",40);
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]=========wp_payin->F_packid=[%s]\n",__FL__,payin.F_packid);
	iRc=iLvUpdRecFromPayin(&payin,0);
	if(iRc){
		vtcp_log("%s,%d ���� ",__FL__);
		WRITEMSG
		return (-1);
	}
	return(0);
}

/******************************************************************************
*  																			  *
*	FUNCTION:		   iLv18_PreTos()										  *
*   DESCRIPTION:	   Ϊ���ذ���ֵ											  *
******************************************************************************/
int iLv18_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
int iSetSSDJ_Addit()
{
	/* rem by LiuHuafeng 20070331
	TCTD_ADD_AREA wd_tctd;
	*************************/
	CXTC_ADD_AREA wd_tctd;
	char *ptr=(char *)&wd_tctd;
	int i,iRc=0;
	memset(&wd_tctd,0,sizeof(wd_tctd));
vtcp_log("[%s][%d] tctw==[%s]\n",__FL__,(char *)&wd_tctd);
	wd_tctd.payacttype[0]='0';
vtcp_log("[%s][%d] tctw==[%s]\n",__FL__,(char *)&wd_tctd);
	wd_tctd.cashacttype[0]=payin.F_actype[0];
	if(memcmp(payin.F_payactno,"10101",5)==0 || memcmp(payin.F_payactno,"1010100",5)==0)
		wd_tctd.ct_ind[0]='0';
	else
		wd_tctd.ct_ind[0]='1';
/** rem by LiuHuafeng 20070331
	memcpy(wd_tctd.pswdcode,(char*)pcLvDesPswdCode(payin.F_pswd),sizeof(wd_tctd.pswdcode));
vtcp_log("[%s][%d] tctw==[%s]\n",__FL__,(char *)&wd_tctd);
	for(i=0;i<sizeof(wd_tctd);i++){
		if(ptr[i]=='\0')
			ptr[i]='0';
	}
***/
vtcp_log("[%s][%d] \n\n",__FL__);
vtcp_log("[%s][%d] tctw==[%s]\n",__FL__,(char *)&wd_tctd);
	iRc=iLvSetAddit(&payin,"000",&wd_tctd,sizeof(wd_tctd));
vtcp_log("%s,%d,payin.T_addid=%.8s\n",__FL__,payin.T_addid);
	if(iRc)
	{
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	return(0);
}

