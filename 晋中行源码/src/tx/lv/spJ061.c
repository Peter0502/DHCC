/***************************************************************
* �� �� ��:   spJ061.c
* ����������  С��ҵ��������뱨��¼��
*
* ��    ��:   dongxy
* ������ڣ�  2006-8-29 
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
#include "lv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#define loc_t sys_loc_t
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"

spJ061()
{
	int  			iOrderno=0;
	int  			iRecordNum = 0;
	int 			ret;
	char 			cBrno[6];		/** ���׻��� **/
	HV_NOTPAY_AREA 		hv_notpay;
	struct lv_pkgreg_c 	lv_pkgreg;
	LV_NOTPAY_ADD  		lv_addinfo;
	HV_FD123_AREA  		fd123;
	struct	 hv_fzf_c 	hv_fzf;
	struct 	 hv_fzfhtr_c 	hv_fzfhtr;


	memset(cBrno,		0x00,	sizeof(cBrno));
	memset(&hv_notpay,	0x00,	sizeof(hv_notpay));
	memset(&lv_pkgreg,	0x00,	sizeof(lv_pkgreg));
	memset(&lv_addinfo,	0x00,	sizeof(lv_addinfo));
	memset(&fd123,		0x00,	sizeof(fd123));
	memset(&hv_fzf,		0x00,	sizeof(hv_fzf));
	memset(&hv_fzfhtr,	0x00,	sizeof(hv_fzfhtr));



	vtcp_log("[%s][%d]spJ061С��ҵ��������뱨��¼��BEGIN",__FILE__,__LINE__);



	/************************
	* 1. ��ʼ��ȫ�ֱ���    *
	************************/

	pub_base_sysinit(); 

	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}




	/************************
	* 2.��ȡ8583������     *
	************************/
	ret=iLvTistoNotpay(&hv_notpay);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}



	/*************************
	* 3. ִ�н��׻������   *
	*************************/
	ret = iLvFzFCommonCheck(&hv_notpay);
	if(ret)
	{
		vtcp_log("[%s][%d]ִ�н��׻���������!\n",__FILE__,__LINE__);
		goto ErrExit;
	} 

	/****���ԭ֧������*****/

	ret = lv_check_payin_record(&hv_notpay,&lv_addinfo,"323",'1');
	if(ret)
	{
		vtcp_log("[%s][%d]ԭ֧�����״�������!\n",__FILE__,__LINE__);
		goto ErrExit;
	}


	/*****************
	* 4.���ݿ����  *
	*****************/
	ret = lv_Ins_Notpay_record(&hv_notpay,&hv_fzf,&lv_addinfo);
	if(ret)
	{
		 vtcp_log("[%s][%d]����hv_fzf/hv_fzfhtr����!\n",__FILE__,__LINE__);
		 goto ErrExit;
	}



	/*****************
	* 5.׼����ˮ    *
	*****************/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}



	/****************
	 * 6.ҵ������� *
	 ****************/

	 ret = lv_get_cmt323_tag(&hv_fzf);
	 if(ret)
	 {
		vtcp_log("[%s][%d]����ҵ�������!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	 }


	/********************
	* 7. ��ȡ����ֵ    *
	********************/
	memcpy(fd123.orderno,hv_fzf.req_orderno,sizeof(fd123.orderno));
	memcpy(fd123.or_br_no,hv_notpay.F_or_br_no,sizeof(fd123.or_br_no));
	pub_base_full_space((char *)&fd123,sizeof(fd123));
	set_zd_data("1230",(char *)&fd123);
	vtcp_log("[%s]\n",(char *)&fd123);


OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;

ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
