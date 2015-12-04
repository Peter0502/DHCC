
/*************************************************
* �� �� ��:  spJ024.c
* ���������� ����֧���ཻ�����˷���
*
* ��    ��:  xyy 
* ������ڣ� 2024��8��9��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_pub.h"
#include "hvnet.h"
#include "hvpack.h"
#include "hv_define.h"
#include "find_debug.h"
#define loc_t sys_loc_t


int spJ024()
{	
	int  iCmtno = 0;
	int  ret    = 0;
	int  iRc   =0;
	int  iSocketID ;
	char cCmtno		[4];
	char cWssrno	[7];
	char cTel		[7];
	char cFindOrderno [9];
	char tmpBuf		[64];
	char cQsbrno[BRNO_LEN+1];
	
	HV_NOTPAY_AREA   hv_notpay;
	THVPACKAGE pack_fzf;
	
	memset(cCmtno			,0,sizeof(cCmtno));
	memset(cQsbrno			,0,sizeof(cQsbrno));
	memset(cFindOrderno		,0,sizeof(cFindOrderno));
	memset(cWssrno			,0,sizeof(cWssrno));
	memset(cTel			,0,sizeof(cTel));
	memset(&hv_notpay       	,0,sizeof(hv_notpay));
	memset(&pack_fzf		,'\0',sizeof(pack_fzf));
                  			
	pub_base_sysinit();

	sprintf(cWssrno,"%6d",g_pub_tx.trace_no);
	vtcp_log("[%s][%d]cWssrno=[%s]",__FILE__,__LINE__,cWssrno);
	vtcp_log("spJ024�[%s][%d]",__FILE__,__LINE__);
	
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
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	MEMCPY_DEBUG(cCmtno	,hv_notpay.F_cmtno	,sizeof(cCmtno)-1);
	MEMCPY_DEBUG(cTel	    ,hv_notpay.F_tel	,sizeof(cTel)-1);
	
	iCmtno=apatoi(cCmtno,3);

	/*����Щ���ײ������������ķ���*/
	if(memcmp(cCmtno,"724",sizeof(cCmtno)-1)==0 || memcmp(cCmtno,"314",sizeof(cCmtno)-1)==0 )
	{
		if(find_qsbrno(cQsbrno)) 
		{
			sprintf(acErrMsg,"ȡ���������ĺų���!\n");
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
		/***����Ƿ�����������****/
		if(memcmp(hv_notpay.F_tx_br_no,cQsbrno,BRNO_LEN)==0) 
		{
			sprintf(acErrMsg,"�������Ĳ������˽���!\n");
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
	}
	
	ret =	iHvTable2Notpayin(&hv_notpay);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к���iHvTable2Notpayin����!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	
	/* ����ˮ�Ŵ�����	*/
	MEMCPY_DEBUG(hv_notpay.T_trace_no,cWssrno,sizeof(cWssrno)-1);
	vtcp_log("[%s][%d]hv_notpay.T_trace_no=[%.6s]",__FILE__,__LINE__,hv_notpay.T_trace_no);
	
	if(memcmp(cCmtno,"314",sizeof(cCmtno)-1)==0)/*�жϸñ�ҵ���״̬*/
	{
		if( hv_notpay.T_hv_fzf_sts[0]!=SEND_ACCT_CHECK)
		{
			sprintf(acErrMsg,"״̬���Ǹ��ˣ���������!\n");
			vtcp_log("T_hv_fzf_sts[0]=[%c]\n",hv_notpay.T_hv_fzf_sts[0]);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
	if( hv_notpay.T_hv_fzf_sts[0]!=SEND_REGIST)
	{
		vtcp_log("[%s][%d]f_sts=[%c]",__FILE__,__LINE__,hv_notpay.T_hv_fzf_sts[0]);
		sprintf(acErrMsg,"״̬����������������[%c]!\n",hv_notpay.T_hv_fzf_sts[0]);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	}
	
	hv_notpay.T_hv_fzf_sts[0]=SEND_ACCT_WAIT;/*����״̬*/
	MEMCPY_DEBUG(hv_notpay.T_tel,cTel,sizeof(cTel)-1);
	hv_notpay.T_lw_ind[0]='1';/*��*/
	g_reply_int = iHvUpdFzfRecFromPayin(&hv_notpay,0); /*���¸ñ�*/
	if( g_reply_int )
	{
		sprintf( acErrMsg,"���ú���iHvUpdFzfRecFromPayin����,[%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	g_reply_int = dg_hv_fzfhtr(&hv_notpay);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"��������ϸ�����!\n");
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	vtcp_log("[%s][%d]iCmtno=[%.3d]",__FILE__,__LINE__,iCmtno);
	switch(iCmtno) /*��ʼƴ��,���뷢�͵Ǽǲ�*/           
	{
	    case 311:  /*��������*/
	    	if(cCheck_stat(hv_notpay))
	    	{
	    		sprintf(acErrMsg,"���ñʳ������׵�ԭ״̬����!\n");
        		WRITEMSG
	    		goto ErrExit;/*״̬����ȷ*/
	    	}
	    	pub_com_CMT311_T2TAG(&hv_notpay,&pack_fzf);
	    	break;
	    case 313:  /*�˻�����*/
	    	pub_com_CMT313_T2TAG(&hv_notpay,&pack_fzf);
	    	break;
	    case 314:  /*�˻�Ӧ��*/
	    	pub_com_CMT314_T2TAG(&hv_notpay,&pack_fzf);
	    	break;
	     case 724:  /*δ�û�Ʊ�˻�,�Ƿ���Ҫ�޸�һ�»�Ʊ������Ϣ��״̬�أ�*/
	    	pub_com_CMT724_T2TAG(&hv_notpay,&pack_fzf);
	    	break;		
	     case 721:  /*�ʽ�����*/
	    	pub_com_CMT721_T2TAG(&hv_notpay,&pack_fzf);
	    	break;	
	    default:
	    	sprintf(acErrMsg,"û�иñ�������!\n");
        	WRITEMSG
	    	goto ErrExit;
	    	break;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	getPackageFlgNo(&pack_fzf); /**�õ�FlgNo**/ 
	ret=hv_gen_pack(&pack_fzf);
	if(ret<0)
	{
		sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	if(hv_insert_sendlist(&pack_fzf,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
		,hv_notpay.T_trace_no,hv_notpay.T_orderno)<0)
	{
		sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	/*���ͱ��ĵ�ǰ�û�**/
	if(hv_sendpack(&pack_fzf)){
		sprintf(acErrMsg,"���ͱ��ĵ�ǰ�û�ʧ��!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	/*���ͽ���Ҫд��ˮ*/
	if( pub_ins_trace_log() )
	{
	sprintf(acErrMsg,"���ͽ���д��ˮ����!");
	strcpy( g_pub_tx.reply, "S023" );
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
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

cCheck_stat(HV_NOTPAY_AREA hv_notpay)/*���������׵�ʱ����Ҫ�ж�ԭ���׵�״̬*/
{
	int ret;
	HV_NOTPAY_AREA hv_notpay_bak;
	memset(&hv_notpay_bak,0,sizeof(hv_notpay_bak));
	MEMCPY_DEBUG(&hv_notpay_bak,&hv_notpay,sizeof(HV_NOTPAY_AREA)); /*����ԭ���Ľṹ��*/
	MEMCPY_DEBUG(hv_notpay.T_orderno ,hv_notpay.T_o_payin_orderno	,sizeof(hv_notpay.T_orderno));
	MEMCPY_DEBUG(hv_notpay.T_or_br_no,hv_notpay.T_or_br_no,sizeof(hv_notpay.T_or_br_no));
	MEMCPY_DEBUG(hv_notpay.T_wt_date ,hv_notpay.T_o_wt_date,sizeof(hv_notpay.T_wt_date));
	MEMCPY_DEBUG(hv_notpay.T_cmtno   ,hv_notpay.T_o_cmtno  ,sizeof(hv_notpay.T_cmtno));
	
	ret = iHvTable2Notpayin(&hv_notpay);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к���iHvTable2Notpayin����!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		return (-1);	
	}
	
	if (hv_notpay.T_hv_fzf_sts[0] != SEND_INQUEUE && hv_notpay.T_hv_fzf_sts[0] != SEND_ACCT_WAIT)
	{
		return 1;
	}
	MEMCPY_DEBUG(&hv_notpay,&hv_notpay_bak,sizeof(HV_NOTPAY_AREA));/*�ָ���ԭ���Ļ�����*/
	return 0;
}

cChange_stat(HV_NOTPAY_AREA hv_notpay)
{
	HV_NOTPAY_AREA hv_notpay_bak;
	memset(&hv_notpay_bak,0,sizeof(hv_notpay_bak));
	MEMCPY_DEBUG(&hv_notpay_bak,&hv_notpay,sizeof(HV_NOTPAY_AREA));/*����ԭ���Ľṹ��*/
	
	MEMCPY_DEBUG(hv_notpay.T_or_br_no ,hv_notpay.T_o_or_br_no,sizeof(hv_notpay.T_or_br_no));	
	MEMCPY_DEBUG(hv_notpay.T_wt_date,hv_notpay.T_o_wt_date,sizeof(hv_notpay.T_wt_date));
	MEMCPY_DEBUG(hv_notpay.T_orderno,hv_notpay.T_req_orderno,sizeof(hv_notpay.T_orderno));
	MEMCPY_DEBUG(hv_notpay.T_cmtno  ,hv_notpay.T_o_cmtno	,sizeof(hv_notpay.T_cmtno));
	iHvTable2Notpayin(&hv_notpay);
	hv_notpay.T_hv_fzf_sts[0]=RECEIVE_BACK_ACCT; /*�Ѳ鸴*/
	iHvUpdZfRecFromPayin(&hv_notpay,0);
	MEMCPY_DEBUG(&hv_notpay,&hv_notpay_bak,sizeof(HV_NOTPAY_AREA));/*�ָ���ԭ���Ļ�����*/
}	    	
