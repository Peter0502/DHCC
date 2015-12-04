/***************************************************************
* �� �� ��:     spJ003.c
* ����������	���֧���˻�ҵ��¼��
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
#include "hv_zf_c.h"
#include "find_debug.h"

spJ003()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin,hv_th_payin;
	struct hv_zf_c o_hv_zf;
	memset(&o_hv_zf,0x00,sizeof(o_hv_zf));
	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_th_payin, 0 , sizeof(hv_th_payin));
	memset(&hv_payin, 0 , sizeof(hv_payin));
	/***�õ����֧��ҵ��8583����***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		sprintf(acErrMsg,"���ú���iHvTistoPayinʧ��!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"���ú���iHvTistoPayinʧ��");
		goto ErrExit;
	}
	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* ׼��д��Ǽǲ������� */
	ret=iHvFrontToTable(&hv_payin);
	if( ret )
	{
		sprintf(acErrMsg,"���ú���iHvFrontToTableʧ��!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"���ú���iHvFrontToTableʧ��");
		goto ErrExit;
	}
	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* ���Ƿ��ַ� */
	/*****20070119 liuyue  �˻㲻��������Ǽ��
	ret=iHvPayin_characterCheck(&hv_payin,acErrMsg);
	if(ret)
	{
		vtcp_log("[%s][%d]���ú���iHvPayin_characterCheckʧ��!\n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	************************************/

	ret = iCharacterType(hv_payin.F_hv_brf,sizeof(hv_payin.F_hv_brf),'g');
	if(ret)
	{
		vtcp_log("%s,%d,����ĸ�����[%.60s]��%dλ���ڷǷ��ַ�",__FILE__,__LINE__,hv_payin.F_hv_brf,ret);
		strncpy(acErrMsg,"����ĸ����д��ڷǷ��ַ�",50);
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	vtcp_log("[%s][%d]oper==[%c],cmtno==[%s]\n",__FILE__,__LINE__,hv_payin.F_operlevel[0],hv_payin.F_cmtno);
	if(hv_payin.F_operlevel[0]=='0' &&
		 (!memcmp(hv_payin.F_cmtno,"100",sizeof(hv_payin.F_cmtno))
		 ||!memcmp(hv_payin.F_cmtno,"101",sizeof(hv_payin.F_cmtno))
		 ||!memcmp(hv_payin.F_cmtno,"102",sizeof(hv_payin.F_cmtno))
	     ||!memcmp(hv_payin.F_cmtno,"103",sizeof(hv_payin.F_cmtno))
	     ||!memcmp(hv_payin.F_cmtno,"105",sizeof(hv_payin.F_cmtno))
	     ||!memcmp(hv_payin.F_cmtno,"108",sizeof(hv_payin.F_cmtno))
        )
    )
    {
        char cInputTxamt[17];
        double dInputTxamt=0.0;
        double dMinTxamt=0.0;
        dMinTxamt=dGetHvMinTxamt();
        memset(cInputTxamt , 0 ,sizeof(cInputTxamt));
        memcpy(cInputTxamt, hv_payin.F_tx_amt,sizeof(cInputTxamt)-1);
        zip_space(cInputTxamt);
        str2dbl(cInputTxamt,strlen(cInputTxamt),0,&dInputTxamt);
            vtcp_log("%s,%d ����Ľ��=[%f], ��С���=[%f] ",__FILE__,__LINE__,dInputTxamt , dMinTxamt);
        if(dInputTxamt < 20000.00)
        {
            vtcp_log("%s,%d ����Ľ��=[%f], ��С���=[%f] ",__FILE__,__LINE__,dInputTxamt , dMinTxamt);
            vtcp_log("%s,%d ��ѡ��Ӽ����ؼ����ȼ���",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"��ѡ��Ӽ����ؼ����ȼ���");
			goto ErrExit;
            WRITEMSG
        }
    }

	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	ret=iHv_checkstat(hv_payin.F_cmtno,hv_payin.F_yw_type);
	if(ret)
	{
		sprintf(acErrMsg,"���ú���iHv_checkstatʧ��!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,"���ϵͳ���ڷǷ�/״̬������");
		goto ErrExit;
	}
	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* �õ�ԭ��������Ϣ */
	
	/*****add by liuyue 20061219 �ı�ԭ�����˵�o_sts�ֶ�**/
	ret = Hv_zf_Dec_Upd(g_pub_tx.reply,"orderno='%.8s' and wt_date='%.8s' and or_br_no='%.12s' ",
			hv_payin.F_o_orderno,hv_payin.F_o_wt_date,hv_payin.F_ac_br_no);
	if(ret)
	{
		vtcp_log("[%s][%d]���ݿ��������!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","���ݿ��������!");
		goto ErrExit;
	}
	ret = Hv_zf_Fet_Upd(&o_hv_zf,g_pub_tx.reply);
	if(ret &&ret!=100)
	{
		vtcp_log("[%s][%d]���ݿ��������!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","���ݿ��������!");
		goto ErrExit;
	}
	else if(ret==100)
	{
		vtcp_log("[%s][%d]����ԭ���ʽ��״���!ret=[%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","����ԭ֧�����ʴ���");
		goto ErrExit;
	}

	/***���� ��¼ ***/
	o_hv_zf.hv_osts[0] = o_hv_zf.hv_sts[0];

	ret = Hv_zf_Upd_Upd(o_hv_zf,g_pub_tx.reply);
	if(ret)
	{
		vtcp_log("[%s][%d]����o_sts ����!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","���ݿ��������!");
		goto ErrExit;
	}
	
	Hv_zf_Clo_Upd();

	/****add end by liuyue 20061219****/
	ret=iHvGetThMsg(&hv_payin,&hv_th_payin,acErrMsg);
	if(ret)
	{
		vtcp_log("���ú���iHvGetThMsgʧ��!\n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	vtcp_log("%s,%d ����Ľ��׽��[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* �Ƚ�������Ϣ��ԭ������Ϣ�Ƿ�һ�� */
	ret=iHvCheckTh_input(&hv_payin,&hv_th_payin,acErrMsg);
	if(ret)
	{
		vtcp_log("[%s][%d]���ú���iHvCheckTh_inputʧ��!\n",__FILE__,__LINE__);
		WRITEMSG
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	/* д��Ǽǲ� */
	ret=iHvUpdZfRecFromPayin(&hv_payin,1);
	if( ret )
	{
		sprintf(acErrMsg,"���ú���iHvCheckTh_inputʧ��!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"���ú���iHvCheckTh_inputʧ��");
		goto ErrExit;
	}

	iHvReset8583(&hv_payin);
	vtcp_log("%s,%d new orderno===[%s][%s]",__FILE__,__LINE__,hv_payin.F_orderno,hv_payin.T_orderno);
	/* д��ҵ����ˮ */

	/*
	vtcp_log("[%s][%d]trace_no=[%d]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]tx_date=[%d]\n",__FILE__,__LINE__,g_pub_tx.tx_date);
	vtcp_log("[%s][%d]tx_time=[%d]\n",__FILE__,__LINE__,g_pub_tx.tx_time);
	vtcp_log("[%s][%d]tx_br_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_br_no);
	vtcp_log("[%s][%d]opn_br_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.opn_br_no);
	vtcp_log("[%s][%d]tx_code=[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
	vtcp_log("[%s][%d]sub_tx_code=[%s]\n",__FILE__,__LINE__,g_pub_tx.sub_tx_code);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);*/
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"�Ǽǽ�����ˮ����");
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
