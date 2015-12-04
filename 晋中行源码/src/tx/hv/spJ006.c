/***************************************************************
* �� �� ��:     spJ006.c
* ����������    ���л�Ʊ��Ϣ�޸�			
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-17 16:51
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

spJ006()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin;
	struct hv_poinfo_c hv_poinfo;
	char optype[2];/***�������� 1-�޸� 0����ѯ****/
	char cDeal_mode [2];
	HV_FD123_AREA fd123;

	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));
	memset(optype   , 0x00 ,  sizeof(optype));
	memset(&fd123    , 0x00 ,  sizeof(fd123));

	/**************************
         *PART 1 ��ȡǰ̨�������� *
         **************************/

	/***�õ����֧��ҵ��8583����***/
	get_zd_data("0710",optype);/****��������****/
	get_zd_data("0720",cDeal_mode);/***ת������***/

	vtcp_log("optype===[%c]\n",optype[0]);
	vtcp_log("optype===[%c]\n",optype[0]);
	
	/****����ǲ�ѯ�Ļ�,ֱ�ӳɹ���������***/
	if(optype[0]=='0')
	{
		vtcp_log("[%s][%d]��ѡ���˲�ѯ����!������������!\n",__FILE__,__LINE__);
		goto OkExit;
	}
	/***��ȡǰ̨���ݵ�payin�ṹ����*******/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{	
		sprintf(acErrMsg,"��ǰ̨��ȡ����ʧ��!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}

	/***po_no,po_type�ֶζ��Ƿ���fd123�н��д��ݵ�**/	
	get_fd_data("1230",(char *)&fd123);

	/*******************
         *PART 2 �����ж�  *
         *******************/
	 MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));
	 MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	 ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s' ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"���ݿ��������!ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"���ݿ��������!ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==100)
	{
		sprintf(acErrMsg,"���ҵļ�¼������!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P001");
		goto ErrExit;
	}
	if(hv_poinfo.bank_type[0]=='1')
	{
		sprintf(acErrMsg,"�Ǳ��еǼǵĻ�Ʊ,��Ȩ�޸�!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	if(hv_poinfo.po_sts[0]!='0')
	{
		sprintf(acErrMsg,"��Ʊ���ǵǼ�״̬,�޷������޸�!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P002");
		goto ErrExit;
	}

	/****��ʼ���� ��¼*****/	
	/****��¼������,��ʼд��Ʊ��Ϣ��****/
	dot_str2dbl(hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt),&hv_poinfo.po_bal);     /***��Ʊ���**/
	if(hv_poinfo.l_tx_date==apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date)))
	{
		hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;/**�ǵ����+1****/	
	}
	else
	{
		hv_poinfo.td_cnt = 1;
	}
        hv_poinfo.l_tx_date = apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date));
        /***�ϴν�������,�ŵ�ǰ����***/
	 MEMCPY_DEBUG(hv_poinfo.pay_ac_no,hv_payin.F_pay_ac_no,sizeof(hv_poinfo.pay_ac_no)-1); /***�������ʺ�**/
        MEMCPY_DEBUG(hv_poinfo.pay_name,hv_payin.F_pay_name,sizeof(hv_poinfo.pay_name)-1);    /***����������**/
        MEMCPY_DEBUG(hv_poinfo.pay_addr,hv_payin.F_pay_addr,sizeof(hv_poinfo.pay_addr)-1);   /***�����˵�ַ***/
        MEMCPY_DEBUG(hv_poinfo.cash_ac_no,hv_payin.F_cash_ac_no,sizeof(hv_poinfo.cash_ac_no)-1);/***�տ����ʺ�**/
        MEMCPY_DEBUG(hv_poinfo.cash_name,hv_payin.F_cash_name,sizeof(hv_poinfo.cash_name)-1);/***�տ�������**/
        MEMCPY_DEBUG(hv_poinfo.hold_ac_no,hv_payin.F_cash_ac_no,sizeof(hv_poinfo.hold_ac_no)-1);/**��ǰ��Ʊ���ʺ�**/
        MEMCPY_DEBUG(hv_poinfo.hold_name,hv_payin.F_cash_name,sizeof(hv_poinfo.hold_name)-1);  /**��ǰ��Ʊ������**/
        hv_poinfo.l_po_sts[0] = HV_HP_REG;
        /***�ϴ��ʻ�״̬***/
        hv_poinfo.po_sts[0] =HV_HP_REG;
        /***��ǰ��Ʊ��״̬***/
        /***�Ǽ�����***/
        hv_poinfo.po_ind[0]       = fd123.po_type[0];
        /***��Ʊ����***/
	 hv_poinfo.deal_mode[0]    =cDeal_mode[0];
        /***ת���ֽ���ʽ***/
        MEMCPY_DEBUG(hv_poinfo.cash_br_no,hv_payin.F_cash_opn_br_no,sizeof(hv_poinfo.cash_br_no)-1);/***�Ҹ����к�**/
        MEMCPY_DEBUG(hv_poinfo.brf,   hv_payin.F_hv_brf,sizeof(hv_poinfo.brf)-1);
        /***����***/
	
	/****��ʼ���¼�¼*****/
	vtcp_log("��ʼ��������\n");
	vtcp_log("��ʼ��������\n");
	vtcp_log("��ʼ��������\n");
	ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
	if(ret)
	{	
		sprintf(acErrMsg,"���»�Ʊ������Ϣ�����!ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("�������ݳɹ�!\n");
	vtcp_log("�������ݳɹ�!\n");
	vtcp_log("�������ݳɹ�!\n");
	vtcp_log("�������ݳɹ�!\n");

	
	/*********************
	 *PART 4  ��β����   *
     *********************/	 
	
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
