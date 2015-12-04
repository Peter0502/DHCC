/*****************************************************
* �� �� ��:  spJ055.c
* ���������� ��ͨ���ҵ���ִ����
* ��    ��:  xyy
* ������ڣ� 2006-9-6 10:13
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
#include "lv_lbctl_c.h"
static PAY_IN_AREA payin;
extern char * pcLvGetDay();
int spJ055()
{
	int iRc = 0;
	long iPackday=0;
	char cPackid[9];
	char cPayqsactno[13];
	LV_FD125_AREA fd125;
	memset(&fd125, 0 , sizeof(fd125));
	memset(cPackid		,0,sizeof(cPackid));
	memset(cPayqsactno,0,sizeof(cPayqsactno));
	
	struct lv_pkgreg_c lv_pkgreg;
	struct lv_lbctl_c  lv_lbctl;
	memset(&payin			,0,sizeof(payin));
	memset(&lv_pkgreg	,0,sizeof(lv_pkgreg));
	memset(&lv_lbctl	,0,sizeof(lv_lbctl));
	vtcp_log("[%s][%d] spJ055��ʼ�\n",__FILE__,__LINE__);

	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	get_fd_data("1250",(char*)&fd125);
	vtcp_log("[%s],[%d] 125",__FILE__,__LINE__);
	vtcp_log("[%s]",(char*)&fd125);
	/****���payin*********/
	g_reply_int = iLvTisToPayin(&payin);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iHvTistoNotpay����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
  vtcp_log("[%s][%d]packid==[%s][%s]",__FILE__,__LINE__,payin.T_packid,payin.F_packid);
	/****ȡ����������****/
	g_reply_int = iLvFetchRecToPayin(&payin);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iLvFetchRecToPayin����,[%s][%d]", __FILE__,__LINE__);
		if(memcmp(g_pub_tx.reply,"0000",4)==0)
		{
			strcpy( g_pub_tx.reply, "P015" );
			set_zd_data("0130",acErrMsg);
		}
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]packid==[%s][%s]",__FILE__,__LINE__,payin.T_packid,payin.F_packid);
	/****��һЩ����Ҫ�����ֶ�ֱ�ӿ���****/
	memcpy(payin.F_ywtype,payin.T_ywtype,sizeof(payin.F_ywtype));
	memcpy(payin.T_feeflag,payin.F_feeflag,sizeof(payin.T_feeflag));
	memcpy(payin.F_packid,payin.T_packid,sizeof(payin.T_packid));
	vtcp_log("[%s][%d]packid==[%s][%s]",__FILE__,__LINE__,payin.T_packid,payin.F_packid);
	memcpy(payin.F_packday,payin.T_packday,sizeof(payin.T_packday));
	
	/****�������****/
	g_reply_int = iLvPayinCheck0(&payin);
	if(g_reply_int)
	{
		if(memcmp(g_pub_tx.reply,"0000",4)==0)
		{
			strcpy( g_pub_tx.reply, "HV01" );
		}
		set_zd_data("0130",acErrMsg);
                if(strlen(acErrMsg)==0)
		{
			sprintf( acErrMsg,"����������,[%s][%d]", __FILE__,__LINE__);
			set_zd_data("0130",acErrMsg);
		}
		WRITEMSG
		vtcp_log("����������,[%s][%d]", __FILE__,__LINE__);
		goto ErrExit;
	}
	/****Ҫ�ؼ��****/
	g_reply_int = iLvPayinCheck4(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"Ҫ�ؼ�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	/****�кż��****/
	g_reply_int = iLvPayinCheck1(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�кż�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	/***Add by SSH,2006.1.10,����Ƿ��Ѿ�ƴ��****/
	iPackday	= apatoi(fd125.pack_date,8);
	memcpy(cPackid		,fd125.packid			,sizeof(fd125.packid));
	memcpy(cPayqsactno,fd125.pay_qs_no	,sizeof(fd125.pay_qs_no));
	vtcp_log("[%s][%d]iPackday==[%d]",__FILE__,__LINE__,iPackday);
	vtcp_log("[%s][%d]cPackid==[%s]",__FILE__,__LINE__,cPackid);
	vtcp_log("[%s][%d]cPayqsactno==[%s]",__FILE__,__LINE__,cPayqsactno);
	
	g_reply_int = Lv_lbctl_Sel(g_pub_tx.reply,&lv_lbctl,"pack_date='%d' and packid='%s' and pay_qs_no='%s'",iPackday,cPackid,cPayqsactno);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,���ݿ�û�д˼�¼,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"���ݿ�û�д˼�¼,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	if(lv_lbctl.o_packid[0]!='\0')
	{
		vtcp_log("%s,%d,�˽����Ѿ�ƴ��,���ܸ��� ,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"�˽����Ѿ�ƴ��,���ܸ��� ,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	/****�����ʺż��****/
	if(payin.T_stat[0]!=STAT_WZLR)
	{
		vtcp_log("%s,%d,���ײ�Ϊ¼��״̬,���ܸ��� ,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"���ײ�Ϊ¼��״̬,���ܸ��� ,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}

	/***Add by SSH,2006.1.10,Ҫ��һ���ͼ��****/
	g_reply_int = iLvPayinCheck6(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,Ҫ��һ���ͼ��,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		set_zd_data("0130",acErrMsg);
		sprintf( acErrMsg,"Ҫ��һ���ͼ��,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	memcpy(payin.T_jzday,pcLvGetDay(),sizeof(payin.T_jzday));
	payin.T_stat[0]=STAT_WZCHECK2;
	vtcp_log("[%s][%d]packid==[%s][%s]",__FILE__,__LINE__,payin.T_packid,payin.F_packid);
	g_reply_int = iLvUpdRecFromPayin(&payin,0);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,���µǼǲ�ʧ��,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		set_zd_data("0130",acErrMsg);
		sprintf( acErrMsg,"���µǼǲ�ʧ��,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]packid==[%s][%s]",__FILE__,__LINE__,payin.T_packid,payin.F_packid);
	iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
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

	if(memcmp(payin.T_rcpstat,"00",2)==0)/**�ɹ���**/
	{
		goto OkExit1;
	}
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"��ִ���ɹ�[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
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
ilv10_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
