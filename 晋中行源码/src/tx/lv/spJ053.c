/*****************************************************
* �� �� ��:  spJ053.c
* ���������� ��ͨ��ǻ�ִҵ��¼��
* ��    ��:  xyy
* ������ڣ� 2006-9-7 16:03
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
static char cFlag[2];
static PAY_IN_AREA payin;
static PAY_IN_AREA payin_hz;
int spJ053()
{
	int iRc = 0;
	char cRcpstat[3];
	char cContent[61];
	struct lv_pkgreg_c lv_pkgreg;
	memset(&payin,0,sizeof(payin));
	memset(&payin_hz,0,sizeof(payin_hz));
	memset(cRcpstat,0,sizeof(cRcpstat));
	memset(cFlag,0,sizeof(cFlag));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	vtcp_log("[%s][%d] spJ053��ʼ�\n",__FILE__,__LINE__);

	pub_base_sysinit();	
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	get_fd_data("0980",cFlag);
	/****���payin*********/
	g_reply_int = iLvTisToPayin(&payin);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]����TisTopayin���� \n",__FILE__,__LINE__);
		sprintf( acErrMsg,"����TisTopayin����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin.F_wtday==[%.8s]\n",__FILE__,__LINE__,payin.F_wtday);
	memcpy(cRcpstat,payin.F_rcpstat,sizeof(cRcpstat)-1);
	iLvPayinAutoInfo(&payin);
	vtcp_log("lyly[%s][%d]payin.F_wtday==[%.8s]\n",__FILE__,__LINE__,payin.F_wtday);
	
	g_reply_int = lv23_Querycurr();
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]����lv23_Querycurr���� \n",__FILE__,__LINE__);
		sprintf( acErrMsg,"����lv23_Querycurr����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.F_owtday=[%s]",__FILE__,__LINE__,payin_hz.F_owtday);
	vtcp_log("[%s][%d]payin_hz.F_txnum=[%s]",__FILE__,__LINE__,payin_hz.F_txnum);
	/****�������****/
	payin_hz.F_crdb[0]=CRDB_DR;
	g_reply_int = iLvPayinCheck0(&payin_hz);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]���������� \n",__FILE__,__LINE__);
		sprintf( acErrMsg,"����������,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.F_owtday=[%s]",__FILE__,__LINE__,payin_hz.F_owtday);
	/****Ҫ�ؼ��****/
	g_reply_int = iLvPayinCheck4(&payin_hz);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]Ҫ�ؼ����� \n",__FILE__,__LINE__);
		sprintf( acErrMsg,"Ҫ�ؼ�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.F_owtday=[%s]",__FILE__,__LINE__,payin_hz.F_owtday);
	/****�кż��****/
	g_reply_int = iLvPayinCheck1(&payin_hz);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]�кż����� \n",__FILE__,__LINE__);
		sprintf( acErrMsg,"�кż�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.F_owtday=[%s]",__FILE__,__LINE__,payin_hz.F_owtday);
	vtcp_log("[%s][%d]F_orderno =[%s]",__FL__,payin_hz.F_orderno);
	payin_hz.F_lw_ind[0]='1';
	memcpy(payin_hz.F_rcpstat,cRcpstat,sizeof(cRcpstat)-1);
	vtcp_log("[%s][%d]payin_hz.F_owtday=[%s]",__FILE__,__LINE__,payin_hz.F_owtday);
	g_reply_int = iLvNewRecFromPayin(&payin_hz);
	vtcp_log("[%s][%d]payin_hz.T_owtday=[%s]",__FILE__,__LINE__,payin_hz.T_owtday);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]����iLvNewRecFromPayin���� \n",__FILE__,__LINE__);
		sprintf( acErrMsg,"����iLvNewRecFromPayin����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
  memcpy(payin.T_packid,"        ",sizeof(payin.T_packid));
  memcpy(payin.T_packday,"        ",sizeof(payin.T_packday));
	/***�����¼�¼****/
	if(cFlag[0]=='0')
	{
		g_reply_int=iLvUpdRecFromPayin(&payin_hz,1);
		vtcp_log("[%s][%d]payin_hz.T_owtday=[%s]",__FILE__,__LINE__,payin_hz.T_owtday);
	}
	else
	{
		g_reply_int=iLvUpdRecFromPayin(&payin_hz,0);
	}
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]����iLvUpdRecFromPayin���� \n",__FILE__,__LINE__);
		sprintf( acErrMsg,"����iLvUpdRecFromPayin11111111����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/* ����ԭ������ͨ��Ǳ��Ľ��ձ��״̬ */
	/****Modified by SSH,2005.12.26,Ӧ���ݻ�ִ״̬����ԭ����****/
	g_reply_int=iUpdRecvTableStat(&payin_hz);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]����iUpdRecvTableStat���� \n",__FILE__,__LINE__);
		sprintf( acErrMsg,"����iUpdRecvTableStat����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* ����ˮ��־ */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		strncpy(g_pub_tx.reply,"S023",4);
		WRITEMSG
		goto ErrExit;
	}
	iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		WRITEMSG
		goto ErrExit;
	}
	/*****д������ϸ�� ******/
  if(memcmp(payin_hz.T_rcpstat,"00",2))
		goto OkExit1;
