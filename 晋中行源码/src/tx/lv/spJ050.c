/*****************************************************
* �� �� ��:  spJ050.c
* ���������� ��ͨ���ҵ��¼��
* ��    ��:  xyy
* ������ڣ� 2006-9-4 9:24 
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
#include "lv_param_c.h"
static PAY_IN_AREA payin;
int spJ050()
{
	int iRc = 0;
	struct lv_pkgreg_c lv_pkgreg;
	struct lv_param_c  lv_param;
	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	memset(&lv_param,0,sizeof(lv_param));
	vtcp_log("[%s][%d] spJ050��ʼ�\n",__FILE__,__LINE__);

	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/****���payin****/
	g_reply_int = iLvTisToPayin(&payin);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iHvTistoNotpay����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
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
		strcpy( g_pub_tx.reply, "P311" );
		WRITEMSG
		goto ErrExit;
	}
		/* �����Ѽ�� */
	if(payin.F_feeflag[0]!='4')
	g_reply_int = iLvCountFeeamt(&payin);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,���������Ѵ���,sqlcode=%d!\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"���������Ѵ���,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	/****�տ��ʺż��****/
	g_reply_int=iLvPayinCheck3(&payin);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�տ��ʺż�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d] payactno[%.32s]\n",__FILE__,__LINE__,payin.F_payactno);
	vtcp_log("[%s][%d] cashactno[%.32s]\n",__FILE__,__LINE__,payin.F_cashactno);
	int iOrderno;
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
	vtcp_log("[%s][%d]===========wd_payin->F_txamt=[%.15s]\n",__FL__,payin.F_txamt);
	vtcp_log("[%s][%d]===========wd_payin->T_txamt=[%.15s]\n",__FL__,payin.T_txamt);
	vtcp_log("[%s][%d]===========wd_payin->T_stat=[%.1s]\n",__FL__,payin.T_stat);
	/***�����¼�¼****/
	g_reply_int = iLvUpdRecFromPayin(&payin,1);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�����¼�¼,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	
	/******************** ���������� 
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
	***************************/
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
	/****Add by SSH,2007.5.15,�����֧Ʊҵ���͵���ӡ������****/
	/**Ӱ������˸��˽��ף����͸ĵ����˽������淢��
	if(memcmp(payin.F_txnum,TXNUM_ZPJL,5)==0||
		memcmp(payin.F_txnum,TXNUM_TYJL,5)==0){
	        ****ȡ��ִ����***
                memcpy(lv_param.par_code,"WDBRTDAY",8);
                g_reply_int = Lv_param_Sel(g_pub_tx.reply, &lv_param, "par_code= '%s'", lv_param.par_code);
                if(g_reply_int){
                        strcpy(payin.T_respday,"1");
                }else{
                        memcpy(payin.T_respday,lv_param.par_value,sizeof(payin.T_respday));
                }
		iRc=SendZpCifToYy(&payin);
		if(iRc){
			sprintf(acErrMsg,"%s,%d ���͵���ӡ������ʧ��!",__FL__);
			strncpy(g_pub_tx.reply,"LS88",4);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�޷����ͱ��ĵ���ӡ������");	
			goto ErrExit;
		}
		**����ɹ�ֱ�Ӹ���״̬Ϊ����**
		payin.T_stat[0]=STAT_WZSEND;
		g_reply_int = iLvUpdRecFromPayin(&payin,0);
		if(g_reply_int)
		{
			sprintf( acErrMsg,"�����¼�¼,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
	}**/
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
ilv10_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
