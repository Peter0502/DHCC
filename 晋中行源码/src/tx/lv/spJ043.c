/*****************************************************
* �� �� ��:  spJ043.c
* ���������� ���ڴ���ҵ��¼��
* ��    ��:  xyy
* ������ڣ� 2006-9-4 17:02
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
static PAY_IN_AREA payin;
int spJ043()
{
	int iRc = 0;
	int ret = 0;
	struct lv_pkgreg_c lv_pkgreg;
	struct lv_pkgreg_c o_lv_pkgreg;
	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));
	memset(&o_lv_pkgreg,0,sizeof(o_lv_pkgreg));
	vtcp_log("[%s][%d] spJ043��ʼ�\n",__FILE__,__LINE__);

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
	/****����Ҫ����Ҫ��ֱ�ӿ���****/
	memcpy(payin.T_feeflag,payin.F_feeflag,sizeof(payin.T_feeflag));
	memcpy(payin.T_feeamt,payin.F_feeamt,sizeof(payin.T_feeamt));
	memcpy(payin.T_vocnum,payin.F_vocnum,sizeof(payin.T_vocnum));
	memcpy(payin.T_voctype,payin.F_voctype,sizeof(payin.T_voctype));

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
	/*****add by liuyue   20061218����ԭ���ʵ�lv_o_sts�ֶΣ�����˻�ʱ�� ȡ�� Ҫ��ԭԭ������״̬�� ����
	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"orderno=%.8s and wt_date='%.8s' and or_br_no='%.12s' ",
			payin.T_oorderno,payin.T_owtday,payin.T_o_or_br_no);
	if(ret)
	{
		vtcp_log("[%s][%d]���ݿ��������!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	ret = Lv_pkgreg_Fet_Upd(&o_lv_pkgreg,g_pub_tx.reply);
	if(ret && ret !=100)
	{
		vtcp_log("[%s][%d]���ݿ��������!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	if(ret==100)
	{
		vtcp_log("[%s][%d]����Ԫ���� ����!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","����ԭ���ʼ�¼����!");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]xier   lv_o_sts=[%c],lv_sts==[%c]\n",__FILE__,__LINE__,o_lv_pkgreg.lv_o_sts[0],o_lv_pkgreg.lv_sts[0]);
	o_lv_pkgreg.lv_o_sts[0] = o_lv_pkgreg.lv_sts[0];
	vtcp_log("[%s][%d]xier   lv_o_sts=[%c],lv_sts==[%c]\n",__FILE__,__LINE__,o_lv_pkgreg.lv_o_sts[0],o_lv_pkgreg.lv_sts[0]);
	ret = Lv_pkgreg_Upd_Upd(o_lv_pkgreg,g_pub_tx.reply);
	if(ret)
	{
		vtcp_log("[%s][%d]�������ݿ� ����!\n",__FILE__,__LINE__);
		set_zd_data("0130","����ԭ����״̬����!\n");
		goto ErrExit;
	}
	
	Lv_pkgreg_Clo_Upd();

	vtcp_log("[%s][%d]����ԭ���ʽ���״̬ �ɹ�!\n",__FILE__,__LINE__);

	 ��Ӧ������� ȥ����   add end by liuyue 20061218****/
	/***�����¼�¼****/
	g_reply_int = iLvUpdRecFromPayin(&payin,1);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"�����¼�¼,[%s][%d]", __FILE__,__LINE__);
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
