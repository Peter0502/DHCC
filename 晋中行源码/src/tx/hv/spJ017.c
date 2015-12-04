/***************************************************************
* �� �� ��:     spJ017.c
* ����������    �����˻����л�Ʊ�⸶  
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-27 21:08
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


spJ017()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin;
	struct hv_poinfo_c hv_poinfo;
	char cPwd[11];/**��ȡ��Ѻ***/
	HV_FD123_AREA  fd123;	

	memset(&hv_payin,0x00,sizeof(hv_payin));
	memset(&hv_poinfo,0x00,sizeof(hv_poinfo));
	memset(cPwd,0x00,sizeof(cPwd));
	memset(&fd123,0x00,sizeof(fd123));




	/******************
	 * ��ʼ��ϵͳ���� *
	 ******************/

	  pub_base_sysinit();
	
     /************************ 
     *PART 1 ��ȡ��ǰ̨���� *
     ************************/

	/***�õ����֧��ҵ��8583����***/
	get_zd_data("0880",cPwd);   /****��ȡ����Ѻ******/
	get_fd_data("1230",(char *)&fd123);
	vtcp_log("fd123=[%s]\n",(char *)&fd123);
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		WRITEMSG("��ǰ̨��ȡ���ݴ���!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	/****���Ǳ���ǩ���Ļ�Ʊ�����԰����ҵ��****/
	if(memcmp(fd123.pay_opn_br_no,hv_payin.F_pay_opn_br_no,sizeof(fd123.pay_opn_br_no)))
	{
		sprintf(acErrMsg,"�Ǳ���ǩ����Ʊ,�����ԶҸ��ñ������˻��ʽ�\n");		WRITEMSG
		vtcp_log("fd123=[%s] hv_payin=[%s]\n",fd123.pay_opn_br_no,hv_payin.F_pay_opn_br_no);
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	/*vtcp_log("+++++++++cPwd=[%s]=[%d]++++++[%s]\n",__FILE__,__LINE__,cPwd);*/
	vtcp_log("%s,%d fd123.po_no==[%s]\n",__FILE__,__LINE__,fd123.po_no);
	vtcp_log("%s,%d fd123.po_no==[%s]\n",__FILE__,__LINE__,fd123.po_no);
	vtcp_log("%s,%d fd123.po_no==[%s]\n",__FILE__,__LINE__,fd123.po_no);
	vtcp_log("%s,%d fd123.po_no==[%s]\n",__FILE__,__LINE__,fd123.po_no);
  /***����ǰ̨��ȡ��ֵ���뵽hv_poinfo�ṹ����**/
	MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,HV_POLEN);
	MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
		
	vtcp_log("po_no==[%s]\n",hv_poinfo.po_no);
	vtcp_log("pay_br_no=[%s]\n",hv_poinfo.pay_br_no);
	/*vtcp_log("pwd===[%s]\n",cPwd);*/		


     /***********************
     *PART 3   �������ݱ�  *
     ***********************/	
	/******��ȡ����********************/
	ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s' ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к���hv_poinfo_dec_upd����!ret==[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P001");
		goto ErrExit;
	}
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);

	ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к���Hv_poinfo_Fet_Upd����ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}
	/*vtcp_log("+[%d]+pwd==[%s]++++++\n",__LINE__,cPwd);*/
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
	if(hv_poinfo.po_sts[0]!=HV_HP_OVERDATE_UNTREAD)
	{
		sprintf(acErrMsg,"��Ʊ��������δ���˻�״̬!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P004");
		goto ErrExit;
	}
	/*vtcp_log("+[%d]+pwd==[%s]++++++\n",__LINE__,cPwd);*/
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
	if(hv_poinfo.bank_type[0]!='0')
	{
		sprintf(acErrMsg,"�Ǳ��еĻ�Ʊ,�޷��⸶!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P004");
		goto ErrExit;
	}
	if(memcmp(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1))
	{
		sprintf(acErrMsg,"�������Ѻ����!\n");
		WRITEMSG
		/*vtcp_log("��=[%s],����=[%s]\n",hv_poinfo.pwd,cPwd);*/
		strcpy(g_pub_tx.reply,"P005");
		goto ErrExit;
	}
	/****************************************************
	 ******************������**************************
		261--cust
		24403 -- 24401
	****************************************************/	
	/** ׼���������¼,�Ӳ������ñ��еõ���Ӧ���ʺ� */
	set_zd_data("0990","0");
	set_zd_double("0400",hv_poinfo.po_bal);
	/***���������!��ʼ���±�**************************/
	hv_poinfo.po_sts[0]  = HV_HP_UNUSED_TOCUST;/**���»�Ʊ״̬**/
	if(hv_poinfo.l_tx_date==apatoi(hv_payin.F_tx_date,sizeof(hv_payin.F_tx_date)))
		hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
	else
		hv_poinfo.td_cnt = 1;
	hv_poinfo.l_tx_date  = apatoi(hv_payin.F_tx_date,sizeof(hv_payin.F_tx_date));
	/******���ڿ�ʼ������Щ�ֶΰ�*************************/		
	ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к���hv_poinfo_upd_upd����!ret==[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	/*******************
     * PART 4 ��β���� *
     ******************/

	/* д��ҵ����ˮ */
	MEMCPY_DEBUG(g_pub_tx.beg_note_no,hv_poinfo.po_no,sizeof(g_pub_tx.beg_note_no));
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

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
