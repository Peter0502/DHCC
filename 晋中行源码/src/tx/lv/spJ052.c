/*****************************************************
* �� �� ��:  spJ052.c
* ���������� ʵʱ���ҵ��¼��
* ��    ��:  xyy
* ������ڣ� 2006-9-2 14:06 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
#include "lv_wbctl_c.h"
static	PAY_IN_AREA payin;
extern char * pcLvDesPswdCode(char *code);

int spJ052()
{
	int iRc = 0;
	struct lv_wbctl_c lv_wbctl;
	struct lv_pkgreg_c lv_pkgreg;
	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	memset(&lv_wbctl,0,sizeof(lv_wbctl));
	memset(&lvca, 0 , sizeof(lvca));
	vtcp_log("[%s][%d] spJ052��ʼ�\n",__FILE__,__LINE__);

	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}
	/****���payin*********/
	g_reply_int = iLvTisToPayin(&payin);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iHvTistoNotpay����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
  vtcp_log("[%s][%d] payin.F_lw_ind = [%c]",__FL__,payin.F_lw_ind[0]);
	/****�������****/
	g_reply_int = iLvPayinCheck0(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����������,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/****Ҫ�ؼ��****/
	g_reply_int = iLvPayinCheck4(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"Ҫ�ؼ�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/****�кż��****/
	g_reply_int = iLvPayinCheck1(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�кż�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/* �����Ѽ�� */
	iLvCountFeeamt(&payin);

	/****�տ��ʺż��****/
	g_reply_int=iLvPayinCheck3(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�տ��ʺż�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] payactno[%s]\n",__FILE__,__LINE__,payin.F_payactno);
	vtcp_log("[%s][%d] cashactno[%s]\n",__FILE__,__LINE__,payin.F_cashactno);
	int iOrderno;
	/*******
	int iPackid;
	iPackid = iLvGetPackid();
	if(iPackid<=0){
		sprintf( acErrMsg,"�õ�֧��������ų���,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	apitoa(iPackid,sizeof(payin.F_packid),payin.F_packid);
	memcpy(payin.F_packday,payin.F_wtday,sizeof(payin.F_wtday));
	*******************/
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0){
		sprintf( acErrMsg,"�õ�֧��������ų���,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
	/***����������������һ���µļ�¼****/
	g_reply_int = iLvNewRecFromPayin(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����һ���µļ�¼����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/***�����¼�¼****/
	g_reply_int = iLvUpdRecFromPayin(&payin,1);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�����¼�¼,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int = iSetSSJJ_Addit();
	if(g_reply_int) 
	{
		vtcp_log("%s,%d,����ʵʱ��Ǹ��������,[%d]",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"����ʵʱ��Ǹ��������,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/*****�����****/
	vtcp_log("[%s][%d]F_packid=[%s]\n",__FILE__,__LINE__,payin.F_packid);
	payin.F_packend[0]='1';
	iRc=iLvPayinCheck8(&payin,&lv_wbctl,NULL);
	if(iRc!=PCHK_UPDATE && iRc!=PCHK_NEW)
	{
		vtcp_log("[%s][%d]======0�°�,1�Ѵ���,2�Ѹ���===iRc=[%d]\n",__FILE__,__LINE__,iRc);
		sprintf( acErrMsg,"�޷�ƴ��ָ����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	memcpy(payin.T_packday,payin.A_tbsdy,sizeof(payin.T_packday));
	memcpy(payin.F_packday,payin.A_tbsdy,sizeof(payin.F_packday));
	vtcp_log("[%s][%d] pkgid====[%s]\n",__FILE__,__LINE__,payin.F_packid);
	vtcp_log("[%s][%d] packday==[%s]\n",__FILE__,__LINE__,payin.T_packday);
	g_reply_int = iLvUpdRecFromPayin(&payin,0);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"iLvUpdRecFromPayin��,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/** �������� **/

	/******************************* ����������
	memcpy(payin.T_jzday,payin.F_tx_date,sizeof(payin.T_jzday));
	g_reply_int = iLv_updAcmrAndAct(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,��������ʧ��,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"��������ʧ��,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	******************************************/
        /* ����ˮ��־ */
        g_reply_int = pub_ins_trace_log();
        if ( g_reply_int )
        {
                sprintf( acErrMsg, "����ˮ��־��" );
                strncpy(g_pub_tx.reply,"S023",4);
                WRITEMSG
                goto ErrExit;
        }
        /*****д������ϸ�� ******/
   iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;

}

int iSetSSJJ_Addit()
{
	TD_ADD_AREA wd_tctd;
	FILE *fp;
	char cFilename[128];
	char *ptr=(char *)&wd_tctd;
	int i;
	int iRc=0;
	/****Add by SSH,2006.1.25,ֻ��ͨ��ͨ�ҵ��ڴ����ø����������ڹ���������*/
	if(memcmp(payin.F_txnum,TXNUM_TD,sizeof(payin.F_txnum))!=0 && \
		memcmp(payin.F_txnum,TXNUM_DGTD,sizeof(payin.F_txnum))!=0)
	{
		return(0);
	}
	/****NEWTCTD:����ͨ��ҵ�񸽼���****/
	memset(&wd_tctd,0,sizeof(wd_tctd));
	memset(cFilename,0,sizeof(cFilename));
	pub_base_AllDwnFilName(cFilename);	
	vtcp_log("[%s][%d],�������ļ���cFilename[%s]\n",__FL__,cFilename);
	fp=fopen(cFilename,"rb");
	if(fp==NULL){
		vtcp_log("[%s][%d],open add_area file[%s] failed!\n",__FL__,cFilename);	
		sprintf(acErrMsg,"open add_area file[%s] failed!\n",cFilename);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data(DC_GET_MSG,"δ���ָ���������");
		return(-1);
	}
	fread((char *)&wd_tctd,sizeof(wd_tctd),1,fp);	
	fclose(fp);
	vtcp_log("[%s][%d],����������:\n[%s]\n",__FILE__,__LINE__,&wd_tctd);
	if(memcmp(wd_tctd.pswdmethod,"01",2)==0){
		iRc=nBepsEncrypt(&payin,&wd_tctd);
		if(iRc){
			vtcp_log("%s,%d,����ʧ��!******\n",__FILE__,__LINE__);
			sprintf(acErrMsg,"����ʧ�������¼���!iRc[%d]",iRc);	
			WRITEMSG
			return(-1);
		}
		vtcp_log("%s,%d,���ܳɹ�!******\n",__FILE__,__LINE__);
	}	
	iRc=iLvSetAddit(&payin,"000",&wd_tctd,strlen((char *)&wd_tctd));
	vtcp_log("%s,%d,payin.T_addid=%.8s\n",__FL__,payin.T_addid);
	if(iRc)
	{
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	return(0);
}
/******************************************************************************
*  																			  *
*	FUNCTION:		   lv10_PreTos()										  *
*   DESCRIPTION:	   Ϊ���ذ���ֵ											  *
******************************************************************************/
ilv10_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
