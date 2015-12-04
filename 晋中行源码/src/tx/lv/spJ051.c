/*****************************************************
* �� �� ��:  spJ051.c
* ���������� ���ڽ��ҵ��¼��
* ��    ��:  liuyue 
* ������ڣ� 2006-9-20 09:00 
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
int spJ051()
{
	int iRc = 0;
	int ret = 0;
	int iOrderno = 0;
	struct lv_pkgreg_c lv_pkgreg;

	memset(&payin,0,sizeof(payin));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));

	vtcp_log("[%s][%d] ��ʼ���ڽ�ǵ�¼�����!\n",__FILE__,__LINE__);


	/********************
	 * 1.��ʼ��ȫ�ֱ��� *
	 ********************/
	pub_base_sysinit();	
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/*********************
	 * 2.��ȡǰ̨8583��  *
	 *********************/
	ret = iLvTisToPayin(&payin);
	if( ret )
	{
	sprintf( acErrMsg,"���ú���iHvTistoNotpay����,[%s][%d]", __FILE__,__LINE__);
	strcpy( g_pub_tx.reply, "P015" );
	WRITEMSG
	goto ErrExit;
	}
	

	/***********************
	 * 3. ���л������     *
	 ***********************/

	/*�������*/
	ret = iLvPayinCheck0(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"����������,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/*Ҫ�ؼ��*/
	ret = iLvPayinCheck4(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"Ҫ�ؼ�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/*�кż��*/
	ret = iLvPayinCheck1(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"�кż�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	/* �����Ѽ�� */

	if(payin.F_feeflag[0]!='4')
	ret = iLvCountFeeamt(&payin);
	if(ret)
	{
		vtcp_log("%s,%d,���������Ѵ���,sqlcode=%d!\n",__FILE__,__LINE__,ret);
		sprintf( acErrMsg,"���������Ѵ���,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/*�����µĽ������*/
	
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0)
	{
		sprintf( acErrMsg,"�õ�֧��������ų���,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);


	/*********************
	 * 4. ���ݿ����     *
	 *********************/

	ret = iLvNewRecFromPayin(&payin);
	if(ret)
	{
		sprintf( acErrMsg,"����һ���µļ�¼����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	/*�����¼�¼*/
	ret = iLvUpdRecFromPayin(&payin,1);
	if(ret)
	{
		sprintf( acErrMsg,"�����¼�¼,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}


	/************************
	 * 5.׼����־ 		*
	 ************************/

	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		strncpy(g_pub_tx.reply,"S023",4);
		WRITEMSG
		goto ErrExit;
	}

	/********************
	 * 6.׼���������   *
	 ********************/
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

/******************************
 * FUNCTIONS :ilv10_PreTos    *
 * PREPARE DATA FOR INTERFACE *
 ******************************/	
ilv10_PreTos()
{
	int iRc;
	/****�����������payout2****/
	iLvSetPayout1(&payin);
	return(0);
}
