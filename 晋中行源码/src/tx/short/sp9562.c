/*****************************************************
* �� �� ��:  sp9562.c
* ���������� ͨ����֧�����ײ�ѯ��Ӧ��֧������ 
* ��    ��:   dadary 
* ������ڣ� 2006��8��20�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
int sp9562()
{
	int ret=0,i;
	int iCmtno = 0;
	char cOrderno[9];
	char cDate[9];
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;
	HV_FD123_AREA fd123;

	memset(cDate, 0 , sizeof(cDate));
	memset(&fd123, 0,sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(&hv_fzf,0x00,sizeof(hv_fzf));
	memset(cOrderno,0x00,sizeof(cOrderno));

	vtcp_log("sp9562 ��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();

	get_zd_long("0460",&hv_fzf.wt_date);/***ԭ��������***/
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("[%s][%d]1230==[%s]",__FILE__,__LINE__,(char*)&fd123);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	vtcp_log("fd123.o_notpay_orderno==[%3s]\n",fd123.o_notpay_orderno);
	memcpy(hv_fzf.cmtno,	fd123.o_cmtno,		sizeof(hv_fzf.cmtno)-1);
	memcpy(cOrderno,"00000",5);
	memcpy(hv_fzf.orderno,"00000",5);
	memcpy(hv_fzf.orderno+5,fd123.o_notpay_orderno,3);
	vtcp_log("[%s][%d]orderno==[%s]\n",__FILE__,__LINE__,hv_fzf.orderno);
	memcpy(hv_fzf.or_br_no,	fd123.ac_br_no,		sizeof(hv_fzf.or_br_no)-1);/**Ӧ����Ľ�����,�����о��Ǹý��׵Ľ����к�********/


	iCmtno	= apatoi(hv_fzf.cmtno,sizeof(hv_fzf.cmtno)-1);

	vtcp_log("sp9562 ��ʼ��[%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]cmtno=[%s]orderno=[%s],or_br_no=[%s]wt_date=[%ld]",__FILE__,__LINE__,hv_fzf.cmtno,hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
	vtcp_log("sp9562 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	
	if(iCmtno!=0)
	{
	ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='1' and cmtno='%s' and orderno='%s' and or_br_no='%s' and wt_date=%ld ", hv_fzf.cmtno,hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
	}
	else if(iCmtno==0)
	{
	ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='1' and  orderno='%s' and or_br_no='%s' and wt_date=%ld ", hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
	}
	
	if(ret)
	{
		sprintf(acErrMsg,"�������ݿ����!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	ret = Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴˷�֧������");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P020" ); 
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ����֧�����׻�����Ϣ����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P020");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_fzf.o_tx_date==[%ld]\n",__FILE__,__LINE__,hv_fzf.o_tx_date);
	vtcp_log("[%s][%d]hv_fzf.o_tx_date==[%ld]\n",__FILE__,__LINE__,hv_fzf.o_tx_date);
	vtcp_log("fzf......o_cmtno=[%3s],o_ordenro=[%8s],o_or_br_no=[%12s],wt_date=[%ld]",hv_fzf.o_cmtno,hv_fzf.o_orderno,hv_fzf.o_or_br_no,hv_fzf.o_tx_date);	
    vtcp_log("��ѯ��֧����¼�ɹ�! [%s][%d]",__FILE__,__LINE__);




 	   /***�ӷ�֧�����л�ȡ��֧����¼����Ϣ******/
   	memcpy(hv_zf.cmtno,hv_fzf.o_cmtno,sizeof(hv_zf.cmtno)-1);
	memcpy(hv_zf.orderno,hv_fzf.o_orderno,sizeof(hv_zf.orderno)-1);
	memcpy(hv_zf.or_br_no,hv_fzf.o_or_br_no,sizeof(hv_zf.or_br_no)-1);
	hv_zf.wt_date = hv_fzf.o_wt_date;

	vtcp_log("��ʼ��ѯ֧�����ױ�![%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]====cmtno=[%s],orderno=[%s],or_br_no=[%s],wt_date=[%ld]\n",__FILE__,__LINE__,hv_zf.cmtno,hv_zf.orderno,hv_zf.or_br_no,hv_zf.wt_date);

	/****��cmt����ֶβ���Ϊwhere����,�������������Ѿ��㹻����һ������**/
	
	ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, " orderno='%s' and or_br_no='%s' and wt_date=%d", hv_zf.orderno,hv_zf.or_br_no,hv_zf.wt_date);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�Ҳ�����Ӧ��֧������");
		WRITEMSG
		strcpy(g_pub_tx.reply,"9999");
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
    vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
    
	 memcpy(fd123.cmtno,hv_zf.cmtno,sizeof(fd123.cmtno));
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	memcpy(fd123.o_cmtno,hv_zf.o_cmtno,sizeof(fd123.cmtno));
	/*******cmt���*********/
	memcpy(fd123.or_br_no        ,hv_zf.ac_br_no        ,sizeof(fd123.or_br_no        ));/**�����к� ԭ�����к� */
	memcpy(fd123.pay_ac_no       ,hv_zf.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));/**�������ʺ�(ԭ�տ����ʺ�)**/
	memcpy(fd123.pay_name        ,hv_zf.pay_name        ,sizeof(fd123.pay_name        ));/**����������(ԭ�տ�������)**/
	memcpy(fd123.ac_br_no        ,hv_zf.or_br_no        ,sizeof(fd123.ac_br_no        ));/**�����к�(ԭ�����к�)**/
	vtcp_log("ԭ�������к�=[%s]\n",fd123.ac_br_no);
	vtcp_log("ԭ�������к�=[%s]\n",fd123.ac_br_no);
	vtcp_log("ԭ�������к�=[%s]\n",fd123.ac_br_no);
	vtcp_log("ԭ�������к�=[%s]\n",fd123.ac_br_no);
	vtcp_log("ԭ�������к�=[%s]\n",fd123.ac_br_no);
	memcpy(fd123.cash_ac_no      ,hv_zf.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));/**�տ����ʺ�,ԭ�������ʺ�**/
	memcpy(fd123.cash_name       ,hv_zf.cash_name       ,sizeof(fd123.cash_name       ));/**�տ�������,ԭ����������**/
	memcpy(fd123.o_orderno       ,hv_zf.orderno       ,sizeof(fd123.o_orderno       ));/**ԭ֧���������**/
	memcpy(fd123.notpay_content       ,hv_fzf.req_content      ,sizeof(fd123.notpay_content       ));/**���븽��**/
	vtcp_log("����=[%s]\n",fd123.notpay_content);
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	/***********��ԭί������***********************/
	set_zd_long("0450",hv_zf.wt_date);
	/*****�ͽ��׽��***********/
        set_zd_double("0390",hv_zf.tx_amt);   /* ���׽�� */
 
OkExit:
	Hv_fzf_Clo_Sel();
	Hv_zf_Clo_Sel();
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	Hv_fzf_Clo_Sel();
	Hv_zf_Clo_Sel();
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


