/***************************************************************
* �� �� ��:   spJ021.c
* ����������  ��ѯ����¼��,�˽����Ƕ��Ѿ�¼��ĸ��ֽ��ױ��Ľ��в�ѯ
*
* ��    ��:   xyy
* ������ڣ�  2006-8-18 20:11
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#define loc_t sys_loc_t
#include "find_debug.h"

spJ021()
{
	int  ret=0;	
	int  iOrderno=0;
	int  iOwtdate=0;
    double dTxamt=0.0;
    
    char cOorbrno[13];
	char cTxamt[33];
	char cAcbrno[13];
	char cOorderno[9];
	char cBuff	[2000];

	HV_NOTPAY_AREA hv_notpay;
	THVPACKAGE pack301;
	struct hv_uniontab_c hv_uniontab;
	struct hv_fzf_c hv_fzf;
	struct hv_zf_c hv_zf;

	memset(cAcbrno ,0 ,sizeof(cAcbrno));
	memset(cOorbrno,0,sizeof(cOorbrno));
	memset(cTxamt ,0,sizeof(cTxamt));
	memset(cBuff  ,0,sizeof(cBuff));
	memset(cOorderno,0,sizeof(cOorderno));
	memset(&hv_notpay,0,sizeof(hv_notpay));
	memset(&hv_uniontab,0,sizeof(hv_uniontab));
	memset(&hv_fzf,0,sizeof(hv_fzf));
	memset(&hv_zf,0,sizeof(hv_zf));
	memset(&pack301, 0 , sizeof(pack301));

	vtcp_log("[%s][%d]spJ021��ѯ����¼��BEGIN",__FILE__,__LINE__);
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();


	/***�õ����֧��ҵ��8583����***/
	g_reply_int = iHvTistoNotpay(&hv_notpay);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"����iHvTistoNotpay����![%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	
	/******���Ӫҵ��״̬******/ 

	g_reply_int = iHv_checkstat(hv_notpay.F_cmtno);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"����״̬������![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("[%s][%d]������iHv_checkstat",__FILE__,__LINE__);

	vtcp_log("[%s][%d]hv_notpay.F_o_txamt=[%s]",__FILE__,__LINE__,hv_notpay.F_o_txamt);
	dot_str2dbl(hv_notpay.F_o_txamt,sizeof(hv_notpay.F_o_txamt),&dTxamt);
	vtcp_log("[%s][%d]dTxamt=[%f]",__FILE__,__LINE__,dTxamt);
	
	if(memcmp(hv_notpay.F_or_br_no,hv_notpay.F_ac_br_no, sizeof(cAcbrno) -1 ) == 0 )
	{
		sprintf( acErrMsg,"��ѯ�кͲ鸴�в���Ϊͬһ��![%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data("0130","��ѯ�кͲ鸴�в���Ϊͬһ��");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]-----------------------------",__FILE__,__LINE__);

	/**�鸴��**/
	MEMCPY_DEBUG(cAcbrno,hv_notpay.F_ac_br_no,sizeof(cAcbrno)-1);
	g_reply_int = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no = '%s' ",cAcbrno);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"��ѯ�����л�����Ϣ����!,[%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}

	if(hv_uniontab.br_sts[0]!='1')
	{
		sprintf( acErrMsg,"�鸴�л���״̬������Ч״̬![%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	
	iOwtdate = apatoi(hv_notpay.F_o_wt_date,8);
	vtcp_log("[%s][%d]iOwtdate=[%d] ",__FILE__,__LINE__,iOwtdate);
	if ( iOwtdate - g_pub_tx.tx_date >0 )
	{
		vtcp_log("[%s][%d]iOwtdate=[%d] g_pub_tx.tx_date=[%d]",__FILE__,__LINE__,iOwtdate,g_pub_tx.tx_date);
		sprintf( acErrMsg,"ԭί�����ڲ��ܴ��ڵ�ǰ����![%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]-----------------------------",__FILE__,__LINE__);
	/*���ԭ���׵��Ƿ���ڣ�����*/
	MEMCPY_DEBUG(cOorbrno,hv_notpay.F_o_or_br_no,sizeof(cOorbrno)-1);
	
	MEMCPY_DEBUG(cOorderno,hv_notpay.F_o_payin_orderno,sizeof(cOorderno)-1);
	g_reply_int = Hv_zf_Sel(g_pub_tx.reply,&hv_zf,"or_br_no = '%s' and wt_date = '%d' and orderno= '%s' and cmtno='%s' ",cOorbrno,iOwtdate,cOorderno,hv_notpay.F_o_cmtno);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]iOwtdate=[%d]",__FILE__,__LINE__,iOwtdate);
		vtcp_log("[%s][%d]F_o_or_br_no=[%s]",__FILE__,__LINE__,hv_notpay.F_o_or_br_no);
		sprintf( acErrMsg,"���ݿ���û��������¼![%s][%d]", __FILE__,__LINE__);
		WRITEMSG
	}
	else
	{
		vtcp_log("[%s][%d]hv_zf.hv_sts=[%c]",__FILE__,__LINE__,hv_zf.hv_sts[0]);
		if ( hv_zf.hv_sts[0] == SEND_REGIST || hv_zf.hv_sts[0] == SEND_ACCT_CHECK \
		|| hv_zf.hv_sts[0] == SEND_REVERSE_REG || hv_zf.hv_sts[0] == SEND_REVERSE_ACCT)
		{
			sprintf( acErrMsg,"��¼״̬��![%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
		
		vtcp_log("[%s][%d]hv_zf.tx_amt=[%f]",__FILE__,__LINE__,hv_zf.tx_amt);
		if (fabs(hv_zf.tx_amt-dTxamt)>0.001)
		{ 
			sprintf( acErrMsg,"ԭ���׽����ԭ��¼����![%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
	} 	
	g_reply_int = iHvfFrontToTable(&hv_notpay,cBuff);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"����iHvfFrontToTable����![%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P018" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]-----------------------------",__FILE__,__LINE__);
	vtcp_log("[%s][%d]hv_notpay->T_otype[0]=[%c]",__FILE__,__LINE__,hv_notpay.T_otype[0]);
	/**����״̬**/
	hv_notpay.T_hv_fzf_sts[0]='1';
	/****add by liuyue ���е�ʱ��301������req_br_noλ05λ****/
	MEMCPY_DEBUG(hv_notpay.T_req_br_no,hv_notpay.T_or_br_no,sizeof(hv_notpay.T_req_br_no));
	/******end********/
	g_reply_int = iHvUpdFzfRecFromPayin(&hv_notpay,1);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"��iHvUpdFzfRecFromPayin����[%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay->T_otype[0]=[%c]",__FILE__,__LINE__,hv_notpay.T_otype[0]);
	vtcp_log("[%s][%d]-----------------------------",__FILE__,__LINE__);
	/*** ��ʼд��ʷ��ϸ ***/ 

	g_reply_int = dg_hv_fzfhtr(&hv_notpay);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"�������ɸ�ʽ��ϸ�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay->T_otype[0]=[%c]",__FILE__,__LINE__,hv_notpay.T_otype[0]);
	
	/*******************
     	*  ׼����ˮ  add by martin 2008/12/22 14:30:01     *
     	*******************/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
	/****Add by SSH,2006.8.28,ֱ�ӷ���****/
	pub_com_CMT301_T2TAG(&hv_notpay,&pack301);
	ret=hv_gen_pack(&pack301);
	if(ret<0){
		sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack301,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
			,hv_notpay.T_trace_no,hv_notpay.T_orderno)<0){
		sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack301)){
		sprintf(acErrMsg,"����301���ĵ�ǰ�û�ʧ��!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	{
		char pcBuf[1024];
		memset(pcBuf,'\0',sizeof(pcBuf));
		get_fd_data("1230",pcBuf);
		vtcp_log("[%s][%d]Gethere,123 area \n[%s]",__FILE__,__LINE__,pcBuf);
	}
	vtcp_log("[%s][%d]--------GAME OVER!!!--------------",__FILE__,__LINE__);
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);

	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
