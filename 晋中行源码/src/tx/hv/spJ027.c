/*************************************************
* �� �� ��:  spJ027.c
* ��������:  ���гжһ�Ʊ�鸴��¼��
* ��    ��:  xyy 
* ������ڣ� 2006��8��18��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#define loc_t sys_loc_t
#include "find_debug.h"

int iGetHvAddid(char * addid);
int CMT302_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack);

int spJ027()
{
	int ret=0;	
	char cContent_flag[2];/***�鸴���ݵı�ǩ***/
	char cBrf[200];
	HV_FD123_AREA      fd123;
	char cContent[256];
	struct hv_fzf_c hv_fzf;
	HV_NOTPAY_AREA hv_notpay;
	THVPACKAGE pack302;
	
	/*�Ѳ�ѯ���е���ؼ�¼�����鸴����**/   
	memset(&fd123,0x00,sizeof(fd123)); 
	memset(cContent_flag, 0x00, sizeof(cContent_flag));
	memset(cBrf	  ,0,sizeof(cBrf));
	memset(&hv_fzf,0,sizeof(hv_fzf));
	memset(&hv_notpay,0,sizeof(hv_notpay));
	memset(&pack302, 0 , sizeof(pack302));
	memset(cContent,0x00,sizeof(cContent));

	MEMCPY_DEBUG(cBrf,hv_notpay.F_content,sizeof(cBrf)-1);
	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/***�õ����֧��ҵ��8583����***/
	ret = iHvTistoNotpay(&hv_notpay);
	if( ret )
	{
		sprintf( acErrMsg,"��ǰ̨��ȡ����ʧ��![%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	MEMCPY_DEBUG(cBrf    ,hv_notpay.F_content,sizeof(cBrf)-1);
	/**�����кͽ����в�����ͬ***/
	ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no, hv_notpay.F_or_br_no );
	if(ret)
	{
		sprintf(acErrMsg,"���ݻ�����ȡ�����кŴ���!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	if(!memcmp(hv_notpay.F_or_br_no,hv_notpay.F_ac_br_no,sizeof(hv_notpay.F_or_br_no)))
	{
		sprintf(acErrMsg,"�����кͽ����в��ܹ���ͬһ��or[%s]ac[%s]\n",hv_notpay.F_or_br_no,hv_notpay.F_ac_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	get_zd_data("0720",cContent_flag);/**��ȡһ����־,ǰ̨����***/

	vtcp_log("cContent_flag==[%s][%d]\n",cContent_flag,__LINE__);
	vtcp_log("[%s][%d]��ʼ��д�鸴������!\n",__FILE__,__LINE__);

	if(cContent_flag[0]=='0')/**��Ϣ���***/
	{
		MEMCPY_DEBUG(cContent,"��ѯ��Ʊ�����гжһ�Ʊ��������һ��",34);	
		strncat(cContent+strlen(cContent),cBrf,sizeof(cBrf));
	}
	else if(cContent_flag[0]=='1')/***��Ϣ������,��һ���̶��ֶ�+�����ֶ�**/
	{
		zip_space(cBrf);
		MEMCPY_DEBUG(cContent, "��ѯ��Ʊ�����гжһ�Ʊ�������ݲ��������岻������Ϊ��",52);
		strncat(cContent+strlen(cContent),cBrf,sizeof(cBrf));
	}
	vtcp_log("cContent==[%s]\n",cContent);
		
	/******���Ӫҵ��״̬******/ 

	ret = iHv_checkstat(hv_notpay.F_cmtno);
	if( ret )
	{
		sprintf( acErrMsg,"����״̬������![%s][%d][%s]",__FILE__,__LINE__,ret);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("[%s][%d]������iHv_checkstat",__FILE__,__LINE__);
		
	
	/** ����ԭ��ѯ��¼ **/
  	vtcp_log("hv_notpay.F_o_fzf_date==[%s]\n",hv_notpay.F_o_fzf_date);
	vtcp_log("hv_notpay.F_ac_br_no==[%s]\n",hv_notpay.F_ac_br_no);
	vtcp_log("hv_notpay.F_orderno==[%s]\n",hv_notpay.F_orderno);
	vtcp_log("hv_notpay.F_o_cmtno==[%s]\n",hv_notpay.F_o_cmtno);	
  	
	ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"or_br_no = '%.12s' and wt_date =%d and orderno ='%s' and cmtno='%s' ",hv_notpay.F_ac_br_no,apatoi(hv_notpay.F_o_fzf_date,sizeof(hv_notpay.F_o_fzf_date)),hv_notpay.F_orderno,hv_notpay.F_o_cmtno);
	if (ret)
	{
		sprintf( acErrMsg,"���������г���,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	if(hv_fzf.hv_fzf_sts[0]!=RECEIVE_ACCT_QS) 
	{
		sprintf( acErrMsg,"��ѯ���¼״̬����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit; 
	}
	/******����ԭ��ѯ��¼����Ϣ*****/

	ret = Hv_fzf_Dec_Upd(g_pub_tx.reply,"or_br_no='%.12s' and wt_date='%s' and cmtno='%s' and orderno='%s' ",
			hv_notpay.F_ac_br_no,hv_notpay.F_o_fzf_date,hv_notpay.F_o_cmtno,hv_notpay.F_orderno);
	if(ret)
	{
		sprintf(acErrMsg,"���ݱ����\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	ret = Hv_fzf_Fet_Upd(&hv_fzf,g_pub_tx.reply);
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"���ݿ����!ret==[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	else if(ret==100)
	{	
		sprintf(acErrMsg,"�Ҳ���ԭ���Ļ�Ʊ��ѯ�ļ�¼!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;	
	}
	
        if(hv_fzf.hv_fzf_sts[0]!=RECEIVE_ACCT_QS)
        {
                sprintf( acErrMsg,"��ѯ���¼״̬����,[%s][%d]", __FILE__,__LINE__);
                strcpy( g_pub_tx.reply, "HV01" );
                WRITEMSG
                goto ErrExit;
        }
	
	
	
	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,hv_notpay.F_or_br_no);
	/*���ݻ��������к�*/
        /*����Ϊԭ��ѯ���׵Ļ�����Ϣ*/
	/**Ӧ������,����ǰ����**/
	get_zd_data("0440",hv_notpay.T_res_date);
	hv_fzf.res_date = apatoi(hv_notpay.T_res_date,sizeof(hv_notpay.T_res_date));
	/*Ӧ���к�*/
	MEMCPY_DEBUG(hv_fzf.res_br_no,hv_notpay.F_or_br_no,sizeof(hv_notpay.F_or_br_no));
	/*Ӧ������*/
	MEMCPY_DEBUG(hv_fzf.res_content,cContent,sizeof(cContent)-1);
	/*Ӧ�����*/
	ret = iGetHvFzfOrderno(hv_fzf.res_orderno);
	vtcp_log("[%s][%d]res_orderno==[%s]\n",__FILE__,__LINE__,hv_fzf.res_orderno);
	MEMCPY_DEBUG(hv_fzf.refno,hv_notpay.F_refno,sizeof(hv_notpay.F_refno));
	/*��¼״̬*/
	hv_notpay.T_hv_fzf_sts[0]=SEND_ACCT_QS;
	vtcp_log("[%s][%d]begin to update !\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]begin to update !\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]begin to update !\n",__FILE__,__LINE__);

	ret = Hv_fzf_Upd_Upd(hv_fzf,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"�������ݱ�ʧ��!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P003");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]UPDATE END!!!!!!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]UPDATE END!!!!!!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]UPDATE END!!!!!!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]UPDATE END!!!!!!\n",__FILE__,__LINE__);

	/****��ʼ��׼��notpay�ṹ�������*****/

	

	vtcp_log("[%s][%d]��ʼ��ԭ��ѯ��¼���и���!\n",__FILE__,__LINE__);
	/***Ϊ������ԭ���Ĳ�ѯ���׵ļ�¼,�Լ����ֶν���ǿ�Ƹ���***/
	MEMCPY_DEBUG(hv_notpay.F_or_br_no,hv_notpay.F_ac_br_no,sizeof(hv_notpay.F_or_br_no));
	MEMCPY_DEBUG(hv_notpay.F_wt_date,hv_notpay.F_o_fzf_date,sizeof(hv_notpay.F_wt_date));
	MEMCPY_DEBUG(hv_notpay.F_cmtno,hv_notpay.F_o_cmtno,sizeof(hv_notpay.F_cmtno));
	ret = iHvTable2Notpayin(&hv_notpay);
	if(ret)
	{
		sprintf(acErrMsg,"�����ݱ��л�ȡ����ʧ��!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	/*****��ԭ��ǿ��ת�����ֶ�*****/
	ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no, hv_notpay.T_or_br_no );
	get_zd_data("0440",hv_notpay.T_wt_date);
	MEMCPY_DEBUG(hv_notpay.T_orderno,hv_fzf.res_orderno,sizeof(hv_notpay.T_orderno));
	MEMCPY_DEBUG(hv_notpay.T_cmtno,"302",3);
	sprintf(hv_notpay.T_trace_no,"%6d" ,g_pub_tx.trace_no);

	/****�ӿ��ǲ�֪����ô�޷���pay_qs_no��ֵ*****/
	MEMCPY_DEBUG(hv_notpay.T_pay_qs_no,hv_fzf.pay_qs_no,sizeof(hv_notpay.T_pay_qs_no));
	sprintf(hv_notpay.T_o_wt_date,"%8d",hv_fzf.o_wt_date);
	MEMCPY_DEBUG(hv_notpay.T_o_or_br_no,hv_fzf.o_or_br_no,sizeof(hv_notpay.T_o_or_br_no));

	vtcp_log("hv_fzf.pay_qs_no===[%s]\n",hv_fzf.pay_qs_no);	
	vtcp_log("hv_fzf.pay_qs_no===[%s]\n",hv_fzf.pay_qs_no);	
	vtcp_log("hv_fzf.pay_qs_no===[%s]\n",hv_fzf.pay_qs_no);	
	vtcp_log("hv_fzf.pay_qs_no===[%s]\n",hv_fzf.pay_qs_no);	
	vtcp_log("hv_fzf.o_tx_date===[%ld]\n",hv_fzf.o_tx_date);	

	print_notpay(&hv_notpay,'T');
	   
	ret = dg_hv_fzfhtr(&hv_notpay);
	if (ret)
	{
		sprintf( acErrMsg,"�������ɸ�ʽ��ϸ�����,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}


	/*******************
	ret =CMT302_T2TAG(&hv_notpay,&pack302);
	ret=hv_gen_pack(&pack302);
	if(ret<0){
		sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack302,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
			,g_pub_tx.trace_no,hv_notpay.T_orderno)<0){
		sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack302)){
		sprintf(acErrMsg,"����302���ĵ�ǰ�û�ʧ��!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	*************************/
	
	/*********׼�����*************/
	MEMCPY_DEBUG(fd123.orderno,hv_notpay.T_res_orderno,sizeof(hv_notpay.T_res_orderno));
	MEMCPY_DEBUG(fd123.ac_br_no,hv_notpay.T_req_br_no,sizeof(hv_notpay.T_req_br_no));
	deal_half_charcter(cContent);
	MEMCPY_DEBUG(fd123.notpay_content,cContent,sizeof(cContent));
	


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

int CMT302_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack)
{
 /****************�鸴����*********************************/
	 char cTmpBuf[19];
	 memset(cTmpBuf,0x00,sizeof(cTmpBuf));

	 pack->opeType='2';/*����*/
	 MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);
	 /*��������*/
	 del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/***���׽��**/
	 MEMCPY_DEBUG(pack->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));
	 MEMCPY_DEBUG(pack->CMTCode,"302",3); /*CMT���*/
	 MEMCPY_DEBUG(pack->workDate,hv_notpay->T_tx_date,8);/*��������*/
	 iHVSETTAG( pack,"CJ1",hv_notpay->T_res_date,sizeof(hv_notpay->T_wt_date));
	/*�鸴����*/
	 iHVSETTAG(pack,"01D",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));/*�鸴���к�*/
	 iHVSETTAG(pack,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));/*�鸴�������к�*/

	iHVSETTAG(pack,"043",hv_notpay->T_res_orderno,sizeof(hv_notpay->T_res_orderno));                     /*�鸴���*/
	iHVSETTAG(pack,"CCB",hv_notpay->T_req_br_no,sizeof(hv_notpay->T_req_br_no));           /*ԭ��ѯ���к�*/
	iHVSETTAG(pack,"CCC",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));         /*ԭ��ѯ�����к�*/
	iHVSETTAG(pack,"CP1",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));             /*ԭ��ѯ���*/
	iHVSETTAG(pack,"CJA",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));           /*ԭ��ѯ����*/
	iHVSETTAG(pack,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));         /*ԭί������*/
	iHVSETTAG(pack,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));               /*ԭ�������к�*/
	iHVSETTAG(pack,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));               /*ԭ�����к�*/
   	iHVSETTAG(pack,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));         /*ԭ��������*/

	 iHVSETTAG(pack,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));     /*ԭ֧���������*/
        iHVSETTAG(pack,"CND",cTmpBuf,sizeof(cTmpBuf));
                        /*ԭ���ҷ���,���*/
     	iHVSETTAG(pack,"010",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));                       /*�������Ĵ���*/
    	iHVSETTAG(pack,"0B9",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco)) ;                       /*�ձ����Ĵ���*/
     	iHVSETTAG(pack,"053",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));             /*�鸴����*/

        /***********û�и�����***************/
        return 0;
}

int deal_half_charcter(char *buf)
{
		int i = 0;
		int j = 0;
		int size = 0;
		int index =0;
		
		
		

		printf("[%s]====\n",buf);
		size = strlen(buf);
		
		char cBuf[1024];
		
		memset(cBuf,0x00,sizeof(cBuf));


		for(i=0;i<size;i++)
		{
				if((int)buf[i]<128 &&(int)buf[i+1]>128 && (i+1+index)%2)
				{
						cBuf[j] = buf[i];
						cBuf[j+1]= ' ';
						j = j+2;
						index++;
				}
				else
				{
						cBuf[j] = buf[i];
						cBuf[j] = buf[i];
						j++;
				}
		}

		MEMCPY_DEBUG(buf,cBuf,sizeof(cBuf));
		return 0;
}
