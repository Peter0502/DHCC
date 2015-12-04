/***************************************************************
* �� �� ��:     spJ014.c
* ����������    ���л�Ʊ���ʧ			
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-18  14:21
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"

/**********************************
*��ʧ���ڷŵ�ԭί�������ֶ�       *
*��Ʊ���� ���ļ�����			  *
***********************************/
typedef struct{
	char po_no[20];		/***��Ʊ����***/
}STRUCT_HP_AREA;

spJ014()
{
	int ret = 0;
	char *cFilename ;
	FILE *fp;
	HV_PAYIN_AREA hv_payin;
	STRUCT_HP_AREA	hv_hp;
	STRUCT_HP_AREA	hv_hp_tb;
	struct hv_poinfo_c hv_poinfo;
	struct hv_poinfo_c hv_poinfo_tb;/***��Ҫ������Ҫ�ؽ��кϷ��Լ��***/

	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(cFilename,  0x00 ,  sizeof(cFilename));
	memset(&hv_hp,	   0x00 ,  sizeof(hv_hp));
	memset(&hv_hp_tb,	   0x00 ,  sizeof(hv_hp_tb));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));
	memset(&hv_poinfo_tb, 0x00 ,  sizeof(hv_poinfo_tb));


	/*****************************
     *PART 1   �õ�ǰ̨����      *
     *****************************/
	/***�õ����֧��ҵ��8583����***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
   	/*****�õ����������������**********/
	MEMCPY_DEBUG(cFilename,"../tmp/",7);
	strcat(cFilename,hv_payin.F_tx_br_no);
	strcat(cFilename,hv_payin.F_tel);
	sprintf(acErrMsg,"�������Ӧ���ļ�����[%s]\n",cFilename);
	WRITEMSG
	pub_base_AllDwnFilName(cFilename);
	fp = fopen(cFilename,"rb");
	if(fp==NULL)
	{
		sprintf(acErrMsg,"�������Ӧ���ļ�����[%s]������\n",cFilename);
		WRITEMSG
		goto ErrExit;	
	}
	fget(&hv_hp,sizeof(hv_hp),fp);
	fclose(fp);
	/******ǰ̨��ȡ���ݶ��뵽hv_poinfo�ṹ��***/
	MEMCPY_DEBUG(hv_poinfo.po_no,hv_hp.po_no,sizeof(hv_poinfo.po_no)-1);
	MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	str2dbl(hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt),0,hv_poinfo.po_bal);
	str2dbl(hv_payin.F_o_wt_date,sizeof(hv_payin.F_o_wt_date),0,hv_poinfo.l_tx_date);/**���뵽l_tx_date�ֶ�����******/
	/**********************
     *PART 2   �������   *
	 **********************/
	
		

	/*********************
	 *PART 3  �������ݿ� *
     *********************/
	
	/******��ȡ����********************/
	MEMCPY_DEBUG(hv_poinfo_tb.po_no,hv_hp.po_no,sizeof(hv_poinfo.po_no)-1);
	MEMCPY_DEBUG(hv_poinfo_tb.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);

		ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no=%s and pay_br_no=%s ",hv_poinfo_tb.po_no,hv_poinfo_tb.pay_br_no);
		if(ret)
		{
			sprintf(acErrMsg,"ִ�к���hv_poinfo_dec_upd����!ret==[%d]\n",ret);
			WRITEMSG
			goto ErrExit;	
		}
		ret = Hv_poinfo_Fet_Upd(&hv_poinfo_tb,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"ִ�к���Hv_poinfo_Fet_Upd����ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;	
		}
		
		/************�Ƚ�һ�½��͹�ʧ���������ֶ�******************/
		if(hv_poinfo.po_bal!=hv_poinfo_tb.po_bal ||hv_poinfo.l_tx_date!=hv_poinfo_tb.l_tx_date)
		{
			sprintf(acErrMsg,"����Ľ����߹�ʧ���������ݱ�ƥ��\n");
			WRITEMSG
			goto ErrExit;
		}
		/***�����ƥ���ˣ����������ֶΣ����ܷ�ʽ�ͻ�Ʊ״̬***/
		/****��ʧ����д�뵽����ϴν������ڰ�???????***/
		hv_poinfo_tb.po_sts[0]		= 'F';/**��ʧ****/
		/******���ڿ�ʼ������Щ�ֶΰ�*************************/		
		ret = Hv_poinfo_Upd_Upd(&hv_poinfo_tb,g_pub_tx.reply);
		{
			sprint(acErrMsg,"ִ�к���hv_poinfo_upd_upd����!ret==[%d]\n",ret);
			WRITEMSG
			goto ErrExit;	
		}
	 

	/*******************
     *PART 4  ��β���� *
     *******************/	
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
