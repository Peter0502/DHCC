/*****************************************************
* �� �� ��:  spJ054.c
* ���������� ���ڽ�ǻ�ִҵ��¼��
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
int lv23_Querycurr(PAY_IN_AREA *payin_hz,PAY_IN_AREA *payin,char flag);
int spJ054()
{
	int iRc = 0;
	int ret = 0;
	char cRcpstat[3];
	struct lv_pkgreg_c lv_pkgreg;

  memset(cRcpstat,0,sizeof(cRcpstat));
	memset(&payin,0,sizeof(payin));
	memset(&payin_hz,0,sizeof(payin_hz));
	memset(cFlag,0,sizeof(cFlag));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));


	vtcp_log("[%s][%d] ��ʼ���ڽ�ǻ�ִ¼��!\n",__FILE__,__LINE__);

	/*********************
	 * 1. ��ʼ��ȫ�ֱ��� *
	 *********************/

	pub_base_sysinit();	
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/*********************
	 * 2. ��ȡ8583������ *
	 *********************/

	get_fd_data("0980",cFlag);
	ret = iLvTisToPayin(&payin);
	if(ret)
	{
		vtcp_log("[%s][%d]����TisTopayin���� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	memcpy(cRcpstat,payin.F_rcpstat,sizeof(cRcpstat)-1);
	vtcp_log("xier[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	vtcp_log("xier[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	vtcp_log("[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	ret = iLvPayinAutoInfo(&payin);
	if(ret)
	{
		vtcp_log("[%s][%d]iLvPayinAutoInfo���� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/********************
	 * 3. ���н��׼��  *
	 ********************/
	vtcp_log("[%s][%d]cFlag===[%c]\n",__FILE__,__LINE__,cFlag[0]);
	vtcp_log("[%s][%d]payin.pkgno===[%s]\n",__FILE__,__LINE__,payin.F_pkgno);
	ret = lv23_Querycurr(&payin_hz,&payin,cFlag[0]);
	if(ret)
	{
		vtcp_log("[%s][%d]����lv23_Querycurr���� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/****�������****/
	ret = iLvPayinCheck0(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]���������� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/****Ҫ�ؼ��****/
	ret = iLvPayinCheck4(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]Ҫ�ؼ����� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/****�кż��****/
	ret = iLvPayinCheck1(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]�кż����� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/*end by LiuHuafeng 2007-1-16 14:48 */
	/******************
	 * 4. ���ݿ����  *
	 ******************/	
	vtcp_log("xier[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	vtcp_log("xier[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	vtcp_log("[%s][%d]F_orderno =[%s]",__FL__,payin_hz.F_orderno);
	payin_hz.F_lw_ind[0]='1';
	memcpy(payin_hz.F_rcpstat,cRcpstat,sizeof(cRcpstat)-1);
	ret = iLvNewRecFromPayin(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]����iLvNewRecFromPayin���� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.T_o_packid==[%s]\n",__FILE__,__LINE__,payin_hz.T_o_packid);
	vtcp_log("[%s][%d]payin_hz.T_o_pack_date==[%s]\n",__FILE__,__LINE__,payin_hz.T_o_pack_date);
		
	vtcp_log("[%s][%d]payin_hz.rcpstat==[%s]\n",__FILE__,__LINE__,payin_hz.F_rcpstat);
	vtcp_log("[%s][%d]payin_hz.rcpstat==[%s]\n",__FILE__,__LINE__,payin_hz.F_rcpstat);

	/***�����¼�¼****/
	if(cFlag[0]=='0')
	{
		iLvResetPayin(&payin_hz);	
		vtcp_log("[%s][%d]payin_hz.rcpstat==[%s]\n",__FILE__,__LINE__,payin_hz.T_rcpstat);
		ret=iLvUpdRecFromPayin(&payin_hz,1);
	}
	else
	{
		iLvResetPayin(&payin_hz);
		vtcp_log("[%s][%d]payin_hz.rcpstat==[%s]\n",__FILE__,__LINE__,payin_hz.T_rcpstat);
		ret=iLvUpdRecFromPayin(&payin_hz,0);
	}
	if(ret)
	{
		vtcp_log("[%s][%d]����iLvUpdRecFromPayin���� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.T_rcpstat==[%s]",__FILE__,__LINE__,payin_hz.T_rcpstat);
	/****Modified by SSH,2005.12.26,Ӧ���ݻ�ִ״̬����ԭ����****/


	vtcp_log("[%s][%d]!!!!!!!!!!!!!!!!!!!!!\n",__FILE__,__LINE__);
	print_payin_ly(&payin_hz);
	vtcp_log("[%s][%d]!!!!!!!!!!!!!!!!!!!!!\n",__FILE__,__LINE__);
	ret=iUpdRecvTableStat(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]����iUpdRecvTableStat���� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/* ���෽Э��� begin add by LiuHuafeng 2007-1-16 14:48*/
	ret = iLvPayinCheck9(&payin_hz);
	if(ret)
	{
		if(memcmp(cRcpstat,"00",2)==0)
		{
			vtcp_log("[%s][%d]Ӧ��Ϊ�ɹ����෽Э��ż����� \n",__FILE__,__LINE__);
			set_zd_data("0130",acErrMsg);
			strcpy( g_pub_tx.reply, "HV01" );
			goto ErrExit;
		}
	}


	/****************
	 * 5. д��ˮ��  *
	 ****************/	
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		strncpy(g_pub_tx.reply,"S023",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.T_rcpstat==[%s]",__FILE__,__LINE__,payin_hz.T_rcpstat);
	/************************
	 * 6. ׼��ǰ̨�������  *
	 ************************/
	iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.T_rcpstat==[%s]",__FILE__,__LINE__,payin_hz.T_rcpstat);
	if(memcmp(payin_hz.T_rcpstat,"00",2))
		goto OkExit1;
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	sprintf(acErrMsg,"��ִΪͬ��[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

OkExit1:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"��ִΪ��ͬ��[%s]\n",g_pub_tx.reply);
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


int lv23_Querycurr(PAY_IN_AREA *payin_hz,PAY_IN_AREA *payin,char flag)
{
	int iRc=0;
  char cPkgno1[4];
  char cPkgno2[4];
  char cOpcd  [3];
  
  memset(cOpcd  ,0,sizeof(cOpcd));
  memset(cPkgno1,0,sizeof(cPkgno1));
  memset(cPkgno2,0,sizeof(cPkgno2));
	if(flag=='1')/****�޸�***/
	{
		vtcp_log("[%s][%d] crdb===[%c] \n",__FILE__,__LINE__,payin->F_crdb[0]);
		
		memcpy(cOpcd  ,payin->F_opcd  ,sizeof(cOpcd  )-1);
		memcpy(cPkgno1,payin->F_pkgno,sizeof(cPkgno1)-1);
		memcpy(cPkgno2,payin->T_pkgno,sizeof(cPkgno2)-1);
		memcpy(payin->F_pkgno,"006",3);
		memcpy(payin->T_pkgno,"006",3);
		vtcp_log("[%s][%d] F_pkgno===[%.3s] \n",__FILE__,__LINE__,payin->F_pkgno);
		if(iLvFetchRecToPayin(payin))
		{
			vtcp_log("[%s][%d]����iLvFetchRecToPayin���� \n",__FILE__,__LINE__);
			return 1;
		}

		iRc=iLvResetPayin(payin);
		if(payin->A_stat[0]!=STAT_YES_NOTSEND&&payin->A_stat[0]!=STAT_NO_NOTSEND )
		{
			vtcp_log("[%s][%d]����¼��״̬�������޸� \n",__FILE__,__LINE__);	
			strcpy( g_pub_tx.reply, "HV01" );
			return 1;
		}
    vtcp_log("[%s][%d] T_rcpstat===[%.2s] \n",__FILE__,__LINE__,payin->T_rcpstat);
		if(!memcmp(payin->T_rcpstat,"20",2 ))
		{
			vtcp_log("[%s][%d]��ǰ�����Ѿ������������޸� \n",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			return 1;
		}
		memcpy(payin->F_pkgno,cPkgno1,sizeof(cPkgno1)-1);
		memcpy(payin->T_pkgno,cPkgno2,sizeof(cPkgno2)-1);
		memcpy(payin->F_opcd ,cOpcd  ,sizeof(cOpcd  )-1);
	}
	if(iRc!=0)
	{
		vtcp_log("[%s][%d]û��δ��˵����� \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		return 1;
	}
	else
	{
		memcpy(payin->F_pkgno,PKGNO_DQJJ,sizeof(payin->F_pkgno));
		vtcp_log("[%s][%d] F_pkgno===[%.3s] \n",__FILE__,__LINE__,payin->F_pkgno);
		if(payin->T_stat[0]=='V' && memcmp(payin->F_rcpstat,"20",2)!=0 )
		{
			vtcp_log("[%s][%d]�Ѿ���ֹ��ҵ��ֻ��ѡ���ѳ��� \n",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			return 1;
		}
		if(flag=='0')
		{
			iRc=iLvCreateHz(payin,payin_hz);
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
			memcpy(payin_hz->F_orderno ,payin->F_oorderno  ,sizeof(payin->F_oorderno));
			memcpy(payin_hz->F_orbrno  ,payin->F_o_or_br_no,sizeof(payin->F_o_or_br_no));
			memcpy(payin_hz->F_wtday   ,payin->F_owtday    ,sizeof(payin->F_owtday));
			payin_hz->F_lw_ind[0]='1';
			payin_hz->T_lw_ind[0]='1';
			memcpy(payin_hz->F_opcd,OPCD_LR,2);
			g_reply_int = iLvFetchRecToPayin(payin_hz);
			if(g_reply_int)
			{
				sprintf( acErrMsg,"����iLvFetchRecToPayin����,[%s][%d]", __FILE__,__LINE__);
				WRITEMSG
				return 1;
			}
			iRc=iLvResetPayin(payin_hz);
		}
		memcpy(payin_hz->F_rcpstat,payin->F_rcpstat,sizeof(payin_hz->F_rcpstat));
	}
	return 0;
}


/****************
 * ����������� *
 ****************/
int ilv10_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin_hz);
	return(0);
}
