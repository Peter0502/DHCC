/***************************************************************
* �� �� ��:     spJ004.c
* ����������	���֧���˻㸴��
*
* ��    ��:   LiuHuafeng
* ������ڣ�  2006-8-9 9:28
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "find_debug.h"

spJ004()
{
	int ret = 0;
	char cAc_no[25];
	char cKinbr[BRNO_LEN+1];
	HV_PAYIN_AREA hv_payin,hv_lz_payin;
	char cWssrno[7];
	char cTxday[9];
	memset(cTxday, 0 , sizeof(cTxday));
	
	memset(cAc_no, 0 , sizeof(cAc_no));
	memset(cKinbr, 0 , sizeof(cKinbr));
	get_zd_data(DC_TX_BR_NO,cKinbr);
	get_zd_data(DC_DATE,cTxday);
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_lz_payin, 0 , sizeof(hv_lz_payin));
	memset(&hv_payin, 0 , sizeof(hv_payin));
	/***�õ����֧��ҵ��8583����***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	ret=FetchRecToPayin(&hv_payin);
	if(ret)
	{
		WRITEMSG
		goto ErrExit;
	}
	ret=iHvCheckInput(&hv_payin,acErrMsg);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	iHvResetPayin(&hv_payin);
	/* ���Ƿ��ַ� */
	/**********�˻㽻�ײ��ü�������   by liuyue 20070205
	ret=iHvPayin_characterCheck(&hv_payin,acErrMsg);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	**********************************************/
	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	ret=iHv_checkstat(hv_payin.F_cmtno,hv_payin.F_yw_type);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* �õ�ԭ��������Ϣ������ԭ������״̬Ϊ�˻صȴ�״̬ */
	ret=iHvGetThMsg(&hv_payin,&hv_lz_payin,acErrMsg);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* ׼��д��Ǽǲ������� */
	ret=iHvFrontToTable(&hv_payin);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* д��Ǽǲ� */
	ret=iHvUpdZfRecFromPayin(&hv_payin,0);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d ��ʼ׼������",__FILE__,__LINE__);
	iPrepareTh_ac_no(&hv_payin);
		/** �õ������ʺ� */
		ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
		if(ret)
		{
			sprintf(acErrMsg,"�õ���������������ʺŴ���!\n");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			vtcp_log("%s,%d �õ���������������ʺŴ���\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"T042");
			goto ErrExit;
		}
		set_zd_data("0980","6");
		set_zd_data("0310",cAc_no);
		set_zd_data("0990","2");
		/** �õ������ʺ�  **/
	vtcp_log("%s,%d ��ʼ׼������",__FILE__,__LINE__);

	/* д��ҵ����ˮ */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	/*** ������� ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("112A",g_pub_tx.ac_no);
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
