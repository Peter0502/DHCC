/***************************************************************
* �� �� ��:     spJ013.c
* ����������    ���л�Ʊ��ʧ/��� 			
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-18  14:21
*
* �޸ļ�¼��  
* ��    ��:   20060824
* �� �� ��:   liuyue
* �޸�����: �ϲ���ʧ�ͽ��
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "hv_brno_orno_c.h"
#include "find_debug.h"


spJ013()
{
	int ret = 0;
	HV_NOTPAY_AREA hv_notpayin;
	struct hv_poinfo_c hv_poinfo;
	struct hv_brno_orno_c hv_brno_orno;
	HV_FD123_AREA fd123;

	char cPwd[11]     ;   /***��$FD88�ֶ��л�ȡ��Ѻ************/
	char cBank_type[2];   /***��$FD72�ֶ��л�ȡ�б�****/
	char cOper_type[2];   /***��$FD71�ֶ��л�ȡ������־ 0-��ʧ 1-���ʧ**/
	int  iExsit;/*****0 ���е��Ǹ���Ʊ���ڣ�1���еĵ��Ǹ���Ʊ������***/
	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00,      sizeof(hv_notpayin));
	memset(&hv_poinfo,    0x00,      sizeof(hv_poinfo));
	memset(&fd123,        0x00,      sizeof(fd123));
	iExsit = 0;


	/*****************************
     *PART 1   �õ�ǰ̨����      *
     *****************************/
	/***�õ����֧��ҵ��8583����***/
	ret=iHvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
        /******�ӱ�ǩ�л�ȡ������־*****/	
	get_zd_data("0720",cBank_type);
	get_zd_data("0710",cOper_type);	
	get_zd_data("0880",cPwd);
	get_fd_data("1230",(char *)&fd123);
	
	if(cBank_type[0]=='0')/***�Ǳ���****/
	 {
	    vtcp_log("[%s][%d]���ݽ��׻����Ų��ҽ����к�!\n",__FILE__,__LINE__);
	    ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'  ",g_pub_tx.tx_br_no);
	    if(ret) {
		 sprintf(acErrMsg,"���ݻ������ҽ��׻�������!\n");
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"D103");
		 goto ErrExit;
	    }
	 MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_brno_orno.or_br_no,sizeof(hv_poinfo.pay_br_no)-1);

        } else {
	  MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	}
	/******��ȡ��Ʊ����*****/
	MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));
	
		


	/**********************
         *PART 2   �������   *
	 **********************/
	vtcp_log("++banktype[%c]+oper[%c]++\n",cBank_type[0],cOper_type[0]);
	if(cBank_type[0]=='0')/*****����ǩ���Ļ�Ʊ�Ĺ�ʧ�ͽ��ʧ*****/
	{
		vtcp_log("+++++++��ʼ���뱾�еĹ�ʧ�ͽ��+++++++\n");
		ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s' ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
		if(ret)
		{
			sprintf(acErrMsg,"ִ�к���hv_poinfo_Dec_Upd����!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
		if(ret==100)
		{
			sprintf(acErrMsg,"���в����ڸû�Ʊ!\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret!=100 &&ret!=0)
		{
			sprintf(acErrMsg,"ȡ��ѯ��Ʊ��Ϣ���α����!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		/*********��ʼ�Ƚ���������ݺ����ݱ��е�����**********/
		if(memcmp(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1))
		{
			sprintf(acErrMsg,"�������Ѻ������!/n");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P005");
			goto ErrExit;
		}
		if(cOper_type[0]=='0')/*****����ǹ�ʧ*******/
		{
		vtcp_log("++++++++��ʼ���뱾�еĹ�ʧ����!\n");
		if(hv_poinfo.po_sts[0]!=HV_HP_MV_CENTER)/*ֻ���ƴ�״̬���Թ�ʧ*/	
		{
		sprintf(acErrMsg,"�û�Ʊ��״̬�����ƴ�״̬,[%c] �������ʧ\n",hv_poinfo.po_sts[0]);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P004");
		goto ErrExit;
		}
		/*********��ʼ��������*****************/
		hv_poinfo.l_po_sts[0]= hv_poinfo.po_sts[0];
		hv_poinfo.po_sts[0]  = HV_HP_LOSS ;  /**��ʧ****/
		if(hv_poinfo.l_tx_date== apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date)))
			hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
		else
			hv_poinfo.td_cnt     =1;     /**���ս��ױ���+1**/
		hv_poinfo.lost_cnt   = 1;           /**��ʧ����+1*****/
		hv_poinfo.l_tx_date  = 
		  apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date));
			
		ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"���»�Ʊ������Ϣ�����!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		Hv_poinfo_Clo_Upd();	     
	   }/*****���й�ʧ���if�����������******/
	else if(cOper_type[0]=='1')/*********����ǽ��ʧ******/
	{
		vtcp_log("+++++++��ʼ���뱾�еĽ��ʧ����!\n");
		if(hv_poinfo.po_sts[0]!=HV_HP_LOSS)
		{
			sprintf(acErrMsg,"�û�Ʊ��û�й�ʧ,�޷����ʧ!\n");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P004");
			goto ErrExit;
		}
		/**********��ʼ���±�Ҫ���ֶ�*********/
		if(hv_poinfo.l_tx_date==apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date)))
		{
			hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
		}
		else
			hv_poinfo.td_cnt = 1;
		hv_poinfo.l_tx_date = apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date));
		hv_poinfo.po_sts[0]	= hv_poinfo.l_po_sts[0];/**��ʧ****/
		vtcp_log("++++++++++++++++++++++++++++++++++\n");
		vtcp_log("+++++[%ld]++++++\n",hv_poinfo.l_tx_date);
		vtcp_log("+++++[%c]+++++++\n",hv_poinfo.po_sts[0]);
		vtcp_log("+++++[%d]+++++++\n",hv_poinfo.td_cnt);
		ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"���»�Ʊ������Ϣ�����!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		Hv_poinfo_Clo_Upd();	     
	      }/*****���еĽ�ҵ����������******/
 	 }/***********���еĴ���͵����������*******/
	 else if(cBank_type[0]=='1')/*****��������л�Ʊ*************/
 	 {
		
           /****��������е��ǲ����������뱾�еĻ����ŵ�*****/
	   if(!memcmp(hv_poinfo.pay_br_no,hv_notpayin.F_or_br_no,sizeof(hv_poinfo.pay_br_no)-1))
	   {
		sprintf(acErrMsg,"�������е�, ȴ�����˱��еĻ�����[%s][%s]\n",hv_poinfo.pay_br_no,hv_notpayin.F_or_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P007");
		goto ErrExit;
	   }
	   
	   vtcp_log("++++��ʼ�������еĹ�ʧ����ʧ����!+++++++++\n");
	   ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s'  and bank_type='1'  ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	   if(ret)
	   {
		sprintf(acErrMsg,"���ݿ����!\n");
		WRITEMSG
		goto ErrExit;
	   }
	   ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);/**�ҵ��˼�¼**/

           if(ret!=100 &&ret)/****�����ݿ���ô���***/
	   {
	       sprintf(acErrMsg,"ִ�к���hv_poinfo_dec_upd����!ret=[%d]\n",ret);
	       WRITEMSG
	       goto ErrExit;
  	   }
	   else if(ret==100) /****��¼�����ڵ����*****/
           {
	     vtcp_log("++++++���������е�������¼,��Ҫ����!+++++++\n");
	     /*******����һ����־��˵����¼������,�����ܿ��������Щ�������**/
	     iExsit = 1; 

	     hv_poinfo.reg_date = apatoi(hv_notpayin.F_o_wt_date,sizeof(hv_notpayin.F_o_wt_date));/***����reg_date********/

	     dot_str2dbl(hv_notpayin.F_o_txamt,sizeof(hv_notpayin.F_o_txamt),&hv_poinfo.po_bal);

	     MEMCPY_DEBUG(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1);

	     hv_poinfo.bank_type[0] = '1';

	     hv_poinfo.po_sts[0] = HV_HP_LOSS;
	     hv_poinfo.l_po_sts[0] = '0';
	      /*********��ʼ�����¼************/
	      ret = Hv_poinfo_Ins(hv_poinfo,g_pub_tx.reply);TRACE
	      if(ret)
	      {
	  	    sprintf(acErrMsg,"�����Ʊ������Ϣ�����!ret=[%d]\n",ret);
		    WRITEMSG
		    goto ErrExit;
	      }
	      vtcp_log("______________�ɹ�����һ�����еļ�¼________\n");
	    }/**else****���в��Ҳ����ڸü�¼�����****/
	 /***********�����л�Ʊ����Ϣ��ѯ,����Ĵ������!****/ 
	 /*********��ʼ�Ƚ���������ݺ����ݱ��е�����**********/
	vtcp_log("___________iExsit==[%d]____________\n",iExsit);
	if(!iExsit&&memcmp(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1))
	 {
	  	 sprintf(acErrMsg,"�������Ѻ������!\n");
	   	 WRITEMSG
		 strcpy(g_pub_tx.reply,"P005");
	   	 goto ErrExit;
         }
	  	
	 if(!iExsit&&strlen(hv_notpayin.F_o_wt_date)!=0&&apatoi(hv_notpayin.F_o_wt_date,sizeof(hv_notpayin.F_o_wt_date))!=hv_poinfo.reg_date)
	 {
		sprintf(acErrMsg,"�����ǩƱ���ڲ�����![%d][%d]\n",apatoi(hv_notpayin.F_o_wt_date,sizeof(hv_notpayin.F_o_wt_date)),hv_poinfo.reg_date);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P006");
		goto ErrExit;
	 }
	 double money = 0.0;
	 vtcp_log("+++++F_o_Txamt==[%s]\n",hv_notpayin.F_o_txamt);
	dot_str2dbl(hv_notpayin.F_o_txamt,sizeof(hv_notpayin.F_o_txamt),&money);	
	 vtcp_log("++++++++++pobal==[%lf]\n",hv_poinfo.po_bal);
         if(!iExsit&&money!=hv_poinfo.po_bal &&hv_poinfo.po_bal!=0.0)
 	 {
		sprintf(acErrMsg,"����ĳ�Ʊ�����[%lf][%lf]!\n",hv_poinfo.po_bal,money);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P008");
		goto ErrExit;	
	 }
	 if(!iExsit&&cOper_type[0]=='0')/****��������й�ʧ*******/
	 {
		vtcp_log("_____________�������еĹ�ʧ����____________\n");
		if(hv_poinfo.po_sts[0]!=HV_HP_MV_CENTER)/****ֻ���ƴ�״̬���Թ�ʧ*/	
		{
		sprintf(acErrMsg,"�û�Ʊ��״̬�����ƴ�״̬, �������ʧ\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P004");
		goto ErrExit;
		}
		/*********��ʼ��������*****************/
		if(hv_poinfo.l_tx_date==apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date)))
		{
			hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
		}
		else
			hv_poinfo.td_cnt = 1;
		hv_poinfo.l_tx_date = apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date));
		hv_poinfo.l_po_sts[0] = hv_poinfo.po_sts[0];
		hv_poinfo.po_sts[0]	= HV_HP_LOSS;/**��ʧ****/
		hv_poinfo.lost_cnt     += 1;           /**��ʧ����+1*****/
			
		ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"���»�Ʊ������Ϣ�����!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		Hv_poinfo_Clo_Upd();	     
	   }/*****���й�ʧ���if�����������******/
	else if(cOper_type[0]=='1')/*********����ǽ��ʧ******/
	{
		vtcp_log("___________�������еĽ��ʧ����_______________\n");
		if(hv_poinfo.po_sts[0]!=HV_HP_LOSS)
		{
			sprintf(acErrMsg,"�û�Ʊ��û�й�ʧ,�޷����ʧ!\n");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P004");
			goto ErrExit;
		}
		/**********��ʼ���±�Ҫ���ֶ�*********/
		if(hv_poinfo.l_tx_date==apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date)))
		{
			hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
		}
		else
			hv_poinfo.td_cnt = 1;

		hv_poinfo.l_tx_date = apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date));
		hv_poinfo.po_sts[0]	= hv_poinfo.l_po_sts[0];/**�⿪��ʧ****/
		ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"���»�Ʊ������Ϣ�����!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		Hv_poinfo_Clo_Upd();	     
		
         }
     }/****���е�����������********/
	
	
		
			
	
		

     /*********************
	 *PART 3  �������ݿ� *
     *********************/
	

	/*******************
     *PART 4  ��β���� *
     *******************/	
	/* д��ҵ����ˮ */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	/*** ������� ***/

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
