/***************************************************************
* �� �� ��:     spJ015.c
* ����������    ���л�Ʊ����ע��		
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-18  17:27
* ����:���»�Ʊ��״̬,ͬʱ�ڷ�֧���������һ�����ɸ�ʽ,�����ͳ�ȥ
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
#include "hv_fzf_c.h"
#include "hvnet.h"
#include "hv_pub.h"
#include "hv_fzf_c.h"
#include "find_debug.h"

/**********************************
*��ʧ���ڷŵ�ԭί�������ֶ�       *
*��Ʊ���� ���ļ�����		  *
*���������һ����ͨѶ             *
***********************************/

spJ015()
{
	int ret = 0;
	HV_NOTPAY_AREA     hv_notpayin;
	HV_FD123_AREA      fd123;
	struct hv_poinfo_c hv_poinfo;
	struct hv_fzf_c    hv_fzf;
	char cPwd	[11]     ;/*****��Ʊ��Ѻ****/
	THVPACKAGE pack303;


	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&fd123,        0x00 ,  sizeof(fd123));
	memset(&hv_poinfo,    0x00 ,  sizeof(hv_poinfo));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(cPwd     ,     0x00 ,  sizeof(cPwd));
	memset(&pack303,      0x00 ,  sizeof(pack303));


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
	vtcp_log("_________O_txamt==[%16s]\n",hv_notpayin.F_o_txamt);


	get_zd_data("0880",cPwd);/***��ȡ��Ѻ*****/
	get_fd_data("1230",(char *)&fd123);/**��ȡ123��****/

	/**********************
         *PART 2   �������   *
	 **********************/
		

	/*********************
	 *PART 3  �������ݿ� *
         *********************/
	
	/******��ȡ����********************/
	MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));
	MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	
	ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s'  ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if(ret)
	{
	sprintf(acErrMsg,"ִ�к���hv_poinfo_dec_upd����!ret==[%d]\n",ret);
	WRITEMSG
	goto ErrExit;	
	}
	ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
	if(ret)
	{
	sprintf(acErrMsg,"ִ�к���Hv_poinfo_Fet_Upd����ret=[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P001");
	goto ErrExit;	
	}

	/****�ж�һ�»�Ʊ��״̬*******************/
	if(hv_poinfo.po_sts[0]!=HV_HP_ASKCASH)
	{
	sprintf(acErrMsg,"�û�Ʊû�жҸ�[%c]\n",hv_poinfo.po_sts[0]);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P004");
	goto ErrExit;
	}
	if(hv_poinfo.po_sts[0]=='5')
	{
	sprintf(acErrMsg,"�û�Ʊ�Ѿ�����[%c]\n",hv_poinfo.po_sts[0]);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P004");
	goto ErrExit;
	}
	if(memcmp(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1))
	{	
	sprintf(acErrMsg,"��Ʊ��Ѻ����![");
	WRITEMSG
	strcpy(g_pub_tx.reply,"P005");
	goto ErrExit;
	}
	double money = 0.0;
	dot_str2dbl(hv_notpayin.F_o_txamt,sizeof(hv_notpayin.F_o_txamt),&money);
	if(hv_poinfo.po_bal!=money)
	{
	sprintf(acErrMsg,"����Ľ���![%lf][%16s][%lf]\n",hv_poinfo.po_bal,hv_notpayin.F_o_txamt,money);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P008");
	goto ErrExit;
	}
		
	/****��ʧ����д�뵽����ϴν������ڰ�???????***/
	hv_poinfo.l_tx_date = apatoi(hv_notpayin.F_wt_date,sizeof(hv_notpayin.F_wt_date));
	hv_poinfo.po_sts[0] = 'C';/***����Ҹ�ȡ��״̬***/

	/******���ڿ�ʼ������Щ�ֶΰ�*************************/		
	ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
	if(ret)
	{
	sprintf(acErrMsg,"ִ�к���hv_poinfo_upd_upd����!ret==[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P009");
	goto ErrExit;	
	}
	/************���֧�����ױ��в���һ����¼**************/

	ret = iHvfFrontToTable(&hv_notpayin);
	if(ret)
	{
	sprintf(acErrMsg,"ִ�к���F->Tʧ��!ret=[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P012");
	goto ErrExit;
	}


	ret = iHvNotPayin_Copy(&hv_notpayin,&hv_fzf);/***��T�ֶε�hv_fzf�ṹ����****/
	if(ret)
	{
	sprintf(acErrMsg,"��������ret=[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P012");
	goto ErrExit;
	}


	MEMCPY_DEBUG(hv_fzf.receco,HP_CENTER,sizeof(hv_fzf.receco)-1); /*****������Ҫǿ�������Ϻ���Ʊ���Ĵ���**************/
	sprintf(hv_notpayin.F_content,"��Ʊ�����գ���ã����Ǵ� �кŴ���Ϊ%.12s����һ����Ʊ�Ҹ�ȡ�����뱨��,��Ʊ����%20s����Ʊ���%16.2f��ǩ����%12s�� ��������!\n",hv_notpayin.F_or_br_no,hv_poinfo.po_no,hv_poinfo.po_bal,hv_poinfo.pay_br_no);
	memset(hv_fzf.cash_qs_no,0x00,sizeof(hv_fzf.cash_qs_no)-1);


	ret = Hv_fzf_Ins(hv_fzf,g_pub_tx.reply);
	if(ret)
	{
	sprintf(acErrMsg,"�����֧�����ױ����!ret=[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P010");
	goto ErrExit;
	}
	vtcp_log("INSERT END!!!!!!\n");
	vtcp_log("INSERT END!!!!!!\n");
	vtcp_log("INSERT END!!!!!!\n");





	/***���ﶨ�弸�����ӵ�����****/
	MEMCPY_DEBUG(hv_notpayin.T_receco,HP_CENTER,sizeof(hv_fzf.receco)-1);
	MEMCPY_DEBUG(hv_notpayin.T_req_content,hv_notpayin.F_content,sizeof(hv_notpayin.T_req_content));
	memset(hv_notpayin.T_ac_br_no,0x00, sizeof(hv_notpayin.T_ac_br_no));
	memset(hv_notpayin.T_cash_qs_no,0x00,sizeof(hv_notpayin.T_cash_qs_no));

	vtcp_log("T_req_content==[%s]\n",hv_notpayin.T_req_content);


	print_notpay(&hv_notpayin,'T');
	/*****�������֧����ʷ��ϸ��***********/
	vtcp_log("___________INSERT HTR_______[%d]\n",__LINE__);
	ret = dg_hv_fzfhtr(&hv_notpayin);
	if(ret)
	{
	sprintf(acErrMsg,"�����֧����ϸ�����!ret=[%d]",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P011");
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
	/****Add by SSH,2006.8.28,ֱ�ӷ���****/
	vtcp_log("T_req_content==[%s]\n",hv_notpayin.T_req_content);
	 pub_com_CMT303_T2TAG(&hv_notpayin,&pack303);
	 ret=hv_gen_pack(&pack303);
	 if(ret<0){
	 sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
	 WRITEMSG
	 strcpy( g_pub_tx.reply, "P021" );
	 goto ErrExit;
	 }
	 if(hv_insert_sendlist(&pack303,hv_notpayin.F_tx_br_no,hv_notpayin.F_tel\
	 ,hv_notpayin.T_trace_no,hv_notpayin.T_orderno)<0){
	 sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
	 WRITEMSG
	 strcpy( g_pub_tx.reply, "P010" );
	 goto ErrExit;
	 }
	 if(hv_sendpack(&pack303)){
	 sprintf(acErrMsg,"����303���ĵ�ǰ�û�ʧ��!\n");
	 strcpy( g_pub_tx.reply, "LS88" );
                WRITEMSG
                goto ErrExit;
        }
        /*************************************/


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0280",hv_notpayin.T_orderno);/****����һ����ž��㹻��*/
	set_zd_data("0440",hv_notpayin.T_wt_date);/****��ί�����ڹ�ȥ******/
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
