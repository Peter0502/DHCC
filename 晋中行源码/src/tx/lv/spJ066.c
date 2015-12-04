/***************************************************************
* �� �� ��:   spJ066.c
* ����������  С��ҵ�����ɸ�ʽ����¼��
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
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#define loc_t sys_loc_t

spJ066()
{
	int  	iOrderno = 0;
	int  	ret = 0;
	char 	cSendco[5];	
   	char cBrno[6];		/** ���׻��� **/
	char cAc_br_no[13];	/**�������к�**/
	HV_NOTPAY_AREA  	hv_notpay;
	struct hv_fzf_c 	hv_fzf;
	struct hv_fzfhtr_c 	hv_fzfhtr;
	HV_FD123_AREA		fd123;
	LV_NOTPAY_ADD		lv_addinfo;

	memset(cBrno,		0x00,	sizeof(cBrno));
	memset(cSendco,		0x00,	sizeof(cSendco));
	memset(cAc_br_no,	0x00, 	sizeof(cAc_br_no));
	memset(&hv_notpay,	0x00,	sizeof(hv_notpay));
	memset(&hv_fzf,		0x00,	sizeof(hv_fzf));
	memset(&hv_fzfhtr,	0x00,	sizeof(hv_fzfhtr));
	memset(&fd123,		0x00,	sizeof(fd123));
	memset(&lv_addinfo,	0x00,	sizeof(lv_addinfo));

	vtcp_log("[%s][%d]spJ066С��ҵ�����ɸ�ʽ����¼��BEGIN",__FILE__,__LINE__);



        /*************************
	 * 1.  ��ʼ��ȫ�ֱ��� 	 *
       	 *************************/  
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

       /*************************
	* 2. ��ȡ8583�������   *
        *************************/	
	ret = iLvTistoNotpay(&hv_notpay);
	if( ret )
	{
		sprintf( acErrMsg,"����front_to_notpayin����![%s][%d]",__FILE__,__LINE__);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P053");
		goto ErrExit;
	}

	/*************************
  	 * 3.���������ݼ��      *
 	 *************************/
	ret = iLvFzFCommonCheck(&hv_notpay);
	if(ret)
	{
		sprintf(acErrMsg,"ִ�к�������������!\n");
		WRITEMSG
		goto ErrExit;
	}

	/**************************
	 * 4.���ݿ����           *
	 **************************/
	
	ret = lv_Ins_Notpay_record(&hv_notpay,&hv_fzf,&lv_addinfo);

	if(ret)
	{
		vtcp_log("[%s][%d]!\n",__FILE__,__LINE__);
		goto ErrExit;
	}
	vtcp_log("[%s][%d]�ñ����ɸ�ʽ�Ƿ��͸������е�[%s]\n",__FILE__,__LINE__,cAc_br_no);

	/**************************
	 * 5. ��ˮ����            *
	 **************************/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	/************************
	 * 6. ����ҵ���        *
	 ************************/


	vtcp_log("[%s][%d]��ʼ����ҵ���!\n",__FILE__,__LINE__);
	ret = lv_get_cmt303_tag(hv_notpay.F_wt_date,hv_fzf.orderno);
	if(ret)
	{
		vtcp_log("[%s][%d]����ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d]���ͳɹ�!\n",__FILE__,__LINE__);
	/**************************
	 * 7.  ׼�����           *
	 **************************/
        memcpy(fd123.orderno,hv_notpay.F_orderno,sizeof(fd123.orderno));
	memcpy(fd123.ac_br_no,hv_fzf.ac_br_no,sizeof(fd123.ac_br_no));
	memcpy(fd123.notpay_content,hv_fzf.req_content,sizeof(fd123.notpay_content));
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
	
