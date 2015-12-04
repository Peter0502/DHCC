/*****************************************************
* �� �� ��:  sp9565.c
* ���������� ͨ����֧�����ײ�ѯ��Ӧ��֧������ С��ר��
* ��    ��:   dadary 
* ������ڣ� 2006��9��9�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_pkgreg_c.h"
#include "hv_fzf_c.h"
int sp9565()
{
	int ret=0,i;
	char cDate[9];
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;
	struct lv_pkgreg_c lv_pkgreg;
	HV_FD123_AREA fd123;

	int iCmtno = 0;

	memset(cDate, 0 , sizeof(cDate));
	memset(&fd123, 0,sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(&hv_fzf,0x00,sizeof(hv_fzf));
	memset(&lv_pkgreg,0x00,sizeof(lv_pkgreg));


	vtcp_log("sp9565 ��ʼ��[%s][%d]",__FILE__,__LINE__);

	/**********************
	 * 1.��ʼ��ȫ�ֱ���   *
	 **********************/
 	
	pub_base_sysinit();


	/*********************
	 * 2.��ȡ8583������  *
	 *********************/

	get_zd_long("0460",&hv_fzf.wt_date);		/***ԭ��������***/
	get_fd_data("1230",(char*)&fd123);
	memcpy(hv_fzf.cmtno,fd123.cmtno,sizeof(hv_fzf.cmtno)-1);	/**���׵�cmt���**/
	memcpy(hv_fzf.orderno,fd123.qr_tx_num,8);      	/**ԭ��֧�����**/

	memcpy(hv_fzf.or_br_no,	fd123.ac_br_no,	sizeof(hv_fzf.or_br_no)-1);
									/**Ӧ����Ľ�����,�����о��Ǹý��׵Ľ����к�********/
	iCmtno = atoi(hv_fzf.cmtno);
	sprintf(hv_fzf.cmtno,"%3d",iCmtno-1);
	vtcp_log("[%s][%d]hv_fzf.cmtno=[%s]\n",__FILE__,__LINE__,hv_fzf.cmtno);

	/**********************
	 * 3.���ԭ��֧������ *
	 **********************/
	ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, " cmtno='%.3s' and  orderno='%s' and or_br_no='%s' and wt_date=%ld  and otype='2' ", hv_fzf.cmtno, hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
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
	vtcp_log("fzf......o_cmtno=[%3s],o_ordenro=[%8s],o_or_br_no=[%12s],wt_date=[%ld]",hv_fzf.o_cmtno,hv_fzf.o_orderno,hv_fzf.o_or_br_no,hv_fzf.o_tx_date);	
    vtcp_log("��ѯ��֧����¼�ɹ�! [%s][%d]",__FILE__,__LINE__);

	/*****�����������������޷�������ϸ��Ϣ��****/
	if(hv_fzf.reqtype[0]=='1')/***����������Ļ�**/
	{
		vtcp_log("[%s][%d]��ѡ��Ľ�������������!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"ZBAO");
		goto ErrExit1; 
	}


	/*************************
	 * 4. ����Ӧ��֧������ *
	 *************************/

	memcpy(lv_pkgreg.orderno,hv_fzf.o_orderno,sizeof(lv_pkgreg.orderno)-1);		/*֧���������*/
	memcpy(lv_pkgreg.or_br_no,hv_fzf.o_or_br_no,sizeof(lv_pkgreg.or_br_no)-1);	/*֧�������к�*/
	lv_pkgreg.wt_date = hv_fzf.o_wt_date;						/*֧������ί������*/

	vtcp_log("��ʼ��ѯ֧�����ױ�![%s][%d]",__FILE__,__LINE__);

	
	ret = Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, " orderno='%s' and or_br_no='%s' and wt_date=%d", lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�Ҳ�����Ӧ��֧������");
		WRITEMSG
		strcpy(g_pub_tx.reply,"NROW");
		goto	ErrExit1;
	}
	else if ( ret&&ret!=100 )
	{
		sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
  	  vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
   

	/******************
	 * 5.׼������ֵ   *
	 ******************/ 
	memcpy(fd123.or_br_no        ,lv_pkgreg.ac_br_no        ,sizeof(fd123.or_br_no        ));/**�����к� ԭ�����к� */
	memcpy(fd123.pay_ac_no       ,lv_pkgreg.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));/**�������ʺ�(ԭ�տ����ʺ�)**/
	memcpy(fd123.pay_name        ,lv_pkgreg.pay_name        ,sizeof(fd123.pay_name        ));/**����������(ԭ�տ�������)**/
	memcpy(fd123.ac_br_no        ,lv_pkgreg.or_br_no        ,sizeof(fd123.ac_br_no        ));/**�����к�(ԭ�����к�)**/
	memcpy(fd123.cash_ac_no      ,lv_pkgreg.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));/**�տ����ʺ�,ԭ�������ʺ�**/
	memcpy(fd123.cash_name       ,lv_pkgreg.cash_name       ,sizeof(fd123.cash_name       ));/**�տ�������,ԭ����������**/
	memcpy(fd123.o_orderno       ,lv_pkgreg.orderno       ,sizeof(fd123.o_orderno       ));/**ԭ֧���������**/
	memcpy(fd123.notpay_content  ,hv_fzf.req_content      ,sizeof(fd123.notpay_content       ));/**���븽��**/
	vtcp_log("����=[%s]\n",fd123.notpay_content);
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	/***********��ԭί������***********************/
	set_zd_long("0450",lv_pkgreg.wt_date);
	/*****�ͽ��׽��***********/
        set_zd_double("0390",lv_pkgreg.tx_amt);   /* ���׽�� */
 
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
ErrExit1:
	memcpy(fd123.notpay_content  ,hv_fzf.req_content      ,sizeof(fd123.notpay_content       ));/**���븽��**/
	vtcp_log("����=[%s]\n",fd123.notpay_content);
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	Hv_fzf_Clo_Sel();
	Hv_zf_Clo_Sel();
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