OkExit:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"��ִΪ��ͬ��[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
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
int lv23_Querycurr()
{
	vtcp_log("[%s][%d]payin.F_wtday==[%.12s]\n",__FILE__,__LINE__,payin.F_wtday);
	int iRc=0;
	char cPkgno1[4];
  char cPkgno2[4];
  char cOpcd  [3];
  
  memset(cOpcd  ,0,sizeof(cOpcd));
  memset(cPkgno1,0,sizeof(cPkgno1));
  memset(cPkgno2,0,sizeof(cPkgno2));
	if(cFlag[0]=='1')/****�޸�***/
	{
		printf("[%s][%d] crdb===[%c] \n",__FILE__,__LINE__,payin.F_crdb[0]);
		payin.F_lw_ind[0]='2';
		payin.T_lw_ind[0]='2';
		
		memcpy(cOpcd  ,payin.F_opcd ,sizeof(cOpcd  )-1);
		memcpy(cPkgno1,payin.F_pkgno,sizeof(cPkgno1)-1);
		memcpy(cPkgno2,payin.T_pkgno,sizeof(cPkgno2)-1);
		memcpy(payin.F_pkgno,"002",3);
		memcpy(payin.T_pkgno,"002",3);
		memcpy(payin.F_opcd,"XX",2);
		vtcp_log("[%s][%d] F_pkgno===[%.3s] \n",__FILE__,__LINE__,payin.F_pkgno);
		if(iLvFetchRecToPayin(&payin))
		{
			vtcp_log("[%s][%d]����iLvFetchRecToPayin���� \n",__FILE__,__LINE__);
			sprintf( acErrMsg,"����iLvFetchRecToPayin����,[%s][%d]", __FILE__,__LINE__);
			WRITEMSG
			return 1;
		}
		iRc=iLvResetPayin(&payin);
		vtcp_log("[%s][%d]payin.A_stat====[%c] \n",__FILE__,__LINE__,payin.A_stat[0]);
		if(payin.A_stat[0]!=STAT_YES_NOTSEND && payin.A_stat[0]!=STAT_NO_NOTSEND)
		{
			vtcp_log("[%s][%d]����¼��״̬�������޸� \n",__FILE__,__LINE__);	
			sprintf( acErrMsg,"����¼��״̬�������޸�,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			return 1;
		}
		if(!memcmp(payin.T_rcpstat,"20",2 ))
		{
			vtcp_log("[%s][%d]��ǰ�����Ѿ������������޸� \n",__FILE__,__LINE__);
			sprintf( acErrMsg,"��ǰ�����Ѿ������������޸�,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			return 1;
		}
		memcpy(payin.F_pkgno,cPkgno1,sizeof(cPkgno1)-1);
		memcpy(payin.T_pkgno,cPkgno2,sizeof(cPkgno2)-1);
		memcpy(payin.F_opcd ,cOpcd  ,sizeof(cOpcd  )-1);
	}
	if(iRc!=0)
	{
		vtcp_log("[%s][%d]û��δ��˵����� \n",__FILE__,__LINE__);
		sprintf( acErrMsg,"û��δ��˵�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		return 1;
	}
	else
	{
		memcpy(payin.F_pkgno,PKGNO_PTJJ,sizeof(payin.F_pkgno));
		if(payin.T_stat[0]=='V' && memcmp(payin.F_rcpstat,"20",2)!=0 )
		{
			vtcp_log("[%s][%d]�Ѿ���ֹ��ҵ��ֻ��ѡ���ѳ��� \n",__FILE__,__LINE__);
			sprintf( acErrMsg,"�Ѿ�ֹ��ҵ��ֻ��ѡ���ѳ���,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			return 1;
		}
		if(cFlag[0]=='0')/**¼��**/
		{
			iRc=iLvCreateHz(&payin,&payin_hz);
			vtcp_log("[%s][%d]payin.F_wtday==[%.12s]\n",__FILE__,__LINE__,payin.F_wtday);
			vtcp_log("[%s][%d]payin_hz.F_owtday=[%.12s]\n",__FILE__,__LINE__,payin_hz.F_owtday);
			if(iRc!=0)
			{
				vtcp_log("[%s][%d]û��δ��˵����� \n",__FILE__,__LINE__);
				sprintf( acErrMsg,"û��δ��˵�����,[%s][%d]", __FILE__,__LINE__);
				strcpy( g_pub_tx.reply, "HV01" );
				WRITEMSG
				return 1;
			}
		}
		else
		{
			memcpy(payin_hz.F_orderno ,payin.F_oorderno  ,sizeof(payin.F_oorderno));
			memcpy(payin_hz.F_orbrno,payin.F_o_or_br_no,sizeof(payin.F_o_or_br_no));
			memcpy(payin_hz.F_wtday ,payin.F_owtday ,sizeof(payin.F_owtday));
			payin_hz.F_lw_ind[0]='1';
			memcpy(payin_hz.F_opcd,OPCD_LR,2);
			g_reply_int = iLvFetchRecToPayin(&payin_hz);
			if(g_reply_int)
			{
				sprintf( acErrMsg,"����iLvFetchRecToPayin����,[%s][%d]", __FILE__,__LINE__);
				WRITEMSG
				return 1;
			}
			iRc=iLvResetPayin(&payin_hz);
		}
		memcpy(payin_hz.F_rcpstat,payin.F_rcpstat,sizeof(payin_hz.F_rcpstat));
	}
	return 0;
}
ilv10_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin_hz);
	return(0);
}
